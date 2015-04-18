#include "vm.hpp"
#include "console.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "on_stack.hpp"
#include "object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/channel.hpp"
#include "builtin/class.hpp"
#include "builtin/fsevent.hpp"
#include "builtin/io.hpp"
#include "builtin/string.hpp"
#include "builtin/thread.hpp"

#include "dtrace/dtrace.h"

#include "util/atomic.hpp"
#include "util/file.hpp"
#include "util/logger.hpp"

#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdio.h>

// read
#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>

namespace rubinius {
  using namespace utilities;

  namespace console {
    static int open_file(STATE, std::string path) {
      int perms = state->shared().config.system_console_access;
      int fd = IO::open_with_cloexec(state, path.c_str(),
          O_CREAT | O_TRUNC | O_RDWR | O_SYNC, perms);

      if(fd < 0) {
        logger::error("%s: console: unable to open: %s", strerror(errno), path.c_str());
      }

      // The umask setting will override our permissions for open().
      if(chmod(path.c_str(), perms) < 0) {
        logger::error("%s: console: unable to set mode: %s", strerror(errno), path.c_str());
      }

      return fd;
    }

    Request::Request(STATE, Console* console)
      : InternalThread(state, "rbx.console.request")
      , console_(console)
      , response_(console->response())
      , enabled_(false)
      , fd_(-1)
      , fsevent_(state)
    {
    }

    void Request::initialize(STATE) {
      path_ = state->shared().fsapi_path + "/console-request";
      console_->server_class(state)->set_const(state, state->symbol("RequestPath"),
          String::create(state, path_.c_str()));
    }

    void Request::setup_request(STATE) {
      if((fd_ = open_file(state, path_)) < 0) {
        logger::error("%s: console request: unable to open file", strerror(errno));
        return;
      }

      FSEvent* fsevent = FSEvent::create(state);
      fsevent->watch_file(state, fd_, path_.c_str());
      fsevent_.set(fsevent);

      enabled_ = true;
    }

    void Request::start_thread(STATE) {
      setup_request(state);

      InternalThread::start_thread(state);
    }

    void Request::wakeup(STATE) {
      InternalThread::wakeup(state);

      if(write(fd_, "\0", 1) < 0) {
        logger::error("%s: console: unable to wake request thread", strerror(errno));
      }
    }

    void Request::close_request(STATE) {
      if(fd_ > 0) {
        close(fd_);
        fd_ = -1;
      }
    }

    void Request::stop_thread(STATE) {
      InternalThread::stop_thread(state);

      close_request(state);
      unlink(path_.c_str());
    }

    void Request::before_exec(STATE) {
      stop_thread(state);
    }

    void Request::after_exec(STATE) {
      start(state);
    }

    void Request::after_fork_child(STATE) {
      close_request(state);

      InternalThread::after_fork_child(state);
    }

    char* Request::read_request(STATE) {
      file::LockGuard guard(fd_, LOCK_EX);

      if(guard.status() != file::eLockSucceeded) {
        logger::error("%s: console: unable to lock request file", strerror(errno));
        return NULL;
      }

      char* buf[1024];

      lseek(fd_, 0, SEEK_SET);
      ssize_t bytes = ::read(fd_, buf, 1024);

      char* req = NULL;
      if(bytes > 0) {
        req = new char[bytes+1];
        memcpy(req, buf, bytes);
        req[bytes] = 0;
        metrics().m.console_metrics.requests_received++;
      } else if(bytes < 0) {
        logger::error("%s: console: unable to read request", strerror(errno));
      }

      if(lseek(fd_, 0, SEEK_SET) < 0) {
        logger::error("%s: console: unable to rewind request file", strerror(errno));
      }
      if(ftruncate(fd_, 0) < 0) {
        logger::error("%s: console: unable to truncate request file", strerror(errno));
      }

      return req;
    }

    void Request::run(STATE) {
      if(!enabled_) return;

      while(!thread_exit_) {
        Object* status = fsevent_.get()->wait_for_event(state);

        if(thread_exit_) break;

        if(status->nil_p()) {
          utilities::logger::error("%s: console: request: wait for event failed",
              strerror(errno));
          continue;
        }

        char* request = read_request(state);

        if(request) {
          response_->send_request(state, request);
        }
      }
    }

    Response::Response(STATE, Console* console)
      : InternalThread(state, "rbx.console.response")
      , console_(console)
      , fd_(-1)
      , request_list_(NULL)
    {
    }

    Response::~Response() {
      delete request_list_;
      request_list_ = NULL;
    }

    void Response::initialize(STATE) {
      path_ = state->shared().fsapi_path + "/console-response";
      console_->server_class(state)->set_const(state, state->symbol("ResponsePath"),
          String::create(state, path_.c_str()));

      Thread::create(state, vm());
    }

    void Response::start_thread(STATE) {
      if((fd_ = open_file(state, path_)) < 0) {
        logger::error("%s: console response: unable to open file", strerror(errno));
        return;
      }

      list_lock_.init();
      response_lock_.init();
      response_cond_.init();

      request_list_ = new RequestList;

      InternalThread::start_thread(state);
    }

    void Response::wakeup(STATE) {
      InternalThread::wakeup(state);

      response_cond_.signal();
    }

    void Response::close_response(STATE) {
      if(fd_ > 0) {
        close(fd_);
        fd_ = -1;
      }
    }

    void Response::clear_requests(STATE) {
      if(request_list_) {
        for(RequestList::const_iterator i = request_list_->begin();
            i != request_list_->end();
            ++i)
        {
          delete[] *i;
        }

        request_list_ = NULL;
      }
    }

    void Response::stop_thread(STATE) {
      InternalThread::stop_thread(state);

      clear_requests(state);

      close_response(state);
      unlink(path_.c_str());
    }

    void Response::before_exec(STATE) {
      stop_thread(state);
    }

    void Response::after_exec(STATE) {
      start(state);
    }

    void Response::after_fork_child(STATE) {
      close_response(state);
      clear_requests(state);

      InternalThread::after_fork_child(state);
    }

    void Response::send_request(STATE, const char* request) {
      utilities::thread::SpinLock::LockGuard guard(list_lock_);

      request_list_->push_back(const_cast<char*>(request));
      response_cond_.signal();
    }

    void Response::write_response(STATE, const char* response, native_int size) {
      file::LockGuard guard(fd_, LOCK_EX);

      if(guard.status() != file::eLockSucceeded) {
        logger::error("%s: unable to lock response file", strerror(errno));
        return;
      }

      if(lseek(fd_, 0, SEEK_SET) < 0) {
        logger::error("%s: console: unable to rewind response file", strerror(errno));
        return;
      }
      if(ftruncate(fd_, 0) < 0) {
        logger::error("%s: console: unable to truncate response file", strerror(errno));
        return;
      }

      if(::write(fd_, response, size) < 0) {
        logger::error("%s: console: unable to write response", strerror(errno));
      }

      metrics().m.console_metrics.responses_sent++;
    }

    void Response::run(STATE) {
      GCTokenImpl gct;
      size_t pending_requests = 0;
      char* request = NULL;

      Channel* inbox = as<Channel>(
          console_->ruby_console()->get_ivar(state, state->symbol("@inbox")));
      Channel* outbox = as<Channel>(
          console_->ruby_console()->get_ivar(state, state->symbol("@outbox")));

      String* response = 0;
      OnStack<3> os(state, inbox, outbox, response);

      while(!thread_exit_) {
        {
          utilities::thread::SpinLock::LockGuard guard(list_lock_);

          if(request_list_->size() > 0) {
            request = request_list_->back();
            request_list_->pop_back();
          }
        }

        if(thread_exit_) break;

        if(request) {
          GCDependent guard(state, 0);

          pending_requests++;

          inbox->send(state, gct, String::create(state, request), 0);

          request = NULL;
        }

        if(pending_requests > 0) {
          if((response = try_as<String>(outbox->try_receive(state, gct, 0)))) {
            write_response(state,
                reinterpret_cast<const char*>(response->byte_address()),
                response->byte_size());
            pending_requests--;
            continue;
          }
        }

        {
          GCIndependent gc_guard(state, 0);
          utilities::thread::Mutex::LockGuard guard(response_lock_);

          if(thread_exit_) break;

          response_cond_.wait(response_lock_);
        }
      }
    }

    Console::Console(STATE)
      : path_(std::string(state->shared().fsapi_path + "/console"))
      , response_(new Response(state, this))
      , request_(new Request(state, this))
      , ruby_console_(state)
    {
      ruby_console_.set(server_class(state)->send(state, 0, state->symbol("new")));
    }

    Console::~Console() {
      delete request_;
      delete response_;
    }

    void Console::start(STATE) {
      request_->start(state);
      response_->start(state);
    }

    Class* Console::server_class(STATE) {
      Module* mod = as<Module>(G(rubinius)->get_const(state, "Console"));
      return as<Class>(mod->get_const(state, "Server"));
    }
  }
}

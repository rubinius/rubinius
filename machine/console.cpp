#include "vm.hpp"
#include "state.hpp"
#include "console.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "on_stack.hpp"
#include "object_utils.hpp"
#include "thread_phase.hpp"

#include "class/array.hpp"
#include "class/channel.hpp"
#include "class/class.hpp"
#include "class/fsevent.hpp"
#include "class/io.hpp"
#include "class/string.hpp"
#include "class/thread.hpp"

#include "dtrace/dtrace.h"

#include "util/atomic.hpp"
#include "util/file.hpp"
#include "logger.hpp"

#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdio.h>
#include <libgen.h>

// read
#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>

#include <ostream>

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

    Request::Request(STATE, Console* console, Response* response)
      : MachineThread(state, "rbx.console.request", MachineThread::eSmall)
      , console_(console)
      , response_(response)
      , enabled_(false)
      , fd_(-1)
      , fsevent_(state)
    {
    }

    void Request::initialize(STATE) {
      if((fd_ = open_file(state, console_->request_path())) < 0) {
        logger::error("%s: console request: unable to open file", strerror(errno));
        return;
      }

      FSEvent* fsevent = FSEvent::create(state);
      fsevent->watch_file(state, fd_, console_->request_path().c_str());
      fsevent_.set(fsevent);

      enabled_ = true;
    }

    void Request::start_thread(STATE) {
      if(!enabled_) return;

      MachineThread::start_thread(state);
    }

    void Request::wakeup(STATE) {
      MachineThread::wakeup(state);

      if(write(fd_, "\0", 1) < 0) {
        logger::error("%s: console: unable to wake request thread", strerror(errno));
      }
    }

    void Request::close_request() {
      if(fd_ > 0) {
        close(fd_);
        fd_ = -1;
      }
    }

    void Request::stop_thread(STATE) {
      MachineThread::stop_thread(state);

      close_request();
    }

    void Request::after_fork_child(STATE) {
      close_request();
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
        vm()->metrics().console.requests_received++;
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
          logger::error("%s: console: request: wait for event failed",
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
      : MachineThread(state, "rbx.console.response", MachineThread::eSmall)
      , console_(console)
      , inbox_(state)
      , outbox_(state)
      , fd_(-1)
      , request_list_(NULL)
    {
      inbox_.set(as<Channel>(
          console_->ruby_console()->get_ivar(state, state->symbol("@inbox"))));
      outbox_.set(as<Channel>(
          console_->ruby_console()->get_ivar(state, state->symbol("@outbox"))));
    }

    Response::~Response() {
      delete request_list_;
      request_list_ = NULL;
    }

    void Response::initialize(STATE) {
      Thread::create(state, vm());
    }

    void Response::start_thread(STATE) {
      if((fd_ = open_file(state, console_->response_path())) < 0) {
        logger::error("%s: console response: unable to open file", strerror(errno));
        return;
      }

      list_lock_.init();
      response_lock_.init();
      response_cond_.init();

      request_list_ = new RequestList;

      MachineThread::start_thread(state);
    }

    void Response::wakeup(STATE) {
      MachineThread::wakeup(state);

      inbox_.get()->send(state, String::create(state, ""));

      response_cond_.signal();
    }

    void Response::close_response() {
      if(fd_ > 0) {
        close(fd_);
        fd_ = -1;
      }
    }

    void Response::clear_requests() {
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
      MachineThread::stop_thread(state);

      close_response();
    }

    void Response::after_fork_child(STATE) {
      close_response();
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

      vm()->metrics().console.responses_sent++;
    }

    void Response::run(STATE) {
      size_t pending_requests = 0;
      char* request = NULL;

      Channel* inbox = inbox_.get();
      Channel* outbox = outbox_.get();

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
          ManagedPhase managed(state);

          pending_requests++;

          inbox->send(state, String::create(state, request));

          request = NULL;
        }

        if(pending_requests > 0) {
          if((response = try_as<String>(outbox->try_receive(state)))) {
            write_response(state,
                reinterpret_cast<const char*>(response->byte_address()),
                response->byte_size());
            pending_requests--;
            continue;
          }
        }

        {
          UnmanagedPhase unmanaged(state);
          utilities::thread::Mutex::LockGuard guard(response_lock_);

          if(thread_exit_) break;

          response_cond_.wait(response_lock_);
        }
      }
    }

    Listener::Listener(STATE, Console* console)
      : MachineThread(state, "rbx.console.listener", MachineThread::eSmall)
      , console_(console)
      , fsevent_(state)
      , fd_(-1)
    {
    }

    Listener::~Listener() {
      close(fd_);
    }

    void Listener::initialize(STATE) {
      fd_ = ::open(console_->console_path().c_str(),
          O_CREAT | O_TRUNC | O_RDWR | O_CLOEXEC,
          state->shared().config.system_console_access.value);

      if(fd_ < 0) {
        logger::error("%s: unable to open Console connection file",
            strerror(errno));
      }

      // The umask setting will override our permissions for open().
      if(chmod(console_->console_path().c_str(),
            state->shared().config.system_console_access.value) < 0) {
        logger::error("%s: unable to set mode for Console connection file",
            strerror(errno));
      }

      FSEvent* fsevent = FSEvent::create(state);
      fsevent->watch_file(state, fd_, console_->console_path().c_str());
      fsevent_.set(fsevent);
    }

    void Listener::start_thread(STATE) {
      MachineThread::start_thread(state);
    }

    void Listener::wakeup(STATE) {
      MachineThread::wakeup(state);

      while(thread_running_p()) {
        if(write(fd_, "\0", 1) < 0) {
          logger::error("%s: console: unable to wake listener thread", strerror(errno));
        }
      }
    }

    bool Listener::connection_initiated() {
      struct stat st;

      bool req = stat(console_->request_path().c_str(), &st) == 0 && S_ISREG(st.st_mode);
      bool res = stat(console_->response_path().c_str(), &st) == 0 && S_ISREG(st.st_mode);

      return req && res;
    }

    void Listener::run(STATE) {
      while(!thread_exit_) {
        Object* status = fsevent_.get()->wait_for_event(state);

        if(thread_exit_) break;

        if(status->nil_p()) {
          logger::error("%s: console: listener: wait for event failed",
              strerror(errno));
          continue;
        }

        if(console_->connected_p()) continue;

        if(connection_initiated()) {
          console_->accept(state);
        }
      }
    }

    Console::Console(STATE)
      : listener_(0)
      , response_(0)
      , request_(0)
      , ruby_console_(state)
    {
      console_path_ = state->shared().config.system_console_path.value;

      std::ostringstream basename;
      basename << state->shared().config.system_console_path.value << "-"
               << state->shared().pid;

      request_path_ = basename.str() + "-request";
      response_path_ = basename.str() + "-response";

      listener_ = new Listener(state, this);
    }

    Console::~Console() {
      if(listener_) delete listener_;
      reset();
    }

    bool Console::connected_p() {
      return request_ && request_->enabled_p();
    }

    void Console::start(STATE) {
      listener_->start(state);
    }

    void Console::accept(STATE) {
      ruby_console_.set(server_class(state)->send(state, 0, state->symbol("new")));

      response_ = new Response(state, this);
      request_ = new Request(state, this, response_);
    }

    void Console::reset() {
      if(request_) {
        delete request_;
        request_ = 0;
      }

      if(response_) {
        delete response_;
        response_ = 0;
      }

      ruby_console_.set(cNil);
    }

    void Console::after_fork_child(STATE) {
      reset();
    }

    Class* Console::server_class(STATE) {
      Module* mod = as<Module>(G(rubinius)->get_const(state, "Console"));
      return as<Class>(mod->get_const(state, "Server"));
    }
  }
}

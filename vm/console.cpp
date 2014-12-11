#include "vm.hpp"
#include "console.hpp"

#include "object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/fsevent.hpp"
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
    Object* console_request_trampoline(STATE) {
      state->shared().console()->process_requests(state);
      GCTokenImpl gct;
      state->gc_dependent(gct, 0);
      return cNil;
    }

    Object* console_response_trampoline(STATE) {
      state->shared().console()->process_responses(state);
      GCTokenImpl gct;
      state->gc_dependent(gct, 0);
      return cNil;
    }

    Console::Console(STATE)
      : AuxiliaryThread()
      , shared_(state->shared())
      , request_vm_(NULL)
      , response_vm_(NULL)
      , request_(state)
      , response_(state)
      , console_(state)
      , fsevent_(state)
      , request_fd_(-1)
      , response_fd_(-1)
      , request_exit_(false)
      , response_exit_(false)
      , request_list_(NULL)
    {
      shared_.auxiliary_threads()->register_thread(this);
    }

    Console::~Console() {
      shared_.auxiliary_threads()->unregister_thread(this);

      delete request_list_;
      request_list_ = NULL;
    }

    void Console::wakeup() {
      request_exit_ = true;
      atomic::memory_barrier();

      if(write(request_fd_, "x", 1) < 0) {
        logger::error("%s: console: unable to wake request thread", strerror(errno));
      }
    }

    void Console::close_files(bool remove_files) {
      if(request_fd_ > 0) {
        close(request_fd_);
        if(remove_files) unlink(request_path_.c_str());
        request_fd_ = -1;
      }

      if(response_fd_ > 0) {
        close(response_fd_);
        if(remove_files) unlink(response_path_.c_str());
        response_fd_ = -1;
      }
    }

    void Console::empty_request_list() {
      if(request_list_) {
        for(RequestList::const_iterator i = request_list_->begin();
            i != request_list_->end();
            ++i)
        {
          delete[] *i;
        }
      }
    }

    void Console::initialize(STATE) {
      request_vm_ = NULL;
      response_vm_ = NULL;

      request_exit_ = false;
      response_exit_ = false;

      list_lock_.init();
      response_lock_.init();
      response_cond_.init();

      std::string path(shared_.fsapi_path + "/console");

      request_path_ = path + "-request";
      response_path_ = path + "-response";

      request_list_ = new RequestList;

      Module* mod = as<Module>(G(rubinius)->get_const(state, "Console"));
      Class* cls = as<Class>(mod->get_const(state, "Server"));
      console_.set(cls->send(state, 0, state->symbol("new")));

      cls->set_const(state, state->symbol("RequestPath"),
          String::create(state, request_path_.c_str()));
      cls->set_const(state, state->symbol("ResponsePath"),
          String::create(state, response_path_.c_str()));
    }

    static int open_file(STATE, std::string path) {
      int perms = state->shared().config.system_console_access;
      int fd = ::open(path.c_str(), O_CREAT | O_TRUNC | O_RDWR | O_SYNC | O_CLOEXEC, perms);

      if(fd < 0) {
        logger::error("%s: console: unable to open: %s", strerror(errno), path.c_str());
      }

      // The umask setting will override our permissions for open().
      if(chmod(path.c_str(), perms) < 0) {
        logger::error("%s: console: unable to set mode: %s", strerror(errno), path.c_str());
      }

      return fd;
    }

    void Console::setup_files(STATE) {
      request_fd_ = open_file(state, request_path_);
      response_fd_ = open_file(state, response_path_);

      FSEvent* fsevent = FSEvent::create(state);
      fsevent->watch_file(state, request_fd_, request_path_.c_str());
      fsevent_.set(fsevent);
    }

    void Console::reset(STATE) {
      close_files(false);
      setup_files(state);
    }

    void Console::start(STATE) {
      initialize(state);
      setup_files(state);
      start_threads(state);
    }

    void Console::run(STATE) {
      if(request_.get()->fork_attached(state)) {
        rubinius::bug("Unable to start console request thread");
      }

      if(response_.get()->fork_attached(state)) {
        rubinius::bug("Unable to start console response thread");
      }
    }

    void Console::start_threads(STATE) {
      SYNC(state);

      // Don't start threads if we couldn't open communication channels.
      if(request_fd_ < 0 || response_fd_ < 0) return;

      if(!request_vm_) {
        request_vm_ = state->shared().new_vm();
        request_vm_->metrics()->init(metrics::eConsoleMetrics);
        request_exit_ = false;
        request_.set(Thread::create(state, request_vm_, G(thread),
                     console_request_trampoline, true));
      }

      if(!response_vm_) {
        response_vm_ = state->shared().new_vm();
        response_vm_->metrics()->init(metrics::eConsoleMetrics);
        response_exit_ = false;
        response_.set(Thread::create(state, response_vm_, G(thread),
                     console_response_trampoline, true));
      }

      if(request_vm_ && response_vm_) {
        run(state);
      }
    }

    void Console::stop_threads(STATE) {
      SYNC(state);

      if(request_vm_) {
        wakeup();

        if(atomic::poll(request_running_, false)) {
          void* return_value;
          pthread_t os = request_vm_->os_thread();
          pthread_join(os, &return_value);
        }

        request_vm_ = NULL;
      }

      if(response_vm_) {
        response_exit_ = true;
        atomic::memory_barrier();

        response_cond_.signal();

        if(atomic::poll(response_running_, false)) {
          void* return_value;
          pthread_t os = response_vm_->os_thread();
          pthread_join(os, &return_value);
        }

        response_vm_ = NULL;
      }
    }

    void Console::shutdown(STATE) {
      stop_threads(state);
      close_files(true);
      empty_request_list();
    }

    void Console::before_exec(STATE) {
      stop_threads(state);
      close_files(true);
    }

    void Console::after_exec(STATE) {
      setup_files(state);
      start_threads(state);
    }

    void Console::after_fork_child(STATE) {
      close_files(false);
      empty_request_list();

      start(state);
    }

    char* Console::read_request(STATE) {
      file::LockGuard guard(request_fd_, LOCK_EX);

      if(guard.status() != file::eLockSucceeded) {
        logger::error("%s: console: unable to lock request file", strerror(errno));
        return NULL;
      }

      char* buf[1024];

      lseek(request_fd_, 0, SEEK_SET);
      ssize_t bytes = ::read(request_fd_, buf, 1024);

      char* req = NULL;
      if(bytes > 0) {
        req = new char[bytes+1];
        memcpy(req, buf, bytes);
        req[bytes] = 0;
        request_vm_->metrics()->m.console_metrics.requests_received++;
      } else if(bytes < 0) {
        logger::error("%s: console: unable to read request", strerror(errno));
      }

      if(lseek(request_fd_, 0, SEEK_SET) < 0) {
        logger::error("%s: console: unable to rewind request file", strerror(errno));
      }
      if(ftruncate(request_fd_, 0) < 0) {
        logger::error("%s: console: unable to truncate request file", strerror(errno));
      }

      return req;
    }

    void Console::process_requests(STATE) {
      GCTokenImpl gct;
      RBX_DTRACE_CONST char* thread_name =
        const_cast<RBX_DTRACE_CONST char*>("rbx.console.request");
      request_vm_->set_name(thread_name);

      RUBINIUS_THREAD_START(const_cast<RBX_DTRACE_CONST char*>(thread_name),
                            state->vm()->thread_id(), 1);

      request_running_ = true;

      state->vm()->thread->hard_unlock(state, gct, 0);
      state->gc_independent(gct, 0);

      while(!request_exit_) {
        Object* status = fsevent_.get()->wait_for_event(state);

        if(request_exit_) break;

        if(status->nil_p()) {
          reset(state);
          continue;
        }

        char* request = read_request(state);

        if(request) {
          utilities::thread::Mutex::LockGuard lg(list_lock_);

          request_list_->push_back(request);
          response_cond_.signal();
        }
      }

      request_running_ = false;

      state->gc_dependent(gct, 0);

      RUBINIUS_THREAD_STOP(const_cast<RBX_DTRACE_CONST char*>(thread_name),
                           state->vm()->thread_id(), 1);
    }

    void Console::write_response(STATE, const char* response, native_int size) {
      file::LockGuard guard(response_fd_, LOCK_EX);

      if(guard.status() != file::eLockSucceeded) {
        logger::error("%s: unable to lock response file", strerror(errno));
        return;
      }

      if(lseek(response_fd_, 0, SEEK_SET) < 0) {
        logger::error("%s: console: unable to rewind response file", strerror(errno));
        return;
      }
      if(ftruncate(response_fd_, 0) < 0) {
        logger::error("%s: console: unable to truncate response file", strerror(errno));
        return;
      }

      if(::write(response_fd_, response, size) < 0) {
        logger::error("%s: console: unable to write response", strerror(errno));
      }

      response_vm_->metrics()->m.console_metrics.responses_sent++;
    }

    void Console::process_responses(STATE) {
      GCTokenImpl gct;
      RBX_DTRACE_CONST char* thread_name =
        const_cast<RBX_DTRACE_CONST char*>("rbx.console.response");
      response_vm_->set_name(thread_name);

      RUBINIUS_THREAD_START(const_cast<RBX_DTRACE_CONST char*>(thread_name),
                            state->vm()->thread_id(), 1);

      response_running_ = true;

      state->vm()->thread->hard_unlock(state, gct, 0);
      state->gc_dependent(gct, 0);

      char* request = NULL;

      while(!response_exit_) {
        {
          utilities::thread::Mutex::LockGuard lg(list_lock_);

          if(request_list_->size() > 0) {
            request = request_list_->back();
            request_list_->pop_back();
          }
        }

        if(response_exit_) break;

        if(request) {
          Array* args = Array::create(state, 1);
          args->aset(state, 0, String::create(state, request));
          Object* result = console_.get()->send(state, 0, state->symbol("evaluate"),
              args, cNil);

          if(String* response = try_as<String>(result)) {
            GCIndependent guard(state, 0);

            write_response(state,
                reinterpret_cast<const char*>(response->byte_address()),
                response->size());
          }

          request = NULL;
        } else {
          utilities::thread::Mutex::LockGuard lg(response_lock_);
          GCIndependent guard(state, 0);

          atomic::memory_barrier();
          if(response_exit_) break;

          response_cond_.wait(response_lock_);
        }
      }

      response_running_ = false;

      RUBINIUS_THREAD_STOP(const_cast<RBX_DTRACE_CONST char*>(thread_name),
                           state->vm()->thread_id(), 1);
    }
  }
}

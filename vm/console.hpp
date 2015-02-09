#ifndef RBX_CONSOLE_HPP
#define RBX_CONSOLE_HPP

#include "lock.hpp"
#include "auxiliary_threads.hpp"

#include "gc/root.hpp"

#include "util/thread.hpp"

#include <list>
#include <string>

namespace rubinius {
  class VM;
  class State;
  class FSEvent;
  class Object;
  class String;

  namespace console {
    class Request;
    class Response;

    class Console {
      std::string path_;

      Response* response_;
      Request* request_;

      TypedRoot<Object*> ruby_console_;

    public:
      Console(STATE);
      virtual ~Console();

      Request* request() {
        return request_;
      }

      Response* response() {
        return response_;
      }

      void start(STATE);
      void stop(STATE);

      Class* server_class(STATE);
      Object* evaluate(STATE, char* request);
    };

    typedef std::list<char*> RequestList;

    class Response : public AuxiliaryThread, public Lockable {
      Console* console_;

      std::string path_;
      int fd_;

      RequestList* request_list_;

      utilities::thread::SpinLock list_lock_;

      utilities::thread::Mutex response_lock_;
      utilities::thread::Condition response_cond_;

    public:

      Response(STATE, Console* console);
      virtual ~Response();

      void initialize(STATE);
      void start_thread(STATE);
      void run(STATE);
      void wakeup(STATE);
      void close_response(STATE);
      void clear_requests(STATE);
      void stop_thread(STATE);

      void before_exec(STATE);
      void after_exec(STATE);
      void after_fork_child(STATE);

      void send_request(STATE, const char* request);
      void write_response(STATE, const char* response, native_int size);
    };

    class Request : public AuxiliaryThread, public Lockable {
      Console* console_;
      Response* response_;

      std::string path_;
      int fd_;

      TypedRoot<FSEvent*> fsevent_;

    public:

      Request(STATE, Console* console);

      void initialize(STATE);
      void start_thread(STATE);
      void run(STATE);
      void wakeup(STATE);
      void setup_request(STATE);
      void close_request(STATE);
      void stop_thread(STATE);

      void before_exec(STATE);
      void after_exec(STATE);
      void after_fork_child(STATE);

      char* read_request(STATE);
    };
  }
}

#endif

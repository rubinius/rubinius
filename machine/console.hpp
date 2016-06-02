#ifndef RBX_CONSOLE_HPP
#define RBX_CONSOLE_HPP

#include "machine_threads.hpp"

#include "memory/root.hpp"

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
    class Listener;
    class Request;
    class Response;

    class Console {
      std::string console_path_;
      std::string request_path_;
      std::string response_path_;

      Listener* listener_;
      Response* response_;
      Request* request_;

      memory::TypedRoot<Object*> ruby_console_;

    public:
      Console(STATE);
      virtual ~Console();

      Object* ruby_console() {
        return ruby_console_.get();
      }

      std::string& console_path() {
        return console_path_;
      }

      std::string& request_path() {
        return request_path_;
      }

      std::string& response_path() {
        return response_path_;
      }

      bool connected_p();

      void reset();

      void start(STATE);
      void stop(STATE);
      void after_fork_child(STATE);

      void accept(STATE);
      Class* server_class(STATE);
    };

    class Listener : public MachineThread {
      Console* console_;

      memory::TypedRoot<FSEvent*> fsevent_;

      int fd_;

    public:

      Listener(STATE, Console* console);
      virtual ~Listener();

      bool connection_initiated();

      void start_thread(STATE);
      void initialize(STATE);
      void run(STATE);
      void wakeup(STATE);
    };

    typedef std::list<char*> RequestList;

    class Response : public MachineThread {
      Console* console_;

      memory::TypedRoot<Channel*> inbox_;
      memory::TypedRoot<Channel*> outbox_;

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
      void stop_thread(STATE);
      void after_fork_child(STATE);

      void close_response();
      void clear_requests();

      void send_request(STATE, const char* request);
      void write_response(STATE, const char* response, native_int size);
    };

    class Request : public MachineThread {
      Console* console_;
      Response* response_;

      bool enabled_;

      int fd_;

      memory::TypedRoot<FSEvent*> fsevent_;

    public:

      Request(STATE, Console* console, Response* response);

      bool enabled_p() {
        return enabled_;
      }

      void initialize(STATE);
      void start_thread(STATE);
      void run(STATE);
      void wakeup(STATE);
      void stop_thread(STATE);
      void after_fork_child(STATE);

      void close_request();

      char* read_request(STATE);
    };
  }
}

#endif

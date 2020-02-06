#ifndef RBX_CONSOLE_HPP
#define RBX_CONSOLE_HPP

#include "machine_thread.hpp"
#include "spinlock.hpp"

#include <list>
#include <mutex>
#include <string>

namespace rubinius {
  class Console;
  class Channel;
  class ThreadState;
  class FSEvent;
  class Object;
  class String;

  namespace console {
    class Listener;
    class Request;
    class Response;

    class Listener : public MachineThread {
      Console* console_;

      FSEvent* fsevent_;

      int fd_;

    public:

      Listener(STATE, Console* console);
      virtual ~Listener();

      bool connection_initiated();

      void start_thread(STATE);
      void initialize(STATE);
      void run(STATE);
      void wakeup(STATE);

      void trace_objects(STATE, std::function<void (STATE, Object**)> f);
    };

    typedef std::list<char*> RequestList;

    class Response : public MachineThread {
      Console* console_;

      Channel* inbox_;
      Channel* outbox_;

      int fd_;

      RequestList* request_list_;

      locks::spinlock_mutex list_lock_;

      std::mutex response_lock_;
      std::condition_variable response_cond_;

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
      void write_response(STATE, const char* response, intptr_t size);

      void trace_objects(STATE, std::function<void (STATE, Object**)> f);
    };

    class Request : public MachineThread {
      Console* console_;
      Response* response_;

      bool enabled_;

      int fd_;

      FSEvent* fsevent_;

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

      void trace_objects(STATE, std::function<void (STATE, Object**)> f);
    };
  }

  using namespace console;

  class Console {
    std::string console_path_;
    std::string request_path_;
    std::string response_path_;

    Listener* listener_;
    Response* response_;
    Request* request_;

    Object* ruby_console_;

  public:
    Console(STATE);
    virtual ~Console();

    Object* ruby_console() {
      return ruby_console_;
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

    void trace_objects(STATE, std::function<void (STATE, Object**)> f);
  };
}

#endif

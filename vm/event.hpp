#ifndef RBX_EVENT_HPP
#define RBX_EVENT_HPP

#include <sys/wait.h>
#include <ev.h>

#include "prelude.hpp"
#include "builtin/task.hpp"
#include "virtual.hpp"

namespace rubinius {
  class IOBuffer;

  namespace event {
    class Loop;

    class Event {
    public:
      STATE;
      ObjectCallback* channel;
      size_t id;
      TypedRoot<IOBuffer*> buffer;
      Loop* loop;

      Event(STATE, ObjectCallback* chan);
      virtual ~Event() { }
      virtual void start() = 0;
      virtual void stop() = 0;
      virtual bool for_fd_p(int fd) { return false; }
      virtual bool activated() = 0;
      bool tracked() { return id > 0; }
    };

    class IO : public Event {
    public:
      struct ev_io ev;

      int fd;

      IO(STATE, ObjectCallback* chan) : Event(state, chan) { }
      virtual ~IO() { stop(); }
      virtual bool for_fd_p(int fd);
      virtual void stop();
      virtual void start();
      virtual bool activated() = 0;
    };

    class Write : public IO {
    public:
      Write(STATE, ObjectCallback*, int fd);
      virtual ~Write() { }
      virtual bool activated();
    };

    class Read : public IO {
    public:
      size_t count;

      Read(STATE, ObjectCallback* chan, int fd);
      virtual ~Read() { }
      void into_buffer(rubinius::IOBuffer* buffer, size_t bytes);
      virtual bool activated();
    };

    class Signal : public Event {
    public:
      struct ev_signal ev;
      int signal;

      Signal(STATE, ObjectCallback *chan, int sig);
      virtual ~Signal() { stop(); }
      virtual void start();
      virtual void stop();
      virtual bool activated();
    };

    class Timer : public Event {
    public:
      struct ev_timer ev;

      Timer(STATE, ObjectCallback* chan, double seconds);
      virtual ~Timer() { stop(); }
      virtual void start();
      virtual void stop();
      virtual bool activated();
    };

    class Child : public Event {
    public:
      pid_t pid;
      int options;

      Child(STATE, ObjectCallback* chan, pid_t pid, int opts);
      bool poll();
    };

    class Loop {
    public:
      struct ev_loop *base;
      std::vector<Event*> events;
      size_t event_ids;
      bool owner;

      Loop();
      Loop(int options);
      Loop(struct ev_loop *loop);
      ~Loop();

      size_t loop_count();
      size_t num_of_events();
      void start(Event* ev);
      void poll();
      void run_and_wait();
      void clear_by_fd(int fd);
      void clear_by_channel(void* chan);
      void clear_by_id(size_t id);
      void remove_event(Event* ev);
      void remove_signal(int sig);
      void add_signal(int sig, Signal* handler);
    };
  };
};

#endif

#ifndef RBX_EVENT_HPP
#define RBX_EVENT_HPP

#include <list>

#include <sys/wait.h>
#include <ev.h>

#include "prelude.hpp"
#include "builtin/task.hpp"
#include "virtual.hpp"

/*
 *  TODO: We are probably leaking ObjectCallbacks */
namespace rubinius {
  class IOBuffer;

  namespace event {

    enum LoopTypes {
      HANDLE_SIGNALS
    };

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
      OBJECT tag;

      Timer(STATE, ObjectCallback* chan, double seconds, OBJECT obj = Qnil);
      virtual ~Timer() { stop(); }
      virtual void start();
      virtual void stop();
      virtual bool activated();
    };

    /**
     *  Slightly different, since these events are not
     *  tracked discretely by libev. Instead, we abstract
     *  them out into their own list to be accessed when
     *  a SIGCHLD occurs. Because of this, there is no
     *  point in using the Event interface from above,
     *  although the callback system is still useful for
     *  us.
     *
     *  TODO: Cancel-by-id handling via negatives?
     *
     *  TODO: Review when multiple loops or native threads.
     */
    class Child {
    public:   /* Types */

      /*
       *  TODO: If there is a really performance-strapped app that
       *        bottlenecks here, revisit the map<pid_t, list<Child*>>
       *        version. It is just overkill for now.
       */
      typedef std::list<Child*> Waiters;

      typedef struct ev_signal Watcher;
      typedef struct ev_loop EVLoop;

      /**
       *  Special case.
       */
      class Event : public Signal {
      public:

        Event(STATE) : Signal(state, NULL, SIGCHLD) {}
        virtual ~Event() {}

      public:

        virtual bool activated() { Child::find_finished(state); return false; }

        /* TODO: Remove when continuum is set up properly. */
        virtual void stop() { throw std::runtime_error("SIGCHLD handler stop()"); }

      };


    private:   /* Ctors */

      Child(STATE, ObjectCallback* channel, pid_t pid, int opts);
      ~Child();


    public:   /* Class interface */

      /** Add a new interested channel. */
      static void     add(STATE, ObjectCallback* channel, pid_t pid, int opts);

      /** Figure which child(ren) finished when SIGCHLD received. */
      static void     find_finished(STATE);

      /** All current Child events. */
      static Waiters& waiters() { static Waiters our_waiters; return our_waiters; };


    public:   /* Accessors */

      ObjectCallback* channel() { return channel_; }
      int             options() { return options_; }
      pid_t           pid()     { return pid_; }


    private:  /* Instance vars */

      ObjectCallback*   channel_;
      int               options_;
      pid_t             pid_;

    };

    /**
     *  TODO: Needs review when multiple loops are introduced.
     */
    class Loop {
    public:
      struct ev_loop *base;
      std::vector<Event*> events;
      size_t event_ids;
      bool owner;

      Loop(int options = 0);
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
    };

  };
};

#endif

#ifndef RBX_EVENT_HPP
#define RBX_EVENT_HPP

#include <list>

#include <sys/wait.h>
#include <sys/signal.h>
#include <ev.h>

#include "prelude.hpp"
#include "builtin/task.hpp"
#include "virtual.hpp"


/** @todo We are leaking ObjectCallbacks. They need to be deleted (elsewhere.) */
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
      void into_buffer(Object* maybe_buffer, std::size_t bytes);
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

    /**
     *  @note   Unlike other Events, we use a dynamically allocated
     *          ev_timer here because some evil shit libev does makes
     *          the compiler think there may be something wrong with
     *          ev_timer_init. I could not be bothered to figure out
     *          whether the warning was for cause. And no, casting
     *          does not work because it would potentially create a
     *          temporary. --rue
     */
    class Timer : public Event {
    public:
      Object* tag;
      struct ev_timer* timer_;

      Timer(STATE, ObjectCallback* chan, double seconds, Object* obj = Qnil);
      virtual ~Timer() { stop(); }
      virtual void start();
      virtual void stop();
      virtual bool activated();
    };

    /**
     *  Slightly different, since these events are not
     *  tracked discretely by libev. Instead, we abstract
     *  them out into their own list to be accessed when
     *  a SIGCHLD occurs.
     *
     *  A single special Event, a Child::Event object, is
     *  inserted to track all SIGCHLDs. The actual Child
     *  "events" are held internally here similarly to what
     *  the real event loop does. When our singular Event
     *  fires, we dispatch using the internal Childs.
     *
     *  @see  Further discussion below and in event.cpp.
     *
     *  @todo Cancel-by-id handling via negatives?
     *
     *  @todo Review when multiple loops or native threads.
     */
    class Child {
    public:   /* Types */

      /*
       *  @todo If there is a really performance-strapped app that
       *        bottlenecks here, revisit the map<pid_t, list<Child*>>
       *        version. It is just overkill for now.
       */
      typedef std::list<Child*> Waiters;

      typedef struct ev_signal Watcher;
      typedef struct ev_loop EVLoop;

      /**
       *  Child::Event is a special case of Signal.
       *
       *  Whereas other Signals merely send to their waiting
       *  Channels or Callbacks, our task is a bit trickier.
       *
       *  This is also a "singleton" Event: only one SIGCHLD
       *  Event exists, and it uses the Child objects as its
       *  internal event-like things somewhat analogously to
       *  the other real Events.
       */
      class Event : public Signal {
      public:

        /** Create the Event. Nothing special here, activated() is the override. */
        Event(STATE): Signal(state, NULL, SIGCHLD) {}
        virtual ~Event() {}

      public:   /* Interface */

        /** Special override that invokes the main Child::find_finished(). */
        virtual bool activated() { Child::find_finished(state); return false; }
      };


    private:   /* Ctors */

      /** Create Child using API similar to "real" Events. */
      Child(ObjectCallback* channel, pid_t pid, int opts);
      ~Child();


    public:   /* Class interface */

      /** Add a new interested channel. */
      static void     add(STATE, ObjectCallback* channel, pid_t pid, int opts);

      /** Figure which child(ren) finished when SIGCHLD received. */
      static void     find_finished(STATE);

      /** All current Child events. */
      static Waiters& waiters() { static Waiters our_waiters; return our_waiters; };


    public:   /* Accessors */

      /** Returns callback to write result to channel. */
      ObjectCallback* channel() { return channel_; }
      /** Returns options given to be passed to ::waitpid. */
      int             options() { return options_; }
      /** Returns pid given to wait for. */
      pid_t           pid()     { return pid_; }


    private:  /* Instance vars */

      /** Callback to write result to channel when ready. */
      ObjectCallback* channel_;
      /** Options to pass to ::waitpid, currently just WNOHANG. */
      int             options_;
      /** Process ID to wait for, also -1 for any and others per system. */
      pid_t           pid_;
    };


    /**
     *  @todo Needs review when multiple loops are introduced.
     */
    class Loop {
    public:   /* Ctors */

      Loop(int options = 0);
      Loop(struct ev_loop *loop);

      /** Stop any remaining watchers and kill the loop if we control it. */
      ~Loop();


    public:   /* Interface */

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


    public:   /* Instance vars */

      struct ev_loop*     base;
      std::vector<Event*> events;
      size_t              event_ids;
      /** Options given at time of creation. */
      int                 options_;
      /** Whether this Loop controls the event loop it uses. */
      bool                owner;
    };

  }     /* event */
}       /* rubinius */

#endif

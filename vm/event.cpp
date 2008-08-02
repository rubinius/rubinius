/* A C++ wrapper around libev, which Rubinius uses for all event handling.
 * This is the folcrum of the thread subsystem. */

#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include "builtin/tuple.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/io.hpp"
#include "builtin/thread.hpp"

#include "event.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ev.h>

#include <vector>

namespace rubinius {
  namespace event {
    static void dispatch(Event *obj) {
      if(obj->activated()) {
        if(obj->loop) {
          obj->loop->remove_event(obj);
        }

        if(obj->tracked()) {
          delete obj;
        } else {
          obj->stop();
        }
      }
    }

    template <typename S>
      void tramp(EV_P_ S* ev, int revents) {
        event::dispatch((Event*)ev->data);
      }

    Event::Event(STATE, ObjectCallback* chan) :
      state(state), channel(chan), id(0),
      buffer(state), loop(NULL) { }

    void IO::stop() {
      ev_io_stop(loop->base, &ev);
    }

    void IO::start() {
      ev_io_start(loop->base, &ev);
    }

    bool IO::for_fd_p(int in) {
      if(fd == in) return true;
      return false;
    }

    Write::Write(STATE, ObjectCallback* chan, int fd) : IO(state, chan) {
      ev_io_init(&ev, event::tramp<struct ev_io>, fd, EV_WRITE);
      ev.data = this;
    }

    bool Write::activated() {
      channel->call(Qnil);
      return true;
    }

    Read::Read(STATE, ObjectCallback* chan, int ifd) :
        IO(state, chan), count(0) {
      fd = ifd;
      ev_io_init(&ev, event::tramp<struct ev_io>, fd, EV_READ);
      ev.data = this;
    }

    void Read::into_buffer(rubinius::IOBuffer *buf, size_t bytes) {
      count = bytes;
      buffer.set(buf);
    }

    bool Read::activated() {
      OBJECT ret;

      if(buffer->nil_p()) {
        ret = Object::i2n(state, fd);
      } else {
        /* the + 1 is for the null on the end */
        size_t bytes_to_read = count + 1;

        if(buffer->left() < bytes_to_read) {
          bytes_to_read = buffer->left() - 1;
        }

        char* start = buffer->at_unused();
        while(1) {
          int i = read(fd, start, bytes_to_read);

          /* EOF seen */
          if(i == 0) {
            ret = Qnil;

          /* didn't work out... */
          } else if(i == -1) {
            /* we were interrupted, how rude. go again. */
            if(errno == EINTR) continue;

            /* not sure. Send a system error */
            ret = Tuple::from(state, 2, state->symbol("error"), Object::i2n(errno));
          } else {
            /* clamp */
            start[i] = 0;

            buffer->read_bytes(i);
            ret = Object::i2n(i);
          }

          break;
        }
      }

      channel->call(ret);

      return true;
    }

    Signal::Signal(STATE, ObjectCallback *chan, int sig) : 
        Event(state, chan), signal(sig) {
      ev_signal_init(&ev, event::tramp<struct ev_signal>, sig);
      ev.data = this;
    }

    void Signal::start() {
      loop->remove_signal(signal);
      loop->add_signal(signal, this);
      ev_signal_start(loop->base, &ev);
    }

    void Signal::stop() {
      ev_signal_stop(loop->base, &ev);
    }

    bool Signal::activated() {
      channel->call(state->current_thread());
      return false;
    }

    Timer::Timer(STATE, ObjectCallback* chan, double seconds) : 
        Event(state, chan) {
      ev_timer_init(&ev, event::tramp<struct ev_timer>, (ev_tstamp)seconds, 0);
      ev.data = this;
    }

    void Timer::start() {
      ev_timer_start(loop->base, &ev);
    }

    void Timer::stop() {
      ev_timer_stop(loop->base, &ev);
    }

    bool Timer::activated() {
      channel->call(Qnil);
      return true;
    }

    Child::Child(STATE, ObjectCallback* chan, pid_t pid, int opts) :
      Event(state, chan), pid(pid), options(opts) { }

    bool Child::poll() {
      pid_t p;
      int status;
      OBJECT ret;

      while((p = waitpid(pid, &status, WNOHANG || options)) <= -1
          && errno == EINTR);

      if(p > 0) {
        if(WIFEXITED(status)) {
          ret = Object::i2n(WEXITSTATUS(status));
        } else {
          /* Could support WIFSIGNALED also. */
          ret = Qtrue;
        }
        channel->call(Tuple::from(state, 2, Object::i2n(p), ret));
      } else if(pid == -1 && errno == ECHILD) {
        channel->call(Qfalse);
      } else if(pid == 0 && (options & WNOHANG)) {
        channel->call(Qnil);
      } else {
        return false;
      }

      return true;
    }

    Loop::Loop(struct ev_loop *loop) : 
      base(loop), event_ids(0), owner(false) { }

    Loop::Loop() : event_ids(0), owner(true) {
      base = ev_default_loop(0);
    }

    Loop::Loop(int opts) : event_ids(0), owner(true) {
      base = ev_loop_new(opts);
    }

    /* Gives this loop ownership of +ev+, letting it delete +ev+
     * when +ev+ is done, then starts the event. */
    void Loop::start(Event* ev) {
      ev->loop = this;
      ev->id = ++event_ids;
      events.push_back(ev);
      ev->start();
    }

    Loop::~Loop() {
      if(owner) ev_loop_destroy(base);
    }

    size_t Loop::num_of_events() {
      return events.size();
    }

    size_t Loop::loop_count() {
      return (size_t)ev_loop_count(base);
    }

    void Loop::poll() {
      ev_loop(base, EVLOOP_NONBLOCK);
    }

    void Loop::run_and_wait() {
      ev_loop(base, EVLOOP_ONESHOT);
    }

    void Loop::clear_by_fd(int fd) {
      std::vector<Event*>::iterator it;
      for(it = events.begin(); it != events.end();) {
        if((*it)->for_fd_p(fd)) {
          delete *it;
          it = events.erase(it);
        } else {
          it++;
        }
      }
    }

    void Loop::clear_by_channel(void* chan) {
      std::vector<Event*>::iterator it;
      for(it = events.begin(); it != events.end();) {
        if((*it)->channel == chan) {
          delete *it;
          it = events.erase(it);
        } else {
          it++;
        }
      }
    }

    void Loop::clear_by_id(size_t id) {
      std::vector<Event*>::iterator it;
      for(it = events.begin(); it != events.end();) {
        if((*it)->id == id) {
          delete *it;
          it = events.erase(it);
        } else {
          it++;
        }
      }
    }

    void Loop::remove_event(Event *ev) {
      std::vector<Event*>::iterator it;
      for(it = events.begin(); it != events.end();) {
        if(*it == ev) {
          it = events.erase(it);
        } else {
          it++;
        }
      }
    }

    void Loop::remove_signal(int sig) {
    }

    void Loop::add_signal(int sig, Signal* ev) {
    }
  } // event
} // rubinius

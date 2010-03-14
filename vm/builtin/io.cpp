#include <iostream>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>

#include "builtin/io.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/channel.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"
#include "builtin/bytearray.hpp"
#include "primitives.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"

#include "vm/object_utils.hpp"
#include "vm/on_stack.hpp"

#include "native_thread.hpp"

namespace rubinius {
  void IO::init(STATE) {
    GO(io).set(state->new_class("IO", G(object)));
    G(io)->set_object_type(state, IOType);

    GO(iobuffer).set(state->new_class("Buffer", G(object), G(io)));
    G(iobuffer)->set_object_type(state, IOBufferType);
  }

  IO* IO::create(STATE, int fd) {
    IO* io = state->new_object<IO>(G(io));
    io->descriptor(state, Fixnum::from(fd));
    io->set_mode(state);
    io->ibuffer(state, IOBuffer::create(state));
    io->eof(state, Qfalse);
    io->lineno(state, Fixnum::from(0));

    // Don't bother to add finalization for stdio
    if(fd >= 3) {
      state->om->needs_finalization(io);
    }

    return io;
  }

  IO* IO::allocate(STATE, Object* self) {
    IO* io = state->new_object<IO>(G(io));
    io->descriptor(state, (Fixnum*)Qnil);
    io->mode(state, (Fixnum*)Qnil);
    io->ibuffer(state, IOBuffer::create(state));
    io->eof(state, Qfalse);
    io->lineno(state, Fixnum::from(0));

    // Ensure the instance's class is set (i.e. for subclasses of IO)
    io->klass(state, as<Class>(self));

    state->om->needs_finalization(io);

    return io;
  }

  Fixnum* IO::open(STATE, String* path, Fixnum* mode, Fixnum* perm) {
    int fd = ::open(path->c_str(), mode->to_native(), perm->to_native());
    return Fixnum::from(fd);
  }


  namespace {
    /** Utility function used by IO::select, returns highest descriptor. */
    static inline native_int hidden_fd_set_from_array(VM* state,
                               Object* maybe_descriptors, fd_set* set)
    {
      if(NULL == set) {
        return 0;
      }

      Array* descriptors = as<Array>(maybe_descriptors);

      FD_ZERO(set);
      native_int highest = 0;

      for(std::size_t i = 0; i < descriptors->size(); ++i) {
        native_int descriptor = as<IO>(descriptors->get(state, i))->to_fd();
        highest = descriptor > highest ? descriptor : highest;

        if(descriptor > 0) FD_SET(descriptor, set);
      }

      return highest;
    }

    /** Utility function used by IO::select, returns Array of IOs that were set. */
    static inline Array* hidden_reject_unset_fds(VM* state,
                           Object* maybe_originals, fd_set* set)
    {
      if(NULL == set) {
        return Array::create(state, 0);
      }

      Array* originals = as<Array>(maybe_originals);
      Array* selected = Array::create(state, originals->size());

      for(std::size_t i = 0; i < originals->size(); ++i) {
        IO* io = as<IO>(originals->get(state, i));

        int fd = io->to_fd();
        if(fd < 0 || FD_ISSET(fd, set)) {
          selected->set(state, i, io);
        }
      }

      return selected;
    }
  }

  /**
   *  Ergh. select/FD_* is not exactly user-oriented design.
   *
   *  @todo This is highly unoptimised since we always rebuild the FD_SETs. --rue
   */
  Object* IO::select(STATE, Object* readables, Object* writables,
                     Object* errorables, Object* timeout,
                     CallFrame* calling_environment)
  {
    // GC protection / awareness
    OnStack<3> os(state, readables, writables, errorables);

    fd_set read_set;
    fd_set* maybe_read_set = readables->nil_p() ? NULL : &read_set;

    fd_set write_set;
    fd_set* maybe_write_set = writables->nil_p() ? NULL : &write_set;

    fd_set error_set;
    fd_set* maybe_error_set = errorables->nil_p() ? NULL : &error_set;

    native_int highest = 0;
    native_int candidate = 0;

    /* Build the sets, track the highest descriptor number. These handle NULLs */
    highest = hidden_fd_set_from_array(state, readables, maybe_read_set);

    candidate = hidden_fd_set_from_array(state, writables, maybe_write_set);
    highest = candidate > highest ? candidate : highest;

    candidate = hidden_fd_set_from_array(state, errorables, maybe_error_set);
    highest = candidate > highest ? candidate : highest;

    struct timeval future;
    struct timeval limit;
    struct timeval* maybe_limit = NULL;

    if(!timeout->nil_p()) {
      unsigned long long microseconds = as<Integer>(timeout)->to_ulong_long();
      limit.tv_sec = microseconds / 1000000;
      limit.tv_usec = microseconds % 1000000;
      maybe_limit = &limit;

      // Get the current time to be used if select is interrupted and we
      // have to recalculate the sleep time
      assert(gettimeofday(&future, NULL) == 0);
      timeradd(&future, &limit, &future);
    }

    WaitingForSignal waiter;
    native_int events;


    /* And the main event, pun intended */
    retry:
    state->install_waiter(waiter);
    {
      GlobalLock::UnlockGuard lock(state->global_lock());

      events = ::select((highest + 1), maybe_read_set,
                                                  maybe_write_set,
                                                  maybe_error_set,
                                                  maybe_limit);
    }
    state->clear_waiter();

    if(events == -1) {
      if(errno == EAGAIN || errno == EINTR) {
        if(!state->check_async(calling_environment)) return NULL;

        // Recalculate the limit and go again.
        if(maybe_limit) {
          struct timeval now;
          assert(gettimeofday(&now, NULL) == 0);
          timersub(&future, &now, &limit);
        }

        goto retry;
      }

      Exception::errno_error(state, "select(2) failed");
      return NULL;
    }

    /* Timeout expired */
    if(events == 0) return Qnil;

    /* Build the results. */
    Array* output = Array::create(state, 3);

    /* These handle NULL sets. */
    output->set(state, 0, hidden_reject_unset_fds(state, readables, maybe_read_set));
    output->set(state, 1, hidden_reject_unset_fds(state, writables, maybe_write_set));
    output->set(state, 2, hidden_reject_unset_fds(state, errorables, maybe_error_set));

    return output;
  }


/* Instance methods */

  Object* IO::reopen(STATE, IO* other) {
    native_int cur_fd   = to_fd();
    native_int other_fd = other->to_fd();

    if(dup2(other_fd, cur_fd) == -1) {
      Exception::errno_error(state, "reopen");
      return NULL;
    }

    set_mode(state);
    if(IOBuffer* ibuf = try_as<IOBuffer>(ibuffer())) {
      ibuf->reset(state);
    }

    return Qtrue;
  }

  Object* IO::ensure_open(STATE) {
    if(descriptor_->nil_p()) {
      Exception::io_error(state, "uninitialized stream");
    }
    else if(to_fd() == -1) {
      Exception::io_error(state, "closed stream");
    }
    else if(to_fd() == -2) {
      Exception::io_error(state, "shutdown stream");
    }

    return Qnil;
  }

  Object* IO::connect_pipe(STATE, IO* lhs, IO* rhs) {
    int fds[2];
    if(pipe(fds) == -1) {
      Exception::errno_error(state, "creating pipe");
    }

    lhs->descriptor(state, Fixnum::from(fds[0]));
    rhs->descriptor(state, Fixnum::from(fds[1]));

    lhs->set_mode(state);
    rhs->set_mode(state);
    return Qtrue;
  }

  Integer* IO::seek(STATE, Integer* amount, Fixnum* whence) {
    ensure_open(state);

    if(Bignum* big = try_as<Bignum>(amount)) {
      if((size_t)mp_count_bits(big->mp_val()) > (sizeof(off_t) * 8)) {
        return (Integer*)Primitives::failure();
      }
    }

    off_t offset = amount->to_long_long();
    off_t position = lseek(to_fd(), offset, whence->to_native());

    if(position == -1) {
      Exception::errno_error(state);
    }

    return Integer::from(state, position);
  }

  /** This is NOT the same as shutdown(). */
  Object* IO::close(STATE) {
    ensure_open(state);

    /** @todo   Should this be just int? --rue */
    native_int desc = to_fd();

    // Already closed, ignore.
    if(desc == -1) {
      return Qnil;
    }

    // Invalid descriptor no matter what.
    descriptor(state, Fixnum::from(-1));

    switch(::close(desc)) {
    case -1:
      Exception::errno_error(state);
      break;

    case 0:
      break;

    default:
      std::ostringstream message;
      message << "::close(): Unknown error on fd " << desc;
      Exception::system_call_error(state, message.str());
    }

    return Qnil;
  }

  /**
   *  This is NOT the same as close().
   *
   *  @todo   Need to build the infrastructure to be able to only
   *          remove read or write waiters if a partial shutdown
   *          is requested. --rue
   */
  Object* IO::shutdown(STATE, Fixnum* how) {
    ensure_open(state);

    int which = how->to_int();
    native_int desc = to_fd();

    if(which != SHUT_RD && which != SHUT_WR && which != SHUT_RDWR) {
      std::ostringstream message;
      message << "::shutdown(): Invalid `how` " << which << " for fd " << desc;
      Exception::argument_error(state, message.str().c_str());
    }

    switch(::shutdown(desc, which)) {
    case -1:
      Exception::errno_error(state);
      break;

    case 0:
      if(which == SHUT_RDWR) {
        /* Yes, it really does need to be closed still. */
        (void) close(state);

        descriptor(state, Fixnum::from(-2));
      }

      break;

    default:
      std::ostringstream message;
      message << "::shutdown(): Unknown error on fd " << desc;
      Exception::system_call_error(state, message.str());
    }

    return how;
  }

  native_int IO::to_fd() {
    return descriptor_->to_native();
  }

  void IO::set_mode(STATE) {
    int acc_mode = fcntl(to_fd(), F_GETFL);
    if(acc_mode < 0) {
      Exception::errno_error(state);
    }
    mode(state, Fixnum::from(acc_mode));
  }

  void IO::unsafe_set_descriptor(native_int fd) {
    descriptor_ = Fixnum::from(fd);
  }

  void IO::force_read_only(STATE) {
    int m = mode_->to_native();
    mode(state, Fixnum::from((m & ~O_ACCMODE) | O_RDONLY));
  }

  void IO::force_write_only(STATE) {
    int m = mode_->to_native();
    mode(state, Fixnum::from((m & ~O_ACCMODE) | O_WRONLY));
  }

  void IO::finalize(STATE) {
    if(descriptor_->nil_p()) return;

    native_int fd = descriptor_->to_native();

    // don't close stdin, stdout, stderr (0, 1, 2)
    if(fd >= 3) {
      ::close(fd);
      descriptor(state, Fixnum::from(-1));
    }
  }

  Object* IO::sysread(STATE, Fixnum* number_of_bytes, CallFrame* calling_environment) {
    std::size_t count = number_of_bytes->to_ulong();
    String* buffer = String::create_pinned(state, number_of_bytes);

    ssize_t bytes_read;
    native_int fd = descriptor()->to_native();

    OnStack<1> variables(state, buffer);

    WaitingForSignal waiter;

  retry:
    state->install_waiter(waiter);

    {
      GlobalLock::UnlockGuard lock(state->global_lock());
      bytes_read = ::read(fd, buffer->byte_address(), count);
    }

    state->clear_waiter();

    buffer->unpin();

    if(bytes_read == -1) {
      if(errno == EAGAIN || errno == EINTR) {
        if(state->check_async(calling_environment)) goto retry;
      } else {
        Exception::errno_error(state, "read(2) failed");
      }

      return NULL;
    }

    if(bytes_read == 0) {
      return Qnil;
    }

    buffer->num_bytes(state, Fixnum::from(bytes_read));
    return buffer;
  }

  Object* IO::read_if_available(STATE, Fixnum* number_of_bytes) {
    fd_set set;
    FD_ZERO(&set);

    native_int fd = descriptor()->to_native();
    FD_SET(fd, &set);

    struct timeval tv = {0,0};
    int res = ::select(fd+1, &set, 0, 0, &tv);

    if(res == 0) {
      Exception::errno_error(state, "no data ready", EAGAIN);
      return 0;
    } else if(res <= 0) {
      Exception::errno_error(state, "read(2) failed");
      return 0;
    }

    std::size_t count = number_of_bytes->to_ulong();
    String* buffer = String::create_pinned(state, number_of_bytes);

    // There is a minor race here. If another thread is running concurrently
    // and it reads from fd, then our select might say there is data, but when
    // we go to read from it, we block.
    //
    // The problem is that twiddle the O_NONBLOCK bit has the same problem,
    // so when there are concurrent IO reads, we'll need to enforce
    // some kind of integrity here.
    ssize_t bytes_read = ::read(fd, buffer->byte_address(), count);

    buffer->unpin();

    if(bytes_read == -1) {
      Exception::errno_error(state, "read(2) failed");
    }

    if(bytes_read == 0) return Qnil;

    buffer->num_bytes(state, Fixnum::from(bytes_read));
    return buffer;
  }

  Object* IO::write(STATE, String* buf) {
    ssize_t cnt = ::write(this->to_fd(), buf->byte_address(), buf->size());

    if(cnt == -1) {
      Exception::errno_error(state);
      return NULL;
    }

    return Integer::from(state, cnt);
  }

  Object* IO::blocking_read(STATE, Fixnum* bytes) {
    String* str = String::create(state, bytes);

    ssize_t cnt = ::read(this->to_fd(), str->byte_address(), bytes->to_native());
    if(cnt == -1) {
      Exception::errno_error(state);
    } else if(cnt == 0) {
      return Qnil;
    }

    str->num_bytes(state, Fixnum::from(cnt));

    return str;
  }

  Object* IO::query(STATE, Symbol* op) {
    ensure_open(state);

    native_int fd = to_fd();

    if(op == state->symbol("tty?")) {
      return isatty(fd) ? Qtrue : Qfalse;
    } else if(op == state->symbol("ttyname")) {
      return String::create(state, ttyname(fd));
    } else {
      return Qnil;
    }
  }

  // Stole/ported from 1.8.7. The system fnmatch doesn't support
  // a bunch of things this does (and must).

#ifndef CASEFOLD_FILESYSTEM
# if defined DOSISH || defined __VMS
#   define CASEFOLD_FILESYSTEM 1
# else
#   define CASEFOLD_FILESYSTEM 0
# endif
#endif

#define FNM_NOESCAPE	0x01
#define FNM_PATHNAME	0x02
#define FNM_DOTMATCH	0x04
#define FNM_CASEFOLD	0x08
#if CASEFOLD_FILESYSTEM
#define FNM_SYSCASE	FNM_CASEFOLD
#else
#define FNM_SYSCASE	0
#endif

#define FNM_NOMATCH	1
#define FNM_ERROR	2

#define downcase(c) (nocase && ISUPPER(c) ? tolower(c) : (c))
#define compare(c1, c2) (((unsigned char)(c1)) - ((unsigned char)(c2)))
#define Next(p) ((p) + 1)
#define Inc(p) (++(p))
#define Compare(p1, p2) (compare(downcase(*(p1)), downcase(*(p2))))

  namespace {

    static char *bracket(const char* p, const char* s, int flags) {
      const int nocase = flags & FNM_CASEFOLD;
      const int escape = !(flags & FNM_NOESCAPE);

      int ok = 0, nope = 0;

      if(*p == '!' || *p == '^') {
        nope = 1;
        p++;
      }

      while(*p != ']') {
        const char *t1 = p;
        if(escape && *t1 == '\\') t1++;
        if(!*t1) return NULL;

        p = Next(t1);
        if(p[0] == '-' && p[1] != ']') {
          const char *t2 = p + 1;
          if(escape && *t2 == '\\') t2++;
          if(!*t2) return NULL;

          p = Next(t2);
          if(!ok && Compare(t1, s) <= 0 && Compare(s, t2) <= 0) ok = 1;
        } else {
          if(!ok && Compare(t1, s) == 0) ok = 1;
        }
      }

      return ok == nope ? NULL : (char *)p + 1;
    }
    /* If FNM_PATHNAME is set, only path element will be matched. (upto '/' or '\0')
       Otherwise, entire string will be matched.
       End marker itself won't be compared.
       And if function succeeds, *pcur reaches end marker.
       */
#define UNESCAPE(p) (escape && *(p) == '\\' ? (p) + 1 : (p))
#define ISEND(p) (!*(p) || (pathname && *(p) == '/'))
#define RETURN(val) return *pcur = p, *scur = s, (val);

    bool mri_fnmatch_helper(const char** pcur, const char** scur, int flags) {
      const int period = !(flags & FNM_DOTMATCH);
      const int pathname = flags & FNM_PATHNAME;
      const int escape = !(flags & FNM_NOESCAPE);
      const int nocase = flags & FNM_CASEFOLD;

      const char *ptmp = 0;
      const char *stmp = 0;

      const char *p = *pcur;
      const char *s = *scur;

      if (period && *s == '.' && *UNESCAPE(p) != '.') /* leading period */
        RETURN(false);

      while(1) {
        switch(*p) {
        case '*':
          do { p++; } while (*p == '*');
          if(ISEND(UNESCAPE(p))) {
            p = UNESCAPE(p);
            RETURN(true);
          }
          if(ISEND(s)) RETURN(false);

          ptmp = p;
          stmp = s;
          continue;

        case '?':
          if(ISEND(s)) RETURN(false);
          p++;
          Inc(s);
          continue;

        case '[': {
          const char *t;
          if(ISEND(s)) RETURN(false);
          if((t = bracket(p + 1, s, flags)) != 0) {
            p = t;
            Inc(s);
            continue;
          }
          goto failed;
        }
        }

        /* ordinary */
        p = UNESCAPE(p);
        if(ISEND(s)) RETURN(ISEND(p) ? true : false);
        if(ISEND(p)) goto failed;
        if(Compare(p, s) != 0) goto failed;

        Inc(p);
        Inc(s);
        continue;

failed: /* try next '*' position */
        if (ptmp && stmp) {
          p = ptmp;
          Inc(stmp); /* !ISEND(*stmp) */
          s = stmp;
          continue;
        }
        RETURN(false);
      }
    }

    int mri_fnmatch(const char* p, const char* s, int flags) {
      const int period = !(flags & FNM_DOTMATCH);
      const int pathname = flags & FNM_PATHNAME;

      const char *ptmp = 0;
      const char *stmp = 0;

      if(pathname) {
        while(1) {
          if(p[0] == '*' && p[1] == '*' && p[2] == '/') {
            do { p += 3; } while (p[0] == '*' && p[1] == '*' && p[2] == '/');
            ptmp = p;
            stmp = s;
          }
          if(mri_fnmatch_helper(&p, &s, flags)) {
            while(*s && *s != '/') Inc(s);
            if(*p && *s) {
              p++;
              s++;
              continue;
            }
            if(!*p && !*s)
              return true;
          }
          /* failed : try next recursion */
          if(ptmp && stmp && !(period && *stmp == '.')) {
            while(*stmp && *stmp != '/') Inc(stmp);
            if(*stmp) {
              p = ptmp;
              stmp++;
              s = stmp;
              continue;
            }
          }
          return false;
        }
      } else {
        return mri_fnmatch_helper(&p, &s, flags);
      }
    }
  }

  Object* IO::fnmatch(STATE, String* pattern, String* path, Fixnum* flags) {
    if(mri_fnmatch(pattern->c_str(), path->c_str(), flags->to_native())) {
      return Qtrue;
    }

    return Qfalse;
  }


  /** Socket methods */
  Object* IO::accept(STATE, CallFrame* calling_environment) {
    WaitingForSignal waiter;
    int fd = descriptor()->to_native();
    int new_fd = 0;
    bool set = false;

    struct sockaddr_storage socka;
    socklen_t sock_len = sizeof(socka);

  retry:
    state->install_waiter(waiter);

    {
      GlobalLock::UnlockGuard lock(state->global_lock());
      new_fd = ::accept(fd, (struct sockaddr*)&socka, &sock_len);
      set = true;
    }

    state->clear_waiter();

    if(new_fd == -1) {
      if(errno == EAGAIN || errno == EINTR) {
        if(state->check_async(calling_environment)) goto retry;
      } else {
        Exception::errno_error(state, "read(2) failed");
      }

      return NULL;
    }

    if(!set) return Qnil;
    return Fixnum::from(new_fd);
  }

/* IOBuffer methods */
  IOBuffer* IOBuffer::create(STATE, size_t bytes) {
    IOBuffer* buf = state->new_object<IOBuffer>(G(iobuffer));
    buf->storage(state, ByteArray::create(state, bytes));
    buf->channel(state, Channel::create(state));
    buf->total(state, Fixnum::from(bytes));
    buf->used(state, Fixnum::from(0));
    buf->reset(state);
    buf->write_synced(state, Qtrue);

    return buf;
  }

  IOBuffer* IOBuffer::allocate(STATE) {
    return create(state);
  }

  Object* IOBuffer::unshift(STATE, String* str, Fixnum* start_pos) {
    write_synced(state, Qfalse);
    native_int start_pos_native = start_pos->to_native();
    native_int total_sz = str->size() - start_pos_native;
    native_int used_native = used_->to_native();
    native_int available_space = total_->to_native() - used_native;

    if(total_sz > available_space) {
      total_sz = available_space;
    }

    memcpy(storage_->raw_bytes() + used_native, str->byte_address() + start_pos_native, total_sz);
    used(state, Fixnum::from(used_native + total_sz));

    return Fixnum::from(total_sz);
  }

  Object* IOBuffer::fill(STATE, IO* io, CallFrame* calling_environment) {
    ssize_t bytes_read;
    WaitingForSignal waiter;

    IOBuffer* self = this;
    OnStack<1> os(state, self);

  retry:
    state->install_waiter(waiter);

    {
      GlobalLock::UnlockGuard lock(state->global_lock());
      bytes_read = read(io->descriptor()->to_native(),
                        self->at_unused(),
                        self->left());
    }

    state->clear_waiter();

    if(bytes_read == -1) {
      if(errno == EAGAIN || errno == EINTR) {
        if(state->check_async(calling_environment)) goto retry;
      } else {
        Exception::errno_error(state, "read(2) failed");
      }

      return NULL;
    }

    if(bytes_read > 0) {
      self->read_bytes(state, bytes_read);
    }

    return Fixnum::from(bytes_read);
  }

  void IOBuffer::reset(STATE) {
    used(state, Fixnum::from(0));
    start(state, Fixnum::from(0));
    eof(state, Qfalse);
  }

  void IOBuffer::read_bytes(STATE, size_t bytes) {
    used(state, Fixnum::from(used_->to_native() + bytes));
  }

  char* IOBuffer::byte_address() {
    return (char*)storage_->raw_bytes();
  }

  size_t IOBuffer::left() {
    return total_->to_native() - used_->to_native();
  }

  char* IOBuffer::at_unused() {
    char* start = (char*)storage_->raw_bytes();
    start += used_->to_native();
    return start;
  }
};

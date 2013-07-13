#include "builtin/atomic.hpp"
#include "builtin/array.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/channel.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/io.hpp"
#include "builtin/string.hpp"
#include "builtin/thread.hpp"
#include "capi/handle.hpp"
#include "configuration.hpp"
#include "objectmemory.hpp"
#include "object_utils.hpp"
#include "on_stack.hpp"
#include "ontology.hpp"
#include "util/spinlock.hpp"
#include "windows_compat.h"
#include "version.h"

#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#ifdef RBX_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>
#endif
#include <sys/types.h>
#include <sys/time.h>
#include <sys/types.h>
#ifdef __OpenBSD__
#include <sys/uio.h>
#endif

namespace rubinius {
  void IO::init(STATE) {
    GO(io).set(ontology::new_class(state, "IO", G(object)));
    G(io)->set_object_type(state, IOType);

    GO(iobuffer).set(ontology::new_class(state, "InternalBuffer",
          G(object), G(io)));
    G(iobuffer)->set_object_type(state, IOBufferType);

    AtomicReference* ref = AtomicReference::create(state, Fixnum::from(-1));
    G(io)->set_ivar(state, state->symbol("@max_open_fd"), ref);
  }

  IO* IO::create(STATE, int fd) {
    IO* io = state->new_object<IO>(G(io));
    io->descriptor(state, Fixnum::from(fd));

#ifdef RBX_WINDOWS
    // TODO: Windows
    int acc_mode = 0;
#else
    int acc_mode = fcntl(fd, F_GETFL);
#endif
    if(acc_mode < 0) {
      // Assume it's closed.
      if(errno == EBADF) {
        io->descriptor(state, Fixnum::from(-1));
      }
      io->mode(state, nil<Fixnum>());
    } else {
      io->mode(state, Fixnum::from(acc_mode));
    }

    io->ibuffer(state, IOBuffer::create(state));
    io->eof(state, cFalse);
    io->lineno(state, Fixnum::from(0));

    // Don't bother to add finalization for stdio
    if(fd >= 3) {
      state->memory()->needs_finalization(io, (FinalizerFunction)&IO::finalize);
    }

    return io;
  }

  IO* IO::allocate(STATE, Object* self) {
    IO* io = state->new_object<IO>(G(io));
    io->descriptor(state, nil<Fixnum>());
    io->mode(state, nil<Fixnum>());
    io->ibuffer(state, IOBuffer::create(state));
    io->eof(state, cFalse);
    io->lineno(state, Fixnum::from(0));

    // Ensure the instance's class is set (i.e. for subclasses of IO)
    io->klass(state, as<Class>(self));

    state->memory()->needs_finalization(io, (FinalizerFunction)&IO::finalize);

    return io;
  }

  Fixnum* IO::open(STATE, String* p, Fixnum* m, Fixnum* perm, CallFrame* calling_environment) {
    char* path = strdup(p->c_str_null_safe(state));
    int mode = m->to_int();
    int permissions = perm->to_int();
    int fd = -1;

    OnStack<1> os(state, p);

    {
      GCIndependent guard(state, calling_environment);
      fd = ::open(path, mode, permissions);
    }

    free(path);

    if(fd < 0) {
      Exception::errno_error(state, p->c_str_null_safe(state));
    }
    update_max_fd(state, fd);

    return Fixnum::from(fd);
  }

  void IO::update_max_fd(STATE, native_int new_fd) {
    AtomicReference* ref = as<AtomicReference>(G(io)->get_ivar(state, state->symbol("@max_open_fd")));
    Fixnum* old_fd = as<Fixnum>(ref->get(state));

    while(old_fd->to_native() < new_fd &&
          ref->compare_and_set(state, old_fd, Fixnum::from(new_fd)) == cFalse) {
      old_fd = as<Fixnum>(ref->get(state));
    }
  }

  namespace {
    /** Utility function used by IO::select, returns highest descriptor. */
    static inline native_int fd_set_from_array(State* state,
                               Object* maybe_descriptors, fd_set* set)
    {
      if(NULL == set) {
        return 0;
      }

      Array* descriptors = as<Array>(maybe_descriptors);

      FD_ZERO(set);
      native_int highest = -1;

      for(native_int i = 0; i < descriptors->size(); ++i) {
        Object* elem = descriptors->get(state, i);
        IO* io;

        if(Array* ary = try_as<Array>(elem)) {
          io = as<IO>(ary->get(state, 1));
        } else {
          io = as<IO>(elem);
        }

        native_int descriptor = io->to_fd();

        // 1024 is selec't limit. If we try to set a value higher, it corrupts
        // memory. YAY FD_SET!
        if(descriptor >= FD_SETSIZE) return -2;
        highest = descriptor > highest ? descriptor : highest;

        if(descriptor >= 0) FD_SET((int_fd_t)descriptor, set);
      }

      return highest;
    }

    /** Utility function used by IO::select, returns Array of IOs that were set. */
    static inline Array* reject_unset_fds(State* state,
                           Object* maybe_originals, fd_set* set)
    {
      if(NULL == set) return Array::create(state, 0);

      Array* originals = as<Array>(maybe_originals);

      // A single value is the most common, so prime for that.
      Array* selected = Array::create(state, 1);

      for(native_int i = 0; i < originals->size(); ++i) {
        Object* elem = originals->get(state, i);
        Object* key;
        IO* io;

        if(Array* ary = try_as<Array>(elem)) {
          key = ary->get(state, 0);
          io = as<IO>(ary->get(state, 1));
        } else {
          key = elem;
          io = as<IO>(elem);
        }

        int fd = io->to_fd();
        if(fd < 0 || FD_ISSET(fd, set)) {
          selected->append(state, key);
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
    highest = fd_set_from_array(state, readables, maybe_read_set);
    if(highest == -2) return Primitives::failure();

    candidate = fd_set_from_array(state, writables, maybe_write_set);
    if(candidate == -2) return Primitives::failure();
    highest = candidate > highest ? candidate : highest;

    candidate = fd_set_from_array(state, errorables, maybe_error_set);
    if(candidate == -2) return Primitives::failure();
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
      gettimeofday(&future, NULL);
      timeradd(&future, &limit, &future);
    }

    native_int events;


    /* And the main event, pun intended */
    retry:
    state->vm()->interrupt_with_signal();
    state->vm()->thread->sleep(state, cTrue);

    {
      GCIndependent guard(state, calling_environment);
      events = ::select((highest + 1), maybe_read_set,
                                                  maybe_write_set,
                                                  maybe_error_set,
                                                  maybe_limit);
    }

    state->vm()->thread->sleep(state, cFalse);
    state->vm()->clear_waiter();

    if(events == -1) {
      if(errno == EAGAIN || errno == EINTR) {
        if(!state->check_async(calling_environment)) return NULL;

        // Recalculate the limit and go again.
        if(maybe_limit) {
          struct timeval now;
          gettimeofday(&now, NULL);
          timersub(&future, &now, &limit);
        }

        goto retry;
      }

      Exception::errno_error(state, "select(2) failed");
      return NULL;
    }

    /* Timeout expired */
    if(events == 0) return cNil;

    /* Build the results. */
    Array* output = Array::create(state, 3);

    /* These handle NULL sets. */
    output->set(state, 0, reject_unset_fds(state, readables, maybe_read_set));
    output->set(state, 1, reject_unset_fds(state, writables, maybe_write_set));
    output->set(state, 2, reject_unset_fds(state, errorables, maybe_error_set));

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

    return cTrue;
  }

  Object* IO::reopen_path(STATE, String* p, Fixnum* m, CallFrame* calling_environment) {
    native_int cur_fd   = to_fd();

    char* path = strdup(p->c_str_null_safe(state));
    int mode = m->to_int();
    int other_fd = -1;

    IO* self = this;
    OnStack<2> os(state, self, p);

    {
      GCIndependent guard(state, calling_environment);
      other_fd = ::open(path, mode, 0666);
    }

    free(path);

    if(other_fd < 0) {
      Exception::errno_error(state, p->c_str_null_safe(state));
    }

    update_max_fd(state, other_fd);

    if(dup2(other_fd, cur_fd) == -1) {
      if(errno == EBADF) { // this means cur_fd is closed
        // Just set ourselves to use the new fd and go on with life.
        descriptor(state, Fixnum::from(other_fd));
      } else {
        Exception::errno_error(state, p->c_str_null_safe(state));
        return NULL;
      }
    }

    self->set_mode(state);
    if(IOBuffer* ibuf = try_as<IOBuffer>(self->ibuffer())) {
      ibuf->reset(state);
    }

    return cTrue;
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

    return cNil;
  }

  Object* IO::connect_pipe(STATE, IO* lhs, IO* rhs) {
    int fds[2];
    if(pipe(fds) == -1) {
      Exception::errno_error(state, "creating pipe");
    }

    update_max_fd(state, fds[0]);
    update_max_fd(state, fds[1]);

    lhs->descriptor(state, Fixnum::from(fds[0]));
    rhs->descriptor(state, Fixnum::from(fds[1]));

    lhs->mode(state, Fixnum::from(O_RDONLY));
    rhs->mode(state, Fixnum::from(O_WRONLY));
    return cTrue;
  }

  Integer* IO::seek(STATE, Integer* amount, Fixnum* whence) {
    ensure_open(state);

    if(Bignum* big = try_as<Bignum>(amount)) {
      if((size_t)mp_count_bits(big->mp_val()) > (sizeof(off_t) * 8)) {
        return static_cast<Integer*>(Primitives::failure());
      }
    }

    off_t offset = amount->to_long_long();
    off_t position = lseek(to_fd(), offset, whence->to_native());

    if(position == -1) {
      Exception::errno_error(state);
    }

    return Integer::from(state, position);
  }

  Integer* IO::ftruncate(STATE, Fixnum* off) {
    ensure_open(state);

    if(Bignum* big = try_as<Bignum>(off)) {
      if((size_t)mp_count_bits(big->mp_val()) > (sizeof(off_t) * 8)) {
        return static_cast<Integer*>(Primitives::failure());
      }
    }

    off_t offset = off->to_long_long();

    int status = ::ftruncate(to_fd(), offset);
    if(status == -1) {
      Exception::errno_error(state);
    }

    return Integer::from(state, status);
  }

  Integer* IO::truncate(STATE, String* name, Fixnum* off) {

    if(Bignum* big = try_as<Bignum>(off)) {
      if((size_t)mp_count_bits(big->mp_val()) > (sizeof(off_t) * 8)) {
        return static_cast<Integer*>(Primitives::failure());
      }
    }

    off_t offset = off->to_long_long();

    int status = ::truncate(name->c_str_null_safe(state), offset);
    if(status == -1) {
      Exception::errno_error(state);
    }

    return Integer::from(state, status);
  }

  /** This is NOT the same as shutdown(). */
  Object* IO::close(STATE) {
    ensure_open(state);

    /** @todo   Should this be just int? --rue */
    native_int desc = to_fd();

    // Already closed, ignore.
    if(desc == -1) {
      return cNil;
    }

    // Invalid descriptor no matter what.
    descriptor(state, Fixnum::from(-1));

    // If there is a handle for this IO, and it's been promoted into
    // a lowlevel RIO struct using fdopen, then we MUST use fclose
    // to close it.

    if(capi::Handle* hdl = handle(state)) {
      if(hdl->is_rio()) {
        if(!hdl->rio_close()) {
          Exception::errno_error(state);
        }
        return cNil;
      }
    }

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

    return cNil;
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
#ifdef F_GETFL
    int acc_mode = fcntl(to_fd(), F_GETFL);
    if(acc_mode < 0) {
      Exception::errno_error(state);
    }
#else
    int acc_mode = 0;
#endif
    mode(state, Fixnum::from(acc_mode));
  }

  void IO::force_read_only(STATE) {
    int m = mode_->to_native();
    mode(state, Fixnum::from((m & ~O_ACCMODE) | O_RDONLY));
  }

  void IO::force_write_only(STATE) {
    int m = mode_->to_native();
    mode(state, Fixnum::from((m & ~O_ACCMODE) | O_WRONLY));
  }

  void IO::finalize(STATE, IO* io) {
    if(io->descriptor()->nil_p()) return;

    native_int fd = io->descriptor()->to_native();

    // Flush the buffer to disk if it's not write sync'd
    if(IOBuffer* buf = try_as<IOBuffer>(io->ibuffer())) {
      if(!CBOOL(buf->write_synced())) {
        native_int start = buf->start()->to_native();
        native_int used = buf->used()->to_native();
        native_int bytes = used - start;

        if(bytes > 0 && start < buf->storage()->size()) {
          fd_set fds;
          FD_ZERO(&fds);
          struct timeval tv = {0,0};

          FD_SET(fd, &fds);

          // We use select(2) to prevent from blocking while
          // trying to flush out the data.

          uint8_t* data = buf->storage()->raw_bytes() + start;
          while(bytes > 0 && ::select(fd+1, 0, &fds, 0, &tv) > 0) {
            ssize_t wrote = ::write(fd, data, bytes);
            // If we couldn't write, then just bail.
            if(wrote == -1) break;
            data += wrote;
            bytes -= wrote;
          }
        }
      }
    }

    // don't close stdin, stdout, stderr (0, 1, 2)
    if(fd > STDERR_FILENO) {
      io->descriptor(state, Fixnum::from(-1));

      // If there is a handle for this IO, and it's been promoted into
      // a lowlevel RIO struct using fdopen, then we MUST use fclose
      // to close it.

      if(capi::Handle* hdl = io->handle(state)) {
        if(hdl->is_rio()) {
          hdl->rio_close();
          return;
        }
      }

      if(LANGUAGE_18_ENABLED) {
        ::close(fd);
      } else if(io->autoclose_ != cFalse) {
        ::close(fd);
      }
    }
  }

#define STACK_BUF_SZ 8096

  Object* IO::sysread(STATE, Fixnum* number_of_bytes,
                             CallFrame* calling_environment)
  {
    char stack_buf[STACK_BUF_SZ];
    char* malloc_buf = 0;

    char* buf = stack_buf;

    size_t count = number_of_bytes->to_ulong();

    if(count > STACK_BUF_SZ) {
      malloc_buf = (char*)malloc(count);
      buf = malloc_buf;
    }

    ssize_t bytes_read;
    native_int fd = descriptor()->to_native();

  retry:
    state->vm()->interrupt_with_signal();
    state->vm()->thread->sleep(state, cTrue);

    {
      GCIndependent guard(state, calling_environment);
      bytes_read = ::read(fd, buf, count);
    }

    state->vm()->thread->sleep(state, cFalse);
    state->vm()->clear_waiter();

    if(bytes_read == -1) {
      if(errno == EINTR) {
        if(state->check_async(calling_environment)) goto retry;

      } else {
        Exception::errno_error(state, "read(2) failed");
      }

      if(malloc_buf) free(malloc_buf);
      return NULL;
    }

    if(bytes_read == 0) {
      if(malloc_buf) free(malloc_buf);
      return cNil;
    }

    String* str = String::create(state, buf, bytes_read);
    if(malloc_buf) free(malloc_buf);

    return str;
  }

  Object* IO::read_if_available(STATE, Fixnum* number_of_bytes) {

    std::size_t count = number_of_bytes->to_ulong();
    if(count == 0) return String::create(state, Fixnum::from(0));

    fd_set set;
    FD_ZERO(&set);

    native_int fd = descriptor()->to_native();
    FD_SET((int_fd_t)fd, &set);

    struct timeval tv = {0,0};
    int res = ::select(fd+1, &set, 0, 0, &tv);

    if(res == 0) {
      Exception::errno_eagain_error(state, "no data ready");
      return 0;
    } else if(res <= 0) {
      Exception::errno_error(state, "read(2) failed");
      return 0;
    }

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

    if(bytes_read == 0) return cNil;

    buffer->num_bytes(state, Fixnum::from(bytes_read));
    return buffer;
  }

  Object* IO::write(STATE, String* buf, CallFrame* call_frame) {
    native_int buf_size = buf->byte_size();
    native_int data_size = as<ByteArray>(buf->data())->size();
    native_int left = buf_size;
    if(unlikely(left > data_size)) {
      left = data_size;
    }
    uint8_t* bytes = new uint8_t[left];
    memcpy(bytes, buf->byte_address(), left);
    int fd = this->to_fd();
    bool error = false;

    {
      GCIndependent guard(state, call_frame);
      uint8_t* cur = bytes;
      while(left > 0) {
        ssize_t cnt = ::write(fd, cur, left);

        if(cnt == -1) {
          switch(errno) {
          case EINTR:
          case EAGAIN: {
            // Pause before continuing
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET((int_fd_t)fd, &fds);

            ::select(fd+1, NULL, &fds, NULL, NULL);

            continue;
          }
          case EPIPE:
            if(fd == STDOUT_FILENO || fd == STDERR_FILENO) {
              left = 0;
              delete[] bytes;
              goto done;
            }
            // fall through
          default:
            error = true;
            break;
          }
        }

        if(error) break;

        left -= cnt;
        cur  += cnt;
      }
    }

    delete[] bytes;

    if(error) {
      Exception::errno_error(state);
      return NULL;
    }

  done:
    return Integer::from(state, buf_size - left);
  }

  Object* IO::write_nonblock(STATE, String* buf) {
    set_nonblock(state);

    native_int buf_size = buf->byte_size();
    native_int data_size = as<ByteArray>(buf->data())->size();
    if(unlikely(buf_size > data_size)) {
      buf_size = data_size;
    }

    // We can use byte_address() here since we use an explicit size
    int n = ::write(descriptor_->to_native(), buf->byte_address(), buf_size);
    if(n == -1) Exception::errno_error(state, "write_nonblock");
    return Fixnum::from(n);
  }

  Object* IO::advise(STATE, Symbol* advice_name, Integer* offset, Integer* len) {
#ifdef HAVE_POSIX_FADVISE
    int advice = 0;

    if(advice_name == state->symbol("normal")) {
      advice = POSIX_FADV_NORMAL;
    } else if(advice_name == state->symbol("sequential")) {
      advice = POSIX_FADV_SEQUENTIAL;
    } else if(advice_name == state->symbol("random")) {
      advice = POSIX_FADV_RANDOM;
    } else if(advice_name == state->symbol("willneed")) {
      advice = POSIX_FADV_WILLNEED;
    } else if(advice_name == state->symbol("dontneed")) {
      advice = POSIX_FADV_DONTNEED;
    } else if(advice_name == state->symbol("noreuse")) {
      advice = POSIX_FADV_NOREUSE;
    } else {
      return Primitives::failure();
    }

    int erno = posix_fadvise(to_fd(), offset->to_long_long(), len->to_long_long(), advice);

    if(erno) {
      Exception::errno_error(state, "posfix_fadvise(2) failed", erno);
    }

#endif

    return cNil;
  }

  Array* ipaddr(STATE, struct sockaddr* addr, socklen_t len) {
    String* family;
    char buf[NI_MAXHOST];
    char pbuf[NI_MAXSERV];

    switch(addr->sa_family) {
    case AF_UNSPEC:
      family = String::create(state, "AF_UNSPEC");
      break;
    case AF_INET:
      family = String::create(state, "AF_INET");
      break;
#ifdef INET6
    case AF_INET6:
      family = String::create(state, "AF_INET6");
      break;
#endif
#ifdef AF_LOCAL
    case AF_LOCAL:
      family = String::create(state, "AF_LOCAL");
      break;
#elif  AF_UNIX
    case AF_UNIX:
      family = String::create(state, "AF_UNIX");
      break;
#endif
    default:
      snprintf(pbuf, NI_MAXSERV, "unknown:%d", addr->sa_family);
      family = String::create(state, pbuf);
      break;
    }

    int e = getnameinfo(addr, len, buf, NI_MAXHOST, pbuf, NI_MAXSERV,
                        NI_NUMERICHOST | NI_NUMERICSERV);

    // TODO this doesn't support doing the DNS bound lookup at all.
    //      Not doing it better than doing it badly, thats why it's
    //      not here.
    //
    String* host;
    if(e) {
      host = String::create(state, (char*)addr, len);
    } else {
      host = String::create(state, buf);
    }

    Array* ary = Array::create(state, 4);
    ary->set(state, 0, family);
    ary->set(state, 1, Fixnum::from(atoi(pbuf)));
    ary->set(state, 2, host);
    ary->set(state, 3, host);

    return ary;
  }

#ifndef RBX_WINDOWS
  static const char* unixpath(struct sockaddr_un *sockaddr, socklen_t len) {
    if(sockaddr->sun_path < (char*)sockaddr + len) {
      return sockaddr->sun_path;
    }
    return "";
  }

  Array* unixaddr(STATE, struct sockaddr_un* addr, socklen_t len) {
    Array* ary = Array::create(state, 2);
    ary->set(state, 0, String::create(state, "AF_UNIX"));
    ary->set(state, 1, String::create(state, unixpath(addr, len)));
    return ary;
  }
#endif

  Object* IO::socket_read(STATE, Fixnum* bytes, Fixnum* flags, Fixnum* type,
                          CallFrame* calling_environment) {
    char buf[1024];
    socklen_t alen = sizeof(buf);
    size_t size = (size_t)bytes->to_native();

    String* buffer = String::create_pinned(state, bytes);

    OnStack<1> variables(state, buffer);

    ssize_t bytes_read;
    native_int t = type->to_native();

  retry:
    state->vm()->interrupt_with_signal();
    state->vm()->thread->sleep(state, cTrue);

    {
      GCIndependent guard(state, calling_environment);
      bytes_read = recvfrom(descriptor()->to_native(),
                            (char*)buffer->byte_address(), size,
                            flags->to_native(),
                            (struct sockaddr*)buf, &alen);
    }

    state->vm()->thread->sleep(state, cFalse);
    state->vm()->clear_waiter();

    buffer->unpin();

    if(bytes_read == -1) {
      if(errno == EINTR) {
        if(state->check_async(calling_environment)) goto retry;
      } else {
        Exception::errno_error(state, "read(2) failed");
      }

      return NULL;
    }

    buffer->num_bytes(state, Fixnum::from(bytes_read));

    if(t == 0) return buffer; // none

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, buffer);

    switch(type->to_native()) {
    case 1: // ip
      // Hack from MRI:
      // OSX doesn't return a 'from' result from recvfrom for connection-oriented sockets
      if(alen && alen != sizeof(buf)) {
        ary->set(state, 1, ipaddr(state, (struct sockaddr*)buf, alen));
      } else {
        ary->set(state, 1, cNil);
      }
      break;
#ifndef RBX_WINDOWS
    case 2: // unix
      ary->set(state, 1, unixaddr(state, (struct sockaddr_un*)buf, alen));
      break;
#endif
    default:
      ary->set(state, 1, String::create(state, buf, alen));
    }

    return ary;
  }

  static int ttyname_lock = RBX_SPINLOCK_UNLOCKED;
  Object* IO::query(STATE, Symbol* op) {
    ensure_open(state);

    native_int fd = to_fd();

    if(op == state->symbol("tty?")) {
      return RBOOL(isatty(fd));
#ifndef RBX_WINDOWS
    } else if(op == state->symbol("ttyname")) {
      rbx_spinlock_lock(&ttyname_lock);
      char* name = ttyname(fd);
      if(name) {
        String* res = String::create(state, name);
        rbx_spinlock_unlock(&ttyname_lock);
        return res;
      } else {
        rbx_spinlock_unlock(&ttyname_lock);
        Exception::errno_error(state, "ttyname(3) failed");
        return NULL;
      }
#endif
    }

    return cNil;
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

      if(period && *s == '.' && *UNESCAPE(p) != '.') /* leading period */
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
        if(ptmp && stmp) {
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
    return RBOOL(mri_fnmatch(pattern->c_str(state), path->c_str(state), flags->to_native()));
  }


  /** Socket methods */
  Object* IO::accept(STATE, CallFrame* calling_environment) {
    int fd = descriptor()->to_native();
    int new_fd = 0;
    bool set = false;

    struct sockaddr_storage socka;
    socklen_t sock_len = sizeof(socka);

  retry:
    state->vm()->interrupt_with_signal();
    state->vm()->thread->sleep(state, cTrue);

    {
      GCIndependent guard(state, calling_environment);
      new_fd = ::accept(fd, (struct sockaddr*)&socka, &sock_len);
      set = true;
    }

    state->vm()->thread->sleep(state, cFalse);
    state->vm()->clear_waiter();

    if(new_fd == -1) {
      if(errno == EAGAIN || errno == EINTR) {
        if(state->check_async(calling_environment)) goto retry;
      } else {
        Exception::errno_error(state, "read(2) failed");
      }

      return NULL;
    }

    if(!set) return cNil;
    return Fixnum::from(new_fd);
  }

  static const int cmsg_space = CMSG_SPACE(sizeof(int));

  Object* IO::send_io(STATE, IO* io) {
#ifdef _WIN32
    return Primitives::failure();
#else
    int fd;
    struct msghdr msg;
    struct iovec vec[1];
    char buf[1];

    struct cmsghdr *cmsg;
    char cmsg_buf[cmsg_space];

    fd = io->descriptor()->to_native();

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    /* Linux and Solaris doesn't work if msg_iov is NULL. */
    buf[0] = '\0';
    vec[0].iov_base = buf;
    vec[0].iov_len = 1;
    msg.msg_iov = vec;
    msg.msg_iovlen = 1;

    msg.msg_control = (caddr_t)cmsg_buf;
    msg.msg_controllen = sizeof(cmsg_buf);
    msg.msg_flags = 0;
    cmsg = CMSG_FIRSTHDR(&msg);
    memset(cmsg_buf, 0, sizeof(cmsg_buf));
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;

    // Workaround for GCC's broken strict-aliasing checks.
    int* fd_data = (int*)CMSG_DATA(cmsg);
    *fd_data = fd;

    if(sendmsg(descriptor()->to_native(), &msg, 0) == -1) {
      return Primitives::failure();
    }

    return cNil;
#endif
  }

  Object* IO::recv_fd(STATE, CallFrame* calling_environment) {
#ifdef _WIN32
    return Primitives::failure();
#else
    struct msghdr msg;
    struct iovec vec[1];
    char buf[1];

    struct cmsghdr *cmsg;
    char cmsg_buf[cmsg_space];

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    /* Linux and Solaris doesn't work if msg_iov is NULL. */
    buf[0] = '\0';
    vec[0].iov_base = buf;
    vec[0].iov_len = 1;
    msg.msg_iov = vec;
    msg.msg_iovlen = 1;

    msg.msg_control = (caddr_t)cmsg_buf;
    msg.msg_controllen = sizeof(cmsg_buf);
    msg.msg_flags = 0;
    cmsg = CMSG_FIRSTHDR(&msg);
    memset(cmsg_buf, 0, sizeof(cmsg_buf));
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;

    // Workaround for GCC's broken strict-aliasing checks.
    int* fd_data = (int *)CMSG_DATA(cmsg);
    *fd_data = -1;

    int read_fd = descriptor()->to_native();

    int code = -1;

    retry:
    state->vm()->interrupt_with_signal();
    state->vm()->thread->sleep(state, cTrue);

    {
      GCIndependent guard(state, calling_environment);
      code = recvmsg(read_fd, &msg, 0);
    }

    state->vm()->thread->sleep(state, cFalse);
    state->vm()->clear_waiter();

    if(code == -1) {
      if(errno == EAGAIN || errno == EINTR) {
        if(!state->check_async(calling_environment)) return NULL;
        goto retry;
      }

      return Primitives::failure();
    }

    if(msg.msg_controllen != CMSG_SPACE(sizeof(int))
        || cmsg->cmsg_len != CMSG_LEN(sizeof(int))
        || cmsg->cmsg_level != SOL_SOCKET
        || cmsg->cmsg_type != SCM_RIGHTS) {
      return Primitives::failure();
    }

    // Workaround for GCC's broken strict-aliasing checks.
    fd_data = (int *)CMSG_DATA(cmsg);
    return Fixnum::from(*fd_data);
#endif
  }

  void IO::set_nonblock(STATE) {
#ifdef F_GETFL
    int flags = fcntl(descriptor_->to_native(), F_GETFL);
    if(flags == -1) return;
#else
    int flags = 0;
#endif

    if((flags & O_NONBLOCK) == 0) {
      flags |= O_NONBLOCK;
      fcntl(descriptor_->to_native(), F_SETFL, flags);
    }
  }

/* IOBuffer methods */
  IOBuffer* IOBuffer::create(STATE, size_t bytes) {
    IOBuffer* buf = state->new_object<IOBuffer>(G(iobuffer));
    buf->storage(state, ByteArray::create(state, bytes));
    buf->total(state, Fixnum::from(bytes));
    buf->used(state, Fixnum::from(0));
    buf->reset(state);
    buf->write_synced(state, cTrue);

    return buf;
  }

  IOBuffer* IOBuffer::allocate(STATE) {
    return create(state);
  }

  Object* IOBuffer::unshift(STATE, String* str, Fixnum* start_pos) {
    write_synced(state, cFalse);
    native_int start_pos_native = start_pos->to_native();
    native_int str_size = str->byte_size();
    native_int data_size = as<ByteArray>(str->data())->size();
    if(unlikely(str_size > data_size)) {
      str_size = data_size;
    }
    native_int total_sz = str_size - start_pos_native;
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
    native_int fd = io->descriptor()->to_native();

    IOBuffer* self = this;
    OnStack<1> os(state, self);

    char temp_buffer[STACK_BUF_SZ];
    size_t count = STACK_BUF_SZ;

    if(self->left() < count) count = self->left();

  retry:
    state->vm()->interrupt_with_signal();
    state->vm()->thread->sleep(state, cTrue);

    {
      GCIndependent guard(state, calling_environment);
      bytes_read = read(fd, temp_buffer, count);
    }

    state->vm()->thread->sleep(state, cFalse);
    state->vm()->clear_waiter();

    if(bytes_read == -1) {
      switch(errno) {
      case ECONNRESET:
      case ETIMEDOUT:
        // Treat as seeing eof
        bytes_read = 0;
        break;
      case EAGAIN:
      case EINTR:
        if(state->check_async(calling_environment)) goto retry;
        return NULL;
      default:
        Exception::errno_error(state, "read(2) failed");
        return NULL;
      }
    }

    if(bytes_read > 0) {
      // Detect if another thread has updated the buffer
      // and now there isn't enough room for this data.
      if(bytes_read > (ssize_t)self->left()) {
        Exception::internal_error(state, calling_environment, "IO buffer overrun");
        return NULL;
      }
      memcpy(self->at_unused(), temp_buffer, bytes_read);
      self->read_bytes(state, bytes_read);
    }

    return Fixnum::from(bytes_read);
  }

  void IOBuffer::reset(STATE) {
    used(state, Fixnum::from(0));
    start(state, Fixnum::from(0));
    eof(state, cFalse);
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

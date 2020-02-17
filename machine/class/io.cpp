#include "configuration.hpp"
#include "memory.hpp"
#include "object_utils.hpp"
#include "on_stack.hpp"
#include "primitives.hpp"
#include "thread_phase.hpp"
#include "windows_compat.h"

#include "class/atomic.hpp"
#include "class/array.hpp"
#include "class/byte_array.hpp"
#include "class/channel.hpp"
#include "class/class.hpp"
#include "class/exception.hpp"
#include "class/ffi_pointer.hpp"
#include "class/fixnum.hpp"
#include "class/io.hpp"
#include "class/native_method.hpp"
#include "class/string.hpp"
#include "class/thread.hpp"

#include "call_frame.hpp"
#include "configuration.hpp"
#include "memory.hpp"
#include "object_utils.hpp"
#include "on_stack.hpp"
#include "ontology.hpp"

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

  void IO::bootstrap(STATE) {
    GO(io).set(state->memory()->new_class<Class, IO>(state, G(object), "IO"));
    G(io)->set_object_type(state, IOType);
  }

  IO* IO::create(STATE, int fd) {
    IO* io = state->memory()->new_object<IO>(state, G(io));

    return io;
  }

  intptr_t IO::open_with_cloexec(STATE, const char* path, int mode, int permissions) {
    if(Class* fd_class = try_as<Class>(G(io)->get_const(state, "FileDescriptor"))) {
      Tuple* args = Tuple::from(state, 3,
          String::create(state, path),
          Fixnum::from(mode),
          Fixnum::from(permissions));

      if(Fixnum* fd = try_as<Fixnum>(fd_class->send(state,
              state->symbol("open_with_cloexec"),
              Array::from_tuple(state, args))))
      {
        return fd->to_native();
      } else {
        Exception::raise_runtime_error(state, "unable to open IO with cloexec");
      }
    } else {
      Exception::raise_runtime_error(state, "unable to access IO::FileDescriptor class");
    }
  }

  intptr_t IO::descriptor(STATE) {
    if(Fixnum* fd = try_as<Fixnum>(send(state, state->symbol("descriptor")))) {
      return fd->to_native();
    }

    Exception::raise_runtime_error(state, "IO descriptor is not a Fixnum");
  }

  void IO::ensure_open(STATE) {
    // Will raise an exception if the file descriptor is not open
    send(state, state->symbol("ensure_open"));
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

  Object* IO::socket_read(STATE, Fixnum* bytes, Fixnum* flags, Fixnum* type) {
    char buf[1024];
    socklen_t alen = sizeof(buf);
    size_t size = (size_t)bytes->to_native();

    String* buffer = String::create_pinned(state, bytes);

    OnStack<1> variables(state, buffer);

    ssize_t bytes_read;
    intptr_t t = type->to_native();

  retry:
    state->interrupt_with_signal();
    state->set_thread_sleep();

    {
      UnmanagedPhase unmanaged(state);

      bytes_read = recvfrom(descriptor(state),
                            (char*)buffer->byte_address(), size,
                            flags->to_native(),
                            (struct sockaddr*)buf, &alen);
    }

    state->set_thread_run();
    state->clear_waiter();

    if(bytes_read == -1) {
      if(errno == EINTR) {
        if(state->thread_interrupted_p()) return nullptr;
        ensure_open(state);
        goto retry;
      } else {
        Exception::raise_errno_error(state, "read(2) failed");
      }

      return nullptr;
    }

    buffer->num_bytes(state, Fixnum::from(bytes_read));

    if(t == 0) return buffer; // none

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, buffer);

    switch(type->to_native()) {
    case 1: // ip
      // Hack from MRI:
      // macOS doesn't return a 'from' result from recvfrom for connection-oriented sockets
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
#define FNM_EXTGLOB 	0x10
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
        if(!*t1) return nullptr;

        p = Next(t1);
        if(p[0] == '-' && p[1] != ']') {
          const char *t2 = p + 1;
          if(escape && *t2 == '\\') t2++;
          if(!*t2) return nullptr;

          p = Next(t2);
          if(!ok && Compare(t1, s) <= 0 && Compare(s, t2) <= 0) ok = 1;
        } else {
          if(!ok && Compare(t1, s) == 0) ok = 1;
        }
      }

      return ok == nope ? nullptr : (char *)p + 1;
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

    fd = io->descriptor(state);

    msg.msg_name = nullptr;
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

    if(sendmsg(descriptor(state), &msg, 0) == -1) {
      return Primitives::failure();
    }

    return cNil;
#endif
  }

  Object* IO::recv_fd(STATE) {
#ifdef _WIN32
    return Primitives::failure();
#else
    struct msghdr msg;
    struct iovec vec[1];
    char buf[1];

    struct cmsghdr *cmsg;
    char cmsg_buf[cmsg_space];

    msg.msg_name = nullptr;
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

    int read_fd = descriptor(state);

    int code = -1;

  retry:
    state->interrupt_with_signal();
    state->set_thread_sleep();

    {
      UnmanagedPhase unmanaged(state);
      code = recvmsg(read_fd, &msg, 0);
    }

    state->set_thread_run();
    state->clear_waiter();

    if(code == -1) {
      if(errno == EAGAIN || errno == EINTR) {
        if(state->thread_interrupted_p()) return nullptr;
        ensure_open(state);
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

  void FDSet::bootstrap(STATE) {
    // Create a constant for FDSet under the IO::Select namespace, i.e. IO::Select::FDSet
    GO(select).set(state->memory()->new_class<Class>(state, G(io), "Select"));
    GO(fdset).set(state->memory()->new_class<Class, FDSet>(state, G(select), "FDSet"));
  }

  FDSet* FDSet::allocate(STATE, Object* self) {
    FDSet* fdset = create(state);
    fdset->klass(state, as<Class>(self));
    return fdset;
  }

  FDSet* FDSet::create(STATE) {
    FDSet* fdset = state->memory()->new_object<FDSet>(state, G(fdset));
    return fdset;
  }

  Object* FDSet::zero(STATE) {
    FD_ZERO((fd_set*)descriptor_set);
    return cTrue;
  }

  Object* FDSet::set(STATE, Fixnum* descriptor) {
    intptr_t fd = descriptor->to_native();

    FD_SET((int_fd_t)fd, (fd_set*)descriptor_set);

    return cTrue;
  }

  Object* FDSet::is_set(STATE, Fixnum* descriptor) {
    intptr_t fd = descriptor->to_native();

    if (FD_ISSET(fd, (fd_set*)descriptor_set)) {
      return cTrue;
    }
    else {
      return cFalse;
    }
  }

  Object* FDSet::to_set(STATE) {
    void *ptr = (void*)&descriptor_set;

    return Pointer::create(state, ptr);
  }

  void RIOStream::bootstrap(STATE) {
    GO(rio_stream).set(state->memory()->new_class<Class, RIOStream>(
          state, G(rubinius), "RIOStream"));
  }

  Object* RIOStream::close(STATE, Object* io, Object* allow_exception) {
    /* If there is a handle for this IO, and it's been promoted into a
     * lowlevel RIO struct using fdopen, then we MUST use fclose to close it.
     */

    if(MemoryHandle* handle = io->get_handle(state)) {
      if(handle->rio_p()) {
        if(!handle->rio_close(state) && CBOOL(allow_exception)) {
          Exception::raise_errno_error(state, "failed to close RIOStream");
        }
        return cTrue;
      }
    }

    return cFalse;
  }
}


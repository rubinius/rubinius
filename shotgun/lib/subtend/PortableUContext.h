
#ifndef PORTABLEUCONTEXT_DEFINED 
#define PORTABLEUCONTEXT_DEFINED 1

#if (defined(__APPLE__) && __DARWIN_UNIX03) || defined(linux) || defined(__NetBSD__) || defined(__FreeBSD__) || (defined(__SVR4) && defined (__sun))
#define HAS_UCONTEXT 1
#endif

#if defined(__FreeBSD__) || (defined(__APPLE__) && !__DARWIN_UNIX03)

#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <signal.h>
#include <sys/utsname.h>
#include <inttypes.h>
#include <ucontext.h>
typedef unsigned long ulong;

#if defined(__FreeBSD__) && __FreeBSD__ < 5
extern	int		getmcontext(mcontext_t*);
extern	void		setmcontext(mcontext_t*);
#define	setcontext(u)	setmcontext(&(u)->uc_mcontext)
#define	getcontext(u)	getmcontext(&(u)->uc_mcontext)
extern	int		swapcontext(ucontext_t*, ucontext_t*);
extern	void		makecontext(ucontext_t*, void(*)(), int, ...);
#endif

#if defined(__APPLE__) && (__DARWIN_UNIX03 == 0)
#	define mcontext libthread_mcontext
#	define mcontext_t libthread_mcontext_t
#	define ucontext libthread_ucontext
#	define ucontext_t libthread_ucontext_t
#	if defined(__i386__)
#		include "PortableUContext386.h"
#   define NEEDX86MAKECONTEXT
#	elif defined(__ppc__)
#		include "PortableUContextPPC.h"
#   define NEEDPOWERMAKECONTEXT
# else
#   error Unsupported Apple platform
#	endif	
#   define NEEDSWAPCONTEXT
#endif

#if defined(__OpenBSD__)
#	define mcontext libthread_mcontext
#	define mcontext_t libthread_mcontext_t
#	define ucontext libthread_ucontext
#	define ucontext_t libthread_ucontext_t
#	if defined __i386__
#		include "PortableUContext386.h"
#	else
#		include "PortableUContextPPC.h"
#	endif
extern pid_t rfork_thread(int, void*, int(*)(void*), void*);
#endif

#if 0 &&  defined(__sun__)
#	define mcontext libthread_mcontext
#	define mcontext_t libthread_mcontext_t
#	define ucontext libthread_ucontext
#	define ucontext_t libthread_ucontext_t
#	include "sparc-ucontext.h"
#endif

#if defined(__arm__)
int getmcontext(mcontext_t*);
void setmcontext(const mcontext_t*);
#define	setcontext(u)	setmcontext(&(u)->uc_mcontext)
#define	getcontext(u)	getmcontext(&(u)->uc_mcontext)
#endif

// --------------------------

#if defined(__FreeBSD__) && defined(__i386__) && __FreeBSD__ < 5
#define NEEDX86MAKECONTEXT
#define NEEDSWAPCONTEXT
#endif


#endif
#endif


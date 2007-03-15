#include <setjmp.h>

/* See http://lua-users.org/lists/lua-l/2004-10/msg00217.html
   It details how lua does the same thing, and for more platforms.
   We should adapt their code to ours. */

#if defined(_BSD_PPC_SETJMP_H_)

#define STACK_ADDR(stack, sz) ((int)(((intptr_t)stack + sz - 64 + 15) & ~15))
#define JB_SP 0
#define JB_PC 21
#define nmc_setjmp _setjmp
#define SETJMP_OSX 1

#elif defined(_BSD_I386_SETJMP_H)

#define JB_SP 9
#define JB_PC 12
#define SETJMP_OSX 1

#endif    

#ifdef SETJMP_OSX

#define SETJMP_PATCH(buf, func, stack) \
buf[JB_SP] = (int)(stack); \
buf[JB_PC] = (int)(func);

#else

#if defined(i386) || defined(__i386__)

#if defined(JB_SP) && defined(JB_PC)

#define SETJMP_PATCH(buf, func, stack) \
buf->__jmpbuf[JB_SP] = (int)(stack); \
buf->__jmpbuf[JB_PC] = (int)(func);

#elif defined(_I386_JMP_BUF_H)

#define SETJMP_PATCH(buf, func, stack) \
buf->__sp = (stack); \
buf->__pc = (func);

#elif defined(_JBLEN)

#define SETJMP_PATCH(buf, func, stack) \
buf->_jb[2] = (long)(stack); \
buf->_jb[0] = (long)(func);

#elif defined(linux) || defined(__linux__)

#define JB_SP	4
#define JB_PC	5

#define SETJMP_PATCH(buf, func, stack) \
buf->__jmpbuf[JB_SP] = (int)(stack); \
buf->__jmpbuf[JB_PC] = (int)(func);

#endif

#endif

#endif

#ifndef STACK_ADDR
#include <stdint.h>
#define STACK_ADDR(stack, sz) ({ int *sp = stack + sz / 4; sp = (void*)((uintptr_t)sp - (uintptr_t)sp % 16); *--sp = 0; sp; })
// #define STACK_ADDR(stack, sz) ((int)((int*)stack + sz))
#endif

#ifndef nmc_setjmp
#define nmc_setjmp setjmp
#endif

#ifndef SETJMP_PATCH
#error "Sorry, you're platform is not supported!"
#endif

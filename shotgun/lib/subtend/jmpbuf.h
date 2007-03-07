#include <setjmp.h>

#ifndef JB_SP

/* See http://lua-users.org/lists/lua-l/2004-10/msg00217.html
   It details how lua does the same thing, and for more platforms.
   We should adapt their code to ours. */

#if defined(_BSD_PPC_SETJMP_H_)
#define STACK_SIZE(stack, sz) ((int)(((intptr_t)stack + sz - 64 + 15) & ~15))
#define JB_SP 0
#define JB_PC 21
#define nmc_setjmp _setjmp
#elif defined(_BSD_I386_SETJMP_H_)
#define JB_SP 9
#define JB_PC 12
#else

#error "Sorry, you're platform is not supported!"

#endif    

#endif

#ifndef STACK_SIZE
#define STACK_SIZE(stack, sz) ((int)((intptr_t)stack + sz))
#endif

#ifndef nmc_setjmp
#define nmc_setjmp setjmp
#endif
#ifndef RBX_DETECTION
#define RBX_DETECTION

/*
 * This file, when included, defines a bunch of macros that have detected
 * values about the current machine.
 *
 * Towards the bottom, we also use those macros to setup some configuration
 * variables.
 *
 */


/** DETECT */

#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1050
#define OS_X_10_5
#define _XOPEN_SOURCE
#define HAS_UCONTEXT
#elif defined(__APPLE__) && defined(__APPLE_CC__)
#define OS_X_ANCIENT
#endif

#if defined(__linux__)
#define HAS_UCONTEXT
#endif

#if defined(__FreeBSD__)
#define HAS_UCONTEXT
#endif

#if defined(i386) || defined(__i386) || defined(__i386__)
#define IS_X86
#endif

#if defined(_LP64) || defined(__LP64__) || defined(__x86_64__) || defined(__amd64__)
#define IS_X8664

#elif defined(IS_X86)
#define IS_X8632

#endif

/** CONFIGURE */

#ifdef HAS_EXECINFO
#define USE_EXECINFO
#endif

#endif

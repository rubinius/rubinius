
#define THREAD_DEBUG 0

extern int debug_enabled;

#define DEBUG log_debug
void debug_enable();
void debug_disable();
void log_debug(const char *fmt, ...);

#if THREAD_DEBUG
#define THDEBUG log_debug
#else
#define THDEBUG(...)
#endif


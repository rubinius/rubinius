extern int debug_enabled;

#define DEBUG log_debug
void debug_enable();
void debug_disable();
void log_debug(const char *fmt, ...);

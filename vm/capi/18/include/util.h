void ruby_setenv(const char *name, const char *value);
#undef setenv
#define setenv(name,val) ruby_setenv(name,val)

char *ruby_strdup(const char *str);
#undef strdup
#define strdup(s) ruby_strdup(s)

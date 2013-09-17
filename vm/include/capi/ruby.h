#ifndef RBX_CAPI_RUBY_H
#define RBX_CAPI_RUBY_H

#define HAVE_RUBY_DEFINES_H     1
#define HAVE_RUBY_ENCODING_H    1
#define HAVE_RUBY_INTERN_H      1
#define HAVE_RUBY_IO_H          1
#define HAVE_RUBY_MISSING_H     1
#define HAVE_RUBY_ONIGURUMA_H   1
#define HAVE_RUBY_RE_H          1
#define HAVE_RUBY_REGEX_H       1
#define HAVE_RUBY_RUBY_H        1
#define HAVE_RUBY_ST_H          1
#define HAVE_RUBY_UTIL_H        1
#define HAVE_RUBY_VERSION_H     1
#define HAVE_RUBY_VM_H          1
#ifdef RBX_WINDOWS
#define HAVE_RUBY_WIN32_H       1
#endif

#include "ruby/ruby.h"

#endif

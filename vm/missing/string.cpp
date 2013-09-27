#include "vm/config.h"

#include <string.h>

extern "C" {

#ifndef HAVE_STRNLEN
  size_t strnlen(const char* s, size_t maxlen) {
    for(size_t i = 0; i < maxlen; i++) {
      if(s[i] == '\0') return i;
    }

    return maxlen;
  }
#endif

}

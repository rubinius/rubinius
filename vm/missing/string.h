#ifndef RBX_VM_MISSING_STRING_H
#define RBX_VM_MISSING_STRING_H

extern "C" {
#ifndef HAVE_STRNLEN
  size_t strnlen(const char* s, size_t maxlen);
#endif
}

#endif

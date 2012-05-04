#ifndef RBX_VM_MISSING_SETPROCTITLE_H
#define RBX_VM_MISSING_SETPROCTITLE_H

extern "C" {
  void setproctitle(const char *fmt, ...);
  void ruby_init_setproctitle(int argc, char *argv[]);
}

#endif


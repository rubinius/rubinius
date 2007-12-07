#include "zlib.h"
#include "ruby.h"
#include <stdio.h>

z_streamp rbx_Zlib_z_stream_Alloc() {
  z_streamp strm;

  strm = (z_streamp)calloc(1, sizeof(z_stream));
  strm->next_in = Z_NULL;
  strm->opaque = Z_NULL;
  strm->zalloc = Z_NULL;
  strm->zfree = Z_NULL;

  return strm;
}

int rbx_Zlib_deflateInit2(z_streamp strm, int level, int method,
                          int windowBits, int memLevel, int strategy) {
  return deflateInit2(strm, level, method, windowBits, memLevel, strategy);
}

Init_zlib() {
  VALUE mZlib;

  mZlib = rb_define_module("Zlib");

  rb_define_const(mZlib, "ZLIB_VERSION", rb_str_new2(ZLIB_VERSION));
  rb_define_const(mZlib, "STREAM_SIZE", INT2NUM(sizeof(z_stream)));
}


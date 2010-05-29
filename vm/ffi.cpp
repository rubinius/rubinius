/* A simple C++ wrapper around libffi. */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dlfcn.h>

#include "vm.hpp"
#include "objectmemory.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/ffi_pointer.hpp"
#include "builtin/nativefunction.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

#include "ffi_util.hpp"
#include "arguments.hpp"

namespace rubinius {

  void VM::init_ffi() {
    Module* mod = new_module("FFI");
    mod->set_const(this, "TYPE_OBJECT",     Fixnum::from(RBX_FFI_TYPE_OBJECT));
    mod->set_const(this, "TYPE_CHAR",       Fixnum::from(RBX_FFI_TYPE_CHAR));
    mod->set_const(this, "TYPE_UCHAR",      Fixnum::from(RBX_FFI_TYPE_UCHAR));
    mod->set_const(this, "TYPE_SHORT",      Fixnum::from(RBX_FFI_TYPE_SHORT));
    mod->set_const(this, "TYPE_USHORT",     Fixnum::from(RBX_FFI_TYPE_USHORT));
    mod->set_const(this, "TYPE_INT",        Fixnum::from(RBX_FFI_TYPE_INT));
    mod->set_const(this, "TYPE_UINT",       Fixnum::from(RBX_FFI_TYPE_UINT));
    mod->set_const(this, "TYPE_LONG",       Fixnum::from(RBX_FFI_TYPE_LONG));
    mod->set_const(this, "TYPE_ULONG",      Fixnum::from(RBX_FFI_TYPE_ULONG));
    mod->set_const(this, "TYPE_LL",         Fixnum::from(RBX_FFI_TYPE_LONG_LONG));
    mod->set_const(this, "TYPE_ULL",        Fixnum::from(RBX_FFI_TYPE_ULONG_LONG));
    mod->set_const(this, "TYPE_FLOAT",      Fixnum::from(RBX_FFI_TYPE_FLOAT));
    mod->set_const(this, "TYPE_DOUBLE",     Fixnum::from(RBX_FFI_TYPE_DOUBLE));
    mod->set_const(this, "TYPE_PTR",        Fixnum::from(RBX_FFI_TYPE_PTR));
    mod->set_const(this, "TYPE_VOID",       Fixnum::from(RBX_FFI_TYPE_VOID));
    mod->set_const(this, "TYPE_STRING",     Fixnum::from(RBX_FFI_TYPE_STRING));
    mod->set_const(this, "TYPE_STATE",      Fixnum::from(RBX_FFI_TYPE_STATE));
    mod->set_const(this, "TYPE_STRPTR",     Fixnum::from(RBX_FFI_TYPE_STRPTR));
    mod->set_const(this, "TYPE_CHARARR",    Fixnum::from(RBX_FFI_TYPE_CHARARR));
  }
};

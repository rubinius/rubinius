/* A simple C++ wrapper around libffi. */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>

#include "config.h"

#ifndef RBX_WINDOWS
#include <dlfcn.h>
#endif

#include "vm.hpp"
#include "object_memory.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/ffi_pointer.hpp"
#include "builtin/native_function.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

#include "ffi_util.hpp"
#include "arguments.hpp"

#include "ontology.hpp"

namespace rubinius {

  void VM::init_ffi(STATE) {
    GO(ffi).set(ontology::new_module(state, "FFI", G(rubinius)));
    Module* mod = G(ffi);
    mod->set_const(state, "TYPE_CHAR",       Fixnum::from(RBX_FFI_TYPE_CHAR));
    mod->set_const(state, "TYPE_UCHAR",      Fixnum::from(RBX_FFI_TYPE_UCHAR));
    mod->set_const(state, "TYPE_BOOL",       Fixnum::from(RBX_FFI_TYPE_BOOL));
    mod->set_const(state, "TYPE_SHORT",      Fixnum::from(RBX_FFI_TYPE_SHORT));
    mod->set_const(state, "TYPE_USHORT",     Fixnum::from(RBX_FFI_TYPE_USHORT));
    mod->set_const(state, "TYPE_INT",        Fixnum::from(RBX_FFI_TYPE_INT));
    mod->set_const(state, "TYPE_UINT",       Fixnum::from(RBX_FFI_TYPE_UINT));
    mod->set_const(state, "TYPE_LONG",       Fixnum::from(RBX_FFI_TYPE_LONG));
    mod->set_const(state, "TYPE_ULONG",      Fixnum::from(RBX_FFI_TYPE_ULONG));
    mod->set_const(state, "TYPE_LL",         Fixnum::from(RBX_FFI_TYPE_LONG_LONG));
    mod->set_const(state, "TYPE_ULL",        Fixnum::from(RBX_FFI_TYPE_ULONG_LONG));
    mod->set_const(state, "TYPE_FLOAT",      Fixnum::from(RBX_FFI_TYPE_FLOAT));
    mod->set_const(state, "TYPE_DOUBLE",     Fixnum::from(RBX_FFI_TYPE_DOUBLE));
    mod->set_const(state, "TYPE_PTR",        Fixnum::from(RBX_FFI_TYPE_PTR));
    mod->set_const(state, "TYPE_VOID",       Fixnum::from(RBX_FFI_TYPE_VOID));
    mod->set_const(state, "TYPE_STRING",     Fixnum::from(RBX_FFI_TYPE_STRING));
    mod->set_const(state, "TYPE_STRPTR",     Fixnum::from(RBX_FFI_TYPE_STRPTR));
    mod->set_const(state, "TYPE_CHARARR",    Fixnum::from(RBX_FFI_TYPE_CHARARR));
    mod->set_const(state, "TYPE_ENUM",       Fixnum::from(RBX_FFI_TYPE_ENUM));
    mod->set_const(state, "TYPE_VARARGS",    Fixnum::from(RBX_FFI_TYPE_VARARGS));
  }
};

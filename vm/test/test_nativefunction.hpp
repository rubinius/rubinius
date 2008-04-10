#include "prelude.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "ffi.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestNativeFunction : public CxxTest::TestSuite {
  public:

#undef state
  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }
  
  void test_type_size() {
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_CHAR) == 1);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_UCHAR) == 1);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_SHORT) == 2);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_USHORT) == 2);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_INT) == 4);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_UINT) == 4);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_LONG) >= 4);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_ULONG) >= 4);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_LL) >= 8);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_ULL) >= 8);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_FLOAT) >= 4);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_DOUBLE) >= 8);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_PTR) >= 4);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_STRING) >= 4);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_STATE) >= 4);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_STRPTR) >= 4);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_OBJECT) >= 4);
  }

  void test_find_symbol() {
    void* ep;
    String* name1 = String::create(state, "_blah_not_here");
    String* name2 = String::create(state, "strlen");

    ep = NativeFunction::find_symbol(state, Qnil, name1);
    TS_ASSERT(!ep);

    ep = NativeFunction::find_symbol(state, Qnil, name2);
    TS_ASSERT(ep);
  }

  void test_find_symbol_in_library() {
    void* ep;
    String* lib1 = String::create(state, "blah");
    String* lib2 = String::create(state, "libc");
    String* name = String::create(state, "strlen");

    ep = NativeFunction::find_symbol(state, lib1, name);
    TS_ASSERT(!ep);

    ep = NativeFunction::find_symbol(state, lib2, name);
    TS_ASSERT(ep);
  }

  void test_create() {
    NativeFunction* func = NativeFunction::create(state, state->symbol("blah"), 0);
    TS_ASSERT_EQUALS(func->name, state->symbol("blah"));
    TS_ASSERT_EQUALS(func->data, Qnil);
  }

  void test_bind() {
    String* lib =  String::create(state, "libc");
    String* name = String::create(state, "strlen");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Object::i2n(RBX_FFI_TYPE_STRING));

    OBJECT ret = Object::i2n(RBX_FFI_TYPE_INT);

    NativeFunction *func = NativeFunction::bind(state, lib, name, args, ret);

    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, name);

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(out->fixnum_p());
    TS_ASSERT_EQUALS(out->n2i(), 6);
  }

};

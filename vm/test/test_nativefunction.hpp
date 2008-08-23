#include "prelude.hpp"
#include "builtin/nativefunction.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "ffi.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

VM *global_state;

class TestNativeFunction : public CxxTest::TestSuite {
  public:

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
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_LONG_LONG) >= 8);
    TS_ASSERT(NativeFunction::type_size(RBX_FFI_TYPE_ULONG_LONG) >= 8);
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

  void test_bind_with_string() {
    String* name = String::create(state, "strlen");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_STRING));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_INT);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, name);

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(out->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), 6);
  }

  void test_bind_with_string_null() {
    String* name = String::create(state, "getcwd");

    Array* args = Array::create(state, 2);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_STRING));
    args->set(state, 1, Fixnum::from(RBX_FFI_TYPE_INT));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_STRING);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 2);
    input->set(state, 0, Qnil);
    input->set(state, 1, Fixnum::from(0));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<String>(out));
  }

  void test_bind_with_char() {
    String* name = String::create(state, "dummy_char");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_CHAR));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_CHAR);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, Fixnum::from(13));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Fixnum>(out));
    TS_ASSERT_EQUALS(as<Fixnum>(out)->to_native(), 13);
  }

  void test_bind_with_unsigned_char() {
    String* name = String::create(state, "dummy_unsigned_char");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_UCHAR));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_UCHAR);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, Fixnum::from(13));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Fixnum>(out));
    TS_ASSERT_EQUALS(as<Fixnum>(out)->to_native(), 13);
  }

  void test_bind_with_short() {
    String* name = String::create(state, "dummy_short");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_SHORT));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_SHORT);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, Fixnum::from(13));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Fixnum>(out));
    TS_ASSERT_EQUALS(as<Fixnum>(out)->to_native(), 13);
  }

  void test_bind_with_unsigned_short() {
    String* lib =  String::create(state, "libc");
    String* name = String::create(state, "umask");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_USHORT));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_USHORT);

    NativeFunction *func = NativeFunction::bind(state, lib, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 2);
    input->set(state, 0, Fixnum::from(0));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Fixnum>(out));

    input = Array::create(state, 2);
    input->set(state, 0, out);

    msg = new Message(state, input);

    out = func->call(state, msg);

    TS_ASSERT(kind_of<Fixnum>(out));
    TS_ASSERT_EQUALS(out, Fixnum::from(0));
  }

  void test_bind_with_int() {
    String* name = String::create(state, "dummy_int");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_INT));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_INT);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, Fixnum::from(13));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Integer>(out));
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), (native_int)13);

    input = Array::create(state, 1);
    input->set(state, 0, Integer::from(state, (int)2147483647));

    msg = new Message(state, input);

    out = func->call(state, msg);

    TS_ASSERT(kind_of<Integer>(out));
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), (native_int)2147483647);
  }

  void test_bind_with_unsigned_int() {
    String* name = String::create(state, "dummy_unsigned_int");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_UINT));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_UINT);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, Fixnum::from(13));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Integer>(out));
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), (native_int)13);

    input = Array::create(state, 1);
    input->set(state, 0, Integer::from(state, (unsigned int)2147483647));

    msg = new Message(state, input);

    out = func->call(state, msg);

    TS_ASSERT(kind_of<Integer>(out));
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), (native_int)2147483647);
  }

  void test_bind_with_long() {
    String* name = String::create(state, "dummy_long");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_LONG));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_LONG);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, Fixnum::from(13));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Integer>(out));
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), (native_int)13);

    input = Array::create(state, 1);
    input->set(state, 0, Integer::from(state, (long)2147483647));

    msg = new Message(state, input);

    out = func->call(state, msg);

    TS_ASSERT(kind_of<Integer>(out));
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), (native_int)2147483647);
  }

  void test_bind_with_unsigned_long() {
    String* name = String::create(state, "dummy_unsigned_long");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_ULONG));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_ULONG);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, Fixnum::from(13));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Integer>(out));
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), (native_int)13);

    input = Array::create(state, 1);
    input->set(state, 0, Integer::from(state, (unsigned long)2147483647));

    msg = new Message(state, input);

    out = func->call(state, msg);

    TS_ASSERT(kind_of<Integer>(out));
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), (native_int)2147483647);
  }

  void test_bind_with_long_long() {
    String* name = String::create(state, "dummy_long_long");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_LONG_LONG));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_LONG_LONG);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, Fixnum::from(13));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Integer>(out));
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), (native_int)13);

    input = Array::create(state, 1);
    input->set(state, 0, Integer::from(state, 214748364741341LL));

    msg = new Message(state, input);

    out = func->call(state, msg);

    TS_ASSERT(kind_of<Bignum>(out));
    TS_ASSERT_EQUALS(as<Bignum>(out)->to_long_long(), 214748364741341LL);
  }

  void test_bind_with_unsigned_long_long() {
    String* name = String::create(state, "dummy_unsigned_long_long");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_ULONG_LONG));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_ULONG_LONG);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, Fixnum::from(13));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Integer>(out));
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), (native_int)13);

    input = Array::create(state, 1);
    input->set(state, 0, Integer::from(state, 214748364741341ULL));

    msg = new Message(state, input);

    out = func->call(state, msg);

    TS_ASSERT(kind_of<Bignum>(out));
    TS_ASSERT_EQUALS(as<Bignum>(out)->to_ulong_long(), 214748364741341ULL);
  }

  void test_bind_with_void() {
    String* name = String::create(state, "dummy_void");

    Array* args = Array::create(state, 0);

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_VOID);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Message* msg = new Message(state);

    OBJECT out = func->call(state, msg);

    TS_ASSERT_EQUALS(out, Qnil);
  }

  void test_bind_with_ptr() {
    String* name = String::create(state, "dummy_ptr");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_PTR));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_PTR);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    char buffer[1];
    buffer[0] = 0;
    MemoryPointer* ptr = MemoryPointer::create(state, buffer);

    Array* input = Array::create(state, 1);
    input->set(state, 0, ptr);

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<MemoryPointer>(out));
    TS_ASSERT_EQUALS(as<MemoryPointer>(out)->pointer, ptr->pointer);

    input = Array::create(state, 1);
    input->set(state, 0, Qnil);

    msg = new Message(state, input);

    out = func->call(state, msg);

    TS_ASSERT_EQUALS(out, Qnil);
  }

  void test_bind_with_float() {
    String* name = String::create(state, "dummy_float");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_FLOAT));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_FLOAT);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, Float::create(state, 13.2));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Float>(out));
    TS_ASSERT(as<Float>(out)->val > 13.19);
    TS_ASSERT(as<Float>(out)->val < 13.21);
  }

  void test_bind_with_double() {
    String* name = String::create(state, "dummy_double");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_DOUBLE));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_DOUBLE);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, Float::create(state, 13.2));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Float>(out));
    TS_ASSERT_EQUALS(as<Float>(out)->val, 13.2);
  }

  void test_bind_with_string_returned() {
    String* name = String::create(state, "dummy_string");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_STRING));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_STRING);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, String::create(state, "whatever"));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<String>(out));
    TS_ASSERT_EQUALS(as<String>(out)->byte_address(), std::string("whatever"));

    input = Array::create(state, 1);
    input->set(state, 0, Qnil);

    msg = new Message(state, input);

    out = func->call(state, msg);

    TS_ASSERT_EQUALS(out, Qnil);
  }

  void test_bind_with_string_and_ptr() {
    String* name = String::create(state, "dummy_string");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_STRING));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_STRPTR);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    input->set(state, 0, String::create(state, "whatever"));

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Array>(out));

    Object* o1 = as<Array>(out)->get(state, 0);
    Object* o2 = as<Array>(out)->get(state, 1);
    TS_ASSERT(kind_of<String>(o1));
    TS_ASSERT(kind_of<MemoryPointer>(o2));

    TS_ASSERT_EQUALS(as<String>(o1)->byte_address(), std::string("whatever"));
    TS_ASSERT(strcmp((char*)(as<MemoryPointer>(o2)->pointer), "whatever") == 0);

    input = Array::create(state, 1);
    input->set(state, 0, Qnil);

    msg = new Message(state, input);

    out = func->call(state, msg);

    TS_ASSERT(kind_of<Array>(out));

    o1 = as<Array>(out)->get(state, 0);
    o2 = as<Array>(out)->get(state, 1);
    TS_ASSERT_EQUALS(o1, Qnil);
    TS_ASSERT_EQUALS(o2, Qnil);
  }

  void test_bind_with_object() {
    String* name = String::create(state, "dummy_ptr");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_OBJECT));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_OBJECT);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Array* input = Array::create(state, 1);
    OBJECT obj = state->om->new_object(G(object), Object::fields);
    input->set(state, 0, obj);

    Message* msg = new Message(state, input);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Object>(out));
    TS_ASSERT_EQUALS(out, obj);

    input = Array::create(state, 1);
    input->set(state, 0, Qnil);

    msg = new Message(state, input);

    out = func->call(state, msg);

    TS_ASSERT_EQUALS(out, Qnil);
  }

  void test_bind_with_state() {
    global_state = state;

    String* name = String::create(state, "dummy_state");

    Array* args = Array::create(state, 1);
    args->set(state, 0, Fixnum::from(RBX_FFI_TYPE_STATE));

    OBJECT ret = Fixnum::from(RBX_FFI_TYPE_INT);

    NativeFunction *func = NativeFunction::bind(state, Qnil, name, args, ret);

    TS_ASSERT(!func->nil_p());
    TS_ASSERT(func->data->check_type(MemPtrType));

    Message* msg = new Message(state);

    OBJECT out = func->call(state, msg);

    TS_ASSERT(kind_of<Fixnum>(out));
    TS_ASSERT(as<Fixnum>(out)->to_native());
  }

};

extern "C" {

  char dummy_char(char c) {
    return c;
  }

  unsigned char dummy_unsigned_char(unsigned char c) {
    return c;
  }

  short dummy_short(short c) {
    return c;
  }

  unsigned short dummy_unsigned_short(unsigned short c) {
    return c;
  }

  int dummy_int(int c) {
    return c;
  }

  unsigned int dummy_unsigned_int(unsigned int c) {
    return c;
  }

  long dummy_long(long c) {
    return c;
  }

  unsigned long dummy_unsigned_long(unsigned long c) {
    return c;
  }

  long long dummy_long_long(long long c) {
    return c;
  }

  unsigned long long dummy_unsigned_long_long(unsigned long long c) {
    return c;
  }

  float dummy_float(float c) {
    return c;
  }

  double dummy_double(double c) {
    return c;
  }

  void* dummy_ptr(void* c) {
    return c;
  }

  void dummy_void() {
    return;
  }

  char* dummy_string(char* c) {
    return c;
  }

  int dummy_state(STATE) {
    return global_state == state;
  }
}

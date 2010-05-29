#include "vm/test/test.hpp"

#include "prelude.hpp"
#include "ffi_util.hpp"

#include "builtin/ffi_pointer.hpp"

class TestPointer : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_create() {
    int thing = 3;
    Pointer* ptr = Pointer::create(state, (void*)&thing);
    TS_ASSERT_EQUALS(ptr->pointer, &thing);
  }

  void test_get_address() {
    int one = 1;
    void *addr = &one;
    Pointer* ptr = Pointer::create(state, addr);
    TS_ASSERT(ptr->get_address(state)->equal(state, Integer::from(state, (size_t)addr)));
  }

  void test_add() {
    int one = 1;
    void *addr = &one;
    Pointer* ptr = Pointer::create(state, addr);
    TS_ASSERT(ptr->add(state, Integer::from(state, (size_t)addr))->equal(
              state, Integer::from(state, (size_t)addr+(size_t)addr)));
  }

  void test_set_autorelease() {
    Pointer* ptr = Pointer::create(state, NULL);
    TS_ASSERT_EQUALS(Qfalse, ptr->set_autorelease(state, Qfalse));
    TS_ASSERT_EQUALS(false, ptr->autorelease);
    TS_ASSERT_EQUALS(Qtrue, ptr->set_autorelease(state, Qtrue));
    TS_ASSERT_EQUALS(true, ptr->autorelease);
  }

  void test_read_string() {
    char str[] = "ruby";
    Pointer* ptr = Pointer::create(state, str);
    String* s = ptr->read_string(state, Fixnum::from(4));
    TS_ASSERT_SAME_DATA(s->byte_address(), "ruby", 4);
  }

  void test_read_string_to_null() {
    char str[] = "ruby";
    Pointer* ptr = Pointer::create(state, str);
    String* s = ptr->read_string_to_null(state);
    TS_ASSERT_SAME_DATA(s->byte_address(), "ruby", 4);
  }

  void test_write_string() {
    String* str = String::create(state, "ruby");
    Pointer* ptr = Pointer::create(state, str->byte_address());
    ptr->write_string(state, String::create(state, "ybur"), Fixnum::from(4));
    String* s = ptr->read_string(state, Fixnum::from(4));
    TS_ASSERT_SAME_DATA(s->byte_address(), "ybur", 4);
  }

  void test_read_int() {
    int one = 1;
    void *addr = &one;
    Pointer* ptr = Pointer::create(state, addr);
    TS_ASSERT(Integer::from(state, 1)->equal(state, ptr->read_int(state, Qtrue)));
  }

  void test_write_int() {
    int one = 1;
    void *addr = &one;
    Pointer* ptr = Pointer::create(state, addr);
    ptr->write_int(state, Integer::from(state, 0xfffffffa));
    TS_ASSERT(Integer::from(state, 0xfffffffa)->equal(state, ptr->read_int(state, Qtrue)));
  }

  void test_read_long() {
    long one = 1;
    void *addr = &one;
    Pointer* ptr = Pointer::create(state, addr);
    TS_ASSERT(Integer::from(state, 1)->equal(state, ptr->read_long(state)));
  }

  void test_write_long() {
    long one = 1;
    void *addr = &one;
    Pointer* ptr = Pointer::create(state, addr);
    ptr->write_long(state, Integer::from(state, 0xfffffffa));
    TS_ASSERT(Integer::from(state, 0xfffffffa)->equal(state, ptr->read_long(state)));
  }

  void test_read_float() {
    float one = 1.0;
    Pointer* ptr = Pointer::create(state, &one);
    Float* f = ptr->read_float(state);
    TS_ASSERT_EQUALS(1.0, f->val);
  }

  void test_write_float() {
    float one = 1.0;
    Pointer* ptr = Pointer::create(state, &one);
    ptr->write_float(state, Float::create(state, 2.0));
    Float* f = ptr->read_float(state);
    TS_ASSERT_EQUALS(2.0, f->val);
  }

  void test_read_double() {
    double one = 1.0;
    Pointer* ptr = Pointer::create(state, &one);
    Float* f = ptr->read_double(state);
    TS_ASSERT_EQUALS(1.0, f->val);
  }

  void test_write_double() {
    double one = 1.0;
    Pointer* ptr = Pointer::create(state, &one);
    ptr->write_double(state, Float::create(state, 2.0));
    Float* f = ptr->read_double(state);
    TS_ASSERT_EQUALS(2.0, f->val);
  }

  void test_read_pointer() {
    uintptr_t one = 1;
    void *addr = &one;
    Pointer* ptr = Pointer::create(state, addr);
    Pointer* p = ptr->read_pointer(state);
    TS_ASSERT_EQUALS((void*)1, p->pointer);
  }

  void test_get_at_offset() {
    char one = 1;
    Pointer* ptr = Pointer::create(state, &one);
    Object* obj = ptr->get_at_offset(state, Fixnum::from(0), Fixnum::from(RBX_FFI_TYPE_CHAR));

    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(obj)->to_native(), 1);
  }

  void test_set_at_offset() {
    char buffer[1024];
    Object* one = Fixnum::from(1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_at_offset(state, Fixnum::from(0), Fixnum::from(RBX_FFI_TYPE_CHAR), one);
    TS_ASSERT_EQUALS(*buffer, static_cast<char>(1));
  }

  void test_get_field_char() {
    char one = 1;
    Pointer* ptr = Pointer::create(state, &one);
    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_CHAR);

    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(obj)->to_native(), 1);
  }

  void test_get_field_uchar() {
    unsigned char one = 1;
    Pointer* ptr = Pointer::create(state, &one);
    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_UCHAR);

    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(obj)->to_native(), 1);
  }

  void test_get_field_short() {
    short one = 1;
    Pointer* ptr = Pointer::create(state, &one);
    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_SHORT);

    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(obj)->to_native(), 1);
  }

  void test_get_field_ushort() {
    unsigned short one = 1;
    Pointer* ptr = Pointer::create(state, &one);
    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_USHORT);

    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(obj)->to_native(), 1);
  }

  void test_get_field_int() {
    int one = 1;
    Pointer* ptr = Pointer::create(state, &one);
    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_INT);

    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(obj)->to_native(), 1);
  }

  void test_get_field_uint() {
    unsigned int one = 1;
    Pointer* ptr = Pointer::create(state, &one);
    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_UINT);

    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(obj)->to_native(), 1);
  }

  void test_get_field_long() {
    long one = 1;
    Pointer* ptr = Pointer::create(state, &one);
    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_LONG);

    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(obj)->to_native(), 1);
  }

  void test_get_field_ulong() {
    unsigned long one = 1;
    Pointer* ptr = Pointer::create(state, &one);
    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_ULONG);

    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(obj)->to_native(), 1);
  }

  void test_get_field_float() {
    float one = 1.0;
    Pointer* ptr = Pointer::create(state, &one);
    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_FLOAT);

    TS_ASSERT(obj->check_type(FloatType));
    TS_ASSERT_EQUALS(as<Float>(obj)->to_double(state), 1.0);
  }

  void test_get_field_double() {
    double one = 1.0;
    Pointer* ptr = Pointer::create(state, &one);
    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_DOUBLE);

    TS_ASSERT(obj->check_type(FloatType));
    TS_ASSERT_EQUALS(as<Float>(obj)->to_double(state), 1.0);
  }

  void test_get_field_ll() {
    long long one = 1;
    Pointer* ptr = Pointer::create(state, &one);
    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_LONG_LONG);

    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(obj)->to_native(), 1);
  }

  void test_get_field_ull() {
    unsigned long long one = 1;
    Pointer* ptr = Pointer::create(state, &one);
    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_ULONG_LONG);

    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(obj)->to_native(), 1);
  }

  void test_get_field_object() {
    Object* one = Fixnum::from(1);

    Pointer* ptr = Pointer::create(state, &one);
    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_OBJECT);

    TS_ASSERT_EQUALS(one, obj);
  }

  void test_get_field_ptr() {
    int thing = 1;
    void *val = &thing;

    Pointer* ptr = Pointer::create(state, &val);

    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_PTR);
    TS_ASSERT(obj->check_type(PointerType));

    TS_ASSERT_EQUALS(as<Pointer>(obj)->pointer, val);
  }

  void test_get_field_ptr_thats_null() {
    void *val = NULL;

    Pointer* ptr = Pointer::create(state, &val);

    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_PTR);
    TS_ASSERT(obj->nil_p());
  }

  void test_get_field_string() {
    const char *str = "blah\0";

    Pointer* ptr = Pointer::create(state, &str);

    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_STRING);
    TS_ASSERT(obj->check_type(StringType));

    String* so = as<String>(obj);

    TS_ASSERT(!strncmp(str, so->c_str(), 4));
  }

  void test_get_field_string_thats_null() {
    char *str = NULL;

    Pointer* ptr = Pointer::create(state, &str);

    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_STRING);
    TS_ASSERT(obj->nil_p());
  }

  void test_get_field_strptr() {
    const char *str = "blah\0";

    Pointer* ptr = Pointer::create(state, &str);

    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_STRPTR);
    TS_ASSERT(obj->check_type(ArrayType));

    Array* ary = as<Array>(obj);
    TS_ASSERT_EQUALS(ary->size(), 2UL);

    TS_ASSERT(ary->get(state, 0)->check_type(StringType));
    String *so = as<String>(ary->get(state, 0));
    TS_ASSERT(!strncmp(str, so->c_str(), 4));

    TS_ASSERT(ary->get(state, 1)->check_type(PointerType));
    Pointer* mp = as<Pointer>(ary->get(state, 1));
    TS_ASSERT_EQUALS(mp->pointer, str);
  }

  void test_get_field_strptr_thats_null() {
    char *str = NULL;

    Pointer* ptr = Pointer::create(state, &str);

    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_STRPTR);
    TS_ASSERT(obj->check_type(ArrayType));

    Array* ary = as<Array>(obj);
    TS_ASSERT_EQUALS(ary->size(), 2U);

    TS_ASSERT(ary->get(state, 0)->nil_p());
    TS_ASSERT(ary->get(state, 1)->nil_p());
  }

  void test_get_field_void() {
    char *str = NULL;
    Pointer* ptr = Pointer::create(state, &str);

    Object* obj = ptr->get_field(state, 0, RBX_FFI_TYPE_VOID);
    TS_ASSERT(obj->nil_p());
  }

  void test_set_field_char() {
    char buffer[1024];
    Object* one = Fixnum::from(1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_CHAR, one);
    TS_ASSERT_EQUALS(*buffer, static_cast<char>(1));
  }

  void test_set_field_uchar() {
    unsigned char buffer[1024];
    Object* one = Fixnum::from(1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_UCHAR, one);
    TS_ASSERT_EQUALS(*buffer, static_cast<unsigned char>(1));
  }

  void test_set_field_short() {
    short buffer[1024];
    Object* one = Fixnum::from(1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_SHORT, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }

  void test_set_field_ushort() {
    unsigned short buffer[1024];
    Object* one = Fixnum::from(1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_USHORT, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }

  void test_set_field_int() {
    int buffer[1024];
    Object* one = Fixnum::from(1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_INT, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }

  void test_set_field_int_bignum() {
    int buffer[1024];
    Object* one = Bignum::from(state, (native_int)1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_INT, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }

  void test_set_field_uint() {
    unsigned int buffer[1024];
    Object* one = Fixnum::from(1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_UINT, one);
    TS_ASSERT_EQUALS(*buffer, 1U);
  }

  void test_set_field_uint_bignum() {
    unsigned int buffer[1024];
    Object* one = Bignum::from(state, (native_int)1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_UINT, one);
    TS_ASSERT_EQUALS(*buffer, 1U);
  }

  void test_set_field_long() {
    long buffer[1024];
    Object* one = Fixnum::from(1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_LONG, one);
    TS_ASSERT_EQUALS(*buffer, 1L);
  }

  void test_set_field_long_bignum() {
    long buffer[1024];
    Object* one = Bignum::from(state, (native_int)1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_LONG, one);
    TS_ASSERT_EQUALS(*buffer, 1L);
  }

  void test_set_field_ulong() {
    unsigned long buffer[1024];
    Object* one = Fixnum::from(1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_ULONG, one);
    TS_ASSERT_EQUALS(*buffer, 1UL);
  }

  void test_set_field_ulong_bignum() {
    unsigned long buffer[1024];
    Object* one = Bignum::from(state, (native_int)1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_ULONG, one);
    TS_ASSERT_EQUALS(*buffer, 1UL);
  }

  void test_set_field_float() {
    float buffer[1024];
    Object* one = Float::create(state, 1.0);

    buffer[0] = 0.0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_FLOAT, one);
    TS_ASSERT_EQUALS(*buffer, 1.0);
  }

  void test_set_field_double() {
    double buffer[1024];
    Object* one = Float::create(state, 1.0);

    buffer[0] = 0.0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_DOUBLE, one);
    TS_ASSERT_EQUALS(*buffer, 1.0);
  }

  void test_set_field_ll() {
    long long buffer[1024];
    Object* one = Fixnum::from(1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_LONG_LONG, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }

  void test_set_field_ll_bignum() {
    long long buffer[1024];
    Object* one = Bignum::from(state, (native_int)1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_LONG_LONG, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }

  void test_set_field_ull() {
    unsigned long long buffer[1024];
    Object* one = Fixnum::from(1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_ULONG_LONG, one);
    TS_ASSERT_EQUALS(*buffer, 1ULL);
  }

  void test_set_field_ull_bignum() {
    unsigned long long buffer[1024];
    Object* one = Bignum::from(state, (native_int)1);

    buffer[0] = 0;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_ULONG_LONG, one);
    TS_ASSERT_EQUALS(*buffer, 1ULL);
  }

  void test_set_field_object() {
    Object* buffer[1024];
    Object* one = Fixnum::from(1);

    buffer[0] = Qnil;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_OBJECT, one);
    TS_ASSERT_EQUALS(*buffer, one);
  }

  void test_set_field_ptr() {
    void* buffer[1024];
    int val = 3;
    Object* one = Pointer::create(state, &val);

    buffer[0] = NULL;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_PTR, one);
    TS_ASSERT_EQUALS(*buffer, &val);
  }

  void test_set_field_ptr_null() {
    void* buffer[1024];
    int val = 3;

    buffer[0] = &val;

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_PTR, Qnil);
    TS_ASSERT_EQUALS(*buffer, (void*)NULL);
  }

  void test_set_field_string() {
    char* buffer[1024];

    String* str = String::create(state, "blah");

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_STRING, str);
    TS_ASSERT_EQUALS(*buffer, str->c_str());
  }

  void test_set_field_string_null() {
    char* buffer[1024];

    Pointer* ptr = Pointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_STRING, Qnil);
    TS_ASSERT_EQUALS(*buffer, (char*)NULL);
  }

};

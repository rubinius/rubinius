#include "prelude.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "ffi.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestMemoryPointer : public CxxTest::TestSuite {
  public:

#undef state
  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_create() {
    int thing = 3.0;
    MemoryPointer *ptr = MemoryPointer::create(state, (void*)&thing);
    TS_ASSERT_EQUALS(ptr->pointer, &thing);
  }

  void test_get_field_char() {
    char one = 1;
    MemoryPointer *ptr = MemoryPointer::create(state, &one);
    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_CHAR);
    
    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(obj->n2i(), 1);
  }
  
  void test_get_field_uchar() {
    unsigned char one = 1;
    MemoryPointer *ptr = MemoryPointer::create(state, &one);
    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_UCHAR);
    
    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(obj->n2i(), 1);
  }

  void test_get_field_short() {
    short one = 1;
    MemoryPointer *ptr = MemoryPointer::create(state, &one);
    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_SHORT);
    
    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(obj->n2i(), 1);
  }
  
  void test_get_field_ushort() {
    unsigned short one = 1;
    MemoryPointer *ptr = MemoryPointer::create(state, &one);
    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_USHORT);
    
    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(obj->n2i(), 1);
  }
  
  void test_get_field_int() {
    int one = 1;
    MemoryPointer *ptr = MemoryPointer::create(state, &one);
    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_INT);
    
    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(obj->n2i(), 1);
  }
  
  void test_get_field_uint() {
    unsigned int one = 1;
    MemoryPointer *ptr = MemoryPointer::create(state, &one);
    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_UINT);
    
    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(obj->n2i(), 1);
  }
  
  void test_get_field_long() {
    long one = 1;
    MemoryPointer *ptr = MemoryPointer::create(state, &one);
    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_LONG);
    
    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(obj->n2i(), 1);
  }
  
  void test_get_field_ulong() {
    unsigned long one = 1;
    MemoryPointer *ptr = MemoryPointer::create(state, &one);
    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_ULONG);
    
    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(obj->n2i(), 1);
  }
  
  void test_get_field_float() {
    float one = 1.0;
    MemoryPointer *ptr = MemoryPointer::create(state, &one);
    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_FLOAT);
   
    TS_ASSERT(obj->check_type(FloatType));
    TS_ASSERT_EQUALS(as<Float>(obj)->to_double(state), 1.0);
  }
  
  void test_get_field_double() {
    double one = 1.0;
    MemoryPointer *ptr = MemoryPointer::create(state, &one);
    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_DOUBLE);
   
    TS_ASSERT(obj->check_type(FloatType));
    TS_ASSERT_EQUALS(as<Float>(obj)->to_double(state), 1.0);
  }
  
  void test_get_field_ll() {
    long long one = 1;
    MemoryPointer *ptr = MemoryPointer::create(state, &one);
    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_LL);
    
    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(obj->n2i(), 1);
  }
  
  void test_get_field_ull() {
    unsigned long long one = 1;
    MemoryPointer *ptr = MemoryPointer::create(state, &one);
    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_ULL);
    
    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(obj->n2i(), 1);
  }

  void test_get_field_object() {
    OBJECT one = Object::i2n(1);
    
    MemoryPointer *ptr = MemoryPointer::create(state, &one);
    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_OBJECT);
    
    TS_ASSERT_EQUALS(one, obj);
  }
  
  void test_get_field_ptr() {
    int thing = 1;
    void *val = &thing;

    MemoryPointer *ptr = MemoryPointer::create(state, &val);

    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_PTR);
    TS_ASSERT(obj->check_type(MemPtrType));

    TS_ASSERT_EQUALS(as<MemoryPointer>(obj)->pointer, val);
  }
  
  void test_get_field_ptr_thats_null() {
    void *val = NULL;

    MemoryPointer *ptr = MemoryPointer::create(state, &val);

    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_PTR);
    TS_ASSERT(obj->nil_p());
  }

  void test_get_field_string() {
    char *str = "blah\0";
    
    MemoryPointer *ptr = MemoryPointer::create(state, &str);

    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_STRING);
    TS_ASSERT(obj->check_type(StringType));

    String* so = as<String>(obj);

    TS_ASSERT(!strncmp(str, *so, 4));
  }
  
  void test_get_field_string_thats_null() {
    char *str = NULL;
    
    MemoryPointer *ptr = MemoryPointer::create(state, &str);

    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_STRING);
    TS_ASSERT(obj->nil_p());
  }
  
  void test_get_field_strptr() {
    char *str = "blah\0";
    
    MemoryPointer *ptr = MemoryPointer::create(state, &str);

    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_STRPTR);
    TS_ASSERT(obj->check_type(ArrayType));

    Array* ary = as<Array>(obj);
    TS_ASSERT_EQUALS(ary->size(), 2);

    TS_ASSERT(ary->get(state, 0)->check_type(StringType));
    String *so = as<String>(ary->get(state, 0));
    TS_ASSERT(!strncmp(str, *so, 4));

    TS_ASSERT(ary->get(state, 1)->check_type(MemPtrType));
    MemoryPointer *mp = as<MemoryPointer>(ary->get(state, 1));
    TS_ASSERT_EQUALS(mp->pointer, str);
  }

  void test_get_field_strptr_thats_null() {
    char *str = NULL;
    
    MemoryPointer *ptr = MemoryPointer::create(state, &str);

    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_STRPTR);
    TS_ASSERT(obj->check_type(ArrayType));

    Array* ary = as<Array>(obj);
    TS_ASSERT_EQUALS(ary->size(), 2);

    TS_ASSERT(ary->get(state, 0)->nil_p());
    TS_ASSERT(ary->get(state, 1)->nil_p());
  }

  void test_get_field_void() {
    char *str = NULL;
    MemoryPointer *ptr = MemoryPointer::create(state, &str);

    OBJECT obj = ptr->get_field(state, 0, RBX_FFI_TYPE_VOID);
    TS_ASSERT(obj->nil_p());
  }

  void test_set_field_char() {
    char buffer[1024];
    OBJECT one = Object::i2n(1);

    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_CHAR, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }
  
  void test_set_field_uchar() {
    unsigned char buffer[1024];
    OBJECT one = Object::i2n(1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_UCHAR, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }
  
  void test_set_field_short() {
    short buffer[1024];
    OBJECT one = Object::i2n(1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_SHORT, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }
  
  void test_set_field_ushort() {
    unsigned short buffer[1024];
    OBJECT one = Object::i2n(1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_USHORT, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }

  void test_set_field_int() {
    int buffer[1024];
    OBJECT one = Object::i2n(1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_INT, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }
  
  void test_set_field_int_bignum() {
    int buffer[1024];
    OBJECT one = Bignum::create(state, 1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_INT, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }

  void test_set_field_uint() {
    unsigned int buffer[1024];
    OBJECT one = Object::i2n(1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_UINT, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }
  
  void test_set_field_uint_bignum() {
    unsigned int buffer[1024];
    OBJECT one = Bignum::create(state, 1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_UINT, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }

  void test_set_field_long() {
    long buffer[1024];
    OBJECT one = Object::i2n(1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_LONG, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }
  
  void test_set_field_long_bignum() {
    long buffer[1024];
    OBJECT one = Bignum::create(state, 1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_LONG, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }
  
  void test_set_field_ulong() {
    unsigned long buffer[1024];
    OBJECT one = Object::i2n(1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_ULONG, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }
  
  void test_set_field_ulong_bignum() {
    unsigned long buffer[1024];
    OBJECT one = Bignum::create(state, 1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_ULONG, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }

  void test_set_field_float() {
    float buffer[1024];
    OBJECT one = Float::create(state, 1.0);
    
    buffer[0] = 0.0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_FLOAT, one);
    TS_ASSERT_EQUALS(*buffer, 1.0);
  }
  
  void test_set_field_double() {
    double buffer[1024];
    OBJECT one = Float::create(state, 1.0);
    
    buffer[0] = 0.0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_DOUBLE, one);
    TS_ASSERT_EQUALS(*buffer, 1.0);
  }
  
  void test_set_field_ll() {
    long long buffer[1024];
    OBJECT one = Object::i2n(1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_LL, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }
  
  void test_set_field_ll_bignum() {
    long long buffer[1024];
    OBJECT one = Bignum::create(state, 1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_LL, one);
    TS_ASSERT_EQUALS(*buffer, 1);
  }
  
  void test_set_field_ull() {
    unsigned long long buffer[1024];
    OBJECT one = Object::i2n(1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_ULL, one);
    TS_ASSERT_EQUALS(*buffer, 1ULL);
  }
  
  void test_set_field_ull_bignum() {
    unsigned long long buffer[1024];
    OBJECT one = Bignum::create(state, 1);
    
    buffer[0] = 0;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_ULL, one);
    TS_ASSERT_EQUALS(*buffer, 1ULL);
  }

  void test_set_field_object() {
    OBJECT buffer[1024];
    OBJECT one = Object::i2n(1);

    buffer[0] = Qnil;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_OBJECT, one);
    TS_ASSERT_EQUALS(*buffer, one);
  }
  
  void test_set_field_ptr() {
    void* buffer[1024];
    int val = 3;
    OBJECT one = MemoryPointer::create(state, &val);

    buffer[0] = NULL;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_PTR, one);
    TS_ASSERT_EQUALS(*buffer, &val);
  }
  
  void test_set_field_ptr_null() {
    void* buffer[1024];
    int val = 3;

    buffer[0] = &val;

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_PTR, Qnil);
    TS_ASSERT_EQUALS(*buffer, (void*)NULL);
  }

  void test_set_field_string() {
    char* buffer[1024];

    String* str = String::create(state, "blah");

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_STRING, str);
    TS_ASSERT_EQUALS(*buffer, str->byte_address());
  }
  
  void test_set_field_string_null() {
    char* buffer[1024];

    MemoryPointer* ptr = MemoryPointer::create(state, buffer);
    ptr->set_field(state, 0, RBX_FFI_TYPE_STRING, Qnil);
    TS_ASSERT_EQUALS(*buffer, (char*)NULL);
  }

};

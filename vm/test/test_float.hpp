#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestFloat : public CxxTest::TestSuite {
  public:

#undef state
  VM *state;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_create() {
    Float* flt = Float::create(state, 1.0);
    TS_ASSERT_EQUALS(flt->val, 1.0);
  }
  
  void test_compare() {
      Float* fltBig = Float::create(state, 4.0);
      Float* fltSmall = Float::create(state, 9.0);
      TS_ASSERT_EQUALS(fltBig->compare(state, fltSmall), Object::i2n(-1));
      TS_ASSERT_EQUALS(fltSmall->compare(state, fltBig), Object::i2n(1));
      TS_ASSERT_EQUALS(fltSmall->compare(state, fltSmall), Object::i2n(0));
  }
  
  void test_coerce() {
      OBJECT o = Object::i2n(5432);
      Float* coerced = Float::coerce(state, o);
      TS_ASSERT(kind_of<Float>(coerced));
      
      Bignum* bn = Bignum::create(state, (native_int)2147483647);
      Float* coercedBn = Float::coerce(state, bn);
      TS_ASSERT(kind_of<Float>(coercedBn));
      
      String* str = String::create(state, "blah");
      Float* coercedStr = Float::coerce(state, str);
      TS_ASSERT(kind_of<Float>(coercedStr));
      TS_ASSERT_EQUALS(coercedStr->val, 0.0);
  }
  
  void test_into_string() {
      Float* flt = Float::create(state, 1.0);
      char buf[10];
      flt->into_string(state, buf,10);
      TS_ASSERT(strcmp(buf, "+1.000000") == 0);
  }
};

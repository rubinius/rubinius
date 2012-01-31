#include "vm/test/test.hpp"

#include "builtin/class.hpp"
#include "builtin/time.hpp"
#include "object_types.hpp"

class TestTime : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_create() {
    Time* tm = Time::now(state, G(time_class));

    TS_ASSERT(kind_of<Integer>(tm->seconds(state)));
    TS_ASSERT(kind_of<Integer>(tm->useconds(state)));
    TS_ASSERT_EQUALS(tm->is_gmt(), cFalse);
  }
};

#include "vm/test/test.hpp"

#include "builtin/iseq.hpp"

class TestCompiledCode : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_write_me() {

  }
};

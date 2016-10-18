#include "machine/test/test.hpp"

#include "class/iseq.hpp"

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

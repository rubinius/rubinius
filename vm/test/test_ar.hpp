#include "ar.hpp"

#include <cxxtest/TestSuite.h>

#include <vector>

using namespace rubinius;

class CollectAr : public Ar {
public:
  std::vector<char*> names;

  CollectAr(const char* path) : Ar(path) { }

  virtual bool each_file(const char* path, uint8_t* data, long len) {
    names.push_back(strdup(path));
    return true;
  }
};

class TestAr : public CxxTest::TestSuite {
public:
  void test_open_for_nonexistant_file() {
    Ar ar("blahnotthere");
    TS_ASSERT(!ar.open());
  }

  void test_open_for_invalid_file() {
    Ar ar("Makefile");
    TS_ASSERT(!ar.open());
  }

  void test_process() {
    CollectAr ar("vm/test/test.a");
    TS_ASSERT(ar.open());
    TS_ASSERT_EQUALS(ar.names.size(), 0U);

    ar.process();
    TS_ASSERT_EQUALS(ar.names.size(), 3U);
    TS_ASSERT(!strcmp("Makefile", ar.names[0]));
    TS_ASSERT(!strcmp("Rakefile", ar.names[1]));
    TS_ASSERT(!strcmp("a_file_with_a_very_long_file_name", ar.names[2]));
  }
};

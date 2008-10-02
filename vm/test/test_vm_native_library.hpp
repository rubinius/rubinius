#include "native_libraries.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;


class TestNativeLibrary : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_find_symbol_default() {
    void* ep;
    String* name1 = String::create(state, "_blah_not_here");
    String* name2 = String::create(state, "strlen");

    ep = NativeLibrary::find_symbol(name1, Qnil);
    TS_ASSERT(!ep);

    ep = NativeLibrary::find_symbol(name2, Qnil);
    TS_ASSERT(ep);
  }

  void test_find_symbol_in_library() {
    void* ep;
    String* lib1 = String::create(state, "blah");
    String* lib2 = String::create(state, "libc");
    String* name = String::create(state, "strlen");

    ep = NativeLibrary::find_symbol(name, lib1);
    TS_ASSERT(!ep);

    // TODO:  This is probably failing on Linux because libc is already loaded
    //        so dlopen is returning NULL.
    ep = NativeLibrary::find_symbol(name, lib2);
    TS_ASSERT(ep);
  }
};

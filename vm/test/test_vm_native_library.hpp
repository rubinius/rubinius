#include "native_libraries.hpp"

#include "builtin/exception.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;


class TestNativeLibrary : public CxxTest::TestSuite {
  public:

  VM *state_;

  void setUp() {
    state_ = new VM();
  }

  void tearDown() {
    delete state_;
  }

  void test_find_symbol_default() {
    void* ep;
    String* name1 = String::create(state_, "_blah_not_here");
    String* name2 = String::create(state_, "strlen");

    TS_ASSERT_THROWS(NativeLibrary::find_symbol(state_, name1, Qnil),
                     RubyException);

    ep = NativeLibrary::find_symbol(state_, name2, Qnil);
    TS_ASSERT(ep);
  }

  void test_find_symbol_in_library() {
    void* ep;
    String* lib1 = String::create(state_, "blah");
    String* lib2 = String::create(state_, "libc");
    String* name = String::create(state_, "strlen");


    TS_ASSERT_THROWS(NativeLibrary::find_symbol(state_, name, lib1),
                     RubyException);

    // TODO:  This is probably failing on Linux because libc is already loaded
    //        so dlopen is returning NULL.
    ep = NativeLibrary::find_symbol(state_, name, lib2);
    TS_ASSERT(ep);
  }

};

#include <cstdlib>

#include "native_libraries.hpp"
#include "builtin/exception.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;


class TestNativeLibrary : public CxxTest::TestSuite {
  public:

  VM* state_;
  const char* lib_name_;

  void setUp() {
    state_ = new VM();
    lib_name_ = ::getenv("LIBRUBY");
  }

  void tearDown() {
    delete state_;
  }

  void test_find_symbol_in_this_process() {
    String* name = String::create(state_, "strlen");      /* libc */

    TS_ASSERT(NativeLibrary::find_symbol(state_, name, Qnil));
  }

  void test_find_symbol_in_this_process_throws_on_unloaded_library() {
    String* name = String::create(state_, "ruby_version"); /* libruby.1.8 */

    TS_ASSERT_THROWS(NativeLibrary::find_symbol(state_, name, Qnil),
                     RubyException);
  }

  void test_find_symbol_in_this_process_throws_on_nonexisting_symbol() {
    String* name = String::create(state_, "nonesuch_just_made__u___p__yep____");

    TS_ASSERT_THROWS(NativeLibrary::find_symbol(state_, name, Qnil),
                     RubyException);
  }

  /**
   *  \file
   *
   *  @todo   This only works under the assumption that anyone running
   *          these tests has MRI installed. We could be more creative
   *          but frankly it requires too much code in C++. This code
   *          _must_ exercise a library that has not been loaded into
   *          the current image (@see test_find_symbol_in_this_process)
   *          because whether the call to load e.g. libc succeeds is
   *          platform-dependent. --rue
   */

  void test_find_symbol_in_library() {
    if(lib_name_) {
      String* lib = String::create(state_, lib_name_);
      String* name = String::create(state_, "ruby_version");

      TS_ASSERT(NativeLibrary::find_symbol(state_, name, lib));
    }
  }

  void test_find_symbol_in_library_throws_on_nonexisting_library() {
    String* lib = String::create(state_, "blah");
    String* name = String::create(state_, "ruby_version");

    TS_ASSERT_THROWS(NativeLibrary::find_symbol(state_, name, lib),
                     RubyException);
  }

  void test_find_symbol_in_library_throws_on_nonexisting_symbol() {
    if(lib_name_) {
      String* lib = String::create(state_, lib_name_);
      String* name = String::create(state_, "python_version");

      TS_ASSERT_THROWS(NativeLibrary::find_symbol(state_, name, lib),
                       RubyException);
    }
  }

};

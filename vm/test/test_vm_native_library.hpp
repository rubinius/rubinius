#include "vm/test/test.hpp"

#include <cstdlib>

#include "native_libraries.hpp"
#include "builtin/exception.hpp"

class TestNativeLibrary : public CxxTest::TestSuite, public VMTest {
public:

  const char* lib_name;

  void setUp() {
    create();
    lib_name = ::getenv("LIBRUBY");
  }

  void tearDown() {
    destroy();
  }

  void test_find_symbol_in_this_process() {
    String* name = String::create(state, "strlen");      /* libc */

    TS_ASSERT(NativeLibrary::find_symbol(state, name, Qnil));
  }

  void test_find_symbol_in_this_process_with_unloaded_library() {
    String* name = String::create(state, "ruby_version"); /* libruby.1.8 */

    TS_ASSERT(!NativeLibrary::find_symbol(state, name, Qnil, false));
    TS_ASSERT_THROWS(NativeLibrary::find_symbol(state, name, Qnil),
                     RubyException);
  }

  void test_find_symbol_in_this_process_with_nonexisting_symbol() {
    String* name = String::create(state, "nonesuch_just_made__u___p__yep____");

    TS_ASSERT(!NativeLibrary::find_symbol(state, name, Qnil, false));
    TS_ASSERT_THROWS(NativeLibrary::find_symbol(state, name, Qnil),
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
    if(lib_name) {
      String* lib = String::create(state, lib_name);
      String* name = String::create(state, "ruby_version");

      TS_ASSERT(NativeLibrary::find_symbol(state, name, lib));
    }
  }

  void test_find_symbol_in_library_with_nonexisting_library() {
    String* lib = String::create(state, "blah");
    String* name = String::create(state, "ruby_version");

    TS_ASSERT(!NativeLibrary::find_symbol(state, name, lib, false));
    TS_ASSERT_THROWS(NativeLibrary::find_symbol(state, name, lib),
                     RubyException);
  }

  void test_find_symbol_in_library_with_nonexisting_symbol() {
    if(lib_name) {
      String* lib = String::create(state, lib_name);
      String* name = String::create(state, "python_version");

      TS_ASSERT(!NativeLibrary::find_symbol(state, name, lib, false));
      TS_ASSERT_THROWS(NativeLibrary::find_symbol(state, name, lib),
                       RubyException);
    }
  }

};

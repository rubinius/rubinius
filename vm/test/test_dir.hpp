#include "vm/test/test.hpp"

#include "builtin/dir.hpp"
#include "builtin/string.hpp"

#include <stdio.h>
#include <sys/stat.h>

class TestDir : public CxxTest::TestSuite, public VMTest {
public:

  Dir *d;
  void setUp() {
    create();
    d = Dir::create(state);
  }

  void tearDown() {
    if(!d->closed_p(state)->true_p()) d->close(state);
    destroy();
  }

  char* make_directory() {
    char* templ = strdup("/tmp/rubinius_TestDir.XXXXXX");
    return mkdtemp(templ);
  }

  void remove_directory(char *dir) {
    rmdir(dir);
    free(dir);
  }

  void test_create() {
    TS_ASSERT(kind_of<Dir>(d));
  }

  void test_open() {
    String* path = String::create(state, ".");
    TS_ASSERT_EQUALS(d->open(state, path), cTrue);
  }

  void test_close() {
    String* path = String::create(state, ".");
    d->open(state, path);
    TS_ASSERT_EQUALS(d->close(state), cNil);
  }

  void test_closed_p() {
    TS_ASSERT_EQUALS(d->closed_p(state), cTrue);
    String* path = String::create(state, ".");
    d->open(state, path);
    TS_ASSERT_EQUALS(d->closed_p(state), cFalse);
  }

  void test_read() {
    char *dir = make_directory();
    String* path = String::create(state, dir);
    d->open(state, path);
    String* name = as<String>(d->read(state));
    TS_ASSERT_EQUALS(name->c_str(state)[0], '.');
    remove_directory(dir);
  }

  void test_read_returns_nil_when_no_more_entries() {
    char *dir = make_directory();
    String* path = String::create(state, dir);
    d->open(state, path);
    d->read(state);
    d->read(state);
    TS_ASSERT(d->read(state)->nil_p());
    remove_directory(dir);
  }

  void test_control_rewinds_read_location() {
    char *dir = make_directory();
    String* path = String::create(state, dir);
    d->open(state, path);
    d->read(state);
    d->read(state);
    TS_ASSERT(d->read(state)->nil_p());
    d->control(state, Fixnum::from(1), Fixnum::from(0));
    String* name = as<String>(d->read(state));
    TS_ASSERT_EQUALS(name->c_str(state)[0], '.');
    remove_directory(dir);
  }

  void test_control_seeks_to_a_known_position() {
    char *dir = make_directory();
    String* path = String::create(state, dir);
    d->open(state, path);
    d->read(state);
    Fixnum* pos = force_as<Fixnum>(d->control(state, Fixnum::from(2), Fixnum::from(0)));
    String* first = as<String>(d->read(state));

    d->control(state, Fixnum::from(0), pos);
    String* second = as<String>(d->read(state));
    TS_ASSERT_EQUALS(first->byte_size(), second->byte_size());
    TS_ASSERT_SAME_DATA(first, second, first->byte_size());
    remove_directory(dir);
  }
};

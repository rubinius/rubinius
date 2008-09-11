#include "builtin/io.hpp"
#include "builtin/string.hpp"

#include <cstdio>
#include <sys/stat.h>
#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestIO : public CxxTest::TestSuite {
  public:

  VM *state;
  IO* io;
  char *fname;
  int fd;

  void setUp() {
    state = new VM();
    fd = make_io();
    io = IO::create(state, fd);
  }

  void tearDown() {
    remove_io(fd);
    delete state;
  }

  int make_io() {
    fname = tmpnam(NULL);
    return open(fname, O_CREAT | O_RDWR | O_TRUNC);
  }

  void remove_io(int fd) {
    close(fd);
    unlink(fname);
  }

  void test_create() {
    TS_ASSERT_EQUALS(io->descriptor()->to_native(), fd);
  }

  void test_write() {
    char buf[4];

    String* s = String::create(state, "abdc");
    io->write(state, s);

    lseek(fd, 0, SEEK_SET);
    TS_ASSERT_EQUALS(::read(fd, buf, 4U), 4);
    TS_ASSERT_SAME_DATA(buf, "abdc", 4);
  }

  void test_query() {
    TS_ASSERT_EQUALS(Qnil, io->query(state, state->symbol("unknown")));

    io->descriptor(state, Fixnum::from(-1));
    TS_ASSERT_THROWS(io->query(state, state->symbol("tty?")), const PrimitiveFailed &);
  }

  void test_query_tty() {
    SYMBOL tty_p = state->symbol("tty?");
    TS_ASSERT_EQUALS(Qfalse, io->query(state, tty_p));

    IO* rb_stdout = try_as<IO>(G(object)->get_const(state, "STDOUT"));
    TS_ASSERT_EQUALS(Qtrue, rb_stdout->query(state, tty_p));
  }

  void test_query_ttyname() {
    IO* rb_stdout = ((IO*)G(object)->get_const(state, "STDOUT"));
    String* tty = try_as<String>(rb_stdout->query(state, state->symbol("ttyname")));

    // TODO: /dev/ttyxxx won't be portable to e.g. windoze
    TS_ASSERT(tty);
  }
};

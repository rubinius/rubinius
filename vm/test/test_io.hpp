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
    TS_ASSERT_EQUALS(io->descriptor->to_native(), fd);
  }

  void test_write() {
    char buf[4];

    String* s = String::create(state, "abdc");
    io->write(state, s);

    lseek(fd, 0, SEEK_SET);
    TS_ASSERT_EQUALS(::read(fd, buf, 4U), 4);
    TS_ASSERT_SAME_DATA(buf, "abdc", 4);
  }
};

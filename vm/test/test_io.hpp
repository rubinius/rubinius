#include "vm/test/test.hpp"

#include "builtin/io.hpp"
#include "builtin/string.hpp"

#include <stdio.h>
#include <sys/stat.h>

class TestIO : public CxxTest::TestSuite, public VMTest {
public:

  IO* io;
  int fd;

  void setUp() {
    create();
    fd = make_io();
    io = IO::create(state, fd);
  }

  void tearDown() {
    remove_io(fd);
    destroy();
  }

  int make_io() {
    char templ[] = "/tmp/rubinius_TestIO.XXXXXX";
    int fd = mkstemp(templ);

    if(fd == -1) {
      throw std::runtime_error(strerror(errno));
    }

    unlink(templ);

    return fd;
  }

  void remove_io(int fd) {
    close(fd);
  }

  void test_create() {
    TS_ASSERT_EQUALS(fd, io->descriptor()->to_native());
    TS_ASSERT_EQUALS(Fixnum::from(0), io->lineno());
    TS_ASSERT(io->eof()->false_p());
    int acc_mode = fcntl(io->to_fd(), F_GETFL);
    TS_ASSERT(acc_mode >= 0);
    TS_ASSERT_EQUALS(Fixnum::from(acc_mode), io->mode());
    TS_ASSERT(kind_of<IOBuffer>(io->ibuffer()));
  }

  void test_allocate() {
    io = IO::allocate(state, G(io));
    TS_ASSERT(io->descriptor()->nil_p());
    TS_ASSERT_EQUALS(Fixnum::from(0), io->lineno());
    TS_ASSERT(io->eof()->false_p());
    TS_ASSERT(io->mode()->nil_p());
    TS_ASSERT(kind_of<IOBuffer>(io->ibuffer()));
  }

  void test_ensure_open() {
    TS_ASSERT(io->ensure_open(state)->nil_p());
    io->descriptor(state, nil<Fixnum>());
    TS_ASSERT_THROWS_ASSERT(io->ensure_open(state), const RubyException &e,
        TS_ASSERT(Exception::io_error_p(state, e.exception)));
    io->descriptor(state, Fixnum::from(-1));
    TS_ASSERT_THROWS_ASSERT(io->ensure_open(state), const RubyException &e,
        TS_ASSERT(Exception::io_error_p(state, e.exception)));
  }

  void test_set_mode() {
    io->mode(state, nil<Fixnum>());
    TS_ASSERT(io->mode()->nil_p());
    io->set_mode(state);
    int acc_mode = fcntl(io->to_fd(), F_GETFL);
    TS_ASSERT(acc_mode >= 0);
    TS_ASSERT_EQUALS(Fixnum::from(acc_mode), io->mode());
  }

  void test_force_read_only() {
    io->force_read_only(state);
    TS_ASSERT((io->mode()->to_native() & O_ACCMODE) == O_RDONLY);
  }

  void test_force_write_only() {
    io->force_write_only(state);
    TS_ASSERT((io->mode()->to_native() & O_ACCMODE) == O_WRONLY);
  }

  void test_write() {
    char buf[4];

    String* s = String::create(state, "abdc");
    io->write(state, s, 0);

    lseek(fd, 0, SEEK_SET);
    TS_ASSERT_EQUALS(::read(fd, buf, 4U), 4);
    TS_ASSERT_SAME_DATA(buf, "abdc", 4);
  }

  void test_query() {
    TS_ASSERT_EQUALS(cNil, io->query(state, state->symbol("unknown")));

    io->descriptor(state, Fixnum::from(-1));
    TS_ASSERT_THROWS_ASSERT(io->query(state, state->symbol("tty?")),
        const RubyException &e,
        TS_ASSERT(Exception::io_error_p(state, e.exception)));
  }

  void test_query_tty() {
    Symbol* tty_p = state->symbol("tty?");
    TS_ASSERT_EQUALS(cFalse, io->query(state, tty_p));
  }

  void test_query_ttyname() {
    IO* rb_stdout = as<IO>(G(object)->get_const(state, "STDOUT"));
    String* tty = try_as<String>(rb_stdout->query(state, state->symbol("ttyname")));

    // TODO: /dev/ttyxxx won't be portable to e.g. windoze
    TS_ASSERT(tty);
  }

  void test_create_buffer() {
    IOBuffer* buf = IOBuffer::create(state, 10);
    Fixnum* zero = Fixnum::from(0);

    TS_ASSERT_EQUALS(zero, buf->start());
    TS_ASSERT_EQUALS(zero, buf->used());
    TS_ASSERT_EQUALS(Fixnum::from(10), buf->total());
    TS_ASSERT_EQUALS(10U, buf->left());
    TS_ASSERT_EQUALS(cFalse, buf->eof());
  }
};

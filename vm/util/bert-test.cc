#include "util/bert.hpp"

void test_nil() {
  const char* buf = "\203h\002d\000\004bertd\000\003nil";
  int size = 16;
  bert::StringReader reader(buf, size);
  bert::Decoder<bert::StringReader> tzr(reader);
  tzr.read_version();
  bert::Value* val = tzr.next_value();

  if(val) {
    val->print(std::cout);
    std::cout << "\n";
  }
}

void test_float() {
  const char* buf = "\203c3.141500000000000e+00\000\000\000\000\000\000\000\000\000\000";
  int size = 33;
  bert::StringReader reader(buf, size);
  bert::Decoder<bert::StringReader> tzr(reader);
  tzr.read_version();
  bert::Value* val = tzr.next_value();

  if(val) {
    val->print(std::cout);
    std::cout << "\n";
  }
}

void test_symbol() {
  const char* buf = "\203d\000\004blah";
  int size = 8;

  bert::StringReader reader(buf, size);
  bert::Decoder<bert::StringReader> tzr(reader);
  tzr.read_version();
  bert::Value* val = tzr.next_value();

  if(val) {
    val->print(std::cout);
    std::cout << "\n";
  } else {
    std::cout << "no value!\n";
  }
}

void test_list() {
  const char* buf = "\203l\000\000\000\002d\000\004namem\000\000\000\004evanj";
  int size = 23;

  bert::StringReader reader(buf, size);
  bert::Decoder<bert::StringReader> tzr(reader);
  tzr.read_version();
  bert::Value* val = tzr.next_value();

  if(val) {
    val->print(std::cout);
    std::cout << "\n";
  } else {
    std::cout << "no value!\n";
  }

}

void test_dict() {
  const char* buf = "\203h\003d\000\004bertd\000\004dictl\000\000\000\001h\002d\000\004namem\000\000\000\004evanj";
  int size = 41;

  bert::StringReader reader(buf, size);
  bert::Decoder<bert::StringReader> tzr(reader);
  tzr.read_version();
  bert::Value* val = tzr.next_value();

  if(val) {
    val->print(std::cout);
    std::cout << "\n";
  } else {
    std::cout << "no value!\n";
  }
}

void test_time() {
  const char* buf = "\203h\005d\000\004bertd\000\004timeb\000\000\004\350b\000\f%3b\000\000\217C";
  int size = 32;

  bert::StringReader reader(buf, size);
  bert::Decoder<bert::StringReader> tzr(reader);
  tzr.read_version();
  bert::Value* val = tzr.next_value();

  if(val) {
    val->print(std::cout);
    std::cout << "\n";
  } else {
    std::cout << "no value!\n";
  }
}

void test_ioreader() {
  const char* buf = "\203d\000\004blah";
  int size = 8;

  int pipes[2];

  assert(pipe(pipes) == 0);

  assert(write(pipes[1], buf, size) == size);

  bert::IOReader reader(pipes[0]);
  bert::Decoder<bert::IOReader> tzr(reader);
  tzr.read_version();

  bert::Value* val = tzr.next_value();

  if(val) {
    val->print(std::cout);
    std::cout << "\n";
  } else {
    std::cout << "no value!\n";
  }
}

void test_write_symbol() {
  const char* buf = "d\000\004blah";
  int size = 8;

  bert::StringReader reader(buf, size);
  bert::Termizer<bert::StringReader> tzr(reader);
  bert::Term* val = tzr.next_term();
  tzr.fill_body(val);

  if(val) val->print();

  char* output = new char[128];
  memset(output, 0, 128);
  bert::StringWriter writer(output, 128);
  bert::Binaryizer<bert::StringWriter> bzr(writer);
  bzr.write_term(val);

  std::cout << "< ";
  for(int i = 0; i < 32; i++) {
    char c = output[i];
    if(c >= 'a' && c <= 'z') {
      std::cout << c << " ";
    } else {
      std::cout << (int)c << " ";
    }
  }
  std::cout << ">\n";

  bert::StringReader reader2(output, 128);
  bert::Termizer<bert::StringReader> tzr2(reader2);
  bert::Term* val2 = tzr2.next_term();
  tzr2.fill_body(val2);

  if(val2) val2->print();
}

void test_encoder() {
  const char* buf = "\203d\000\004blah";
  int size = 8;

  bert::StringReader reader(buf, size);
  bert::Decoder<bert::StringReader> tzr(reader);
  tzr.read_version();
  bert::Value* val = tzr.next_value();

  if(val) {
    val->print(std::cout);
    std::cout << "\n";
  } else {
    std::cout << "no value!\n";
  }

  char* output = new char[128];
  memset(output, 0, 128);
  bert::StringWriter writer(output, 128);
  bert::Encoder<bert::StringWriter> bzr(writer);
  bzr.write_version();
  bzr.write_value(val);

  std::cout << "< ";
  for(int i = 0; i < 32; i++) {
    unsigned char c = output[i];
    if(c >= 'a' && c <= 'z') {
      std::cout << c << " ";
    } else {
      std::cout << (unsigned int)c << " ";
    }
  }
  std::cout << ">\n";

  bert::StringReader reader2(output, 128);
  bert::Decoder<bert::StringReader> tzr2(reader2);
  tzr2.read_version();
  bert::Value* val2 = tzr2.next_value();

  if(val2) val2->print(std::cout);
}

int main(int argc, char **argv) {
  std::cout << "nil: ";
  test_nil();

  std::cout << "float: ";
  test_float();

  std::cout << "ioreader: ";
  test_ioreader();

  std::cout << "list: ";
  test_list();

  std::cout << "dict: ";
  test_dict();

  std::cout << "time: ";
  test_time();

  test_write_symbol();

  test_encoder();
}

#include "compiled_file.hpp"

#include <cxxtest/TestSuite.h>

#include <iostream>
#include <sstream>
#include <fstream>

using namespace rubinius;

class TestCompiledFile : public CxxTest::TestSuite {
public:
#undef state
  VM* state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_load() {
    std::istringstream stream;
    stream.str("!RBIX\n1\naoeu\nt");

    CompiledFile* cf = CompiledFile::load(stream);
    TS_ASSERT_EQUALS(cf->magic, std::string("!RBIX"));
    TS_ASSERT_EQUALS(cf->version, 1);
    TS_ASSERT_EQUALS(cf->sum, std::string("aoeu"));
    TS_ASSERT_EQUALS((long)stream.tellg(), 13);
  }

  void test_body() {
    std::istringstream stream;
    stream.str("!RBIX\n1\naoeu\nt");

    CompiledFile* cf = CompiledFile::load(stream);
    TS_ASSERT_EQUALS(cf->body(state), Qtrue);
  }

  void test_load_file() {
    std::fstream stream("test/fixture.rbc");
    TS_ASSERT(!!stream);

    CompiledFile* cf = CompiledFile::load(stream);
    TS_ASSERT_EQUALS(cf->magic, "!RBIX");

    CompiledMethod* cm = as<CompiledMethod>(cf->body(state));
    TS_ASSERT(cm);

    Task* task = Task::create(state);

    Message msg(state);
    msg.args = 0;
    msg.recv = G(main);

    SET(cm, scope, StaticScope::create(state));
    SET(cm->scope, module, G(object));

    cm->execute(state, task, msg);
    TS_ASSERT_THROWS(task->execute(), Task::Halt);

    Class* cls = try_as<Class>(G(object)->get_const(state, "Blah"));
    TS_ASSERT(cls);

    cm = try_as<CompiledMethod>(cls->method_table->fetch(state, state->symbol("sweet")));

    TS_ASSERT(cm);
  }
};

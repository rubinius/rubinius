#include "compiled_file.hpp"
#include "objectmemory.hpp"
#include "builtin/task.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/symbol.hpp"

#include <cxxtest/TestSuite.h>

#include <iostream>
#include <sstream>
#include <fstream>

using namespace rubinius;

class TestCompiledFile : public CxxTest::TestSuite {
public:

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
    std::fstream stream("vm/test/fixture.rbc_");
    TS_ASSERT(!!stream);

    CompiledFile* cf = CompiledFile::load(stream);
    TS_ASSERT_EQUALS(cf->magic, "!RBIX");

    CompiledMethod* cm = as<CompiledMethod>(cf->body(state));
    TS_ASSERT(cm);

    Task* task = state->new_task();

    Message msg(state);
    msg.set_args(0);
    msg.recv = G(main);
    msg.module = G(object);
    msg.name = state->symbol("moob");
    msg.method = cm;

    cm->scope(state, StaticScope::create(state));
    cm->scope()->module(state, G(object));

    cm->execute(state, task, msg);

    TS_ASSERT_THROWS(state->run_and_monitor(), Task::Halt);

    Class* cls = try_as<Class>(G(object)->get_const(state, "Blah"));
    TS_ASSERT(cls);

    cm = try_as<CompiledMethod>(cls->method_table()->fetch(state, state->symbol("sweet")));

    TS_ASSERT(cm);
  }
};

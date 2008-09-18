#include "builtin/taskprobe.hpp"

#include "builtin/contexts.hpp"
#include "builtin/exception.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"
#include "global_cache.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestTaskProbe : public CxxTest::TestSuite {
  public:

  VM* state;
  TaskProbe* p;

  void setUp() {
    state = new VM();
    p = TaskProbe::create(state);
  }

  void tearDown() {
    delete state;
  }

  void test_parse_env() {
    TS_ASSERT_EQUALS(false, p->parse_env("unrecognizable"));
  }

  void test_parse_env_all() {
    TS_ASSERT(p->parse_env("all"));

    TS_ASSERT(p->enabled_p(PROBE_ADD_METHOD));
    TS_ASSERT(p->enabled_p(PROBE_START_METHOD));
    TS_ASSERT(p->enabled_p(PROBE_LOOKUP_FAILED));
    TS_ASSERT(p->enabled_p(PROBE_EXECUTE_INSTRUCTION));
    TS_ASSERT(p->enabled_p(PROBE_LOAD_RUNTIME));
  }

  void test_parse_env_every() {
    TS_ASSERT(p->parse_env("add_method,start_method,lookup_failed,execute_instruction,load_runtime"));

    TS_ASSERT(p->enabled_p(PROBE_ADD_METHOD));
    TS_ASSERT(p->enabled_p(PROBE_START_METHOD));
    TS_ASSERT(p->enabled_p(PROBE_LOOKUP_FAILED));
    TS_ASSERT(p->enabled_p(PROBE_EXECUTE_INSTRUCTION));
    TS_ASSERT(p->enabled_p(PROBE_LOAD_RUNTIME));
  }

  void test_parse_env_add_method() {
    TS_ASSERT(p->parse_env("add_method"));

    TS_ASSERT(p->enabled_p(PROBE_ADD_METHOD));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_START_METHOD));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_LOOKUP_FAILED));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_EXECUTE_INSTRUCTION));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_LOAD_RUNTIME));
  }

  void test_parse_env_start_method() {
    TS_ASSERT(p->parse_env("start_method"));

    TS_ASSERT(p->enabled_p(PROBE_START_METHOD));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_ADD_METHOD));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_LOOKUP_FAILED));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_EXECUTE_INSTRUCTION));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_LOAD_RUNTIME));
  }

  void test_parse_env_lookup_failed() {
    TS_ASSERT(p->parse_env("lookup_failed"));

    TS_ASSERT(p->enabled_p(PROBE_LOOKUP_FAILED));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_ADD_METHOD));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_START_METHOD));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_EXECUTE_INSTRUCTION));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_LOAD_RUNTIME));
  }

  void test_parse_env_execute_instruction() {
    TS_ASSERT(p->parse_env("execute_instruction"));

    TS_ASSERT(p->enabled_p(PROBE_EXECUTE_INSTRUCTION));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_ADD_METHOD));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_START_METHOD));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_LOOKUP_FAILED));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_LOAD_RUNTIME));
  }

  void test_parse_env_load_runtime() {
    TS_ASSERT(p->parse_env("load_runtime"));

    TS_ASSERT(p->enabled_p(PROBE_LOAD_RUNTIME));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_ADD_METHOD));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_START_METHOD));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_LOOKUP_FAILED));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_EXECUTE_INSTRUCTION));
  }

  void test_enable_prim() {
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_ADD_METHOD));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_START_METHOD));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_LOOKUP_FAILED));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_EXECUTE_INSTRUCTION));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_LOAD_RUNTIME));

    p->enable(state, state->symbol("add_method"));
    TS_ASSERT(p->enabled_p(PROBE_ADD_METHOD));

    p->enable(state, state->symbol("start_method"));
    TS_ASSERT(p->enabled_p(PROBE_START_METHOD));

    p->enable(state, state->symbol("lookup_failed"));
    TS_ASSERT(p->enabled_p(PROBE_LOOKUP_FAILED));

    p->enable(state, state->symbol("execute_instruction"));
    TS_ASSERT(p->enabled_p(PROBE_EXECUTE_INSTRUCTION));

    p->enable(state, state->symbol("load_runtime"));
    TS_ASSERT(p->enabled_p(PROBE_LOAD_RUNTIME));
  }

  void test_disable_prim() {
    p->parse_env("all");

    TS_ASSERT(p->enabled_p(PROBE_ADD_METHOD));
    TS_ASSERT(p->enabled_p(PROBE_START_METHOD));
    TS_ASSERT(p->enabled_p(PROBE_LOOKUP_FAILED));
    TS_ASSERT(p->enabled_p(PROBE_EXECUTE_INSTRUCTION));
    TS_ASSERT(p->enabled_p(PROBE_LOAD_RUNTIME));

    p->disable(state, state->symbol("add_method"));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_ADD_METHOD));

    p->disable(state, state->symbol("start_method"));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_START_METHOD));

    p->disable(state, state->symbol("lookup_failed"));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_LOOKUP_FAILED));

    p->disable(state, state->symbol("execute_instruction"));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_EXECUTE_INSTRUCTION));

    p->disable(state, state->symbol("load_runtime"));
    TS_ASSERT_EQUALS(false, p->enabled_p(PROBE_LOAD_RUNTIME));
  }

  void test_enabled_p_prim() {
    p->parse_env("all");

    TS_ASSERT(p->enabled_p(state, state->symbol("add_method")));
    TS_ASSERT(p->enabled_p(state, state->symbol("start_method")));
    TS_ASSERT(p->enabled_p(state, state->symbol("lookup_failed")));
    TS_ASSERT(p->enabled_p(state, state->symbol("execute_instruction")));
    TS_ASSERT(p->enabled_p(state, state->symbol("load_runtime")));
  }
};

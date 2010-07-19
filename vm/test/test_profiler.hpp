#include "vm/test/test.hpp"

#include "instruments/profiler.hpp"

#include "builtin/symbol.hpp"

class TestProfiler : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_method_to_s_normal() {
    Symbol* meth = state->symbol("meth");
    Symbol* klass = state->symbol("Object");

    profiler::Method method(0U, meth, klass, profiler::kNormal);
    String* name = method.to_s(state);

    TS_ASSERT_SAME_DATA("Object#meth", name->byte_address(), 11);
  }

  void test_method_to_s_singleton() {
    Symbol* meth = state->symbol("meth");
    Symbol* klass = state->symbol("Object");

    profiler::Method method(0U, meth, klass, profiler::kSingleton);
    String* name = method.to_s(state);

    TS_ASSERT_SAME_DATA("Object.meth", name->byte_address(), 11);
  }

  void test_method_to_s_block() {
    Symbol* meth = state->symbol("meth");
    Symbol* klass = state->symbol("Object");

    profiler::Method method(0U, meth, klass, profiler::kBlock);
    String* name = method.to_s(state);

    TS_ASSERT_EQUALS(std::string("Object::meth<0> {}"), std::string(name->c_str()));
  }

  void test_module_name() {
    Symbol* name = state->symbol("ModName");

    Module* mod = Module::create(state);
    mod->name(state, name);

    IncludedModule* im = IncludedModule::create(state);
    im->module(state, mod);

    profiler::Profiler prof(state);

    TS_ASSERT_EQUALS(name, prof.module_name(im));
    TS_ASSERT_EQUALS(name, prof.module_name(mod));
  }

  void test_enter_block() {
    Symbol* meth = state->symbol("meth");
    CompiledMethod* cm = CompiledMethod::create(state);
    cm->name(state, meth);

    Symbol* name = state->symbol("ModName");
    Module* mod = Module::create(state);
    mod->name(state, name);

    state->shared.enable_profiling(state);
    profiler::Profiler* prof = state->profiler();

    profiler::MethodEntry* me1 = new profiler::MethodEntry(state, meth, mod, cm);
    TS_ASSERT_EQUALS(prof->methods_.size(), 1U);

    TS_ASSERT_EQUALS(me1->method_, prof->methods_.find(me1->method_->id())->second);

    mod = G(object)->metaclass(state);
    profiler::MethodEntry* me2 = new profiler::MethodEntry(state, meth, mod, cm);
    TS_ASSERT_EQUALS(prof->methods_.size(), 2U);

    TS_ASSERT_EQUALS(me2->method_, prof->methods_.find(me2->method_->id())->second);

    delete me1;
    delete me2;
  }

  void test_enter_method() {
    Symbol* meth = state->symbol("meth");

    CompiledMethod* cm = CompiledMethod::create(state);
    cm->name(state, meth);

    state->shared.enable_profiling(state);
    profiler::Profiler* prof = state->profiler();

    Dispatch dis(meth, G(object), cm);
    Arguments args;

    profiler::MethodEntry* me1 = new profiler::MethodEntry(state, dis, args, cm);
    TS_ASSERT_EQUALS(prof->methods_.size(), 1U);

    TS_ASSERT_EQUALS(me1->method_, prof->methods_.find(me1->method_->id())->second);

    dis.module = G(object)->metaclass(state);
    profiler::MethodEntry* me2 = new profiler::MethodEntry(state, dis, args);
    TS_ASSERT_EQUALS(prof->methods_.size(), 2U);

    TS_ASSERT_EQUALS(me2->method_, prof->methods_.find(me2->method_->id())->second);

    delete me1;
    delete me2;
  }
};

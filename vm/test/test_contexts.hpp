#include "vm.hpp"
#include "objectmemory.hpp"
#include "ffi.hpp"
#include "builtin/contexts.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/tuple.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestContexts : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_create() {
    MethodContext* ctx = MethodContext::create(state, 10);
    TS_ASSERT(kind_of<MethodContext>(ctx));
    TS_ASSERT(ctx->stack_size >= 10);
    TS_ASSERT_EQUALS(ctx->ip, 0);
  }

  void test_line() {
    MethodContext* ctx = MethodContext::create(state, 10);
    ctx->ip = 0;
    ctx->cm(state, CompiledMethod::create(state));

    ctx->cm()->lines(state, Tuple::from(state, 1,
                                      Tuple::from(state, 3,
                                             Fixnum::from(0),
                                             Fixnum::from(20),
                                             Fixnum::from(10))));

    TS_ASSERT_EQUALS(10, ctx->line(state));
  }

  void test_recycle() {
    MethodContext* ctx = MethodContext::create(state, 10);

    TS_ASSERT(state->om->context_on_stack_p(ctx));
    TS_ASSERT(!state->om->context_referenced_p(ctx));

    TS_ASSERT(ctx->recycle(state));

    MethodContext* ctx2 = MethodContext::create(state, 10);
    TS_ASSERT_EQUALS(ctx, ctx2);
  }

  void test_recycle_ignores_mature_contexts() {
    MethodContext* ctx = MethodContext::create(state, 10);
    ctx->zone = MatureObjectZone; // GROSS
    TS_ASSERT(!ctx->recycle(state));
  }

  void test_reference() {
    MethodContext* ctx = MethodContext::create(state, 10);
    TS_ASSERT(!state->om->context_referenced_p(ctx));
    TS_ASSERT_EQUALS(ctx->klass_, Qnil);

    ctx->reference(state);

    TS_ASSERT_EQUALS(ctx->klass(), G(methctx));
    TS_ASSERT(state->om->context_referenced_p(ctx));

    MethodContext* ctx2 = MethodContext::create(state, 10);
    TS_ASSERT(ctx != ctx2);
  }

  void test_dup() {
    // Create a realistic MethodContext
    // Is there a better way to do this?
    Task* task = Task::create(state);

    // create a target CM
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq(state, InstructionSequence::create(state, 1));
    target->iseq()->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_ret));
    target->total_args(state, Fixnum::from(0));
    target->required_args(state, target->total_args());
    target->stack_size(state, Fixnum::from(10));
    target->formalize(state);

    // create a containing CM
    CompiledMethod* cm = CompiledMethod::create(state);
    cm->iseq(state, InstructionSequence::create(state, 10));
    cm->stack_size(state, Fixnum::from(10));
    cm->local_count(state, Fixnum::from(0));
    cm->literals(state, Tuple::create(state, 10));

    Symbol* name = state->symbol("blah");
    G(true_class)->method_table()->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    cm->literals()->put(state, 0, ss);

    cm->formalize(state);

    MethodContext* ctx = MethodContext::create(state, Qnil, cm);
    task->make_active(ctx);
    task->push(Qtrue);

    // Dup right before we run so we can compare later
    MethodContext* dup = ctx->dup(state);

    // Compare the dup'd with the original
    TS_ASSERT_EQUALS(ctx->sender(), dup->sender());
    TS_ASSERT_EQUALS(dup, dup->home());
    TS_ASSERT_EQUALS(ctx->self(), dup->self());
    TS_ASSERT_EQUALS(ctx->cm(), dup->cm());
    TS_ASSERT_EQUALS(ctx->module(), dup->module());
    TS_ASSERT_EQUALS(ctx->block(), dup->block());
    TS_ASSERT_EQUALS(ctx->name(), dup->name());

    TS_ASSERT_EQUALS(ctx->vmm, dup->vmm);
    TS_ASSERT_EQUALS(ctx->ip, dup->ip);
    TS_ASSERT_EQUALS(ctx->args, dup->args);
    TS_ASSERT_EQUALS(ctx->stack_size, dup->stack_size);

    TS_ASSERT_SAME_DATA(&ctx->js, &dup->js, sizeof(dup->js));
  }
};


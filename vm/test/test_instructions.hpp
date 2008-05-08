#include "objects.hpp"
#include "builtin_list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestInstructions : public CxxTest::TestSuite {
  public:

#undef state
  VM* state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }
void test_noop() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_noop;
#define run(val) task->execute_stream(stream)
#line 255 "instructions.rb"
    run();
    TS_ASSERT_EQUALS(task->sp, -1);
#undef run
}
void test_push_nil() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_nil;
#define run(val) task->execute_stream(stream)
#line 437 "instructions.rb"
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qnil);
#undef run
}
void test_push_true() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_true;
#define run(val) task->execute_stream(stream)
#line 463 "instructions.rb"
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qtrue);
#undef run
}
void test_push_false() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_false;
#define run(val) task->execute_stream(stream)
#line 489 "instructions.rb"
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qfalse);
#undef run
}
void test_push_int() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_int;
#define run(val) task->execute_stream(stream)
#line 289 "instructions.rb"
    stream[1] = (opcode)47;
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT(kind_of<Fixnum>(stack->at(task->sp)));
    TS_ASSERT_EQUALS(as<Integer>(stack->at(task->sp))->n2i(), 47);
#undef run
}
void test_push_context() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_context;
#define run(val) task->execute_stream(stream)
#line 517 "instructions.rb"
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), task->active);
#undef run
}
void test_push_literal() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_literal;
#define run(val) task->execute_stream(stream)
#line 550 "instructions.rb"
    task->literals = Tuple::from(state, 1, Qtrue);
    stream[1] = (opcode)0;
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qtrue);
#undef run
}
void test_push_self() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_self;
#define run(val) task->execute_stream(stream)
#line 619 "instructions.rb"
    task->self = Qtrue;
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qtrue);
#undef run
}
void test_goto() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_goto;
#define run(val) task->execute_stream(stream)
#line 970 "instructions.rb"
    stream[1] = (opcode)15;
    run();
    TS_ASSERT_EQUALS(task->ip, 15);
#undef run
}
void test_goto_if_false() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_goto_if_false;
#define run(val) task->execute_stream(stream)
#line 1007 "instructions.rb"
    stack->put(state, ++task->sp, Qtrue);
    stream[1] = (opcode)15;
    run();
    TS_ASSERT_EQUALS(task->sp, -1);
    TS_ASSERT_EQUALS(task->ip, 0);

    stack->put(state, ++task->sp, Qfalse);
    run();
    TS_ASSERT_EQUALS(task->ip, 15);
    TS_ASSERT_EQUALS(task->sp, -1);
#undef run
}
void test_goto_if_true() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_goto_if_true;
#define run(val) task->execute_stream(stream)
#line 1051 "instructions.rb"
    stack->put(state, ++task->sp, Qfalse);
    stream[1] = (opcode)15;
    run();
    TS_ASSERT_EQUALS(task->sp, -1);
    TS_ASSERT_EQUALS(task->ip, 0);

    stack->put(state, ++task->sp, Qtrue);
    run();
    TS_ASSERT_EQUALS(task->ip, 15);
    TS_ASSERT_EQUALS(task->sp, -1);
#undef run
}
void test_goto_if_defined() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_goto_if_defined;
#define run(val) task->execute_stream(stream)
#line 1096 "instructions.rb"
    stack->put(state, ++task->sp, Qundef);
    stream[1] = (opcode)15;
    run();
    TS_ASSERT_EQUALS(task->sp, -1);
    TS_ASSERT_EQUALS(task->ip, 0);

    stack->put(state, ++task->sp, Qtrue);
    run();
    TS_ASSERT_EQUALS(task->ip, 15);
    TS_ASSERT_EQUALS(task->sp, -1);
#undef run
}
void test_swap_stack() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_swap_stack;
#define run(val) task->execute_stream(stream)
#line 1136 "instructions.rb"
    stack->put(state, ++task->sp, Qtrue);
    stack->put(state, ++task->sp, Qfalse);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Qfalse);
    TS_ASSERT_EQUALS(stack->at(1), Qtrue);

#undef run
}
void test_dup_top() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_dup_top;
#define run(val) task->execute_stream(stream)
#line 1171 "instructions.rb"
    stack->put(state, ++task->sp, Qtrue);

    run();

    TS_ASSERT_EQUALS(task->sp, 1);
    TS_ASSERT_EQUALS(stack->at(1), Qtrue);
#undef run
}
void test_pop() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_pop;
#define run(val) task->execute_stream(stream)
#line 1203 "instructions.rb"
    stack->put(state, ++task->sp, Qtrue);

    run();

    TS_ASSERT_EQUALS(task->sp, -1);
#undef run
}
void test_set_local() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_set_local;
#define run(val) task->execute_stream(stream)
#line 1243 "instructions.rb"
    task->sp++; /* reserve space */
    stack->put(state, ++task->sp, Qtrue);

    stream[1] = (opcode)0;
    run();

    TS_ASSERT_EQUALS(stack->at(0), Qtrue);
    TS_ASSERT_EQUALS(task->sp, 1);
#undef run
}
void test_push_local() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_local;
#define run(val) task->execute_stream(stream)
#line 649 "instructions.rb"
    stack->put(state, ++task->sp, Qtrue);
    stream[1] = (opcode)0;
    run();
    TS_ASSERT_EQUALS(task->sp, 1);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qtrue);
#undef run
}
void test_push_exception() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_exception;
#define run(val) task->execute_stream(stream)
#line 749 "instructions.rb"
    Exception* exc = Exception::create(state);
    task->exception = exc;
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(task->active->stack->at(task->sp), exc);
#undef run
}
void test_make_array() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_make_array;
#define run(val) task->execute_stream(stream)
#line 1362 "instructions.rb"
    stack->put(state, ++task->sp, Qtrue);
    stack->put(state, ++task->sp, Qfalse);

    stream[1] = 2;
    run();

    TS_ASSERT_EQUALS(task->sp, 0);
    Array* ary = as<Array>(stack->at(task->sp));
    TS_ASSERT_EQUALS(ary->get(state, 0), Qtrue);
    TS_ASSERT_EQUALS(ary->get(state, 1), Qfalse);
#undef run
}
void test_set_ivar() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_set_ivar;
#define run(val) task->execute_stream(stream)
#line 1651 "instructions.rb"
    SYMBOL name = state->symbol("@blah");
    task->self = Qtrue;
    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    stack->put(state, ++task->sp, Qfalse);
    run();

    TS_ASSERT_EQUALS(Qtrue->get_ivar(state, name), Qfalse);
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qfalse);
#undef run
}
void test_push_ivar() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_ivar;
#define run(val) task->execute_stream(stream)
#line 841 "instructions.rb"
    SYMBOL name = state->symbol("@blah");
    task->self = Qtrue;
    task->self->set_ivar(state, name, Qtrue);
    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qtrue);
#undef run
}
void test_push_const() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_const;
#define run(val) task->execute_stream(stream)
#line 1699 "instructions.rb"
    Module* parent = state->new_module("Parent");
    Module* child =  state->new_module("Parent::Child");

    StaticScope* ps = StaticScope::create(state);
    SET(ps, module, parent);
    ps->parent = (StaticScope*)Qnil;

    StaticScope* cs = StaticScope::create(state);
    SET(cs, module, child);
    SET(cs, parent, ps);

    SET(cm, scope, cs);

    SYMBOL name = state->symbol("Number");
    parent->set_const(state, name, Object::i2n(3));

    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(stack->at(0), Object::i2n(3));

#undef run
}
void test_set_const() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_set_const;
#define run(val) task->execute_stream(stream)
#line 1801 "instructions.rb"
    Module* parent = state->new_module("Parent");

    StaticScope* ps = StaticScope::create(state);
    SET(ps, module, parent);
    ps->parent = (StaticScope*)Qnil;

    SET(cm, scope, ps);
    SYMBOL name = state->symbol("Age");

    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    stack->put(state, ++task->sp, Object::i2n(3));
    run();

    TS_ASSERT_EQUALS(parent->get_const(state, name), Object::i2n(3));
#undef run
}
void test_set_const_at() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_set_const_at;
#define run(val) task->execute_stream(stream)
#line 1849 "instructions.rb"
    SYMBOL name = state->symbol("Age");
    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    stack->put(state, ++task->sp, G(true_class));
    stack->put(state, ++task->sp, Qtrue);

    run();

    TS_ASSERT_EQUALS(G(true_class)->get_const(state, name), Qtrue);

#undef run
}
void test_find_const() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_find_const;
#define run(val) task->execute_stream(stream)
#line 1762 "instructions.rb"
    SYMBOL name = state->symbol("Number");
    G(true_class)->set_const(state, name, Object::i2n(3));

    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    stack->put(state, ++task->sp, G(true_class));

    run();

    TS_ASSERT_EQUALS(stack->at(0), Object::i2n(3));
#undef run
}
void test_attach_method() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_attach_method;
#define run(val) task->execute_stream(stream)
#line 2186 "instructions.rb"
    SYMBOL name = state->symbol("blah");
    task->literals->put(state, 0, name);

    stack->put(state, ++task->sp, cm);
    stack->put(state, ++task->sp, Qtrue);

    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(Qtrue->metaclass(state)->method_table->fetch(state, name), cm);
#undef run
}
void test_add_method() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_add_method;
#define run(val) task->execute_stream(stream)
#line 2239 "instructions.rb"
    SYMBOL name = state->symbol("blah");
    task->literals->put(state, 0, name);

    stack->put(state, ++task->sp, cm);
    stack->put(state, ++task->sp, G(true_class));

    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(G(true_class)->method_table->fetch(state, name), cm);
#undef run
}
void test_open_class() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_open_class;
#define run(val) task->execute_stream(stream)
#line 2004 "instructions.rb"
    SYMBOL name = state->symbol("C");

    StaticScope* ps = StaticScope::create(state);
    SET(ps, module, G(true_class));
    ps->parent = (StaticScope*)Qnil;
    SET(cm, scope, ps);

    stack->put(state, ++task->sp, Qnil);

    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT(kind_of<Class>(G(true_class)->get_const(state, name)));
#undef run
}
void test_open_class_under() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_open_class_under;
#define run(val) task->execute_stream(stream)
#line 1942 "instructions.rb"
    SYMBOL name = state->symbol("C");
    stack->put(state, ++task->sp, G(true_class));
    stack->put(state, ++task->sp, Qnil);

    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT(kind_of<Class>(G(true_class)->get_const(state, name)));
#undef run
}
void test_open_module() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_open_module;
#define run(val) task->execute_stream(stream)
#line 2093 "instructions.rb"
    SYMBOL name = state->symbol("C");

    StaticScope* ps = StaticScope::create(state);
    SET(ps, module, G(true_class));
    ps->parent = (StaticScope*)Qnil;
    SET(cm, scope, ps);

    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT(kind_of<Module>(G(true_class)->get_const(state, name)));
#undef run
}
void test_open_module_under() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_open_module_under;
#define run(val) task->execute_stream(stream)
#line 2052 "instructions.rb"
    SYMBOL name = state->symbol("C");
    stack->put(state, ++task->sp, G(true_class));

    task->literals->put(state, 0, name);
    stream[1] = (opcode)0;

    run();

    TS_ASSERT(kind_of<Module>(G(true_class)->get_const(state, name)));
#undef run
}
void test_cast_tuple() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_cast_tuple;
#define run(val) task->execute_stream(stream)
#line 1475 "instructions.rb"
    Array* custom = Array::create(state, 1);
    custom->set(state, 0, Qtrue);
    stack->put(state, ++task->sp, custom);
    run();

    Tuple* tup = as<Tuple>(stack->at(0));
    TS_ASSERT_EQUALS(tup->at(0), Qtrue);


    stack->put(state, 0, Qfalse);
    run();

    tup = as<Tuple>(stack->at(0));
    TS_ASSERT_EQUALS(tup->at(0), Qfalse);

    tup = Tuple::create(state, 1);
    stack->put(state, 0, tup);
    run();

    TS_ASSERT_EQUALS(tup, stack->at(0));
#undef run
}
void test_send_method() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_send_method;
#define run(val) task->execute_stream(stream)
#line 2289 "instructions.rb"
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq = InstructionSequence::create(state, 0);
    target->total_args = Object::i2n(0);
    target->required_args = target->total_args;
    target->stack_size = Object::i2n(10);

    SYMBOL name = state->symbol("blah");
    G(true_class)->method_table->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    task->literals->put(state, 0, ss);
    stack->put(state, ++task->sp, Qtrue);

    stream[1] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(task->active->cm, target);
    TS_ASSERT_EQUALS(task->active->args, 0);
    TS_ASSERT_EQUALS(task->self, Qtrue);
#undef run
}
void test_send_stack() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_send_stack;
#define run(val) task->execute_stream(stream)
#line 2381 "instructions.rb"
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq = InstructionSequence::create(state, 0);
    target->total_args = Object::i2n(1);
    target->required_args = target->total_args;
    target->stack_size = Object::i2n(1);

    SYMBOL name = state->symbol("blah");
    G(true_class)->method_table->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    task->literals->put(state, 0, ss);
    stack->put(state, ++task->sp, Qtrue);
    stack->put(state, ++task->sp, Object::i2n(3));

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    run();

    TS_ASSERT_EQUALS(task->active->cm, target);
    TS_ASSERT_EQUALS(task->active->args, 1);
    TS_ASSERT_EQUALS(task->stack->at(0), Object::i2n(3));
    TS_ASSERT_EQUALS(task->self, Qtrue);
#undef run
}
void test_send_stack_with_block() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_send_stack_with_block;
#define run(val) task->execute_stream(stream)
#line 2451 "instructions.rb"
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq = InstructionSequence::create(state, 0);
    target->total_args = Object::i2n(1);
    target->required_args = target->total_args;
    target->stack_size = Object::i2n(1);

    SYMBOL name = state->symbol("blah");
    G(true_class)->method_table->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    task->literals->put(state, 0, ss);
    stack->put(state, ++task->sp, Qtrue);
    stack->put(state, ++task->sp, Object::i2n(3));

    BlockEnvironment* be = BlockEnvironment::under_context(state, target, task->active, task->active);
    stack->put(state, ++task->sp, be);

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    run();

    TS_ASSERT_EQUALS(task->active->cm, target);
    TS_ASSERT_EQUALS(task->active->args, 1);
    TS_ASSERT_EQUALS(task->stack->at(0), Object::i2n(3));
    TS_ASSERT_EQUALS(task->active->block, be);
    TS_ASSERT_EQUALS(task->self, Qtrue);
#undef run
}
void test_send_stack_with_splat() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_send_stack_with_splat;
#define run(val) task->execute_stream(stream)
#line 2542 "instructions.rb"
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq = InstructionSequence::create(state, 0);
    target->total_args = Object::i2n(2);
    target->required_args = target->total_args;
    target->stack_size = Object::i2n(2);

    SYMBOL name = state->symbol("blah");
    G(true_class)->method_table->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    task->literals->put(state, 0, ss);
    stack->put(state, ++task->sp, Qtrue);
    stack->put(state, ++task->sp, Object::i2n(3));

    Array* splat = Array::create(state, 1);
    splat->set(state, 0, Object::i2n(47));
    stack->put(state, ++task->sp, splat);

    BlockEnvironment* be = BlockEnvironment::under_context(state, target, task->active, task->active);
    stack->put(state, ++task->sp, be);

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    run();

    TS_ASSERT_EQUALS(task->active->cm, target);
    TS_ASSERT_EQUALS(task->active->args, 2);
    TS_ASSERT_EQUALS(task->stack->at(0), Object::i2n(3));
    TS_ASSERT_EQUALS(task->stack->at(1), Object::i2n(47));
    TS_ASSERT_EQUALS(task->active->block, be);
    TS_ASSERT_EQUALS(task->self, Qtrue);
#undef run
}
void test_send_super_stack_with_block() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_send_super_stack_with_block;
#define run(val) task->execute_stream(stream)
#line 2617 "instructions.rb"
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq = InstructionSequence::create(state, 0);
    target->total_args = Object::i2n(1);
    target->required_args = target->total_args;
    target->stack_size = Object::i2n(1);

    Class* parent = state->new_class("Parent", 1);
    Class* child =  state->new_class("Child", parent, 1);

    SYMBOL name = state->symbol("blah");
    parent->method_table->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    OBJECT obj = state->new_object(child);
    task->self = obj;

    StaticScope *sc = StaticScope::create(state);
    SET(sc, module, child);

    SET(cm, scope, sc);

    task->literals->put(state, 0, ss);
    stack->put(state, ++task->sp, obj);
    stack->put(state, ++task->sp, Object::i2n(3));

    BlockEnvironment* be = BlockEnvironment::under_context(state, target, task->active, task->active);
    stack->put(state, ++task->sp, be);

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    run();

    TS_ASSERT_EQUALS(task->active->cm, target);
    TS_ASSERT_EQUALS(task->active->args, 1);
    TS_ASSERT_EQUALS(task->stack->at(0), Object::i2n(3));
    TS_ASSERT_EQUALS(task->active->block, be);
    TS_ASSERT_EQUALS(task->self, obj);
#undef run
}
void test_send_super_stack_with_splat() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_send_super_stack_with_splat;
#define run(val) task->execute_stream(stream)
#line 2710 "instructions.rb"
    CompiledMethod* target = CompiledMethod::create(state);
    target->iseq = InstructionSequence::create(state, 0);
    target->total_args = Object::i2n(2);
    target->required_args = target->total_args;
    target->stack_size = Object::i2n(2);

    Class* parent = state->new_class("Parent", 1);
    Class* child =  state->new_class("Child", parent, 1);

    SYMBOL name = state->symbol("blah");
    parent->method_table->store(state, name, target);
    SendSite* ss = SendSite::create(state, name);

    OBJECT obj = state->new_object(child);
    task->self = obj;

    StaticScope *sc = StaticScope::create(state);
    SET(sc, module, child);

    SET(cm, scope, sc);

    task->literals->put(state, 0, ss);
    stack->put(state, ++task->sp, obj);
    stack->put(state, ++task->sp, Object::i2n(3));

    Array* splat = Array::create(state, 1);
    splat->set(state, 0, Object::i2n(47));
    stack->put(state, ++task->sp, splat);

    BlockEnvironment* be = BlockEnvironment::under_context(state, target, task->active, task->active);
    stack->put(state, ++task->sp, be);

    stream[1] = (opcode)0;
    stream[2] = (opcode)1;

    run();

    TS_ASSERT_EQUALS(task->active->cm, target);
    TS_ASSERT_EQUALS(task->active->args, 2);
    TS_ASSERT_EQUALS(task->stack->at(0), Object::i2n(3));
    TS_ASSERT_EQUALS(task->stack->at(1), Object::i2n(47));
    TS_ASSERT_EQUALS(task->active->block, be);
    TS_ASSERT_EQUALS(task->self, obj);
#undef run
}
void test_push_block() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_block;
#define run(val) task->execute_stream(stream)
#line 811 "instructions.rb"
    BlockEnvironment* be = BlockEnvironment::under_context(state, cm, task->active, task->active);
    task->active->block = be;
    run();

    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), be);
#undef run
}
void test_clear_exception() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_clear_exception;
#define run(val) task->execute_stream(stream)
#line 777 "instructions.rb"
    task->exception = Exception::create(state);
    run();
    TS_ASSERT_EQUALS(task->exception, Qnil);
#undef run
}
void test_cast_array() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_cast_array;
#define run(val) task->execute_stream(stream)
#line 1414 "instructions.rb"
    stack->put(state, ++task->sp, Qtrue);
    run();

    Array* ary = as<Array>(stack->at(0));
    TS_ASSERT_EQUALS(ary->get(state, 0), Qtrue);

    stack->put(state, 0, Tuple::from(state, 1, Qfalse));
    run();

    ary = as<Array>(stack->at(0));
    TS_ASSERT_EQUALS(ary->get(state, 0), Qfalse);

    Array* custom = Array::create(state, 1);
    stack->put(state, 0, custom);
    run();

    TS_ASSERT_EQUALS(stack->at(0), custom);
#undef run
}
void test_push_cpath_top() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_cpath_top;
#define run(val) task->execute_stream(stream)
#line 1888 "instructions.rb"
    run();
    TS_ASSERT_EQUALS(stack->at(task->sp), G(object));
#undef run
}
void test_push_my_field() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_my_field;
#define run(val) task->execute_stream(stream)
#line 880 "instructions.rb"
    Tuple* tup = Tuple::create(state, 3);
    tup->put(state, 0, Qtrue);

    task->self = tup;

    stream[1] = (opcode)0;

    TS_ASSERT_THROWS(run(), std::runtime_error);

    Class* cls = state->new_class("Blah");

    task->self = cls;

    stream[1] = (opcode)2;

    task->sp = -1;
    run();

    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), state->symbol("Blah"));
#undef run
}
void test_store_my_field() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_store_my_field;
#define run(val) task->execute_stream(stream)
#line 929 "instructions.rb"
    Tuple* tup = Tuple::create(state, 3);
    tup->put(state, 0, Qnil);

    task->self = tup;

    stack->put(state, ++task->sp, Qtrue);
    stream[1] = (opcode)0;
    run();

    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qtrue);
    TS_ASSERT_EQUALS(tup->at(0), Qtrue);
#undef run
}
void test_open_metaclass() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_open_metaclass;
#define run(val) task->execute_stream(stream)
#line 2140 "instructions.rb"
    Tuple* tup = Tuple::create(state, 1);
    stack->put(state, ++task->sp, tup);

    run();

    TS_ASSERT_EQUALS(stack->at(0), tup->metaclass(state));
#undef run
}
void test_push_local_depth() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_push_local_depth;
#define run(val) task->execute_stream(stream)
#line 697 "instructions.rb"
    BlockEnvironment* be = BlockEnvironment::under_context(state, cm, task->active, task->active);
    BlockContext* bc = be->create_context(state);

    BlockEnvironment* be2 = BlockEnvironment::under_context(state, cm, bc, bc);
    BlockContext* bc2 = be2->create_context(state);

    task->make_active(bc2);

    bc->locals()->put(state, 0, Qtrue);

    stream[1] = (opcode)1;
    stream[2] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(task->active->stack->at(task->sp), Qtrue);
#undef run
}
void test_set_local_depth() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_set_local_depth;
#define run(val) task->execute_stream(stream)
#line 1305 "instructions.rb"
    BlockEnvironment* be = BlockEnvironment::under_context(state, cm, task->active, task->active);
    BlockContext* bc = be->create_context(state);

    BlockEnvironment* be2 = BlockEnvironment::under_context(state, cm, bc, bc);
    BlockContext* bc2 = be2->create_context(state);

    task->make_active(bc2);

    task->stack->put(state, ++task->sp, Qtrue);
    bc->locals()->put(state, 0, Qnil);

    stream[1] = (opcode)1;
    stream[2] = (opcode)0;

    run();

    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(task->active->stack->at(task->sp), Qtrue);
    TS_ASSERT_EQUALS(bc->locals()->at(0), Qtrue);
#undef run
}
void test_locate_method() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_locate_method;
#define run(val) task->execute_stream(stream)
#line 2786 "instructions.rb"
    SYMBOL name = state->symbol("blah");
    G(true_class)->method_table->store(state, name, cm);

    stack->put(state, ++task->sp, Qtrue);
    stack->put(state, ++task->sp, name);
    stack->put(state, ++task->sp, Qfalse);

    run();

    TS_ASSERT_EQUALS(stack->at(0), cm);
#undef run
}
void test_set_literal() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_set_literal;
#define run(val) task->execute_stream(stream)
#line 589 "instructions.rb"
    task->literals = Tuple::from(state, 1, Qtrue);
    stream[1] = (opcode)0;
    stack->put(state, ++task->sp, Qtrue);
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT_EQUALS(stack->at(task->sp), Qtrue);
    TS_ASSERT_EQUALS(task->literals->at(0), Qtrue);
#undef run
}
void test_cast_for_single_block_arg() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_cast_for_single_block_arg;
#define run(val) task->execute_stream(stream)
#line 1537 "instructions.rb"
    Tuple* tup = Tuple::create(state, 0);
    stack->put(state, ++task->sp, tup);
    run();

    TS_ASSERT_EQUALS(stack->at(0), Qnil);

    tup = Tuple::from(state, 1, Qtrue);
    stack->put(state, 0, tup);
    run();

    TS_ASSERT_EQUALS(stack->at(0), Qtrue);

    tup = Tuple::from(state, 2, Qtrue, Qfalse);
    stack->put(state, 0, tup);
    run();

    Array* ary = as<Array>(stack->at(0));
    TS_ASSERT_EQUALS(ary->get(state, 0), Qtrue);
    TS_ASSERT_EQUALS(ary->get(state, 1), Qfalse);
#undef run
}
void test_cast_for_multi_block_arg() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_cast_for_multi_block_arg;
#define run(val) task->execute_stream(stream)
#line 1608 "instructions.rb"
    Tuple* tup = Tuple::from(state, 2, Qtrue, Qfalse);
    stack->put(state, ++task->sp, tup);
    run();

    TS_ASSERT_EQUALS(stack->at(0), tup);

    Array* ary = Array::create(state, 1);
    ary->set(state, 0, Object::i2n(1));
    tup = Tuple::from(state, 1, ary);
    stack->put(state, task->sp, tup);
    run();

    tup = as<Tuple>(stack->at(0));
    TS_ASSERT_EQUALS(tup->at(0), Object::i2n(1));
#undef run
}
void test_meta_push_neg_1() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_meta_push_neg_1;
#define run(val) task->execute_stream(stream)
#line 320 "instructions.rb"
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT(kind_of<Fixnum>(stack->at(task->sp)));
    TS_ASSERT_EQUALS(as<Integer>(stack->at(task->sp))->n2i(), -1);
#undef run
}
void test_meta_push_0() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_meta_push_0;
#define run(val) task->execute_stream(stream)
#line 350 "instructions.rb"
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT(kind_of<Fixnum>(stack->at(task->sp)));
    TS_ASSERT_EQUALS(as<Integer>(stack->at(task->sp))->n2i(), 0);
#undef run
}
void test_meta_push_1() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_meta_push_1;
#define run(val) task->execute_stream(stream)
#line 380 "instructions.rb"
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT(kind_of<Fixnum>(stack->at(task->sp)));
    TS_ASSERT_EQUALS(as<Integer>(stack->at(task->sp))->n2i(), 1);
#undef run
}
void test_meta_push_2() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_meta_push_2;
#define run(val) task->execute_stream(stream)
#line 410 "instructions.rb"
    run();
    TS_ASSERT_EQUALS(task->sp, 0);
    TS_ASSERT(kind_of<Fixnum>(stack->at(task->sp)));
    TS_ASSERT_EQUALS(as<Integer>(stack->at(task->sp))->n2i(), 2);
#undef run
}
void test_meta_send_op_plus() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_meta_send_op_plus;
#define run(val) task->execute_stream(stream)
#line 2834 "instructions.rb"
    OBJECT one = Object::i2n(1);
    OBJECT two = Object::i2n(2);

    stack->put(state, ++task->sp, one);
    stack->put(state, ++task->sp, two);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Object::i2n(3));

#undef run
}
void test_meta_send_op_minus() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_meta_send_op_minus;
#define run(val) task->execute_stream(stream)
#line 2882 "instructions.rb"
    OBJECT one = Object::i2n(1);
    OBJECT two = Object::i2n(2);

    stack->put(state, ++task->sp, two);
    stack->put(state, ++task->sp, one);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Object::i2n(1));

#undef run
}
void test_meta_send_op_equal() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_meta_send_op_equal;
#define run(val) task->execute_stream(stream)
#line 2931 "instructions.rb"
    OBJECT one = Object::i2n(1);
    OBJECT two = Object::i2n(2);

    stack->put(state, ++task->sp, two);
    stack->put(state, ++task->sp, one);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Qfalse);

#undef run
}
void test_meta_send_op_lt() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_meta_send_op_lt;
#define run(val) task->execute_stream(stream)
#line 3084 "instructions.rb"
    OBJECT one = Object::i2n(1);
    OBJECT two = Object::i2n(2);

    stack->put(state, ++task->sp, one);
    stack->put(state, ++task->sp, two);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Qtrue);

#undef run
}
void test_meta_send_op_gt() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_meta_send_op_gt;
#define run(val) task->execute_stream(stream)
#line 3134 "instructions.rb"
    OBJECT one = Object::i2n(1);
    OBJECT two = Object::i2n(2);

    stack->put(state, ++task->sp, one);
    stack->put(state, ++task->sp, two);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Qfalse);

#undef run
}
void test_meta_send_op_tequal() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_meta_send_op_tequal;
#define run(val) task->execute_stream(stream)
#line 3034 "instructions.rb"
    OBJECT one = Object::i2n(1);
    OBJECT two = Object::i2n(2);

    stack->put(state, ++task->sp, two);
    stack->put(state, ++task->sp, one);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Qfalse);

#undef run
}
void test_meta_send_op_nequal() {
Task* task = Task::create(state);
CompiledMethod* cm = CompiledMethod::create(state);
cm->iseq = InstructionSequence::create(state, 10);
cm->stack_size = Object::i2n(10);
MethodContext* ctx = task->generate_context(Qnil, cm);
task->make_active(ctx);
Tuple* stack = task->stack; stack += 0;
task->literals = Tuple::create(state, 10);
opcode stream[100];
stream[0] = InstructionSequence::insn_meta_send_op_nequal;
#define run(val) task->execute_stream(stream)
#line 2983 "instructions.rb"
    OBJECT one = Object::i2n(1);
    OBJECT two = Object::i2n(2);

    stack->put(state, ++task->sp, two);
    stack->put(state, ++task->sp, one);

    run();

    TS_ASSERT_EQUALS(stack->at(0), Qtrue);

#undef run
}
  };

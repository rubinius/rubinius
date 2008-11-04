#include "vm.hpp"
#include "objectmemory.hpp"
#include "gc_debug.hpp"

#include <cxxtest/TestSuite.h>

#include <map>
#include <vector>

using namespace rubinius;

class TestVM : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_probe_is_nil() {
    TS_ASSERT_EQUALS(Qnil, state->probe.get());
  }

  void test_symbol_given_cstr() {
    Symbol* sym1 = state->symbol("blah");
    Symbol* sym2 = state->symbol("blah");

    TS_ASSERT_EQUALS(sym1, sym2);
  }

  void test_symbol_given_string() {
    String* str1 = String::create(state, "symbolic");
    String* str2 = String::create(state, "symbolic");

    Symbol* sym1 = state->symbol(str1);
    Symbol* sym2 = state->symbol(str2);

    TS_ASSERT_EQUALS(sym1, sym2);
  }

  void test_symbol_given_std_string() {
    std::string str1("standard");
    std::string str2("standard");

    Symbol* sym1 = state->symbol(str1);
    Symbol* sym2 = state->symbol(str2);

    TS_ASSERT_EQUALS(sym1, sym2);
  }

  void test_new_object_uses_field_count_from_class() {
    Class* cls = state->new_class("Blah", G(object), 3);

    Object* blah = state->new_object(cls);

    TS_ASSERT_EQUALS(blah->num_fields(), 3U);
  }

  void test_globals() {
    TS_ASSERT_EQUALS(state->globals.roots.size(), 121U);
  }

  void test_collection() {
    std::map<int, Object*> objs;

    int index = 0;
    Root* root = static_cast<Root*>(state->globals.roots.head());
    while(root) {
      Object* tmp = root->get();
      if(tmp->reference_p() && tmp->zone == YoungObjectZone) {
        objs[index] = tmp;
      }
      index++;

      root = static_cast<Root*>(root->next());
    }

    //std::cout << "young: " << index << " (" <<
    //  state->om->young.total_objects << ")" << std::endl;

    state->om->collect_young(state->globals.roots);

    index = 0;
    root = static_cast<Root*>(state->globals.roots.head());
    while(root) {
      if(Object* tmp = objs[index]) {
        TS_ASSERT(root->get() != tmp);
      }
      index++;

      root = static_cast<Root*>(root->next());
    }

    HeapDebug hd(state->om);
    hd.walk(state->globals.roots);

    //std::cout << "total: " << hd.seen_objects << " (" <<
    //  state->om->young.total_objects << ")" << std::endl;
  }

  void test_current_thread() {
    Object* current_thread = state->globals.current_thread.get();

    TS_ASSERT_EQUALS(state->current_thread(), current_thread);
  }

  void test_run_best_thread() {
    Thread* cur = Thread::current(state);
    Thread* low = Thread::create(state);

    low->priority(state, Fixnum::from(0));

    cur->wakeup(state);
    low->wakeup(state);

    state->run_best_thread();

    TS_ASSERT_EQUALS(cur, Thread::current(state));
  }

  void test_activate_thread_sets_as_current() {
    Thread* thread = Thread::create(state);

    state->activate_thread(thread);

    TS_ASSERT_EQUALS(thread, Thread::current(state));
  }

  void test_activate_thread_queues_previous_current() {
    Thread* cur = Thread::current(state);
    Thread* thread = Thread::create(state);

    state->activate_thread(thread);

    TS_ASSERT_EQUALS(Qtrue, cur->queued());
  }

  void test_find_and_activate() {
    Thread* cur = Thread::current(state);
    Thread* thread = Thread::create(state);

    thread->wakeup(state);
    state->queue_thread(thread);

    bool ret = state->find_and_activate_thread();
    TS_ASSERT_EQUALS(true, ret);

    TS_ASSERT_EQUALS(Qfalse, thread->queued());
    TS_ASSERT_EQUALS(thread, Thread::current(state));

    TS_ASSERT_EQUALS(Qtrue, cur->queued());
  }

  void test_find_and_activate_none_is_false() {
    Thread* cur = Thread::current(state);

    bool ret = state->find_and_activate_thread();

    TS_ASSERT_EQUALS(false, ret);
    TS_ASSERT_EQUALS(cur, Thread::current(state));
    TS_ASSERT_EQUALS(Qfalse, cur->queued());
  }

  void test_find_and_activate_sleeping_does_not() {
    Thread* cur = Thread::current(state);
    Thread* thread = Thread::create(state);
    Thread* thread2 = Thread::create(state);
    
    thread->wakeup(state);
    thread2->wakeup(state);

    state->queue_thread(thread);
    state->queue_thread(thread2);
    thread->sleep(state, Qtrue);

    bool ret = state->find_and_activate_thread();
    TS_ASSERT_EQUALS(true, ret);

    TS_ASSERT_EQUALS(Qfalse, thread->queued());

    TS_ASSERT_EQUALS(thread2, Thread::current(state));

    TS_ASSERT_EQUALS(Qtrue, cur->queued());
  }

  void test_find_and_activate_dead_does_not() {
    Thread* cur = Thread::current(state);
    Thread* thread = Thread::create(state);
    Thread* thread2 = Thread::create(state);

    thread->wakeup(state);
    thread2->wakeup(state);

    state->queue_thread(thread);
    state->queue_thread(thread2);
    thread->alive(state, Qfalse);

    bool ret = state->find_and_activate_thread();
    TS_ASSERT_EQUALS(true, ret);

    TS_ASSERT_EQUALS(Qfalse, thread->queued());

    TS_ASSERT_EQUALS(thread2, Thread::current(state));

    TS_ASSERT_EQUALS(Qtrue, cur->queued());
  }

  void test_queue_thread() {
    Thread* thread = Thread::create(state);
    thread->priority(state, Fixnum::from(0));
    thread->sleep(state, Qfalse);

    TS_ASSERT_EQUALS(Qfalse, thread->queued());

    List* zeroes = as<List>(state->globals.scheduled_threads->at(state, 0));
    TS_ASSERT_EQUALS(0, zeroes->count()->to_native());

    state->queue_thread(thread);

    TS_ASSERT_EQUALS(Qtrue, thread->queued());
    TS_ASSERT_EQUALS(1, zeroes->count()->to_native());
    TS_ASSERT_EQUALS(thread, as<Thread>(zeroes->shift(state)));
  }

  void test_queue_already_queued_thread_is_noop() {
    Thread* thread = Thread::create(state);
    thread->queued(state, Qtrue);

    state->queue_thread(thread);

    TS_ASSERT_EQUALS(Qtrue, thread->queued());
  }

  void test_queue_sleeping_thread_leaves_sleeping() {
    Thread* thread = Thread::create(state);
    thread->priority(state, Fixnum::from(0));

    TS_ASSERT_EQUALS(Qfalse, thread->queued());
    TS_ASSERT_EQUALS(Qtrue, thread->sleep());

    List* zeroes = as<List>(state->globals.scheduled_threads->at(state, 0));
    TS_ASSERT_EQUALS(0, zeroes->count()->to_native());

    state->queue_thread(thread);

    TS_ASSERT_EQUALS(Qtrue, thread->queued());
    TS_ASSERT_EQUALS(1, zeroes->count()->to_native());
    TS_ASSERT_EQUALS(thread, as<Thread>(zeroes->shift(state)));

    TS_ASSERT_EQUALS(Qtrue, thread->sleep());
  }

  void test_dequeue_thread_sets_queued_false() {
    Thread* thread = Thread::create(state);
    thread->queued(state, Qtrue);

    state->dequeue_thread(thread);
    
    TS_ASSERT_EQUALS(Qfalse, thread->queued());
  }

  void test_dequeue_thread_removes_from_scheduled() {
    Thread* thread = Thread::create(state);
    thread->priority(state, Fixnum::from(0));

    state->queue_thread(thread);

    List* zeroes = as<List>(state->globals.scheduled_threads->at(state, 0));
    TS_ASSERT_EQUALS(1, zeroes->count()->to_native());

    state->dequeue_thread(thread);

    TS_ASSERT_EQUALS(Qfalse, thread->queued());
    TS_ASSERT_EQUALS(0, zeroes->count()->to_native());
  }

};

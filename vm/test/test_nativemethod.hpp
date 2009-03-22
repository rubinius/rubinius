#include "vm/test/test.hpp"

#include "builtin/nativemethod.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

using namespace rubinius;

class TestNativeMethod : public CxxTest::TestSuite, public VMTest {
  public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_immediate_handles() {
    TS_ASSERT_EQUALS(0, cCApiQfalse);
    TS_ASSERT_EQUALS(-1, cCApiQtrue);
    TS_ASSERT_EQUALS(-2, cCApiQnil);
    TS_ASSERT_EQUALS(-3, cCApiQundef);
  }

  void test_handle_constants() {
    TS_ASSERT_EQUALS(1, cHandleOffset);
    TS_ASSERT_EQUALS(-4, cGlobalHandleStart);
  }

  void test_native_method_frame_first_handle() {
    NativeMethodFrame nmf(NULL);

    TS_ASSERT_EQUALS(1, nmf.get_handle(state, Qnil));
  }

  void test_native_method_handle_get_object() {
    NativeMethodFrame nmf(NULL);
    Handle handle = nmf.get_handle(state, Qnil);

    TS_ASSERT_EQUALS(Qnil, nmf.get_object(handle));
  }

  NativeMethodEnvironment* create_native_method_environment() {
    NativeMethodFrame* nmf = new NativeMethodFrame(NULL);
    CallFrame* cf = new CallFrame;
    NativeMethodEnvironment* nme = new NativeMethodEnvironment;

    nme->set_current_call_frame(cf);
    nme->set_current_native_frame(nmf);
    nme->set_state(state);

    return nme;
  }

  void destroy_native_method_environment(NativeMethodEnvironment* nme) {
    delete nme->current_call_frame();
    delete nme->current_native_frame();
    delete nme;
  }

  void test_native_method_environment_first_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();

    TS_ASSERT_EQUALS(1, nme->get_handle(Qnil));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_object() {
    NativeMethodEnvironment* nme = create_native_method_environment();
    Handle handle = nme->get_handle(Qnil);

    TS_ASSERT_EQUALS(Qnil, nme->get_object(handle));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_first_global_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();

    TS_ASSERT_EQUALS(cGlobalHandleStart - cHandleOffset, nme->get_handle_global(Qnil));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_object_global() {
    NativeMethodEnvironment* nme = create_native_method_environment();
    Handle handle = nme->get_handle_global(Qnil);

    TS_ASSERT_EQUALS(Qnil, nme->get_object(handle));

    destroy_native_method_environment(nme);
  }
};

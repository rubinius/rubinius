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

  void test_native_method_frame_first_handle() {
    NativeMethodFrame nmf(NULL);

    TS_ASSERT_EQUALS(CAPI_APPLY_LOCAL_TAG(0),
        nmf.get_handle(state, String::create(state, "test")));
  }

  void test_native_method_frame_get_object() {
    NativeMethodFrame nmf(NULL);
    String* str = String::create(state, "test");
    Integer* id = str->id(state);
    Handle handle = nmf.get_handle(state, str);

    TS_ASSERT_EQUALS(id, nmf.get_object(handle)->id(state));
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

    TS_ASSERT_EQUALS(CAPI_APPLY_LOCAL_TAG(0),
        nme->get_handle(String::create(state, "test")));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_Qfalse_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();

    TS_ASSERT_EQUALS(cCApiHandleQfalse, nme->get_handle(Qfalse));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_Qtrue_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();

    TS_ASSERT_EQUALS(cCApiHandleQtrue, nme->get_handle(Qtrue));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_Qnil_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();

    TS_ASSERT_EQUALS(cCApiHandleQnil, nme->get_handle(Qnil));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_Qundef_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();

    TS_ASSERT_EQUALS(cCApiHandleQundef, nme->get_handle(Qundef));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_Symbol_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();
    Symbol* sym = state->symbol("capi_symbol");

    TS_ASSERT_EQUALS(sym, reinterpret_cast<Symbol*>(nme->get_handle(sym)));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_Fixnum_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();
    Fixnum* fix = Fixnum::from(28);

    TS_ASSERT_EQUALS(fix, reinterpret_cast<Fixnum*>(nme->get_handle(fix)));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_object() {
    NativeMethodEnvironment* nme = create_native_method_environment();
    String* str = String::create(state, "test");
    Integer* id = str->id(state);
    Handle handle = nme->get_handle(str);

    TS_ASSERT_EQUALS(id, nme->get_object(handle)->id(state));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_first_global_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();

    TS_ASSERT_EQUALS(CAPI_APPLY_GLOBAL_TAG(0),
        nme->get_handle_global(String::create(state, "test")));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_object_global() {
    NativeMethodEnvironment* nme = create_native_method_environment();
    String* str = String::create(state, "test");
    Integer* id = str->id(state);
    Handle handle = nme->get_handle_global(str);

    TS_ASSERT_EQUALS(id, nme->get_object(handle)->id(state));

    destroy_native_method_environment(nme);
  }
};

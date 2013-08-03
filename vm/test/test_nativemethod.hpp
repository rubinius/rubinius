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

  void test_native_method_frame_get_object() {
    NativeMethodEnvironment* nme = create_native_method_environment();
    NativeMethodFrame nmf(nme, NULL, NULL);
    String* str = String::create(state, "test");
    Integer* id = str->id(state);
    VALUE handle = nmf.get_handle(state, str);

    TS_ASSERT_EQUALS(id, nmf.get_object(handle)->id(state));
  }

  NativeMethodEnvironment* create_native_method_environment() {
    NativeMethodEnvironment* nme = new NativeMethodEnvironment(state);
    NativeMethodFrame* nmf = new NativeMethodFrame(nme, NULL, NULL);
    CallFrame* cf = new CallFrame;

    nme->set_current_call_frame(cf);
    nme->set_current_native_frame(nmf);

    return nme;
  }

  void destroy_native_method_environment(NativeMethodEnvironment* nme) {
    delete nme->current_call_frame();
    delete nme->current_native_frame();
    delete nme;
  }

  void test_native_method_environment_get_cFalse_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();

    TS_ASSERT_EQUALS(Qfalse, nme->get_handle(cFalse));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_cTrue_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();

    TS_ASSERT_EQUALS(Qtrue, nme->get_handle(cTrue));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_cNil_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();

    TS_ASSERT_EQUALS(Qnil, nme->get_handle(cNil));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_cUndef_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();

    TS_ASSERT_EQUALS(Qundef, nme->get_handle(cUndef));

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
    VALUE handle = nme->get_handle(str);

    TS_ASSERT_EQUALS(id, nme->get_object(handle)->id(state));

    destroy_native_method_environment(nme);
  }
};

#include "machine/test/test.hpp"

#include "class/native_method.hpp"
#include "vm.hpp"
#include "state.hpp"
#include "memory.hpp"

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
    Integer* id = str->object_id(state);
    VALUE handle = str->get_handle(state)->as_value();

    TS_ASSERT_EQUALS(str, MemoryHandle::object(handle));
    TS_ASSERT_EQUALS(id, MemoryHandle::object(handle)->object_id(state));
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

    TS_ASSERT_EQUALS(Qfalse, MemoryHandle::value(cFalse));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_cTrue_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();

    TS_ASSERT_EQUALS(Qtrue, MemoryHandle::value(cTrue));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_cNil_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();

    TS_ASSERT_EQUALS(Qnil, MemoryHandle::value(cNil));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_cUndef_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();

    TS_ASSERT_EQUALS(Qundef, MemoryHandle::value(cUndef));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_Symbol_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();
    Symbol* sym = state->symbol("capi_symbol");

    TS_ASSERT_EQUALS(sym, reinterpret_cast<Symbol*>(MemoryHandle::value(sym)));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_Fixnum_handle() {
    NativeMethodEnvironment* nme = create_native_method_environment();
    Fixnum* fix = Fixnum::from(28);

    TS_ASSERT_EQUALS(fix, reinterpret_cast<Fixnum*>(MemoryHandle::value(fix)));

    destroy_native_method_environment(nme);
  }

  void test_native_method_environment_get_object() {
    NativeMethodEnvironment* nme = create_native_method_environment();
    String* str = String::create(state, "test");
    Integer* id = str->object_id(state);
    VALUE handle = MemoryHandle::value(str);

    TS_ASSERT_EQUALS(str, MemoryHandle::object(handle));
    TS_ASSERT_EQUALS(id, MemoryHandle::object(handle)->object_id(state));

    destroy_native_method_environment(nme);
  }
};

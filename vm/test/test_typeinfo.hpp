#include "vm/test/test.hpp"

#include "object_types.hpp"
#include "type_info.hpp"

#include "builtin/object.hpp"

class TestTypeInfo : public CxxTest::TestSuite, public VMTest {

  class BaseNoCleanup : public TypeInfo {
  public:
    BaseNoCleanup(object_type type, bool cleanup = false)
      : TypeInfo(type, cleanup)
    {}
    virtual void auto_mark(Object* obj, ObjectMark& mark) {}
  };

  class FirstGenWithCleanup : public BaseNoCleanup {
  public:
    FirstGenWithCleanup(object_type type, bool cleanup = true)
      : BaseNoCleanup(type, true), firstgen_cleaned(false)
    {}

    virtual void cleanup(Object* obj) { firstgen_cleaned = true; }

    bool firstgen_cleaned;
  };

  class SecondGenNoCleanup : public FirstGenWithCleanup {
  public:
    SecondGenNoCleanup(object_type type, bool cleanup = false)
      : FirstGenWithCleanup(type, cleanup) {}
  };

  class ThirdGenWithCleanup : public SecondGenNoCleanup {
  public:
    ThirdGenWithCleanup(object_type type, bool cleanup = true)
      : SecondGenNoCleanup(type, true), thirdgen_cleaned(false) {}

    virtual void cleanup(Object* obj) { thirdgen_cleaned = true; FirstGenWithCleanup::cleanup(obj); }

    bool thirdgen_cleaned;
  };

  class BadThirdGenWithCleanup : public SecondGenNoCleanup {
  public:
    BadThirdGenWithCleanup(object_type type, bool cleanup = true)
      : SecondGenNoCleanup(type, true), bad_thirdgen_cleaned(false) {}

    virtual void cleanup(Object* obj) { bad_thirdgen_cleaned = true; }

    bool bad_thirdgen_cleaned;
  };

  public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_inherits_cleanup()
  {
    FirstGenWithCleanup* ti = new FirstGenWithCleanup(NilType);

    TS_ASSERT_EQUALS(ti->instances_need_cleanup, true);
  }

  void test_shows_cleanup_if_a_superclass_needs_cleanup()
  {
    SecondGenNoCleanup* ti = new SecondGenNoCleanup(NilType);

    TS_ASSERT_EQUALS(ti->instances_need_cleanup, true);
  }

  void test_shows_no_cleanup_only_if_self_nor_superclass_needs()
  {
    BaseNoCleanup* ti = new BaseNoCleanup(NilType);

    TS_ASSERT_EQUALS(ti->instances_need_cleanup, false);
  }

  void test_subclass_cleanup_function_not_calling_its_super_means_leakage()
  {
    BadThirdGenWithCleanup* ti = new BadThirdGenWithCleanup(NilType);

    TS_ASSERT_EQUALS(dynamic_cast<FirstGenWithCleanup*>(ti)->firstgen_cleaned, false);
    TS_ASSERT_EQUALS(ti->bad_thirdgen_cleaned, false);

    ti->cleanup(static_cast<Object*>(Qnil));

    TS_ASSERT_EQUALS(dynamic_cast<FirstGenWithCleanup*>(ti)->firstgen_cleaned, false);
    TS_ASSERT_EQUALS(ti->bad_thirdgen_cleaned, true);
  }

  void test_subclass_cleanup_function_can_call_parent_cleanup_directly_and_should()
  {
    ThirdGenWithCleanup* ti = new ThirdGenWithCleanup(NilType);

    TS_ASSERT_EQUALS(dynamic_cast<FirstGenWithCleanup*>(ti)->firstgen_cleaned, false);
    TS_ASSERT_EQUALS(ti->thirdgen_cleaned, false);

    ti->cleanup(static_cast<Object*>(Qnil));

    TS_ASSERT_EQUALS(dynamic_cast<FirstGenWithCleanup*>(ti)->firstgen_cleaned, true);
    TS_ASSERT_EQUALS(ti->thirdgen_cleaned, true);
  }
};

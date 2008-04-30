#include "objects.hpp"
#include "vm.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestRegexp : public CxxTest::TestSuite {
  public:

#undef state
  VM *state;
  String *str;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_version() {
    char* ver = Regexp::version(state);
    TS_ASSERT(strlen(ver) > 0);
  }

  void test_create() {
    String *pat = String::create(state, ".");
    Regexp* re = Regexp::create(state, pat, Object::i2n(0));
    TS_ASSERT_EQUALS(re->source, pat);
    TS_ASSERT_EQUALS(re->names,  Qnil);
  }

  void test_create_with_named_captures() {
    String *pat = String::create(state, "(?<blah>.)");
    Regexp* re = Regexp::create(state, pat, Object::i2n(0));
    TS_ASSERT_EQUALS(re->source, pat);
    TS_ASSERT(re->names->kind_of_p(state, G(hash)));
  }

  void test_create_with_bad_pattern() {
    char buf[1024];
    memset(buf, 0, 1024);

    String *pat = String::create(state, "(?");
    OBJECT re = Regexp::create(state, pat, Object::i2n(0), buf);
    TS_ASSERT(re->nil_p());
    TS_ASSERT(strlen(buf) > 0);
  }

  void test_options() {
    String *pat = String::create(state, ".");
    Regexp* re = Regexp::create(state, pat, Object::i2n(0));

    TS_ASSERT_EQUALS(as<Integer>(re->options(state))->n2i(), 16);
  }

  void test_match_region() {
    String *pat = String::create(state, ".");
    Regexp* re = Regexp::create(state, pat, Object::i2n(0));

    String *input = String::create(state, "abc");

    FIXNUM start = Object::i2n(0);
    FIXNUM end =   Object::i2n(3);
    OBJECT forward = Qtrue;

    MatchData* matches = (MatchData*)re->match_region(state, input, start, end, forward);
    TS_ASSERT(!matches->nil_p());
    TS_ASSERT_EQUALS(matches->region->field_count, 0);
    TS_ASSERT_EQUALS(as<Integer>(matches->full->at(0))->n2i(), 0);
    TS_ASSERT_EQUALS(as<Integer>(matches->full->at(1))->n2i(), 1);
  }
  
  void test_match_region_with_captures() {
    String *pat = String::create(state, ".(.)");
    Regexp* re = Regexp::create(state, pat, Object::i2n(0));

    String *input = String::create(state, "abc");

    FIXNUM start = Object::i2n(0);
    FIXNUM end =   Object::i2n(3);
    OBJECT forward = Qtrue;

    MatchData* matches = (MatchData*)re->match_region(state, input, start, end, forward);
    TS_ASSERT(!matches->nil_p());
    TS_ASSERT_EQUALS(as<Integer>(matches->full->at(0))->n2i(), 0);
    TS_ASSERT_EQUALS(as<Integer>(matches->full->at(1))->n2i(), 2);
    
    TS_ASSERT_EQUALS(matches->region->field_count, 1);
    TS_ASSERT_EQUALS(as<Integer>(matches->region->at(0)->at(0))->n2i(), 1);
    TS_ASSERT_EQUALS(as<Integer>(matches->region->at(0)->at(1))->n2i(), 2);
  }
};

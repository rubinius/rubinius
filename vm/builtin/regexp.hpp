#ifndef RBX_REGEXP_HPP
#define RBX_REGEXP_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

// HACK gross.
// Forward declare ONLY if we haven't already included onig.h
// We do this because onigurama seems to have regex_t be a weird
// typedef. It's easier to just not bother with trying to duplicate
// what it does and do this.
#ifndef ONIGURUMA_H
struct regex_t;
#endif

namespace rubinius {
  class String;
  class Tuple;
  class LookupTable;

  class Regexp : public Object {
  public:
    const static object_type type = RegexpType;

  private:
    String* source_;     // slot
    LookupTable* names_; // slot
    regex_t* onig_data;
    bool forced_encoding_;

  public:
    /* accessors */

    attr_accessor(source, String);
    attr_accessor(names, LookupTable);

    /* interface */

    static void init(STATE);
    static Regexp* create(STATE);
    static char*  version(STATE);

    // Ruby.primitive :regexp_initialize
    Regexp* initialize(STATE, String* pattern, Integer* options, Object* lang);
    void maybe_recompile(STATE);

    // Ruby.primitive :regexp_options
    Object* options(STATE);

    // Ruby.primitive :regexp_search_region
    Object* match_region(STATE, String* string, Integer* start, Integer* end, Object* forward);

    // Ruby.primitive :regexp_match_start
    Object* match_start(STATE, String* string, Integer* start);

    // Ruby.primitive :regexp_allocate
    static Regexp* allocate(STATE, Object* self);

    // Ruby.primitive :regexp_last_match_result
    static Object* last_match_result(STATE, Fixnum* mode, Fixnum* which, CallFrame* calling_environment);

    // Ruby.primitive :regexp_last_match
    static Object* last_match(STATE, Arguments& args, CallFrame* calling_environment);

    // Ruby.primitive :regexp_set_last_match
    static Object* set_last_match(STATE, Object* obj, CallFrame* calling_environment);

    // Ruby.primitive :regexp_propagate_last_match
    static Object* propagate_last_match(STATE, CallFrame* calling_environment);

    // Ruby.primitive :regexp_set_block_last_match
    static Object* set_block_last_match(STATE, CallFrame* calling_environment);

    void make_managed(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(Object* obj, ObjectMark& mark);
      virtual void visit(Object* obj, ObjectVisitor& visit);
      virtual void auto_mark(Object* obj, ObjectMark& mark);
      virtual void auto_visit(Object*, ObjectVisitor&);
      virtual void populate_slot_locations();
      virtual void set_field(STATE, Object*, size_t, Object*);
      virtual Object* get_field(STATE, Object*, size_t);
    };

    friend class Info;

  };

  class MatchData : public Object {
  public:
    const static size_t fields = 4;
    const static object_type type = MatchDataType;

  private:
    String* source_; // slot
    Regexp* regexp_; // slot
    Tuple* full_;    // slot
    Tuple* region_;  // slot

  public:
    /* accessors */

    attr_accessor(source, String);
    attr_accessor(regexp, Regexp);
    attr_accessor(full, Tuple);
    attr_accessor(region, Tuple);

    String* matched_string(STATE);
    String* pre_matched(STATE);
    String* post_matched(STATE);
    Object* last_capture(STATE);
    Object* nth_capture(STATE, size_t which);

    /* interface */

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

}

#endif

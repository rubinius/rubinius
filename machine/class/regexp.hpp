#ifndef RBX_REGEXP_HPP
#define RBX_REGEXP_HPP

#include "object_utils.hpp"
#include "spinlock.hpp"

#include "class/lookup_table.hpp"
#include "class/object.hpp"
#include "class/string.hpp"
#include "class/tuple.hpp"

// HACK gross.
// Forward declare ONLY if we haven't already included oniguruma.h
// We do this because onigurama seems to have regex_t be a weird
// typedef. It's easier to just not bother with trying to duplicate
// what it does and do this.
#ifndef ONIGURUMA_H
struct regex_t;
#endif

namespace rubinius {
  class Encoding;
  class String;
  class Tuple;
  class LookupTable;
  class Regexp;

  // Cache up to 4 entries, for ASCII, Binary, UTF-8
  // and other encodings.
  const static int cCachedOnigDatas = 4;

  class MatchData : public Object {
  public:
    const static object_type type = MatchDataType;

    attr_accessor(source, String);
    attr_accessor(regexp, Regexp);
    attr_accessor(full, Tuple);
    attr_accessor(region, Tuple);

    String* matched_string(STATE);
    String* pre_matched(STATE);
    String* post_matched(STATE);
    String* last_capture(STATE);
    String* nth_capture(STATE, intptr_t which);

    /* interface */
    static void initialize(STATE, MatchData* obj) {
      obj->source(nil<String>());
      obj->regexp(nil<Regexp>());
      obj->full(nil<Tuple>());
      obj->region(nil<Tuple>());
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  class Regexp : public Object {
  public:
    const static object_type type = RegexpType;

    attr_accessor(source, String);
    attr_accessor(names, LookupTable);

  private:
    regex_t* onig_data[cCachedOnigDatas];
    locks::spinlock_mutex lock_;

    attr_field(fixed_encoding, bool);
    attr_field(no_encoding, bool);

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, Regexp* obj) {
      obj->source(nil<String>());
      obj->names(nil<LookupTable>());

      for(int i = 0; i < cCachedOnigDatas; ++i) {
        obj->onig_data[i] = NULL;
      }

      new(&obj->lock_) locks::spinlock_mutex;
      obj->fixed_encoding(false);
      obj->no_encoding(false);
    }

    static Regexp* create(STATE);
    static char*  version(STATE);

    /* The Regexp options bit flags fit well within a Fixnum. If more
     * options are added, this should be double-checked.
     */
    // Rubinius.primitive :regexp_initialize
    Regexp* initialize(STATE, String* pattern, Fixnum* options);
    regex_t* maybe_recompile(STATE, String* string);

    // Rubinius.primitive :regexp_options
    Fixnum* options(STATE);

    // Rubinius.primitive+ :regexp_fixed_encoding_p
    Object* fixed_encoding_p(STATE) {
      return RBOOL(fixed_encoding());
    }

    // Rubinius.primitive :regexp_search_region
    MatchData* match_region(STATE, String* string, Fixnum* start, Fixnum* end, Object* forward);

    // Rubinius.primitive :regexp_match_start
    MatchData* match_start(STATE, String* string, Fixnum* start);

    // Rubinius.primitive :regexp_search_from
    MatchData* search_from(STATE, String* string, Fixnum* start);

    // Rubinius.primitive :regexp_allocate
    static Regexp* allocate(STATE, Object* self);

    // Rubinius.primitive :regexp_last_match_result
    static Object* last_match_result(STATE, Fixnum* mode, Fixnum* which);

    static Object* last_match(STATE);

    // Rubinius.primitive :regexp_last_match
    static Object* last_match(STATE, Arguments& args);

    // Rubinius.primitive :regexp_set_last_match
    static Object* set_last_match(STATE, Object* obj);

    // Rubinius.primitive :regexp_propagate_last_match
    static Object* propagate_last_match(STATE);

    // Rubinius.primitive :regexp_set_block_last_match
    static Object* set_block_last_match(STATE);

    // Rubinius.primitive+ :regexp_encoding
    Encoding* encoding(STATE);

    Encoding* encoding(STATE, Encoding* enc);

    regex_t* make_managed(STATE, Encoding* enc, regex_t* reg);
    regex_t* onig_source_data(STATE);
    regex_t* onig_data_encoded(STATE, Encoding* enc);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(STATE, Object* obj, std::function<void (STATE, Object**)> f);
      virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f);
      virtual void populate_slot_locations();
      virtual void set_field(STATE, Object*, size_t, Object*);
      virtual Object* get_field(STATE, Object*, size_t);
      virtual void before_visit(STATE, Object* o, std::function<void (STATE, Object**)> f) {}
      virtual void after_visit(STATE, Object* o, std::function<void (STATE, Object**)> f) {}
      virtual void visit(STATE, Object* o, std::function<void (STATE, Object**)> f);
    };

    friend class Info;

  };

}

#endif

#ifndef RBX_REGEXP_HPP
#define RBX_REGEXP_HPP

namespace rubinius {
  class RegexpData : public Object {
    public:
    const static size_t fields = 0;
    const static object_type type = RegexpDataType;

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(STATE, OBJECT t, ObjectMark& mark);
      virtual void cleanup(OBJECT obj);
    };
  };

  class Regexp : public Object {
    public:
    const static size_t fields = 4;
    const static object_type type = RegexpType;

    OBJECT __ivars__; // slot
    String* source; // slot
    RegexpData* data; // slot
    Hash* names; // slot

    static void cleanup(STATE, OBJECT data);
    static void init(STATE);
    static Regexp* create(STATE, String* pattern, INTEGER options, char* err_buf = NULL);
    static char*  version(STATE);

    // Ruby.primitive :regexp_options
    OBJECT options(STATE);

    // Ruby.primitive :regexp_search_region
    OBJECT match_region(STATE, String* string, INTEGER start, INTEGER end, OBJECT forward);

    // Ruby.primitive :regexp_match_start
    OBJECT match_start(STATE, String* string, INTEGER start);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };

  class MatchData : public Object {
    public:
    const static size_t fields = 5;
    const static object_type type = MatchDataType;

    OBJECT __ivars__; // slot
    String* source; // slot
    Regexp* regexp; // slot
    Tuple* full; // slot
    Tuple* region; // slot

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

}

#endif

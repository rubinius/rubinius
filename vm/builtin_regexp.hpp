#ifndef RBX_REGEXP_HPP
#define RBX_REGEXP_HPP


namespace rubinius {
  class RegexpData : public BuiltinType {
    public:
    const static size_t fields = 0;
    const static object_type type = RegexpDataType;

    class Info : public TypeInfo {
    public:
      Info(Class* cls) : TypeInfo(cls) { }
      virtual void cleanup(OBJECT obj);
    };
  };
  
  class MatchData : public BuiltinType {
    public:
    const static size_t fields = 5;
    const static object_type type = MatchDataType;

    OBJECT instance_variables;
    OBJECT source;
    OBJECT regexp;
    OBJECT full;
    OBJECT region;
  };
  
  class Regexp : public BuiltinType {
    public:
    const static size_t fields = 4;
    const static object_type type = RegexpType;

    OBJECT instance_variables;
    OBJECT source;
    OBJECT data;
    OBJECT names;

    static void cleanup(STATE, OBJECT data);
    static void init(STATE);
    static Regexp* create(STATE, String* pattern, INTEGER options, char* err_buf = NULL);
    static char*  version(STATE);

    OBJECT options(STATE);
    OBJECT match_region(STATE, String* string, INTEGER start, INTEGER end, OBJECT forward);

  };
}

#endif

namespace rubinius {
  class Staticscope : public BuiltinType {
    public:
    const static size_t fields = 3;
    OBJECT instance_variables;
    OBJECT module;
    OBJECT parent;
  };
};

namespace rubinius {
  class Metaclass : public BuiltinType {
    public:
    const static size_t fields = 12;
    OBJECT instance_variables;
    OBJECT method_table;
    OBJECT method_cache;
    OBJECT name;
    OBJECT constants;
    OBJECT encloser;
    OBJECT superclass;
    OBJECT instance_fields;
    OBJECT has_ivars;
    OBJECT needs_cleanup;
    OBJECT object_type;
    OBJECT attached_instance;
  };
};

namespace rubinius {
  class Symboltable : public BuiltinType {
    public:
    const static size_t fields = 3;
    OBJECT instance_variables;
    OBJECT symbols;
    OBJECT strings;
  };
};

namespace rubinius {
  class Tuple : public BuiltinType {
    public:
    const static size_t fields = 0;
  };
};

namespace rubinius {
  class Hash : public BuiltinType {
    public:
    const static size_t fields = 7;
    OBJECT instance_variables;
    OBJECT keys;
    OBJECT values;
    OBJECT bins;
    OBJECT entries;
    OBJECT default_value;
    OBJECT default_proc;
  };
};

namespace rubinius {
  class Blockcontext : public BuiltinType {
    public:
    const static size_t fields = 0;
  };
};

namespace rubinius {
  class Selector : public BuiltinType {
    public:
    const static size_t fields = 2;
    OBJECT name;
    OBJECT send_sites;
  };
};

namespace rubinius {
  class Regexpdata : public BuiltinType {
    public:
    const static size_t fields = 0;
  };
};

namespace rubinius {
  class Class : public BuiltinType {
    public:
    const static size_t fields = 11;
    OBJECT instance_variables;
    OBJECT method_table;
    OBJECT method_cache;
    OBJECT name;
    OBJECT constants;
    OBJECT encloser;
    OBJECT superclass;
    OBJECT instance_fields;
    OBJECT has_ivars;
    OBJECT needs_cleanup;
    OBJECT object_type;
  };
};

namespace rubinius {
  class Matchdata : public BuiltinType {
    public:
    const static size_t fields = 5;
    OBJECT instance_variables;
    OBJECT source;
    OBJECT regexp;
    OBJECT full;
    OBJECT region;
  };
};

namespace rubinius {
  class Object : public BuiltinType {
    public:
    const static size_t fields = 1;
    OBJECT instance_variables;
  };
};

namespace rubinius {
  class Lookuptable : public BuiltinType {
    public:
    const static size_t fields = 4;
    OBJECT field0;
    OBJECT values;
    OBJECT bins;
    OBJECT entries;
  };
};

namespace rubinius {
  class Array : public BuiltinType {
    public:
    const static size_t fields = 4;
    OBJECT total;
    OBJECT tuple;
    OBJECT start;
    OBJECT shared;
  };
};

namespace rubinius {
  class Exception : public BuiltinType {
    public:
    const static size_t fields = 3;
    OBJECT instance_variables;
    OBJECT message;
    OBJECT context;
  };
};

namespace rubinius {
  class Blockenvironment : public BuiltinType {
    public:
    const static size_t fields = 9;
    OBJECT instance_variables;
    OBJECT home;
    OBJECT initial_ip;
    OBJECT last_ip;
    OBJECT post_send;
    OBJECT home_block;
    OBJECT local_count;
    OBJECT bonus;
    OBJECT method;
  };
};

namespace rubinius {
  class Io : public BuiltinType {
    public:
    const static size_t fields = 4;
    OBJECT instance_variables;
    OBJECT descriptor;
    OBJECT buffer;
    OBJECT mode;
  };
};

namespace rubinius {
  class Regexp : public BuiltinType {
    public:
    const static size_t fields = 4;
    OBJECT instance_variables;
    OBJECT source;
    OBJECT data;
    OBJECT names;
  };
};

namespace rubinius {
  class Bignum : public BuiltinType {
    public:
    const static size_t fields = 0;
  };
};

namespace rubinius {
  class Symbol : public BuiltinType {
    public:
    const static size_t fields = 0;
  };
};

namespace rubinius {
  class Module : public BuiltinType {
    public:
    const static size_t fields = 7;
    OBJECT instance_variables;
    OBJECT method_table;
    OBJECT method_cache;
    OBJECT name;
    OBJECT constants;
    OBJECT encloser;
    OBJECT superclass;
  };
};

namespace rubinius {
  class String : public BuiltinType {
    public:
    const static size_t fields = 6;
    OBJECT bytes;
    OBJECT characters;
    OBJECT encoding;
    OBJECT data;
    OBJECT hash;
    OBJECT shared;
  };
};

namespace rubinius {
  class Bytearray : public BuiltinType {
    public:
    const static size_t fields = 0;
  };
};

namespace rubinius {
  class Methodtable : public BuiltinType {
    public:
    const static size_t fields = 4;
    OBJECT field0;
    OBJECT values;
    OBJECT bins;
    OBJECT entries;
  };
};

namespace rubinius {
  class Methodcontext : public BuiltinType {
    public:
    const static size_t fields = 0;
  };
};

namespace rubinius {
  class Compiledmethod : public BuiltinType {
    public:
    const static size_t fields = 19;
    OBJECT instance_variables;
    OBJECT primitive;
    OBJECT required;
    OBJECT serial;
    OBJECT bytecodes;
    OBJECT name;
    OBJECT file;
    OBJECT local_count;
    OBJECT literals;
    OBJECT total_args;
    OBJECT splat;
    OBJECT exceptions;
    OBJECT lines;
    OBJECT path;
    OBJECT field14;
    OBJECT bonus;
    OBJECT compiled;
    OBJECT staticscope;
    OBJECT args;
  };
};

namespace rubinius {
  class Float : public BuiltinType {
    public:
    const static size_t fields = 0;
  };
};


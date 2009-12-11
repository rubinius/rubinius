#ifndef RBX_LOOKUP_DATA_HPP
#define RBX_LOOKUP_DATA_HPP

namespace rubinius {
  class Object;
  class Module;

  struct LookupData {
    Object* recv;
    Module* from;
    bool priv;

    LookupData(Object* recv, Module* mod, bool priv = false)
      : recv(recv)
      , from(mod)
      , priv(priv)
    {}
  };
}

#endif

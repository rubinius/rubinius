#ifndef RBX_LOOKUP_DATA_HPP
#define RBX_LOOKUP_DATA_HPP

namespace rubinius {
  class Object;
  class Module;

  struct LookupData {
    Object* recv;
    Module* from;
    Symbol* min_visibility;

    LookupData()
      : recv(NULL)
      , from(NULL)
      , min_visibility(NULL)
    { }

    LookupData(Object* recv, Module* mod, Symbol* min_vis)
      : recv(recv)
      , from(mod)
      , min_visibility(min_vis)
    { }
  };
}

#endif

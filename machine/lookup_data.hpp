#ifndef RBX_LOOKUP_DATA_HPP
#define RBX_LOOKUP_DATA_HPP

namespace rubinius {
  class Object;
  class Module;

  struct LookupData {
    Object* recv;
    Module* from;
    Symbol* min_visibility;

    LookupData(Object* recv, Module* mod, Symbol* min_vis)
      : recv(recv)
      , from(mod)
      , min_visibility(min_vis)
    {}
  };
}

#endif

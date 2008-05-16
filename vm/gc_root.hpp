#ifndef RBX_GC_ROOT_HPP
#define RBX_GC_ROOT_HPP

#include "prelude.hpp"
#include <list>

namespace rubinius {
  class Root;

  typedef std::list<Root*> Roots;

  class Root {
  public:
    OBJECT object;
    Roots* roots;

    Root(Roots* roots, OBJECT obj) : object(obj), roots(roots) {
      roots->push_back(this);
    }

    Root(Roots* roots) : object(NULL), roots(roots) {
      roots->push_back(this);
    }

    Root(STATE);
    Root(STATE, OBJECT obj);

    Root() : object(NULL), roots(NULL) { }

    ~Root() {
      if(roots) roots->remove(this);
    }

    OBJECT get() { 
      if(object) {
        return object;
      } else {
        return Qnil;
      }
    }

    void set(OBJECT obj, Roots* r) {
      object = obj;
      roots = r;
      roots->push_back(this);
    }

    void set(OBJECT obj) {
      object = obj;
      if(!roots) {
        throw "invalid Root usage. Set object before roots";
      }
    }

    void set(Roots* r) {
      if(roots) roots->remove(this);
      roots = r;
      roots->push_back(this);
    }

    Root operator=(Root& other) {
      if(roots) roots->remove(this);
      roots = other.roots;
      object = other.object;
      if(roots) roots->push_back(this);

      return *this;
    }
  };

  template <typename T>
    class TypedRoot : public Root {
    public:
      T operator->() {
        if(object) {
          return (T)object;
        } else {
          return (T)Qnil;
        }
      }

      TypedRoot() : Root() { }
      TypedRoot(Roots* roots) : Root(roots) { }
      TypedRoot(STATE) : Root(state) { }
      TypedRoot(STATE, T obj) : Root(state, obj) { }
      T get() { return (T)object; }
    };
}

#endif

#ifndef RBX_GC_ROOT_HPP
#define RBX_GC_ROOT_HPP

#include "prelude.hpp"
#include "oop.hpp"
#include <stdexcept>

#include "linkedlist.hpp"

namespace rubinius {
  class Root;

  class Roots : public LinkedList {
  public:
    Roots(size_t _dummy = 0) : LinkedList() { }
    Root* front();
  };

  class Root : public LinkedList::Node {
  protected:
    OBJECT object;

  private:
    Roots* roots;

  public:
    Root(Roots* roots, OBJECT obj) : object(obj), roots(roots) {
      roots->add(this);
    }

    Root(Roots* roots) : object(NULL), roots(roots) { }

    Root(STATE);
    Root(STATE, OBJECT obj);

    explicit Root() : object(NULL), roots(NULL) { }

    Root(const Root& other) {
      set(other.object, other.roots);
    }

    ~Root() {
      if(roots && object) roots->remove(this);
    }

    OBJECT get() {
      if(object) {
        return object;
      } else {
        return Qnil;
      }
    }

    void set(OBJECT obj, Roots* r) {
      // Still in the same set, no problem, just repoint
      // object.
      if(roots == r) {

        // We don't add the root until it's got an object.
        if(!object) roots->add(this);
        object = obj;

      // Moving to a new set. Remove ourselves from
      // the current set if we added ourself (we have an
      // object)
      } else {
        if(object) roots->remove(this);
        object = obj;
        roots = r;
        if(object) roots->add(this);
      }
    }

    void set(OBJECT obj) {
      if(!roots) {
        throw std::runtime_error("invalid Root usage. Set object before roots");
      }

      set(obj, roots);
    }

    void set(Roots* r) {
      set(object, roots);
    }

    Root& operator=(Root& other) {
      set(other.object, other.roots);
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

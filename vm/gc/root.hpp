#ifndef RBX_GC_ROOT_HPP
#define RBX_GC_ROOT_HPP


#include <stdexcept>

#include "linkedlist.hpp"
#include "oop.hpp"
#include "prelude.hpp"

#include "util/thread.hpp"

namespace rubinius {
  class Root;

  /**
   *  Roots is a structure comprising of Root objects.
   *
   *  @todo Add more information about class. --rue
   *  @todo Document methods. --rue
   */
  class Roots : public LinkedList {
    utilities::thread::Mutex lock_;

  public:   /* Ctors */
    Roots()
      : LinkedList()
    {}

  public:   /* Interface */
    Root* front();

    typedef LinkedList::Iterator<Roots, Root> Iterator;

    void add(Root*);
    void remove(Root*);
  };

  /**
   *  A Root envelops an Object.
   *
   *  Each Root is also associated with a certain Roots structure
   *  and could be used to access its other members. The Root can
   *  be associated with (or "migrated to") a different Roots.
   *
   *  @todo Document remaining methods. --rue
   */
  class Root : public LinkedList::Node {
    /** The Roots structure this Root belongs to. */
    Roots*  roots_;

  protected:
    /** Enveloped Object. */
    Object*  object_;

  public: /** Constructors */

    Root()
      : LinkedList::Node()
      , roots_(NULL)
      , object_(cUndef)
    {}

    Root(Roots* roots)
      : LinkedList::Node()
      , roots_(roots)
      , object_(cUndef)
    {}

    Root(Roots* roots, Object* obj)
      : LinkedList::Node()
      , roots_(roots)
      , object_(obj)
    {
      roots_->add(this);
    }

    Root(STATE);
    Root(STATE, Object* obj);

    Root(VM*);
    Root(VM*, Object* obj);

    /** Copy construction uses set() semantics. */
    Root(const Root& other)
      : LinkedList::Node()
      , roots_(NULL)
      , object_(cUndef)
    {
      set(other.object_, other.roots_);
    }

    ~Root() {
      if(roots_ && object_ && object_ != cUndef) roots_->remove(this);
    }

  public: /** Methods */

    /** Assignment uses set() semantics. */
    Root& operator=(Root& other) {
      set(other.object_, other.roots_);
      return *this;
    }

    /** Obtain the enveloped Object. */
    Object* get() {
      return object_;
    }

    /** Envelope the given Object. Must have roots already. */
    void set(Object* obj) {
      assert(roots_ && "invalid Root usage. Cannot set object before roots");
      set(obj, roots_);
    }

    /** Envelope the given Object, migrating to given Roots if it is new. */
    void set(Object* obj, Roots* r);

    // Used in the JIT to allow for loading of Roots directly.
    Object** object_address() {
      return &object_;
    }
  };

  /**
   *  TypedRoot is a Root that knows the type of its Object.
   *
   *  @todo Use base type of object rather than pointer type
   *        as ObjType and change usage accordingly? This
   *        allows, among other things, using `as()` rather
   *        than a direct C++ cast. --rue
   */
  template <typename ObjType>
    class TypedRoot : public Root {
    public:
      TypedRoot()
        : Root()
      {}

      /** As Root::Root(roots), but retains object's type. */
      TypedRoot(Roots* roots)
        : Root(roots)
      {}

      /** As Root::Root(STATE), but retains object's type. */
      TypedRoot(STATE)
        : Root(state)
      {}

      TypedRoot(VM* state)
        : Root(state)
      {}
      /** As Root::Root(STATE, Object*), but retains object's type. */
      TypedRoot(STATE, ObjType obj)
        : Root(state, reinterpret_cast<Object*>(obj))
      {}

      TypedRoot(VM* state, ObjType obj)
        : Root(state, reinterpret_cast<Object*>(obj))
      {}

      /** Transparently delegate dereferencing to enveloped object. */
      /** @todo Use as<ObjType>() when using base type instead of pointer. --rue */
      ObjType operator->() {
        // assert(object_ && "Using an unassigned root!");
        return reinterpret_cast<ObjType>(object_);
      }

      /** Return the enveloped object as the real ObjType. */
      /** @todo Use as<ObjType>() when using base type instead of pointer. --rue */
      ObjType get() {
        // assert(object_ && "Using an unassigned root!");
        return reinterpret_cast<ObjType>(object_);
      }
    };
}

#endif

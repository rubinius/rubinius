#ifndef RBX_GC_ROOT_HPP
#define RBX_GC_ROOT_HPP


#include <stdexcept>

#include "vm/linkedlist.hpp"
#include "vm/oop.hpp"
#include "vm/prelude.hpp"


namespace rubinius {

  class Root;


  /**
   *  Roots is a structure comprising of Root objects.
   *
   *  @todo Add more information about class. --rue
   *  @todo Document methods. --rue
   */
  class Roots : public LinkedList {
  public:   /* Ctors */

    explicit Roots();

  public:   /* Interface */

    Root* front();
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
  public:   /* Ctors */

    /** Default-constructed root is blank. */
    explicit Root();

    Root(STATE);
    Root(STATE, Object* obj);
    Root(Roots* roots);
    Root(Roots* roots, Object* obj);

    /** Copy construction uses set() semantics. */
    Root(const Root& other);

    /** Assignment uses set() semantics. */
    Root& operator=(Root& other);

    /** Destruction removes this one from the roots. */
    ~Root();


  public:   /* Interface */

    /** Obtain the enveloped Object or Qnil if none. */
    Object*  get();

    /**
     *  Redoes the set of the existing Object and Roots.
     *
     *  @todo Review. This method makes no sense to me. --rue
     */
    void    set(Roots* r);

    /** Envelope the given Object. Must have roots already. */
    void    set(Object* obj);

    /** Envelope the given Object, migrating to given Roots if it is new. */
    void    set(Object* obj, Roots* r);


  protected:  /* Instance vars */

    /** Enveloped Object. */
    Object*  object;


  private:    /* Instance vars */

    /** The Roots structure this Root belongs to. */
    Roots*  roots;
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
    public:   /* Ctors */

      /** Default construction gives an empty TypedRoot. */
      explicit TypedRoot();

      /** As Root::Root(STATE), but retains object's type. */
      TypedRoot(STATE);

      /** As Root::Root(STATE, Object*), but retains object's type. */
      TypedRoot(STATE, ObjType obj);

      /** As Root::Root(roots), but retains object's type. */
      TypedRoot(Roots* roots);


    public:   /* Operators */

      /** Transparently delegate dereferencing to enveloped object. */
      ObjType   operator->();


    public:   /* Interface */

      /** Return the enveloped object as the real ObjType. */
      ObjType   get();
    };


/* Roots inlines */

  inline Roots::Roots():
    LinkedList()
  { }


/* Root inlines */


  inline Root::Root():
    LinkedList::Node(), object(NULL), roots(NULL)
  { }

  inline Root::Root(Roots* roots):
    LinkedList::Node(), object(NULL), roots(roots)
  { }

  inline Root::Root(Roots* roots, Object* obj):
    LinkedList::Node(), object(obj), roots(roots)
  {
      roots->add(this);
  }

  inline Root::Root(const Root& other):
    LinkedList::Node(), object(NULL), roots(NULL)
  {
    set(other.object, other.roots);
  }

  inline Root::~Root() {
    if(roots && object) {
      roots->remove(this);
    }
  }

  inline Root& Root::operator=(Root& other) {
    set(other.object, other.roots);
    return *this;
  }

  inline Object* Root::get() {
    if(object) {
      return object;
    } else {
      return Qnil;
    }
  }

  inline void Root::set(Roots* /* other_roots */) {
    set(object, roots);
  }

  inline void Root::set(Object* obj) {
    if(!roots) {
      throw std::runtime_error("invalid Root usage. Cannot set object before roots");
    }

    set(obj, roots);
  }


/* TypedRoot inlines */

  template<typename ObjType>
    inline TypedRoot<ObjType>::TypedRoot():
      Root()
    { }

  template<typename ObjType>
    inline TypedRoot<ObjType>::TypedRoot(Roots* roots):
      Root(roots)
    { }

  template<typename ObjType>
    inline TypedRoot<ObjType>::TypedRoot(STATE):
      Root(state)
    { }

  template<typename ObjType>
    inline TypedRoot<ObjType>::TypedRoot(STATE, ObjType obj):
      Root(state, (Object*)obj)
    { }

  /** @todo Use as<ObjType>() when using base type instead of pointer. --rue */
  template<typename ObjType>
    inline ObjType TypedRoot<ObjType>::operator->() {
      if(object) {
        return reinterpret_cast<ObjType>(object);
      }
      else {
        return reinterpret_cast<ObjType>(Qnil);
      }
    }

  /** @todo Use as<ObjType>() when using base type instead of pointer. --rue */
  template<typename ObjType>
    inline ObjType TypedRoot<ObjType>::get() {
      return reinterpret_cast<ObjType>(object);
    }

}

#endif

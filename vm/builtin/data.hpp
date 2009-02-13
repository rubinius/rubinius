#ifndef RBX_BUILTIN_DATA_HPP
#define RBX_BUILTIN_DATA_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {

  class Data : public Object {
  public:
    const static object_type type = DataType;

    /**  Register class with the VM. */
    static void   init(STATE);


  public:   /* Types */

    /** The signature for the mark function. */
    typedef   void (*MarkFunctor)(void*);

    /** The signature for the free function. */
    typedef   void (*FreeFunctor)(void*);


  public:   /* Interface */

    /** New Data instance. */
    static Data*  create(STATE, void* data, MarkFunctor mark, FreeFunctor free);


  private:  /* Instance variables */

/** @todo Disabled, no more NMC. Replace with current if possible/need. --rue */
//    NativeMethodContext*  mark_context; // slot

    FreeFunctor           free_;
    MarkFunctor           mark_;
    void*                 data_;


  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) { }
      virtual void mark(Object* t, ObjectMark& mark);
    };
  };

}

#endif

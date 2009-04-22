#ifndef RBX_CAPI_HANDLE_HPP
#define RBX_CAPI_HANDLE_HPP

#include "vm.hpp"
#include "gc/root.hpp"

#include "capi/value.hpp"

#include <list>
#include <vector>

struct RArray;
struct RString;
struct RData;

namespace rubinius {
  class NativeMethodEnvironment;

  namespace capi {

    enum HandleType {
      cUnknown,
      cRArray,
      cRString,
      cRData
    };

    class Handle : public LinkedList::Node {
      Object* object_;
      HandleType type_;
      int references_;

      union {
        RArray* rarray;
        RString* rstring;
        RData* rdata;
      } as_;

    public:
      Handle(STATE, Object* obj)
        : LinkedList::Node()
        , object_(obj)
        , type_(cUnknown)
        , references_(0)
      {
        as_.rarray = 0;
      }

      ~Handle();

      Object* object() {
        return object_;
      }

      void set_object(Object* obj) {
        object_ = obj;
      }

      bool weak_p() {
        return references_ == 0;
      }

      void ref() {
        references_++;
      }

      void deref() {
        references_--;
      }

      // Explict conversion functions, to keep the code clean.
      VALUE as_value() {
        return reinterpret_cast<VALUE>(this);
      }

      static Handle* from(VALUE val) {
        return reinterpret_cast<Handle*>(val);
      }

      bool is_rarray() {
        return type_ == cRArray;
      }

      bool is_rdata() {
        return type_ == cRData;
      }

      bool is_rstring() {
        return type_ == cRString;
      }

      RArray* as_rarray(NativeMethodEnvironment* env);
      RData*  as_rdata(NativeMethodEnvironment* env);
      RString* as_rstring(NativeMethodEnvironment* env);

      void free_data();
    };

    class Handles : public LinkedList {
    public:
      Handle* front() {
        return static_cast<Handle*>(head());
      }

      typedef LinkedList::Iterator<Handles, Handle> Iterator;
    };


    typedef std::list<Handle*> HandleList;
  }
}

#endif

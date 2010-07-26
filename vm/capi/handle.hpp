#ifndef RBX_CAPI_HANDLE_HPP
#define RBX_CAPI_HANDLE_HPP

#include "vm.hpp"
#include "gc/root.hpp"

#include "capi/value.hpp"

#include <tr1/unordered_set>
#include <vector>

struct RArray;
struct RString;
struct RData;
struct RFloat;
struct RIO;

namespace rubinius {
  class NativeMethodEnvironment;

  namespace capi {

    enum HandleType {
      cUnknown,
      cRArray,
      cRString,
      cRData,
      cRFloat,
      cRIO
    };

    class Handle : public LinkedList::Node {
      Object* object_;
      HandleType type_;
      int references_;
      unsigned int checksum_;

      typedef void (*CApiCacheFlusher)(NativeMethodEnvironment* env, Handle* handle);
      typedef void (*CApiCacheUpdater)(NativeMethodEnvironment* env, Handle* handle);

      CApiCacheFlusher flush_;
      CApiCacheUpdater update_;

      union {
        RArray*   rarray;
        RString*  rstring;
        RData*    rdata;
        RFloat*   rfloat;
        RIO*      rio;
        intptr_t  cache_data;
      } as_;

    public:
      Handle(STATE, Object* obj)
        : LinkedList::Node()
        , object_(obj)
        , type_(cUnknown)
        , references_(0)
        , checksum_(0xffff)
        , flush_(0)
        , update_(0)
      {
        as_.cache_data = 0;
      }

      static bool valid_handle_p(STATE, Handle* handle);

      ~Handle();

      void flush(NativeMethodEnvironment* env) {
        if(flush_) (*flush_)(env, this);
      }

      void update(NativeMethodEnvironment* env) {
        if(update_) (*update_)(env, this);
      }

      bool valid_p() {
        return checksum_ == 0xffff;
      }

      void invalidate() {
        checksum_ = 0;
      }

      Object* object() {
        return object_;
      }

      bool in_use_p() {
        return object_ != 0;
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

      void debug_print();

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

      bool is_rfloat() {
        return type_ == cRFloat;
      }

      bool is_rio() {
        return type_ == cRIO;
      }

      HandleType type() {
         return type_;
      }

      void forget_object() {
        free_data();
        object_ = 0;
      }

      RArray* as_rarray(NativeMethodEnvironment* env);
      RData*  as_rdata(NativeMethodEnvironment* env);
      RString* as_rstring(NativeMethodEnvironment* env);
      RFloat* as_rfloat(NativeMethodEnvironment* env);
      RIO* as_rio(NativeMethodEnvironment* env);

      void free_data();
    };

    class Handles : public LinkedList {
    public:

      ~Handles();

      Handle* front() {
        return static_cast<Handle*>(head());
      }

      void move(Node* node, Handles* handles) {
        remove(node);
        handles->add(node);
      }

      typedef LinkedList::Iterator<Handles, Handle> Iterator;
    };


    typedef std::tr1::unordered_set<Handle*> HandleSet;
  }
}

#endif

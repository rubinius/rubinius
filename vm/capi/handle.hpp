#ifndef RBX_CAPI_HANDLE_HPP
#define RBX_CAPI_HANDLE_HPP

#include "detection.hpp"

#if defined(__APPLE__) && __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1070
#ifndef RBX_HAVE_TR1_HASH
#include "missing/leopard_hashtable.hpp"
#endif
#endif

#include "vm.hpp"
#include "gc/root.hpp"

#include "capi/value.hpp"

#include <set>

#ifndef RIO
#define RIO rb_io_t
#endif

struct RArray;
struct RString;
struct RData;
struct RTypedData;
struct RFloat;
struct RIO;
struct RFile;

namespace rubinius {
  class NativeMethodEnvironment;

  namespace capi {

    enum HandleType {
      cUnknown,
      cRArray,
      cRString,
      cRData,
      cRFloat,
      cRIO,
      cRFile
    };

    class Handle {
      Object* object_;
      HandleType type_;
      int references_;
      unsigned int checksum_;

      typedef void (*CApiCacheFlusher)(NativeMethodEnvironment* env, Handle* handle);
      typedef void (*CApiCacheUpdater)(NativeMethodEnvironment* env, Handle* handle);

      CApiCacheFlusher flush_;
      CApiCacheUpdater update_;

      union {
        RArray*     rarray;
        RString*    rstring;
        RData*      rdata;
        RTypedData* rtypeddata;
        RFloat*     rfloat;
        RIO*        rio;
        RFile*      rfile;
        uintptr_t   next_index_;
        intptr_t    cache_data;
      } as_;

    public:
      Handle()
        : object_(NULL)
        , type_(cUnknown)
        , references_(0)
        , checksum_(0)
        , flush_(0)
        , update_(0)
      {
        as_.cache_data = 0;
      }

      static bool valid_handle_p(STATE, Handle* handle);

      void flush(NativeMethodEnvironment* env) {
        if(flush_) (*flush_)(env, this);
      }

      void update(NativeMethodEnvironment* env) {
        if(update_) (*update_)(env, this);
      }

#define RBX_CAPI_HANDLE_CHECKSUM    0xffff

      bool valid_p() const {
        return checksum_ == RBX_CAPI_HANDLE_CHECKSUM;
      }

      void validate() {
        checksum_ = RBX_CAPI_HANDLE_CHECKSUM;
      }

      void invalidate() {
        checksum_ = 0;
      }

      Object* object() const {
        return object_;
      }

      bool in_use_p() const {
        return object_ != 0;
      }

      void set_object(Object* obj) {
        object_ = obj;
      }

      bool weak_p() const {
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
      VALUE as_value() const {
        return reinterpret_cast<VALUE>(this);
      }

      static Handle* from(VALUE val) {
        return reinterpret_cast<Handle*>(val);
      }

      bool is_rarray() const {
        return type_ == cRArray;
      }

      bool is_rdata() const {
        return type_ == cRData;
      }

      bool is_rstring() const {
        return type_ == cRString;
      }

      bool is_rfloat() const {
        return type_ == cRFloat;
      }

      bool is_rio() const {
        return type_ == cRIO;
      }

      bool is_rfile() const {
        return type_ == cRFile;
      }

      HandleType type() const {
         return type_;
      }

      void clear() {
        forget_object();
        type_ = cUnknown;
        references_ = 0;
        flush_ = 0;
        update_ = 0;
      }

      void forget_object() {
        free_data();
        invalidate();
        object_ = 0;
      }

      RString* get_rstring() const {
        return as_.rstring;
      }

      uintptr_t next() const {
        return as_.next_index_;
      }

      void set_next(uintptr_t next_index) {
        as_.next_index_ = next_index;
      }

      RData*  as_rdata(NativeMethodEnvironment* env);
      RTypedData*  as_rtypeddata(NativeMethodEnvironment* env);
      RArray* as_rarray(NativeMethodEnvironment* env);
      RString* as_rstring(NativeMethodEnvironment* env, int cache_level);
      RFloat* as_rfloat(NativeMethodEnvironment* env);
      RIO* as_rio(NativeMethodEnvironment* env);
      RFile* as_rfile(NativeMethodEnvironment* env);

      void free_data();
      bool rio_close();
    };

    class GlobalHandle {
    private:
      Handle** handle_;
      const char* file_;
      int line_;

    public:
      GlobalHandle(Handle** handle, const char* file, int line)
        : handle_(handle)
        , file_(file)
        , line_(line)
      {}

      GlobalHandle(Handle** handle)
        : handle_(handle)
        , file_(NULL)
        , line_(0)
      {}

      Handle** handle() {
        return handle_;
      }

      const char* file() {
        return file_;
      }

      int line() {
        return line_;
      }
    };

    typedef std::set<Handle*> SlowHandleSet;

    class HandleSet {
    public:
      const static int cFastHashSize = 16;

    private:
      SlowHandleSet* slow_;
      Handle* table_[cFastHashSize];

    public:
      HandleSet();
      ~HandleSet() {
        if(slow_) delete slow_;
      }

      void deref_all();
      void flush_all(NativeMethodEnvironment* env);
      void update_all(NativeMethodEnvironment* env);
      void gc_scan(GarbageCollector* gc);

      bool add_if_absent(Handle* handle) {
        // ref() ONLY if it's not already in there!
        // otherwise the refcount is wrong and we leak handles.

        if(unlikely(slow_)) {
          return slow_add_if_absent(handle);
        } else {
          for(int i = 0; i < cFastHashSize; i++) {
            if(!table_[i]) {
              table_[i] = handle;
              handle->ref();

              return true;
            }
            if(table_[i] == handle) return false;
          }

          make_slow_and_add(handle);
          return true;
        }
      }

    private:
      void make_slow_and_add(Handle* handle);
      bool slow_add_if_absent(Handle* handle);
    };
  }
}

#endif

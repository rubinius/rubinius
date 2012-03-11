#ifndef RBX_LLVM_JIT_RUNTIME
#define RBX_LLVM_JIT_RUNTIME

#include "gc/code_resource.hpp"
#include <list>

namespace llvm {
  class Function;
}

namespace rubinius {
  class Object;
  class CompiledMethod;
  class Symbol;
  class Module;
  class GarbageCollector;
  class RuntimeDataHolder;
  class ObjectMark;
  class VM;

  namespace gc {
    class WriteBarrier;
  }

  namespace jit {
    class GCLiteral {
      Object* object_;
      GCLiteral* next_;

    public:
      GCLiteral(Object* obj, GCLiteral* nxt)
        : object_(obj)
        , next_(nxt)
      {}

      Object* object() {
        return object_;
      }

      void set_object(Object* obj) {
        object_ = obj;
      }

      GCLiteral* next() {
        return next_;
      }

      void* address_of_object() {
        return &object_;
      }
    };

    class RuntimeData {

    public:
      CompiledMethod* method_;
      Symbol* name_;
      Module* module_;

      GCLiteral* literals_;

      RuntimeData(CompiledMethod* method, Symbol* name, Module* mod)
        : method_(method)
        , name_(name)
        , module_(mod)
        , literals_(0)
      {}

      CompiledMethod* method() {
        return method_;
      }

      Symbol* name() {
        return name_;
      }

      Module* module() {
        return module_;
      }

      GCLiteral* literals() {
        return literals_;
      }

      GCLiteral* new_literal(Object* obj) {
        literals_ = new GCLiteral(obj, literals_);
        return literals_;
      }

      // For GC access.
      friend class RuntimeDataHolder;
      friend class rubinius::GarbageCollector;
    };

    class RuntimeDataHolder : public CodeResource {
      std::list<RuntimeData*> runtime_data_;
      llvm::Function* function_;
      void* native_func_;
      int  native_size_;

    public:
      RuntimeDataHolder()
        : function_(0)
        , native_func_(0)
        , native_size_(0)
      {}

      ~RuntimeDataHolder();

      virtual void cleanup(State* vm, CodeManager* cm);

      virtual const char* kind() {
        return "jit-runtime";
      }

      std::list<RuntimeData*>& runtime_data() {
        return runtime_data_;
      }

      void add_runtime_data(RuntimeData* rd) {
        runtime_data_.push_back(rd);
      }

      void set_function(llvm::Function* func, void* native, int size) {
        function_ = func;
        native_func_ = native;
        native_size_ = size;
      }

      llvm::Function* llvm_function() {
        return function_;
      }

      void* native_func() {
        return native_func_;
      }

      int native_size() {
        return native_size_;
      }

      void mark_all(Object* obj, ObjectMark& mark);
      void run_write_barrier(gc::WriteBarrier* wb, Object* obj);
    };
  }
}

#endif

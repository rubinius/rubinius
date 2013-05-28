#ifndef RBX_LLVM_JIT_RUNTIME
#define RBX_LLVM_JIT_RUNTIME

#include "gc/code_resource.hpp"
#include <vector>

namespace llvm {
  class Function;
}

namespace rubinius {
  class Object;
  class CompiledCode;
  class Symbol;
  class Module;
  class GarbageCollector;
  class RuntimeDataHolder;
  class ObjectMark;
  class ObjectMemory;
  class VM;

  namespace jit {

    class RuntimeData {

    public:
      CompiledCode* method_;
      Symbol* name_;
      Module* module_;

      RuntimeData(CompiledCode* method, Symbol* name, Module* mod)
        : method_(method)
        , name_(name)
        , module_(mod)
      {}

      CompiledCode* method() {
        return method_;
      }

      Symbol* name() {
        return name_;
      }

      Module* module() {
        return module_;
      }

      // For GC access.
      friend class RuntimeDataHolder;
      friend class rubinius::GarbageCollector;
    };

    class RuntimeDataHolder : public CodeResource {
      std::vector<RuntimeData*> runtime_data_;
      void* function_allocation_;
      void* native_func_;
      int  native_size_;

    public:
      RuntimeDataHolder()
        : function_allocation_(0)
        , native_func_(0)
        , native_size_(0)
      {}

      ~RuntimeDataHolder();

      virtual void cleanup(State* vm, CodeManager* cm);

      virtual const char* kind() {
        return "jit-runtime";
      }

      std::vector<RuntimeData*>& runtime_data() {
        return runtime_data_;
      }

      void add_runtime_data(RuntimeData* rd) {
        runtime_data_.push_back(rd);
      }

      void set_function(void* func, void* native, int size) {
        function_allocation_ = func;
        native_func_ = native;
        native_size_ = size;
      }

      void* function_allocation() {
        return function_allocation_;
      }

      void* native_func() {
        return native_func_;
      }

      int native_size() {
        return native_size_;
      }

      void mark_all(Object* obj, ObjectMark& mark);
      void run_write_barrier(ObjectMemory* om, Object* obj);
    };
  }
}

#endif

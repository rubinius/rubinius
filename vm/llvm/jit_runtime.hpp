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
  class ObjectVisitor;

  namespace gc {
    class WriteBarrier;
  }

  namespace jit {
    class RuntimeData {

    public:
      CompiledMethod* method_;
      Symbol* name_;
      Module* module_;

      RuntimeData(CompiledMethod* method, Symbol* name, Module* mod)
        : method_(method)
        , name_(name)
        , module_(mod)
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

      virtual void cleanup(CodeManager* cm);

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

      void mark_all(Object* obj, ObjectMark& mark);
      void visit_all(ObjectVisitor& visit);
      void run_write_barrier(gc::WriteBarrier* wb, Object* obj);
    };
  }
}

#endif

#include "memory.hpp"
#include "object_utils.hpp"
#include "logger.hpp"
#include "memory.hpp"
#include "on_stack.hpp"


#include "class/data.hpp"
#include "class/class.hpp"

#include "capi/capi.hpp"

#include "diagnostics/memory.hpp"

namespace rubinius {

  void Data::bootstrap(STATE) {
    GO(data).set(state->memory()->new_class<Class, Data>(state, "Data"));
  }

  Data* Data::create(STATE, void* data_ptr, Data::MarkFunctor mark, Data::FreeFunctor free) {
    Data* data;

    data = state->memory()->new_object<Data>(state, G(data));

    MemoryHandle* handle = data->get_handle(state);

    // TODO: MemoryHandle
    RDataShadow* rdata = new RDataShadow(); // = reinterpret_cast<RDataShadow*>(handle->as_rdata(0));
    handle->set_data(rdata, MemoryHandle::eRData);

    rdata->data = data_ptr;
    rdata->d.untyped.dmark = mark;
    rdata->d.untyped.dfree = free;

    data->internal(rdata);

    if(mark || free) {
      state->memory()->extension_finalizer(state, data,
          (memory::FinalizerFunction)&Data::finalize);
    }

    state->shared().memory_metrics()->data_objects++;

    return data;
  }

  Data* Data::create_typed(STATE, void* data_ptr, const struct rb_data_type_struct_shadow* type) {
    Data* data;

    data = state->memory()->new_object<Data>(state, G(data));

    MemoryHandle* handle = data->get_handle(state);

    // TODO: MemoryHandle
    // RDataShadow* rdata = reinterpret_cast<RDataShadow*>(handle->as_rtypeddata(0));
    RDataShadow* rdata = new RDataShadow();
    handle->set_data(rdata, MemoryHandle::eRData);

    rdata->data = data_ptr;
    rdata->d.typed.typed = 1;
    rdata->d.typed.type = type;

    data->internal(rdata);

    if(type->function.dmark || type->function.dfree) {
      state->memory()->extension_finalizer(state, data,
          (memory::FinalizerFunction)&Data::finalize);
    }

    state->shared().memory_metrics()->data_objects++;

    return data;
  }

  void* Data::data() {
    return rdata()->data;
  }

  Data::FreeFunctor Data::free() {
    RDataShadow* data = rdata();
    if(typed()) {
      return data->d.typed.type->function.dfree;
    } else {
      return data->d.untyped.dfree;
    }
  }

  Data::MarkFunctor Data::mark() {
    RDataShadow* data = rdata();
    if(typed()) {
      return data->d.typed.type->function.dmark;
    } else {
      return data->d.untyped.dmark;
    }
  }

  void Data::finalize(STATE, Data* data) {
    /* TODO: MemoryHandle
    MemoryHandle* handle = data->get_handle(state);

    if(!handle->valid_p()) {
      logger::error("finalizer: Data object has invalid handle");
      return;
    }

    if(handle->object() != data) {
      logger::error("finalizer: Data object handle does not reference the object");
      return;
    }

    if(data->freed_p()) {
      logger::error("finalizer: Data finalize called for already freed object");
      return;
    }
    data->set_freed();

    // MRI only calls free if the data_ptr is not NULL.
    if(void* data_ptr = data->data()) {
      Data::FreeFunctor f = data->free();
      if(f) {
        // If the user specifies -1, then we call free. We check here rather
        // than when Data_Make_Struct is called because the user is allowed to
        // change dfree.
        if(reinterpret_cast<intptr_t>(f) == -1) {
          ::free(data_ptr);
        } else {
          f(data_ptr);
        }
      }
    }
    */
  }

  void Data::Info::mark(Object* t, memory::ObjectMark& mark) {
    auto_mark(t, mark);

    Data* data = force_as<Data>(t);

    if(data->freed_p()) {
      logger::error("finalizer: Data mark called for already freed object");
      return;
    }

    Data::MarkFunctor marker = data->mark();

    if(marker) {
      memory::ObjectMark* cur = capi::current_mark();
      capi::set_current_mark(&mark);

      (*marker)(data->data());

      capi::set_current_mark(cur);
    }
  }

}

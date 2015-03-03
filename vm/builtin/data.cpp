#include "builtin/data.hpp"
#include "builtin/class.hpp"
#include "capi/capi.hpp"
#include "object_memory.hpp"
#include "object_utils.hpp"
#include "ontology.hpp"
#include "on_stack.hpp"

namespace rubinius {

  void Data::init(STATE) {
    GO(data).set(ontology::new_class(state, "Data", G(object)));
    G(data)->set_object_type(state, DataType);
  }

  Data* Data::create(STATE, void* data_ptr, Data::MarkFunctor mark, Data::FreeFunctor free) {
    Data* data;

    data = state->new_object<Data>(G(data));
    data->freed_ = false;

    // Data is just a heap alias for the handle, so go ahead and create
    // the handle and populate it as an RData now.
    capi::Handle* handle = data->handle(state);

    assert(!handle && "can't already have a handle, it's brand new!");

    handle = state->memory()->add_capi_handle(state, data);

    // Don't call ->ref() on handle! We don't want the handle to keep the object
    // alive by default. The handle needs to have the lifetime of the object.

    RDataShadow* rdata = reinterpret_cast<RDataShadow*>(handle->as_rdata(0));

    rdata->data = data_ptr;
    rdata->d.untyped.dmark = mark;
    rdata->d.untyped.dfree = free;

    data->internal_ = rdata;

    if(mark || free) {
      state->memory()->needs_finalization(data, (FinalizerFunction)&Data::finalize);
    }

    state->vm()->metrics().m.ruby_metrics.memory_data_objects_total++;

    return data;
  }

  Data* Data::create_typed(STATE, void* data_ptr, const struct rb_data_type_struct_shadow* type) {
    Data* data;

    data = state->new_object<Data>(G(data));
    data->freed_ = false;

    // Data is just a heap alias for the handle, so go ahead and create
    // the handle and populate it as an RData now.
    capi::Handle* handle = data->handle(state);

    assert(!handle && "can't already have a handle, it's brand new!");

    handle = state->memory()->add_capi_handle(state, data);

    // Don't call ->ref() on handle! We don't want the handle to keep the object
    // alive by default. The handle needs to have the lifetime of the object.

    RDataShadow* rdata = reinterpret_cast<RDataShadow*>(handle->as_rtypeddata(0));

    rdata->data = data_ptr;
    rdata->d.typed.typed = 1;
    rdata->d.typed.type = type;

    data->internal_ = rdata;

    if(type->function.dmark || type->function.dfree) {
      state->memory()->needs_finalization(data, (FinalizerFunction)&Data::finalize);
    }

    state->vm()->metrics().m.ruby_metrics.memory_data_objects_total++;

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
    capi::Handle* handle = data->handle(state);

    if(!handle->valid_p()) {
      std::cerr << "Data::finalize: object has invalid handle!" << std::endl;
      return;
    }

    if(handle->object() != data) {
      std::cerr << "Data::finalize: handle does not reference object!" << std::endl;
      return;
    }

    if(data->freed_p()) {
      // TODO: Fix the issue of finalizer ordering.
      // std::cerr << "Data::finalize called for already freed object" << std::endl;
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
  }

  void Data::Info::mark(Object* t, ObjectMark& mark) {
    auto_mark(t, mark);

    Data* data = force_as<Data>(t);

    if(mark.mature_gc_in_progress()) {
      // Don't scan objects concurrently since this might
      // not be thread safe. The C library in use here
      // might be in the process of freeing up malloc'ed
      // resources so we would see objects in an invalid
      // state and scan wrong pointers etc.
      return;
    }

    if(data->freed_p()) {
      // TODO: Fix the issue of finalizer ordering.
      // std::cerr << "Data::Info::mark called for already freed object" << std::endl;
      return;
    }

    Data::MarkFunctor marker = data->mark();

    if(marker) {
      ObjectMark* cur = capi::current_mark();
      capi::set_current_mark(&mark);

      (*marker)(data->data());

      capi::set_current_mark(cur);
    }
  }

}

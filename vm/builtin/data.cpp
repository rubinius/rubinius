#include "builtin/data.hpp"
#include "builtin/class.hpp"
#include "objectmemory.hpp"
#include "object_utils.hpp"

#include "gc/gc.hpp"

#include "capi/capi.hpp"

namespace rubinius {

  void Data::init(STATE) {
    GO(data).set(state->new_class("Data", G(object)));
    G(data)->set_object_type(state, DataType);
  }

  Data* Data::create(STATE, void* data_ptr, Data::MarkFunctor mark, Data::FreeFunctor free) {
    Data* data;

    data = state->new_object<Data>(G(data));

    // Data is just a heap alias for the handle, so go ahead and create
    // the handle and populate it as an RData now.
    InflatedHeader* ih = state->om->inflate_header(data);
    capi::Handle* handle = ih->handle();

    assert(!handle && "can't already have a handle, it's brand new!");

    handle = new capi::Handle(state, data);
    ih->set_handle(handle);

    // Don't call ->ref() on handle! We don't want the handle to keep the object
    // alive by default. The handle needs to have the lifetime of the object.

    state->shared.global_handles()->add(handle);

    RDataShadow* rdata = reinterpret_cast<RDataShadow*>(handle->as_rdata(0));

    rdata->data = data_ptr;
    rdata->dmark = mark;
    rdata->dfree = free;

    // If this Data requires a free function, register this object
    // as needing finalization.
    if(free) {
      state->om->needs_finalization(data, (FinalizerFunction)&Data::finalize);
    }

    return data;
  }

  RDataShadow* Data::rdata(STATE) {
    InflatedHeader* ih = state->om->inflate_header(this);
    capi::Handle* handle = ih->handle();

    assert(handle && handle->is_rdata() && "invalid initialized Data object");

    return reinterpret_cast<RDataShadow*>(handle->as_rdata(0));
  }

  void* Data::data(STATE) {
    return rdata(state)->data;
  }

  Data::FreeFunctor Data::free(STATE) {
    return rdata(state)->dfree;
  }

  Data::MarkFunctor Data::mark(STATE) {
    return rdata(state)->dmark;
  }

  void Data::finalize(STATE, Data* data) {
    // MRI only calls free if the data_ptr is not NULL.
    void* data_ptr = data->data(state);

    if(data_ptr) {
      Data::FreeFunctor f = data->free(state);
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

    STATE = mark.gc->state();

    Data* data = as<Data>(t);

    if(data->mark(state)) {
      ObjectMark* cur = capi::current_mark();
      capi::set_current_mark(&mark);

      (*data->mark(state))(data->data(state));

      capi::set_current_mark(cur);
    }
  }

}

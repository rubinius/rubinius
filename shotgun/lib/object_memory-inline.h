#include "shotgun/lib/auto.h"

static inline void _om_apply_class_flags(OBJECT obj, OBJECT cls) {
  obj->CanStoreIvars = (class_get_has_ivars(cls) == Qtrue);
  obj->RequiresCleanup = (class_get_needs_cleanup(cls) == Qtrue);
  obj->obj_type = FIXNUM_TO_INT(class_get_object_type(cls));
}

static inline OBJECT _om_inline_new_object(object_memory om, OBJECT cls, unsigned int fields) {
  unsigned int size;
  gc_zone loc;
  OBJECT obj;
  
  if(fields > LargeObjectThreshold) {
    mark_sweep_gc ms = om->ms;
    obj = mark_sweep_allocate(ms, fields);
    if(ms->enlarged) { 
      om->collect_now |= OMCollectMature;
    }
    
    loc = MatureObjectZone;
  } else {
    size = SIZE_IN_BYTES_FIELDS(fields);
    if(!heap_enough_space_p(om->gc->current, size)) {
      if(!heap_enough_space_p(om->gc->next, size)) {
        mark_sweep_gc ms = om->ms;
        obj = mark_sweep_allocate(ms, fields);
        if(ms->enlarged) { 
          om->collect_now |= OMCollectMature;
        }

        loc = MatureObjectZone;
      } else {
        loc = YoungObjectZone;
        obj = (OBJECT)baker_gc_allocate_spilled(om->gc, size);
        // DEBUG("Ran out of space! spilled into %p\n", obj);
        om->collect_now |= OMCollectYoung;
        // baker_gc_enlarge_next(om->gc, om->gc->current->size * GC_SCALING_FACTOR);
      }
    } else {
      obj = (OBJECT)baker_gc_allocate(om->gc, size);
      loc = YoungObjectZone;
    }
  }
  
  CLEAR_FLAGS(obj);
  
  obj->gc_zone = loc;
  
  rbs_set_class(om, obj, cls);
  SET_NUM_FIELDS(obj, fields);
  if(cls && REFERENCE_P(cls)) {
    _om_apply_class_flags(obj, cls);
  }

  return obj;
}

static inline OBJECT _om_inline_new_object_init(object_memory om, OBJECT cls, unsigned int fields) {
  OBJECT obj;
  obj = _om_inline_new_object(om, cls, fields);
  fast_memfill((void*)BYTES_OF(obj), (uintptr_t)Qnil, fields);
  
  return obj;
}

static inline OBJECT _om_new_ultra(object_memory om, OBJECT cls, unsigned int size) {
  OBJECT obj;
  /* I'd love to remove this check, but context allocation could flow over. */
  if(!heap_enough_space_p(om->gc->current, size)) {
    obj = (OBJECT)baker_gc_allocate_spilled_ultra(om->gc, size);
    om->collect_now |= OMCollectYoung;
  } else {
    obj = (OBJECT)baker_gc_allocate_ultra(om->gc, size);
  }
  
  CLEAR_FLAGS(obj);
  obj->gc_zone = YoungObjectZone;
  rbs_set_class(om, obj, cls);
  
  return obj; 
}

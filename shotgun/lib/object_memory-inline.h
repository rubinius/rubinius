static inline OBJECT _om_inline_new_object(object_memory om, OBJECT cls, int fields) {
  int size, f, loc;
  OBJECT obj, flags;
  struct rubinius_object *header;
  
  if(fields > LargeObjectThreshold) {
    mark_sweep_gc ms = om->ms;
    obj = mark_sweep_allocate(ms, fields);
    if(ms->enlarged) { 
      om->collect_now |= OMCollectMature;
    }
    
    loc = GC_MATURE_OBJECTS;    
  } else {
    //fields += 4; /* PAD */
    size = (HEADER_SIZE + fields) * REFSIZE;
    if(!heap_enough_space_p(om->gc->current, size)) {
      obj = (OBJECT)baker_gc_allocate_spilled(om->gc, size);
      xassert(heap_enough_space_p(om->gc->next, size));
      // DEBUG("Ran out of space! spilled into %p\n", obj);
      om->collect_now |= OMCollectYoung;
      // baker_gc_enlarge_next(om->gc, om->gc->current->size * GC_SCALING_FACTOR);
    } else {
      obj = (OBJECT)baker_gc_allocate(om->gc, size);
    }
    
    loc = GC_YOUNG_OBJECTS;
  }
  
  // memset(obj, 0, HEADER_SIZE * REFSIZE);
  
  header = (struct rubinius_object*)obj;
  header->flags2 = 0;
  header->gc = 0;
  
  GC_ZONE_SET(obj, loc);
  
  rbs_set_class(om, obj, cls);
  SET_NUM_FIELDS(obj, fields);
  if(cls && REFERENCE_P(cls)) {
    /* #define CLASS_f_INSTANCE_FLAGS 8 */
    flags = NTH_FIELD(cls, 8);
    f = FIXNUM_TO_INT(flags);
    header->flags = f;
  } else {
    header->flags = 0;
  }

  return obj;
}

static inline OBJECT _om_inline_new_object_init(object_memory om, OBJECT cls, int fields) {
  OBJECT obj;
  obj = _om_inline_new_object(om, cls, fields);
  fast_memfill((void*)BYTES_OF(obj), Qnil, fields);
  
  return obj;
}

static inline OBJECT _om_new_ultra(object_memory om, OBJECT cls, int size) {
  OBJECT obj;
  /* I'd love to remove this check, but context allocation could flow over. */
  if(!heap_enough_space_p(om->gc->current, size)) {
    obj = (OBJECT)baker_gc_allocate_spilled_ultra(om->gc, size);
    om->collect_now |= OMCollectYoung;
  } else {
    obj = (OBJECT)baker_gc_allocate_ultra(om->gc, size);
  }
  
  rbs_set_class(om, obj, cls);
  HEADER(obj)->flags = 0;
  HEADER(obj)->flags2 = 0;
  HEADER(obj)->gc = 0;
  
  GC_ZONE_SET(obj, GC_YOUNG_OBJECTS);
  
  return obj; 
}

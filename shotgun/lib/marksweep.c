/*

 A mark/sweep GC.
 
 Starting at the roots, all objects that are
 reachable are marked, regardless if they are
 located in the mark/sweep or not (ie, they could
 be in another generation).
 
 Then, all objects allocated under the mark/sweep
 are traversed and any that aren't marked are
 placed by in the free_list.
 
 Finally, all objects in other generations have
 their mark cleared.
 
*/

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/object.h"
#include "shotgun/lib/cpu.h"
#include "shotgun/lib/methctx.h"
#include "shotgun/lib/bytearray.h"
#include "shotgun/lib/tuple.h"

#define to_header(obj) ((struct ms_header*)((uintptr_t)(obj) - sizeof(struct ms_header)))
#define to_object(hed) ((OBJECT)((uintptr_t)(hed) + sizeof(struct ms_header)))

#define FREE_OBJECT 0x10000
#define BARRIER (2**SIZE_OF_OBJECT)

#define TABLE_INCS 4096

#define FREE_FLAG 0xff

#define TRACK_REFERENCE 0
#define TRACK_DONT_FREE 0

#undef MS_COLLECTION_FREQUENCY
#define MS_COLLECTION_FREQUENCY 5000

// 2 Megs
#define MS_COLLECTION_BYTES 10485760


mark_sweep_gc mark_sweep_new() {
  mark_sweep_gc ms;
  
  ms = calloc(1, sizeof(struct _mark_sweep_gc));
  ms->remember_set = ptr_array_new(8);
  mark_sweep_add_chunk(ms);
  ms->chunks = ms->current;
  ms->enlarged = 0;
  ms->seen_weak_refs = NULL;
  ms->next_collection_objects = MS_COLLECTION_FREQUENCY;
  ms->next_collection_bytes =   MS_COLLECTION_BYTES;
  ms->become_from = ms->become_to = Qnil;
#if TRACK_REFERENCE
  if(getenv("MSTRACK")) {
    ms->track = (OBJECT)atoi(getenv("MSTRACK"));
  }
#endif
  return ms;
}

void mark_sweep_destroy(mark_sweep_gc ms) {
  int i, count;
  struct ms_entry *ent;
  ms_chunk *cur, *old;
  
  cur = ms->chunks;
  count = 0;
  
  while(cur) {
      
    for(i = 0; i < cur->num_entries; i++) {
      ent = &(cur->entries[i]);
      if(ent->bytes) {
        free(ent->object);
      }
    }
    
    old = cur;
    cur = cur->next;
    free(old->entries);
    free(old);
  }
 
  free(ms);
}

void mark_sweep_add_chunk(mark_sweep_gc ms) {
  ms_chunk *new;
  struct ms_entry *ent;
  int i;
  
  ms->enlarged = 1;
  
  new = calloc(1, sizeof(struct ms_chunk));
  
  ms->num_chunks++;
  new->num_entries = TABLE_INCS;
  new->entries = calloc(new->num_entries, sizeof(struct ms_entry));
  new->next = NULL;
  new->next_entry = 0;
  
  if(!ms->current) {
    ms->current = new;
  } else {
    ms->current->next = new;
    ms->current = new;
  }
  for(i = 0; i < TABLE_INCS; i++) {
    ent = new->entries + i;
    ent->object = (struct ms_header*)(ms->free_list);
    ms->free_list = ent;
  }
}


OBJECT mark_sweep_allocate(mark_sweep_gc ms, int obj_fields) {
  int bytes;
  struct ms_header *oh;
  struct ms_entry *ent;
  OBJECT ro;
  
  bytes = sizeof(struct ms_header) + SIZE_IN_BYTES_FIELDS(obj_fields);
  
  ent = ms->free_list;
  
  if(!ent) {
    mark_sweep_add_chunk(ms);
    ent = ms->free_list;
  }
  
  ms->free_list = (struct ms_entry*)ent->object;
  oh = (struct ms_header*)calloc(1, bytes);
  ent->object = oh;
  ent->fields = obj_fields;
  ent->bytes = bytes;
  ent->marked = 0;
  
  ms->allocated_bytes += bytes;
  ms->last_allocated += bytes;
  ms->allocated_objects++;
  // ms->next_collection_objects--;
  ms->next_collection_bytes -= bytes;
  
  if(obj_fields > 10000) {
    // printf("[GC M allocating large object %d]\n", obj_fields);
  }
  
  if(!ms->enlarged) {
    
    if(ms->next_collection_objects <= 0) {
      // printf("[GC M Collecting based on objects]\n");
      ms->enlarged = 1;
      ms->next_collection_bytes = MS_COLLECTION_FREQUENCY;
    } else if(ms->next_collection_bytes <= 0) {
      // printf("[GC M Collecting based on bytes: %d]\n", ms->allocated_bytes);
      ms->enlarged = 1;
    }
    
  
  /*  
  
    if(ms->last_allocated_bytes) {
      if(ms->last_allocated_bytes * 2 <= ms->allocated_bytes) {
        printf("[GC M Collecting based on heap size (%d, %d)]\n", ms->last_allocated_bytes, ms->allocated_bytes);
        ms->enlarged = 1;
      }
    } else if(ms->last_allocated >= ms->next_collection) {
      curt = clock();
      ms->last_allocated = 0;
      ms->enlarged = 1;
      printf("[GC M Collecting based on bytes (every %d bytes, %d total)]\n", MS_COLLECTION_BYTES, ms->allocated_bytes);
      printf("[GC M last_clock=%d, cur=%d]\n", ms->last_clock, curt);
      ms->last_clock = curt;
    }
    */
  }
  
  oh->entry = ent;
  ro = to_object(oh);
  // printf("Allocated %d\n", ro);
  SET_NUM_FIELDS(ro, obj_fields);
  
#if TRACK_REFERENCE
  if(ms->track == ro) {
    printf("Allocated tracked object %p\n", ro);
  }
#endif

    
  return ro;
}

void mark_sweep_free_entry(STATE, mark_sweep_gc ms, struct ms_entry *ent) {
  OBJECT obj;
  
  obj = to_object(ent->object);
  
#if TRACK_REFERENCE
  if(ms->track == obj) {
    printf("Free'ing tracked object %p\n", obj);
  }
#endif
  
  if(obj->Remember) {
    ptr_array_remove_fast(state->om->gc->remember_set, (xpointer)obj);
  }
    
  if(obj->RequiresCleanup) {
    if(obj->obj_type == MemPtrType) {
      void *addr = *DATA_STRUCT(obj, void**);
      if(addr) free(addr);
      obj->RequiresCleanup = 0;
    } else {
      state_run_cleanup(state, obj);
    }
  }
  
  if(obj->obj_type == WrapsStructType) FREE_WRAPPED_STRUCT(obj);
  
#if TRACK_DONT_FREE
  memset(obj, 0, SIZE_IN_BYTES(obj));
  *((int*)(obj)) = 0xbaddecaf;
#else
  free(ent->object);
#endif
  
  ms->last_freed++;
  
  ms->allocated_bytes -= ent->bytes;
  ms->allocated_objects--;
  
  ent->bytes = 0;
  ent->marked = 0;
  ent->fields = 0;
  
  ent->object = (struct ms_header*)(ms->free_list);
  ms->free_list = ent;
}

#define mark_sweep_free_object(obj) (obj->flags == FREE_FLAG)

void mark_sweep_describe(mark_sweep_gc ms) {
  printf("Last marked: %d\n", ms->last_marked);
}

#define BCM_P(obj) (ms->become_from == obj)
#define BCM_TO (ms->become_to)

int _object_stores_bytes(OBJECT self);

int mark_sweep_contains_p(mark_sweep_gc ms, OBJECT obj) {
  return obj->gc_zone == MatureObjectZone;
}


static OBJECT mark_sweep_mark_object(STATE, mark_sweep_gc ms, OBJECT iobj) {
  OBJECT cls, tmp;
  int i;
  struct ms_header *header;
  
#if TRACK_REFERENCE
  if(ms->track == iobj) {
    printf("Found tracked object %p\n", iobj);
  }
#endif
    
  if(iobj->gc_zone == MatureObjectZone) {
    header = to_header(iobj);
    
    assert(header->entry->object == header);
        
    /* Already marked! */
    if(header->entry->marked) return iobj;
    header->entry->marked = 1;
    
    if(iobj->obj_type == WrapsStructType) MARK_WRAPPED_STRUCT(iobj);
  } else {
    if(iobj->Marked) return iobj;
    
    iobj->Marked = TRUE;
    if(iobj->obj_type == WrapsStructType) MARK_WRAPPED_STRUCT(iobj);
  }
  
  // printf("Marked %d\n", iobj);
  
  ms->last_marked++;
  
  cls = CLASS_OBJECT(iobj);
  if(REFERENCE_P(cls)) {
    if(BCM_P(cls)) {
      SET_CLASS(iobj, BCM_TO);
    } else {
      mark_sweep_mark_object(state, ms, cls);
    }
  }
  
  if(iobj->RefsAreWeak) {
    // printf("%p has weak refs.\n", (void*)iobj);
    ptr_array_append(ms->seen_weak_refs, (xpointer)iobj);
    return iobj;
  }
  
  if(!_object_stores_bytes(iobj)) {
    for(i = 0; i < NUM_FIELDS(iobj); i++) {
      tmp = NTH_FIELD(iobj, i);
      if(!REFERENCE_P(tmp)) continue;
      
      if(BCM_P(tmp)) {
        SET_FIELD(iobj, i, BCM_TO);
      } else {
        mark_sweep_mark_object(state, ms, tmp);
      }
    }
  } else {
#define fc_mutate(field) if(REFERENCE2_P(fc->field)) { \
    if(BCM_P(fc->field)) { fc->field = BCM_TO; \
    } else { mark_sweep_mark_object(state, ms, fc->field); } }
    
    if(methctx_is_context_p(state, iobj)) {
      struct fast_context *fc = FASTCTX(iobj);
      fc_mutate(sender);
      
      fc_mutate(block);
      fc_mutate(method);
      fc_mutate(literals);
      fc_mutate(self);
      fc_mutate(locals);
      fc_mutate(method_module);
      fc_mutate(name);
      
      if(!NIL_P(fc->method) && fc->method->obj_type == CMethodType) {
        /* We cache the bytecode in a char*, so adjust it. */
        OBJECT ba;
        ba = cmethod_get_compiled(fc->method);
        fc->data = BYTEARRAY_ADDRESS(ba);
      }
    } else if(iobj->obj_type == TaskType) {
      struct cpu_task *fc = (struct cpu_task*)BYTES_OF(iobj);
      
      if(!fc->active) {
        fc_mutate(exception);
        fc_mutate(enclosing_class);
        fc_mutate(active_context);
        fc_mutate(home_context);
        fc_mutate(main);
        fc_mutate(debug_channel);
        fc_mutate(control_channel);
        fc_mutate(current_scope);
      
        OBJECT *sp;

        sp = fc->stack_top;
        while(sp <= fc->sp_ptr) {
          if(REFERENCE2_P(*sp)) {
            if(BCM_P(*sp)) {
              *sp = BCM_TO;
            } else {
              mark_sweep_mark_object(state, ms, *sp);
            }
          }
          sp++;
        }
      
        int i;
        for(i = 0; i < ptr_array_length(fc->paths); i++) {
          tmp = (OBJECT)ptr_array_get_index(fc->paths, i);
          if(BCM_P(tmp)) {
            ptr_array_set_index(fc->paths,i,(xpointer)BCM_TO);
          } else {
            mark_sweep_mark_object(state, ms, tmp);
          }
        }
      }
    }
#undef fc_mutate    
  }
  
  return iobj;
}

void mark_sweep_mark_context(STATE, mark_sweep_gc ms, OBJECT iobj) {
  #define fc_mutate(field) if(fc->field && REFERENCE_P(fc->field)) mark_sweep_mark_object(state, ms, fc->field)
  if (iobj->Marked) return;

  iobj->Marked = TRUE;
  
  struct fast_context *fc = FASTCTX(iobj);

  if(REFERENCE_P(fc->sender) && om_in_heap(state->om, fc->sender)) {
    fc_mutate(sender);
  }

  fc_mutate(method);
  fc_mutate(block);
  fc_mutate(literals);
  fc_mutate(self);
  fc_mutate(locals);
  fc_mutate(method_module);
  fc_mutate(name);
  fc_mutate(locals);
  
  if(!NIL_P(fc->method) && fc->method->obj_type == CMethodType) {
    /* We cache the bytecode in a char*, so adjust it. */
    OBJECT ba;
    ba = cmethod_get_compiled(fc->method);
    fc->data = BYTEARRAY_ADDRESS(ba);
  }
#undef fc_mutate
}

void mark_sweep_clear_mark(STATE, OBJECT iobj) {
  iobj->Marked = FALSE;
}

void mark_sweep_mark_phase(STATE, mark_sweep_gc ms, ptr_array roots) {
  int i, sz;
  OBJECT root, tmp;
    
  if(!NIL_P(ms->become_to)) {
    mark_sweep_mark_object(state, ms, ms->become_to);
  }
  
  if(!NIL_P(ms->become_from)) {
    mark_sweep_mark_object(state, ms, ms->become_from);
  }
  
  sz = ptr_array_length(roots);
  for(i = 0; i < sz; i++) {
    root = (OBJECT)(ptr_array_get_index(roots, i));
    if(!REFERENCE2_P(root)) { continue; }
    if(BCM_P(root)) {
      ptr_array_set_index(roots, i, (xpointer)BCM_TO);
    } else {
      mark_sweep_mark_object(state, ms, root);
    }
  }
  
  sz = ptr_array_length(ms->remember_set);
  for(i = 0; i < sz; i++) {
    root = (OBJECT)(ptr_array_get_index(ms->remember_set, i));
    if(!REFERENCE2_P(root)) { continue; }
    if(BCM_P(root)) {
      ptr_array_set_index(ms->remember_set, i, (xpointer)BCM_TO);
    } else {
      mark_sweep_mark_object(state, ms, root);
    }
  }
  
  /* Now the stack. */
  OBJECT *sp;
  
  sp = state->current_stack;
  while(sp <= state->current_sp) {
    if(REFERENCE2_P(*sp)) {
      if(BCM_P(*sp)) {
        *sp = BCM_TO;
      } else {
        mark_sweep_mark_object(state, ms, *sp);
      }
    }
    sp++;
  }
  
  /* Now the handle table. */
  for(i = 0; i < state->handle_tbl->total; i++) {
    if(state->handle_tbl->entries[i]) {
      tmp = state->handle_tbl->entries[i]->object;
      if(BCM_P(tmp)) {
        state->handle_tbl->entries[i]->object = BCM_TO;
      } else {
        mark_sweep_mark_object(state, ms, tmp);
      }
    }
  }
  
  cpu_event_each_channel(state,
      (cpu_event_each_channel_cb) mark_sweep_mark_object, ms);
  cpu_sampler_collect(state,
      (cpu_sampler_collect_cb) mark_sweep_mark_object, ms);
                      
  object_memory_mark_contexts(state, state->om);
}

void mark_sweep_sweep_phase(STATE, mark_sweep_gc ms) {
  int i, count;
  OBJECT obj;
  struct ms_entry *ent;
  ms_chunk *cur;
  
  cur = ms->chunks;
  count = 0;
  
  while(cur) {
      
    for(i = 0; i < cur->num_entries; i++) {
      ent = &(cur->entries[i]);
      count++;
      if(ent->bytes) {
        obj = to_object(ent->object);
#if TRACK_REFERENCE
        if(ms->track == obj) {
          printf("Detect tracked object in sweep %p\n", obj);
        }
#endif
        
        assert(ent->fields == NUM_FIELDS(obj));
        if(!ent->marked) {
          mark_sweep_free_entry(state, ms, ent);
        } else {
          obj->Marked = FALSE;
          ent->marked = 0;
        }
      }
    }
    
    cur = cur->next;
  }
  
}

void mark_sweep_collect(STATE, mark_sweep_gc ms, ptr_array roots) {
  struct method_cache *end, *ent;
  ms->enlarged = 0;
  ms->last_freed = 0;
  ms->last_marked = 0;
  
  ms->seen_weak_refs = ptr_array_new(8);
  mark_sweep_mark_phase(state, ms, roots);
  
  /* We handle the method cache a little differently. We treat it like every
   * ref is weak so that it doesn't cause objects to live longer than they should. */

  ent = state->method_cache;
  end = ent + CPU_CACHE_SIZE;
  
  while(ent < end) {
    if(ent->klass) {
      if(ent->klass->gc_zone == MatureObjectZone) {
        if(!to_header(ent->klass)->entry->marked) {
          ent->klass = 0;
        }
      }
    }
    
    if(ent->module) {
      if(ent->module->gc_zone == MatureObjectZone) {
        if(!to_header(ent->module)->entry->marked) {
          ent->module = 0;
        }
      }
    }
    
    if(ent->method) {
      if(ent->method->gc_zone == MatureObjectZone) {
        if(!to_header(ent->method)->entry->marked) {
          ent->method = 0;
        }
      }
    }
    
    if(!ent->klass || !ent->module || !ent->method) {
      ent->name = 0;
      ent->klass = 0;
      ent->module = 0;
      ent->method = 0;
    }
    
    ent++;
  }
 
  /* We update the weakrefs BEFORE we sweep, since the sweep will free
   * the data for the object, and we'll loose important info. */
  int j, i;
  OBJECT tmp, t2;
  for(i = 0; i < ptr_array_length(ms->seen_weak_refs); i++) {
    tmp = (OBJECT)ptr_array_get_index(ms->seen_weak_refs, i);
    for(j = 0; j < NUM_FIELDS(tmp); j++) {
      t2 = tuple_at(state, tmp, j);
      if(REFERENCE_P(t2) && t2->gc_zone == MatureObjectZone) {
        if(!to_header(t2)->entry->marked) {
          tuple_put(state, tmp, j, Qnil);
        }
      }
    }
  }
  
  mark_sweep_sweep_phase(state, ms);
  
  ptr_array_free(ms->seen_weak_refs);
  ms->seen_weak_refs = NULL;
  
  // printf("[GC M compacted to %d bytes]\n", ms->allocated_bytes);
  ms->next_collection_bytes = ms->allocated_bytes + MS_COLLECTION_BYTES;
}

void mark_sweep_collect_references(STATE, mark_sweep_gc ms, OBJECT mark, ptr_array refs) {
  int i, count;
  OBJECT obj;
  struct ms_entry *ent;
  ms_chunk *cur;
  
  cur = ms->chunks;
  count = 0;
  
  while(cur) {
      
    for(i = 0; i < cur->num_entries; i++) {
      ent = &(cur->entries[i]);
      count++;
      if(ent->bytes) {
        obj = to_object(ent->object);
        
        assert(ent->fields == NUM_FIELDS(obj));

        int j;
        if(!_object_stores_bytes(obj)) {
          for(j = 0; j < NUM_FIELDS(obj); j++) {
            if(NTH_FIELD(obj, j) == mark) {
              ptr_array_append(refs, (xpointer)obj);
            }
          }
        }
      }
    }
    
    cur = cur->next;
  }
  
}

/*

void mark_sweep_collect_references(STATE, mark_sweep_gc ms, OBJECT mark, ptr_array refs) {
  ms_chunk *cur;
  char *addr, *last;
  OBJECT obj;
  int osz, used, i;
  
  cur = ms->chunks;
  
  freed_objects = 0;
  
  while(cur) {
    
    addr = (char*)(cur->address);
    last = (addr + cur->size) - (sizeof(struct rubinius_object));
    
    used = 0;
    while(addr < last) {
      obj = (OBJECT)addr;
      osz = SIZE_IN_BYTES(obj);
      
      if(obj->flags != FREE_FLAG && !_object_stores_bytes(obj)) {
        for(i = 0; i < NUM_FIELDS(obj); i++) {
          if(NTH_FIELD(obj, i) == mark) {
            ptr_array_append(refs, (xpointer)obj);
          }
        }
      }
      addr += osz;
    }
    
    cur = cur->next;
  } 
}

*/

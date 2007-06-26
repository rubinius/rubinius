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

#include "shotgun.h"
#include "object.h"
#include "methctx.h"
#include "cpu.h"
#include "bytearray.h"
#include "tuple.h"

#define to_header(obj) ((struct ms_header*)((obj) - sizeof(struct ms_header)))
#define to_object(hed) ((OBJECT)(((OBJECT)(hed)) + sizeof(struct ms_header)))

#define FREE_OBJECT 0x10000
#define BARRIER (2**REFSIZE)

#define TABLE_INCS 4096

#define FREE_FLAG 0xff

static int objects_marked = 0;

mark_sweep_gc mark_sweep_new() {
  mark_sweep_gc ms;
  
  ms = calloc(1, sizeof(struct _mark_sweep_gc));
  ms->remember_set = g_ptr_array_new();
  mark_sweep_add_chunk(ms);
  ms->chunks = ms->current;
  ms->enlarged = 0;
  ms->seen_weak_refs = NULL;
  return ms;
}

void mark_sweep_add_chunk(mark_sweep_gc ms) {
  ms_chunk *new;
  
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
}

OBJECT mark_sweep_allocate(mark_sweep_gc ms, int obj_fields) {
  int bytes, idx;
  struct ms_header *oh;
  ms_chunk *chk;
  OBJECT ro;
  
  bytes = sizeof(struct ms_header) + ((obj_fields + HEADER_SIZE) * REFSIZE);
  
  chk = ms->current;
  
  if(chk->next_entry == chk->num_entries) {
    mark_sweep_add_chunk(ms);
    chk = ms->current;
  }
  
  idx = chk->next_entry;
  chk->next_entry++;
  
  oh = (struct ms_header*)calloc(1, bytes);
  assert(oh != NULL);
  chk->entries[idx].object = oh;
  chk->entries[idx].fields = obj_fields;
  chk->entries[idx].bytes = bytes;
  chk->entries[idx].marked = 0;
  
  oh->entry = chk->entries + idx;
  ro = to_object(oh);
  // printf("Allocated %d\n", ro);
  SET_NUM_FIELDS(ro, obj_fields);
    
  return ro;
}

void mark_sweep_free_entry(STATE, mark_sweep_gc ms, struct ms_entry *ent) {
  OBJECT obj;
  
  obj = to_object(ent->object);
  
  // printf("Freeing %d\n", obj);
  
  if(FLAG_SET_ON_P(obj, gc, REMEMBER_FLAG)) {
    g_ptr_array_remove(state->om->gc->remember_set, (gpointer)obj);
  }
    
  if(SHOULD_CLEANUP_P(obj)) {
    state_run_cleanup(state, obj);
  }
  
  free(ent->object);
  
  ent->object = NULL;
  ent->bytes = 0;
  ent->marked = 0;
  ent->fields = 0;
}

static int freed_objects = 0;
static int marked_objects = 0;

#define mark_sweep_free_object(obj) (HEADER(obj)->flags == FREE_FLAG)

void mark_sweep_describe(mark_sweep_gc ms) {
  printf("Last marked: %d\n", objects_marked);
}

#define BCM_P(obj) (ms->become_from == obj)
#define BCM_TO (ms->become_to)

int _object_stores_bytes(OBJECT self);

int mark_sweep_contains_p(mark_sweep_gc ms, OBJECT obj) {
  return GC_ZONE(obj) == GC_MATURE_OBJECTS;
}

#define MARK_OBJ(obj) (HEADER(obj)->gc |= MS_MARK)
#define UNMARK_OBJ(obj) (HEADER(obj)->gc ^= MS_MARK)
#define MARKED_P(obj) (HEADER(obj)->gc & MS_MARK)

OBJECT mark_sweep_mark_object(STATE, mark_sweep_gc ms, OBJECT iobj) {
  OBJECT cls, tmp;
  int i;
  struct ms_header *header;
    
  if(GC_ZONE(iobj) == GC_MATURE_OBJECTS) {
    header = to_header(iobj);
    
    assert(header->entry->object == header);
        
    /* Already marked! */
    if(header->entry->marked) return iobj;
    header->entry->marked = 1;
    
  } else {
    if(MARKED_P(iobj)) return iobj;
    MARK_OBJ(iobj);
  }
  
  // printf("Marked %d\n", iobj);
  
  objects_marked++;
  marked_objects++;
  
  cls = CLASS_OBJECT(iobj);
  if(REFERENCE_P(cls)) {
    if(BCM_P(cls)) {
      SET_CLASS(iobj, BCM_TO);
    } else {
      mark_sweep_mark_object(state, ms, cls);
    }
  }
  
  if(WEAK_REFERENCES_P(iobj)) {
    // printf("%p has weak refs.\n", (void*)iobj);
    g_ptr_array_add(ms->seen_weak_refs, (gpointer)iobj);
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
    
    if(methctx_is_fast_p(state, iobj)) {
      struct fast_context *fc = FASTCTX(iobj);
      fc_mutate(sender);
      fc_mutate(block);
      fc_mutate(method);
      fc_mutate(literals);
      fc_mutate(self);
      fc_mutate(locals);
      fc_mutate(method_module);

      /* We cache the bytecode in a char*, so adjust it. */
      OBJECT ba;
      ba = cmethod_get_bytecodes(fc->method);
      fc->data = BYTEARRAY_ADDRESS(ba);
      fc->data_size = BYTEARRAY_SIZE(ba);
    } else if(ISA(iobj, BASIC_CLASS(task))) {
      struct cpu_task *fc = (struct cpu_task*)BYTES_OF(iobj);
      
      fc_mutate(exception);
      fc_mutate(new_class_of);
      fc_mutate(enclosing_class);
      fc_mutate(top_context);
      fc_mutate(exceptions);
      fc_mutate(active_context);
      fc_mutate(home_context);
      fc_mutate(main);
      fc_mutate(outstanding);
      fc_mutate(debug_channel);
      fc_mutate(control_channel);
      
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
      for(i = 0; i < fc->paths->len; i++) {
        tmp = (OBJECT)g_ptr_array_index(fc->paths, i);
        if(BCM_P(tmp)) {
          fc->paths->pdata[i] = (gpointer)BCM_TO;
        } else {
          mark_sweep_mark_object(state, ms, tmp);
        }
      }
    }
#undef fc_mutate    
  }
  
  return iobj;
}

void mark_sweep_mark_phase(STATE, mark_sweep_gc ms, GPtrArray *roots) {
  int i, sz;
  OBJECT root, tmp;
  struct method_cache *end, *ent;
  
  marked_objects = 0;
  
  if(ms->become_to) {
    mark_sweep_mark_object(state, ms, ms->become_to);
  }
  
  if(ms->become_from) {
    mark_sweep_mark_object(state, ms, ms->become_from);
  }
  
  sz = roots->len;
  for(i = 0; i < sz; i++) {
    root = (OBJECT)(g_ptr_array_index(roots, i));
    if(!REFERENCE2_P(root)) { continue; }
    if(BCM_P(root)) {
      roots->pdata[i] = (gpointer)BCM_TO;
    } else {
      mark_sweep_mark_object(state, ms, root);
    }
  }
  
  sz = ms->remember_set->len;
  for(i = 0; i < sz; i++) {
    root = (OBJECT)(g_ptr_array_index(ms->remember_set, i));
    if(!REFERENCE2_P(root)) { continue; }
    if(BCM_P(root)) {
      ms->remember_set->pdata[i] = (gpointer)BCM_TO;
    } else {
      mark_sweep_mark_object(state, ms, root);
    }
  }
  
  ent = state->method_cache;
  end = ent + CPU_CACHE_SIZE;
  
  while(ent < end) {
    if(ent->klass) {
      if(BCM_P(ent->klass)) {
        ent->klass = BCM_TO;
      } else {
        mark_sweep_mark_object(state, ms, ent->klass);
      }
    }
     
    if(ent->module) {
      if(BCM_P(ent->module)) {
        ent->module = BCM_TO;
      } else {
        mark_sweep_mark_object(state, ms, ent->module);
      }
    }
      
    if(ent->method) {
      if(BCM_P(ent->method)) {
        ent->method = BCM_TO;
      } else {
        mark_sweep_mark_object(state, ms, ent->method);
      }
    }

    ent++;
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
                         (cpu_event_each_channel_cb) mark_sweep_mark_object,
                         ms);
  cpu_sampler_collect(state,
                      (cpu_sampler_collect_cb) mark_sweep_mark_object,
                      ms);
  
  // printf("Marked Objects: %d\n", marked_objects);
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
      if(ent->object) {
        obj = to_object(ent->object);
        assert(ent->fields == NUM_FIELDS(obj));
        if(!ent->marked) {
          mark_sweep_free_entry(state, ms, ent);
        } else {
          UNMARK_OBJ(obj);
          ent->marked = 0;
        }
      }
    }
    
    cur = cur->next;
  }
  
}

void mark_sweep_collect(STATE, mark_sweep_gc ms, GPtrArray *roots) {
  ms->enlarged = 0;
  objects_marked = 0;
  ms->seen_weak_refs = g_ptr_array_new();  
  mark_sweep_mark_phase(state, ms, roots);
  // printf("%d objects marked.\n", objects_marked);
  mark_sweep_sweep_phase(state, ms);
  
  int j, i;
  OBJECT tmp, t2;
  for(i = 0; i < ms->seen_weak_refs->len; i++) {
    tmp = (OBJECT)g_ptr_array_index(ms->seen_weak_refs, i);
    for(j = 0; j < NUM_FIELDS(tmp); j++) {
      t2 = tuple_at(state, tmp, j);
      if(REFERENCE_P(t2) && GC_ZONE(t2) == GC_MATURE_OBJECTS) {
        if(!to_header(t2)->entry->object) {
          tuple_put(state, tmp, j, Qnil);
        }
      }
    }
  }
  
  g_ptr_array_free(ms->seen_weak_refs, TRUE);
  ms->seen_weak_refs = NULL;
}

/*

void mark_sweep_collect_references(STATE, mark_sweep_gc ms, OBJECT mark, GPtrArray *refs) {
  ms_chunk *cur;
  char *addr, *last;
  OBJECT obj;
  int osz, used, i;
  
  cur = ms->chunks;
  
  freed_objects = 0;
  
  while(cur) {
    
    addr = (char*)(cur->address);
    last = (addr + cur->size) - (HEADER_SIZE * REFSIZE);
    
    used = 0;
    while(addr < last) {
      obj = (OBJECT)addr;
      osz = SIZE_IN_BYTES(obj);
      
      if(HEADER(obj)->flags != FREE_FLAG && !_object_stores_bytes(obj)) {
        for(i = 0; i < NUM_FIELDS(obj); i++) {
          if(NTH_FIELD(obj, i) == mark) {
            g_ptr_array_add(refs, (gpointer)obj);
          }
        }
      }
      addr += osz;
    }
    
    cur = cur->next;
  } 
}

*/

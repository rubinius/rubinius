#include "shotgun.h"
#include <stdlib.h>
#include <string.h>
#include "heap.h"
#include "methctx.h"
#include "cpu.h"
#include "bytearray.h"
#include "baker.h"
#include "tuple.h"

static int promoted = 0;

#define DEFAULT_TENURE_AGE 6

baker_gc baker_gc_new(int size) {
  baker_gc g;
  g = (baker_gc)calloc(1, sizeof(struct baker_gc_struct));
  g->remember_set = g_ptr_array_new();
  g->seen_weak_refs = g_ptr_array_new();
  
  g->space_a = heap_new(size);
  g->space_b = heap_new(size);
  g->current = g->space_a;
  g->next =    g->space_b;
  g->used =    0;
  g->tenure_age = DEFAULT_TENURE_AGE;
  return g;
}

void baker_gc_describe(baker_gc g) {
  printf("Size:    %x (%d)\n", g->current->size, g->current->size);
  printf("Used:    %d\n", g->used);
  printf("Current: %p => %p\n", (void*)g->current->address, 
      (void*)g->current->last);
  printf("Next:    %p => %p\n", (void*)g->next->address, (void*)g->next->last);
  printf("RS Size: %d\n", g->remember_set->len);
  printf("Promoted last: %d\n", promoted);
}

int baker_gc_enlarge_next(baker_gc g, int sz) {
  rheap h;
  h = heap_new(sz);
  return baker_gc_set_next(g, h);
}

int baker_gc_set_next(baker_gc g, rheap h) {
  if(g->next == g->space_a) {
    g->space_a = h;
  } else {
    g->space_b = h;
  }
  g->next = h;
  return TRUE;
}

address baker_gc_start_address(baker_gc g) {
  return g->current->address;
}

int baker_gc_used(baker_gc g) {
  return g->used;
}

int baker_gc_swap(baker_gc g) {
  rheap tmp;
  tmp = g->current;
  g->current = g->next;
  g->next = tmp;
  
  heap_reset(tmp);
  /* Reset used to the what the current has used. */
  g->used = g->current->current - g->current->address;
  return TRUE;
}

int baker_gc_destroy(baker_gc g) {
  heap_deallocate(g->space_a);
  heap_deallocate(g->space_b);
  return TRUE;
}

#define FORWARDING_MAGIC 0xff

void baker_gc_set_forwarding_address(OBJECT obj, OBJECT dest) {
  HEADER(obj)->flags = FORWARDING_MAGIC;
  HEADER(obj)->klass = dest;
}

static inline int baker_gc_forwarded_p(OBJECT obj) {
  return HEADER(obj)->flags == FORWARDING_MAGIC;
}

OBJECT baker_gc_forwarded_object(OBJECT obj) {
  OBJECT out = HEADER(obj)->klass;
  CHECK_PTR(out);
  return out;
}

#define AGE(obj) (HEADER(obj)->gc & 0x7f)
#define CLEAR_AGE(obj) (HEADER(obj)->gc = 0)
#define FOREVER_YOUNG(obj) (HEADER(obj)->gc & 0x8000)

#define baker_gc_maybe_mutate(st, g, iobj) ({     \
  OBJECT ret;                                 \
  if(baker_gc_forwarded_p(iobj)) {            \
    ret = baker_gc_forwarded_object(iobj);    \
  } else if(baker_gc_contains_p(g, iobj)) {   \
    ret = baker_gc_mutate_object(st, g, iobj);    \
  } else {                                    \
    ret = iobj;                               \
  }                                           \
  ret; })

/*
static inline OBJECT baker_gc_maybe_mutate(baker_gc g, OBJECT iobj) {
  OBJECT ret;

  if(baker_gc_forwarded_p(iobj)) {
    ret = baker_gc_forwarded_object(iobj);
  } else if(baker_gc_contains_p(g, iobj)) {
    ret = baker_gc_mutate_object(g, iobj);
  } else {
    ret = iobj;
  }
  
  // assert(baker_gc_contains_spill_p(g, ret));
    
  CHECK_PTR(ret);
  
  return ret;
}
*/

int _object_stores_bytes(OBJECT self);
static int depth = 0;
static GPtrArray *_track_refs = NULL;

void track_ref(OBJECT obj) {
  if(!_track_refs) {
    _track_refs = g_ptr_array_new();
  }
  g_ptr_array_add(_track_refs, (gpointer)obj);
}

void untrack_ref(OBJECT obj) {
  if(!_track_refs) return;
  g_ptr_array_remove(_track_refs, (gpointer)obj);
}

static inline void _mutate_references(STATE, baker_gc g, OBJECT iobj) {
  OBJECT cls, tmp, mut;
  int i;
  
  if(_track_refs) {
    int i;
    for(i = 0; i < _track_refs->len; i++) {
      if(_track_refs->pdata[i] == (gpointer)iobj) {
        printf("Found2 %p!\n", (void*)iobj);
      }
    }
  }
  
  //printf("%d: Mutating class of %p\n", ++depth, iobj);
  
  cls = CLASS_OBJECT(iobj);
  if(REFERENCE_P(cls)) {
    cls = baker_gc_maybe_mutate(state, g, cls);
  }
  
  SET_CLASS(iobj, cls);
  
  assert(HEADER(iobj)->flags != FORWARDING_MAGIC);
  
  if(WEAK_REFERENCES_P(iobj)) {
    // printf("%p has weak refs.\n", (void*)iobj);
    g_ptr_array_add(g->seen_weak_refs, (gpointer)iobj);
    depth--;
    return;
  }
  
  /*
  if(HAS_WEAK_REFS_P(iobj)) {
    printf("%p with weak refs is still alive.\n", (void*)iobj);
  }
  */
  
  //printf("%d: Mutating references of %p\n", depth, iobj);
  
  if(!_object_stores_bytes(iobj)) {
    for(i = 0; i < NUM_FIELDS(iobj); i++) {
      tmp = NTH_FIELD(iobj, i);
      if(!REFERENCE_P(tmp)) continue;
    
      mut = baker_gc_maybe_mutate(state, g, tmp);
      rbs_set_field(g->om, iobj, i, mut);
    }
  } else {
#define fc_mutate(field) if(fc->field && REFERENCE_P(fc->field)) SET_STRUCT_FIELD(iobj, fc->field, baker_gc_maybe_mutate(state, g, fc->field))
// #define fc_mutate(field) if(REFERENCE_P(fc->field)) fc->field = baker_gc_maybe_mutate(state, g, fc->field)
    if(methctx_is_fast_p(state, iobj)) {
      struct fast_context *fc = FASTCTX(iobj);
      fc_mutate(sender);
      fc_mutate(block);
      fc_mutate(method);
      fc_mutate(literals);
      fc_mutate(self);
      fc_mutate(locals);
      fc_mutate(method_module);

      /* We cache the bytecode in a char*, so adjust it. 
         We mutate the data first so we cache the newest address. */
      OBJECT ba;
      ba = cmethod_get_bytecodes(fc->method);
      ba = baker_gc_maybe_mutate(state, g, ba);
      
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
        if(*sp && REFERENCE_P(*sp)) {
          *sp = baker_gc_mutate_from(state, g, *sp);
        }
        sp++;
      }
      
      int i;
      for(i = 0; i < fc->paths->len; i++) {
        fc->paths->pdata[i] = 
          (gpointer)baker_gc_maybe_mutate(state, g, (OBJECT)g_ptr_array_index(fc->paths, i));
      }
    }
    
#undef fc_mutate
  }
  
  depth--;
}

OBJECT baker_gc_mutate_object(STATE, baker_gc g, OBJECT obj) {
  OBJECT dest;
  if(obj == g->become_from) {
    return baker_gc_maybe_mutate(state, g, g->become_to);
  }
  
  if(_track_refs) {
    int i;
    for(i = 0; i < _track_refs->len; i++) {
      if(_track_refs->pdata[i] == (gpointer)obj) {
        printf("Found %p!\n", (void*)obj);
      }
    }
  }
  
  if((AGE(obj) == g->tenure_age) && !FOREVER_YOUNG(obj)) {
    // int age = AGE(obj);
    assert(HEADER(obj)->klass != state->global->fastctx);
    CLEAR_AGE(obj);
    promoted++;
    dest = (*g->tenure)(g->tenure_data, obj);
    baker_gc_set_forwarding_address(obj, dest);
    //printf("Tenuring object %p to %p, age %d (%d).\n", obj, dest, age, g->tenure_now);
    _mutate_references(state, g, dest);
  } else {
    if(heap_enough_fields_p(g->next, NUM_FIELDS(obj))) {
      dest = heap_copy_object(g->next, obj);
      baker_gc_set_forwarding_address(obj, dest);
      HEADER(dest)->gc++;
    } else {
      CLEAR_AGE(obj);
      promoted++;
      dest = (*g->tenure)(g->tenure_data, obj);
      baker_gc_set_forwarding_address(obj, dest);
      _mutate_references(state, g, dest);
    }
  }
  
  if(_track_refs) {
    int i;
    for(i = 0; i < _track_refs->len; i++) {
      if(_track_refs->pdata[i] == (gpointer)dest) {
        printf("Found3 %p!\n", (void*)dest);
      }
    }
  }
  
  return dest;
}

int baker_gc_contains_p(baker_gc g, OBJECT obj) {
  return heap_contains_p(g->current, obj);
}

int baker_gc_contains_spill_p(baker_gc g, OBJECT obj) {
  return heap_contains_p(g->next, obj) || heap_contains_p(g->current, obj);
}


OBJECT baker_gc_mutate_from(STATE, baker_gc g, OBJECT orig) {
  OBJECT ret, iobj;
  
  //printf("!!!\n   => From %p\n!!!\n", iobj);
  
  ret = baker_gc_maybe_mutate(state, g, orig);
  
  // assert(baker_gc_contains_spill_p(g, ret));

  iobj = ret;
  
  while(iobj) {
    _mutate_references(state, g, iobj);
    iobj = heap_next_unscanned(g->next);
  }
  
  return ret;
}


#define g_ptr_array_set_index(ary, idx, val) (ary->pdata[idx] = (void*)val)

int baker_gc_collect(STATE, baker_gc g, GPtrArray *roots) {
  int i, sz;
  OBJECT tmp, root;
  struct method_cache *end, *ent;
  GPtrArray *rs;
  promoted = 0;
  
  /* empty it out. */
  g->seen_weak_refs->len = 0;
  
  // printf("Running garbage collector...\n");
  
  /* To maintain the remember set, we setup a totally new
     set before do any walking of objects, so that only objects
     which truely still contain pointers to this generation
     are added back to the new rs. */
  
  rs = g->remember_set;
  g->remember_set = g_ptr_array_new();
  
  sz = rs->len;
  for(i = 0; i < sz; i++) {
    root = (OBJECT)(g_ptr_array_index(rs, i));
    if(!REFERENCE2_P(root)) { continue; }
    FLAG_CLEAR_ON(root, gc, REMEMBER_FLAG);
    tmp = baker_gc_mutate_from(state, g, root);
    // g_ptr_array_set_index(g->remember_set, i, tmp);
  }
          
  sz = roots->len;
  for(i = 0; i < sz; i++) {
    root = (OBJECT)(g_ptr_array_index(roots, i));
    // printf("Collecting from root %d\n", i);
    // printf("%p => %p\n", g->current->address, g->next->address);
    // printf("Start at RS (%d): %p\n", i, root);
    if(!REFERENCE2_P(root)) { continue; }
    tmp = baker_gc_mutate_from(state, g, root);
    g_ptr_array_set_index(roots, i, tmp);
  }
  
  
  ent = state->method_cache;
  end = ent + CPU_CACHE_SIZE;
  
  while(ent < end) {
    if(ent->klass)
      ent->klass = baker_gc_mutate_from(state, g, ent->klass);
      
    if(ent->module)
      ent->module = baker_gc_mutate_from(state, g, ent->module);
      
    if(ent->method)
      ent->method = baker_gc_mutate_from(state, g, ent->method);

    ent++;
  }
  
  /* Now the stack. */
  OBJECT *sp;
  
  sp = state->current_stack;
  while(sp <= state->current_sp) {
    if(REFERENCE2_P(*sp)) {
      *sp = baker_gc_mutate_from(state, g, *sp);
    }
    sp++;
  }
  
  /* Now the handle table. */
  for(i = 0; i < state->handle_tbl->total; i++) {
    if(state->handle_tbl->entries[i]) {
      state->handle_tbl->entries[i]->object = 
            baker_gc_mutate_from(state, g, state->handle_tbl->entries[i]->object);
    }
  }
  
  cpu_event_each_channel(state,
      (cpu_event_each_channel_cb) baker_gc_mutate_from, g);
  cpu_sampler_collect(state,
      (cpu_sampler_collect_cb) baker_gc_mutate_from, g);
  
  int j;
  OBJECT t2;
  for(i = 0; i < g->seen_weak_refs->len; i++) {
    tmp = (OBJECT)g_ptr_array_index(g->seen_weak_refs, i);
    for(j = 0; j < NUM_FIELDS(tmp); j++) {
      t2 = tuple_at(state, tmp, j);
      if(GC_ZONE(t2) == GC_YOUNG_OBJECTS) {
        if(baker_gc_forwarded_p(t2)) {
          tuple_put(state, tmp, j, baker_gc_forwarded_object(t2));
        } else {
          tuple_put(state, tmp, j, Qnil);
        }
      }
    }
  }
  
  baker_gc_swap(g);
  
  if(g->current->size != g->next->size) {
    baker_gc_enlarge_next(g, g->current->size);
  }

  if(g->used > g->current->size * 0.90) {
    DEBUG("Enlarging next!\n");
    baker_gc_enlarge_next(g, g->current->size * 1.5);
  }
  
  g_ptr_array_free(rs, TRUE);
  // printf("%d objects promoted.\n", promoted);
  return TRUE;
}

void baker_gc_clear_gc_flag(baker_gc g, int flag) {
  int sz, osz;
  char *end, *cur;
  OBJECT obj;
  
  sz = baker_gc_used(g);
  cur = (char*)g->current->address;
  end = cur + sz;
  
  while(cur < end) {
    obj = (OBJECT)cur;
    osz = SIZE_IN_BYTES(cur);
    
    HEADER(obj)->gc ^= flag;
        
    cur += osz;
  }  
}

void baker_gc_find_lost_souls(STATE, baker_gc g) {
  int sz, osz;
  char *end, *cur;
  OBJECT obj;
  
  sz = baker_gc_used(g);
  cur = (char*)g->next->address;
  end = cur + sz;
  
  while(cur < end) {
    obj = (OBJECT)cur;
    osz = SIZE_IN_BYTES(cur);
    
    if(!baker_gc_forwarded_p(obj)) {      
      if(SHOULD_CLEANUP_P(obj)) {
        state_run_cleanup(state, obj);
      }
    }

    cur += osz;
  }
}

void baker_gc_collect_references(STATE, baker_gc g, OBJECT mark, GPtrArray *refs) {
  int sz, osz, i;
  char *end, *cur;
  OBJECT obj;
  
  sz = baker_gc_used(g);
  cur = (char*)g->current->address;
  end = cur + sz;
    
  while(cur < end) {
    obj = (OBJECT)cur;
    osz = SIZE_IN_BYTES(cur);
    
    if(!_object_stores_bytes(obj)) {
      for(i = 0; i < NUM_FIELDS(obj); i++) {
        if(NTH_FIELD(obj, i) == mark) {
          g_ptr_array_add(refs, (gpointer)obj);
        }
      }
    }
    
    cur += osz;
  }
}


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

#define FREE_OBJECT 0x10000
#define BARRIER (2**REFSIZE)

#define FREE_FLAG 0xff

static int objects_marked = 0;

mark_sweep_gc mark_sweep_new(int chunk_size) {
  mark_sweep_gc ms;
  
  ms = calloc(1, sizeof(struct _mark_sweep_gc));
  ms->chunk_size = chunk_size;
  ms->remember_set = g_ptr_array_new();
  mark_sweep_add_chunk(ms);
  ms->enlarged = 0;
  return ms;
}

void mark_sweep_adjust_extremes(mark_sweep_gc ms, ms_chunk *new) {
  if(!ms->extreme_min || new->address < ms->extreme_min) {
    ms->extreme_min = new->address;
  }
  
  if(!ms->extreme_max || new->last_address > ms->extreme_max) {
    ms->extreme_max = new->last_address;
  }
}

void mark_sweep_add_chunk(mark_sweep_gc ms) {
  ms_chunk *new;
  OBJECT obj;
  
  ms->enlarged = 1;
  ms->num_chunks++;
  
  new = calloc(1, sizeof(struct ms_chunk));
  new->size = ms->chunk_size * REFSIZE;
  new->address = calloc(1, new->size);
  new->last_address = ((char*)new->address) + new->size - 1;
  
  // printf("Added a new chunk: %p (%d)\n", new->address, ms->num_chunks);
  
  mark_sweep_adjust_extremes(ms, new);
  
  if(!ms->chunks) {
    ms->chunks = new;
  } else {
    ms_chunk *cur;
    cur = ms->chunks;
    while(cur->next) { cur = cur->next; }
    cur->next = new;
  }
    
  obj = (OBJECT)new->address;
  HEADER(obj)->flags = FREE_FLAG;
  HEADER(obj)->klass = 0;
  SET_NUM_FIELDS(obj, ms->chunk_size - HEADER_SIZE);
  
  // TODO figure out how to use the barrier
  /* Scribble the barrier value at the end of the object. */
  // SET_FIELD(obj, ms->chunk_size-1, BARRIER);
  
  if(!ms->free_list) {
    ms->free_list = obj;
  } else {
    HEADER(ms->free_list)->klass = obj;
  }
}

void mark_sweep_free_chunk(mark_sweep_gc ms, ms_chunk *chunk) {
  ms_chunk *cur, *lst;
  
  cur = ms->chunks;
  lst = NULL;
  
  ms->extreme_min = ms->extreme_max = 0;
  
  while(cur) {
    if(cur == chunk) {
      if(!lst) {
        ms->chunks = chunk->next;
      } else {
        lst->next = chunk->next;
      }
      cur = chunk->next;
    } else {
      mark_sweep_adjust_extremes(ms, cur);
      lst = cur;
      cur = cur->next;
    }
  }
  
  // printf("Free'd chunk: %p\n", chunk->address);
  
  free(chunk->address);
  free(chunk);
  
  ms->num_chunks--;
}

OBJECT mark_sweep_allocate(mark_sweep_gc ms, int obj_fields) {
  OBJECT obj, lst, rest;
  int orig_fields, fields, bytes;
    
  /* Make sure we allocate enough room for the header of the
     object too. */

  fields = obj_fields + HEADER_SIZE;
  bytes = fields * REFSIZE;
  
  assert(fields < ms->chunk_size);
  
  lst = 0;
  
  obj = ms->free_list;
  while(obj && NUM_FIELDS(obj) < fields) {
    lst = obj;
    obj = HEADER(obj)->klass;
  }
  
  if(!obj) {
    mark_sweep_add_chunk(ms);
    obj = ms->free_list;
    lst = 0;
    
    while(obj && NUM_FIELDS(obj) < fields) {
      lst = obj;
      obj = HEADER(obj)->klass;
    }
  }
  
  /* We found exactly the right size. */
  if(NUM_FIELDS(obj) == fields) {
    rest = HEADER(obj)->klass;
  } else {
    orig_fields = NUM_FIELDS(obj);
    SET_NUM_FIELDS(obj, obj_fields);
  
    rest = (OBJECT)(obj + bytes);
    HEADER(rest)->flags = FREE_FLAG;
    HEADER(rest)->klass = HEADER(obj)->klass;
    SET_NUM_FIELDS(rest, orig_fields - fields);
  }
  
  if(!lst) {
    ms->free_list = rest;
  } else {
    HEADER(lst)->klass = rest;
  }
  
  HEADER(obj)->flags = 0;
    
  return obj;
}

static int freed_objects = 0;
static int marked_objects = 0;

void mark_sweep_free(mark_sweep_gc ms, OBJECT obj) {
  OBJECT nxt, fl, lst;
  int found;
  
  freed_objects++;
  
  // printf("freeing %p (%s)\n", obj, _inspect(obj));
  
  fl = ms->free_list;
  /* Free list is empty, the fast case. */
  if(!fl) {
    ms->free_list = obj;
    return;
  }
  
  HEADER(obj)->klass = 0;
  HEADER(obj)->flags = FREE_FLAG;
  
  /* Try and collapse this object into the one after it. */
  /* TODO: find a better way than searching for a next object. */
  
  nxt = (OBJECT)(obj + SIZE_IN_BYTES(obj));
  
  found = 0;
  
  lst = 0;
  while(fl) {
    // printf(" looking through %p\n", fl);
    if(fl == nxt) {
      found = 1;
      break;
    }
    lst = fl;
    fl = HEADER(fl)->klass;
  }
  
  if(found) {
    SET_NUM_FIELDS(fl, (HEADER_SIZE + NUM_FIELDS(obj)));
    return;
  }
  
  if(lst == obj) {}
  
  assert(lst != obj);
  /* Couldn't collapse it, append it to the end. */
  HEADER(lst)->klass = obj;
}

void mark_sweep_free_fast(mark_sweep_gc ms, OBJECT obj) {
  if(!ms->free_list) {
    ms->free_list = obj;
  } else {
    HEADER(obj)->klass = ms->free_list;
    ms->free_list = obj;
  }
}

#define MARK_OBJ(obj) (HEADER(obj)->gc |= MS_MARK)
#define UNMARK_OBJ(obj) (HEADER(obj)->gc ^= MS_MARK)
#define MARKED_P(obj) (HEADER(obj)->gc & MS_MARK)

int mark_sweep_contains_p(mark_sweep_gc ms, OBJECT obj) {
  ms_chunk *cur;
  if(obj < (OBJECT)ms->extreme_min || obj >= (OBJECT)ms->extreme_max) {
    return FALSE;
  }
  
  cur = ms->chunks;
  while(cur) {
    if(obj >= (OBJECT)cur->address && obj < (OBJECT)cur->last_address) {
      return TRUE;
    }
    cur = cur->next;
  }
  
  return FALSE;
}

void mark_sweep_describe(mark_sweep_gc ms) {
  ms_chunk *cur;
  int i;
  
  printf("Number of chunks: %d\n", ms->num_chunks);
  
  cur = ms->chunks;
  i = 0;
  while(cur) {
    printf("   %d:  %p => %p\n", i++, cur->address, cur->last_address);
    cur = cur->next;
  }
  printf("Last marked: %d\n", objects_marked);
}

int _object_stores_bytes(OBJECT self);

void mark_sweep_mark_object(mark_sweep_gc ms, OBJECT iobj) {
  OBJECT cls, tmp;
  int i;
  
  /* Already marked! */
  if(MARKED_P(iobj)) return;
  
  // printf("Marking object %p\n", iobj);
  if(mark_sweep_contains_p(ms, iobj)) {
    objects_marked++;
  }
  
  marked_objects++;
    
  MARK_OBJ(iobj);
  cls = CLASS_OBJECT(iobj);
  if(REFERENCE_P(cls)) {
    mark_sweep_mark_object(ms, cls);
  }
  
  if(!_object_stores_bytes(iobj)) {
    for(i = 0; i < NUM_FIELDS(iobj); i++) {
      tmp = NTH_FIELD(iobj, i);
      if(!REFERENCE_P(tmp)) continue;
      
      mark_sweep_mark_object(ms, tmp);
    }
  }
}

void mark_sweep_mark_phase(mark_sweep_gc ms, GPtrArray *roots) {
  int i, sz;
  OBJECT root;
  
  marked_objects = 0;
  
  sz = roots->len;
  for(i = 0; i < sz; i++) {
    root = (OBJECT)(g_ptr_array_index(roots, i));
    if(!REFERENCE_P(root)) { continue; }
    mark_sweep_mark_object(ms, root);
  }
  
  sz = ms->remember_set->len;
  for(i = 0; i < sz; i++) {
    root = (OBJECT)(g_ptr_array_index(ms->remember_set, i));
    if(!REFERENCE_P(root)) { continue; }
    mark_sweep_mark_object(ms, root);
  }
  
  // printf("Marked Objects: %d\n", marked_objects);
}

void mark_sweep_sweep_phase(mark_sweep_gc ms) {
  ms_chunk *cur;
  char *addr, *last;
  OBJECT obj;
  int osz, used;
  
  cur = ms->chunks;
  
  freed_objects = 0;
  
  /* For each chunk... */
  while(cur) {
    
    addr = (char*)(cur->address);
    last = (addr + cur->size) - (HEADER_SIZE * REFSIZE);
    
    used = 0;
    /* For each object.. */
    while(addr < last) {
      obj = (OBJECT)addr;
      osz = SIZE_IN_BYTES(obj);
      
      if(HEADER(obj)->flags != FREE_FLAG) {
        /* Check if it's marked and recycle if not. */
        if(!MARKED_P(obj)) {
          mark_sweep_free_fast(ms, obj);
        } else {
          UNMARK_OBJ(obj);
          used++;
        }
      }
      addr += osz;
    }
    
    if(!used && ms->num_chunks > 1) {
      ms_chunk *nxt;
      nxt = cur->next;
      mark_sweep_free_chunk(ms, cur);
      cur = nxt;
    } else {
      cur = cur->next;
    }
  }
  
  // printf("Free'd Objects: %d\n", freed_objects);
}

void mark_sweep_collect(mark_sweep_gc ms, GPtrArray *roots) {
  ms->enlarged = 0;
  objects_marked = 0;
  mark_sweep_mark_phase(ms, roots);
  // printf("%d objects marked.\n", objects_marked);
  mark_sweep_sweep_phase(ms);  
}

#include <stdlib.h>

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/cpu.h"
#include "shotgun/lib/machine.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/methctx.h"
#include "shotgun/lib/object.h"
#include "shotgun/lib/bytearray.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/class.h"
#include "shotgun/lib/hash.h"
#include "shotgun/lib/symbol.h"
#include "shotgun/lib/selector.h"

void cpu_clear_cache(STATE, cpu c) {
  struct method_cache *ent, *end;
  
  ent = state->method_cache;
  end = ent + CPU_CACHE_SIZE;
  
  while(ent < end) {
    ent++;
  }
}

void cpu_clear_cache_for_method(STATE, cpu c, OBJECT meth, int full) {
  struct method_cache *ent, *end;
  
  selector_clear_by_name(state, meth);
  
  ent = state->method_cache;
  end = ent + CPU_CACHE_SIZE;
  
  while(ent < end) {
    if(ent->name == meth) {
      if(full && ent->method && REFERENCE_P(ent->method)) {
        OBJECT meth;
        meth = ent->method;
        if(ISA(meth, state->global->tuple)) {
          meth = tuple_at(state, meth, 1);
        }
        // Don't this for now. Inline caches are disabled and we're using
        // the serial number to detect core methods.
        // fast_inc(meth, CMETHOD_f_SERIAL); 
      }
      
      ent->name = 0;
    }
    ent++;
  }
}

void cpu_clear_cache_for_class(STATE, cpu c, OBJECT klass) {
  struct method_cache *ent, *end;
  
  ent = state->method_cache;
  end = ent + CPU_CACHE_SIZE;
  
  while(ent < end) {
    if(ent->klass == klass || ent->module == klass) {
      ent->name = 0;
    }
    ent++;
  }
}

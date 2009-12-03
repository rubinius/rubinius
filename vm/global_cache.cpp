#include "vm/global_cache.hpp"
#include "vm/object_position.hpp"
#include "vm/objectmemory.hpp"
#include "vm/builtin/module.hpp"

namespace rubinius {
  void GlobalCache::prune_young() {
    cache_entry* entry;
    for(size_t i = 0; i < CPU_CACHE_SIZE; i++) {
      entry = &entries[i];
      bool clear = false;

      Object* klass = reinterpret_cast<Object*>(entry->klass);
      if(!klass) continue;

      if(klass->young_object_p()) {
        if(klass->forwarded_p()) {
          Module* fwd = (Module*)klass->forward();
          assert(rubinius::VM::current_state()->om->validate_object(fwd) != cUnknown);
          entry->klass = fwd;
        } else {
          clear = true;
        }
      }

      Object* mod = reinterpret_cast<Object*>(entry->module);
      if(mod->young_object_p()) {
        if(mod->forwarded_p()) {
          entry->module = (Module*)mod->forward();
        } else {
          clear = true;
        }
      }

      Object* exec = reinterpret_cast<Object*>(entry->method);
      if(exec->young_object_p()) {
        if(exec->forwarded_p()) {
          entry->method = (Executable*)exec->forward();
        } else {
          clear = true;
        }
      }

      if(clear) {
        entry->klass = 0;
        entry->name = 0;
        entry->module = 0;
        entry->is_public = true;
        entry->method_missing = false;
      }
    }
  }

  void GlobalCache::prune_unmarked(int mark) {
    cache_entry* entry;
    for(size_t i = 0; i < CPU_CACHE_SIZE; i++) {
      entry = &entries[i];
      Object* klass = reinterpret_cast<Object*>(entry->klass);
      if(!klass) continue;

      Object* mod = reinterpret_cast<Object*>(entry->module);
      Object* exec = reinterpret_cast<Object*>(entry->method);

      if(!klass->marked_p(mark) || !mod->marked_p(mark) || !exec->marked_p(mark)) {
        entry->klass = 0;
        entry->name = 0;
        entry->module = 0;
        entry->is_public = true;
        entry->method_missing = false;
      }
    }
  }
}

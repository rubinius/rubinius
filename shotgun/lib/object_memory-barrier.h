/*

 We need to have a write barrier that performs checks whenever a reference
 is written. This allows the new generation to collected without collecting
 the old generation too.
 
 The idea is that if +target+ is on object in the old generation, and +val+
 is an object in the new generation, we need to add +target+ to the remember
 set of new generation so that when we go to collect the new generation,
 we are sure that +val+ survives. 
 
 The flag is so that we don't put the object into the remember set
 more than once.
 */

#define REMEMBER_FLAG 0x10

/* TODO: This routine MUST be optimized because it's hit constantly. */
static inline void object_memory_write_barrier(object_memory om, OBJECT target, OBJECT val) {
  unsigned long tz, vz;
  if(!REFERENCE_P(val)) return;
  
  tz = GC_ZONE(target);
  vz = GC_ZONE(val);
  
  /* if the target is in a higher numbered zone than val, then
     that means it needs to be in the remember set. */
  if(tz < vz) {
  
  //if(mark_sweep_contains_p(om->ms, target) && baker_gc_contains_spill_p(om->gc, val)) {
    //assert(tz < vz);
    if(!FLAG_SET_ON_P(target, gc, REMEMBER_FLAG)) {
      g_ptr_array_add(om->gc->remember_set, (gpointer)target);
      FLAG_SET_ON(target, gc, REMEMBER_FLAG);
    }
  } 
  /* else {
    assert(tz >= vz);
  }
  */
}

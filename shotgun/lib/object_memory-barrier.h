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


/* TODO: This routine MUST be optimized because it's hit constantly. */
static inline void object_memory_write_barrier(object_memory om, OBJECT target, OBJECT val) {
  gc_zone tz, vz;
  if(!REFERENCE_P(val)) return;
  
  tz = target->gc_zone;
  vz = val->gc_zone;
  
  xassert(tz > 0);
  xassert(vz > 0);
  xassert(val->klass != Qnil);
    
  /* if the target is in a higher numbered zone than val, then
     that means it needs to be in the remember set. */
  if(tz < vz) {
    object_memory_update_rs(om, target, val);
  } 
}

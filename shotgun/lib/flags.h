#ifndef RBS_FLAGS
#define RBS_FLAGS

/* types, takes up 3 bits */
typedef enum
{
  ObjectType    = 0,
  MContextType  = 1,
  BContextType  = 2,
  ClassType     = 3,
  MetaclassType = 4,
  MTType        = 5,
} object_type;

/* gc_zone, takes up two bits */
typedef enum
{ 
  UnspecifiedZone  = 0,
  MatureObjectZone = 1,
  YoungObjectZone  = 2,
  LargeObjectZone  = 3,
} gc_zone;


#define RAW_FLAG(obj)        (obj->all_flags)
#define CLEAR_FLAGS(obj)     RAW_FLAG(obj) = 0
#define stack_context_p(obj) (obj->gc_zone == UnspecifiedZone)
#define SET_FORWARDED(obj)   obj->Forwarded = TRUE;
#define FORWARDED_P(obj)     (obj->Forwarded)

#define AGE(obj)           (obj->copy_count)
#define CLEAR_AGE(obj)     (obj->copy_count = 0)
#define INCREMENT_AGE(obj) (obj->copy_count++)

#endif 

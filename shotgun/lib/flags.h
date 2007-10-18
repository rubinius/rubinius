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

/* gc_zone, takes up two bits and is shifted right to allow for type beneath it. */
typedef enum
{ 
  UnspecifiedZone  = 0,
  MatureObjectZone = 1,
  YoungObjectZone  = 2,
  LargeObjectZone  = 3,
} gc_zone;

struct flag_layout_t
{
  object_type     obj_type    : 3;
  gc_zone         gc_zone     : 2;
  unsigned int    copy_count  : 3;

  int Remember               : 1;
  int Marked                 : 1;
  int ForeverYoung           : 1;
  int CanStoreIvars          : 1;
  int StoresBytes            : 1;
  int RequiresCleanup        : 1;
  int IsBlockContext         : 1;
  int IsMeta                 : 1;

  int CTXFast                : 1;
  int IsTainted              : 1;
  int IsFrozen               : 1;
  int IsLittleEndian         : 1;
  int RefsAreWeak            : 1;
};

typedef union
{
  struct flag_layout_t flag;
  unsigned int         forwarded_object; 
} flags_t;

#define FLAGS(obj)           (obj->f.flag)
#define RAW_FLAG(obj)        (obj->f.forwarded_object)
#define CLEAR_FLAGS(obj)     RAW_FLAG(obj) = 0
#define stack_context_p(obj) (FLAGS(obj).gc_zone == UnspecifiedZone && !FLAGS(obj).IsTainted && !FLAGS(obj).IsFrozen && !FLAGS(obj).IsLittleEndian && !FLAGS(obj).RefsAreWeak)
#define SET_FORWARDED(obj)   RAW_FLAG(obj) = FORWARDED_OBJECT
#define FORWARDED_P(obj)     ((RAW_FLAG(obj)) == FORWARDED_OBJECT)

#define AGE(obj)           (FLAGS(obj).copy_count)
#define CLEAR_AGE(obj)     (FLAGS(obj).copy_count = 0)
#define INCREMENT_AGE(obj) (FLAGS(obj).copy_count++)

// #define COPY_RAW_FLAGS(target, source) RAW_FLAG(target) = RAW_FLAG(source)
#define FORWARDED_OBJECT 0xffffffff

#endif 

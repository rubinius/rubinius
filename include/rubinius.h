typedef unsigned long OBJECT;

struct rubinius_object {
  char flags;
  char flags2;
  char unused;
  char unused2;
  
  OBJECT klass;
  unsigned long fields;  
};


#define HEADER(obj) ((struct rubinius_object*)obj)
#define OBJECTS(obj) ((OBJECT*)obj)
#define FLAGS_OFFSET 0
#define FLAGS2_OFFSET 1
#define CLASS_OFFSET 4
#define FIELDS_OFFSET 8
/* Header size is in longs */
#define HEADER_SIZE 3

#define CLASS_OBJECT(obj) (HEADER(obj)->klass)
#define NUM_FIELDS(obj) (HEADER(obj)->fields)
#define NTH_FIELD(obj, fel) ((OBJECT)(OBJECTS(obj) + HEADER_SIZE + fel))

#define BYTES_OF(obj) ((char*)(OBJECTS(obj) + HEADER_SIZE))
#define FIXNUM_NEG(obj) ((((unsigned long)obj) & 4) == 4)

#define SET_FIELD(obj, fel, val) rbs_set_field(obj, fel, val)

inline OBJECT rbs_set_field(OBJECT obj, int fel, OBJECT val) {
  OBJECT *slot = (OBJECT*)NTH_FIELD(obj, fel);
  *slot = val;
  return val;
}

inline long rbs_to_int(OBJECT obj) {
  long val = ((unsigned long)obj) >> 3;
  if(FIXNUM_NEG(obj)) {
    val = -val;
  }
  return val;
}

inline OBJECT rbs_int_to_fixnum(int num) {
  OBJECT ret;
  ret = (num << 3) | 1;
  if(num < 0) {
    ret = ret | 4;
  }
  return ret;
}

#define FIXNUM_TO_INT(obj) rbs_to_int(obj)
#define INT_TO_FIXNUM(int) rbs_int_to_fixnum(int)
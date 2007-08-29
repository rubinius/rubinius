#ifndef RBS_FLAGS
#define RBS_FLAGS

/* for ->flags */

/* RESERVED FOR TYPE         (1<<0) */
/* RESERVED FOR TYPE         (1<<1) */
/* RESERVED FOR TYPE         (1<<2) */

#define CanStoreIvarsFlag    (1<<3)
#define StoresBytesFlag      (1<<4)
#define RequiresCleanupFlag  (1<<5)
#define IsBlockContextFlag   (1<<6)
#define IsMetaFlag           (1<<6)
#define CTXFastFlag          (1<<7)

/* for ->flags2 */

/* RESERVED FOR GC           (1<<0) */
/* RESERVED FOR GC           (1<<1) */

#define IsTaintedFlag        (1<<2)
#define IsFrozenFlag         (1<<3)
#define IsLittleEndianFlag   (1<<4)
#define RefsAreWeakFlag      (1<<5)

/* Values for the Type bits */

#define TYPE_OBJECT     0
#define TYPE_MCONTEXT   1
#define TYPE_BCONTEXT   2
#define TYPE_CLASS      3
#define TYPE_METACLASS  4
#define TYPE_MT         5
#endif 

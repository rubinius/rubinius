#ifndef RBS_FLAGS
#define RBS_FLAGS

/* for ->flags */

#define RequiresCleanupFlag  (1<<0)
#define CanStoreIvarsFlag    (1<<1)
#define StoresBytesFlag      (1<<2)
#define WasReferencedFlag    (1<<3)
#define IsBlockContextFlag   (1<<4)
#define IsMetaFlag           (1<<4)
#define HasWeakRefsFlag      (1<<6)
#define RefsAreWeakFlag      (1<<5)
#define CTXFastFlag          (1<<7)

/* for ->flags2 */

#define IsTaintedFlag        (1<<0)
#define IsFrozenFlag         (1<<1)
#define IsLittleEndianFlag   (1<<2)
#endif 

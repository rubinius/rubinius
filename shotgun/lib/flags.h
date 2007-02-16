#ifndef RBS_FLAGS
#define RBS_FLAGS

#define CanStoreIvarsFlag    0x02
#define StoresBytesFlag      0x04
#define WasReferencedFlag    0x20
#define IsBlockContextFlag   0x40
#define IsMetaFlag           0x80
#define IsTaintedFlag        0x22
#define IsFrozenFlag         0x24
#define CTXFastFlag          (1<<7)

#endif 
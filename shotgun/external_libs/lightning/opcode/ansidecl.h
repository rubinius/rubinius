#ifndef __ANSIDECL_H_SEEN
#define __ANSIDECL_H_SEEN

#ifdef __STDC__
#define PARAMS(x) x
typedef void *PTR;
#else
#define CONST const
#define PARAMS(x) ()
typedef char *PTR;
#endif

#endif

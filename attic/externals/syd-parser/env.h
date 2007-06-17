/**********************************************************************

  env.h -

  $Author: matz $
  $Date: 2003/10/20 08:33:48 $
  created at: Mon Jul 11 11:53:03 JST 1994

  Copyright (C) 1993-2003 Yukihiro Matsumoto

**********************************************************************/

#ifndef ENV_H
#define ENV_H

struct FRAME {
    VALUE self;
    int argc;
    VALUE *argv;
    ID last_func;
    ID orig_func;
    VALUE last_class;
    struct FRAME *prev;
    struct FRAME *tmp;
    struct RNode *node;
    int iter;
    int flags;
    unsigned long uniq;
    struct SCOPE *scope;
    VALUE metadata;
};

/*
// VALUE rb_cFrame;
// VALUE rb_cBacktrace;
// VALUE rb_cMain;
*/
void rb_gc_mark_frame _((struct FRAME *));

#define FRAME_ALLOCA 0
#define FRAME_MALLOC 1

struct SCOPE {
    struct RBasic super;
    ID *local_tbl;
    VALUE *local_vars;
    int flags;
};

#define SCOPE_ALLOCA  0
#define SCOPE_MALLOC  1
#define SCOPE_NOSTACK 2
#define SCOPE_DONT_RECYCLE 4

/* extern int ruby_in_eval;

// extern VALUE ruby_class;
*/
struct RVarmap {
    struct RBasic super;
    ID id;
    VALUE val;
    struct RVarmap *next;
};

#endif /* ENV_H */

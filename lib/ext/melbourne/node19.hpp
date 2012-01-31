/**********************************************************************

  node.h -

  $Author: nobu $
  $Date: 2004/10/02 11:34:29 $
  created at: Fri May 28 15:14:02 JST 1993

  Copyright (C) 1993-2003 Yukihiro Matsumoto

**********************************************************************/

#ifndef MEL_NODE19_HPP
#define MEL_NODE19_HPP

#include "melbourne.hpp"

namespace melbourne {
  namespace grammar19 {

    typedef struct RNode {
      unsigned long flags;
      char *nd_file;
      union {
        RNode *node;
        ID id;
        VALUE value;
        ID *tbl;
      } u1;
      union {
        struct RNode *node;
        ID id;
        long argc;
        VALUE value;
      } u2;
      union {
        struct RNode *node;
        ID id;
        long state;
        long cnt;
        VALUE value;
      } u3;
    } NODE;

#define RNODE(obj)  ((NODE*)(obj))

/* 0..4:T_TYPES, 5:FL_MARK, 6:reserved, 7:NODE_FL_NEWLINE */
#define NODE_FL_NEWLINE (((VALUE)1)<<7)
#define NODE_FL_CREF_PUSHED_BY_EVAL NODE_FL_NEWLINE

#define NODE_TYPESHIFT 8
#define NODE_TYPEMASK  (((VALUE)0x7f)<<NODE_TYPESHIFT)

#define nd_type(n) ((int) (((RNODE(n))->flags & NODE_TYPEMASK)>>NODE_TYPESHIFT))
#define nd_set_type(n,t) \
    RNODE(n)->flags=((RNODE(n)->flags&~NODE_TYPEMASK)|((((unsigned long)t)<<NODE_TYPESHIFT)&NODE_TYPEMASK))

#define NODE_LSHIFT (NODE_TYPESHIFT+7)
#define NODE_LMASK  (((intptr_t)1<<(sizeof(VALUE)*CHAR_BIT-NODE_LSHIFT))-1)
#define nd_line(n) (int)(RNODE(n)->flags>>NODE_LSHIFT)
#define nd_set_line(n,l) \
    RNODE(n)->flags=((RNODE(n)->flags&~(-1<<NODE_LSHIFT))|(((l)&NODE_LMASK)<<NODE_LSHIFT))

#define nd_head  u1.node
#define nd_alen  u2.argc
#define nd_next  u3.node

#define nd_cond  u1.node
#define nd_body  u2.node
#define nd_else  u3.node

#define nd_orig  u3.value

#define nd_resq  u2.node
#define nd_ensr  u3.node

#define nd_1st   u1.node
#define nd_2nd   u2.node

#define nd_stts  u1.node

#define nd_entry u3.entry
#define nd_vid   u1.id
#define nd_cflag u2.id
#define nd_cval  u3.value

#define nd_oid   u1.id
#define nd_cnt   u3.cnt
#define nd_tbl   u1.tbl

#define nd_var   u1.node
#define nd_iter  u3.node

#define nd_value u2.node
#define nd_aid   u3.id

#define nd_lit   u1.value

#define nd_frml  u2.argc
#define nd_rest  u1.id
#define nd_opt   u1.node
#define nd_pid   u1.id
#define nd_plen  u2.argc

#define nd_recv  u1.node
#define nd_mid   u2.id
#define nd_args  u3.node

#define nd_noex  u3.id
#define nd_defn  u3.node

#define nd_cfnc  u1.cfunc
#define nd_argc  u2.argc

#define nd_cpath u1.node
#define nd_super u3.node

#define nd_modl  u1.id
#define nd_clss  u1.value

#define nd_beg   u1.node
#define nd_end   u2.node
#define nd_state u3.state
#define nd_rval  u2.value

#define nd_nth   u2.argc

#define nd_tag   u1.id
#define nd_tval  u2.value

#define nd_visi  u2.argc

#undef NEW_NODE
#undef REF

#define NEW_NODE(t,a0,a1,a2)    node_newnode((t),(VALUE)(a0),(VALUE)(a1),(VALUE)(a2))

#ifdef RUBINIUS
/* In Rubinius, the object references are handles and the handle
 * in the native method frame keeps the object from being collected.
 */
#define REF(n)                  (n)
#else
/* In MRI, we are holding references to objects in memory that the
 * MRI GC does not see. To keep these objects from being collected,
 * we add a reference to them to an Array that MRI GC does see.
 */
#define REF(n)                  node_add_reference(n)
#endif

#define NEW_DEFN(i,a,d,p)       NEW_NODE(NODE_DEFN,0,i,NEW_SCOPE(a,d))
#define NEW_DEFS(r,i,a,d)       NEW_NODE(NODE_DEFS,r,i,NEW_SCOPE(a,d))
#define NEW_IFUNC(f,c)          NEW_NODE(NODE_IFUNC,f,c,0)
#define NEW_SCOPE(a,b)          NEW_NODE(NODE_SCOPE,local_tbl(),b,a)
#define NEW_BLOCK(a)            NEW_NODE(NODE_BLOCK,a,0,0)
#define NEW_IF(c,t,e)           NEW_NODE(NODE_IF,c,t,e)
#define NEW_UNLESS(c,t,e)       NEW_IF(c,e,t)
#define NEW_CASE(h,b)           NEW_NODE(NODE_CASE,h,b,0)
#define NEW_WHEN(c,t,e)         NEW_NODE(NODE_WHEN,c,t,e)
#define NEW_OPT_N(b)            NEW_NODE(NODE_OPT_N,0,b,0)
#define NEW_WHILE(c,b,n)        NEW_NODE(NODE_WHILE,c,b,n)
#define NEW_UNTIL(c,b,n)        NEW_NODE(NODE_UNTIL,c,b,n)
#define NEW_FOR(v,i,b)          NEW_NODE(NODE_FOR,v,b,i)
#define NEW_ITER(a,b)           NEW_NODE(NODE_ITER,0,NEW_SCOPE(a,b),0)
#define NEW_LAMBDA(a)           NEW_NODE(NODE_LAMBDA,a,0,0)
#define NEW_BREAK(s)            NEW_NODE(NODE_BREAK,s,0,0)
#define NEW_NEXT(s)             NEW_NODE(NODE_NEXT,s,0,0)
#define NEW_REDO()              NEW_NODE(NODE_REDO,0,0,0)
#define NEW_RETRY()             NEW_NODE(NODE_RETRY,0,0,0)
#define NEW_BEGIN(b)            NEW_NODE(NODE_BEGIN,0,b,0)
#define NEW_RESCUE(b,res,e)     NEW_NODE(NODE_RESCUE,b,res,e)
#define NEW_RESBODY(a,ex,n)     NEW_NODE(NODE_RESBODY,n,ex,a)
#define NEW_ENSURE(b,en)        NEW_NODE(NODE_ENSURE,b,0,en)
#define NEW_RETURN(s)           NEW_NODE(NODE_RETURN,s,0,0)
#define NEW_YIELD(a,s)          NEW_NODE(NODE_YIELD,a,0,s)
#define NEW_LIST(a)             NEW_ARRAY(a)
#define NEW_ARRAY(a)            NEW_NODE(NODE_ARRAY,a,1,0)
#define NEW_ZARRAY()            NEW_NODE(NODE_ZARRAY,0,0,0)
#define NEW_HASH(a)             NEW_NODE(NODE_HASH,a,0,0)
#define NEW_MASGN(l,r)          NEW_NODE(NODE_MASGN,l,0,r)
#define NEW_GASGN(v,val)        NEW_NODE(NODE_GASGN,v,val,0)
#define NEW_LASGN(v,val)        NEW_NODE(NODE_LASGN,v,val,0)
#define NEW_DASGN(v,val)        NEW_NODE(NODE_DASGN,v,val,0)
#define NEW_DASGN_CURR(v,val)   NEW_NODE(NODE_DASGN_CURR,v,val,0)
#define NEW_IASGN(v,val)        NEW_NODE(NODE_IASGN,v,val,0)
#define NEW_IASGN2(v,val)       NEW_NODE(NODE_IASGN2,v,val,0)
#define NEW_CDECL(v,val,path)   NEW_NODE(NODE_CDECL,v,val,path)
#define NEW_CVASGN(v,val)       NEW_NODE(NODE_CVASGN,v,val,0)
#define NEW_CVDECL(v,val)       NEW_NODE(NODE_CVDECL,v,val,0)
#define NEW_OP_ASGN1(p,id,a)    NEW_NODE(NODE_OP_ASGN1,p,id,a)
#define NEW_OP_ASGN2(r,i,o,val) NEW_NODE(NODE_OP_ASGN2,r,val,NEW_OP_ASGN22(i,o))
#define NEW_OP_ASGN22(i,o)      NEW_NODE(NODE_OP_ASGN2,i,o,rb_id_attrset(i))
#define NEW_OP_ASGN_OR(i,val)   NEW_NODE(NODE_OP_ASGN_OR,i,val,0)
#define NEW_OP_ASGN_AND(i,val)  NEW_NODE(NODE_OP_ASGN_AND,i,val,0)
#define NEW_GVAR(v)             NEW_NODE(NODE_GVAR,v,0,0)
#define NEW_LVAR(v)             NEW_NODE(NODE_LVAR,v,0,0)
#define NEW_DVAR(v)             NEW_NODE(NODE_DVAR,v,0,0)
#define NEW_IVAR(v)             NEW_NODE(NODE_IVAR,v,0,0)
#define NEW_CONST(v)            NEW_NODE(NODE_CONST,v,0,0)
#define NEW_CVAR(v)             NEW_NODE(NODE_CVAR,v,0,0)
#define NEW_NTH_REF(n)          NEW_NODE(NODE_NTH_REF,0,n,0)
#define NEW_BACK_REF(n)         NEW_NODE(NODE_BACK_REF,0,n,0)
#define NEW_MATCH(c)            NEW_NODE(NODE_MATCH,c,0,0)
#define NEW_MATCH2(n1,n2)       NEW_NODE(NODE_MATCH2,n1,n2,0)
#define NEW_MATCH3(r,n2)        NEW_NODE(NODE_MATCH3,r,n2,0)
#define NEW_LIT(l)              NEW_NODE(NODE_LIT,l,0,0)
#define NEW_STR(s)              NEW_NODE(NODE_STR,REF(s),0,0)
#define NEW_DSTR(s)             NEW_NODE(NODE_DSTR,REF(s),1,0)
#define NEW_XSTR(s)             NEW_NODE(NODE_XSTR,REF(s),0,0)
#define NEW_DXSTR(s)            NEW_NODE(NODE_DXSTR,REF(s),0,0)
#define NEW_DSYM(s)             NEW_NODE(NODE_DSYM,REF(s),0,0)
#define NEW_EVSTR(n)            NEW_NODE(NODE_EVSTR,0,(n),0)
#define NEW_CALL(r,m,a)         NEW_NODE(NODE_CALL,r,convert_op(m),a)
#define NEW_FCALL(m,a)          NEW_NODE(NODE_FCALL,0,convert_op(m),a)
#define NEW_VCALL(m)            NEW_NODE(NODE_VCALL,0,convert_op(m),0)
#define NEW_SUPER(a)            NEW_NODE(NODE_SUPER,0,0,a)
#define NEW_ZSUPER()            NEW_NODE(NODE_ZSUPER,0,0,0)
#define NEW_ARGS(m,o)           NEW_NODE(NODE_ARGS,o,m,0)
#define NEW_ARGS_AUX(r,b)       NEW_NODE(NODE_ARGS_AUX,r,b,0)
#define NEW_OPT_ARG(i,v)        NEW_NODE(NODE_OPT_ARG,i,v,0)
#define NEW_POSTARG(i,v)        NEW_NODE(NODE_POSTARG,i,v,0)
#define NEW_ARGSCAT(a,b)        NEW_NODE(NODE_ARGSCAT,a,b,0)
#define NEW_ARGSPUSH(a,b)       NEW_NODE(NODE_ARGSPUSH,a,b,0)
#define NEW_SPLAT(a)            NEW_NODE(NODE_SPLAT,a,0,0)
#define NEW_TO_ARY(a)           NEW_NODE(NODE_TO_ARY,a,0,0)
#define NEW_BLOCK_ARG(v)        NEW_NODE(NODE_BLOCK_ARG,v,0,local_cnt(v))
#define NEW_BLOCK_PASS(b)       NEW_NODE(NODE_BLOCK_PASS,0,b,0)
#define NEW_ALIAS(n,o)          NEW_NODE(NODE_ALIAS,n,o,0)
#define NEW_VALIAS(n,o)         NEW_NODE(NODE_VALIAS,n,o,0)
#define NEW_UNDEF(i)            NEW_NODE(NODE_UNDEF,0,i,0)
#define NEW_CLASS(n,b,s)        NEW_NODE(NODE_CLASS,n,NEW_SCOPE(0,b),(s))
#define NEW_SCLASS(r,b)         NEW_NODE(NODE_SCLASS,r,NEW_SCOPE(0,b),0)
#define NEW_MODULE(n,b)         NEW_NODE(NODE_MODULE,n,NEW_SCOPE(0,b),0)
#define NEW_COLON2(c,i)         NEW_NODE(NODE_COLON2,c,i,0)
#define NEW_COLON3(i)           NEW_NODE(NODE_COLON3,0,i,0)
#define NEW_DOT2(b,e)           NEW_NODE(NODE_DOT2,b,e,0)
#define NEW_DOT3(b,e)           NEW_NODE(NODE_DOT3,b,e,0)
#define NEW_SELF()              NEW_NODE(NODE_SELF,0,0,0)
#define NEW_NIL()               NEW_NODE(NODE_NIL,0,0,0)
#define NEW_TRUE()              NEW_NODE(NODE_TRUE,0,0,0)
#define NEW_FALSE()             NEW_NODE(NODE_FALSE,0,0,0)
#define NEW_ERRINFO()           NEW_NODE(NODE_ERRINFO,0,0,0)
#define NEW_DEFINED(e)          NEW_NODE(NODE_DEFINED,e,0,0)
#define NEW_PREEXE(b)           NEW_NODE(NODE_PREEXE,0,b,0)
#define NEW_POSTEXE(b)          NEW_NODE(NODE_POSTEXE,0,b,0)
#define NEW_BMETHOD(b)          NEW_NODE(NODE_BMETHOD,0,0,b)
#define NEW_ATTRASGN(r,m,a)     NEW_NODE(NODE_ATTRASGN,r,m,a)
#define NEW_PRELUDE(p,b)        NEW_NODE(NODE_PRELUDE,p,b,0)
#define NEW_OPTBLOCK(a)         NEW_NODE(NODE_OPTBLOCK,a,0,0)
#define NEW_REGEX(l,o)          NEW_NODE(NODE_REGEX,REF(l),0,o)
#define NEW_FILE()              NEW_NODE(NODE_FILE,0,0,0)
#define NEW_NUMBER(l)           NEW_NODE(NODE_NUMBER,REF(l),0,0)
#define NEW_FLOAT(l)            NEW_NODE(NODE_FLOAT,REF(l),0,0)
#define NEW_ENCODING(n)         NEW_NODE(NODE_ENCODING,REF(n),0,0)

#define NOEX_PUBLIC             0x00,
#define NOEX_NOSUPER            0x01,
#define NOEX_PRIVATE            0x02,
#define NOEX_PROTECTED          0x04,
#define NOEX_MASK               0x06,
#define NOEX_BASIC              0x08,
#define NOEX_UNDEF              NOEX_NOSUPER,
#define NOEX_MODFUNC            0x12,
#define NOEX_SUPER              0x20,
#define NOEX_VCALL              0x40,
#define NOEX_RESPONDS           0x80

  }; // namespace grammar19
}; // namespace melbourne

#endif

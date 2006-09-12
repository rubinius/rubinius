
#include "ruby.h"
#include "intern.h"
#include "node.h"
#include "st.h"

#define METH(name) static NODE *_ac_process_ ## name(VALUE x)
#define PROCESS(node) _ac_process(node)
#define SHIFT rb_ary_shift(x)
#define EMPTY_PA(x) (RARRAY(x)->len == 0)
#define EMPTY_P EMPTY_PA(x)
#define PAS PROCESS(SHIFT)
#define S2I sym_convert(SHIFT)
#define CALL(op, arg) _ac_process_ ## op(arg)

#include "dispatch.h"

static ID sym_convert(VALUE sym) {
  if(!SYMBOL_P(sym)) {
    printf("Not a symbol!\n");
    rb_p(sym);
    abort();
  }
  return SYM2ID(sym);
}

static ID _id_or, _id_and, _id_self;

static NODE *_ac_process(VALUE x);

struct ac_wrapper {
  NODE *node;
};

static void ac_mark(struct ac_wrapper *wrap) {
  rb_gc_mark((VALUE)wrap->node);
}

static VALUE ac_alloc(VALUE klass) {
  struct ac_wrapper *wrap;
  wrap = malloc(sizeof(struct ac_wrapper));
  return Data_Wrap_Struct(klass, ac_mark, free, wrap);
}

static VALUE ac_init(VALUE self, VALUE sexp) {
  struct ac_wrapper *wrap;
  Data_Get_Struct(self, struct ac_wrapper, wrap);
  wrap->node = _ac_process(sexp);
  return self;
}

extern VALUE rb_pt_wrap_node(NODE *node);

static VALUE ac_eval(VALUE self) {
  struct ac_wrapper *wrap;
  Data_Get_Struct(self, struct ac_wrapper, wrap);
  return rb_node_eval((VALUE)NULL, wrap->node);
}

static VALUE ac_to_pt(VALUE self) {
  struct ac_wrapper *wrap;
  Data_Get_Struct(self, struct ac_wrapper, wrap);
  return rb_pt_wrap_node(wrap->node);
}

void Init_accelload() {
  VALUE ac;
  _id_or = rb_intern("or");
  _id_and = rb_intern("and");
  _id_self = rb_intern("self");
  _init_dispatch();
  ac = rb_define_class("Accelload", rb_cObject);
  rb_define_alloc_func(ac, ac_alloc);
  rb_define_method(ac, "initialize", ac_init, 1);
  rb_define_method(ac, "eval", ac_eval, 0);
  rb_define_method(ac, "to_pt", ac_to_pt, 0);
}

static int _cur_line;
static char *_cur_file;

static NODE *create_node(enum node_type type, VALUE a, VALUE b, VALUE c) {
  NODE *n = (NODE*)rb_newobj();
  n->flags |= T_NODE;
  nd_set_type(n, type);
  nd_set_line(n, _cur_line);
  n->nd_file = _cur_file;
  n->u1.value = a;
  n->u2.value = b;
  n->u3.value = c;
  return n;
}

#undef NEW_NODE
#define NEW_NODE(t,a,b,c) create_node(t,(VALUE)a,(VALUE)b,(VALUE)c)

static NODE *_ac_process(VALUE x) {
  VALUE sym;
  ID cur;
  if(!RTEST(x)) { return NULL; }
  sym = SHIFT;
  if(!RTEST(sym)) { return NULL; }
  cur = sym_convert(sym);
  return _dispatch(cur, rb_ary_dup(x));
}

METH(block) {
  NODE *curr;
  VALUE tmp;
  
  curr = PROCESS(SHIFT);
  if(EMPTY_P) {
    return NEW_BLOCK(curr);
  } else {
    while(!EMPTY_P) {
      curr = rb_node_block_append(curr, PROCESS(SHIFT));
    }
  }
  return curr;
}

METH(defined) {
  return NEW_DEFINED(PAS);
}

METH(colon2) {
  if(RARRAY(x)->len == 1) {
    return NEW_COLON2(0, S2I);
  } else {
    return NEW_COLON2(PAS, S2I);    
  }
}

METH(match2) {
  return NEW_MATCH2(PAS, PAS);
}

METH(match3) {
  return NEW_MATCH3(PAS, PAS);
}

METH(begin) {
  return NEW_BEGIN(PAS);
}

METH(opt_n) {
  return NEW_OPT_N(PAS);
}

METH(not) {
  return NEW_NOT(PAS);
}

METH(if) {
  return NEW_IF(PAS, PAS, PAS);
}

METH(case) {
  NODE *expr, *curr, *ary;
  
  expr = PAS;
  curr = PROCESS(rb_ary_pop(x));
  
  x = SHIFT;
  while(!EMPTY_P) {
    x = SHIFT;
    SHIFT;
    curr = NEW_WHEN(PAS, PAS, curr);
  }
  
  return NEW_CASE(expr, curr);
}

METH(when) {
  return NEW_WHEN(PAS, PAS, PAS);
}

METH(while) {
  NODE *f1, *f2;
  int f3;
  f1 = PAS;
  f2 = PAS;
  if(SHIFT == Qtrue) {
    f3 = 1;
  } else {
    f3 = 0;
  }
  
  return NEW_WHILE(f1, f2, f3);
}

METH(until) {
  NODE *f1, *f2;
  int f3;
  f1 = PAS;
  f2 = PAS;
  if(SHIFT == Qtrue) {
    f3 = 1;
  } else {
    f3 = 0;
  }
  
  return NEW_UNTIL(f1, f2, f3);
}

METH(block_pass) {
  return NEW_NODE(NODE_BLOCK_PASS, 0, PAS, PAS);
}

#define NF(name, up) \
METH(name) { \
  return NEW_ ## up();\
}

NF(retry, RETRY);
NF(false, FALSE);
NF(nil, NIL);
NF(self, SELF);
NF(true, TRUE);
NF(zarray, ZARRAY);
NF(zsuper, ZSUPER);
NF(redo, REDO);

#define OF(name, up) \
METH(name) { \
  return NEW_ ## up(PAS);\
}

OF(break, BREAK);
OF(next, NEXT);
OF(return, RETURN);
OF(super, SUPER);
// OF(method, METHOD);
OF(splat, SPLAT);
OF(to_ary, TO_ARY);
OF(svalue, SVALUE);
OF(dsym, DSYM);
OF(evstr, EVSTR);

METH(method) {
  return NULL;
}

METH(scope) {
  VALUE body, vars;
  ID *ids;
  int i;
  body = SHIFT;
  if(!NIL_P(body) && EMPTY_PA(body)) {
    body = (VALUE)NULL;
  }
  
  vars = SHIFT;
  if(NIL_P(vars)) { vars = rb_ary_new(); }
  
  ids = (ID*)malloc(RARRAY(vars)->len + 3);
  memset(ids, 0, RARRAY(vars)->len + 3);
  ids[0] = (ID)RARRAY(vars)->len;
  ids[1] = '_';
  ids[2] = '~';
  for(i = 0; i < RARRAY(vars)->len; i++) {
    ids[i + 3] = sym_convert(RARRAY(vars)->ptr[i]);
  }
  
  return NEW_NODE(NODE_SCOPE, ids, 0, PROCESS(body));
}

METH(yield) {
  NODE *f1;
  VALUE f2;
  
  f1 = PAS;
  if(NIL_P(SHIFT)) {
    f2 = Qfalse;
  } else {
    f2 = Qtrue;
  }
  
  return NEW_YIELD(f1, f2);
}

METH(rescue) {
  return NEW_RESCUE(PAS, PAS, PAS);
}

METH(resbody) {
  return NEW_RESBODY(PAS, PAS, PAS);
}

METH(ensure) {
  return NEW_ENSURE(PAS, PAS);
}

#define TF(name, up) \
METH(name) { \
  return NEW_ ## up(PAS, PAS);\
}

TF(dot2, DOT2);
TF(dot3, DOT3);
TF(argscat, ARGSCAT);
TF(argspush, ARGSPUSH);
TF(op_asgn_and, OP_ASGN_AND);
TF(op_asgn_or, OP_ASGN_OR);

METH(flip2) {
  return NEW_NODE(NODE_FLIP2, PAS, PAS, 0);
}

METH(flip3) {
  return NEW_NODE(NODE_FLIP3, PAS, PAS, 0);
}


METH(and) {
  return NEW_NODE(NODE_AND, PAS, PAS, 0);
}

METH(or) {
  return NEW_NODE(NODE_OR, PAS, PAS, 0);
}

METH(call) {
  return NEW_CALL(PAS, S2I, PAS);
}

METH(fcall) {
  return NEW_FCALL(S2I, PAS);
}

METH(vcall) {
  return NEW_VCALL(S2I);
}

METH(op_asgn1) {
  NODE *recv;
  ID kind, kname;
  recv = PAS;
  kname = S2I;
  if(kname == _id_or) {
    kname = 0;
  } else if(kname == _id_and) {
    kname = 1;
  }
  return NEW_OP_ASGN1(recv, kname, PAS);
}

METH(op_asgn2) {
  NODE *recv;
  ID kind, kname, meth;
  recv = PAS;
  meth = S2I;
  kname = S2I;
  if(kname == _id_or) {
    kname = 0;
  } else if(kname == _id_and) {
    kname = 1;
  }
  return NEW_OP_ASGN2(recv, meth, kname, PAS);
}

METH(masgn) {
  NODE *f1, *f3, *f2;
  VALUE star;
  
  f1 = PAS;
  star = SHIFT;
  if(NIL_P(star)) {
    f3 = 0;
  } else if(star == Qtrue) {
    f3 = (NODE*)-1;
  } else {
    f3 = PROCESS(star);
  }
  f2 = PAS;
  return NEW_NODE(NODE_MASGN, f1, f2, f3);
}

METH(lasgn) {
  ID name;
  int cnt;
  name = S2I;
  cnt = FIX2INT(SHIFT);
  return NEW_NODE(NODE_LASGN, name, PAS, cnt);
}

#define SF(name, up) \
METH(name) { \
  return NEW_ ## up(S2I, PAS);\
}

SF(iasgn, IASGN);
SF(dasgn, DASGN);
SF(dasgn_curr, DASGN_CURR);
SF(cvasgn, CVASGN);
SF(cvdecl, CVDECL);

METH(gasgn) {
  ID i;
  i = S2I;
  return NEW_GASGN(i, PAS);
}

METH(cdecl) {
  return NEW_CDECL(S2I, PAS, PAS);
}

METH(alias) {
  ID u1, u2;
  u1 = S2I;
  u2 = S2I;
  return NEW_ALIAS(u2, u1);
}

METH(valias) {
  ID u1, u2;
  u1 = S2I;
  u2 = S2I;
  return NEW_VALIAS(u2, u1);
}

METH(array) {
  NODE *curr;
  curr = NEW_LIST(PAS);
  while(!EMPTY_P) {
    curr = rb_node_list_append(curr, PAS);
  }
  return curr;
}

METH(hash) {
  if(EMPTY_P) {
    return NEW_HASH(NULL);
  }
  return NEW_HASH(CALL(array, x));
}

METH(dstr) {
  VALUE lit;
  NODE *curr;
  lit = SHIFT;
  curr = NEW_LIST(PAS);
  while(!EMPTY_P) {
    curr = rb_node_list_append(curr, PAS);
  }
  return NEW_NODE(NODE_DSTR, lit, 0, curr);
}

METH(dxstr) {
  VALUE lit;
  NODE *curr;
  lit = SHIFT;
  curr = NEW_LIST(PAS);
  while(!EMPTY_P) {
    curr = rb_node_list_append(curr, PAS);
  }
  return NEW_NODE(NODE_DXSTR, lit, 0, curr);
}

METH(dregx) {
  VALUE lit;
  NODE *curr;
  lit = SHIFT;
  curr = NEW_LIST(PAS);
  while(!EMPTY_P) {
    curr = rb_node_list_append(curr, PAS);
  }
  return NEW_NODE(NODE_DREGX, lit, 0, curr);
}

METH(defn) {
  return NEW_NODE(NODE_DEFN, 0, S2I, PAS);
}

METH(defs) {
  return NEW_NODE(NODE_DEFS, PAS, S2I, PAS);
}

METH(class) {
  NODE *path, *body, *sup;
  path = PAS;
  sup = PAS;
  body = PAS;
  return NEW_NODE(NODE_CLASS, path, body, sup);
}

METH(module) {
  return NEW_NODE(NODE_MODULE, PAS, PAS, 0);
}

METH(sclass) {
  return NEW_NODE(NODE_ARGS, PAS, PAS, 0);
}

METH(args) {
  VALUE locals, opt_names, rest, opts;
  int n_rest, sz;
  
  locals = SHIFT;
  opt_names = SHIFT;
  rest = SHIFT;
  opts = SHIFT;
  
  if(!NIL_P(rest)) {
    n_rest = FIX2INT(rb_ary_pop(rest));
  } else {
    n_rest = -1;
  }
  
  if(!NIL_P(locals)) {
    sz = RARRAY(locals)->len;
  } else {
    sz = 0;
  }
  
  return NEW_ARGS(sz, PROCESS(opts), n_rest);
}

METH(lvar) {
  return NEW_NODE(NODE_LVAR, S2I, 0, FIX2INT(SHIFT));
}

#define SMF(name, up) \
METH(name) {\
  return NEW_ ## up(S2I);\
}

SMF(dvar, DVAR);
SMF(ivar, IVAR);
SMF(cvar, CVAR);
// SMF(gvar, GVAR);
SMF(const, CONST);
SMF(attrset, ATTRSET);

METH(gvar) {
  ID i;
  i = S2I;
  return NEW_GVAR(i);
}

METH(str) {
  return NEW_STR(SHIFT); 
}

METH(xstr) {
  return NEW_XSTR(SHIFT); 
}

METH(match) {
  return NEW_MATCH(SHIFT); 
}

METH(lit) {
  return NEW_LIT(SHIFT);
}

METH(newline) {
  _cur_line = FIX2INT(SHIFT);
  _cur_file = rb_source_filename(RSTRING(SHIFT)->ptr);
  return NEW_NEWLINE(PAS);
}

METH(nth_ref) {
  return NEW_NODE(NODE_NTH_REF, 0, SHIFT, 1);
}

METH(back_ref) {
  char *str;
  VALUE sym;
  sym = SHIFT;
  str = rb_id2name(sym_convert(sym));
  return NEW_NODE(NODE_BACK_REF, 0, str[0], 1);
}

METH(block_arg) {
  return NEW_NODE(NODE_BLOCK_ARG, S2I, 0, SHIFT);
}

METH(attrasgn) {
  VALUE recv;
  NODE *recv_val;
  recv = SHIFT;
  if(NIL_P(recv) || sym_convert(rb_ary_entry(recv, 0)) == _id_self) {
    recv_val = (NODE*)1;
  } else {
    recv_val = PROCESS(recv);
  }
  return NEW_ATTRASGN(recv_val, S2I, PAS);
}

METH(iter) {
  return NEW_ITER(PAS, PAS, PAS);
}

METH(for) {
  return NEW_FOR(PAS, PAS, PAS);
}
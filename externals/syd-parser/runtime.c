#include "ruby.h"
#include "sydnode.h"
#include "sydparse.h"
#include "runtime.h"
#include "internal.h"

rb_parse_state *alloc_parse_state() {
    rb_parse_state *st = (rb_parse_state*)malloc(sizeof(rb_parse_state));
    memset(st, 0, sizeof(rb_parse_state));

    st->command_start = Qtrue;
    st->class_nest = 0;
    st->in_single = 0;
    st->in_def = 0;
    st->compile_for_eval = 0;
    st->cur_mid = 0;
    st->token_buffer = NULL;
    st->tokidx = 0;
    st->toksiz = 0;
    st->comments = 0;
    st->memory_cur = NULL;
    st->memory_last_addr = NULL;
    st->current_pool = 0;
    st->pool_size = 0;
    st->memory_size = 204800;
    st->memory_pools = NULL;
    st->embedded = rb_ary_new();

    return st;
}

void *rb_pt_mark(void *vps) {
    rb_parse_state *parse_state = (rb_parse_state*)vps;

    rb_gc_mark(parse_state->lex_lastline);
    rb_gc_mark(parse_state->lex_input);
    rb_gc_mark(parse_state->self);
    rb_gc_mark(parse_state->embedded);
    return NULL;
}

void pt_free();

VALUE rb_cSydParser;

VALUE rb_pt_create() {
    VALUE obj;
    rb_parse_state *st;
    st = alloc_parse_state();
    obj = Data_Wrap_Struct(rb_cSydParser, rb_pt_mark, pt_free, st);
    st->self = obj;
    return obj;
}

VALUE rb_pt_wrap_node(NODE *node) {
    rb_parse_state *st;
    VALUE pt = rb_pt_create();
    Data_Get_Struct(pt, rb_parse_state, st);
    st->top = (NODE*)node;
    return pt;
}

void compile_error(const char *);

VALUE rb_pt_load_string(int argc, VALUE *argv, VALUE self) {
    VALUE str, get_comments;
    VALUE pt, old_err;
/*    int state;*/
    rb_parse_state *st;
    
    get_comments = Qfalse;
    rb_scan_args(argc, argv, "11", &str, &get_comments);
    
    pt = rb_pt_create();
    Data_Get_Struct(pt, rb_parse_state, st);
    
    if(RTEST(get_comments)) {
        st->comments = rb_ary_new();
    }
    
    old_err = ruby_errinfo;
    ruby_errinfo = Qnil;
    // PUSH_TAG(PROT_NONE);
    // ruby_in_eval++;
    // RUN_TAG(state) {
        syd_compile_string("(eval)", str, 1, st);
    // }
    // POP_TAG();
    // ruby_in_eval--;
    if(ruby_nerrs > 0) {
        rb_exc_raise(ruby_errinfo);
    }
    ruby_errinfo = old_err;

    return pt;
}

#define SYMBOL(sym) rb_intern(#sym)

VALUE rb_pt_load_file(int argc, VALUE *argv, VALUE self) {
    VALUE pt, old_err;
/*    int state;*/
    rb_parse_state *st;
    VALUE file, fname, get_comments;

    pt = rb_pt_create();
    Data_Get_Struct(pt, rb_parse_state, st);
    
    get_comments = Qfalse;
    fname = Qfalse;
    rb_scan_args(argc, argv, "12", &file, &fname, &get_comments);
    
    if(!RTEST(fname)) {
        fname = rb_funcall(file, SYMBOL(path), 0);
    }
    
    if(RTEST(get_comments)) {        
        st->comments = rb_ary_new();
    }

    old_err = ruby_errinfo;
    // ruby_errinfo = Qnil;
    // PUSH_TAG(PROT_NONE);
    // ruby_in_eval++;
    // RUN_TAG(state) {
        syd_compile_file_from_state(RSTRING(fname)->ptr, file, 1, st);
    // }
    // POP_TAG();
    // ruby_in_eval--;
    if(ruby_nerrs > 0) {
        rb_exc_raise(ruby_errinfo);
    }
    ruby_errinfo = old_err;

    return pt;
}

/*
VALUE eval_node(VALUE, NODE*);

void jump_tag_but_local_jump(int, VALUE);
VALUE rb_pt_eval(int argc, VALUE *argv, VALUE self) {
    VALUE old_err, result, wrapper, current_wrapper;
    int state;
    NODE *cref;
    rb_parse_state *st;

    Data_Get_Struct(self, rb_parse_state, st);
    if(!rb_scan_args(argc, argv, "01", &wrapper)) {
        wrapper = 0;
    }

    result = Qnil;
    old_err = ruby_errinfo;
    ruby_errinfo = Qnil;
    current_wrapper = ruby_wrapper;
    cref = ruby_cref;
    PUSH_VARS();
    PUSH_CLASS(ruby_wrapper);
    ruby_cref = top_cref;
    if (!RTEST(wrapper)) {
	rb_secure(4);		
	ruby_class = rb_cObject;
	ruby_wrapper = 0;
    }
    else {
        if(wrapper == Qtrue) {
	    ruby_class = ruby_wrapper = rb_module_new();
        } else {
            if(rb_obj_is_kind_of(wrapper, CPATH(Module))) {
                ruby_class = ruby_wrapper = wrapper;
            } else {
                rb_raise(CPATH(ArgumentError), "wrapper is not a module");
            }
        }
	self = rb_obj_clone(ruby_top_self);
	rb_extend_object(self, ruby_wrapper);
	PUSH_CREF(ruby_wrapper);
    }
    PUSH_SCOPE();
    SCOPE_SET(SCOPE_PRIVATE);
    PUSH_TAG(PROT_NONE);
    RUN_TAG(state) {
        result = eval_node(self, st->top);
    }
    POP_TAG();
    ruby_cref = cref;
    POP_SCOPE();
    POP_CLASS();
    POP_VARS();
    ruby_wrapper = current_wrapper;

    if(state) jump_tag_but_local_jump(state, Qundef);

    return result;
}

*/

VALUE rb_pt_loaded(VALUE self) {
    rb_parse_state *st;
    Data_Get_Struct(self, rb_parse_state, st);

    if(st->top) {
        return Qtrue;
    } else {
        return Qfalse;
    }
}

NODE *rb_pt_top(VALUE self) {
    rb_parse_state *st;
    Data_Get_Struct(self, rb_parse_state, st);
    return st->top;
}

void *pt_allocate(rb_parse_state *st, int size) {
  void *ptr;
  void *cur;
  
  if(!st->memory_cur || ((st->memory_cur + size) >= st->memory_last_addr)) {
    if(st->memory_cur) st->current_pool++;

    if(st->current_pool == st->pool_size) {
      st->pool_size += 10;
      if(st->memory_pools) {
        st->memory_pools = (void**)realloc(st->memory_pools, sizeof(void*) * st->pool_size);
      } else {
        st->memory_pools = (void**)malloc(sizeof(void*) * st->pool_size);
      }
    }
    st->memory_pools[st->current_pool] = malloc(st->memory_size);
    st->memory_cur = st->memory_pools[st->current_pool];
    memset(st->memory_cur, 0, st->memory_size);
    st->memory_last_addr = st->memory_cur + st->memory_size - 1;
  }
  
  cur = (void*)st->memory_cur;
  st->memory_cur = st->memory_cur + size;
  
  return cur;
}

void pt_free(rb_parse_state *st) {
  int i;
  if(!st->memory_pools) return;
  
  for(i = 0; i <= st->current_pool; i++) {
    free(st->memory_pools[i]);
  }
  free(st->memory_pools);
}

/* 
 *
 * 100% of the credit for the following node tree => sexp transform goes
 * to Ryan Davis (zenspider) and Eric Hodel (drbrain).
 *
 * It was mearly imported into this space by Evan Webb.
 */
static char node_type_string[][60] = {
    /*  00 */
    "method", "fbody", "cfunc", "scope", "block",
    "if", "case", "when", "opt_n", "while",
    /*  10 */
    "until", "iter", "for", "break", "next",
    "redo", "retry", "begin", "rescue", "resbody",
    /*  20 */
    "ensure", "and", "or", "not", "masgn",
    "lasgn", "dasgn", "dasgn_curr", "gasgn", "iasgn",
    /*  30 */
    "cdecl", "cvasgn", "cvdecl", "op_asgn1", "op_asgn2",
    "op_asgn_and", "op_asgn_or", "call", "fcall", "vcall",
    /*  40 */
    "super", "zsuper", "array", "zarray", "hash",
    "return", "yield", "lvar", "dvar", "gvar",
    /*  50 */
    "ivar", "const", "cvar", "nth_ref", "back_ref",
    "match", "match2", "match3", "lit", "str",
    /*  60 */
    "dstr", "xstr", "dxstr", "evstr", "dregx",
    "dregx_once", "args", "argscat", "argspush", "splat",
    /*  70 */
    "to_ary", "svalue", "block_arg", "block_pass", "defn",
    "defs", "alias", "valias", "undef", "class",
    /*  80 */
    "module", "sclass", "colon2", "colon3", "cref",
    "dot2", "dot3", "flip2", "flip3", "attrset",
    /*  90 */
    "self", "nil", "true", "false", "defined",
    /*  95 */
    "newline", "postexe",
#ifdef C_ALLOCA
    "alloca",
#endif
    "dmethod", "bmethod",
    /* 100 / 99 */
    "memo", "ifunc", "dsym", "attrasgn",
    /* 104 / 103 */
    "last" 
};

#define nd_3rd   u3.node
        

#define ADD_LINE if(node && RTEST(line_numbers)) { rb_ary_push(current, INT2NUM(nd_line(node))); }

void add_to_parse_tree(VALUE ary,
                              NODE * n,
                              VALUE newlines,
                              ID * locals,
                              VALUE line_numbers) {
  NODE * volatile node = n;
  NODE * volatile contnode = NULL;
  VALUE old_ary = Qnil;
  VALUE current;
  VALUE node_name;

  if (!node) return;

again:

  if (node) {
    node_name = ID2SYM(rb_intern(node_type_string[nd_type(node)]));
    if (RTEST(ruby_debug)) {
      fprintf(stderr, "%15s: %s%s%s\n",
        node_type_string[nd_type(node)],
        (RNODE(node)->u1.node != NULL ? "u1 " : "   "),
        (RNODE(node)->u2.node != NULL ? "u2 " : "   "),
        (RNODE(node)->u3.node != NULL ? "u3 " : "   "));
    }
  } else {
    node_name = ID2SYM(rb_intern("ICKY"));
  }

  current = rb_ary_new();
  rb_ary_push(ary, current);
  rb_ary_push(current, node_name);

again_no_block:

    switch (nd_type(node)) {

    case NODE_BLOCK:
      if (contnode) {
        add_to_parse_tree(current, node, newlines, locals, line_numbers);
        break;
      }

      contnode = node->nd_next;

      /* NOTE: this will break the moment there is a block w/in a block */
      old_ary = ary;
      ary = current;
      node = node->nd_head;
      goto again;
      break;

    case NODE_FBODY:
    case NODE_DEFINED:
      add_to_parse_tree(current, node->nd_head, newlines, locals, line_numbers);
      break;

    case NODE_COLON2:
      add_to_parse_tree(current, node->nd_head, newlines, locals, line_numbers);
      rb_ary_push(current, ID2SYM(node->nd_mid));
      break;

    case NODE_MATCH2:
    case NODE_MATCH3:
      add_to_parse_tree(current, node->nd_recv, newlines, locals, line_numbers);
      add_to_parse_tree(current, node->nd_value, newlines, locals, line_numbers);
      break;

    case NODE_BEGIN:
    case NODE_OPT_N:
    case NODE_NOT:
      add_to_parse_tree(current, node->nd_body, newlines, locals, line_numbers);
      break;

    case NODE_IF:
      add_to_parse_tree(current, node->nd_cond, newlines, locals, line_numbers);
      if (node->nd_body) {
        add_to_parse_tree(current, node->nd_body, newlines, locals, line_numbers);
      } else {
        rb_ary_push(current, Qnil);
      }
      if (node->nd_else) {
        add_to_parse_tree(current, node->nd_else, newlines, locals, line_numbers);
      } else {
        rb_ary_push(current, Qnil);
      }
      break;

  case NODE_CASE:
    {
      VALUE tmp;
      add_to_parse_tree(current, node->nd_head, newlines, locals, line_numbers); /* expr */
      node = node->nd_body;
      tmp = rb_ary_new();
      rb_ary_push(current, tmp);
      while (node) {
        add_to_parse_tree(tmp, node, newlines, locals, line_numbers);
        if (nd_type(node) == NODE_WHEN) {                 /* when */
          node = node->nd_next; 
        } else {
          rb_ary_push(current, rb_ary_pop(tmp));
          break;                                          /* else */
        }
        if (! node) {
          rb_ary_push(current, Qnil);                     /* no else */
        }
      }
      break;
    }
  case NODE_WHEN:
    add_to_parse_tree(current, node->nd_head, newlines, locals, line_numbers); /* args */
    if (node->nd_body) {
      add_to_parse_tree(current, node->nd_body, newlines, locals, line_numbers); /* body */
    } else {
      rb_ary_push(current, Qnil);
    }
    break;

  case NODE_WHILE:
  case NODE_UNTIL:
    add_to_parse_tree(current,  node->nd_cond, newlines, locals, line_numbers);
    if(node->nd_body) {
      add_to_parse_tree(current,  node->nd_body, newlines, locals, line_numbers); 
    } else {
      rb_ary_push(current, Qnil);
    }
    rb_ary_push(current, node->nd_3rd == 0 ? Qfalse : Qtrue);
    break;

  case NODE_BLOCK_PASS:
    add_to_parse_tree(current, node->nd_body, newlines, locals, line_numbers);
    add_to_parse_tree(current, node->nd_iter, newlines, locals, line_numbers);
    break;

  case NODE_ITER:
  case NODE_FOR:
    add_to_parse_tree(current, node->nd_iter, newlines, locals, line_numbers);
    if (node->nd_var != (NODE *)1
        && node->nd_var != (NODE *)2
        && node->nd_var != NULL) {
      add_to_parse_tree(current, node->nd_var, newlines, locals, line_numbers);
    } else {
      rb_ary_push(current, Qnil);
    }
    add_to_parse_tree(current, node->nd_body, newlines, locals, line_numbers);
    break;

  case NODE_BREAK:
  case NODE_NEXT:
    if (node->nd_stts)
      add_to_parse_tree(current, node->nd_stts, newlines, locals, line_numbers);
    break;
  case NODE_YIELD:
    if (node->nd_stts) {
      add_to_parse_tree(current, node->nd_stts, newlines, locals, line_numbers);
    } else {
      rb_ary_push(current, Qnil);
    }
    rb_ary_push(current, node->u3.value);
    break;

  case NODE_RESCUE:
      add_to_parse_tree(current, node->nd_1st, newlines, locals, line_numbers);
      add_to_parse_tree(current, node->nd_2nd, newlines, locals, line_numbers);
      add_to_parse_tree(current, node->nd_3rd, newlines, locals, line_numbers);
    break;

  /* rescue body:
   * begin stmt rescue exception => var; stmt; [rescue e2 => v2; s2;]* end 
   * stmt rescue stmt
   * a = b rescue c */

  case NODE_RESBODY:
      if(node->nd_3rd) {
          add_to_parse_tree(current, node->nd_3rd, newlines, locals, line_numbers);
      } else {
          rb_ary_push(current, Qnil);
      }
      if(node->nd_2nd) {
        add_to_parse_tree(current, node->nd_2nd, newlines, locals, line_numbers);
      } else {
          rb_ary_push(current, Qnil);
      }
     if(node->nd_1st) { 
         add_to_parse_tree(current, node->nd_1st, newlines, locals, line_numbers);
     } else {
         rb_ary_push(current, Qnil);
     }
    break;
	
  case NODE_ENSURE:
    add_to_parse_tree(current, node->nd_head, newlines, locals, line_numbers);
    if (node->nd_ensr) {
      add_to_parse_tree(current, node->nd_ensr, newlines, locals, line_numbers);
    }
    break;

  case NODE_AND:
  case NODE_OR:
    add_to_parse_tree(current, node->nd_1st, newlines, locals, line_numbers);
    add_to_parse_tree(current, node->nd_2nd, newlines, locals, line_numbers);
    break;

  case NODE_DOT2:
  case NODE_DOT3:
  case NODE_FLIP2:
  case NODE_FLIP3:
    add_to_parse_tree(current, node->nd_beg, newlines, locals, line_numbers);
    add_to_parse_tree(current, node->nd_end, newlines, locals, line_numbers);
    break;

  case NODE_RETURN:
    if (node->nd_stts)
      add_to_parse_tree(current, node->nd_stts, newlines, locals, line_numbers);
    break;

  case NODE_ARGSCAT:
  case NODE_ARGSPUSH:
    add_to_parse_tree(current, node->nd_head, newlines, locals, line_numbers);
    add_to_parse_tree(current, node->nd_body, newlines, locals, line_numbers);
    break;

  case NODE_CALL:
  case NODE_FCALL:
  case NODE_VCALL:
    if (nd_type(node) != NODE_FCALL)
      add_to_parse_tree(current, node->nd_recv, newlines, locals, line_numbers);
    rb_ary_push(current, ID2SYM(node->nd_mid));
    if (node->nd_args || nd_type(node) != NODE_FCALL)
      add_to_parse_tree(current, node->nd_args, newlines, locals, line_numbers);
    break;

  case NODE_SUPER:
    add_to_parse_tree(current, node->nd_args, newlines, locals, line_numbers);
    break;

  case NODE_BMETHOD:
    {
      struct BLOCK *data;
      Data_Get_Struct(node->nd_cval, struct BLOCK, data);
      add_to_parse_tree(current, data->var, newlines, locals, line_numbers);
      add_to_parse_tree(current, data->body, newlines, locals, line_numbers);
      break;
    }
    break;

  case NODE_DMETHOD:
    {
      struct METHOD *data;
      Data_Get_Struct(node->nd_cval, struct METHOD, data);
      rb_ary_push(current, ID2SYM(data->id));
      add_to_parse_tree(current, data->body, newlines, locals, line_numbers);
      break;
    }

  case NODE_METHOD:
    fprintf(stderr, "u1 = %p u2 = %p u3 = %p\n", node->nd_1st, node->nd_2nd, node->nd_3rd);
    add_to_parse_tree(current, node->nd_3rd, newlines, locals, line_numbers);
    break;

  case NODE_SCOPE:
    {
      VALUE tbl;
      int i;
      tbl = rb_ary_new();
      // printf("=> scope %x, %d\n", node->nd_tbl, node->nd_tbl[0]);
      add_to_parse_tree(current, node->nd_next, newlines, node->nd_tbl, line_numbers);
      for(i = 0; i < node->nd_tbl[0]; i++) {
        rb_ary_push(tbl, ID2SYM(node->nd_tbl[i + 3]));
      }
      rb_ary_push(current, tbl);
    }
    break;

  case NODE_OP_ASGN1:
    add_to_parse_tree(current, node->nd_recv, newlines, locals, line_numbers);
    switch(node->nd_mid) {
      case 0:
        rb_ary_push(current, ID2SYM(rb_intern("or")));
        break;
      case 1:
        rb_ary_push(current, ID2SYM(rb_intern("and")));
        break;
      default:
        rb_ary_push(current, ID2SYM(node->nd_mid));
    }
    //add_to_parse_tree(current, node->nd_args->nd_next, newlines, locals, line_numbers);
    add_to_parse_tree(current, node->nd_args, newlines, locals, line_numbers);
    break;

  case NODE_OP_ASGN2:
    add_to_parse_tree(current, node->nd_recv, newlines, locals, line_numbers);
    rb_ary_push(current, ID2SYM(node->nd_next->nd_vid));
    switch(node->nd_next->nd_mid) {
      case 0:
        rb_ary_push(current, ID2SYM(rb_intern("or")));
        break;
      case 1:
        rb_ary_push(current, ID2SYM(rb_intern("and")));
        break;
      default:
        rb_ary_push(current, ID2SYM(node->nd_next->nd_mid));
    }
    
    rb_ary_push(current, ID2SYM(node->nd_next->nd_aid));    
    add_to_parse_tree(current, node->nd_value, newlines, locals, line_numbers);
    break;

  case NODE_OP_ASGN_AND:
  case NODE_OP_ASGN_OR:
    add_to_parse_tree(current, node->nd_head, newlines, locals, line_numbers);
    add_to_parse_tree(current, node->nd_value, newlines, locals, line_numbers);
    break;

  case NODE_MASGN:
    add_to_parse_tree(current, node->nd_head, newlines, locals, line_numbers);
    if (node->nd_args) {
      if(node->nd_args != (NODE *)-1) {
        add_to_parse_tree(current, node->nd_args, newlines, locals, line_numbers);
      } else {
        rb_ary_push(current, Qtrue);
      }
    } else {
        rb_ary_push(current, Qnil);
    }
    if(node->nd_value) {
        add_to_parse_tree(current, node->nd_value, newlines, locals, line_numbers);
    } else {
        rb_ary_push(current, Qnil);
    }
    break;

  case NODE_LASGN:
    rb_ary_push(current, ID2SYM(node->nd_vid));
	  rb_ary_push(current, INT2NUM(node->nd_cnt));
    add_to_parse_tree(current, node->nd_value, newlines, locals, line_numbers);
    break;
  case NODE_IASGN:
  case NODE_DASGN:
  case NODE_DASGN_CURR:
  case NODE_CVASGN:
  case NODE_CVDECL:
  case NODE_GASGN:
    rb_ary_push(current, ID2SYM(node->nd_vid));
    add_to_parse_tree(current, node->nd_value, newlines, locals, line_numbers);
    break;
  
  case NODE_CDECL:
    if(node->nd_vid == 0) {
        rb_ary_push(current, Qnil);
    } else {
        rb_ary_push(current, ID2SYM(node->nd_vid));
    }
    add_to_parse_tree(current, node->nd_value, newlines, locals, line_numbers);
    if(node->nd_next) {
      add_to_parse_tree(current, node->nd_next, newlines, locals, line_numbers);      
    } else {
      rb_ary_push(current, Qnil);
    }
    break;

  case NODE_ALIAS:            /* u1 u2 (alias :blah :blah2) */
  case NODE_VALIAS:           /* u1 u2 (alias $global $global2) */
    rb_ary_push(current, ID2SYM(node->u1.id));
    rb_ary_push(current, ID2SYM(node->u2.id));
    break;

  case NODE_COLON3:           /* u2    (::OUTER_CONST) */
  case NODE_UNDEF:            /* u2    (undef instvar) */
    rb_ary_push(current, ID2SYM(node->u2.id));
    break;

  case NODE_HASH:
    {
      NODE *list;
      
      /* Support for sydneys flag on a Hash which indicates that it was 
         create implicitly from using a hash style syntax in a method call
         but without using {}'s */
      if(node->u2.argc) {
          rb_ary_store(current, 0, ID2SYM(rb_intern("ihash")));
      }

      list = node->nd_head;
      while (list) {
          add_to_parse_tree(current, list->nd_head, newlines, locals, line_numbers);
          list = list->nd_next;
          if (list == 0)
              rb_bug("odd number list for Hash");
          add_to_parse_tree(current, list->nd_head, newlines, locals, line_numbers);
          list = list->nd_next;
      }
    }
    break;

  case NODE_ARRAY:
      while (node) {
        add_to_parse_tree(current, node->nd_head, newlines, locals, line_numbers);
        node = node->nd_next;
      }
    break;

  case NODE_DSTR:
  case NODE_DXSTR:
  case NODE_DREGX:
  case NODE_DREGX_ONCE:
    {
      NODE *list = node->nd_next;
      //if (nd_type(node) == NODE_DREGX || nd_type(node) == NODE_DREGX_ONCE) {
      //  break;
      //}
      if(0 && node->u2.id) {
          rb_ary_pop(current);
          rb_ary_push(current, ID2SYM(rb_intern("xstr_custom")));
          rb_ary_push(current, ID2SYM(node->u2.id));
      }
      rb_ary_push(current, rb_str_new4(node->nd_lit));
      while (list) {
	      if (list->nd_head) {
          switch (nd_type(list->nd_head)) {
            case NODE_STR:
              add_to_parse_tree(current, list->nd_head, newlines, locals, line_numbers);
            break;
            case NODE_EVSTR:
              add_to_parse_tree(current, list->nd_head, newlines, locals, line_numbers);
            break;
            default:
              add_to_parse_tree(current, list->nd_head, newlines, locals, line_numbers);
            break;
          }
        }
        list = list->nd_next;
      }
    }
    break;

  case NODE_DEFN:
  case NODE_DEFS:
    if (node->nd_defn) {
      if (nd_type(node) == NODE_DEFS)
          add_to_parse_tree(current, node->nd_recv, newlines, locals, line_numbers);
      rb_ary_push(current, ID2SYM(node->nd_mid));
      add_to_parse_tree(current, node->nd_defn, newlines, locals, line_numbers);
    }
    break;

  case NODE_CLASS:
  case NODE_MODULE:
    add_to_parse_tree(current, node->nd_cpath, newlines, locals, line_numbers);
    // rb_ary_push(current, ID2SYM((ID)node->nd_cpath->nd_mid));
    if (nd_type(node) == NODE_CLASS) {
      if(node->nd_super) {
        add_to_parse_tree(current, node->nd_super, newlines, locals, line_numbers);
      } else {
        rb_ary_push(current, Qnil);
      }
    }
    add_to_parse_tree(current, node->nd_body, newlines, locals, line_numbers);
    break;

  case NODE_SCLASS:
    add_to_parse_tree(current, node->nd_recv, newlines, locals, line_numbers);
    add_to_parse_tree(current, node->nd_body, newlines, locals, line_numbers);
    break;

  case NODE_ARGS:
    if (locals && 
          (node->nd_cnt || node->nd_opt || node->nd_rest != -1)) {
      int i;
      NODE *optnode;
      VALUE tmp;
      long arg_count;

      if(locals[0] < node->nd_cnt) {
          rb_raise(rb_eStandardError, "Corrupted args detected (count of %d, local size of %d)", node->nd_cnt, locals[0]);
      }
      tmp = rb_ary_new();
      rb_ary_push(current, tmp);
      // printf("locals: %x (%d / cnt:%d)\n", locals, locals[0], node->nd_cnt);
      for (i = 0; i < node->nd_cnt; i++) {
        /* regular arg names */
        // printf("Pushing %d/%d %d (%s)\n", i, node->nd_cnt, locals[i + 3], rb_id2name(locals[i + 3]));
        rb_ary_push(tmp, ID2SYM(locals[i + 3]));
      }
      
      tmp = rb_ary_new();
      rb_ary_push(current, tmp);

      optnode = node->nd_opt;
      while (optnode) {
        /* optional arg names */
        rb_ary_push(tmp, ID2SYM(locals[i + 3]));
	      i++;
	      optnode = optnode->nd_next;
      }

      arg_count = node->nd_rest;
      if (arg_count > 0) {
        /* *arg name */
        tmp = rb_ary_new();
        rb_ary_push(tmp, ID2SYM(locals[arg_count+1]));
        rb_ary_push(tmp, INT2FIX(arg_count));
        //VALUE sym = rb_str_intern(rb_str_plus(rb_str_new2("*"),
        //        rb_str_new2(rb_id2name(locals[node->nd_rest + 1]))));
        rb_ary_push(current, tmp);
      } else if (arg_count == -1) {
        rb_ary_push(current, Qnil);
        /* nothing to do in this case, handled above */
      } else if (arg_count == -2) {
        rb_ary_push(current, Qnil);
        /* nothing to do in this case, no name == no use */
      } else {
        printf("Unknown arg_count %ld encountered while processing args.\n", arg_count);
        break;
        // exit(1);
      }

      optnode = node->nd_opt;
      /* block? */
      if (optnode) {
          add_to_parse_tree(current, node->nd_opt, newlines, locals, line_numbers);
      } else {
        rb_ary_push(current, Qnil);
      }
    }
    break;
	
  case NODE_LVAR:
    rb_ary_push(current, ID2SYM(node->nd_vid));
	  rb_ary_push(current, INT2NUM(node->nd_cnt));
	break;
	
  case NODE_DVAR:
  case NODE_IVAR:
  case NODE_CVAR:
  case NODE_GVAR:
  case NODE_CONST:
  case NODE_ATTRSET:
    rb_ary_push(current, ID2SYM(node->nd_vid));
    break;

  case NODE_XSTR:             /* u1    (%x{ls}) */
    rb_ary_push(current, node->nd_lit);
    break;
  case NODE_STR:              /* u1 */
    rb_ary_push(current, rb_str_dup(node->nd_lit));
    break;
  case NODE_MATCH:
    rb_ary_push(current, rb_obj_dup(node->nd_lit));
    break;
  case NODE_LIT:
    rb_ary_push(current, node->nd_lit);
    break;
  case NODE_NEWLINE:
    ADD_LINE;
  
    if(RTEST(newlines)) {
      rb_ary_push(current, INT2FIX(nd_line(node)));
      rb_ary_push(current, rb_str_new2(node->nd_file));
      add_to_parse_tree(current, node->nd_next, newlines, locals, line_numbers);
    } else {
      rb_ary_pop(ary); 
      node = node->nd_next;
      goto again;
    }
    break;

  case NODE_NTH_REF:          /* u2 u3 ($1) - u3 is local_cnt('~') ignorable? */
    rb_ary_push(current, INT2FIX(node->nd_nth));
    break;

  case NODE_BACK_REF:         /* u2 u3 ($& etc) */
    {
    char c = node->nd_nth;
    rb_ary_push(current, rb_str_intern(rb_str_new(&c, 1)));
    }
    break;

  case NODE_BLOCK_ARG:        /* u1 u3 (def x(&b) */
    rb_ary_push(current, ID2SYM(node->u1.id));
    rb_ary_push(current, INT2FIX(node->nd_cnt));
    break;

  /* these nodes are empty and do not require extra work: */
  case NODE_RETRY:
  case NODE_FALSE:
  case NODE_NIL:
  case NODE_SELF:
  case NODE_TRUE:
  case NODE_ZARRAY:
  case NODE_ZSUPER:
  case NODE_REDO:
    break;

  case NODE_SPLAT:
  case NODE_TO_ARY:
  case NODE_SVALUE:             /* a = b, c */
    add_to_parse_tree(current, node->nd_head, newlines, locals, line_numbers);
    break;

  case NODE_ATTRASGN:           /* literal.meth = y u1 u2 u3 */
    /* node id node */
    if (node->nd_1st == RNODE(1)) {
      rb_ary_push(current, rb_ary_new3(1, ID2SYM(rb_intern("self"))));
      // add_to_parse_tree(current, Qnil, newlines, locals, line_numbers);
    } else {
      add_to_parse_tree(current, node->nd_1st, newlines, locals, line_numbers);
    }
    rb_ary_push(current, ID2SYM(node->u2.id));
    add_to_parse_tree(current, node->nd_3rd, newlines, locals, line_numbers);
    break;

  case NODE_DSYM:               /* :"#{foo}" u1 u2 u3 */
    add_to_parse_tree(current, node->nd_3rd, newlines, locals, line_numbers);
    break;

  case NODE_EVSTR:
    add_to_parse_tree(current, node->nd_2nd, newlines, locals, line_numbers);
    break;

  case NODE_POSTEXE:            /* END { ... } */
    /* Nothing to do here... we are in an iter block */
    break;

  case NODE_CFUNC:
    rb_ary_push(current, INT2FIX(node->nd_cfnc));
    rb_ary_push(current, INT2FIX(node->nd_argc));
    break;

  /* Nodes we found but have yet to decypher
     I think these are all runtime only... not positive but... */
  case NODE_MEMO:               /* enum.c zip */
  case NODE_CREF:
  case NODE_IFUNC:
  /* #defines:
     case NODE_LMASK:
     case NODE_LSHIFT: */
  default:
    rb_warn("Unhandled node #%d type '%s'", nd_type(node), node_type_string[nd_type(node)]);
    if (RNODE(node)->u1.node != NULL) rb_warning("unhandled u1 value");
    if (RNODE(node)->u2.node != NULL) rb_warning("unhandled u2 value");
    if (RNODE(node)->u3.node != NULL) rb_warning("unhandled u3 value");
    if (RTEST(ruby_debug)) fprintf(stderr, "u1 = %p u2 = %p u3 = %p\n", node->nd_1st, node->nd_2nd, node->nd_3rd);
    rb_ary_push(current, INT2FIX(-99));
    rb_ary_push(current, INT2FIX(nd_type(node)));
    break;
  }
  
  ADD_LINE;

 /*  finish: */
  if (contnode) {
      node = contnode;
      contnode = NULL;
      current = ary;
      ary = old_ary;
      old_ary = Qnil;
      goto again_no_block;
  }
}

void rb_add_to_parse_tree(VALUE, NODE*, int, void*);

VALUE rb_pt_sexp(int argc, VALUE *argv, VALUE self) {
    rb_parse_state *st;
    VALUE result;
    VALUE line_numbers, newlines;

    line_numbers = Qfalse;
    newlines = Qfalse;

    rb_scan_args(argc, argv, "02", &line_numbers, &newlines);

    Data_Get_Struct(self, rb_parse_state, st);
    result = rb_ary_new();
    if(!st->top) {
        return result;
    }

    add_to_parse_tree(result, st->top, newlines, NULL, line_numbers);
    /* Seems that the way the parser is setup now, we always get one
       element in the array, so just return it. */
    return RARRAY(result)->ptr[0];
}

VALUE rb_pt_comments(VALUE self) {
    rb_parse_state *st;

    Data_Get_Struct(self, rb_parse_state, st);

    if(st->comments) {
        return st->comments;
    } else {
        return Qnil;
    }
}

void Init_sydney_parser() {
    VALUE cParser;
    setup_parser();
    
    cParser = rb_define_class("SydneyParser",rb_cObject);
    rb_define_singleton_method(cParser, "load_string", rb_pt_load_string, -1);
    rb_define_singleton_method(cParser, "load_file", rb_pt_load_file, -1);
    rb_define_singleton_method(cParser, "new", rb_pt_create, 0);
    rb_define_method(cParser, "loaded?", rb_pt_loaded, 0);
    // rb_define_method(cParser, "evaluate", rb_pt_eval, -1);
    rb_define_method(cParser, "sexp", rb_pt_sexp, -1);
    rb_define_method(cParser, "comments", rb_pt_comments, 0);
    rb_cSydParser = cParser;
    
    // Init_accelload();
}

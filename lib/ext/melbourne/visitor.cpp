#include <string.h>

#include "ruby.h"

#include "grammar.hpp"
#include "visitor.hpp"
#include "node.hpp"
#include "internal.hpp"

namespace melbourne {

  static VALUE string_newfrombstr(bstring str)
  {
    if(str == NULL) {
      return rb_str_new2("");
    }

    return rb_str_new((const char*)str->data, str->slen);
  }

  rb_parse_state *alloc_parse_state() {
      rb_parse_state *parse_state = (rb_parse_state*)calloc(1, sizeof(rb_parse_state));

      #undef command_start
      #undef class_nest
      #undef in_single
      #undef in_def
      #undef compile_for_eval
      #undef cur_mid
      #undef tokidx
      #undef toksiz
      parse_state->command_start = true;
      parse_state->class_nest = 0;
      parse_state->in_single = 0;
      parse_state->in_def = 0;
      parse_state->compile_for_eval = 0;
      parse_state->cur_mid = 0;
      parse_state->token_buffer = NULL;
      parse_state->tokidx = 0;
      parse_state->toksiz = 0;
      parse_state->comments = 0;
      parse_state->memory_cur = NULL;
      parse_state->memory_last_addr = NULL;
      parse_state->current_pool = 0;
      parse_state->pool_size = 0;
      parse_state->memory_size = 204800;
      parse_state->memory_pools = NULL;
      parse_state->emit_warnings = 0;

      return parse_state;
  }

  void compile_error(const char *);

  void *pt_allocate(rb_parse_state *st, int size) {
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
      st->memory_cur = (char*)st->memory_pools[st->current_pool];
      st->memory_last_addr = st->memory_cur + st->memory_size - 1;
    }

    cur = (void*)st->memory_cur;
    st->memory_cur = st->memory_cur + size;

    return cur;
  }

  void pt_free(rb_parse_state *st) {
    int i;

    if(st->line_buffer) {
      bdestroy(st->line_buffer);
    }

    if(st->lex_lastline) {
    bdestroy(st->lex_lastline);
    }

    free(st->token_buffer);
    delete st->variables;

    if(!st->memory_pools) return;

    for(i = 0; i <= st->current_pool; i++) {
      free(st->memory_pools[i]);
    }
    free(st->memory_pools);
  }

  extern long mel_sourceline;

  void create_error(rb_parse_state *parse_state, char *msg) {
    int col = parse_state->lex_p - parse_state->lex_pbeg;

    parse_state->error = rb_funcall(parse_state->processor,
                                    rb_intern("process_parse_error"),4,
                                    rb_str_new2(msg),
                                    INT2FIX(col),
                                    INT2FIX(mel_sourceline),
                                    string_newfrombstr(parse_state->lex_lastline));
    parse_state->error = Qtrue;
  }

  const char *op_to_name(QUID id);

  static VALUE quark_to_symbol(quark quark) {
    const char *op;
    op = melbourne::op_to_name(quark);
    if(op) {
      return rb_intern(op);
    }
    return rb_intern(quark_to_string(id_to_quark(quark)));
  }

#define nd_3rd    u3.node
#define Q2SYM(v)  quark_to_symbol(v)

  /* Visits all the nodes in the parse tree.
   * Adapted from ParseTree by Ryan Davis and Eric Hodel.
   */
  VALUE process_parse_tree(rb_parse_state *parse_state,
      VALUE ptp, NODE *n, QUID *locals)
  {
    NODE * volatile node = n;

    static int masgn_level = 0;
    static unsigned case_level = 0;
    static unsigned when_level = 0;
    static unsigned inside_case_args = 0;

    VALUE line, tree = Qnil;

    if(!node) return tree;

    again:

    if(node) {
      line = INT2FIX(nd_line(node));
    } else {
      tree = rb_funcall(ptp, rb_intern("process_dangling_node"), 1, line);
    }

    switch(nd_type(node)) {

    case NODE_BLOCK:
      tree = rb_funcall(ptp, rb_intern("process_block"), 1, line);
      while (node) {
        process_parse_tree(parse_state, ptp, node->nd_head, locals);
        node = node->nd_next;
      }
      /*
      if (!masgn_level && array_size(current) == 2) {
        array_pop(state, ary);
        array_push(state, ary, array_pop(state, current));
      }
      */
      break;

    case NODE_FBODY:
      tree = rb_funcall(ptp, rb_intern("process_fbody"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_DEFINED:
      tree = rb_funcall(ptp, rb_intern("process_defined"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_COLON2:
      tree = rb_funcall(ptp, rb_intern("process_colon2"), 2, line, Q2SYM(node->nd_mid));
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_MATCH2:
      tree = rb_funcall(ptp, rb_intern("process_match2"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_recv, locals);
      process_parse_tree(parse_state, ptp, node->nd_value, locals);
      break;

    case NODE_MATCH3:
      tree = rb_funcall(ptp, rb_intern("process_match3"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_recv, locals);
      process_parse_tree(parse_state, ptp, node->nd_value, locals);
      break;

    case NODE_BEGIN:
      tree = rb_funcall(ptp, rb_intern("process_begin"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_body, locals);
      break;

    case NODE_OPT_N:
      tree = rb_funcall(ptp, rb_intern("process_opt_n"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_body, locals);
      break;

    case NODE_NOT:
      tree = rb_funcall(ptp, rb_intern("process_not"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_body, locals);
      break;

    case NODE_IF:
      tree = rb_funcall(ptp, rb_intern("process_if"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_cond, locals);
      if (node->nd_body) {
        process_parse_tree(parse_state, ptp, node->nd_body, locals);
      } else {
        // array_push(state, current, Qnil);
      }
      if (node->nd_else) {
        process_parse_tree(parse_state, ptp, node->nd_else, locals);
      } else {
        // array_push(state, current, Qnil);
      }
      break;

    case NODE_CASE:
      tree = rb_funcall(ptp, rb_intern("process_case"), 1, line);
      case_level++;
      if(node->nd_head) {
        process_parse_tree(parse_state, ptp, node->nd_head, locals); /* expr */
      } else {
        // array_push(state, current, Qnil);
      }
      node = node->nd_body;
      while(node) {
        process_parse_tree(parse_state, ptp, node, locals);
        if (nd_type(node) == NODE_WHEN) {                 /* when */
          node = node->nd_next;
        } else {
          break;                                          /* else */
        }
        if (!node) {
          // array_push(state, current, Qnil);               /* no else */
        }
      }
      case_level--;
      break;

    case NODE_WHEN:
      tree = rb_funcall(ptp, rb_intern("process_when"), 1, line);
      when_level++;
      /* when without case, ie, no expr in case */
      if(!inside_case_args && case_level < when_level) {
        if(when_level > 0) when_level--;
        // array_pop(state, ary); /* reset what current is pointing at */
        node = NEW_CASE(0, node);
        goto again;
      }
      inside_case_args++;
      process_parse_tree(parse_state, ptp, node->nd_head, locals); /* args */
      inside_case_args--;

      if(node->nd_body) {
        process_parse_tree(parse_state, ptp, node->nd_body, locals); /* body */
      } else {
        // array_push(state, current, Qnil);
      }

      if(when_level > 0) when_level--;
      break;

    case NODE_WHILE:
      tree = rb_funcall(ptp, rb_intern("process_while"), 1, line);
      process_parse_tree(parse_state, ptp,  node->nd_cond, locals);
      if(node->nd_body) {
        process_parse_tree(parse_state, ptp,  node->nd_body, locals);
      } else {
        // array_push(state, current, Qnil);
      }
      // array_push(state, current, node->nd_3rd == 0 ? Qfalse : Qtrue);
      break;

    case NODE_UNTIL:
      tree = rb_funcall(ptp, rb_intern("process_until"), 1, line);
      process_parse_tree(parse_state, ptp,  node->nd_cond, locals);
      if(node->nd_body) {
        process_parse_tree(parse_state, ptp,  node->nd_body, locals);
      } else {
        // array_push(state, current, Qnil);
      }
      // array_push(state, current, node->nd_3rd == 0 ? Qfalse : Qtrue);
      break;

    case NODE_BLOCK_PASS:
      tree = rb_funcall(ptp, rb_intern("process_block_pass"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_body, locals);
      process_parse_tree(parse_state, ptp, node->nd_iter, locals);
      break;

    case NODE_FOR:
      // TODO: this was combined with ITER
      tree = rb_funcall(ptp, rb_intern("process_for"), 1, line);
      break;

    case NODE_ITER:
      tree = rb_funcall(ptp, rb_intern("process_iter"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_iter, locals);
      masgn_level++;
      if (node->nd_var != (NODE *)1
          && node->nd_var != (NODE *)2
          && node->nd_var != NULL) {
        process_parse_tree(parse_state, ptp, node->nd_var, locals);
      } else {
        if (node->nd_var == NULL) {
          // e.g. proc {}
          // array_push(state, current, Qnil);
        } else {
          // e.g. proc {||}
          // array_push(state, current, I2N(0));
        }
      }
      masgn_level--;
      process_parse_tree(parse_state, ptp, node->nd_body, locals);
      break;

    case NODE_BREAK:
      tree = rb_funcall(ptp, rb_intern("process_break"), 1, line);
      if (node->nd_stts) {
        process_parse_tree(parse_state, ptp, node->nd_stts, locals);
      }
      break;

    case NODE_NEXT:
      tree = rb_funcall(ptp, rb_intern("process_next"), 1, line);
      if (node->nd_stts) {
        process_parse_tree(parse_state, ptp, node->nd_stts, locals);
      }
      break;

    case NODE_YIELD:
      tree = rb_funcall(ptp, rb_intern("process_yield"), 1, line);
      if (node->nd_stts) {
        process_parse_tree(parse_state, ptp, node->nd_stts, locals);
      } else {
        // array_push(state, current, Qnil);
      }
      // array_push(state, current, node->u3.value);
      break;

    case NODE_RESCUE:
      tree = rb_funcall(ptp, rb_intern("process_rescue"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_1st, locals);
      process_parse_tree(parse_state, ptp, node->nd_2nd, locals);
      process_parse_tree(parse_state, ptp, node->nd_3rd, locals);
      break;

    /* rescue body:
     * begin stmt rescue exception => var; stmt; [rescue e2 => v2; s2;]* end
     * stmt rescue stmt
     * a = b rescue c
     */

    case NODE_RESBODY:
      tree = rb_funcall(ptp, rb_intern("process_resbody"), 1, line);
      if(node->nd_3rd) {
        process_parse_tree(parse_state, ptp, node->nd_3rd, locals);
      } else {
        // array_push(state, current, Qnil);
      }
      process_parse_tree(parse_state, ptp, node->nd_2nd, locals);
      process_parse_tree(parse_state, ptp, node->nd_1st, locals);
      break;

    case NODE_ENSURE:
      tree = rb_funcall(ptp, rb_intern("process_ensure"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      if (node->nd_ensr) {
        process_parse_tree(parse_state, ptp, node->nd_ensr, locals);
      }
      break;

    case NODE_AND:
      tree = rb_funcall(ptp, rb_intern("process_and"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_1st, locals);
      process_parse_tree(parse_state, ptp, node->nd_2nd, locals);
      break;

    case NODE_OR:
      tree = rb_funcall(ptp, rb_intern("process_or"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_1st, locals);
      process_parse_tree(parse_state, ptp, node->nd_2nd, locals);
      break;

    case NODE_DOT2:
      tree = rb_funcall(ptp, rb_intern("process_dot2"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_beg, locals);
      process_parse_tree(parse_state, ptp, node->nd_end, locals);
      break;

    case NODE_DOT3:
      tree = rb_funcall(ptp, rb_intern("process_dot3"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_beg, locals);
      process_parse_tree(parse_state, ptp, node->nd_end, locals);
      break;

    case NODE_FLIP2:
      tree = rb_funcall(ptp, rb_intern("process_flip2"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_beg, locals);
      process_parse_tree(parse_state, ptp, node->nd_end, locals);
      break;

    case NODE_FLIP3:
      tree = rb_funcall(ptp, rb_intern("process_flip3"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_beg, locals);
      process_parse_tree(parse_state, ptp, node->nd_end, locals);
      break;

    case NODE_RETURN:
      tree = rb_funcall(ptp, rb_intern("process_return"), 1, line);
      if (node->nd_stts) {
        process_parse_tree(parse_state, ptp, node->nd_stts, locals);
      }
      break;

    case NODE_ARGSCAT:
      tree = rb_funcall(ptp, rb_intern("process_argscat"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      process_parse_tree(parse_state, ptp, node->nd_body, locals);
      break;

    case NODE_ARGSPUSH:
      tree = rb_funcall(ptp, rb_intern("process_argspush"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      process_parse_tree(parse_state, ptp, node->nd_body, locals);
      break;

    case NODE_CALL:
      tree = rb_funcall(ptp, rb_intern("process_call"), 2, line, Q2SYM(node->nd_mid));
      process_parse_tree(parse_state, ptp, node->nd_recv, locals);
      if (node->nd_args) {
        process_parse_tree(parse_state, ptp, node->nd_args, locals);
      }
      break;

    case NODE_FCALL:
      tree = rb_funcall(ptp, rb_intern("process_fcall"), 2, line, Q2SYM(node->nd_mid));
      if (node->nd_args) {
        process_parse_tree(parse_state, ptp, node->nd_args, locals);
      }
      break;

    case NODE_VCALL:
      tree = rb_funcall(ptp, rb_intern("process_vcall"), 2, line, Q2SYM(node->nd_mid));
      process_parse_tree(parse_state, ptp, node->nd_recv, locals);
      if (node->nd_args) {
        process_parse_tree(parse_state, ptp, node->nd_args, locals);
      }
      break;

    case NODE_SUPER:
      tree = rb_funcall(ptp, rb_intern("process_super"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_args, locals);
      break;

  /*
    case NODE_BMETHOD:
      {
        struct BLOCK *data;
        Data_Get_Struct(node->nd_cval, struct BLOCK, data);
        process_parse_tree(parse_state, ptp, data->var, locals);
        process_parse_tree(parse_state, ptp, data->body, locals);
        break;
      }
      break;

    case NODE_DMETHOD:
      {
        struct METHOD *data;
        Data_Get_Struct(node->nd_cval, struct METHOD, data);
        array_push(state, current, Q2SYM(data->id));
        process_parse_tree(parse_state, ptp, data->body, locals);
        break;
      }
  */

    case NODE_METHOD:
      tree = rb_funcall(ptp, rb_intern("process_method"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_3rd, locals);
      break;

    case NODE_SCOPE:
      tree = rb_funcall(ptp, rb_intern("process_scope"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_next, node->nd_tbl);
      break;

    case NODE_OP_ASGN1:
      tree = rb_funcall(ptp, rb_intern("process_op_asgn1"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_recv, locals);
      process_parse_tree(parse_state, ptp, node->nd_args->nd_2nd, locals);
      switch(node->nd_mid) {
        case 0:
          // array_push(state, current, SYMBOL("||"));
          break;
        case 1:
          // array_push(state, current, SYMBOL("&&"));
          break;
        //default:
          // array_push(state, current, Q2SYM(node->nd_mid));
      }
      process_parse_tree(parse_state, ptp, node->nd_args->nd_head, locals);
      break;

    case NODE_OP_ASGN2:
      tree = rb_funcall(ptp, rb_intern("process_op_asgn2"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_recv, locals);
      // array_push(state, current, Q2SYM2(node->nd_next->nd_aid, "="));
      switch(node->nd_next->nd_mid) {
        case 0:
          // array_push(state, current, SYMBOL("||"));
          break;
        case 1:
          // array_push(state, current, SYMBOL("&&"));
          break;
        //default:
          // array_push(state, current, Q2SYM(node->nd_next->nd_mid));
      }
      process_parse_tree(parse_state, ptp, node->nd_value, locals);
      break;

    case NODE_OP_ASGN_AND:
      tree = rb_funcall(ptp, rb_intern("process_op_asgn_and"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      process_parse_tree(parse_state, ptp, node->nd_value, locals);
      break;

    case NODE_OP_ASGN_OR:
      tree = rb_funcall(ptp, rb_intern("process_op_asgn_or"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      process_parse_tree(parse_state, ptp, node->nd_value, locals);
      break;

    case NODE_MASGN:
      tree = rb_funcall(ptp, rb_intern("process_masgn"), 1, line);
      masgn_level++;
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      if (node->nd_args) {
        if(node->nd_args != (NODE *)-1) {
          process_parse_tree(parse_state, ptp, node->nd_args, locals);
        } else {
          // array_push(state, current, wrap_into_node(state, "splat"));
        }
      }
      process_parse_tree(parse_state, ptp, node->nd_value, locals);
      masgn_level--;
      break;

    case NODE_LASGN:
      tree = rb_funcall(ptp, rb_intern("process_lasgn"), 2, line, Q2SYM(node->nd_vid));
      process_parse_tree(parse_state, ptp, node->nd_value, locals);
      break;

    case NODE_IASGN:
      tree = rb_funcall(ptp, rb_intern("process_iasgn"), 2, line, Q2SYM(node->nd_vid));
      process_parse_tree(parse_state, ptp, node->nd_value, locals);
      break;

    case NODE_CVASGN:
      tree = rb_funcall(ptp, rb_intern("process_cvasgn"), 2, line, Q2SYM(node->nd_vid));
      process_parse_tree(parse_state, ptp, node->nd_value, locals);
      break;

    case NODE_CVDECL:
      tree = rb_funcall(ptp, rb_intern("process_cvdecl"), 2, line, Q2SYM(node->nd_vid));
      process_parse_tree(parse_state, ptp, node->nd_value, locals);
      break;

    case NODE_GASGN:
      tree = rb_funcall(ptp, rb_intern("process_gasgn"), 2, line, Q2SYM(node->nd_vid));
      process_parse_tree(parse_state, ptp, node->nd_value, locals);
      break;

    case NODE_CDECL:
      tree = rb_funcall(ptp, rb_intern("process_cdecl"), 1, line);
      if(node->nd_vid) {
        // array_push(state, current, Q2SYM(node->nd_vid));
      } else {
        process_parse_tree(parse_state, ptp, node->nd_else, locals);
      }

      process_parse_tree(parse_state, ptp, node->nd_value, locals);
      break;

    case NODE_VALIAS:           /* u1 u2 (alias $global $global2) */
      tree = rb_funcall(ptp, rb_intern("process_valias"), 3, line,
          Q2SYM(node->u2.id), Q2SYM(node->u1.id));
      break;

    case NODE_ALIAS:            /* u1 u2 (alias :blah :blah2) */
      tree = rb_funcall(ptp, rb_intern("process_alias"), 3, line,
          Q2SYM(node->u2.id), Q2SYM(node->u1.id));
      break;

    case NODE_UNDEF:            /* u2    (undef instvar) */
      tree = rb_funcall(ptp, rb_intern("process_undef"), 2, line, Q2SYM(node->u2.id));
      break;

    case NODE_COLON3:           /* u2    (::OUTER_CONST) */
      tree = rb_funcall(ptp, rb_intern("process_colon3"), 2, line, Q2SYM(node->u2.id));
      break;

    case NODE_HASH: {
      tree = rb_funcall(ptp, rb_intern("process_hash"), 1, line);
      NODE *list;
      list = node->nd_head;
      while (list) {
        process_parse_tree(parse_state, ptp, list->nd_head, locals);
        list = list->nd_next;
        if (list == 0) {
          printf("odd number list for Hash");
          abort();
        }
        process_parse_tree(parse_state, ptp, list->nd_head, locals);
        list = list->nd_next;
      }
      break;
    }

    case NODE_ARRAY:
      tree = rb_funcall(ptp, rb_intern("process_array"), 1, line);
      while (node) {
        process_parse_tree(parse_state, ptp, node->nd_head, locals);
        node = node->nd_next;
      }
      break;

    case NODE_DSTR:
    case NODE_DSYM:
    case NODE_DXSTR:
    case NODE_DREGX:
    case NODE_DREGX_ONCE:
      {
        NODE *list = node->nd_next;
        // array_push(state, current, string_newfrombstr(state, node->nd_str));
        while (list) {
          if (list->nd_head) {
            switch (nd_type(list->nd_head)) {
              case NODE_STR:
                process_parse_tree(parse_state, ptp, list->nd_head, locals);
              break;
              case NODE_EVSTR:
                process_parse_tree(parse_state, ptp, list->nd_head, locals);
              break;
              default:
                process_parse_tree(parse_state, ptp, list->nd_head, locals);
              break;
            }
          }
          list = list->nd_next;
        }

        /* the regex options. */
        switch(nd_type(node)) {
        case NODE_DREGX:
        case NODE_DREGX_ONCE:
          if (node->nd_cflag) {
            // array_push(state, current, I2N(node->nd_cflag));
          }
        }
      }
      break;

    case NODE_DEFN:
      tree = rb_funcall(ptp, rb_intern("process_defn"), 2, line, Q2SYM(node->nd_mid));
      if (node->nd_defn) {
        process_parse_tree(parse_state, ptp, node->nd_defn, locals);
      }

      break;

    case NODE_DEFS:
      tree = rb_funcall(ptp, rb_intern("process_defs"), 2, line, Q2SYM(node->nd_mid));
      if (node->nd_defn) {
        process_parse_tree(parse_state, ptp, node->nd_recv, locals);
        process_parse_tree(parse_state, ptp, node->nd_defn, locals);
      }
      break;

    case NODE_CLASS:
      tree = rb_funcall(ptp, rb_intern("process_class"), 1, line);
      if (nd_type(node->nd_cpath) == NODE_COLON2 && !node->nd_cpath->nd_vid) {
        // array_push(state, current, Q2SYM((ID)node->nd_cpath->nd_mid));
      } else {
        process_parse_tree(parse_state, ptp, node->nd_cpath, locals);
      }
      if(node->nd_super) {
        process_parse_tree(parse_state, ptp, node->nd_super, locals);
      } else {
        // array_push(state, current, Qnil);
      }
      process_parse_tree(parse_state, ptp, node->nd_body, locals);
      break;

    case NODE_MODULE:
      tree = rb_funcall(ptp, rb_intern("process_module"), 1, line);
      if (nd_type(node->nd_cpath) == NODE_COLON2 && !node->nd_cpath->nd_vid) {
        // array_push(state, current, Q2SYM((ID)node->nd_cpath->nd_mid));
      } else {
        process_parse_tree(parse_state, ptp, node->nd_cpath, locals);
      }
      process_parse_tree(parse_state, ptp, node->nd_body, locals);
      break;

    case NODE_SCLASS:
      tree = rb_funcall(ptp, rb_intern("process_sclass"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_recv, locals);
      process_parse_tree(parse_state, ptp, node->nd_body, locals);
      break;

    case NODE_ARGS: {
      NODE *optnode;
      int i = 0, max_args = node->nd_cnt;

      /* push regular argument names */
      for (; i < max_args; i++) {
        // array_push(state, current, Q2SYM(locals[i + 3]));
      }

      /* look for optional arguments: we cannot assume these
       * are left-to-right what the locals array contains
       * (e.g. def(a=1, b=2, c=lambda {|n| n } will have 'n'
       * at i (above) + 2 + 3); instead we walk the chain
       * and look at the actual LASGN nodes
       */
      masgn_level++;
      optnode = node->nd_opt;
      while (optnode) {
        if(nd_type(optnode) == NODE_LASGN) {
          // array_push(state, current, Q2SYM(optnode->nd_vid));
        } else if(nd_type(optnode) == NODE_BLOCK
                  && nd_type(optnode->nd_head) == NODE_LASGN) {
          // array_push(state, current, Q2SYM(optnode->nd_head->nd_vid));
        }
        i++;  // do not use here but keep track for '*args' name below
        optnode = optnode->nd_next;
      }

      /* look for vargs */
      long arg_count = (long)node->nd_rest;
      if (arg_count > 0) {
        /* *arg name */
        // Object* sym = string_new(state, "*");
        if (locals[i + 3]) {
          // string_concat(state, sym, string_new(state,
                // quark_to_string(id_to_quark(locals[i + 3]))));
        }
        // array_push(state, current, STR2SYM(sym));
      } else if (arg_count == 0) {
        /* nothing to do in this case, empty list */
      } else if (arg_count == -1) {
        /* nothing to do in this case, handled above */
      } else if (arg_count == -2) {
        /* nothing to do in this case, no name == no use */
        // array_push(state, current, SYMBOL("*"));
      } else {
        // HACK: replace with Exception::argument_error()
        printf("Unknown arg_count %ld encountered while processing args.\n", arg_count);
      }

      tree = rb_funcall(ptp, rb_intern("process_args"), 1, line);

      optnode = node->nd_opt;
      if (optnode) {
        process_parse_tree(parse_state, ptp, node->nd_opt, locals);
      }
      masgn_level--;
    }  break;

    case NODE_LVAR:
      tree = rb_funcall(ptp, rb_intern("process_lvar"), 2, line, Q2SYM(node->nd_vid));
      break;

    case NODE_DVAR:
      tree = rb_funcall(ptp, rb_intern("process_dvar"), 2, line, Q2SYM(node->nd_vid));
      break;

    case NODE_IVAR:
      tree = rb_funcall(ptp, rb_intern("process_ivar"), 2, line, Q2SYM(node->nd_vid));
      break;

    case NODE_CVAR:
      tree = rb_funcall(ptp, rb_intern("process_cvar"), 2, line, Q2SYM(node->nd_vid));
      break;

    case NODE_GVAR:
      tree = rb_funcall(ptp, rb_intern("process_gvar"), 2, line, Q2SYM(node->nd_vid));
      break;

    case NODE_CONST:
      tree = rb_funcall(ptp, rb_intern("process_const"), 2, line, Q2SYM(node->nd_vid));
      break;

    case NODE_ATTRSET:
      tree = rb_funcall(ptp, rb_intern("process_attrset"), 2, line, Q2SYM(node->nd_vid));
      break;

    case NODE_FIXNUM:
      tree = rb_funcall(ptp, rb_intern("process_fixnum"), 2, line, INT2FIX(node->nd_cnt));
      break;

    case NODE_NUMBER:
      tree = rb_funcall(ptp, rb_intern("process_number"), 3, line, INT2FIX(0),
          string_newfrombstr(node->nd_str));
      bdestroy(node->nd_str);
      break;

    case NODE_HEXNUM:
      tree = rb_funcall(ptp, rb_intern("process_number"), 3, line, INT2FIX(16),
          string_newfrombstr(node->nd_str));
      bdestroy(node->nd_str);
      break;

    case NODE_BINNUM:
      tree = rb_funcall(ptp, rb_intern("process_number"), 3, line, INT2FIX(2),
          string_newfrombstr(node->nd_str));
      bdestroy(node->nd_str);
      break;

    case NODE_OCTNUM:
      tree = rb_funcall(ptp, rb_intern("process_number"), 3, line, INT2FIX(8),
          string_newfrombstr(node->nd_str));
      bdestroy(node->nd_str);
    break;

    case NODE_FLOAT:
      tree = rb_funcall(ptp, rb_intern("process_float"), 2, line,
          string_newfrombstr(node->nd_str));
      bdestroy(node->nd_str);
    break;

    case NODE_XSTR:             /* u1    (%x{ls}) */
      tree = rb_funcall(ptp, rb_intern("process_xstr"), 2, line,
          string_newfrombstr(node->nd_str));
      bdestroy(node->nd_str);
      break;

    case NODE_STR:              /* u1 */
      tree = rb_funcall(ptp, rb_intern("process_str"), 2, line,
          string_newfrombstr(node->nd_str));
      bdestroy(node->nd_str);
      break;

    case NODE_REGEX:
      tree = rb_funcall(ptp, rb_intern("process_regex"), 3, line,
          string_newfrombstr(node->nd_str), INT2FIX(node->nd_cnt));
      bdestroy(node->nd_str);
      break;

    case NODE_MATCH:
      tree = rb_funcall(ptp, rb_intern("process_match"), 3, line,
          string_newfrombstr(node->nd_str), INT2FIX(node->nd_cnt));
      bdestroy(node->nd_str);
      break;

    case NODE_LIT:
      tree = rb_funcall(ptp, rb_intern("process_lit"), 2, line,
          Q2SYM((uintptr_t)node->nd_lit));
      break;

    case NODE_NEWLINE:
      node = node->nd_next;
      goto again;

      break;

    case NODE_NTH_REF:          /* u2 u3 ($1) - u3 is local_cnt('~') ignorable? */
      tree = rb_funcall(ptp, rb_intern("process_nth_ref"), 2, line, INT2FIX(node->nd_nth));
      break;

    case NODE_BACK_REF: {        /* u2 u3 ($& etc) */
      char str[2];
      str[0] = node->nd_nth;
      str[1] = 0;
      tree = rb_funcall(ptp, rb_intern("process_back_ref"), 2, line, rb_intern(str));
      break;
    }

    case NODE_BLOCK_ARG:        /* u1 u3 (def x(&b) */
      tree = rb_funcall(ptp, rb_intern("process_block_arg"), 2, line, Q2SYM(node->u1.id));
      break;

    case NODE_RETRY:
      tree = rb_funcall(ptp, rb_intern("process_retry"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_FALSE:
      tree = rb_funcall(ptp, rb_intern("process_false"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_NIL:
      tree = rb_funcall(ptp, rb_intern("process_nil"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_SELF:
      tree = rb_funcall(ptp, rb_intern("process_self"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_TRUE:
      tree = rb_funcall(ptp, rb_intern("process_true"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_ZARRAY:
      tree = rb_funcall(ptp, rb_intern("process_zarray"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_ZSUPER:
      tree = rb_funcall(ptp, rb_intern("process_zsuper"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_REDO:
      tree = rb_funcall(ptp, rb_intern("process_redo"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_FILE:
      break;

    case NODE_SPLAT:
      tree = rb_funcall(ptp, rb_intern("process_splat"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_TO_ARY:
      tree = rb_funcall(ptp, rb_intern("process_to_ary"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_SVALUE:             /* a = b, c */
      tree = rb_funcall(ptp, rb_intern("process_svalue"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_ATTRASGN:           /* literal.meth = y u1 u2 u3 */
      tree = rb_funcall(ptp, rb_intern("process_attrasgn"), 2, line, Q2SYM(node->u2.id));

      /* node id node */
      if (node->nd_1st == RNODE(1)) {
        process_parse_tree(parse_state, ptp, NEW_SELF(), locals);
      } else {
        process_parse_tree(parse_state, ptp, node->nd_1st, locals);
      }
      process_parse_tree(parse_state, ptp, node->nd_3rd, locals);
      break;

    case NODE_EVSTR:
      tree = rb_funcall(ptp, rb_intern("process_evstr"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_2nd, locals);
      break;

    case NODE_NEGATE:
      tree = rb_funcall(ptp, rb_intern("process_negate"), 1, line);
      process_parse_tree(parse_state, ptp, node->nd_head, locals);
      break;

    case NODE_POSTEXE:            /* END { ... } */
      /* Nothing to do here... we are in an iter block */
      break;

    /*
    case NODE_CFUNC:
      array_push(state, current, I2N(node->nd_cfnc));
      array_push(state, current, I2N(node->nd_argc));
      break;
    */

    /* Nodes we found but have yet to decypher
       I think these are all runtime only... not positive but... */
    case NODE_MEMO:               /* enum.c zip */
    case NODE_CREF:
    case NODE_IFUNC:
    /* #defines:
       case NODE_LMASK:
       case NODE_LSHIFT: */

    default: {

      VALUE node_name = rb_str_new2(get_node_type_string((enum node_type)nd_type(node)));
      VALUE node_type = INT2FIX(nd_type(node));
      tree = rb_funcall(ptp, rb_intern("process_missing_node"), 3,
          line, node_name, node_type);
      break;
      }
    }

    return tree;
  }
};

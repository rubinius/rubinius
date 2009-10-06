#include <string.h>

#include "parser/grammar.hpp"
#include "parser/grammar_runtime.hpp"
#include "parser/grammar_internal.hpp"

#include "builtin/array.hpp"
#include "builtin/bignum.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"

#include "vm.hpp"

namespace rubinius {
  namespace parser {

/* A bunch of convenience macros that are closer to the MRI
 * API than to Rubinius', making porting from ParseTree a
 * little easier. This whole thing is a big hack and someday
 * we'll have a PEG parser and we'll throw a big bonfire
 * party and burn this code. Till then, it's hella fast.
 */
#define array_new(s, n)       (Object*)Array::create(s, n)
#define array_size(a)         as<Array>(a)->size()
#define array_push(s, a, v)   as<Array>(a)->append(s, v)
#define array_append(s, a, v) as<Array>(a)->append(s, v)
#define array_pop(s, a)       as<Array>(a)->pop(s)
#define array_get(s, a, i)    as<Array>(a)->get(s, i)
#define array_set(s, a, i, v) as<Array>(a)->set(s, i, v)
#define array_entry(s, a, i)  as<Array>(a)->aref(s, Fixnum::from(i))
#define array_get_total(a)    as<Array>(a)->total()

#define tuple_new(s, n)       (Object*)Tuple::create(s, n)
#define tuple_put(s, t, i, v) as<Tuple>(t)->put(s, i, v)

#define string_new(s, c)         (Object*)String::create(s, c)
#define string_concat(s, d, o)   (Object*)as<String>(d)->append(s, as<String>(o))
#define string_append(s, d, o)   (Object*)as<String>(d)->append(s, o)
#define string_c_str(s, d)       (char*)as<String>(d)->c_str()

#define float_from_string(s, d)  (Object*)String::create(s, d)->to_f(s)

#define bignum_from_string(s, d, r) string_new(s, d)->to_i(s, Fixnum::from(r), Qfalse)
#define bignum_from_string_detect(s, d) bignum_from_string(s, d, 0)

#define I2N(i) Fixnum::from(i)
#define N2I(n) (n)->to_native()

#define SYMBOL(str)               state->symbol(str)
#define STR2SYM(str)              state->symbol(as<String>(str))

    static Object* float_from_bstring(STATE, bstring str) {
      return float_from_string(state, bdata(str));
    }

    static Object* bignum_from_bstring_detect(STATE, bstring str) {
      return bignum_from_string_detect(state, bdata(str));
    }

    static Object* bignum_from_bstring(STATE, bstring str, int radix) {
      return bignum_from_string(state, bdata(str), radix);
    }

    static Object* string_newfrombstr(STATE, bstring str)
    {
      if(str == NULL) {
        return String::create(state, "");
      }

      return String::create(state, (const char*)str->data, str->slen);
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

    extern int syd_sourceline;

    void create_error(rb_parse_state *parse_state, char *msg) {
      int col;
      STATE;
      Object* tup;

      state = parse_state->state;

      col = parse_state->lex_p - parse_state->lex_pbeg;

      tup = tuple_new(state, 4);
      tuple_put(state, tup, 0, string_new(state, msg));
      tuple_put(state, tup, 1, I2N(col));
      tuple_put(state, tup, 2, I2N(syd_sourceline));
      tuple_put(state, tup, 3, string_newfrombstr(state, parse_state->lex_lastline));
      parse_state->error = tup;
    }

    /*
     *
     * 100% of the credit for the following node tree => sexp transform goes
     * to Ryan Davis (zenspider) and Eric Hodel (drbrain).
     *
     * It was mearly imported into this space by Evan Webb.
     */

#define nd_3rd   u3.node

#define VALUE Object*

#define add_to_parse_tree(a,n,l) syd_add_to_parse_tree(state,parse_state,a,n,l)
#undef ID2SYM
#define Q2SYM(v)      quark_to_symbol(state, v)
#define Q2SYM2(v, s)  quark_to_symbol_append(state, v, s)

    const char *op_to_name(ID id);

    static Object* wrap_into_node(STATE, const char * name, Object* val) {
      Object* ary = array_new(state, val ? 2 : 1);
      array_push(state, ary, SYMBOL(name));
      if (val) array_push(state, ary, val);
      return ary;
    }

    static Object* quark_to_symbol(STATE, quark quark) {
      const char *op;
      op = op_to_name(quark);
      if(op) {
        return SYMBOL(op);
      }
      return SYMBOL(quark_to_string(id_to_quark(quark)));
    }

    static Object* quark_to_symbol_append(STATE, quark quark, const char* other) {
      Object* str;
      const char *op, *s;

      op = op_to_name(quark);
      if(op) {
        str = string_new(state, op);
      } else {
        str = string_new(state, quark_to_string(id_to_quark(quark)));
      }

      s = string_c_str(state, str);
      if(s[strlen(s)-1] == '=') {
        return STR2SYM(str);
      } else {
        return STR2SYM(string_append(state, str, other));
      }
    }

    void syd_add_to_parse_tree(STATE, rb_parse_state* parse_state,
                               Object* ary, NODE* n, ID* locals) {
      NODE * volatile node = n;
      VALUE current;
      VALUE node_name;

      static int masgn_level = 0;
      static unsigned case_level = 0;
      static unsigned when_level = 0;
      static unsigned inside_case_args = 0;

      if (!node) return;

    again:

      if (node) {
        node_name = SYMBOL(get_node_type_string((enum node_type)nd_type(node)));
        /*
        if (RTEST(ruby_debug)) {
          fprintf(stderr, "%15s: %s%s%s\n",
            get_node_type_string(nd_type(node)),
            (RNODE(node)->u1.node != NULL ? "u1 " : "   "),
            (RNODE(node)->u2.node != NULL ? "u2 " : "   "),
            (RNODE(node)->u3.node != NULL ? "u3 " : "   "));
        }
        */
      } else {
        node_name = SYMBOL("ICKY");
      }

      current = array_new(state, 4);
      array_push(state, ary, current);
      array_push(state, current, node_name);

      current->set_ivar(state, SYMBOL("@file"), string_new(state, node->nd_file));
      current->set_ivar(state, SYMBOL("@line"), Fixnum::from(nd_line(node)));

        switch (nd_type(node)) {

        case NODE_BLOCK:
          while (node) {
            add_to_parse_tree(current, node->nd_head, locals);
            node = node->nd_next;
          }
          if (!masgn_level && array_size(current) == 2) {
            array_pop(state, ary);
            array_push(state, ary, array_pop(state, current));
            return;
          }
          break;

        case NODE_FBODY:
        case NODE_DEFINED:
          add_to_parse_tree(current, node->nd_head, locals);
          break;

        case NODE_COLON2:
          add_to_parse_tree(current, node->nd_head, locals);
          array_push(state, current, Q2SYM(node->nd_mid));
          break;

        case NODE_MATCH2:
        case NODE_MATCH3:
          add_to_parse_tree(current, node->nd_recv, locals);
          add_to_parse_tree(current, node->nd_value, locals);
          break;

        case NODE_BEGIN:
        case NODE_OPT_N:
        case NODE_NOT:
          add_to_parse_tree(current, node->nd_body, locals);
          break;

        case NODE_IF:
          add_to_parse_tree(current, node->nd_cond, locals);
          if (node->nd_body) {
            add_to_parse_tree(current, node->nd_body, locals);
          } else {
            array_push(state, current, Qnil);
          }
          if (node->nd_else) {
            add_to_parse_tree(current, node->nd_else, locals);
          } else {
            array_push(state, current, Qnil);
          }
          break;

      case NODE_CASE:
        case_level++;
        if(node->nd_head) {
          add_to_parse_tree(current, node->nd_head, locals); /* expr */
        } else {
          array_push(state, current, Qnil);
        }
        node = node->nd_body;
        while(node) {
          add_to_parse_tree(current, node, locals);
          if (nd_type(node) == NODE_WHEN) {                 /* when */
            node = node->nd_next;
          } else {
            break;                                          /* else */
          }
          if (!node) {
            array_push(state, current, Qnil);               /* no else */
          }
        }
        case_level--;
        break;

      case NODE_WHEN:
        when_level++;
        /* when without case, ie, no expr in case */
        if(!inside_case_args && case_level < when_level) {
          if(when_level > 0) when_level--;
          array_pop(state, ary); /* reset what current is pointing at */
          node = NEW_CASE(0, node);
          goto again;
        }
        inside_case_args++;
        add_to_parse_tree(current, node->nd_head, locals); /* args */
        inside_case_args--;

        if(node->nd_body) {
          add_to_parse_tree(current, node->nd_body, locals); /* body */
        } else {
          array_push(state, current, Qnil);
        }

        if(when_level > 0) when_level--;
        break;

      case NODE_WHILE:
      case NODE_UNTIL:
        add_to_parse_tree(current,  node->nd_cond, locals);
        if(node->nd_body) {
          add_to_parse_tree(current,  node->nd_body, locals);
        } else {
          array_push(state, current, Qnil);
        }
        array_push(state, current, node->nd_3rd == 0 ? Qfalse : Qtrue);
        break;

      case NODE_BLOCK_PASS:
        add_to_parse_tree(current, node->nd_body, locals);
        add_to_parse_tree(current, node->nd_iter, locals);
        break;

      case NODE_ITER:
      case NODE_FOR:
        add_to_parse_tree(current, node->nd_iter, locals);
        masgn_level++;
        if (node->nd_var != (NODE *)1
            && node->nd_var != (NODE *)2
            && node->nd_var != NULL) {
          add_to_parse_tree(current, node->nd_var, locals);
        } else {
          if (node->nd_var == NULL) {
            // e.g. proc {}
            array_push(state, current, Qnil);
          } else {
            // e.g. proc {||}
            array_push(state, current, I2N(0));
          }
        }
        masgn_level--;
        add_to_parse_tree(current, node->nd_body, locals);
        break;

      case NODE_BREAK:
      case NODE_NEXT:
        if (node->nd_stts) {
          add_to_parse_tree(current, node->nd_stts, locals);
        }
        break;

      case NODE_YIELD:
        if (node->nd_stts) {
          add_to_parse_tree(current, node->nd_stts, locals);
        } else {
          array_push(state, current, Qnil);
        }
        array_push(state, current, node->u3.value);
        break;

      case NODE_RESCUE:
        add_to_parse_tree(current, node->nd_1st, locals);
        add_to_parse_tree(current, node->nd_2nd, locals);
        add_to_parse_tree(current, node->nd_3rd, locals);
        break;

      /* rescue body:
       * begin stmt rescue exception => var; stmt; [rescue e2 => v2; s2;]* end
       * stmt rescue stmt
       * a = b rescue c
       */

      case NODE_RESBODY:
        if(node->nd_3rd) {
          add_to_parse_tree(current, node->nd_3rd, locals);
        } else {
          array_push(state, current, Qnil);
        }
        add_to_parse_tree(current, node->nd_2nd, locals);
        add_to_parse_tree(current, node->nd_1st, locals);
        break;

      case NODE_ENSURE:
        add_to_parse_tree(current, node->nd_head, locals);
        if (node->nd_ensr) {
          add_to_parse_tree(current, node->nd_ensr, locals);
        }
        break;

      case NODE_AND:
      case NODE_OR:
        add_to_parse_tree(current, node->nd_1st, locals);
        add_to_parse_tree(current, node->nd_2nd, locals);
        break;

      case NODE_DOT2:
      case NODE_DOT3:
      case NODE_FLIP2:
      case NODE_FLIP3:
        add_to_parse_tree(current, node->nd_beg, locals);
        add_to_parse_tree(current, node->nd_end, locals);
        break;

      case NODE_RETURN:
        if (node->nd_stts) {
          add_to_parse_tree(current, node->nd_stts, locals);
        }
        break;

      case NODE_ARGSCAT:
      case NODE_ARGSPUSH:
        add_to_parse_tree(current, node->nd_head, locals);
        add_to_parse_tree(current, node->nd_body, locals);
        break;

      case NODE_CALL:
      case NODE_FCALL:
      case NODE_VCALL:
        if (nd_type(node) != NODE_FCALL) {
          add_to_parse_tree(current, node->nd_recv, locals);
        }
        array_push(state, current, Q2SYM(node->nd_mid));
        if (node->nd_args || nd_type(node) != NODE_FCALL) {
          add_to_parse_tree(current, node->nd_args, locals);
        }
        break;

      case NODE_SUPER:
        add_to_parse_tree(current, node->nd_args, locals);
        break;

    /*
      case NODE_BMETHOD:
        {
          struct BLOCK *data;
          Data_Get_Struct(node->nd_cval, struct BLOCK, data);
          add_to_parse_tree(current, data->var, locals);
          add_to_parse_tree(current, data->body, locals);
          break;
        }
        break;

      case NODE_DMETHOD:
        {
          struct METHOD *data;
          Data_Get_Struct(node->nd_cval, struct METHOD, data);
          array_push(state, current, Q2SYM(data->id));
          add_to_parse_tree(current, data->body, locals);
          break;
        }
    */

      case NODE_METHOD:
        add_to_parse_tree(current, node->nd_3rd, locals);
        break;

      case NODE_SCOPE:
        add_to_parse_tree(current, node->nd_next, node->nd_tbl);
        break;

      case NODE_OP_ASGN1:
        add_to_parse_tree(current, node->nd_recv, locals);
        add_to_parse_tree(current, node->nd_args->nd_2nd, locals);
        switch(node->nd_mid) {
          case 0:
            array_push(state, current, SYMBOL("||"));
            break;
          case 1:
            array_push(state, current, SYMBOL("&&"));
            break;
          default:
            array_push(state, current, Q2SYM(node->nd_mid));
        }
        add_to_parse_tree(current, node->nd_args->nd_head, locals);
        break;

      case NODE_OP_ASGN2:
        add_to_parse_tree(current, node->nd_recv, locals);
        array_push(state, current, Q2SYM2(node->nd_next->nd_aid, "="));
        switch(node->nd_next->nd_mid) {
          case 0:
            array_push(state, current, SYMBOL("||"));
            break;
          case 1:
            array_push(state, current, SYMBOL("&&"));
            break;
          default:
            array_push(state, current, Q2SYM(node->nd_next->nd_mid));
        }
        add_to_parse_tree(current, node->nd_value, locals);
        break;

      case NODE_OP_ASGN_AND:
      case NODE_OP_ASGN_OR:
        add_to_parse_tree(current, node->nd_head, locals);
        add_to_parse_tree(current, node->nd_value, locals);
        break;

      case NODE_MASGN:
        masgn_level++;
        add_to_parse_tree(current, node->nd_head, locals);
        if (node->nd_args) {
          if(node->nd_args != (NODE *)-1) {
            add_to_parse_tree(current, node->nd_args, locals);
          } else {
            array_push(state, current, wrap_into_node(state, "splat", 0));
          }
        }
        add_to_parse_tree(current, node->nd_value, locals);
        masgn_level--;
        break;

      case NODE_LASGN:
      case NODE_IASGN:
      case NODE_DASGN:
      case NODE_CVASGN:
      case NODE_CVDECL:
      case NODE_GASGN:
        array_push(state, current, Q2SYM(node->nd_vid));
        add_to_parse_tree(current, node->nd_value, locals);
        break;

      case NODE_CDECL:
        if(node->nd_vid) {
          array_push(state, current, Q2SYM(node->nd_vid));
        } else {
          add_to_parse_tree(current, node->nd_else, locals);
        }

        add_to_parse_tree(current, node->nd_value, locals);
        break;

      case NODE_DASGN_CURR:
        array_push(state, current, Q2SYM(node->nd_vid));
        if (node->nd_value) {
          add_to_parse_tree(current, node->nd_value, locals);
          if (!masgn_level && array_size(current) == 2) {
            array_pop(state, ary);
            return;
          }
        } else {
          if (!masgn_level) {
            array_pop(state, ary);
            return;
          }
        }
        break;

      case NODE_VALIAS:           /* u1 u2 (alias $global $global2) */
        array_push(state, current, Q2SYM(node->u2.id));
        array_push(state, current, Q2SYM(node->u1.id));
        break;

      case NODE_ALIAS:            /* u1 u2 (alias :blah :blah2) */
        add_to_parse_tree(current, node->u2.node, locals);
        add_to_parse_tree(current, node->u1.node, locals);
        break;

      case NODE_UNDEF:            /* u2    (undef instvar) */
        add_to_parse_tree(current, node->u2.node, locals);
        break;

      case NODE_COLON3:           /* u2    (::OUTER_CONST) */
        array_push(state, current, Q2SYM(node->u2.id));
        break;

      case NODE_HASH:
        {
          NODE *list;
          list = node->nd_head;
          while (list) {
            add_to_parse_tree(current, list->nd_head, locals);
            list = list->nd_next;
            if (list == 0) {
              printf("odd number list for Hash");
              abort();
            }
            add_to_parse_tree(current, list->nd_head, locals);
            list = list->nd_next;
          }
        }
        break;

      case NODE_ARRAY:
        while (node) {
          add_to_parse_tree(current, node->nd_head, locals);
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
          array_push(state, current, string_newfrombstr(state, node->nd_str));
          while (list) {
            if (list->nd_head) {
              switch (nd_type(list->nd_head)) {
                case NODE_STR:
                  add_to_parse_tree(current, list->nd_head, locals);
                break;
                case NODE_EVSTR:
                  add_to_parse_tree(current, list->nd_head, locals);
                break;
                default:
                  add_to_parse_tree(current, list->nd_head, locals);
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
              array_push(state, current, I2N(node->nd_cflag));
            }
          }
        }
        break;

      case NODE_DEFN:
      case NODE_DEFS:
        if (node->nd_defn) {
          if (nd_type(node) == NODE_DEFS)
              add_to_parse_tree(current, node->nd_recv, locals);
          array_push(state, current, Q2SYM(node->nd_mid));
          add_to_parse_tree(current, node->nd_defn, locals);
        }
        break;

      case NODE_CLASS:
      case NODE_MODULE:
        if (nd_type(node->nd_cpath) == NODE_COLON2 && !node->nd_cpath->nd_vid) {
          array_push(state, current, Q2SYM((ID)node->nd_cpath->nd_mid));
        } else {
          add_to_parse_tree(current, node->nd_cpath, locals);
        }

        if (nd_type(node) == NODE_CLASS) {
          if(node->nd_super) {
            add_to_parse_tree(current, node->nd_super, locals);
          } else {
            array_push(state, current, Qnil);
          }
        }
        add_to_parse_tree(current, node->nd_body, locals);
        break;

      case NODE_SCLASS:
        add_to_parse_tree(current, node->nd_recv, locals);
        add_to_parse_tree(current, node->nd_body, locals);
        break;

      case NODE_ARGS: {
        NODE *optnode;
        int i = 0, max_args = node->nd_cnt;

        /* push regular argument names */
        for (; i < max_args; i++) {
          array_push(state, current, Q2SYM(locals[i + 3]));
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
            array_push(state, current, Q2SYM(optnode->nd_vid));
          } else if(nd_type(optnode) == NODE_BLOCK
                    && nd_type(optnode->nd_head) == NODE_LASGN) {
            array_push(state, current, Q2SYM(optnode->nd_head->nd_vid));
          }
          i++;  // do not use here but keep track for '*args' name below
          optnode = optnode->nd_next;
        }

        /* look for vargs */
        long arg_count = (long)node->nd_rest;
        if (arg_count > 0) {
          /* *arg name */
          Object* sym = string_new(state, "*");
          if (locals[i + 3]) {
            string_concat(state, sym, string_new(state,
                  quark_to_string(id_to_quark(locals[i + 3]))));
          }
          array_push(state, current, STR2SYM(sym));
        } else if (arg_count == 0) {
          /* nothing to do in this case, empty list */
        } else if (arg_count == -1) {
          /* nothing to do in this case, handled above */
        } else if (arg_count == -2) {
          /* nothing to do in this case, no name == no use */
          array_push(state, current, SYMBOL("*"));
        } else {
          // HACK: replace with Exception::argument_error()
          printf("Unknown arg_count %ld encountered while processing args.\n", arg_count);
        }

        optnode = node->nd_opt;
        if (optnode) {
          add_to_parse_tree(current, node->nd_opt, locals);
        }
        masgn_level--;
      }  break;

      case NODE_LVAR:
      case NODE_DVAR:
      case NODE_IVAR:
      case NODE_CVAR:
      case NODE_GVAR:
      case NODE_CONST:
      case NODE_ATTRSET:
        array_push(state, current, Q2SYM(node->nd_vid));
        break;

      case NODE_FIXNUM:
        array_set(state, current, 0, SYMBOL("lit"));
        array_push(state, current, I2N(node->nd_cnt));
        break;

      case NODE_NUMBER:
        array_set(state, current, 0, SYMBOL("lit"));
        array_push(state, current, bignum_from_bstring_detect(state, node->nd_str));
        bdestroy(node->nd_str);
        break;

      case NODE_HEXNUM:
        array_set(state, current, 0, SYMBOL("lit"));
        array_push(state, current, bignum_from_bstring(state, node->nd_str, 16));
        bdestroy(node->nd_str);
        break;

      case NODE_BINNUM:
        array_set(state, current, 0, SYMBOL("lit"));
        array_push(state, current, bignum_from_bstring(state, node->nd_str, 2));
        bdestroy(node->nd_str);
        break;

      case NODE_OCTNUM:
        array_set(state, current, 0, SYMBOL("lit"));
        array_push(state, current, bignum_from_bstring(state, node->nd_str, 8));
        bdestroy(node->nd_str);
      break;

      case NODE_FLOAT:
        array_set(state, current, 0, SYMBOL("lit"));
        array_push(state, current, float_from_bstring(state, node->nd_str));
        bdestroy(node->nd_str);
      break;

      case NODE_XSTR:             /* u1    (%x{ls}) */
      case NODE_STR:              /* u1 */
      case NODE_END_DATA:
        array_push(state, current, string_newfrombstr(state, node->nd_str));
        bdestroy(node->nd_str);
        break;

      case NODE_REGEX:
      case NODE_MATCH:
        array_push(state, current, string_newfrombstr(state, node->nd_str));
        array_push(state, current, I2N(node->nd_cnt));
        bdestroy(node->nd_str);
        break;

      case NODE_LIT:
        array_push(state, current, Q2SYM((uintptr_t)node->nd_lit));
        break;

      case NODE_NEWLINE:
        // newline nodes make Ryan deathly ill; ignore them completely
        array_pop(state, ary);
        node = node->nd_next;
        goto again;

        break;

      case NODE_NTH_REF:          /* u2 u3 ($1) - u3 is local_cnt('~') ignorable? */
        array_push(state, current, I2N(node->nd_nth));
        break;

      case NODE_BACK_REF:         /* u2 u3 ($& etc) */
        {
          char str[2];
          str[0] = node->nd_nth;
          str[1] = 0;
          array_push(state, current, SYMBOL(str));
        }
        break;

      case NODE_BLOCK_ARG:        /* u1 u3 (def x(&b) */
        array_push(state, current, Q2SYM(node->u1.id));
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
      case NODE_FILE:
        break;

      case NODE_SPLAT:
      case NODE_TO_ARY:
      case NODE_SVALUE:             /* a = b, c */
        add_to_parse_tree(current, node->nd_head, locals);
        break;

      case NODE_ATTRASGN:           /* literal.meth = y u1 u2 u3 */
        /* node id node */
        if (node->nd_1st == RNODE(1)) {
          add_to_parse_tree(current, NEW_SELF(), locals);
        } else {
          add_to_parse_tree(current, node->nd_1st, locals);
        }
        array_push(state, current, Q2SYM2(node->u2.id, "="));
        add_to_parse_tree(current, node->nd_3rd, locals);
        break;

      case NODE_EVSTR:
        add_to_parse_tree(current, node->nd_2nd, locals);
        break;

      case NODE_NEGATE:
        add_to_parse_tree(current, node->nd_head, locals);
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
      default:
        printf("Unhandled node #%d type '%s'", nd_type(node),
               get_node_type_string((enum node_type)nd_type(node)));
        if (RNODE(node)->u1.node != NULL) printf("unhandled u1 value");
        if (RNODE(node)->u2.node != NULL) printf("unhandled u2 value");
        if (RNODE(node)->u3.node != NULL) printf("unhandled u3 value");
        if (0) fprintf(stderr, "u1 = %p u2 = %p u3 = %p\n", node->nd_1st,
                       node->nd_2nd, node->nd_3rd);
        array_push(state, current, I2N(-99));
        array_push(state, current, I2N(nd_type(node)));
        break;
      }
    }

  };
};

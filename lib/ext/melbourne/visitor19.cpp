#include <sstream>

#include "melbourne.hpp"
#include "parser_state19.hpp"
#include "visitor19.hpp"
#include "symbols.hpp"

namespace melbourne {
  namespace grammar19 {

  void create_error(rb_parser_state *parser_state, char *msg) {
    VALUE err_msg;

    // Cleanup one of the common and ugly syntax errors.
    if(std::string("syntax error, unexpected $end, expecting kEND") ==
        std::string(msg)) {
      if(start_lines->size() > 0) {
        StartPosition& pos = start_lines->back();

        std::ostringstream ss;
        ss << "missing 'end' for '"
           << pos.kind
           << "' started on line "
           << pos.line;
        err_msg = rb_str_new2(ss.str().c_str());

      } else {
        err_msg = rb_str_new2("missing 'end' for unknown start");
      }
    } else {
      err_msg = 0;

      std::string pmsg(msg);
      std::string comma(", ");

      size_t one = pmsg.find(comma);
      if(one != std::string::npos) {
        size_t two = pmsg.find(comma, one+1);
        if(two != std::string::npos) {
          std::string sub = pmsg.substr(two+2);

          err_msg = rb_str_new2(sub.c_str());
        }
      }

      if(!err_msg) err_msg = rb_str_new2(msg);
    }

    int col = (int)(lex_p - lex_pbeg);

    rb_funcall(processor,
               rb_intern("process_parse_error"),4,
               err_msg,
               INT2FIX(col),
               INT2FIX(sourceline),
               lex_lastline);

    parse_error = true;
  }

#define nd_3rd    u3.node

  VALUE process_parse_tree(rb_parser_state*, VALUE, NODE*, ID*);

  static VALUE process_dynamic(rb_parser_state *parser_state,
      VALUE ptp, NODE *node, ID *locals)
  {
    VALUE array = rb_ary_new();

    while(node) {
      if (node->nd_head) {
        rb_ary_push(array, process_parse_tree(parser_state, ptp, node->nd_head, locals));
      }
      node = node->nd_next;
    }

    return array;
  }

  static VALUE process_iter(rb_parser_state *parser_state,
      VALUE ptp, NODE *node, ID *locals, int *level, ID method, VALUE line)
  {
    VALUE iter, body, args;

    iter = process_parse_tree(parser_state, ptp, node->nd_iter, locals);
    (*level)++;
    if (node->nd_var != (NODE *)1
        && node->nd_var != (NODE *)2
        && node->nd_var != NULL) {
      args = process_parse_tree(parser_state, ptp, node->nd_var, locals);
    } else {
      if (node->nd_var == NULL) {
        // e.g. proc {}
        args = Qnil;
      } else {
        // e.g. proc {||}
        args = INT2FIX(0);
      }
    }
    (*level)--;
    body = process_parse_tree(parser_state, ptp, node->nd_body, locals);
    return rb_funcall(ptp, method, 4, line, iter, args, body);
  }

  /* Visits all the nodes in the parse tree.
   * Adapted from ParseTree by Ryan Davis and Eric Hodel.
   */
  VALUE process_parse_tree(rb_parser_state *parser_state,
      VALUE ptp, NODE *n, ID *locals)
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
      return rb_funcall(ptp, rb_intern("process_dangling_node"), 0);
    }

    // fprintf(stderr, "%s\n", get_node_type_string((enum node_type)nd_type(node)));

    switch(nd_type(node)) {

    case NODE_BLOCK: {
      VALUE array = rb_ary_new();

      while (node) {
        rb_ary_push(array, process_parse_tree(parser_state, ptp, node->nd_head, locals));
        node = node->nd_next;
      }
      tree = rb_funcall(ptp, rb_sBlock, 2, line, array);
      break;
    }
    case NODE_DEFINED: {
      VALUE expr = process_parse_tree(parser_state, ptp, node->nd_head, locals);
      tree = rb_funcall(ptp, rb_sDefined, 2, line, expr);
      break;
    }
    case NODE_COLON2: {
      VALUE container = process_parse_tree(parser_state, ptp, node->nd_head, locals);
      tree = rb_funcall(ptp, rb_sColon2, 3, line,
          container, ID2SYM(node->nd_mid));
      break;
    }
    case NODE_MATCH2: {
      VALUE pattern = process_parse_tree(parser_state, ptp, node->nd_recv, locals);
      VALUE value = process_parse_tree(parser_state, ptp, node->nd_value, locals);
      tree = rb_funcall(ptp, rb_sMatch2, 3, line, pattern, value);
      break;
    }
    case NODE_MATCH3: {
      VALUE pattern = process_parse_tree(parser_state, ptp, node->nd_recv, locals);
      VALUE value = process_parse_tree(parser_state, ptp, node->nd_value, locals);
      tree = rb_funcall(ptp, rb_sMatch3, 3, line, pattern, value);
      break;
    }
    case NODE_BEGIN: {
      VALUE body = process_parse_tree(parser_state, ptp, node->nd_body, locals);
      tree = rb_funcall(ptp, rb_sBegin, 2, line, body);
      break;
    }
    case NODE_IF: {
      VALUE cond, body = Qnil, else_body = Qnil;

      cond = process_parse_tree(parser_state, ptp, node->nd_cond, locals);
      if (node->nd_body) {
        body = process_parse_tree(parser_state, ptp, node->nd_body, locals);
      }
      if (node->nd_else) {
        else_body = process_parse_tree(parser_state, ptp, node->nd_else, locals);
      }
      tree = rb_funcall(ptp, rb_sIf, 4, line, cond, body, else_body);
      break;
    }
    case NODE_CASE: {
      VALUE expr = Qnil, els = Qnil;
      NODE* else_node = 0;

      case_level++;
      if(node->nd_head) {
        expr = process_parse_tree(parser_state, ptp, node->nd_head, locals); /* expr */
      }

      VALUE whens = rb_ary_new();
      node = node->nd_body;
      while(node) {
        if (nd_type(node) == NODE_WHEN) {                 /* when */
          rb_ary_push(whens, process_parse_tree(parser_state, ptp, node, locals));
          node = node->nd_next;
        } else {
          else_node = node;
          break;                                          /* else */
        }
      }

      case_level--;

      // Be sure to decrease the case_level before processing the else.
      // See http://github.com/rubinius/rubinius/issues#issue/240 for an example of
      // why.
      if(else_node) {
        els = process_parse_tree(parser_state, ptp, else_node, locals);
      }

      tree = rb_funcall(ptp, rb_sCase, 4, line, expr, whens, els);
      break;
    }
    case NODE_WHEN: {
      VALUE body = Qnil;

      when_level++;
      /* when without case, ie, no expr in case */
      if(!inside_case_args && case_level < when_level) {
        if(when_level > 0) when_level--;
        node = NEW_CASE(0, node);
        goto again;
      }
      inside_case_args++;
      VALUE args = process_parse_tree(parser_state, ptp, node->nd_head, locals); /* args */
      inside_case_args--;

      if(node->nd_body) {
        body = process_parse_tree(parser_state, ptp, node->nd_body, locals); /* body */
      }
      if(when_level > 0) when_level--;

      tree = rb_funcall(ptp, rb_sWhen, 3, line, args, body);
      break;
    }
    case NODE_WHILE: {
      VALUE cond, body = Qnil, post_cond;

      cond = process_parse_tree(parser_state, ptp,  node->nd_cond, locals);
      if(node->nd_body) {
        body = process_parse_tree(parser_state, ptp,  node->nd_body, locals);
      }
      post_cond = node->nd_3rd == 0 ? Qfalse : Qtrue;
      tree = rb_funcall(ptp, rb_sWhile, 4, line, cond, body, post_cond);
      break;
    }
    case NODE_UNTIL: {
      VALUE cond, body = Qnil, post_cond;

      cond = process_parse_tree(parser_state, ptp,  node->nd_cond, locals);
      if(node->nd_body) {
        body = process_parse_tree(parser_state, ptp,  node->nd_body, locals);
      }
      post_cond = node->nd_3rd == 0 ? Qfalse : Qtrue;
      tree = rb_funcall(ptp, rb_sUntil, 4, line, cond, body, post_cond);
      break;
    }
    case NODE_BLOCK_PASS: {
      VALUE args = Qnil;

      if(node->nd_1st) {
        args = process_parse_tree(parser_state, ptp, node->nd_1st, locals);
      }

      VALUE body = process_parse_tree(parser_state, ptp, node->nd_body, locals);

      tree = rb_funcall(ptp, rb_sBlockPass, 3, line, args, body);
      break;
    }
    case NODE_FOR:
      tree = process_iter(parser_state, ptp, node, locals, &masgn_level, rb_sFor, line);
      break;

    case NODE_ITER: {
      VALUE iter = process_parse_tree(parser_state, ptp, node->nd_iter, locals);
      VALUE body = process_parse_tree(parser_state, ptp, node->nd_body, locals);
      tree = rb_funcall(ptp, rb_sIter, 3, line, iter, body);
      break;
    }
    case NODE_LAMBDA: {
      VALUE scope = process_parse_tree(parser_state, ptp, node->nd_body, locals);
      tree = rb_funcall(ptp, rb_sLambda, 2, line, scope);
      break;
    }
    case NODE_BREAK: {
      VALUE expr = Qnil;

      if (node->nd_stts) {
        expr = process_parse_tree(parser_state, ptp, node->nd_stts, locals);
      }
      tree = rb_funcall(ptp, rb_sBreak, 2, line, expr);
      break;
    }
    case NODE_NEXT: {
      VALUE expr = Qnil;

      if (node->nd_stts) {
        expr = process_parse_tree(parser_state, ptp, node->nd_stts, locals);
      }
      tree = rb_funcall(ptp, rb_sNext, 2, line, expr);
      break;
    }
    case NODE_YIELD: {
      VALUE expr = Qnil;

      if (node->nd_stts) {
        expr = process_parse_tree(parser_state, ptp, node->nd_stts, locals);
      }
      tree = rb_funcall(ptp, rb_sYield, 3, line, expr, node->u3.value);
      break;
    }
    case NODE_RESCUE: {
      VALUE body = process_parse_tree(parser_state, ptp, node->nd_1st, locals);
      VALUE resc = process_parse_tree(parser_state, ptp, node->nd_2nd, locals);
      VALUE els = process_parse_tree(parser_state, ptp, node->nd_3rd, locals);
      tree = rb_funcall(ptp, rb_sRescue, 4, line, body, resc, els);
      break;
    }
    case NODE_RESBODY: {
      /* rescue body:
       * begin stmt rescue exception => var; stmt; [rescue e2 => v2; s2;]* end
       * stmt rescue stmt
       * a = b rescue c
       */

      VALUE conditions = Qnil;

      if(node->nd_3rd) {
        conditions = process_parse_tree(parser_state, ptp, node->nd_3rd, locals);
      }
      VALUE body = process_parse_tree(parser_state, ptp, node->nd_2nd, locals);
      VALUE next = process_parse_tree(parser_state, ptp, node->nd_1st, locals);
      tree = rb_funcall(ptp, rb_sResbody, 4, line, conditions, body, next);
      break;
    }
    case NODE_ENSURE: {
      VALUE head, ensr = Qnil;

      head = process_parse_tree(parser_state, ptp, node->nd_head, locals);
      if (node->nd_ensr) {
        ensr = process_parse_tree(parser_state, ptp, node->nd_ensr, locals);
      }
      tree = rb_funcall(ptp, rb_sEnsure, 3, line, head, ensr);
      break;
    }
    case NODE_AND: {
      VALUE left = process_parse_tree(parser_state, ptp, node->nd_1st, locals);
      VALUE right = process_parse_tree(parser_state, ptp, node->nd_2nd, locals);
      tree = rb_funcall(ptp, rb_sAnd, 3, line, left, right);
      break;
    }
    case NODE_OR: {
      VALUE left = process_parse_tree(parser_state, ptp, node->nd_1st, locals);
      VALUE right = process_parse_tree(parser_state, ptp, node->nd_2nd, locals);
      tree = rb_funcall(ptp, rb_sOr, 3, line, left, right);
      break;
    }
    case NODE_DOT2: {
      VALUE start = process_parse_tree(parser_state, ptp, node->nd_beg, locals);
      VALUE finish = process_parse_tree(parser_state, ptp, node->nd_end, locals);
      tree = rb_funcall(ptp, rb_sDot2, 3, line, start, finish);
      break;
    }
    case NODE_DOT3: {
      VALUE start = process_parse_tree(parser_state, ptp, node->nd_beg, locals);
      VALUE finish = process_parse_tree(parser_state, ptp, node->nd_end, locals);
      tree = rb_funcall(ptp, rb_sDot3, 3, line, start, finish);
      break;
    }
    case NODE_FLIP2: {
      VALUE start = process_parse_tree(parser_state, ptp, node->nd_beg, locals);
      VALUE finish = process_parse_tree(parser_state, ptp, node->nd_end, locals);
      tree = rb_funcall(ptp, rb_sFlip2, 3, line, start, finish);
      break;
    }
    case NODE_FLIP3: {
      VALUE start = process_parse_tree(parser_state, ptp, node->nd_beg, locals);
      VALUE finish = process_parse_tree(parser_state, ptp, node->nd_end, locals);
      tree = rb_funcall(ptp, rb_sFlip3, 3, line, start, finish);
      break;
    }
    case NODE_RETURN: {
      VALUE expr = Qnil;

      if (node->nd_stts) {
        expr = process_parse_tree(parser_state, ptp, node->nd_stts, locals);
      }
      tree = rb_funcall(ptp, rb_sReturn, 2, line, expr);
      break;
    }
    case NODE_ARGSCAT: {
      VALUE array = process_parse_tree(parser_state, ptp, node->nd_head, locals);
      VALUE rest = process_parse_tree(parser_state, ptp, node->nd_body, locals);
      tree = rb_funcall(ptp, rb_sArgsCat, 3, line, array, rest);
      break;
    }
    case NODE_ARGSPUSH: {
      VALUE head = process_parse_tree(parser_state, ptp, node->nd_head, locals);
      VALUE body = process_parse_tree(parser_state, ptp, node->nd_body, locals);
      tree = rb_funcall(ptp, rb_sArgsPush, 3, line, head, body);
      break;
    }
    case NODE_CALL: {
      VALUE args = Qnil;

      VALUE recv = process_parse_tree(parser_state, ptp, node->nd_recv, locals);
      if (node->nd_args) {
        args = process_parse_tree(parser_state, ptp, node->nd_args, locals);
      }
      tree = rb_funcall(ptp, rb_sCall, 4, line, recv, ID2SYM(node->nd_mid), args);
      break;
    }
    case NODE_FCALL: {
      VALUE args = Qnil;

      if (node->nd_args) {
        args = process_parse_tree(parser_state, ptp, node->nd_args, locals);
      }
      tree = rb_funcall(ptp, rb_sFCall, 3, line, ID2SYM(node->nd_mid), args);
      break;
    }
    case NODE_VCALL:
      tree = rb_funcall(ptp, rb_sVCall, 2, line, ID2SYM(node->nd_mid));
      break;

    case NODE_SUPER: {
      VALUE args = process_parse_tree(parser_state, ptp, node->nd_args, locals);
      tree = rb_funcall(ptp, rb_sSuper, 2, line, args);
      break;
    }
    case NODE_SCOPE: {
      VALUE args = Qnil;
      VALUE bv_locals = Qnil;

      if(node->nd_args) {
        if(nd_type(node->nd_args) == NODE_ARGS_AUX) {
          args = process_parse_tree(parser_state, ptp, node->nd_args->nd_1st, node->nd_tbl);
          bv_locals = process_parse_tree(parser_state, ptp, node->nd_args->nd_2nd, node->nd_tbl);
        } else {
          args = process_parse_tree(parser_state, ptp, node->nd_args, node->nd_tbl);
        }
      }
      VALUE body = process_parse_tree(parser_state, ptp, node->nd_body, node->nd_tbl);
      tree = rb_funcall(ptp, rb_sScope, 4, line, args, body, bv_locals);
      break;
    }
    case NODE_OP_ASGN1: {
      VALUE op;

      VALUE recv = process_parse_tree(parser_state, ptp, node->nd_recv, locals);
      VALUE args = process_parse_tree(parser_state, ptp, node->nd_args->nd_2nd, locals);
      switch(node->nd_mid) {
        case 0:
          op = ID2SYM(parser_intern("or"));
          break;
        case 1:
          op = ID2SYM(parser_intern("and"));
          break;
        default:
          op = ID2SYM(node->nd_mid);
      }
      VALUE value = process_parse_tree(parser_state, ptp, node->nd_args->nd_head, locals);
      tree = rb_funcall(ptp, rb_sOpAsgn1, 5, line, recv, value, op, args);
      break;
    }
    case NODE_OP_ASGN2: {
      VALUE op;

      VALUE recv = process_parse_tree(parser_state, ptp, node->nd_recv, locals);
      switch(node->nd_next->nd_mid) {
        case 0:
          op = ID2SYM(parser_intern("or"));
          break;
        case 1:
          op = ID2SYM(parser_intern("and"));
          break;
        default:
          op = ID2SYM(node->nd_next->nd_mid);
      }
      VALUE value = process_parse_tree(parser_state, ptp, node->nd_value, locals);
      tree = rb_funcall(ptp, rb_sOpAsgn2, 5, line,
          recv, ID2SYM(node->nd_next->nd_aid), op, value);
      break;
    }
    case NODE_OP_ASGN_AND: {
      VALUE var = process_parse_tree(parser_state, ptp, node->nd_head, locals);
      VALUE value = process_parse_tree(parser_state, ptp, node->nd_value, locals);
      tree = rb_funcall(ptp, rb_sOpAsgnAnd, 3, line, var, value);
      break;
    }
    case NODE_OP_ASGN_OR: {
      VALUE var = process_parse_tree(parser_state, ptp, node->nd_head, locals);
      VALUE value = process_parse_tree(parser_state, ptp, node->nd_value, locals);
      tree = rb_funcall(ptp, rb_sOpAsgnOr, 3, line, var, value);
      break;
    }
    case NODE_MASGN: {
      VALUE args = Qnil;

      masgn_level++;
      VALUE head = process_parse_tree(parser_state, ptp, node->nd_head, locals);
      if (node->nd_args) {
        if(node->nd_args != (NODE *)-1) {
          args = process_parse_tree(parser_state, ptp, node->nd_args, locals);
        } else {
          args = Qtrue;
        }
      }
      VALUE value = process_parse_tree(parser_state, ptp, node->nd_value, locals);
      tree = rb_funcall(ptp, rb_sMAsgn, 4, line, head, value, args);
      masgn_level--;
      break;
    }
    case NODE_LASGN: {
      VALUE expr = process_parse_tree(parser_state, ptp, node->nd_value, locals);
      tree = rb_funcall(ptp, rb_sLAsgn, 3, line, ID2SYM(node->nd_vid), expr);
      break;
    }
    case NODE_IASGN: {
      VALUE expr = process_parse_tree(parser_state, ptp, node->nd_value, locals);
      tree = rb_funcall(ptp, rb_sIAsgn, 3, line, ID2SYM(node->nd_vid), expr);
      break;
    }
    case NODE_CVASGN: {
      VALUE expr = process_parse_tree(parser_state, ptp, node->nd_value, locals);
      tree = rb_funcall(ptp, rb_sCVAsgn, 3, line, ID2SYM(node->nd_vid), expr);
      break;
    }
    case NODE_CVDECL: {
      VALUE expr = process_parse_tree(parser_state, ptp, node->nd_value, locals);
      tree = rb_funcall(ptp, rb_sCVDecl, 3, line, ID2SYM(node->nd_vid), expr);
      break;
    }
    case NODE_GASGN: {
      VALUE expr = process_parse_tree(parser_state, ptp, node->nd_value, locals);
      tree = rb_funcall(ptp, rb_sGAsgn, 3, line, ID2SYM(node->nd_vid), expr);
      break;
    }
    case NODE_CDECL: {
      VALUE expr;

      if(node->nd_vid) {
        expr = ID2SYM(node->nd_vid);
      } else {
        expr = process_parse_tree(parser_state, ptp, node->nd_else, locals);
      }
      VALUE value = process_parse_tree(parser_state, ptp, node->nd_value, locals);
      tree = rb_funcall(ptp, rb_sCDecl, 3, line, expr, value);
      break;
    }
    case NODE_VALIAS:           /* u1 u2 (alias $global $global2) */
      tree = rb_funcall(ptp, rb_sVAlias, 3, line,
          ID2SYM(node->u1.id), ID2SYM(node->u2.id));
      break;

    case NODE_ALIAS: {          /* u1 u2 (alias :blah :blah2) */
      VALUE to = process_parse_tree(parser_state, ptp, node->u1.node, locals);
      VALUE from = process_parse_tree(parser_state, ptp, node->u2.node, locals);
      tree = rb_funcall(ptp, rb_sAlias, 3, line, to, from);
      break;
    }
    case NODE_UNDEF: {          /* u2    (undef instvar) */
      VALUE name = process_parse_tree(parser_state, ptp, node->u2.node, locals);
      tree = rb_funcall(ptp, rb_sUndef, 2, line, name);
      break;
    }
    case NODE_COLON3:           /* u2    (::OUTER_CONST) */
      tree = rb_funcall(ptp, rb_sColon3, 2, line, ID2SYM(node->u2.id));
      break;

    case NODE_HASH: {
      VALUE array = rb_ary_new();

      node = node->nd_head;
      while (node) {
        rb_ary_push(array, process_parse_tree(parser_state, ptp, node->nd_head, locals));
        if (!(node = node->nd_next)) {
          // @todo: properly process the parse error
          printf("odd number list for Hash");
          abort();
        }
        rb_ary_push(array, process_parse_tree(parser_state, ptp, node->nd_head, locals));
        node = node->nd_next;
      }
      tree = rb_funcall(ptp, rb_sHash, 2, line, array);
      break;
    }
    case NODE_ARRAY: {
      VALUE array = rb_ary_new();

      while (node) {
        rb_ary_push(array, process_parse_tree(parser_state, ptp, node->nd_head, locals));
        node = node->nd_next;
      }
      tree = rb_funcall(ptp, rb_sArray, 2, line, array);
      break;
    }
    case NODE_DSTR: {
      VALUE array = process_dynamic(parser_state, ptp, node->nd_next, locals);
      tree = rb_funcall(ptp, rb_sDStr, 3, line, node->nd_lit, array);
      break;
    }
    case NODE_DSYM: {
      VALUE array = process_dynamic(parser_state, ptp, node->nd_next, locals);
      tree = rb_funcall(ptp, rb_sDSym, 3, line, node->nd_lit, array);
      break;
    }
    case NODE_DXSTR: {
      VALUE array = process_dynamic(parser_state, ptp, node->nd_next, locals);
      tree = rb_funcall(ptp, rb_sDXStr, 3, line, node->nd_lit, array);
      break;
    }
    case NODE_DREGX: {
      VALUE flags = Qnil;

      VALUE array = process_dynamic(parser_state, ptp, node->nd_next, locals);
      if (node->nd_cflag) flags = INT2FIX(node->nd_cflag);
      tree = rb_funcall(ptp, rb_sDRegx, 4, line, node->nd_lit, array, flags);
      break;
    }
    case NODE_DREGX_ONCE: {
      VALUE flags = Qnil;

      VALUE array = process_dynamic(parser_state, ptp, node->nd_next, locals);
      if (node->nd_cflag) flags = INT2FIX(node->nd_cflag);
      tree = rb_funcall(ptp, rb_sDRegxOnce, 4, line, node->nd_lit, array, flags);
      break;
    }
    case NODE_DEFN: {
      VALUE body = Qnil;

      if (node->nd_defn) {
        body = process_parse_tree(parser_state, ptp, node->nd_defn, locals);
      }
      tree = rb_funcall(ptp, rb_sDefn, 3, line, ID2SYM(node->nd_mid), body);
      break;
    }
    case NODE_DEFS: {
      VALUE recv = Qnil, body = Qnil;

      if (node->nd_defn) {
        recv = process_parse_tree(parser_state, ptp, node->nd_recv, locals);
        body = process_parse_tree(parser_state, ptp, node->nd_defn, locals);
      }
      tree = rb_funcall(ptp, rb_sDefs, 4, line, recv, ID2SYM(node->nd_mid), body);
      break;
    }
    case NODE_CLASS: {
      VALUE name, super = Qnil;

      if (nd_type(node->nd_cpath) == NODE_COLON2 && !node->nd_cpath->nd_vid) {
        name = ID2SYM((ID)node->nd_cpath->nd_mid);
      } else {
        name = process_parse_tree(parser_state, ptp, node->nd_cpath, locals);
      }
      if(node->nd_super) {
        super = process_parse_tree(parser_state, ptp, node->nd_super, locals);
      }
      VALUE body = process_parse_tree(parser_state, ptp, node->nd_body, locals);
      tree = rb_funcall(ptp, rb_sClass, 4, line, name, super, body);
      break;
    }
    case NODE_MODULE: {
      VALUE name;

      if (nd_type(node->nd_cpath) == NODE_COLON2 && !node->nd_cpath->nd_vid) {
        name = ID2SYM((ID)node->nd_cpath->nd_mid);
      } else {
        name = process_parse_tree(parser_state, ptp, node->nd_cpath, locals);
      }
      VALUE body = process_parse_tree(parser_state, ptp, node->nd_body, locals);
      tree = rb_funcall(ptp, rb_sModule, 3, line, name, body);
      break;
    }
    case NODE_SCLASS: {
      VALUE recv = process_parse_tree(parser_state, ptp, node->nd_recv, locals);
      VALUE body = process_parse_tree(parser_state, ptp, node->nd_body, locals);
      tree = rb_funcall(ptp, rb_sSClass, 3, line, recv, body);
      break;
    }
    case NODE_OPT_ARG: {
      VALUE args = rb_ary_new();

      do {
        rb_ary_push(args, process_parse_tree(parser_state, ptp, node->nd_body, locals));
        node = node->nd_next;
      } while(node);

      tree = rb_funcall(ptp, rb_sOptArg, 2, line, args);
      break;
    }
    case NODE_ARGS: {
      VALUE args = Qnil;
      VALUE opts = Qnil;
      VALUE splat = Qnil;
      VALUE post = Qnil;
      VALUE block = Qnil;

      int total_args = 0;
      ID* args_ary = 0;

      NODE* aux = node->nd_args;
      NODE* post_args = aux->nd_next;
      NODE* masgn = 0;
      NODE* next = 0;

      if(post_args && post_args->nd_next && nd_type(post_args->nd_next) == NODE_AND) {
        if(post_args->nd_next->nd_head) {
          if (nd_type(post_args->nd_next->nd_head) == NODE_BLOCK) {
            masgn = post_args->nd_next->nd_head->nd_head;
            next = post_args->nd_next->nd_head->nd_next;
          } else {
            masgn = post_args->nd_next->nd_head;
            next = masgn->nd_next;
            // -1 comes from: mlhs_head tSTAR
            if(masgn->nd_cnt == -1) next = 0;
          }
        } else {
          masgn = post_args->nd_next->nd_2nd;
          if(masgn) {
            next = masgn->nd_next;
            if (nd_type(masgn) == NODE_BLOCK) {
              masgn = masgn->nd_head;
            }
          }
        }
      }

      if(node->nd_argc > 0) {
        total_args = (int)locals[0];
        args_ary = locals + 1;

        args = rb_ary_new();
        for(int i = 0; i < node->nd_argc && i < total_args; i++) {
          VALUE arg = Qnil;

          if(!INTERNAL_ID_P(args_ary[i])) {
            arg = ID2SYM(args_ary[i]);
          } else if(masgn) {
            arg = process_parse_tree(parser_state, ptp, masgn, locals);
            if(next && nd_type(next) == NODE_BLOCK) {
              masgn = next->nd_head;
              next = next->nd_next;
            } else {
              masgn = next;
              if(masgn) next = masgn->nd_next;
            }
          }

          rb_ary_push(args, arg);
        }
      }

      if(node->nd_opt) {
        opts = process_parse_tree(parser_state, ptp, node->nd_opt, locals);
      }

      if(INTERNAL_ID_P(aux->nd_rest)) {
        splat = Qtrue;
      } else if(aux->nd_rest) {
        if(aux->nd_rest == 1) {
          // m { |a,| ... }
          splat = Qfalse;
        } else {
          splat = ID2SYM(aux->nd_rest);
        }
      }
      if(aux->nd_mid) block = ID2SYM(aux->nd_mid);

      if(post_args && post_args->nd_pid) {
        total_args = (int)locals[0];
        args_ary = locals + 1;

        int start;
        for(start = 0; start < total_args; start++) {
          if(args_ary[start] == post_args->nd_pid)
            break;
        }

        post = rb_ary_new();
        for(int i = 0; i < post_args->nd_argc && start + i < total_args; i++) {
          VALUE arg = Qnil;

          if(!INTERNAL_ID_P(args_ary[start + i])) {
            arg = ID2SYM(args_ary[start + i]);
          } else if(masgn) {
            arg = process_parse_tree(parser_state, ptp, masgn, locals);
            if(next && nd_type(next) == NODE_BLOCK) {
              masgn = next->nd_head;
              next = next->nd_next;
            } else {
              masgn = next;
              if(masgn) next = masgn->nd_next;
            }
          }
          rb_ary_push(post, arg);
        }
      }

      tree = rb_funcall(ptp, rb_sArgs, 6, line, args, opts, splat, post, block);
      break;
    }
    case NODE_LVAR:
      if(!INTERNAL_ID_P(node->nd_vid)) {
        tree = rb_funcall(ptp, rb_sLVar, 2, line, ID2SYM(node->nd_vid));
      }
      break;

    case NODE_IVAR:
      tree = rb_funcall(ptp, rb_sIVar, 2, line, ID2SYM(node->nd_vid));
      break;

    case NODE_CVAR:
      tree = rb_funcall(ptp, rb_sCVar, 2, line, ID2SYM(node->nd_vid));
      break;

    case NODE_GVAR:
      tree = rb_funcall(ptp, rb_sGVar, 2, line, ID2SYM(node->nd_vid));
      break;

    case NODE_CONST:
      tree = rb_funcall(ptp, rb_sConst, 2, line, ID2SYM(node->nd_vid));
      break;

    case NODE_XSTR:             /* u1    (%x{ls}) */
      tree = rb_funcall(ptp, rb_sXStr, 2, line, node->nd_lit);
      break;

    case NODE_STR:              /* u1 */
      tree = rb_funcall(ptp, rb_sStr, 2, line, node->nd_lit);
      break;

    case NODE_REGEX:
      tree = rb_funcall(ptp, rb_sRegex, 3, line, node->nd_lit,
                        INT2FIX(node->nd_cnt));
      break;

    case NODE_MATCH:
      tree = rb_funcall(ptp, rb_sMatch, 3, line, node->nd_lit,
                        INT2FIX(node->nd_cnt));
      break;

    case NODE_LIT:
      tree = rb_funcall(ptp, rb_sLit, 2, line, node->nd_lit);
      break;

    case NODE_VALUES: {
      VALUE first = process_parse_tree(parser_state, ptp, node->nd_head, locals);
      VALUE rest = process_parse_tree(parser_state, ptp, node->nd_args, locals);
      tree = rb_funcall(ptp, rb_sValues, 3, line, first, rest);
      break;
    }

    case NODE_NUMBER:
      tree = rb_funcall(ptp, rb_sNumber, 2, line, node->nd_lit);
      break;

    case NODE_FLOAT:
      tree = rb_funcall(ptp, rb_sFloat, 2, line, node->nd_lit);
      break;

    case NODE_NTH_REF:          /* u2 u3 ($1) - u3 is local_cnt('~') ignorable? */
      tree = rb_funcall(ptp, rb_sNthRef, 2, line, INT2FIX(node->nd_nth));
      break;

    case NODE_BACK_REF: {        /* u2 u3 ($& etc) */
      char str[2];
      str[0] = node->nd_nth;
      str[1] = 0;
      tree = rb_funcall(ptp, rb_sBackRef, 2, line, ID2SYM(parser_intern(str)));
      break;
    }

    case NODE_BLOCK_ARG:        /* u1 u3 (def x(&b) */
      tree = rb_funcall(ptp, rb_sBlockArg, 2, line, ID2SYM(node->u1.id));
      break;

    case NODE_RETRY:
      tree = rb_funcall(ptp, rb_sRetry, 1, line);
      break;

    case NODE_FALSE:
      tree = rb_funcall(ptp, rb_sFalse, 1, line);
      break;

    case NODE_NIL:
      tree = rb_funcall(ptp, rb_sNil, 1, line);
      break;

    case NODE_SELF:
      tree = rb_funcall(ptp, rb_sSelf, 1, line);
      break;

    case NODE_TRUE:
      tree = rb_funcall(ptp, rb_sTrue, 1, line);
      break;

    case NODE_ZARRAY:
      tree = rb_funcall(ptp, rb_sZArray, 1, line);
      break;

    case NODE_ZSUPER:
      tree = rb_funcall(ptp, rb_sZSuper, 1, line);
      break;

    case NODE_REDO:
      tree = rb_funcall(ptp, rb_sRedo, 1, line);
      break;

    case NODE_FILE:
      tree = rb_funcall(ptp, rb_sFile, 1, line);
      break;

    case NODE_ENCODING:
      tree = rb_funcall(ptp, rb_sEncoding, 2, line, node->nd_lit);
      break;

    case NODE_SPLAT: {
      VALUE expr = process_parse_tree(parser_state, ptp, node->nd_head, locals);
      tree = rb_funcall(ptp, rb_sSplat, 2, line, expr);
      break;
    }
    case NODE_TO_ARY: {
      VALUE expr = process_parse_tree(parser_state, ptp, node->nd_head, locals);
      tree = rb_funcall(ptp, rb_sToAry, 2, line, expr);
      break;
    }
    case NODE_ATTRASGN: {         /* literal.meth = y u1 u2 u3 */
      VALUE recv;

      /* node id node */
      if (node->nd_1st == RNODE(1)) {
        recv = process_parse_tree(parser_state, ptp, NEW_SELF(), locals);
      } else {
        recv = process_parse_tree(parser_state, ptp, node->nd_1st, locals);
      }
      VALUE value = process_parse_tree(parser_state, ptp, node->nd_3rd, locals);
      tree = rb_funcall(ptp, rb_sAttrAsgn, 4, line,
          recv, ID2SYM(node->u2.id), value);
      break;
    }
    case NODE_EVSTR: {
      VALUE value = process_parse_tree(parser_state, ptp, node->nd_2nd, locals);
      tree = rb_funcall(ptp, rb_sEvStr, 2, line, value);
      break;
    }
    case NODE_PREEXE: {           /* BEGIN { ... } */
      VALUE scope = process_parse_tree(parser_state, ptp, node->nd_2nd, locals);
      tree = rb_funcall(ptp, rb_sPreExe, 2, line, scope);
      break;
    }
    case NODE_POSTEXE: {          /* END { ... } */
      VALUE scope = process_parse_tree(parser_state, ptp, node->nd_2nd, locals);
      tree = rb_funcall(ptp, rb_sPostExe, 2, line, scope);
      break;
    }
    case NODE_POSTARG: {
      VALUE into = Qnil;
      if(node->nd_1st != RNODE(-1)) {
        into = process_parse_tree(parser_state, ptp, node->nd_1st, locals);
      }

      VALUE rest = process_parse_tree(parser_state, ptp, node->nd_2nd, locals);

      tree = rb_funcall(ptp, rb_sPostArg, 3, line, into, rest);
      break;
    }
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
  }; // namespace grammar19
};

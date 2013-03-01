#ifndef MEL_PARSER_STATE19_HPP
#define MEL_PARSER_STATE19_HPP

#include "node19.hpp"
#include "node_types19.hpp"
#include "var_table19.hpp"
#include "encoding_compat.hpp"

#include "bstrlib.h"

#include <vector>
#include <list>

namespace melbourne {
  namespace grammar19 {

    enum lex_state_e {
      EXPR_BEG,       /* ignore newline, +/- is a sign. */
      EXPR_END,       /* newline significant, +/- is an operator. */
      EXPR_ENDARG,    /* ditto, and unbound braces. */
      EXPR_ENDFN,     /* ditto, and unbound braces. */
      EXPR_ARG,       /* newline significant, +/- is an operator. */
      EXPR_CMDARG,    /* newline significant, +/- is an operator. */
      EXPR_MID,       /* newline significant, +/- is an operator. */
      EXPR_FNAME,     /* ignore newline, no reserved words. */
      EXPR_DOT,       /* right after `.' or `::', no reserved words. */
      EXPR_CLASS,     /* immediate after `class', no here document. */
      EXPR_VALUE,     /* like EXPR_BEG but label is disallowed. */
      EXPR_MAX_STATE
    };

typedef VALUE stack_type;

    struct StartPosition {
      int line;
      const char* kind;

      StartPosition(int l, const char* k)
        : line(l)
        , kind(k)
      {}
    };

    typedef struct rb_parser_state {
      int ruby__end__seen;
      int debug_lines;
      int heredoc_end;
      int command_start;
      NODE *lex_strterm;
      int paren_nest;
      int lpar_beg;
      int class_nest;
      int in_single;
      int in_def;
      int compile_for_eval;
      ID cur_mid;
      char *token_buffer;
      int tokidx;
      int toksiz;
      int emit_warnings;
      /* Mirror'ing the 1.8 parser, There are 2 input methods,
         from IO and directly from a string. */

      /* this function reads a line from lex_io and stores it in
       * line_buffer.
       */
      VALUE (*lex_gets)(rb_parser_state*, VALUE);

      /* If this is set, we use the io method. */
      int lex_io;
      char* lex_io_buf;
      ssize_t lex_io_index;
      ssize_t lex_io_total;
      long lex_io_count;

      /* Otherwise, we use this. */
      long lex_gets_ptr;
      VALUE lex_input;
      VALUE lex_lastline;
      VALUE lex_nextline;

      char *lex_pbeg;
      char *lex_p;
      char *lex_pend;
      int lex_str_used;

      enum lex_state_e lex_state;
      int in_defined;
      stack_type cond_stack;
      stack_type cmdarg_stack;

      void *lval; /* the parser's yylval */
      bool eofp;

      int column;
      NODE *top_node;

      struct local_vars* locals_table;

      int ternary_colon;

      void **memory_pools;
      int pool_size, current_pool;
      char *memory_cur;
      char *memory_last_addr;
      int memory_size;

      bool verbose;

      bool parse_error;

      // Reference to the object to call methods on to convert the
      // C parse tree into a Ruby AST.
      VALUE processor;

      // Keep track of any object literals created in the parser.
      VALUE references;

      // Keeps track of lines that 'end' starters are on, to enable
      // better error reporting.
      std::list<StartPosition>* start_lines;

      int line_count;
      bool has_shebang;

      char *sourcefile;
      int sourceline;

      rb_encoding *enc;
      rb_encoding *utf8;
    } rb_parser_state;


#define PARSER_STATE        ((rb_parser_state*)parser_state)
#define PARSER_VAR(var)     (PARSER_STATE->var)

#define end_seen            PARSER_VAR(end_seen)
#define ruby__end__seen     PARSER_VAR(ruby__end__seen)
#define ruby_debug_lines    PARSER_VAR(debug_lines)
#define heredoc_end         PARSER_VAR(heredoc_end)
#define command_start       PARSER_VAR(command_start)
#define lex_strterm         PARSER_VAR(lex_strterm)
#define paren_nest          PARSER_VAR(paren_nest)
#define lpar_beg            PARSER_VAR(lpar_beg)
#define class_nest          PARSER_VAR(class_nest)
#define in_single           PARSER_VAR(in_single)
#define in_def              PARSER_VAR(in_def)
#define compile_for_eval    PARSER_VAR(compile_for_eval)
#define cur_mid             PARSER_VAR(cur_mid)
#define tokenbuf            PARSER_VAR(token_buffer)
#define tokidx              PARSER_VAR(tokidx)
#define toksiz              PARSER_VAR(toksiz)
#define emit_warnings       PARSER_VAR(emit_warnings)
#define lex_gets            PARSER_VAR(lex_gets)
#define line_buffer         PARSER_VAR(line_buffer)
#define line_count          PARSER_VAR(line_count)
#define has_shebang         PARSER_VAR(has_shebang)
#define lex_io              PARSER_VAR(lex_io)
#define lex_io_buf          PARSER_VAR(lex_io_buf)
#define lex_io_index        PARSER_VAR(lex_io_index)
#define lex_io_total        PARSER_VAR(lex_io_total)
#define lex_io_count        PARSER_VAR(lex_io_count)
#define lex_gets_ptr        PARSER_VAR(lex_gets_ptr)
#define lex_input           PARSER_VAR(lex_input)
#define lex_lastline        PARSER_VAR(lex_lastline)
#define lex_nextline        PARSER_VAR(lex_nextline)
#define lex_pbeg            PARSER_VAR(lex_pbeg)
#define lex_p               PARSER_VAR(lex_p)
#define lex_pend            PARSER_VAR(lex_pend)
#define lex_str_used        PARSER_VAR(lex_str_used)
#define lex_state           PARSER_VAR(lex_state)
#define in_defined          PARSER_VAR(in_defined)
#define cond_stack          PARSER_VAR(cond_stack)
#define cmdarg_stack        PARSER_VAR(cmdarg_stack)
#define lval                PARSER_VAR(lval)
#define eofp                PARSER_VAR(eofp)
#define column              PARSER_VAR(column)
#define top_node            PARSER_VAR(top_node)
#define locals_table        PARSER_VAR(locals_table)
#define ternary_colon       PARSER_VAR(ternary_colon)
#define memory_pools        PARSER_VAR(memory_pools)
#define pool_size           PARSER_VAR(pool_size)
#define current_pool        PARSER_VAR(current_pool)
#define memory_cur          PARSER_VAR(memory_cur)
#define memory_last_addr    PARSER_VAR(memory_last_addr)
#define memory_size         PARSER_VAR(memory_size)
#define verbose             PARSER_VAR(verbose)
#define parse_error         PARSER_VAR(parse_error)
#define processor           PARSER_VAR(processor)
#define references          PARSER_VAR(references)
#define start_lines         PARSER_VAR(start_lines)
#define sourcefile          PARSER_VAR(sourcefile)
#define sourceline          PARSER_VAR(sourceline)

#define node_newnode(t, a, b, c)  \
    parser_node_newnode((rb_parser_state*)parser_state, t, a, b, c)
#define node_add_reference(obj)   \
    parser_add_reference((rb_parser_state*)parser_state, obj)

    NODE *parser_node_newnode(rb_parser_state*, enum node_type, VALUE, VALUE, VALUE);
    VALUE parser_add_reference(rb_parser_state* parser_state, VALUE obj);

#undef ID_SCOPE_SHIFT
#undef ID_SCOPE_MASK
#undef ID_LOCAL
#undef ID_INSTANCE
#undef ID_GLOBAL
#undef ID_ATTRSET
#undef ID_CONST
#undef ID_CLASS
#undef ID_JUNK
#undef ID_INTERNAL

    ID parser_intern(const char*);
    ID parser_intern2(const char*, long);
    ID parser_intern3(const char*, long, rb_encoding*);
    ID parser_intern_str(VALUE);
    char* parser_id2name(ID);

#undef ID2SYM

/* ID_SCOPE_SHIFT must be at least 4 because at 3 the values will overlap
 * the values of the tokens, causing the parser to mistake the symbol for
 * '*' with the token tAREF. Hilarity ensues when Fixnum * Fixnum ends up
 * parsed as Fixnum[Fixnum].
 */
#define ID_SCOPE_SHIFT  7
#define ID_SCOPE_MASK   0x0f
#define ID_LOCAL        0x00
#define ID_INSTANCE     0x01
#define ID_GLOBAL       0x03
#define ID_ATTRSET      0x04
#define ID_CONST        0x05
#define ID_CLASS        0x06
#define ID_JUNK         0x07
#define ID_INTERNAL     ID_JUNK

#ifdef RUBINIUS
#undef SYMBOL_FLAG
#define ID2SYM(id)  (VALUE)((long)(id >> ID_SCOPE_SHIFT))
#else
#define ID2SYM(id)  ((VALUE)(((long)(id >> ID_SCOPE_SHIFT))<<8|SYMBOL_FLAG))
#endif

#define INTERNAL_ID_P(a)  ((a & ID_INTERNAL) == ID_INTERNAL)

  };  // namespace grammar19
};  // namespace melbourne

#endif

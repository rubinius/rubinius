#ifndef MEL_INTERNAL_HPP
#define MEL_INTERNAL_HPP

#include <list>

#include "quark.hpp"
#include "bstrlib.h"

#ifdef RBX_GRAMMAR_19
#include "encoding_compat.hpp"
#endif

#include "local_state.hpp"

#include <vector>

namespace melbourne {

#define QUID    quark

  enum lex_state_e {
#ifdef RBX_GRAMMAR_19
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
#else
      EXPR_BEG,       /* ignore newline, +/- is a sign. */
      EXPR_END,       /* newline significant, +/- is a operator. */
      EXPR_ARG,       /* newline significant, +/- is a operator. */
      EXPR_CMDARG,    /* newline significant, +/- is a operator. */
      EXPR_ENDARG,    /* newline significant, +/- is a operator. */
      EXPR_MID,       /* newline significant, +/- is a operator. */
      EXPR_FNAME,     /* ignore newline, no reserved words. */
      EXPR_DOT,       /* right after `.' or `::', no reserved words. */
      EXPR_CLASS,     /* immediate after `class', no here document. */
#endif
  };

#ifdef HAVE_LONG_LONG
    typedef unsigned LONG_LONG stack_type;
#else
    typedef unsigned long stack_type;
#endif

}; // namespace melbourne

#include "node.hpp"

namespace melbourne {

  struct StartPosition {
    int line;
    const char* kind;

    StartPosition(int l, const char* k)
      : line(l)
      , kind(k)
    {}
  };

  typedef struct rb_parser_state {
    int end_seen;
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
    bool (*lex_gets)(rb_parser_state*);
    bstring line_buffer;

    /* If this is set, we use the io method. */
    FILE *lex_io;
    /* Otherwise, we use this. */
    bstring lex_string;
    bstring lex_lastline;
    bstring lex_nextline;

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

    std::vector<bstring>* magic_comments;
    int column;
    NODE *top_node;

    LocalState* variables;

    int ternary_colon;

    void **memory_pools;
    int pool_size, current_pool;
    char *memory_cur;
    char *memory_last_addr;
    int memory_size;

    bool verbose;

    bool parse_error;
    VALUE processor;

    // Keeps track of lines that 'end' starters are on, to enable
    // better error reporting.
    std::list<StartPosition>* start_lines;

#ifdef RBX_GRAMMAR_19
    rb_encoding *enc;
    rb_encoding *utf8;
#endif

  } rb_parser_state;


#define PARSER_STATE        ((rb_parser_state*)parser_state)
#define PARSER_VAR(var)     (PARSER_STATE->var)

#define end_seen            PARSER_VAR(end_seen)
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
#define lex_io              PARSER_VAR(lex_io)
#define lex_string          PARSER_VAR(lex_string)
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
#define magic_comments      PARSER_VAR(magic_comments)
#define column              PARSER_VAR(column)
#define top_node            PARSER_VAR(top_node)
#define variables           PARSER_VAR(variables)
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
#define start_lines         PARSER_VAR(start_lines)

}; // namespace melbourne

#endif

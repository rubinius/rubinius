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
    VALUE eofp;

    std::vector<bstring>* magic_comments;
    int column;
    NODE *top_node;
    ID *locals;

    LocalState* variables;

    int ternary_colon;

    void **memory_pools;
    int pool_size, current_pool;
    char *memory_cur;
    char *memory_last_addr;
    int memory_size;

    bool verbose;

    VALUE error;
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

#define lex_state           PARSER_VAR(lex_state)
#define lex_pbeg            PARSER_VAR(lex_pbeg)
#define lex_p               PARSER_VAR(lex_p)
#define lex_pend            PARSER_VAR(lex_pend)
#define lex_lastline        PARSER_VAR(lex_lastline)
#define lex_nextline        PARSER_VAR(lex_nextline)
#define ruby_debug_lines    PARSER_VAR(debug_lines)
#define heredoc_end         PARSER_VAR(heredoc_end)
#define command_start       PARSER_VAR(command_start)
#define lex_strterm         PARSER_VAR(lex_strterm)
#define class_nest          PARSER_VAR(class_nest)
#define in_single           PARSER_VAR(in_single)
#define in_def              PARSER_VAR(in_def)
#define compile_for_eval    PARSER_VAR(compile_for_eval)
#define cur_mid             PARSER_VAR(cur_mid)
#define variables           PARSER_VAR(variables)
#define top_node            PARSER_VAR(top_node)
#define cmdarg_stack        PARSER_VAR(cmdarg_stack)
#define cond_stack          PARSER_VAR(cond_stack)
#define in_defined          PARSER_VAR(in_defined)
#define paren_nest          PARSER_VAR(paren_nest)
#define lpar_beg            PARSER_VAR(lpar_beg)

#define tokenbuf            PARSER_VAR(token_buffer)
#define tokidx              PARSER_VAR(tokidx)
#define toksiz              PARSER_VAR(toksiz)

}; // namespace melbourne

#endif

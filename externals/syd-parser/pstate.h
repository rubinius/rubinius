#ifndef __SYD_PSTATE__
#define __SYD_PSTATE__ 1

enum lex_state {
    EXPR_BEG,			/* ignore newline, +/- is a sign. */
    EXPR_END,			/* newline significant, +/- is a operator. */
    EXPR_ARG,			/* newline significant, +/- is a operator. */
    EXPR_CMDARG,		/* newline significant, +/- is a operator. */
    EXPR_ENDARG,		/* newline significant, +/- is a operator. */
    EXPR_MID,			/* newline significant, +/- is a operator. */
    EXPR_FNAME,			/* ignore newline, no reserved words. */
    EXPR_DOT,			/* right after `.' or `::', no reserved words. */
    EXPR_CLASS,			/* immediate after `class', no here document. */
}; 

struct METHOD {
    VALUE klass, rklass;
    VALUE recv;
    ID id, oid;
    NODE *body;
};

#ifdef HAVE_LONG_LONG
typedef unsigned LONG_LONG stack_type;
#else
typedef unsigned long stack_type;
#endif

typedef struct rb_parse_state {
    int end_seen;
    VALUE debug_lines;
    int heredoc_end;
    int command_start;
    NODE *lex_strterm;
    int class_nest;
    int in_single;
    int in_def;
    int compile_for_eval;
    ID cur_mid;
    char *token_buffer;
    int tokidx;
    int toksiz;

    VALUE (*lex_gets)();        /* gets function */
    VALUE lex_input;            /* non-nil if File */
    VALUE lex_lastline; /* gc protect */
    char *lex_pbeg;
    char *lex_p;
    char *lex_pend;
    int lex_gets_ptr;

    enum lex_state lex_state;
    int in_defined;
    stack_type cond_stack;
    stack_type cmdarg_stack;

    void *lval; /* the parser's yylval */

    VALUE comments;
    int column;
    NODE *top;
    ID *locals;
    
    VALUE self;

    int ternary_colon;
    
    void **memory_pools;
    int pool_size, current_pool;
    char *memory_cur;
    char *memory_last_addr;
    int memory_size;
    
} rb_parse_state;

#endif

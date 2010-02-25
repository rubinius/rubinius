/**********************************************************************

  parse.y -

  $Author: matz $
  $Date: 2004/11/29 06:13:51 $
  created at: Fri May 28 18:02:42 JST 1993

  Copyright (C) 1993-2003 Yukihiro Matsumoto

**********************************************************************/

%{

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "ruby.h"

#include "internal.hpp"
#include "visitor.hpp"
#include "symbols.hpp"
#include "local_state.hpp"

namespace melbourne {

#ifndef isnumber
#define isnumber isdigit
#endif

/* Defined at least in mach/boolean.h on OS X. */
#ifdef  TRUE
  #undef  TRUE
#endif

#ifdef  FALSE
  #undef FALSE
#endif

#define TRUE  true
#define FALSE false

/*
#define ISALPHA isalpha
#define ISSPACE isspace
#define ISALNUM(x) (isalpha(x) || isnumber(x))
#define ISDIGIT isdigit
#define ISXDIGIT isxdigit
#define ISUPPER isupper
*/

#define ismbchar(c) (0)
#define mbclen(c) (1)

#define string_new(ptr, len) blk2bstr(ptr, len)
#define string_new2(ptr) cstr2bstr(ptr)

long mel_sourceline;
static char *mel_sourcefile;

#define ruby_sourceline mel_sourceline
#define ruby_sourcefile mel_sourcefile

static int
mel_yyerror(const char *, rb_parse_state*);
#define yyparse mel_yyparse
#define yylex mel_yylex
#define yyerror(str) mel_yyerror(str, (rb_parse_state*)parse_state)
#define yylval mel_yylval
#define yychar mel_yychar
#define yydebug mel_yydebug

#define YYPARSE_PARAM parse_state
#define YYLEX_PARAM parse_state

#define ID_SCOPE_SHIFT 3
#define ID_SCOPE_MASK 0x07
#define ID_LOCAL    0x01
#define ID_INSTANCE 0x02
#define ID_GLOBAL   0x03
#define ID_ATTRSET  0x04
#define ID_CONST    0x05
#define ID_CLASS    0x06
#define ID_JUNK     0x07
#define ID_INTERNAL ID_JUNK

#define is_notop_id(id) ((id)>tLAST_TOKEN)
#define is_local_id(id) (is_notop_id(id)&&((id)&ID_SCOPE_MASK)==ID_LOCAL)
#define is_global_id(id) (is_notop_id(id)&&((id)&ID_SCOPE_MASK)==ID_GLOBAL)
#define is_instance_id(id) (is_notop_id(id)&&((id)&ID_SCOPE_MASK)==ID_INSTANCE)
#define is_attrset_id(id) (is_notop_id(id)&&((id)&ID_SCOPE_MASK)==ID_ATTRSET)
#define is_const_id(id) (is_notop_id(id)&&((id)&ID_SCOPE_MASK)==ID_CONST)
#define is_class_id(id) (is_notop_id(id)&&((id)&ID_SCOPE_MASK)==ID_CLASS)
#define is_junk_id(id) (is_notop_id(id)&&((id)&ID_SCOPE_MASK)==ID_JUNK)

#define is_asgn_or_id(id) ((is_notop_id(id)) && \
        (((id)&ID_SCOPE_MASK) == ID_GLOBAL || \
         ((id)&ID_SCOPE_MASK) == ID_INSTANCE || \
         ((id)&ID_SCOPE_MASK) == ID_CLASS))


/* FIXME these went into the ruby_state instead of parse_state
   because a ton of other crap depends on it
char *ruby_sourcefile;          current source file
int   ruby_sourceline;          current line no.
*/
static int yylex(void*, void *);


#define BITSTACK_PUSH(stack, n) (stack = (stack<<1)|((n)&1))
#define BITSTACK_POP(stack)     (stack >>= 1)
#define BITSTACK_LEXPOP(stack)  (stack = (stack >> 1) | (stack & 1))
#define BITSTACK_SET_P(stack)   (stack&1)

#define COND_PUSH(n)    BITSTACK_PUSH(vps->cond_stack, n)
#define COND_POP()      BITSTACK_POP(vps->cond_stack)
#define COND_LEXPOP()   BITSTACK_LEXPOP(vps->cond_stack)
#define COND_P()        BITSTACK_SET_P(vps->cond_stack)

#define CMDARG_PUSH(n)  BITSTACK_PUSH(vps->cmdarg_stack, n)
#define CMDARG_POP()    BITSTACK_POP(vps->cmdarg_stack)
#define CMDARG_LEXPOP() BITSTACK_LEXPOP(vps->cmdarg_stack)
#define CMDARG_P()      BITSTACK_SET_P(vps->cmdarg_stack)

/*
static int class_nest = 0;
static int in_single = 0;
static int in_def = 0;
static int compile_for_eval = 0;
static ID cur_mid = 0;
*/

static NODE *cond(NODE*,rb_parse_state*);
static NODE *logop(enum node_type,NODE*,NODE*,rb_parse_state*);
static int cond_negative(NODE**);

static NODE *newline_node(rb_parse_state*,NODE*);
static void fixpos(NODE*,NODE*);

static int value_expr0(NODE*,rb_parse_state*);
static void void_expr0(NODE*,rb_parse_state*);
static void void_stmts(NODE*,rb_parse_state*);
static NODE *remove_begin(NODE*,rb_parse_state*);
#define  value_expr(node)  value_expr0((node) = \
                              remove_begin(node, (rb_parse_state*)parse_state), \
                              (rb_parse_state*)parse_state)
#define void_expr(node) void_expr0((node) = \
                              remove_begin(node, (rb_parse_state*)parse_state), \
                              (rb_parse_state*)parse_state)

static NODE *block_append(rb_parse_state*,NODE*,NODE*);
static NODE *list_append(rb_parse_state*,NODE*,NODE*);
static NODE *list_concat(NODE*,NODE*);
static NODE *arg_concat(rb_parse_state*,NODE*,NODE*);
static NODE *arg_prepend(rb_parse_state*,NODE*,NODE*);
static NODE *literal_concat(rb_parse_state*,NODE*,NODE*);
static NODE *new_evstr(rb_parse_state*,NODE*);
static NODE *evstr2dstr(rb_parse_state*,NODE*);
static NODE *call_op(NODE*,QUID,int,NODE*,rb_parse_state*);

/* static NODE *negate_lit(NODE*); */
static NODE *ret_args(rb_parse_state*,NODE*);
static NODE *arg_blk_pass(NODE*,NODE*);
static NODE *new_call(rb_parse_state*,NODE*,QUID,NODE*);
static NODE *new_fcall(rb_parse_state*,QUID,NODE*);
static NODE *new_super(rb_parse_state*,NODE*);
static NODE *new_yield(rb_parse_state*,NODE*);

static NODE *mel_gettable(rb_parse_state*,QUID);
#define gettable(i) mel_gettable((rb_parse_state*)parse_state, i)
static NODE *assignable(QUID,NODE*,rb_parse_state*);
static NODE *aryset(NODE*,NODE*,rb_parse_state*);
static NODE *attrset(NODE*,QUID,rb_parse_state*);
static void rb_backref_error(NODE*,rb_parse_state*);
static NODE *node_assign(NODE*,NODE*,rb_parse_state*);

static NODE *match_gen(NODE*,NODE*,rb_parse_state*);
static void mel_local_push(rb_parse_state*, int cnt);
#define local_push(cnt) mel_local_push(vps, cnt)
static void mel_local_pop(rb_parse_state*);
#define local_pop() mel_local_pop(vps)
static intptr_t  mel_local_cnt(rb_parse_state*,QUID);
#define local_cnt(i) mel_local_cnt(vps, i)
static int  mel_local_id(rb_parse_state*,QUID);
#define local_id(i) mel_local_id(vps, i)
static QUID  *mel_local_tbl(rb_parse_state *st);
static QUID   convert_op(QUID id);

#define QUID2SYM(x)   (x)

static void tokadd(char c, rb_parse_state *parse_state);
static int tokadd_string(int, int, int, QUID*, rb_parse_state*);

#define SHOW_PARSER_WARNS 0

static int rb_compile_error(rb_parse_state *st, const char *fmt, ...) {
  va_list ar;
  char msg[256];
  int count;

  va_start(ar, fmt);
  count = vsnprintf(msg, 256, fmt, ar);
  va_end(ar);

  mel_yyerror(msg, st);

  return count;
}

static int _debug_print(const char *fmt, ...) {
#if SHOW_PARSER_WARNS
  va_list ar;
  int i;

  va_start(ar, fmt);
  i = vprintf(fmt, ar);
  va_end(ar);
  return i;
#else
  return 0;
#endif
}

#define rb_warn _debug_print
#define rb_warning _debug_print

void push_start_line(rb_parse_state* st, int line, const char* which) {
  st->start_lines->push_back(StartPosition(line, which));
}

#define PUSH_LINE(which) push_start_line((rb_parse_state*)parse_state, ruby_sourceline, which)

void pop_start_line(rb_parse_state* st) {
  st->start_lines->pop_back();
}

#define POP_LINE() pop_start_line((rb_parse_state*)parse_state)

static QUID rb_parser_sym(const char *name);
static QUID rb_id_attrset(QUID);

rb_parse_state *alloc_parse_state();

static unsigned long scan_oct(const char *start, int len, int *retlen);
static unsigned long scan_hex(const char *start, int len, int *retlen);

static void reset_block(rb_parse_state *parse_state);
static NODE *extract_block_vars(rb_parse_state *parse_state, NODE* node, var_table vars);

#define RE_OPTION_IGNORECASE         (1L)
#define RE_OPTION_EXTENDED           (2L)
#define RE_OPTION_MULTILINE          (4L)
#define RE_OPTION_DONT_CAPTURE_GROUP (128L)
#define RE_OPTION_CAPTURE_GROUP      (256L)
#define RE_OPTION_ONCE               (8192L)

#define NODE_STRTERM NODE_ZARRAY        /* nothing to gc */
#define NODE_HEREDOC NODE_ARRAY         /* 1, 3 to gc */
#define SIGN_EXTEND(x,n) (((1<<((n)-1))^((x)&~(~0<<(n))))-(1<<((n)-1)))
#define nd_func u1.id
#if SIZEOF_SHORT != 2
#define nd_term(node) SIGN_EXTEND((node)->u2.id, (CHAR_BIT*2))
#else
#define nd_term(node) ((signed short)(node)->u2.id)
#endif
#define nd_paren(node) (char)((node)->u2.id >> (CHAR_BIT*2))
#define nd_nest u3.id

#define NEW_BLOCK_VAR(b, v) NEW_NODE(NODE_BLOCK_PASS, 0, b, v)

/* Older versions of Yacc set YYMAXDEPTH to a very low value by default (150,
   for instance).  This is too low for Ruby to parse some files, such as
   date/format.rb, therefore bump the value up to at least Bison's default. */
#ifdef OLD_YACC
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif
#endif

#define vps ((rb_parse_state*)parse_state)

%}

%pure-parser

%union {
    NODE *node;
    QUID id;
    int num;
    var_table vars;
}

%token  kCLASS
        kMODULE
        kDEF
        kUNDEF
        kBEGIN
        kRESCUE
        kENSURE
        kEND
        kIF
        kUNLESS
        kTHEN
        kELSIF
        kELSE
        kCASE
        kWHEN
        kWHILE
        kUNTIL
        kFOR
        kBREAK
        kNEXT
        kREDO
        kRETRY
        kIN
        kDO
        kDO_COND
        kDO_BLOCK
        kRETURN
        kYIELD
        kSUPER
        kSELF
        kNIL
        kTRUE
        kFALSE
        kAND
        kOR
        kNOT
        kIF_MOD
        kUNLESS_MOD
        kWHILE_MOD
        kUNTIL_MOD
        kRESCUE_MOD
        kALIAS
        kDEFINED
        klBEGIN
        klEND
        k__LINE__
        k__FILE__

%token <id>   tIDENTIFIER tFID tGVAR tIVAR tCONSTANT tCVAR tXSTRING_BEG
%token <node> tINTEGER tFLOAT tSTRING_CONTENT
%token <node> tNTH_REF tBACK_REF
%token <num>  tREGEXP_END
%type <node> singleton strings string string1 xstring regexp
%type <node> string_contents xstring_contents string_content
%type <node> words qwords word_list qword_list word
%type <node> literal numeric dsym cpath
%type <node> bodystmt compstmt stmts stmt expr arg primary command command_call method_call
%type <node> expr_value arg_value primary_value
%type <node> if_tail opt_else case_body cases opt_rescue exc_list exc_var opt_ensure
%type <node> args when_args call_args call_args2 open_args paren_args opt_paren_args
%type <node> command_args aref_args opt_block_arg block_arg var_ref var_lhs
%type <node> mrhs superclass block_call block_command
%type <node> f_arglist f_args f_optarg f_opt f_block_arg opt_f_block_arg
%type <node> assoc_list assocs assoc undef_list backref string_dvar
%type <node> for_var block_var opt_block_var block_par
%type <node> brace_block cmd_brace_block do_block lhs none fitem
%type <node> mlhs mlhs_head mlhs_basic mlhs_entry mlhs_item mlhs_node
%type <id>   fsym variable sym symbol operation operation2 operation3
%type <id>   cname fname op f_rest_arg
%type <num>  f_norm_arg f_arg
%token tUPLUS           /* unary+ */
%token tUMINUS          /* unary- */
%token tUBS             /* unary\ */
%token tPOW             /* ** */
%token tCMP             /* <=> */
%token tEQ              /* == */
%token tEQQ             /* === */
%token tNEQ             /* != */
%token tGEQ             /* >= */
%token tLEQ             /* <= */
%token tANDOP tOROP     /* && and || */
%token tMATCH tNMATCH   /* =~ and !~ */
%token tDOT2 tDOT3      /* .. and ... */
%token tAREF tASET      /* [] and []= */
%token tLSHFT tRSHFT    /* << and >> */
%token tCOLON2          /* :: */
%token tCOLON3          /* :: at EXPR_BEG */
%token <id> tOP_ASGN    /* +=, -=  etc. */
%token tASSOC           /* => */
%token tLPAREN          /* ( */
%token tLPAREN_ARG      /* ( */
%token tRPAREN          /* ) */
%token tLBRACK          /* [ */
%token tLBRACE          /* { */
%token tLBRACE_ARG      /* { */
%token tSTAR            /* * */
%token tAMPER           /* & */
%token tSYMBEG tSTRING_BEG tREGEXP_BEG tWORDS_BEG tQWORDS_BEG
%token tSTRING_DBEG tSTRING_DVAR tSTRING_END

/*
 *      precedence table
 */

%nonassoc tLOWEST
%nonassoc tLBRACE_ARG

%nonassoc  kIF_MOD kUNLESS_MOD kWHILE_MOD kUNTIL_MOD
%left  kOR kAND
%right kNOT
%nonassoc kDEFINED
%right '=' tOP_ASGN
%left kRESCUE_MOD
%right '?' ':'
%nonassoc tDOT2 tDOT3
%left  tOROP
%left  tANDOP
%nonassoc  tCMP tEQ tEQQ tNEQ tMATCH tNMATCH
%left  '>' tGEQ '<' tLEQ
%left  '|' '^'
%left  '&'
%left  tLSHFT tRSHFT
%left  '+' '-'
%left  '*' '/' '%'
%right tUMINUS_NUM tUMINUS
%right tPOW
%right '!' '~' tUPLUS

%token tLAST_TOKEN

%%
program         :  {
                        vps->lex_state = EXPR_BEG;
                        vps->variables = new LocalState(0);
                        class_nest = 0;
                    }
                  compstmt
                    {
                        if ($2 && !compile_for_eval) {
                            /* last expression should not be void */
                            if (nd_type($2) != NODE_BLOCK) void_expr($2);
                            else {
                                NODE *node = $2;
                                while (node->nd_next) {
                                    node = node->nd_next;
                                }
                                void_expr(node->nd_head);
                            }
                        }
                        vps->top = block_append(vps, vps->top, $2);
                        class_nest = 0;
                    }
                ;

bodystmt        : compstmt
                  opt_rescue
                  opt_else
                  opt_ensure
                    {
                        $$ = $1;
                        if ($2) {
                            $$ = NEW_RESCUE($1, $2, $3);
                        }
                        else if ($3) {
                            rb_warn("else without rescue is useless");
                            $$ = block_append(vps, $$, $3);
                        }
                        if ($4) {
                            $$ = NEW_ENSURE($$, $4);
                        }
                        fixpos($$, $1);
                    }
                ;

compstmt        : stmts opt_terms
                    {
                        void_stmts($1, vps);
                        $$ = $1;
                    }
                ;

stmts           : none
                | stmt
                    {
                        $$ = newline_node(vps, $1);
                    }
                | stmts terms stmt
                    {
                        $$ = block_append(vps, $1, newline_node(vps, $3));
                    }
                | error stmt
                    {
                        $$ = remove_begin($2, vps);
                    }
                ;

stmt            : kALIAS fitem {vps->lex_state = EXPR_FNAME;} fitem
                    {
                        $$ = NEW_ALIAS($2, $4);
                    }
                | kALIAS tGVAR tGVAR
                    {
                        $$ = NEW_VALIAS($2, $3);
                    }
                | kALIAS tGVAR tBACK_REF
                    {
                        char buf[3];

                        snprintf(buf, sizeof(buf), "$%c", (char)$3->nd_nth);
                        $$ = NEW_VALIAS($2, rb_parser_sym(buf));
                    }
                | kALIAS tGVAR tNTH_REF
                    {
                        yyerror("can't make alias for the number variables");
                        $$ = 0;
                    }
                | kUNDEF undef_list
                    {
                        $$ = $2;
                    }
                | stmt kIF_MOD expr_value
                    {
                        $$ = NEW_IF(cond($3, vps), remove_begin($1, vps), 0);
                        fixpos($$, $3);
                        if (cond_negative(&$$->nd_cond)) {
                            $$->nd_else = $$->nd_body;
                            $$->nd_body = 0;
                        }
                    }
                | stmt kUNLESS_MOD expr_value
                    {
                        $$ = NEW_UNLESS(cond($3, vps), remove_begin($1, vps), 0);
                        fixpos($$, $3);
                        if (cond_negative(&$$->nd_cond)) {
                            $$->nd_body = $$->nd_else;
                            $$->nd_else = 0;
                        }
                    }
                | stmt kWHILE_MOD expr_value
                    {
                        if ($1 && nd_type($1) == NODE_BEGIN) {
                            $$ = NEW_WHILE(cond($3, vps), $1->nd_body, 0);
                        }
                        else {
                            $$ = NEW_WHILE(cond($3, vps), $1, 1);
                        }
                        if (cond_negative(&$$->nd_cond)) {
                            nd_set_type($$, NODE_UNTIL);
                        }
                    }
                | stmt kUNTIL_MOD expr_value
                    {
                        if ($1 && nd_type($1) == NODE_BEGIN) {
                            $$ = NEW_UNTIL(cond($3, vps), $1->nd_body, 0);
                        }
                        else {
                            $$ = NEW_UNTIL(cond($3, vps), $1, 1);
                        }
                        if (cond_negative(&$$->nd_cond)) {
                            nd_set_type($$, NODE_WHILE);
                        }
                    }
                | stmt kRESCUE_MOD stmt
                    {
                        NODE *resq = NEW_RESBODY(0, remove_begin($3, vps), 0);
                        $$ = NEW_RESCUE(remove_begin($1, vps), resq, 0);
                    }
                | klBEGIN
                    {
                        if (in_def || in_single) {
                            yyerror("BEGIN in method");
                        }
                        local_push(0);
                    }
                  '{' compstmt '}'
                    {
                        /*
                        ruby_eval_tree_begin = block_append(ruby_eval_tree_begin,
                                                            NEW_PREEXE($4));
                        */
                        local_pop();
                        $$ = 0;
                    }
                | klEND '{' compstmt '}'
                    {
                        if (in_def || in_single) {
                            rb_warn("END in method; use at_exit");
                        }

                        $$ = NEW_ITER(0, NEW_POSTEXE(), $3);
                    }
                | lhs '=' command_call
                    {
                        $$ = node_assign($1, $3, vps);
                    }
                | mlhs '=' command_call
                    {
                        value_expr($3);
                        $1->nd_value = ($1->nd_head) ? NEW_TO_ARY($3) : NEW_ARRAY($3);
                        $$ = $1;
                    }
                | var_lhs tOP_ASGN command_call
                    {
                        value_expr($3);
                        if ($1) {
                            QUID vid = $1->nd_vid;
                            if ($2 == tOROP) {
                                $1->nd_value = $3;
                                $$ = NEW_OP_ASGN_OR(gettable(vid), $1);
                                if (is_asgn_or_id(vid)) {
                                    $$->nd_aid = vid;
                                }
                            }
                            else if ($2 == tANDOP) {
                                $1->nd_value = $3;
                                $$ = NEW_OP_ASGN_AND(gettable(vid), $1);
                            }
                            else {
                                $$ = $1;
                                $$->nd_value = call_op(gettable(vid),$2,1,$3, vps);
                            }
                        }
                        else {
                            $$ = 0;
                        }
                    }
                | primary_value '[' aref_args ']' tOP_ASGN command_call
                    {
                        NODE *args;

                        value_expr($6);
                        if (!$3) $3 = NEW_ZARRAY();
                        args = arg_concat(vps, $6, $3);
                        if ($5 == tOROP) {
                            $5 = 0;
                        }
                        else if ($5 == tANDOP) {
                            $5 = 1;
                        }
                        $$ = NEW_OP_ASGN1($1, $5, args);
                        fixpos($$, $1);
                    }
                | primary_value '.' tIDENTIFIER tOP_ASGN command_call
                    {
                        value_expr($5);
                        if ($4 == tOROP) {
                            $4 = 0;
                        }
                        else if ($4 == tANDOP) {
                            $4 = 1;
                        }
                        $$ = NEW_OP_ASGN2($1, $3, $4, $5);
                        fixpos($$, $1);
                    }
                | primary_value '.' tCONSTANT tOP_ASGN command_call
                    {
                        value_expr($5);
                        if ($4 == tOROP) {
                            $4 = 0;
                        }
                        else if ($4 == tANDOP) {
                            $4 = 1;
                        }
                        $$ = NEW_OP_ASGN2($1, $3, $4, $5);
                        fixpos($$, $1);
                    }
                | primary_value tCOLON2 tIDENTIFIER tOP_ASGN command_call
                    {
                        value_expr($5);
                        if ($4 == tOROP) {
                            $4 = 0;
                        }
                        else if ($4 == tANDOP) {
                            $4 = 1;
                        }
                        $$ = NEW_OP_ASGN2($1, $3, $4, $5);
                        fixpos($$, $1);
                    }
                | backref tOP_ASGN command_call
                    {
                        rb_backref_error($1, vps);
                        $$ = 0;
                    }
                | lhs '=' mrhs
                    {
                        $$ = node_assign($1, NEW_SVALUE($3), vps);
                    }
                | mlhs '=' arg_value
                    {
                        $1->nd_value = ($1->nd_head) ? NEW_TO_ARY($3) : NEW_ARRAY($3);
                        $$ = $1;
                    }
                | mlhs '=' mrhs
                    {
                        $1->nd_value = $3;
                        $$ = $1;
                    }
                | expr
                ;

expr            : command_call
                | expr kAND expr
                    {
                        $$ = logop(NODE_AND, $1, $3, vps);
                    }
                | expr kOR expr
                    {
                        $$ = logop(NODE_OR, $1, $3, vps);
                    }
                | kNOT expr
                    {
                        $$ = NEW_NOT(cond($2, vps));
                    }
                | '!' command_call
                    {
                        $$ = NEW_NOT(cond($2, vps));
                    }
                | arg
                ;

expr_value      : expr
                    {
                        value_expr($$);
                        $$ = $1;
                    }
                ;

command_call    : command
                | block_command
                | kRETURN call_args
                    {
                        $$ = NEW_RETURN(ret_args(vps, $2));
                    }
                | kBREAK call_args
                    {
                        $$ = NEW_BREAK(ret_args(vps, $2));
                    }
                | kNEXT call_args
                    {
                        $$ = NEW_NEXT(ret_args(vps, $2));
                    }
                ;

block_command   : block_call
                | block_call '.' operation2 command_args
                    {
                        $$ = new_call(vps, $1, $3, $4);
                    }
                | block_call tCOLON2 operation2 command_args
                    {
                        $$ = new_call(vps, $1, $3, $4);
                    }
                ;

cmd_brace_block : tLBRACE_ARG
                    {
                        $<num>1 = ruby_sourceline;
                        reset_block(vps);
                    }
                  opt_block_var { $<vars>$ = vps->variables->block_vars; }
                  compstmt
                  '}'
                    {
                        $$ = NEW_ITER($3, 0, extract_block_vars(vps, $5, $<vars>4));
                        nd_set_line($$, $<num>1);
                    }
                ;

command         : operation command_args       %prec tLOWEST
                    {
                        $$ = new_fcall(vps, $1, $2);
                        fixpos($$, $2);
                   }
                | operation command_args cmd_brace_block
                    {
                        $$ = new_fcall(vps, $1, $2);
                        if ($3) {
                            if (nd_type($$) == NODE_BLOCK_PASS) {
                                rb_compile_error(vps, "both block arg and actual block given");
                            }
                            $3->nd_iter = $$;
                            $$ = $3;
                        }
                        fixpos($$, $2);
                   }
                | primary_value '.' operation2 command_args     %prec tLOWEST
                    {
                        $$ = new_call(vps, $1, $3, $4);
                        fixpos($$, $1);
                    }
                | primary_value '.' operation2 command_args cmd_brace_block
                    {
                        $$ = new_call(vps, $1, $3, $4);
                        if ($5) {
                            if (nd_type($$) == NODE_BLOCK_PASS) {
                                rb_compile_error(vps, "both block arg and actual block given");
                            }
                            $5->nd_iter = $$;
                            $$ = $5;
                        }
                        fixpos($$, $1);
                   }
                | primary_value tCOLON2 operation2 command_args %prec tLOWEST
                    {
                        $$ = new_call(vps, $1, $3, $4);
                        fixpos($$, $1);
                    }
                | primary_value tCOLON2 operation2 command_args cmd_brace_block
                    {
                        $$ = new_call(vps, $1, $3, $4);
                        if ($5) {
                            if (nd_type($$) == NODE_BLOCK_PASS) {
                                rb_compile_error(vps, "both block arg and actual block given");
                            }
                            $5->nd_iter = $$;
                            $$ = $5;
                        }
                        fixpos($$, $1);
                   }
                | kSUPER command_args
                    {
                        $$ = new_super(vps, $2);
                        fixpos($$, $2);
                    }
                | kYIELD command_args
                    {
                        $$ = new_yield(vps, $2);
                        fixpos($$, $2);
                    }
                ;

mlhs            : mlhs_basic
                | tLPAREN mlhs_entry ')'
                    {
                        $$ = $2;
                    }
                ;

mlhs_entry      : mlhs_basic
                | tLPAREN mlhs_entry ')'
                    {
                        $$ = NEW_MASGN(NEW_LIST($2), 0);
                    }
                ;

mlhs_basic      : mlhs_head
                    {
                        $$ = NEW_MASGN($1, 0);
                    }
                | mlhs_head mlhs_item
                    {
                        $$ = NEW_MASGN(list_append(vps, $1,$2), 0);
                    }
                | mlhs_head tSTAR mlhs_node
                    {
                        $$ = NEW_MASGN($1, $3);
                    }
                | mlhs_head tSTAR
                    {
                        $$ = NEW_MASGN($1, -1);
                    }
                | tSTAR mlhs_node
                    {
                        $$ = NEW_MASGN(0, $2);
                    }
                | tSTAR
                    {
                        $$ = NEW_MASGN(0, -1);
                    }
                ;

mlhs_item       : mlhs_node
                | tLPAREN mlhs_entry ')'
                    {
                        $$ = $2;
                    }
                ;

mlhs_head       : mlhs_item ','
                    {
                        $$ = NEW_LIST($1);
                    }
                | mlhs_head mlhs_item ','
                    {
                        $$ = list_append(vps, $1, $2);
                    }
                ;

mlhs_node       : variable
                    {
                        $$ = assignable($1, 0, vps);
                    }
                | primary_value '[' aref_args ']'
                    {
                        $$ = aryset($1, $3, vps);
                    }
                | primary_value '.' tIDENTIFIER
                    {
                        $$ = attrset($1, $3, vps);
                    }
                | primary_value tCOLON2 tIDENTIFIER
                    {
                        $$ = attrset($1, $3, vps);
                    }
                | primary_value '.' tCONSTANT
                    {
                        $$ = attrset($1, $3, vps);
                    }
                | primary_value tCOLON2 tCONSTANT
                    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        $$ = NEW_CDECL(0, 0, NEW_COLON2($1, $3));
                    }
                | tCOLON3 tCONSTANT
                    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        $$ = NEW_CDECL(0, 0, NEW_COLON3($2));
                    }
                | backref
                    {
                        rb_backref_error($1, vps);
                        $$ = 0;
                    }
                ;

lhs             : variable
                    {
                        $$ = assignable($1, 0, vps);
                    }
                | primary_value '[' aref_args ']'
                    {
                        $$ = aryset($1, $3, vps);
                    }
                | primary_value '.' tIDENTIFIER
                    {
                        $$ = attrset($1, $3, vps);
                    }
                | primary_value tCOLON2 tIDENTIFIER
                    {
                        $$ = attrset($1, $3, vps);
                    }
                | primary_value '.' tCONSTANT
                    {
                        $$ = attrset($1, $3, vps);
                    }
                | primary_value tCOLON2 tCONSTANT
                    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        $$ = NEW_CDECL(0, 0, NEW_COLON2($1, $3));
                    }
                | tCOLON3 tCONSTANT
                    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        $$ = NEW_CDECL(0, 0, NEW_COLON3($2));
                    }
                | backref
                    {
                        rb_backref_error($1, vps);
                        $$ = 0;
                    }
                ;

cname           : tIDENTIFIER
                    {
                        yyerror("class/module name must be CONSTANT");
                    }
                | tCONSTANT
                ;

cpath           : tCOLON3 cname
                    {
                        $$ = NEW_COLON3($2);
                    }
                | cname
                    {
                        $$ = NEW_COLON2(0, $$);
                    }
                | primary_value tCOLON2 cname
                    {
                        $$ = NEW_COLON2($1, $3);
                    }
                ;

fname           : tIDENTIFIER
                | tCONSTANT
                | tFID
                | op
                    {
                        vps->lex_state = EXPR_END;
                        $$ = convert_op($1);
                    }
                | reswords
                    {
                        vps->lex_state = EXPR_END;
                        $$ = $<id>1;
                    }
                ;

fsym            : fname
                | symbol
                ;

fitem           : fsym
                    {
                        $$ = NEW_LIT(QUID2SYM($1));
                    }
                | dsym
                ;

undef_list      : fitem
                    {
                        $$ = NEW_UNDEF($1);
                    }
                | undef_list ',' {vps->lex_state = EXPR_FNAME;} fitem
                    {
                        $$ = block_append(vps, $1, NEW_UNDEF($4));
                    }
                ;

op              : '|'           { $$ = '|'; }
                | '^'           { $$ = '^'; }
                | '&'           { $$ = '&'; }
                | tCMP          { $$ = tCMP; }
                | tEQ           { $$ = tEQ; }
                | tEQQ          { $$ = tEQQ; }
                | tMATCH        { $$ = tMATCH; }
                | '>'           { $$ = '>'; }
                | tGEQ          { $$ = tGEQ; }
                | '<'           { $$ = '<'; }
                | tLEQ          { $$ = tLEQ; }
                | tLSHFT        { $$ = tLSHFT; }
                | tRSHFT        { $$ = tRSHFT; }
                | '+'           { $$ = '+'; }
                | '-'           { $$ = '-'; }
                | '*'           { $$ = '*'; }
                | tSTAR         { $$ = '*'; }
                | '/'           { $$ = '/'; }
                | '%'           { $$ = '%'; }
                | tPOW          { $$ = tPOW; }
                | '~'           { $$ = '~'; }
                | tUPLUS        { $$ = tUPLUS; }
                | tUMINUS       { $$ = tUMINUS; }
                | tAREF         { $$ = tAREF; }
                | tASET         { $$ = tASET; }
                | '`'           { $$ = '`'; }
                ;

reswords        : k__LINE__ | k__FILE__  | klBEGIN | klEND
                | kALIAS | kAND | kBEGIN | kBREAK | kCASE | kCLASS | kDEF
                | kDEFINED | kDO | kELSE | kELSIF | kEND | kENSURE | kFALSE
                | kFOR | kIN | kMODULE | kNEXT | kNIL | kNOT
                | kOR | kREDO | kRESCUE | kRETRY | kRETURN | kSELF | kSUPER
                | kTHEN | kTRUE | kUNDEF | kWHEN | kYIELD
                | kIF_MOD | kUNLESS_MOD | kWHILE_MOD | kUNTIL_MOD | kRESCUE_MOD
                ;

arg             : lhs '=' arg
                    {
                        $$ = node_assign($1, $3, vps);
                    }
                | lhs '=' arg kRESCUE_MOD arg
                    {
                        $$ = node_assign($1, NEW_RESCUE($3, NEW_RESBODY(0,$5,0), 0), vps);
                    }
                | var_lhs tOP_ASGN arg
                    {
                        value_expr($3);
                        if ($1) {
                            QUID vid = $1->nd_vid;
                            if ($2 == tOROP) {
                                $1->nd_value = $3;
                                $$ = NEW_OP_ASGN_OR(gettable(vid), $1);
                                if (is_asgn_or_id(vid)) {
                                    $$->nd_aid = vid;
                                }
                            }
                            else if ($2 == tANDOP) {
                                $1->nd_value = $3;
                                $$ = NEW_OP_ASGN_AND(gettable(vid), $1);
                            }
                            else {
                                $$ = $1;
                                $$->nd_value = call_op(gettable(vid),$2,1,$3, vps);
                            }
                        }
                        else {
                            $$ = 0;
                        }
                    }
                | primary_value '[' aref_args ']' tOP_ASGN arg
                    {
                        NODE *args;

                        value_expr($6);
                        if (!$3) $3 = NEW_ZARRAY();
                        args = arg_concat(vps, $6, $3);
                        if ($5 == tOROP) {
                            $5 = 0;
                        }
                        else if ($5 == tANDOP) {
                            $5 = 1;
                        }
                        $$ = NEW_OP_ASGN1($1, $5, args);
                        fixpos($$, $1);
                    }
                | primary_value '.' tIDENTIFIER tOP_ASGN arg
                    {
                        value_expr($5);
                        if ($4 == tOROP) {
                            $4 = 0;
                        }
                        else if ($4 == tANDOP) {
                            $4 = 1;
                        }
                        $$ = NEW_OP_ASGN2($1, $3, $4, $5);
                        fixpos($$, $1);
                    }
                | primary_value '.' tCONSTANT tOP_ASGN arg
                    {
                        value_expr($5);
                        if ($4 == tOROP) {
                            $4 = 0;
                        }
                        else if ($4 == tANDOP) {
                            $4 = 1;
                        }
                        $$ = NEW_OP_ASGN2($1, $3, $4, $5);
                        fixpos($$, $1);
                    }
                | primary_value tCOLON2 tIDENTIFIER tOP_ASGN arg
                    {
                        value_expr($5);
                        if ($4 == tOROP) {
                            $4 = 0;
                        }
                        else if ($4 == tANDOP) {
                            $4 = 1;
                        }
                        $$ = NEW_OP_ASGN2($1, $3, $4, $5);
                        fixpos($$, $1);
                    }
                | primary_value tCOLON2 tCONSTANT tOP_ASGN arg
                    {
                        yyerror("constant re-assignment");
                        $$ = 0;
                    }
                | tCOLON3 tCONSTANT tOP_ASGN arg
                    {
                        yyerror("constant re-assignment");
                        $$ = 0;
                    }
                | backref tOP_ASGN arg
                    {
                        rb_backref_error($1, vps);
                        $$ = 0;
                    }
                | arg tDOT2 arg
                    {
                        value_expr($1);
                        value_expr($3);
                        $$ = NEW_DOT2($1, $3);
                    }
                | arg tDOT3 arg
                    {
                        value_expr($1);
                        value_expr($3);
                        $$ = NEW_DOT3($1, $3);
                    }
                | arg '+' arg
                    {
                        $$ = call_op($1, '+', 1, $3, vps);
                    }
                | arg '-' arg
                    {
                        $$ = call_op($1, '-', 1, $3, vps);
                    }
                | arg '*' arg
                    {
                        $$ = call_op($1, '*', 1, $3, vps);
                    }
                | arg '/' arg
                    {
                        $$ = call_op($1, '/', 1, $3, vps);
                    }
                | arg '%' arg
                    {
                        $$ = call_op($1, '%', 1, $3, vps);
                    }
                | arg tPOW arg
                    {
                        $$ = call_op($1, tPOW, 1, $3, vps);
                    }
                | tUMINUS_NUM tINTEGER tPOW arg
                    {
                        $$ = call_op(call_op($2, tPOW, 1, $4, vps), tUMINUS, 0, 0, vps);
                    }
                | tUMINUS_NUM tFLOAT tPOW arg
                    {
                        $$ = call_op(call_op($2, tPOW, 1, $4, vps), tUMINUS, 0, 0, vps);
                    }
                | tUPLUS arg
                    {
                        if ($2 && nd_type($2) == NODE_LIT) {
                            $$ = $2;
                        }
                        else {
                            $$ = call_op($2, tUPLUS, 0, 0, vps);
                        }
                    }
                | tUMINUS arg
                    {
                        $$ = call_op($2, tUMINUS, 0, 0, vps);
                    }
                | arg '|' arg
                    {
                        $$ = call_op($1, '|', 1, $3, vps);
                    }
                | arg '^' arg
                    {
                        $$ = call_op($1, '^', 1, $3, vps);
                    }
                | arg '&' arg
                    {
                        $$ = call_op($1, '&', 1, $3, vps);
                    }
                | arg tCMP arg
                    {
                        $$ = call_op($1, tCMP, 1, $3, vps);
                    }
                | arg '>' arg
                    {
                        $$ = call_op($1, '>', 1, $3, vps);
                    }
                | arg tGEQ arg
                    {
                        $$ = call_op($1, tGEQ, 1, $3, vps);
                    }
                | arg '<' arg
                    {
                        $$ = call_op($1, '<', 1, $3, vps);
                    }
                | arg tLEQ arg
                    {
                        $$ = call_op($1, tLEQ, 1, $3, vps);
                    }
                | arg tEQ arg
                    {
                        $$ = call_op($1, tEQ, 1, $3, vps);
                    }
                | arg tEQQ arg
                    {
                        $$ = call_op($1, tEQQ, 1, $3, vps);
                    }
                | arg tNEQ arg
                    {
                        $$ = NEW_NOT(call_op($1, tEQ, 1, $3, vps));
                    }
                | arg tMATCH arg
                    {
                        $$ = match_gen($1, $3, vps);
                    }
                | arg tNMATCH arg
                    {
                        $$ = NEW_NOT(match_gen($1, $3, vps));
                    }
                | '!' arg
                    {
                        $$ = NEW_NOT(cond($2, vps));
                    }
                | '~' arg
                    {
                        $$ = call_op($2, '~', 0, 0, vps);
                    }
                | arg tLSHFT arg
                    {
                        $$ = call_op($1, tLSHFT, 1, $3, vps);
                    }
                | arg tRSHFT arg
                    {
                        $$ = call_op($1, tRSHFT, 1, $3, vps);
                    }
                | arg tANDOP arg
                    {
                        $$ = logop(NODE_AND, $1, $3, vps);
                    }
                | arg tOROP arg
                    {
                        $$ = logop(NODE_OR, $1, $3, vps);
                    }
                | kDEFINED opt_nl {vps->in_defined = 1;} arg
                    {
                        vps->in_defined = 0;
                        $$ = NEW_DEFINED($4);
                    }
                | arg '?' {vps->ternary_colon++;} arg ':' arg
                    {
                        $$ = NEW_IF(cond($1, vps), $4, $6);
                        fixpos($$, $1);
                        vps->ternary_colon--;
                    }
                | primary
                    {
                        $$ = $1;
                    }
                ;

arg_value       : arg
                    {
                        value_expr($1);
                        $$ = $1;
                    }
                ;

aref_args       : none
                | command opt_nl
                    {
                        rb_warn("parenthesize argument(s) for future version");
                        $$ = NEW_LIST($1);
                    }
                | args trailer
                    {
                        $$ = $1;
                    }
                | args ',' tSTAR arg opt_nl
                    {
                        value_expr($4);
                        $$ = arg_concat(vps, $1, $4);
                    }
                | assocs trailer
                    {
                        $$ = NEW_LIST(NEW_HASH($1));
                    }
                | tSTAR arg opt_nl
                    {
                        value_expr($2);
                        $$ = NEW_NEWLINE(NEW_SPLAT($2));
                    }
                ;

paren_args      : '(' none ')'
                    {
                        $$ = $2;
                    }
                | '(' call_args opt_nl ')'
                    {
                        $$ = $2;
                    }
                | '(' block_call opt_nl ')'
                    {
                        rb_warn("parenthesize argument for future version");
                        $$ = NEW_LIST($2);
                    }
                | '(' args ',' block_call opt_nl ')'
                    {
                        rb_warn("parenthesize argument for future version");
                        $$ = list_append(vps, $2, $4);
                    }
                ;

opt_paren_args  : none
                | paren_args
                ;

call_args       : command
                    {
                        rb_warn("parenthesize argument(s) for future version");
                        $$ = NEW_LIST($1);
                    }
                | args opt_block_arg
                    {
                        $$ = arg_blk_pass($1, $2);
                    }
                | args ',' tSTAR arg_value opt_block_arg
                    {
                        $$ = arg_concat(vps, $1, $4);
                        $$ = arg_blk_pass($$, $5);
                    }
                | assocs opt_block_arg
                    {
                        $$ = NEW_LIST(NEW_HASH($1));
                        $$ = arg_blk_pass($$, $2);
                    }
                | assocs ',' tSTAR arg_value opt_block_arg
                    {
                        $$ = arg_concat(vps, NEW_LIST(NEW_HASH($1)), $4);
                        $$ = arg_blk_pass($$, $5);
                    }
                | args ',' assocs opt_block_arg
                    {
                        $$ = list_append(vps, $1, NEW_HASH($3));
                        $$ = arg_blk_pass($$, $4);
                    }
                | args ',' assocs ',' tSTAR arg opt_block_arg
                    {
                        value_expr($6);
                        $$ = arg_concat(vps, list_append(vps, $1, NEW_HASH($3)), $6);
                        $$ = arg_blk_pass($$, $7);
                    }
                | tSTAR arg_value opt_block_arg
                    {
                        $$ = arg_blk_pass(NEW_SPLAT($2), $3);
                    }
                | block_arg
                ;

call_args2      : arg_value ',' args opt_block_arg
                    {
                        $$ = arg_blk_pass(list_concat(NEW_LIST($1),$3), $4);
                    }
                | arg_value ',' block_arg
                    {
                        $$ = arg_blk_pass($1, $3);
                    }
                | arg_value ',' tSTAR arg_value opt_block_arg
                    {
                        $$ = arg_concat(vps, NEW_LIST($1), $4);
                        $$ = arg_blk_pass($$, $5);
                    }
                | arg_value ',' args ',' tSTAR arg_value opt_block_arg
                    {
                        $$ = arg_concat(vps, list_concat(NEW_LIST($1),$3), $6);
                        $$ = arg_blk_pass($$, $7);
                    }
                | assocs opt_block_arg
                    {
                        $$ = NEW_LIST(NEW_HASH($1));
                        $$ = arg_blk_pass($$, $2);
                    }
                | assocs ',' tSTAR arg_value opt_block_arg
                    {
                        $$ = arg_concat(vps, NEW_LIST(NEW_HASH($1)), $4);
                        $$ = arg_blk_pass($$, $5);
                    }
                | arg_value ',' assocs opt_block_arg
                    {
                        $$ = list_append(vps, NEW_LIST($1), NEW_HASH($3));
                        $$ = arg_blk_pass($$, $4);
                    }
                | arg_value ',' args ',' assocs opt_block_arg
                    {
                        $$ = list_append(vps, list_concat(NEW_LIST($1),$3), NEW_HASH($5));
                        $$ = arg_blk_pass($$, $6);
                    }
                | arg_value ',' assocs ',' tSTAR arg_value opt_block_arg
                    {
                        $$ = arg_concat(vps, list_append(vps, NEW_LIST($1), NEW_HASH($3)), $6);
                        $$ = arg_blk_pass($$, $7);
                    }
                | arg_value ',' args ',' assocs ',' tSTAR arg_value opt_block_arg
                    {
                        $$ = arg_concat(vps, list_append(vps,
                                        list_concat(NEW_LIST($1), $3), NEW_HASH($5)), $8);
                        $$ = arg_blk_pass($$, $9);
                    }
                | tSTAR arg_value opt_block_arg
                    {
                        $$ = arg_blk_pass(NEW_SPLAT($2), $3);
                    }
                | block_arg
                ;

command_args    :  {
                        $<num>$ = vps->cmdarg_stack;
                        CMDARG_PUSH(1);
                    }
                  open_args
                    {
                        /* CMDARG_POP() */
                        vps->cmdarg_stack = $<num>1;
                        $$ = $2;
                    }
                ;

open_args       : call_args
                | tLPAREN_ARG  {vps->lex_state = EXPR_ENDARG;} ')'
                    {
                        rb_warn("don't put space before argument parentheses");
                        $$ = 0;
                    }
                | tLPAREN_ARG call_args2 {vps->lex_state = EXPR_ENDARG;} ')'
                    {
                        rb_warn("don't put space before argument parentheses");
                        $$ = $2;
                    }
                ;

block_arg       : tAMPER arg_value
                    {
                        $$ = NEW_BLOCK_PASS($2);
                    }
                ;

opt_block_arg   : ',' block_arg
                    {
                        $$ = $2;
                    }
                | none
                ;

args            : arg_value
                    {
                        $$ = NEW_LIST($1);
                    }
                | args ',' arg_value
                    {
                        $$ = list_append(vps, $1, $3);
                    }
                ;

mrhs            : args ',' arg_value
                    {
                        $$ = list_append(vps, $1, $3);
                    }
                | args ',' tSTAR arg_value
                    {
                        $$ = arg_concat(vps, $1, $4);
                    }
                | tSTAR arg_value
                    {
                        $$ = NEW_SPLAT($2);
                    }
                ;

primary         : literal
                | strings
                | xstring
                | regexp
                | words
                | qwords
                | var_ref
                | backref
                | tFID
                    {
                        $$ = NEW_FCALL($1, 0);
                    }
                | kBEGIN
                    {
                        $<num>1 = ruby_sourceline;
                        PUSH_LINE("begin");
                    }
                  bodystmt
                  kEND
                    {
                        POP_LINE();
                        if ($3 == NULL)
                            $$ = NEW_NIL();
                        else
                            $$ = NEW_BEGIN($3);
                        nd_set_line($$, $<num>1);
                    }
                | tLPAREN_ARG expr {vps->lex_state = EXPR_ENDARG;} opt_nl ')'
                    {
                        rb_warning("(...) interpreted as grouped expression");
                        $$ = $2;
                    }
                | tLPAREN compstmt ')'
                    {
                        if (!$2) $$ = NEW_NIL();
                        else $$ = $2;
                    }
                | primary_value tCOLON2 tCONSTANT
                    {
                        $$ = NEW_COLON2($1, $3);
                    }
                | tCOLON3 tCONSTANT
                    {
                        $$ = NEW_COLON3($2);
                    }
                | primary_value '[' aref_args ']'
                    {
                        if ($1 && nd_type($1) == NODE_SELF) {
                            $$ = NEW_FCALL(convert_op(tAREF), $3);
                        } else {
                            $$ = NEW_CALL($1, convert_op(tAREF), $3);
                        }
                        fixpos($$, $1);
                    }
                | tLBRACK aref_args ']'
                    {
                        if ($2 == 0) {
                            $$ = NEW_ZARRAY(); /* zero length array*/
                        }
                        else {
                            $$ = $2;
                        }
                    }
                | tLBRACE assoc_list '}'
                    {
                        $$ = NEW_HASH($2);
                    }
                | kRETURN
                    {
                        $$ = NEW_RETURN(0);
                    }
                | kYIELD '(' call_args ')'
                    {
                        $$ = new_yield(vps, $3);
                    }
                | kYIELD '(' ')'
                    {
                        $$ = NEW_YIELD(0, Qfalse);
                    }
                | kYIELD
                    {
                        $$ = NEW_YIELD(0, Qfalse);
                    }
                | kDEFINED opt_nl '(' {vps->in_defined = 1;} expr ')'
                    {
                        vps->in_defined = 0;
                        $$ = NEW_DEFINED($5);
                    }
                | operation brace_block
                    {
                        $2->nd_iter = NEW_FCALL($1, 0);
                        $$ = $2;
                        fixpos($2->nd_iter, $2);
                    }
                | method_call
                | method_call brace_block
                    {
                        if ($1 && nd_type($1) == NODE_BLOCK_PASS) {
                            rb_compile_error(vps, "both block arg and actual block given");
                        }
                        $2->nd_iter = $1;
                        $$ = $2;
                        fixpos($$, $1);
                    }
                | kIF {
                    PUSH_LINE("if");
                  } expr_value then
                  compstmt
                  if_tail
                  kEND
                    {
                        POP_LINE();
                        $$ = NEW_IF(cond($3, vps), $5, $6);
                        fixpos($$, $3);
                        if (cond_negative(&$$->nd_cond)) {
                            NODE *tmp = $$->nd_body;
                            $$->nd_body = $$->nd_else;
                            $$->nd_else = tmp;
                        }
                    }
                | kUNLESS {
                    PUSH_LINE("unless");
                  } expr_value then
                  compstmt
                  opt_else
                  kEND
                    {
                        POP_LINE();
                        $$ = NEW_UNLESS(cond($3, vps), $5, $6);
                        fixpos($$, $3);
                        if (cond_negative(&$$->nd_cond)) {
                            NODE *tmp = $$->nd_body;
                            $$->nd_body = $$->nd_else;
                            $$->nd_else = tmp;
                        }
                    }
                | kWHILE {
                    PUSH_LINE("while");
                    COND_PUSH(1);
                  } expr_value do {COND_POP();}
                  compstmt
                  kEND
                    {
                        POP_LINE();
                        $$ = NEW_WHILE(cond($3, vps), $6, 1);
                        fixpos($$, $3);
                        if (cond_negative(&$$->nd_cond)) {
                            nd_set_type($$, NODE_UNTIL);
                        }
                    }
                | kUNTIL {
                    PUSH_LINE("until");
                    COND_PUSH(1);
                  } expr_value do {COND_POP();}
                  compstmt
                  kEND
                    {
                        POP_LINE();
                        $$ = NEW_UNTIL(cond($3, vps), $6, 1);
                        fixpos($$, $3);
                        if (cond_negative(&$$->nd_cond)) {
                            nd_set_type($$, NODE_WHILE);
                        }
                    }
                | kCASE {
                    PUSH_LINE("case");
                  } expr_value opt_terms
                  case_body
                  kEND
                    {
                        POP_LINE();
                        $$ = NEW_CASE($3, $5);
                        fixpos($$, $3);
                    }
                | kCASE opt_terms { 
                    push_start_line((rb_parse_state*)parse_state, ruby_sourceline - 1, "case");
                  } case_body kEND
                    {
                        POP_LINE();
                        $$ = $4;
                    }
                | kCASE opt_terms {
                    push_start_line((rb_parse_state*)parse_state, ruby_sourceline - 1, "case");
                  } kELSE compstmt kEND
                    {
                        POP_LINE();
                        $$ = $5;
                    }
                | kFOR {
                    PUSH_LINE("for");
                  } for_var kIN {COND_PUSH(1);} expr_value do {COND_POP();}
                  compstmt
                  kEND
                    {
                        POP_LINE();
                        $$ = NEW_FOR($3, $6, $9);
                        fixpos($$, $3);
                    }
                | kCLASS cpath superclass
                    {
                        PUSH_LINE("class");
                        if (in_def || in_single)
                            yyerror("class definition in method body");
                        class_nest++;
                        local_push(0);
                        $<num>$ = ruby_sourceline;
                    }
                  bodystmt
                  kEND
                    {
                        POP_LINE();
                        $$ = NEW_CLASS($2, $5, $3);
                        nd_set_line($$, $<num>4);
                        local_pop();
                        class_nest--;
                    }
                | kCLASS tLSHFT expr
                    {
                        PUSH_LINE("class");
                        $<num>$ = in_def;
                        in_def = 0;
                    }
                  term
                    {
                        $<num>$ = in_single;
                        in_single = 0;
                        class_nest++;
                        local_push(0);
                    }
                  bodystmt
                  kEND
                    {
                        POP_LINE();
                        $$ = NEW_SCLASS($3, $7);
                        fixpos($$, $3);
                        local_pop();
                        class_nest--;
                        in_def = $<num>4;
                        in_single = $<num>6;
                    }
                | kMODULE cpath
                    {
                        PUSH_LINE("module");
                        if (in_def || in_single)
                            yyerror("module definition in method body");
                        class_nest++;
                        local_push(0);
                        $<num>$ = ruby_sourceline;
                    }
                  bodystmt
                  kEND
                    {
                        POP_LINE();
                        $$ = NEW_MODULE($2, $4);
                        nd_set_line($$, $<num>3);
                        local_pop();
                        class_nest--;
                    }
                | kDEF fname
                    {
                        PUSH_LINE("def");
                        $<id>$ = cur_mid;
                        cur_mid = $2;
                        in_def++;
                        local_push(0);
                    }
                  f_arglist
                  bodystmt
                  kEND
                    {
                        POP_LINE();
                        if (!$5) $5 = NEW_NIL();
                        $$ = NEW_DEFN($2, $4, $5, NOEX_PRIVATE);
                        fixpos($$, $4);
                        local_pop();
                        in_def--;
                        cur_mid = $<id>3;
                    }
                | kDEF singleton dot_or_colon {vps->lex_state = EXPR_FNAME;} fname
                    {
                        PUSH_LINE("def");
                        in_single++;
                        local_push(0);
                        vps->lex_state = EXPR_END; /* force for args */
                    }
                  f_arglist
                  bodystmt
                  kEND
                    {
                        POP_LINE();
                        $$ = NEW_DEFS($2, $5, $7, $8);
                        fixpos($$, $2);
                        local_pop();
                        in_single--;
                    }
                | kBREAK
                    {
                        $$ = NEW_BREAK(0);
                    }
                | kNEXT
                    {
                        $$ = NEW_NEXT(0);
                    }
                | kREDO
                    {
                        $$ = NEW_REDO();
                    }
                | kRETRY
                    {
                        $$ = NEW_RETRY();
                    }
                ;

primary_value   : primary
                    {
                        value_expr($1);
                        $$ = $1;
                    }
                ;

then            : term
                | ':'
                | kTHEN
                | term kTHEN
                ;

do              : term
                | ':'
                | kDO_COND
                ;

if_tail         : opt_else
                | kELSIF expr_value then
                  compstmt
                  if_tail
                    {
                        $$ = NEW_IF(cond($2, vps), $4, $5);
                        fixpos($$, $2);
                    }
                ;

opt_else        : none
                | kELSE compstmt
                    {
                        $$ = $2;
                    }
                ;

for_var         : lhs
                | mlhs
                ;

block_par       : mlhs_item
                    {
                        $$ = NEW_LIST($1);
                    }
                | block_par ',' mlhs_item
                    {
                        $$ = list_append(vps, $1, $3);
                    }
                ;

block_var       : block_par
                    {
                        if ($1->nd_alen == 1) {
                            $$ = $1->nd_head;
                        }
                        else {
                            $$ = NEW_MASGN($1, 0);
                        }
                    }
                | block_par ','
                    {
                        $$ = NEW_MASGN($1, 0);
                    }
                | block_par ',' tAMPER lhs
                    {
                        $$ = NEW_BLOCK_VAR($4, NEW_MASGN($1, 0));
                    }
                | block_par ',' tSTAR lhs ',' tAMPER lhs
                    {
                        $$ = NEW_BLOCK_VAR($7, NEW_MASGN($1, $4));
                    }
                | block_par ',' tSTAR ',' tAMPER lhs
                    {
                        $$ = NEW_BLOCK_VAR($6, NEW_MASGN($1, -1));
                    }
                | block_par ',' tSTAR lhs
                    {
                        $$ = NEW_MASGN($1, $4);
                    }
                | block_par ',' tSTAR
                    {
                        $$ = NEW_MASGN($1, -1);
                    }
                | tSTAR lhs ',' tAMPER lhs
                    {
                        $$ = NEW_BLOCK_VAR($5, NEW_MASGN(0, $2));
                    }
                | tSTAR ',' tAMPER lhs
                    {
                        $$ = NEW_BLOCK_VAR($4, NEW_MASGN(0, -1));
                    }
                | tSTAR lhs
                    {
                        $$ = NEW_MASGN(0, $2);
                    }
                | tSTAR
                    {
                        $$ = NEW_MASGN(0, -1);
                    }
                | tAMPER lhs
                    {
                        $$ = NEW_BLOCK_VAR($2, (NODE*)1);
                    }
                ;

opt_block_var   : none
                | '|' /* none */ '|'
                    {
                        $$ = (NODE*)1;
                    }
                | tOROP
                    {
                        $$ = (NODE*)1;
                    }
                | '|' block_var '|'
                    {
                        $$ = $2;
                    }
                ;

do_block        : kDO_BLOCK
                    {
                        PUSH_LINE("do");
                        $<num>1 = ruby_sourceline;
                        reset_block(vps);
                    }
                  opt_block_var
                    {
                      $<vars>$ = vps->variables->block_vars;
                    }
                  compstmt
                  kEND
                    {
                        POP_LINE();
                        $$ = NEW_ITER($3, 0, extract_block_vars(vps, $5, $<vars>4));
                        nd_set_line($$, $<num>1);
                    }
                ;

block_call      : command do_block
                    {
                        if ($1 && nd_type($1) == NODE_BLOCK_PASS) {
                            rb_compile_error(vps, "both block arg and actual block given");
                        }
                        $2->nd_iter = $1;
                        $$ = $2;
                        fixpos($$, $1);
                    }
                | block_call '.' operation2 opt_paren_args
                    {
                        $$ = new_call(vps, $1, $3, $4);
                    }
                | block_call tCOLON2 operation2 opt_paren_args
                    {
                        $$ = new_call(vps, $1, $3, $4);
                    }
                ;

method_call     : operation paren_args
                    {
                        $$ = new_fcall(vps, $1, $2);
                        fixpos($$, $2);
                    }
                | primary_value '.' operation2 opt_paren_args
                    {
                        $$ = new_call(vps, $1, $3, $4);
                        fixpos($$, $1);
                    }
                | primary_value tCOLON2 operation2 paren_args
                    {
                        $$ = new_call(vps, $1, $3, $4);
                        fixpos($$, $1);
                    }
                | primary_value tCOLON2 operation3
                    {
                        $$ = new_call(vps, $1, $3, 0);
                    }
                | primary_value '\\' operation2
                    {
                        $$ = NEW_CALL($1, rb_parser_sym("get_reference"),
                        NEW_LIST(NEW_LIT(QUID2SYM($3))));
                    }
                | tUBS operation2
                    {
                        $$ = NEW_FCALL(rb_parser_sym("get_reference"),
                        NEW_LIST(NEW_LIT(QUID2SYM($2))));
                    }
                | kSUPER paren_args
                    {
                        $$ = new_super(vps, $2);
                    }
                | kSUPER
                    {
                        $$ = NEW_ZSUPER();
                    }
                ;

brace_block     : '{'
                    {
                        $<num>1 = ruby_sourceline;
                        reset_block(vps);
                    }
                  opt_block_var { $<vars>$ = vps->variables->block_vars; }
                  compstmt '}'
                    {
                        $$ = NEW_ITER($3, 0, extract_block_vars(vps, $5, $<vars>4));
                        nd_set_line($$, $<num>1);
                    }
                | kDO
                    {
                        PUSH_LINE("do");
                        $<num>1 = ruby_sourceline;
                        reset_block(vps);
                    }
                  opt_block_var { $<vars>$ = vps->variables->block_vars; }
                  compstmt kEND
                    {
                        POP_LINE();
                        $$ = NEW_ITER($3, 0, extract_block_vars(vps, $5, $<vars>4));
                        nd_set_line($$, $<num>1);
                    }
                ;

case_body       : kWHEN when_args then
                  compstmt
                  cases
                    {
                        $$ = NEW_WHEN($2, $4, $5);
                    }
                ;
when_args       : args
                | args ',' tSTAR arg_value
                    {
                        $$ = list_append(vps, $1, NEW_WHEN($4, 0, 0));
                    }
                | tSTAR arg_value
                    {
                        $$ = NEW_LIST(NEW_WHEN($2, 0, 0));
                    }
                ;

cases           : opt_else
                | case_body
                ;

opt_rescue      : kRESCUE exc_list exc_var then
                  compstmt
                  opt_rescue
                    {
                        if ($3) {
                            $3 = node_assign($3, NEW_GVAR(rb_parser_sym("$!")), vps);
                            $5 = block_append(vps, $3, $5);
                        }
                        $$ = NEW_RESBODY($2, $5, $6);
                        fixpos($$, $2?$2:$5);
                    }
                | none
                ;

exc_list        : arg_value
                    {
                        $$ = NEW_LIST($1);
                    }
                | mrhs
                | none
                ;

exc_var         : tASSOC lhs
                    {
                        $$ = $2;
                    }
                | none
                ;

opt_ensure      : kENSURE compstmt
                    {
                        if ($2)
                            $$ = $2;
                        else
                            /* place holder */
                            $$ = NEW_NIL();
                    }
                | none
                ;

literal         : numeric
                | symbol
                    {
                        $$ = NEW_LIT(QUID2SYM($1));
                    }
                | dsym
                ;

strings         : string
                    {
                        NODE *node = $1;
                        if (!node) {
                            node = NEW_STR(string_new(0, 0));
                        }
                        else {
                            node = evstr2dstr(vps, node);
                        }
                        $$ = node;
                    }
                ;

string          : string1
                | string string1
                    {
                        $$ = literal_concat(vps, $1, $2);
                    }
                ;

string1         : tSTRING_BEG string_contents tSTRING_END
                    {
                        $$ = $2;
                    }
                ;

xstring         : tXSTRING_BEG xstring_contents tSTRING_END
                    {
                        NODE *node = $2;
                        if (!node) {
                            node = NEW_XSTR(string_new(0, 0));
                        }
                        else {
                            switch (nd_type(node)) {
                              case NODE_STR:
                                nd_set_type(node, NODE_XSTR);
                                break;
                              case NODE_DSTR:
                                nd_set_type(node, NODE_DXSTR);
                                break;
                              default:
                                node = NEW_NODE(NODE_DXSTR, string_new(0, 0), 1, NEW_LIST(node));
                                break;
                            }
                        }
                        $$ = node;
                    }
                ;

regexp          : tREGEXP_BEG xstring_contents tREGEXP_END
                    {
                        intptr_t options = $3;
                        NODE *node = $2;
                        if (!node) {
                            node = NEW_REGEX(string_new2(""), options & ~RE_OPTION_ONCE);
                        }
                        else switch (nd_type(node)) {
                          case NODE_STR:
                            {
                                nd_set_type(node, NODE_REGEX);
                                node->nd_cnt = options & ~RE_OPTION_ONCE;
                            }
                            break;
                          default:
                            node = NEW_NODE(NODE_DSTR, string_new(0, 0), 1, NEW_LIST(node));
                          case NODE_DSTR:
                            if (options & RE_OPTION_ONCE) {
                                nd_set_type(node, NODE_DREGX_ONCE);
                            }
                            else {
                                nd_set_type(node, NODE_DREGX);
                            }
                            node->nd_cflag = options & ~RE_OPTION_ONCE;
                            break;
                        }
                        $$ = node;
                    }
                ;

words           : tWORDS_BEG ' ' tSTRING_END
                    {
                        $$ = NEW_ZARRAY();
                    }
                | tWORDS_BEG word_list tSTRING_END
                    {
                        $$ = $2;
                    }
                ;

word_list       : /* none */
                    {
                        $$ = 0;
                    }
                | word_list word ' '
                    {
                        $$ = list_append(vps, $1, evstr2dstr(vps, $2));
                    }
                ;

word            : string_content
                | word string_content
                    {
                        $$ = literal_concat(vps, $1, $2);
                    }
                ;

qwords          : tQWORDS_BEG ' ' tSTRING_END
                    {
                        $$ = NEW_ZARRAY();
                    }
                | tQWORDS_BEG qword_list tSTRING_END
                    {
                        $$ = $2;
                    }
                ;

qword_list      : /* none */
                    {
                        $$ = 0;
                    }
                | qword_list tSTRING_CONTENT ' '
                    {
                        $$ = list_append(vps, $1, $2);
                    }
                ;

string_contents : /* none */
                    {
                        $$ = 0;
                    }
                | string_contents string_content
                    {
                        $$ = literal_concat(vps, $1, $2);
                    }
                ;

xstring_contents: /* none */
                    {
                        $$ = 0;
                    }
                | xstring_contents string_content
                    {
                        $$ = literal_concat(vps, $1, $2);
                    }
                ;

string_content  : tSTRING_CONTENT
                | tSTRING_DVAR
                    {
                        $<node>$ = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                    }
                  string_dvar
                    {
                        lex_strterm = $<node>2;
                        $$ = NEW_EVSTR($3);
                    }
                | tSTRING_DBEG
                    {
                        $<node>$ = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                        COND_PUSH(0);
                        CMDARG_PUSH(0);
                    }
                  compstmt '}'
                    {
                        lex_strterm = $<node>2;
                        COND_LEXPOP();
                        CMDARG_LEXPOP();
                        if (($$ = $3) && nd_type($$) == NODE_NEWLINE) {
                            $$ = $$->nd_next;
                        }
                        $$ = new_evstr(vps, $$);
                    }
                ;

string_dvar     : tGVAR {$$ = NEW_GVAR($1);}
                | tIVAR {$$ = NEW_IVAR($1);}
                | tCVAR {$$ = NEW_CVAR($1);}
                | backref
                ;

symbol          : tSYMBEG sym
                    {
                        vps->lex_state = EXPR_END;
                        $$ = $2;
                    }
                ;

sym             : fname
                | tIVAR
                | tGVAR
                | tCVAR
                ;

dsym            : tSYMBEG xstring_contents tSTRING_END
                    {
                        vps->lex_state = EXPR_END;
                        if (!($$ = $2)) {
                            yyerror("empty symbol literal");
                        }
                        else {
                            switch (nd_type($$)) {
                              case NODE_DSTR:
                                nd_set_type($$, NODE_DSYM);
                                break;
                              case NODE_STR:
                                /* TODO: this line should never fail unless nd_str is binary */
                                if (strlen(bdatae($$->nd_str,"")) == (size_t)blength($$->nd_str)) {
                                  QUID tmp = rb_parser_sym(bdata($$->nd_str));
                                  bdestroy($$->nd_str);
                                  $$->nd_lit = QUID2SYM(tmp);
                                  nd_set_type($$, NODE_LIT);
                                  break;
                                } else {
                                  bdestroy($$->nd_str);
                                }
                                /* fall through */
                              default:
                                $$ = NEW_NODE(NODE_DSYM, string_new(0, 0), 1, NEW_LIST($$));
                                break;
                            }
                        }
                    }
                ;

numeric         : tINTEGER
                | tFLOAT
                | tUMINUS_NUM tINTEGER         %prec tLOWEST
                    {
                        $$ = NEW_NEGATE($2);
                    }
                | tUMINUS_NUM tFLOAT           %prec tLOWEST
                    {
                        $$ = NEW_NEGATE($2);
                    }
                ;

variable        : tIDENTIFIER
                | tIVAR
                | tGVAR
                | tCONSTANT
                | tCVAR
                | kNIL {$$ = kNIL;}
                | kSELF {$$ = kSELF;}
                | kTRUE {$$ = kTRUE;}
                | kFALSE {$$ = kFALSE;}
                | k__FILE__ {$$ = k__FILE__;}
                | k__LINE__ {$$ = k__LINE__;}
                ;

var_ref         : variable
                    {
                        $$ = gettable($1);
                    }
                ;

var_lhs         : variable
                    {
                        $$ = assignable($1, 0, vps);
                    }
                ;

backref         : tNTH_REF
                | tBACK_REF
                ;

superclass      : term
                    {
                        $$ = 0;
                    }
                | '<'
                    {
                        vps->lex_state = EXPR_BEG;
                    }
                  expr_value term
                    {
                        $$ = $3;
                    }
                | error term {yyerrok; $$ = 0;}
                ;

f_arglist       : '(' f_args opt_nl ')'
                    {
                        $$ = $2;
                        vps->lex_state = EXPR_BEG;
                        command_start = TRUE;
                    }
                | f_args term
                    {
                        $$ = $1;
                    }
                ;

f_args          : f_arg ',' f_optarg ',' f_rest_arg opt_f_block_arg
                    {
                        $$ = block_append(vps, NEW_ARGS((intptr_t)$1, $3, $5), $6);
                    }
                | f_arg ',' f_optarg opt_f_block_arg
                    {
                        $$ = block_append(vps, NEW_ARGS((intptr_t)$1, $3, 0), $4);
                    }
                | f_arg ',' f_rest_arg opt_f_block_arg
                    {
                        $$ = block_append(vps, NEW_ARGS((intptr_t)$1, 0, $3), $4);
                    }
                | f_arg opt_f_block_arg
                    {
                        $$ = block_append(vps, NEW_ARGS((intptr_t)$1, 0, 0), $2);
                    }
                | f_optarg ',' f_rest_arg opt_f_block_arg
                    {
                        $$ = block_append(vps, NEW_ARGS(0, $1, $3), $4);
                    }
                | f_optarg opt_f_block_arg
                    {
                        $$ = block_append(vps, NEW_ARGS(0, $1, 0), $2);
                    }
                | f_rest_arg opt_f_block_arg
                    {
                        $$ = block_append(vps, NEW_ARGS(0, 0, $1), $2);
                    }
                | f_block_arg
                    {
                        $$ = block_append(vps, NEW_ARGS(0, 0, 0), $1);
                    }
                | /* none */
                    {
                        $$ = NEW_ARGS(0, 0, 0);
                    }
                ;

f_norm_arg      : tCONSTANT
                    {
                        yyerror("formal argument cannot be a constant");
                    }
                | tIVAR
                    {
                        yyerror("formal argument cannot be an instance variable");
                    }
                | tGVAR
                    {
                        yyerror("formal argument cannot be a global variable");
                    }
                | tCVAR
                    {
                        yyerror("formal argument cannot be a class variable");
                    }
                | tIDENTIFIER
                    {
                        if (!is_local_id($1))
                            yyerror("formal argument must be local variable");
                        else if (local_id($1))
                            yyerror("duplicate argument name");
                        local_cnt($1);
                        $$ = 1;
                    }
                ;

f_arg           : f_norm_arg
                | f_arg ',' f_norm_arg
                    {
                        $$ += 1;
                    }
                ;

f_opt           : tIDENTIFIER '=' arg_value
                    {
                        if (!is_local_id($1))
                            yyerror("formal argument must be local variable");
                        else if (local_id($1))
                            yyerror("duplicate optional argument name");
                        $$ = assignable($1, $3, vps);
                    }
                ;

f_optarg        : f_opt
                    {
                        $$ = NEW_BLOCK($1);
                        $$->nd_end = $$;
                    }
                | f_optarg ',' f_opt
                    {
                        $$ = block_append(vps, $1, $3);
                    }
                ;

restarg_mark    : '*'
                | tSTAR
                ;

f_rest_arg      : restarg_mark tIDENTIFIER
                    {
                        if (!is_local_id($2))
                            yyerror("rest argument must be local variable");
                        else if (local_id($2))
                            yyerror("duplicate rest argument name");
                        $$ = local_cnt($2) + 1;
                    }
                | restarg_mark
                    {
                        $$ = -2;
                    }
                ;

blkarg_mark     : '&'
                | tAMPER
                ;

f_block_arg     : blkarg_mark tIDENTIFIER
                    {
                        if (!is_local_id($2))
                            yyerror("block argument must be local variable");
                        else if (local_id($2))
                            yyerror("duplicate block argument name");
                        $$ = NEW_BLOCK_ARG($2);
                    }
                ;

opt_f_block_arg : ',' f_block_arg
                    {
                        $$ = $2;
                    }
                | none
                ;

singleton       : var_ref
                    {
                        $$ = $1;
                        value_expr($$);
                    }
                | '(' {vps->lex_state = EXPR_BEG;} expr opt_nl ')'
                    {
                        if ($3 == 0) {
                            yyerror("can't define singleton method for ().");
                        }
                        else {
                            switch (nd_type($3)) {
                              case NODE_STR:
                              case NODE_DSTR:
                              case NODE_XSTR:
                              case NODE_DXSTR:
                              case NODE_DREGX:
                              case NODE_LIT:
                              case NODE_ARRAY:
                              case NODE_ZARRAY:
                                yyerror("can't define singleton method for literals");
                              default:
                                value_expr($3);
                                break;
                            }
                        }
                        $$ = $3;
                    }
                ;

assoc_list      : none
                | assocs trailer
                    {
                        $$ = $1;
                    }
                | args trailer
                    {
                        if ($1->nd_alen%2 != 0) {
                            yyerror("odd number list for Hash");
                        }
                        $$ = $1;
                    }
                ;

assocs          : assoc
                | assocs ',' assoc
                    {
                        $$ = list_concat($1, $3);
                    }
                ;

assoc           : arg_value tASSOC arg_value
                    {
                        $$ = list_append(vps, NEW_LIST($1), $3);
                    }
                ;

operation       : tIDENTIFIER
                | tCONSTANT
                | tFID
                ;

operation2      : tIDENTIFIER
                | tCONSTANT
                | tFID
                | op
                ;

operation3      : tIDENTIFIER
                | tFID
                | op
                ;

dot_or_colon    : '.'
                | tCOLON2
                ;

opt_terms       : /* none */
                | terms
                ;

opt_nl          : /* none */
                | '\n'
                ;

trailer         : /* none */
                | '\n'
                | ','
                ;

term            : ';' {yyerrok;}
                | '\n'
                ;

terms           : term
                | terms ';' {yyerrok;}
                ;

none            : /* none */ {$$ = 0;}
                ;
%%

/* We remove any previous definition of `SIGN_EXTEND_CHAR',
   since ours (we hope) works properly with all combinations of
   machines, compilers, `char' and `unsigned char' argument types.
   (Per Bothner suggested the basic approach.)  */
#undef SIGN_EXTEND_CHAR
#if __STDC__
# define SIGN_EXTEND_CHAR(c) ((signed char)(c))
#else  /* not __STDC__ */
/* As in Harbison and Steele.  */
# define SIGN_EXTEND_CHAR(c) ((((unsigned char)(c)) ^ 128) - 128)
#endif
#define is_identchar(c) (SIGN_EXTEND_CHAR(c)!=-1&&(ISALNUM(c) || (c) == '_' || ismbchar(c)))

#define LEAVE_BS 1

static int
mel_yyerror(const char *msg, rb_parse_state *parse_state)
{
    create_error(parse_state, (char *)msg);

    return 1;
}

static int
yycompile(rb_parse_state *parse_state, char *f, int line)
{
    int n;
    /* Setup an initial empty scope. */
    heredoc_end = 0;
    lex_strterm = 0;
    parse_state->end_seen = 0;
    ruby_sourcefile = f;
    command_start = TRUE;
    n = yyparse(parse_state);
    ruby_debug_lines = 0;
    compile_for_eval = 0;
    parse_state->cond_stack = 0;
    parse_state->cmdarg_stack = 0;
    command_start = TRUE;
    class_nest = 0;
    in_single = 0;
    in_def = 0;
    cur_mid = 0;

    lex_strterm = 0;

    return n;
}

static bool
lex_get_str(rb_parse_state *parse_state)
{
    const char *str;
    const char *beg, *end, *pend;
    int sz;

    str = bdata(parse_state->lex_string);
    beg = str;

    if (parse_state->lex_str_used) {
      if (blength(parse_state->lex_string) == parse_state->lex_str_used) {
        return false;
      }

      beg += parse_state->lex_str_used;
    }

    pend = str + blength(parse_state->lex_string);
    end = beg;

    while(end < pend) {
      if(*end++ == '\n') break;
    }

    sz = end - beg;
    bcatblk(parse_state->line_buffer, beg, sz);
    parse_state->lex_str_used += sz;

    return TRUE;
}

static bool
lex_getline(rb_parse_state *parse_state)
{
  if(!parse_state->line_buffer) {
    parse_state->line_buffer = cstr2bstr("");
  } else {
    btrunc(parse_state->line_buffer, 0);
  }

  return parse_state->lex_gets(parse_state);
}

VALUE process_parse_tree(rb_parse_state*, VALUE, NODE*, QUID*);

VALUE
string_to_ast(VALUE ptp, const char *f, bstring s, int line)
{
    int n;
    rb_parse_state *parse_state;
    VALUE ret;
    parse_state = alloc_parse_state();
    parse_state->lex_string = s;
    parse_state->lex_gets = lex_get_str;
    parse_state->lex_pbeg = 0;
    parse_state->lex_p = 0;
    parse_state->lex_pend = 0;
    parse_state->error = Qfalse;
    parse_state->processor = ptp;
    ruby_sourceline = line - 1;
    compile_for_eval = 1;

    n = yycompile(parse_state, (char*)f, line);

    if(parse_state->error == Qfalse) {
      for(std::vector<bstring>::iterator i = parse_state->magic_comments->begin();
          i != parse_state->magic_comments->end();
          i++) {
        rb_funcall(ptp, rb_intern("add_magic_comment"), 1,
          rb_str_new((const char*)(*i)->data, (*i)->slen));
      }
      ret = process_parse_tree(parse_state, ptp, parse_state->top, NULL);
    } else {
        ret = Qnil;
    }
    pt_free(parse_state);
    free(parse_state);
    return ret;
}

static bool parse_io_gets(rb_parse_state *parse_state) {
  if(feof(parse_state->lex_io)) {
    return false;
  }

  while(TRUE) {
    char *ptr, buf[1024];
    int read;

    ptr = fgets(buf, sizeof(buf), parse_state->lex_io);
    if(!ptr) {
      return false;
    }

    read = strlen(ptr);
    bcatblk(parse_state->line_buffer, ptr, read);

    /* check whether we read a full line */
    if(!(read == (sizeof(buf) - 1) && ptr[read] != '\n')) {
      break;
    }
  }

  return TRUE;
}

VALUE
file_to_ast(VALUE ptp, const char *f, FILE *file, int start)
{
    int n;
    VALUE ret;
    rb_parse_state *parse_state;
    parse_state = alloc_parse_state();
    parse_state->lex_io = file;
    parse_state->lex_gets = parse_io_gets;
    parse_state->lex_pbeg = 0;
    parse_state->lex_p = 0;
    parse_state->lex_pend = 0;
    parse_state->error = Qfalse;
    parse_state->processor = ptp;
    ruby_sourceline = start - 1;

    n = yycompile(parse_state, (char*)f, start);

    if(parse_state->error == Qfalse) {
      for(std::vector<bstring>::iterator i = parse_state->magic_comments->begin();
          i != parse_state->magic_comments->end();
          i++) {
        rb_funcall(ptp, rb_intern("add_magic_comment"), 1,
          rb_str_new((const char*)(*i)->data, (*i)->slen));
      }
        ret = process_parse_tree(parse_state, ptp, parse_state->top, NULL);

        if (parse_state->end_seen && parse_state->lex_io) {
          rb_funcall(ptp, rb_sData, 1, ULONG2NUM(ftell(parse_state->lex_io)));
        }
    } else {
        ret = Qnil;
    }

    pt_free(parse_state);
    free(parse_state);
    return ret;
}

#define nextc() ps_nextc(parse_state)

static inline int
ps_nextc(rb_parse_state *parse_state)
{
    int c;

    if (parse_state->lex_p == parse_state->lex_pend) {
        bstring v;

        if (!lex_getline(parse_state)) return -1;
        v = parse_state->line_buffer;

        if (heredoc_end > 0) {
            ruby_sourceline = heredoc_end;
            heredoc_end = 0;
        }
        ruby_sourceline++;

        /* This code is setup so that lex_pend can be compared to
           the data in lex_lastline. Thats important, otherwise
           the heredoc code breaks. */
        if(parse_state->lex_lastline) {
          bassign(parse_state->lex_lastline, v);
        } else {
          parse_state->lex_lastline = bstrcpy(v);
        }

        v = parse_state->lex_lastline;

        parse_state->lex_pbeg = parse_state->lex_p = bdata(v);
        parse_state->lex_pend = parse_state->lex_p + blength(v);
    }
    c = (unsigned char)*(parse_state->lex_p++);
    if (c == '\r' && parse_state->lex_p < parse_state->lex_pend && *(parse_state->lex_p) == '\n') {
        parse_state->lex_p++;
        c = '\n';
        parse_state->column = 0;
    } else if(c == '\n') {
        parse_state->column = 0;
    } else {
        parse_state->column++;
    }

    return c;
}

static void
pushback(int c, rb_parse_state *parse_state)
{
    if (c == -1) return;
    parse_state->lex_p--;
}

/* Indicates if we're currently at the beginning of a line. */
#define was_bol() (parse_state->lex_p == parse_state->lex_pbeg + 1)
#define peek(c) (parse_state->lex_p != parse_state->lex_pend && (c) == *(parse_state->lex_p))

/* The token buffer. It's just a global string that has
   functions to build up the string easily. */

#define tokfix() (tokenbuf[tokidx]='\0')
#define tok() tokenbuf
#define toklen() tokidx
#define toklast() (tokidx>0?tokenbuf[tokidx-1]:0)

static char*
newtok(rb_parse_state *parse_state)
{
    tokidx = 0;
    if (!tokenbuf) {
        toksiz = 60;
        tokenbuf = ALLOC_N(char, 60);
    }
    if (toksiz > 4096) {
        toksiz = 60;
        REALLOC_N(tokenbuf, char, 60);
    }
    return tokenbuf;
}

static void tokadd(char c, rb_parse_state *parse_state)
{
    assert(tokidx < toksiz && tokidx >= 0);
    tokenbuf[tokidx++] = c;
    if (tokidx >= toksiz) {
        toksiz *= 2;
        REALLOC_N(tokenbuf, char, toksiz);
    }
}

static int
read_escape(rb_parse_state *parse_state)
{
    int c;

    switch (c = nextc()) {
      case '\\':        /* Backslash */
        return c;

      case 'n': /* newline */
        return '\n';

      case 't': /* horizontal tab */
        return '\t';

      case 'r': /* carriage-return */
        return '\r';

      case 'f': /* form-feed */
        return '\f';

      case 'v': /* vertical tab */
        return '\13';

      case 'a': /* alarm(bell) */
        return '\007';

      case 'e': /* escape */
        return 033;

      case '0': case '1': case '2': case '3': /* octal constant */
      case '4': case '5': case '6': case '7':
        {
            int numlen;

            pushback(c, parse_state);
            c = scan_oct(parse_state->lex_p, 3, &numlen);
            parse_state->lex_p += numlen;
        }
        return c;

      case 'x': /* hex constant */
        {
            int numlen;

            c = scan_hex(parse_state->lex_p, 2, &numlen);
            if (numlen == 0) {
                yyerror("Invalid escape character syntax");
                return 0;
            }
            parse_state->lex_p += numlen;
        }
        return c;

      case 'b': /* backspace */
        return '\010';

      case 's': /* space */
        return ' ';

      case 'M':
        if ((c = nextc()) != '-') {
            yyerror("Invalid escape character syntax");
            pushback(c, parse_state);
            return '\0';
        }
        if ((c = nextc()) == '\\') {
            return read_escape(parse_state) | 0x80;
        }
        else if (c == -1) goto eof;
        else {
            return ((c & 0xff) | 0x80);
        }

      case 'C':
        if ((c = nextc()) != '-') {
            yyerror("Invalid escape character syntax");
            pushback(c, parse_state);
            return '\0';
        }
      case 'c':
        if ((c = nextc())== '\\') {
            c = read_escape(parse_state);
        }
        else if (c == '?')
            return 0177;
        else if (c == -1) goto eof;
        return c & 0x9f;

      eof:
      case -1:
        yyerror("Invalid escape character syntax");
        return '\0';

      default:
        return c;
    }
}

static int
tokadd_escape(int term, rb_parse_state *parse_state)
{
    int c;

    switch (c = nextc()) {
      case '\n':
        return 0;               /* just ignore */

      case '0': case '1': case '2': case '3': /* octal constant */
      case '4': case '5': case '6': case '7':
        {
            int i;

            tokadd((char)'\\', parse_state);
            tokadd((char)c, parse_state);
            for (i=0; i<2; i++) {
                c = nextc();
                if (c == -1) goto eof;
                if (c < '0' || '7' < c) {
                    pushback(c, parse_state);
                    break;
                }
                tokadd((char)c, parse_state);
            }
        }
        return 0;

      case 'x': /* hex constant */
        {
            int numlen;

            tokadd('\\', parse_state);
            tokadd((char)c, parse_state);
            scan_hex(parse_state->lex_p, 2, &numlen);
            if (numlen == 0) {
                yyerror("Invalid escape character syntax");
                return -1;
            }
            while (numlen--)
                tokadd((char)nextc(), parse_state);
        }
        return 0;

      case 'M':
        if ((c = nextc()) != '-') {
            yyerror("Invalid escape character syntax");
            pushback(c, parse_state);
            return 0;
        }
        tokadd('\\',parse_state);
        tokadd('M', parse_state);
        tokadd('-', parse_state);
        goto escaped;

      case 'C':
        if ((c = nextc()) != '-') {
            yyerror("Invalid escape character syntax");
            pushback(c, parse_state);
            return 0;
        }
        tokadd('\\', parse_state);
        tokadd('C', parse_state);
        tokadd('-', parse_state);
        goto escaped;

      case 'c':
        tokadd('\\', parse_state);
        tokadd('c', parse_state);
      escaped:
        if ((c = nextc()) == '\\') {
            return tokadd_escape(term, parse_state);
        }
        else if (c == -1) goto eof;
        tokadd((char)c, parse_state);
        return 0;

      eof:
      case -1:
        yyerror("Invalid escape character syntax");
        return -1;

      default:
        if (c != '\\' || c != term)
            tokadd('\\', parse_state);
        tokadd((char)c, parse_state);
    }
    return 0;
}

static int
regx_options(rb_parse_state *parse_state)
{
    char kcode = 0;
    int options = 0;
    int c;

    newtok(parse_state);
    while (c = nextc(), ISALPHA(c)) {
        switch (c) {
          case 'i':
            options |= RE_OPTION_IGNORECASE;
            break;
          case 'x':
            options |= RE_OPTION_EXTENDED;
            break;
          case 'm':
            options |= RE_OPTION_MULTILINE;
            break;
          case 'o':
            options |= RE_OPTION_ONCE;
            break;
          case 'G':
            options |= RE_OPTION_CAPTURE_GROUP;
            break;
          case 'g':
            options |= RE_OPTION_DONT_CAPTURE_GROUP;
            break;
          case 'n':
            kcode = 16;
            break;
          case 'e':
            kcode = 32;
            break;
          case 's':
            kcode = 48;
            break;
          case 'u':
            kcode = 64;
            break;
          default:
            tokadd((char)c, parse_state);
            break;
        }
    }
    pushback(c, parse_state);
    if (toklen()) {
        tokfix();
        rb_compile_error(parse_state, "unknown regexp option%s - %s",
                         toklen() > 1 ? "s" : "", tok());
    }
    return options | kcode;
}

#define STR_FUNC_ESCAPE 0x01
#define STR_FUNC_EXPAND 0x02
#define STR_FUNC_REGEXP 0x04
#define STR_FUNC_QWORDS 0x08
#define STR_FUNC_SYMBOL 0x10
#define STR_FUNC_INDENT 0x20

enum string_type {
    str_squote = (0),
    str_dquote = (STR_FUNC_EXPAND),
    str_xquote = (STR_FUNC_EXPAND),
    str_regexp = (STR_FUNC_REGEXP|STR_FUNC_ESCAPE|STR_FUNC_EXPAND),
    str_sword  = (STR_FUNC_QWORDS),
    str_dword  = (STR_FUNC_QWORDS|STR_FUNC_EXPAND),
    str_ssym   = (STR_FUNC_SYMBOL),
    str_dsym   = (STR_FUNC_SYMBOL|STR_FUNC_EXPAND),
};

static int tokadd_string(int func, int term, int paren, quark *nest, rb_parse_state *parse_state)
{
    int c;

    while ((c = nextc()) != -1) {
        if (paren && c == paren) {
            ++*nest;
        }
        else if (c == term) {
            if (!nest || !*nest) {
                pushback(c, parse_state);
                break;
            }
            --*nest;
        }
        else if ((func & STR_FUNC_EXPAND) && c == '#' && parse_state->lex_p < parse_state->lex_pend) {
            int c2 = *(parse_state->lex_p);
            if (c2 == '$' || c2 == '@' || c2 == '{') {
                pushback(c, parse_state);
                break;
            }
        }
        else if (c == '\\') {
            c = nextc();
            switch (c) {
              case '\n':
                if (func & STR_FUNC_QWORDS) break;
                if (func & STR_FUNC_EXPAND) continue;
                tokadd('\\', parse_state);
                break;

              case '\\':
                if (func & STR_FUNC_ESCAPE) tokadd((char)c, parse_state);
                break;

              default:
                if (func & STR_FUNC_REGEXP) {
                    pushback(c, parse_state);
                    if (tokadd_escape(term, parse_state) < 0)
                        return -1;
                    continue;
                }
                else if (func & STR_FUNC_EXPAND) {
                    pushback(c, parse_state);
                    if (func & STR_FUNC_ESCAPE) tokadd('\\', parse_state);
                    c = read_escape(parse_state);
                }
                else if ((func & STR_FUNC_QWORDS) && ISSPACE(c)) {
                    /* ignore backslashed spaces in %w */
                }
                else if (c != term && !(paren && c == paren)) {
                    tokadd('\\', parse_state);
                }
            }
        }
        else if (ismbchar(c)) {
            int i, len = mbclen(c)-1;

            for (i = 0; i < len; i++) {
                tokadd((char)c, parse_state);
                c = nextc();
            }
        }
        else if ((func & STR_FUNC_QWORDS) && ISSPACE(c)) {
            pushback(c, parse_state);
            break;
        }
        if (!c && (func & STR_FUNC_SYMBOL)) {
            func &= ~STR_FUNC_SYMBOL;
            rb_compile_error(parse_state, "symbol cannot contain '\\0'");
            continue;
        }
        tokadd((char)c, parse_state);
    }
    return c;
}

#define NEW_STRTERM(func, term, paren) \
  node_newnode(parse_state, NODE_STRTERM, (VALUE)(func), \
               (VALUE)((term) | ((paren) << (CHAR_BIT * 2))), NULL)
#define pslval ((YYSTYPE *)parse_state->lval)
static int
parse_string(NODE *quote, rb_parse_state *parse_state)
{
    int func = quote->nd_func;
    int term = nd_term(quote);
    int paren = nd_paren(quote);
    int c, space = 0;

    if (func == -1) return tSTRING_END;
    c = nextc();
    if ((func & STR_FUNC_QWORDS) && ISSPACE(c)) {
        do {c = nextc();} while (ISSPACE(c));
        space = 1;
    }
    if (c == term && !quote->nd_nest) {
        if (func & STR_FUNC_QWORDS) {
            quote->nd_func = -1;
            return ' ';
        }
        if (!(func & STR_FUNC_REGEXP)) return tSTRING_END;
        pslval->num = regx_options(parse_state);
        return tREGEXP_END;
    }
    if (space) {
        pushback(c, parse_state);
        return ' ';
    }
    newtok(parse_state);
    if ((func & STR_FUNC_EXPAND) && c == '#') {
        switch (c = nextc()) {
          case '$':
          case '@':
            pushback(c, parse_state);
            return tSTRING_DVAR;
          case '{':
            return tSTRING_DBEG;
        }
        tokadd('#', parse_state);
    }
    pushback(c, parse_state);
    if (tokadd_string(func, term, paren, &quote->nd_nest, parse_state) == -1) {
        ruby_sourceline = nd_line(quote);
        rb_compile_error(parse_state, "unterminated string meets end of file");
        return tSTRING_END;
    }

    tokfix();
    pslval->node = NEW_STR(string_new(tok(), toklen()));
    return tSTRING_CONTENT;
}

/* Called when the lexer detects a heredoc is beginning. This pulls
   in more characters and detects what kind of heredoc it is. */
static int
heredoc_identifier(rb_parse_state *parse_state)
{
    int c = nextc(), term, func = 0;
    size_t len;

    if (c == '-') {
        c = nextc();
        func = STR_FUNC_INDENT;
    }
    switch (c) {
      case '\'':
        func |= str_squote; goto quoted;
      case '"':
        func |= str_dquote; goto quoted;
      case '`':
        func |= str_xquote;
      quoted:
        /* The heredoc indent is quoted, so its easy to find, we just
           continue to consume characters into the token buffer until
           we hit the terminating character. */

        newtok(parse_state);
        tokadd((char)func, parse_state);
        term = c;

        /* Where of where has the term gone.. */
        while ((c = nextc()) != -1 && c != term) {
            len = mbclen(c);
            do {
              tokadd((char)c, parse_state);
            } while (--len > 0 && (c = nextc()) != -1);
        }
        /* Ack! end of file or end of string. */
        if (c == -1) {
            rb_compile_error(parse_state, "unterminated here document identifier");
            return 0;
        }

        break;

      default:
        /* Ok, this is an unquoted heredoc ident. We just consume
           until we hit a non-ident character. */

        /* Do a quick check that first character is actually valid.
           if it's not, then this isn't actually a heredoc at all!
           It sucks that it's way down here in this function that in
           finally bails with this not being a heredoc.*/

        if (!is_identchar(c)) {
            pushback(c, parse_state);
            if (func & STR_FUNC_INDENT) {
                pushback('-', parse_state);
            }
            return 0;
        }

        /* Finally, setup the token buffer and begin to fill it. */
        newtok(parse_state);
        term = '"';
        tokadd((char)(func |= str_dquote), parse_state);
        do {
            len = mbclen(c);
            do { tokadd((char)c, parse_state); } while (--len > 0 && (c = nextc()) != -1);
        } while ((c = nextc()) != -1 && is_identchar(c));
        pushback(c, parse_state);
        break;
    }


    /* Fixup the token buffer, ie set the last character to null. */
    tokfix();
    len = parse_state->lex_p - parse_state->lex_pbeg;
    parse_state->lex_p = parse_state->lex_pend;
    pslval->id = 0;

    /* Tell the lexer that we're inside a string now. nd_lit is
       the heredoc identifier that we watch the stream for to
       detect the end of the heredoc. */
    bstring str = bstrcpy(parse_state->lex_lastline);
    lex_strterm = node_newnode(parse_state, NODE_HEREDOC,
                               (VALUE)string_new(tok(), toklen()),  /* nd_lit */
                               (VALUE)len,                          /* nd_nth */
                               (VALUE)str);                         /* nd_orig */
    return term == '`' ? tXSTRING_BEG : tSTRING_BEG;
}

static void
heredoc_restore(NODE *here, rb_parse_state *parse_state)
{
    bstring line = here->nd_orig;

    bdestroy(parse_state->lex_lastline);

    parse_state->lex_lastline = line;
    parse_state->lex_pbeg = bdata(line);
    parse_state->lex_pend = parse_state->lex_pbeg + blength(line);
    parse_state->lex_p = parse_state->lex_pbeg + here->nd_nth;
    heredoc_end = ruby_sourceline;
    ruby_sourceline = nd_line(here);
    bdestroy((bstring)here->nd_lit);
}

static int
whole_match_p(const char *eos, int len, int indent, rb_parse_state *parse_state)
{
    char *p = parse_state->lex_pbeg;
    int n;

    if (indent) {
        while (*p && ISSPACE(*p)) p++;
    }
    n = parse_state->lex_pend - (p + len);
    if (n < 0 || (n > 0 && p[len] != '\n' && p[len] != '\r')) return FALSE;
    if (strncmp(eos, p, len) == 0) return TRUE;
    return FALSE;
}

/* Called when the lexer knows it's inside a heredoc. This function
   is responsible for detecting an expandions (ie #{}) in the heredoc
   and emitting a lex token and also detecting the end of the heredoc. */

static int
here_document(NODE *here, rb_parse_state *parse_state)
{
    int c, func, indent = 0;
    char *eos, *p, *pend;
    long len;
    bstring str = NULL;

    /* eos == the heredoc ident that we found when the heredoc started */
    eos = bdata(here->nd_str);
    len = blength(here->nd_str) - 1;

    /* indicates if we should search for expansions. */
    indent = (func = *eos++) & STR_FUNC_INDENT;

    /* Ack! EOF or end of input string! */
    if ((c = nextc()) == -1) {
      error:
        rb_compile_error(parse_state, "can't find string \"%s\" anywhere before EOF", eos);
        heredoc_restore(lex_strterm, parse_state);
        lex_strterm = 0;
        return 0;
    }
    /* Gr. not yet sure what was_bol() means other than it seems like
       it means only 1 character has been consumed. */

    if (was_bol() && whole_match_p(eos, len, indent, parse_state)) {
        heredoc_restore(lex_strterm, parse_state);
        return tSTRING_END;
    }

    /* If aren't doing expansions, we can just scan until
       we find the identifier. */

    if ((func & STR_FUNC_EXPAND) == 0) {
        do {
            p = bdata(parse_state->lex_lastline);
            pend = parse_state->lex_pend;
            if (pend > p) {
                switch (pend[-1]) {
                  case '\n':
                    if (--pend == p || pend[-1] != '\r') {
                        pend++;
                        break;
                    }
                  case '\r':
                    --pend;
                }
            }
            if (str) {
                bcatblk(str, p, pend - p);
            } else {
                str = blk2bstr(p, pend - p);
            }
            if (pend < parse_state->lex_pend) bcatblk(str, "\n", 1);
            parse_state->lex_p = parse_state->lex_pend;
            if (nextc() == -1) {
                if (str) bdestroy(str);
                goto error;
            }
        } while (!whole_match_p(eos, len, indent, parse_state));
    }
    else {
        newtok(parse_state);
        if (c == '#') {
            switch (c = nextc()) {
              case '$':
              case '@':
                pushback(c, parse_state);
                return tSTRING_DVAR;
              case '{':
                return tSTRING_DBEG;
            }
            tokadd('#', parse_state);
        }

        /* Loop while we haven't found a the heredoc ident. */
        do {
            pushback(c, parse_state);
            /* Scan up until a \n and fill in the token buffer. */
            if ((c = tokadd_string(func, '\n', 0, NULL, parse_state)) == -1) goto error;

            /* We finished scanning, but didn't find a \n, so we setup the node
               and have the lexer file in more. */
            if (c != '\n') {
                pslval->node = NEW_STR(string_new(tok(), toklen()));
                return tSTRING_CONTENT;
            }

            /* I think this consumes the \n */
            tokadd((char)nextc(), parse_state);
            if ((c = nextc()) == -1) goto error;
        } while (!whole_match_p(eos, len, indent, parse_state));
        str = string_new(tok(), toklen());
    }
    heredoc_restore(lex_strterm, parse_state);
    lex_strterm = NEW_STRTERM(-1, 0, 0);
    pslval->node = NEW_STR(str);
    return tSTRING_CONTENT;
}

#include "lex.c.tab"

static void
arg_ambiguous()
{
    rb_warning("ambiguous first argument; put parentheses or even spaces");
}

#define IS_ARG() (parse_state->lex_state == EXPR_ARG || parse_state->lex_state == EXPR_CMDARG)


static char* parse_comment(struct rb_parse_state* parse_state) {
  int len = parse_state->lex_pend - parse_state->lex_p;

  char* str = parse_state->lex_p;
  while(len-- > 0 && ISSPACE(str[0])) str++;
  if(len <= 2) return NULL;

  if(str[0] == '-' && str[1] == '*' && str[2] == '-') return str;

  return NULL;
}

static int
yylex(void *yylval_v, void *vstate)
{
    register int c;
    int space_seen = 0;
    int cmd_state, comment_column;
    struct rb_parse_state *parse_state;
    bstring cur_line;
    enum lex_state last_state;

    YYSTYPE *yylval = (YYSTYPE*)yylval_v;
    parse_state = (struct rb_parse_state*)vstate;

    parse_state->lval = (void *)yylval;

    /*
    c = nextc();
    printf("lex char: %c\n", c);
    pushback(c, parse_state);
    */

    if (lex_strterm) {
        int token;
        if (nd_type(lex_strterm) == NODE_HEREDOC) {
            token = here_document(lex_strterm, parse_state);
            if (token == tSTRING_END) {
                lex_strterm = 0;
                parse_state->lex_state = EXPR_END;
            }
        }
        else {
            token = parse_string(lex_strterm, parse_state);
            if (token == tSTRING_END || token == tREGEXP_END) {
                lex_strterm = 0;
                parse_state->lex_state = EXPR_END;
            }
        }
        return token;
    }

    cmd_state = command_start;
    command_start = FALSE;
  retry:
    switch (c = nextc()) {
      case '\0':                /* NUL */
      case '\004':              /* ^D */
      case '\032':              /* ^Z */
      case -1:                  /* end of script. */
        return 0;

        /* white spaces */
      case ' ': case '\t': case '\f': case '\r':
      case '\13': /* '\v' */
        space_seen++;
        goto retry;

      case '#':         /* it's a comment */
        if(char* str = parse_comment(parse_state)) {
            int len = parse_state->lex_pend - str - 1; // - 1 for the \n
            cur_line = blk2bstr(str, len);
            parse_state->magic_comments->push_back(cur_line);
        }
        parse_state->lex_p = parse_state->lex_pend;
        /* fall through */
      case '\n':
        switch (parse_state->lex_state) {
          case EXPR_BEG:
          case EXPR_FNAME:
          case EXPR_DOT:
          case EXPR_CLASS:
            goto retry;
          default:
            break;
        }
        command_start = TRUE;
        parse_state->lex_state = EXPR_BEG;
        return '\n';

      case '*':
        if ((c = nextc()) == '*') {
            if ((c = nextc()) == '=') {
                pslval->id = tPOW;
                parse_state->lex_state = EXPR_BEG;
                return tOP_ASGN;
            }
            pushback(c, parse_state);
            c = tPOW;
        }
        else {
            if (c == '=') {
                pslval->id = '*';
                parse_state->lex_state = EXPR_BEG;
                return tOP_ASGN;
            }
            pushback(c, parse_state);
            if (IS_ARG() && space_seen && !ISSPACE(c)){
                rb_warning("`*' interpreted as argument prefix");
                c = tSTAR;
            }
            else if (parse_state->lex_state == EXPR_BEG || parse_state->lex_state == EXPR_MID) {
                c = tSTAR;
            }
            else {
                c = '*';
            }
        }
        switch (parse_state->lex_state) {
          case EXPR_FNAME: case EXPR_DOT:
            parse_state->lex_state = EXPR_ARG; break;
          default:
            parse_state->lex_state = EXPR_BEG; break;
        }
        return c;

      case '!':
        parse_state->lex_state = EXPR_BEG;
        if ((c = nextc()) == '=') {
            return tNEQ;
        }
        if (c == '~') {
            return tNMATCH;
        }
        pushback(c, parse_state);
        return '!';

      case '=':
        if (was_bol()) {
            /* skip embedded rd document */
            if (strncmp(parse_state->lex_p, "begin", 5) == 0 && ISSPACE(parse_state->lex_p[5])) {
                for (;;) {
                    parse_state->lex_p = parse_state->lex_pend;
                    c = nextc();
                    if (c == -1) {
                        rb_compile_error(parse_state, "embedded document meets end of file");
                        return 0;
                    }
                    if (c != '=') continue;
                    if (strncmp(parse_state->lex_p, "end", 3) == 0 &&
                        (parse_state->lex_p + 3 == parse_state->lex_pend || ISSPACE(parse_state->lex_p[3]))) {
                        break;
                    }
                }
                parse_state->lex_p = parse_state->lex_pend;
                goto retry;
            }
        }

        switch (parse_state->lex_state) {
          case EXPR_FNAME: case EXPR_DOT:
            parse_state->lex_state = EXPR_ARG; break;
          default:
            parse_state->lex_state = EXPR_BEG; break;
        }
        if ((c = nextc()) == '=') {
            if ((c = nextc()) == '=') {
                return tEQQ;
            }
            pushback(c, parse_state);
            return tEQ;
        }
        if (c == '~') {
            return tMATCH;
        }
        else if (c == '>') {
            return tASSOC;
        }
        pushback(c, parse_state);
        return '=';

      case '<':
        c = nextc();
        if (c == '<' &&
            parse_state->lex_state != EXPR_END &&
            parse_state->lex_state != EXPR_DOT &&
            parse_state->lex_state != EXPR_ENDARG &&
            parse_state->lex_state != EXPR_CLASS &&
            (!IS_ARG() || space_seen)) {
            int token = heredoc_identifier(parse_state);
            if (token) return token;
        }
        switch (parse_state->lex_state) {
          case EXPR_FNAME: case EXPR_DOT:
            parse_state->lex_state = EXPR_ARG; break;
          default:
            parse_state->lex_state = EXPR_BEG; break;
        }
        if (c == '=') {
            if ((c = nextc()) == '>') {
                return tCMP;
            }
            pushback(c, parse_state);
            return tLEQ;
        }
        if (c == '<') {
            if ((c = nextc()) == '=') {
                pslval->id = tLSHFT;
                parse_state->lex_state = EXPR_BEG;
                return tOP_ASGN;
            }
            pushback(c, parse_state);
            return tLSHFT;
        }
        pushback(c, parse_state);
        return '<';

      case '>':
        switch (parse_state->lex_state) {
          case EXPR_FNAME: case EXPR_DOT:
            parse_state->lex_state = EXPR_ARG; break;
          default:
            parse_state->lex_state = EXPR_BEG; break;
        }
        if ((c = nextc()) == '=') {
            return tGEQ;
        }
        if (c == '>') {
            if ((c = nextc()) == '=') {
                pslval->id = tRSHFT;
                parse_state->lex_state = EXPR_BEG;
                return tOP_ASGN;
            }
            pushback(c, parse_state);
            return tRSHFT;
        }
        pushback(c, parse_state);
        return '>';

      case '"':
        lex_strterm = NEW_STRTERM(str_dquote, '"', 0);
        return tSTRING_BEG;

      case '`':
        if (parse_state->lex_state == EXPR_FNAME) {
            parse_state->lex_state = EXPR_END;
            return c;
        }
        if (parse_state->lex_state == EXPR_DOT) {
            if (cmd_state)
                parse_state->lex_state = EXPR_CMDARG;
            else
                parse_state->lex_state = EXPR_ARG;
            return c;
        }
        lex_strterm = NEW_STRTERM(str_xquote, '`', 0);
        pslval->id = 0; /* so that xstring gets used normally */
        return tXSTRING_BEG;

      case '\'':
        lex_strterm = NEW_STRTERM(str_squote, '\'', 0);
        pslval->id = 0; /* so that xstring gets used normally */
        return tSTRING_BEG;

      case '?':
        if (parse_state->lex_state == EXPR_END || parse_state->lex_state == EXPR_ENDARG) {
            parse_state->lex_state = EXPR_BEG;
            return '?';
        }
        c = nextc();
        if (c == -1) {
            rb_compile_error(parse_state, "incomplete character syntax");
            return 0;
        }
        if (ISSPACE(c)){
            if (!IS_ARG()){
                int c2 = 0;
                switch (c) {
                  case ' ':
                    c2 = 's';
                    break;
                  case '\n':
                    c2 = 'n';
                    break;
                  case '\t':
                    c2 = 't';
                    break;
                  case '\v':
                    c2 = 'v';
                    break;
                  case '\r':
                    c2 = 'r';
                    break;
                  case '\f':
                    c2 = 'f';
                    break;
                }
                if (c2) {
                    rb_warn("invalid character syntax; use ?\\%c", c2);
                }
            }
          ternary:
            pushback(c, parse_state);
            parse_state->lex_state = EXPR_BEG;
            parse_state->ternary_colon = 1;
            return '?';
        }
        else if (ismbchar(c)) {
            rb_warn("multibyte character literal not supported yet; use ?\\%.3o", c);
            goto ternary;
        }
        else if ((ISALNUM(c) || c == '_') && parse_state->lex_p < parse_state->lex_pend && is_identchar(*(parse_state->lex_p))) {
            goto ternary;
        }
        else if (c == '\\') {
            c = read_escape(parse_state);
        }
        c &= 0xff;
        parse_state->lex_state = EXPR_END;
        pslval->node = NEW_FIXNUM((intptr_t)c);
        return tINTEGER;

      case '&':
        if ((c = nextc()) == '&') {
            parse_state->lex_state = EXPR_BEG;
            if ((c = nextc()) == '=') {
                pslval->id = tANDOP;
                parse_state->lex_state = EXPR_BEG;
                return tOP_ASGN;
            }
            pushback(c, parse_state);
            return tANDOP;
        }
        else if (c == '=') {
            pslval->id = '&';
            parse_state->lex_state = EXPR_BEG;
            return tOP_ASGN;
        }
        pushback(c, parse_state);
        if (IS_ARG() && space_seen && !ISSPACE(c)){
            rb_warning("`&' interpreted as argument prefix");
            c = tAMPER;
        }
        else if (parse_state->lex_state == EXPR_BEG || parse_state->lex_state == EXPR_MID) {
            c = tAMPER;
        }
        else {
            c = '&';
        }
        switch (parse_state->lex_state) {
          case EXPR_FNAME: case EXPR_DOT:
            parse_state->lex_state = EXPR_ARG; break;
          default:
            parse_state->lex_state = EXPR_BEG;
        }
        return c;

      case '|':
        if ((c = nextc()) == '|') {
            parse_state->lex_state = EXPR_BEG;
            if ((c = nextc()) == '=') {
                pslval->id = tOROP;
                parse_state->lex_state = EXPR_BEG;
                return tOP_ASGN;
            }
            pushback(c, parse_state);
            return tOROP;
        }
        if (c == '=') {
            pslval->id = '|';
            parse_state->lex_state = EXPR_BEG;
            return tOP_ASGN;
        }
        if (parse_state->lex_state == EXPR_FNAME || parse_state->lex_state == EXPR_DOT) {
            parse_state->lex_state = EXPR_ARG;
        }
        else {
            parse_state->lex_state = EXPR_BEG;
        }
        pushback(c, parse_state);
        return '|';

      case '+':
        c = nextc();
        if (parse_state->lex_state == EXPR_FNAME || parse_state->lex_state == EXPR_DOT) {
            parse_state->lex_state = EXPR_ARG;
            if (c == '@') {
                return tUPLUS;
            }
            pushback(c, parse_state);
            return '+';
        }
        if (c == '=') {
            pslval->id = '+';
            parse_state->lex_state = EXPR_BEG;
            return tOP_ASGN;
        }
        if (parse_state->lex_state == EXPR_BEG || parse_state->lex_state == EXPR_MID ||
            (IS_ARG() && space_seen && !ISSPACE(c))) {
            if (IS_ARG()) arg_ambiguous();
            parse_state->lex_state = EXPR_BEG;
            pushback(c, parse_state);
            if (ISDIGIT(c)) {
                c = '+';
                goto start_num;
            }
            return tUPLUS;
        }
        parse_state->lex_state = EXPR_BEG;
        pushback(c, parse_state);
        return '+';

      case '-':
        c = nextc();
        if (parse_state->lex_state == EXPR_FNAME || parse_state->lex_state == EXPR_DOT) {
            parse_state->lex_state = EXPR_ARG;
            if (c == '@') {
                return tUMINUS;
            }
            pushback(c, parse_state);
            return '-';
        }
        if (c == '=') {
            pslval->id = '-';
            parse_state->lex_state = EXPR_BEG;
            return tOP_ASGN;
        }
        if (parse_state->lex_state == EXPR_BEG || parse_state->lex_state == EXPR_MID ||
            (IS_ARG() && space_seen && !ISSPACE(c))) {
            if (IS_ARG()) arg_ambiguous();
            parse_state->lex_state = EXPR_BEG;
            pushback(c, parse_state);
            if (ISDIGIT(c)) {
                return tUMINUS_NUM;
            }
            return tUMINUS;
        }
        parse_state->lex_state = EXPR_BEG;
        pushback(c, parse_state);
        return '-';

      case '.':
        parse_state->lex_state = EXPR_BEG;
        if ((c = nextc()) == '.') {
            if ((c = nextc()) == '.') {
                return tDOT3;
            }
            pushback(c, parse_state);
            return tDOT2;
        }
        pushback(c, parse_state);
        if (ISDIGIT(c)) {
            yyerror("no .<digit> floating literal anymore; put 0 before dot");
        }
        parse_state->lex_state = EXPR_DOT;
        return '.';

      start_num:
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        {
            int is_float, seen_point, seen_e, nondigit;

            is_float = seen_point = seen_e = nondigit = 0;
            parse_state->lex_state = EXPR_END;
            newtok(parse_state);
            if (c == '-' || c == '+') {
                tokadd((char)c,parse_state);
                c = nextc();
            }
            if (c == '0') {
                int start = toklen();
                c = nextc();
                if (c == 'x' || c == 'X') {
                    /* hexadecimal */
                    c = nextc();
                    if (ISXDIGIT(c)) {
                        do {
                            if (c == '_') {
                                if (nondigit) break;
                                nondigit = c;
                                continue;
                            }
                            if (!ISXDIGIT(c)) break;
                            nondigit = 0;
                            tokadd((char)c,parse_state);
                        } while ((c = nextc()) != -1);
                    }
                    pushback(c, parse_state);
                    tokfix();
                    if (toklen() == start) {
                        yyerror("numeric literal without digits");
                    }
                    else if (nondigit) goto trailing_uc;
                    pslval->node = NEW_HEXNUM(string_new2(tok()));
                    return tINTEGER;
                }
                if (c == 'b' || c == 'B') {
                    /* binary */
                    c = nextc();
                    if (c == '0' || c == '1') {
                        do {
                            if (c == '_') {
                                if (nondigit) break;
                                nondigit = c;
                                continue;
                            }
                            if (c != '0' && c != '1') break;
                            nondigit = 0;
                            tokadd((char)c, parse_state);
                        } while ((c = nextc()) != -1);
                    }
                    pushback(c, parse_state);
                    tokfix();
                    if (toklen() == start) {
                        yyerror("numeric literal without digits");
                    }
                    else if (nondigit) goto trailing_uc;
                    pslval->node = NEW_BINNUM(string_new2(tok()));
                    return tINTEGER;
                }
                if (c == 'd' || c == 'D') {
                    /* decimal */
                    c = nextc();
                    if (ISDIGIT(c)) {
                        do {
                            if (c == '_') {
                                if (nondigit) break;
                                nondigit = c;
                                continue;
                            }
                            if (!ISDIGIT(c)) break;
                            nondigit = 0;
                            tokadd((char)c, parse_state);
                        } while ((c = nextc()) != -1);
                    }
                    pushback(c, parse_state);
                    tokfix();
                    if (toklen() == start) {
                        yyerror("numeric literal without digits");
                    }
                    else if (nondigit) goto trailing_uc;
                    pslval->node = NEW_NUMBER(string_new2(tok()));
                    return tINTEGER;
                }
                if (c == '_') {
                    /* 0_0 */
                    goto octal_number;
                }
                if (c == 'o' || c == 'O') {
                    /* prefixed octal */
                    c = nextc();
                    if (c == '_') {
                        yyerror("numeric literal without digits");
                    }
                }
                if (c >= '0' && c <= '7') {
                    /* octal */
                  octal_number:
                    do {
                        if (c == '_') {
                            if (nondigit) break;
                            nondigit = c;
                            continue;
                        }
                        if (c < '0' || c > '7') break;
                        nondigit = 0;
                        tokadd((char)c, parse_state);
                    } while ((c = nextc()) != -1);
                    if (toklen() > start) {
                        pushback(c, parse_state);
                        tokfix();
                        if (nondigit) goto trailing_uc;
                        pslval->node = NEW_OCTNUM(string_new2(tok()));
                        return tINTEGER;
                    }
                    if (nondigit) {
                        pushback(c, parse_state);
                        goto trailing_uc;
                    }
                }
                if (c > '7' && c <= '9') {
                    yyerror("Illegal octal digit");
                }
                else if (c == '.' || c == 'e' || c == 'E') {
                    tokadd('0', parse_state);
                }
                else {
                    pushback(c, parse_state);
                    pslval->node = NEW_FIXNUM(0);
                    return tINTEGER;
                }
            }

            for (;;) {
                switch (c) {
                  case '0': case '1': case '2': case '3': case '4':
                  case '5': case '6': case '7': case '8': case '9':
                    nondigit = 0;
                    tokadd((char)c, parse_state);
                    break;

                  case '.':
                    if (nondigit) goto trailing_uc;
                    if (seen_point || seen_e) {
                        goto decode_num;
                    }
                    else {
                        int c0 = nextc();
                        if (!ISDIGIT(c0)) {
                            pushback(c0, parse_state);
                            goto decode_num;
                        }
                        c = c0;
                    }
                    tokadd('.', parse_state);
                    tokadd((char)c, parse_state);
                    is_float++;
                    seen_point++;
                    nondigit = 0;
                    break;

                  case 'e':
                  case 'E':
                    if (nondigit) {
                        pushback(c, parse_state);
                        c = nondigit;
                        goto decode_num;
                    }
                    if (seen_e) {
                        goto decode_num;
                    }
                    tokadd((char)c, parse_state);
                    seen_e++;
                    is_float++;
                    nondigit = c;
                    c = nextc();
                    if (c != '-' && c != '+') continue;
                    tokadd((char)c, parse_state);
                    nondigit = c;
                    break;

                  case '_':     /* `_' in number just ignored */
                    if (nondigit) goto decode_num;
                    nondigit = c;
                    break;

                  default:
                    goto decode_num;
                }
                c = nextc();
            }

          decode_num:
            pushback(c, parse_state);
            tokfix();
            if (nondigit) {
                char tmp[30];
              trailing_uc:
                snprintf(tmp, sizeof(tmp), "trailing `%c' in number", nondigit);
                yyerror(tmp);
            }
            if (is_float) {
                pslval->node = NEW_FLOAT(string_new2(tok()));
                return tFLOAT;
            }
            pslval->node = NEW_NUMBER(string_new2(tok()));
            return tINTEGER;
        }

      case ']':
      case '}':
      case ')':
        COND_LEXPOP();
        CMDARG_LEXPOP();
        parse_state->lex_state = EXPR_END;
        return c;

      case ':':
        c = nextc();
        if (c == ':') {
            if (parse_state->lex_state == EXPR_BEG ||  parse_state->lex_state == EXPR_MID ||
                parse_state->lex_state == EXPR_CLASS || (IS_ARG() && space_seen)) {
                parse_state->lex_state = EXPR_BEG;
                return tCOLON3;
            }
            parse_state->lex_state = EXPR_DOT;
            return tCOLON2;
        }
        if (parse_state->lex_state == EXPR_END || parse_state->lex_state == EXPR_ENDARG || ISSPACE(c)) {
            pushback(c, parse_state);
            parse_state->lex_state = EXPR_BEG;
            return ':';
        }
        switch (c) {
          case '\'':
            lex_strterm = NEW_STRTERM(str_ssym, (intptr_t)c, 0);
            break;
          case '"':
            lex_strterm = NEW_STRTERM(str_dsym, (intptr_t)c, 0);
            break;
          default:
            pushback(c, parse_state);
            break;
        }
        parse_state->lex_state = EXPR_FNAME;
        return tSYMBEG;

      case '/':
        if (parse_state->lex_state == EXPR_BEG || parse_state->lex_state == EXPR_MID) {
            lex_strterm = NEW_STRTERM(str_regexp, '/', 0);
            return tREGEXP_BEG;
        }
        if ((c = nextc()) == '=') {
            pslval->id = '/';
            parse_state->lex_state = EXPR_BEG;
            return tOP_ASGN;
        }
        pushback(c, parse_state);
        if (IS_ARG() && space_seen) {
            if (!ISSPACE(c)) {
                arg_ambiguous();
                lex_strterm = NEW_STRTERM(str_regexp, '/', 0);
                return tREGEXP_BEG;
            }
        }
        switch (parse_state->lex_state) {
          case EXPR_FNAME: case EXPR_DOT:
            parse_state->lex_state = EXPR_ARG; break;
          default:
            parse_state->lex_state = EXPR_BEG; break;
        }
        return '/';

      case '^':
        if ((c = nextc()) == '=') {
            pslval->id = '^';
            parse_state->lex_state = EXPR_BEG;
            return tOP_ASGN;
        }
        switch (parse_state->lex_state) {
          case EXPR_FNAME: case EXPR_DOT:
            parse_state->lex_state = EXPR_ARG; break;
          default:
            parse_state->lex_state = EXPR_BEG; break;
        }
        pushback(c, parse_state);
        return '^';

      case ';':
        command_start = TRUE;
      case ',':
        parse_state->lex_state = EXPR_BEG;
        return c;

      case '~':
        if (parse_state->lex_state == EXPR_FNAME || parse_state->lex_state == EXPR_DOT) {
            if ((c = nextc()) != '@') {
                pushback(c, parse_state);
            }
        }
        switch (parse_state->lex_state) {
          case EXPR_FNAME: case EXPR_DOT:
            parse_state->lex_state = EXPR_ARG; break;
          default:
            parse_state->lex_state = EXPR_BEG; break;
        }
        return '~';

      case '(':
        command_start = TRUE;
        if (parse_state->lex_state == EXPR_BEG || parse_state->lex_state == EXPR_MID) {
            c = tLPAREN;
        }
        else if (space_seen) {
            if (parse_state->lex_state == EXPR_CMDARG) {
                c = tLPAREN_ARG;
            }
            else if (parse_state->lex_state == EXPR_ARG) {
                rb_warn("don't put space before argument parentheses");
                c = '(';
            }
        }
        COND_PUSH(0);
        CMDARG_PUSH(0);
        parse_state->lex_state = EXPR_BEG;
        return c;

      case '[':
        if (parse_state->lex_state == EXPR_FNAME || parse_state->lex_state == EXPR_DOT) {
            parse_state->lex_state = EXPR_ARG;
            if ((c = nextc()) == ']') {
                if ((c = nextc()) == '=') {
                    return tASET;
                }
                pushback(c, parse_state);
                return tAREF;
            }
            pushback(c, parse_state);
            return '[';
        }
        else if (parse_state->lex_state == EXPR_BEG || parse_state->lex_state == EXPR_MID) {
            c = tLBRACK;
        }
        else if (IS_ARG() && space_seen) {
            c = tLBRACK;
        }
        parse_state->lex_state = EXPR_BEG;
        COND_PUSH(0);
        CMDARG_PUSH(0);
        return c;

      case '{':
        if (IS_ARG() || parse_state->lex_state == EXPR_END)
            c = '{';          /* block (primary) */
        else if (parse_state->lex_state == EXPR_ENDARG)
            c = tLBRACE_ARG;  /* block (expr) */
        else
            c = tLBRACE;      /* hash */
        COND_PUSH(0);
        CMDARG_PUSH(0);
        parse_state->lex_state = EXPR_BEG;
        return c;

      case '\\':
        c = nextc();
        if (c == '\n') {
            space_seen = 1;
            goto retry; /* skip \\n */
        }
        pushback(c, parse_state);
        if(parse_state->lex_state == EXPR_BEG
           || parse_state->lex_state == EXPR_MID || space_seen) {
            parse_state->lex_state = EXPR_DOT;
            return tUBS;
        }
        parse_state->lex_state = EXPR_DOT;
        return '\\';

      case '%':
        if (parse_state->lex_state == EXPR_BEG || parse_state->lex_state == EXPR_MID) {
            intptr_t term;
            intptr_t paren;
            char tmpstr[256];
            char *cur;

            c = nextc();
          quotation:
            if (!ISALNUM(c)) {
                term = c;
                c = 'Q';
            }
            else {
                term = nextc();
                if (ISALNUM(term) || ismbchar(term)) {
                    cur = tmpstr;
                    *cur++ = c;
                    while(ISALNUM(term) || ismbchar(term)) {
                        *cur++ = term;
                        term = nextc();
                    }
                    *cur = 0;
                    c = 1;

                }
            }
            if (c == -1 || term == -1) {
                rb_compile_error(parse_state, "unterminated quoted string meets end of file");
                return 0;
            }
            paren = term;
            if (term == '(') term = ')';
            else if (term == '[') term = ']';
            else if (term == '{') term = '}';
            else if (term == '<') term = '>';
            else paren = 0;

            switch (c) {
              case 'Q':
                lex_strterm = NEW_STRTERM(str_dquote, term, paren);
                return tSTRING_BEG;

              case 'q':
                lex_strterm = NEW_STRTERM(str_squote, term, paren);
                return tSTRING_BEG;

              case 'W':
                lex_strterm = NEW_STRTERM(str_dquote | STR_FUNC_QWORDS, term, paren);
                do {c = nextc();} while (ISSPACE(c));
                pushback(c, parse_state);
                return tWORDS_BEG;

              case 'w':
                lex_strterm = NEW_STRTERM(str_squote | STR_FUNC_QWORDS, term, paren);
                do {c = nextc();} while (ISSPACE(c));
                pushback(c, parse_state);
                return tQWORDS_BEG;

              case 'x':
                lex_strterm = NEW_STRTERM(str_xquote, term, paren);
                pslval->id = 0;
                return tXSTRING_BEG;

              case 'r':
                lex_strterm = NEW_STRTERM(str_regexp, term, paren);
                return tREGEXP_BEG;

              case 's':
                lex_strterm = NEW_STRTERM(str_ssym, term, paren);
                parse_state->lex_state = EXPR_FNAME;
                return tSYMBEG;

              case 1:
                lex_strterm = NEW_STRTERM(str_xquote, term, paren);
                pslval->id = rb_parser_sym(tmpstr);
                return tXSTRING_BEG;

              default:
                lex_strterm = NEW_STRTERM(str_xquote, term, paren);
                tmpstr[0] = c;
                tmpstr[1] = 0;
                pslval->id = rb_parser_sym(tmpstr);
                return tXSTRING_BEG;
            }
        }
        if ((c = nextc()) == '=') {
            pslval->id = '%';
            parse_state->lex_state = EXPR_BEG;
            return tOP_ASGN;
        }
        if (IS_ARG() && space_seen && !ISSPACE(c)) {
            goto quotation;
        }
        switch (parse_state->lex_state) {
          case EXPR_FNAME: case EXPR_DOT:
            parse_state->lex_state = EXPR_ARG; break;
          default:
            parse_state->lex_state = EXPR_BEG; break;
        }
        pushback(c, parse_state);
        return '%';

      case '$':
        last_state = parse_state->lex_state;
        parse_state->lex_state = EXPR_END;
        newtok(parse_state);
        c = nextc();
        switch (c) {
          case '_':             /* $_: last read line string */
            c = nextc();
            if (is_identchar(c)) {
                tokadd('$', parse_state);
                tokadd('_', parse_state);
                break;
            }
            pushback(c, parse_state);
            c = '_';
            /* fall through */
          case '~':             /* $~: match-data */
            local_cnt(c);
            /* fall through */
          case '*':             /* $*: argv */
          case '$':             /* $$: pid */
          case '?':             /* $?: last status */
          case '!':             /* $!: error string */
          case '@':             /* $@: error position */
          case '/':             /* $/: input record separator */
          case '\\':            /* $\: output record separator */
          case ';':             /* $;: field separator */
          case ',':             /* $,: output field separator */
          case '.':             /* $.: last read line number */
          case '=':             /* $=: ignorecase */
          case ':':             /* $:: load path */
          case '<':             /* $<: reading filename */
          case '>':             /* $>: default output handle */
          case '\"':            /* $": already loaded files */
            tokadd('$', parse_state);
            tokadd((char)c, parse_state);
            tokfix();
            pslval->id = rb_parser_sym(tok());
            return tGVAR;

          case '-':
            tokadd('$', parse_state);
            tokadd((char)c, parse_state);
            c = nextc();
            tokadd((char)c, parse_state);
          gvar:
            tokfix();
            pslval->id = rb_parser_sym(tok());
            /* xxx shouldn't check if valid option variable */
            return tGVAR;

          case '&':             /* $&: last match */
          case '`':             /* $`: string before last match */
          case '\'':            /* $': string after last match */
          case '+':             /* $+: string matches last paren. */
	    if (last_state == EXPR_FNAME) {
		tokadd((char)'$', parse_state);
		tokadd(c, parse_state);
		goto gvar;
	    }
            pslval->node = NEW_BACK_REF((intptr_t)c);
            return tBACK_REF;

          case '1': case '2': case '3':
          case '4': case '5': case '6':
          case '7': case '8': case '9':
            tokadd('$', parse_state);
            do {
                tokadd((char)c, parse_state);
                c = nextc();
            } while (ISDIGIT(c));
            pushback(c, parse_state);
	    if (last_state == EXPR_FNAME) goto gvar;
            tokfix();
            pslval->node = NEW_NTH_REF((intptr_t)atoi(tok()+1));
            return tNTH_REF;

          default:
            if (!is_identchar(c)) {
                pushback(c, parse_state);
                return '$';
            }
          case '0':
            tokadd('$', parse_state);
        }
        break;

      case '@':
        c = nextc();
        newtok(parse_state);
        tokadd('@', parse_state);
        if (c == '@') {
            tokadd('@', parse_state);
            c = nextc();
        }
        if (ISDIGIT(c)) {
            if (tokidx == 1) {
                rb_compile_error(parse_state,
                    "`@%c' is not allowed as an instance variable name", c);
            }
            else {
                rb_compile_error(parse_state,
                    "`@@%c' is not allowed as a class variable name", c);
            }
        }
        if (!is_identchar(c)) {
            pushback(c, parse_state);
            return '@';
        }
        break;

      case '_':
        if (was_bol() && whole_match_p("__END__", 7, 0, parse_state)) {
            parse_state->end_seen = 1;
            return -1;
        }
        newtok(parse_state);
        break;

      default:
        if (!is_identchar(c)) {
            rb_compile_error(parse_state, "Invalid char `\\%03o' in expression", c);
            goto retry;
        }

        newtok(parse_state);
        break;
    }

    do {
        tokadd((char)c, parse_state);
        if (ismbchar(c)) {
            int i, len = mbclen(c)-1;

            for (i = 0; i < len; i++) {
                c = nextc();
                tokadd((char)c, parse_state);
            }
        }
        c = nextc();
    } while (is_identchar(c));
    if ((c == '!' || c == '?') && is_identchar(tok()[0]) && !peek('=')) {
        tokadd((char)c, parse_state);
    }
    else {
        pushback(c, parse_state);
    }
    tokfix();

    {
        int result = 0;

        last_state = parse_state->lex_state;
        switch (tok()[0]) {
          case '$':
            parse_state->lex_state = EXPR_END;
            result = tGVAR;
            break;
          case '@':
            parse_state->lex_state = EXPR_END;
            if (tok()[1] == '@')
                result = tCVAR;
            else
                result = tIVAR;
            break;

          default:
            if (toklast() == '!' || toklast() == '?') {
                result = tFID;
            }
            else {
                if (parse_state->lex_state == EXPR_FNAME) {
                    if ((c = nextc()) == '=' && !peek('~') && !peek('>') &&
                        (!peek('=') || (parse_state->lex_p + 1 < parse_state->lex_pend && (parse_state->lex_p)[1] == '>'))) {
                        result = tIDENTIFIER;
                        tokadd((char)c, parse_state);
                        tokfix();
                    }
                    else {
                        pushback(c, parse_state);
                    }
                }
                if (result == 0 && ISUPPER(tok()[0])) {
                    result = tCONSTANT;
                }
                else {
                    result = tIDENTIFIER;
                }
            }

            if (parse_state->lex_state != EXPR_DOT) {
                const struct kwtable *kw;

                /* See if it is a reserved word.  */
                kw = mel_reserved_word(tok(), toklen());
                if (kw) {
                    enum lex_state state = parse_state->lex_state;
                    parse_state->lex_state = kw->state;
                    if (state == EXPR_FNAME) {
                        pslval->id = rb_parser_sym(kw->name);
                    }
                    if (kw->id[0] == kDO) {
                        if (COND_P()) return kDO_COND;
                        if (CMDARG_P() && state != EXPR_CMDARG)
                            return kDO_BLOCK;
                        if (state == EXPR_ENDARG)
                            return kDO_BLOCK;
                        return kDO;
                    }
                    if (state == EXPR_BEG)
                        return kw->id[0];
                    else {
                        if (kw->id[0] != kw->id[1])
                            parse_state->lex_state = EXPR_BEG;
                        return kw->id[1];
                    }
                }
            }

            if (parse_state->lex_state == EXPR_BEG ||
                parse_state->lex_state == EXPR_MID ||
                parse_state->lex_state == EXPR_DOT ||
                parse_state->lex_state == EXPR_ARG ||
                parse_state->lex_state == EXPR_CMDARG) {
                if (cmd_state) {
                    parse_state->lex_state = EXPR_CMDARG;
                }
                else {
                    parse_state->lex_state = EXPR_ARG;
                }
            }
            else {
                parse_state->lex_state = EXPR_END;
            }
        }
        pslval->id = rb_parser_sym(tok());
        if(is_local_id(pslval->id) &&
           last_state != EXPR_DOT &&
           local_id(pslval->id)) {
           parse_state->lex_state = EXPR_END;
        }

/*         if (is_local_id(pslval->id) && local_id(pslval->id)) { */
/*             parse_state->lex_state = EXPR_END; */
/*         } */

        return result;
    }
}


NODE*
node_newnode(rb_parse_state *st, enum node_type type,
                 VALUE a0, VALUE a1, VALUE a2)
{
    NODE *n = (NODE*)pt_allocate(st, sizeof(NODE));

    n->flags = 0;
    nd_set_type(n, type);
    nd_set_line(n, ruby_sourceline);
    n->nd_file = ruby_sourcefile;

    n->u1.value = a0;
    n->u2.value = a1;
    n->u3.value = a2;

    return n;
}

static NODE*
newline_node(rb_parse_state *parse_state, NODE *node)
{
    NODE *nl = 0;
    if (node) {
        if (nd_type(node) == NODE_NEWLINE) return node;
        nl = NEW_NEWLINE(node);
        fixpos(nl, node);
        nl->nd_nth = nd_line(node);
    }
    return nl;
}

static void
fixpos(NODE *node, NODE *orig)
{
    if (!node) return;
    if (!orig) return;
    if (orig == (NODE*)1) return;
    node->nd_file = orig->nd_file;
    nd_set_line(node, nd_line(orig));
}

static void
parser_warning(rb_parse_state *parse_state, NODE *node, const char *mesg)
{
    int line = ruby_sourceline;
    if(parse_state->emit_warnings) {
      ruby_sourceline = nd_line(node);
      printf("%s:%li: warning: %s\n", ruby_sourcefile, ruby_sourceline, mesg);
      ruby_sourceline = line;
    }
}

static NODE*
block_append(rb_parse_state *parse_state, NODE *head, NODE *tail)
{
    NODE *end, *h = head;

    if (tail == 0) return head;

  again:
    if (h == 0) return tail;
    switch (nd_type(h)) {
      case NODE_NEWLINE:
        h = h->nd_next;
        goto again;
      case NODE_STR:
      case NODE_LIT:
        parser_warning(parse_state, h, "unused literal ignored");
        return tail;
      default:
        h = end = NEW_BLOCK(head);
        end->nd_end = end;
        fixpos(end, head);
        head = end;
        break;
      case NODE_BLOCK:
        end = h->nd_end;
        break;
    }

    if (parse_state->verbose) {
        NODE *nd = end->nd_head;
      newline:
        switch (nd_type(nd)) {
          case NODE_RETURN:
          case NODE_BREAK:
          case NODE_NEXT:
          case NODE_REDO:
          case NODE_RETRY:
            parser_warning(parse_state, nd, "statement not reached");
            break;

        case NODE_NEWLINE:
            nd = nd->nd_next;
            goto newline;

          default:
            break;
        }
    }

    if (nd_type(tail) != NODE_BLOCK) {
        tail = NEW_BLOCK(tail);
        tail->nd_end = tail;
    }
    end->nd_next = tail;
    h->nd_end = tail->nd_end;
    return head;
}

/* append item to the list */
static NODE*
list_append(rb_parse_state *parse_state, NODE *list, NODE *item)
{
    NODE *last;

    if (list == 0) return NEW_LIST(item);
    if (list->nd_next) {
        last = list->nd_next->nd_end;
    }
    else {
        last = list;
    }

    list->nd_alen += 1;
    last->nd_next = NEW_LIST(item);
    list->nd_next->nd_end = last->nd_next;
    return list;
}

/* concat two lists */
static NODE*
list_concat(NODE *head, NODE *tail)
{
    NODE *last;

    if (head->nd_next) {
        last = head->nd_next->nd_end;
    }
    else {
        last = head;
    }

    head->nd_alen += tail->nd_alen;
    last->nd_next = tail;
    if (tail->nd_next) {
        head->nd_next->nd_end = tail->nd_next->nd_end;
    }
    else {
        head->nd_next->nd_end = tail;
    }

    return head;
}

/* concat two string literals */
static NODE *
literal_concat(rb_parse_state *parse_state, NODE *head, NODE *tail)
{
    enum node_type htype;

    if (!head) return tail;
    if (!tail) return head;

    htype = (enum node_type)nd_type(head);
    if (htype == NODE_EVSTR) {
        NODE *node = NEW_DSTR(string_new(0, 0));
        head = list_append(parse_state, node, head);
    }
    switch (nd_type(tail)) {
      case NODE_STR:
        if (htype == NODE_STR) {
            if(head->nd_str) {
                bconcat(head->nd_str, tail->nd_str);
                bdestroy(tail->nd_str);
            } else {
                head = tail;
            }
        }
        else {
            list_append(parse_state, head, tail);
        }
        break;

      case NODE_DSTR:
        if (htype == NODE_STR) {
            bconcat(head->nd_str, tail->nd_str);
            bdestroy(tail->nd_str);

            tail->nd_lit = head->nd_lit;
            head = tail;
        }
        else {
            nd_set_type(tail, NODE_ARRAY);
            tail->nd_head = NEW_STR(tail->nd_lit);
            list_concat(head, tail);
        }
        break;

      case NODE_EVSTR:
        if (htype == NODE_STR) {
            nd_set_type(head, NODE_DSTR);
            head->nd_alen = 1;
        }
        list_append(parse_state, head, tail);
        break;
    }
    return head;
}

static NODE *
evstr2dstr(rb_parse_state *parse_state, NODE *node)
{
    if (nd_type(node) == NODE_EVSTR) {
        node = list_append(parse_state, NEW_DSTR(string_new(0, 0)), node);
    }
    return node;
}

static NODE *
new_evstr(rb_parse_state *parse_state, NODE *node)
{
    NODE *head = node;

  again:
    if (node) {
        switch (nd_type(node)) {
          case NODE_STR: case NODE_DSTR: case NODE_EVSTR:
            return node;
          case NODE_NEWLINE:
            node = node->nd_next;
            goto again;
        }
    }
    return NEW_EVSTR(head);
}

static const struct {
    QUID token;
    const char name[12];
} op_tbl[] = {
    {tDOT2,     ".."},
    {tDOT3,     "..."},
    {'+',       "+"},
    {'-',       "-"},
    {'+',       "+(binary)"},
    {'-',       "-(binary)"},
    {'*',       "*"},
    {'/',       "/"},
    {'%',       "%"},
    {tPOW,      "**"},
    {tUPLUS,    "+@"},
    {tUMINUS,   "-@"},
    {tUPLUS,    "+(unary)"},
    {tUMINUS,   "-(unary)"},
    {'|',       "|"},
    {'^',       "^"},
    {'&',       "&"},
    {tCMP,      "<=>"},
    {'>',       ">"},
    {tGEQ,      ">="},
    {'<',       "<"},
    {tLEQ,      "<="},
    {tEQ,       "=="},
    {tEQQ,      "==="},
    {tNEQ,      "!="},
    {tMATCH,    "=~"},
    {tNMATCH,   "!~"},
    {'!',       "!"},
    {'~',       "~"},
    {'!',       "!(unary)"},
    {'~',       "~(unary)"},
    {'!',       "!@"},
    {'~',       "~@"},
    {tAREF,     "[]"},
    {tASET,     "[]="},
    {tLSHFT,    "<<"},
    {tRSHFT,    ">>"},
    {tCOLON2,   "::"},
    {'`',       "`"},
    {0, ""}
};

static QUID convert_op(QUID id) {
    int i;
    for(i = 0; op_tbl[i].token; i++) {
        if(op_tbl[i].token == id) {
            return rb_parser_sym(op_tbl[i].name);
        }
    }
    return id;
}

static NODE *
call_op(NODE *recv, QUID id, int narg, NODE *arg1, rb_parse_state *parse_state)
{
    value_expr(recv);
    if (narg == 1) {
        value_expr(arg1);
        arg1 = NEW_LIST(arg1);
    }
    else {
        arg1 = 0;
    }

    id = convert_op(id);


    return NEW_CALL(recv, id, arg1);
}

static NODE*
match_gen(NODE *node1, NODE *node2, rb_parse_state *parse_state)
{
    local_cnt('~');

    value_expr(node1);
    value_expr(node2);
    if (node1) {
        switch (nd_type(node1)) {
          case NODE_DREGX:
          case NODE_DREGX_ONCE:
            return NEW_MATCH2(node1, node2);

          case NODE_REGEX:
              return NEW_MATCH2(node1, node2);
        }
    }

    if (node2) {
        switch (nd_type(node2)) {
          case NODE_DREGX:
          case NODE_DREGX_ONCE:
            return NEW_MATCH3(node2, node1);

          case NODE_REGEX:
            return NEW_MATCH3(node2, node1);
        }
    }

    return NEW_CALL(node1, convert_op(tMATCH), NEW_LIST(node2));
}

static NODE*
mel_gettable(rb_parse_state *parse_state, QUID id)
{
    if (id == kSELF) {
        return NEW_SELF();
    }
    else if (id == kNIL) {
        return NEW_NIL();
    }
    else if (id == kTRUE) {
        return NEW_TRUE();
    }
    else if (id == kFALSE) {
        return NEW_FALSE();
    }
    else if (id == k__FILE__) {
        return NEW_FILE();
    }
    else if (id == k__LINE__) {
        return NEW_FIXNUM(ruby_sourceline);
    }
    else if (is_local_id(id)) {
        if (local_id(id)) return NEW_LVAR(id);
        /* method call without arguments */
        return NEW_VCALL(id);
    }
    else if (is_global_id(id)) {
        return NEW_GVAR(id);
    }
    else if (is_instance_id(id)) {
        return NEW_IVAR(id);
    }
    else if (is_const_id(id)) {
        return NEW_CONST(id);
    }
    else if (is_class_id(id)) {
        return NEW_CVAR(id);
    }
    /* FIXME: indicate which identifier. */
    rb_compile_error(parse_state, "identifier is not valid 1\n");
    return 0;
}

static void
reset_block(rb_parse_state *parse_state) {
  if(!parse_state->variables->block_vars) {
    parse_state->variables->block_vars = var_table_create();
  } else {
    parse_state->variables->block_vars = var_table_push(parse_state->variables->block_vars);
  }
}

static NODE *
extract_block_vars(rb_parse_state *parse_state, NODE* node, var_table vars)
{
    int i;
    NODE *var, *out = node;

    // we don't create any DASGN_CURR nodes
    goto out;

    if (!node) goto out;
    if(var_table_size(vars) == 0) goto out;

    var = NULL;
    for(i = 0; i < var_table_size(vars); i++) {
        var = NEW_DASGN_CURR(var_table_get(vars, i), var);
    }
    out = block_append(parse_state, var, node);

out:
  parse_state->variables->block_vars = var_table_pop(parse_state->variables->block_vars);

  return out;
}

static NODE*
assignable(QUID id, NODE *val, rb_parse_state *parse_state)
{
    value_expr(val);
    if (id == kSELF) {
        yyerror("Can't change the value of self");
    }
    else if (id == kNIL) {
        yyerror("Can't assign to nil");
    }
    else if (id == kTRUE) {
        yyerror("Can't assign to true");
    }
    else if (id == kFALSE) {
        yyerror("Can't assign to false");
    }
    else if (id == k__FILE__) {
        yyerror("Can't assign to __FILE__");
    }
    else if (id == k__LINE__) {
        yyerror("Can't assign to __LINE__");
    }
    else if (is_local_id(id)) {
        if(parse_state->variables->block_vars) {
          var_table_add(parse_state->variables->block_vars, id);
        }
        return NEW_LASGN(id, val);
    }
    else if (is_global_id(id)) {
        return NEW_GASGN(id, val);
    }
    else if (is_instance_id(id)) {
        return NEW_IASGN(id, val);
    }
    else if (is_const_id(id)) {
        if (in_def || in_single)
            yyerror("dynamic constant assignment");
        return NEW_CDECL(id, val, 0);
    }
    else if (is_class_id(id)) {
        if (in_def || in_single) return NEW_CVASGN(id, val);
        return NEW_CVDECL(id, val);
    }
    else {
        /* FIXME: indicate which identifier. */
        rb_compile_error(parse_state, "identifier is not valid 2 (%d)\n", id);
    }
    return 0;
}

static NODE *
aryset(NODE *recv, NODE *idx, rb_parse_state *parse_state)
{
    if (recv && nd_type(recv) == NODE_SELF)
        recv = (NODE *)1;
    else
        value_expr(recv);
    return NEW_ATTRASGN(recv, convert_op(tASET), idx);
}


static QUID
rb_id_attrset(QUID id)
{
    id &= ~ID_SCOPE_MASK;
    id |= ID_ATTRSET;
    return id;
}

static NODE *
attrset(NODE *recv, QUID id, rb_parse_state *parse_state)
{
    if (recv && nd_type(recv) == NODE_SELF)
        recv = (NODE *)1;
    else
        value_expr(recv);
    return NEW_ATTRASGN(recv, rb_id_attrset(id), 0);
}

static void
rb_backref_error(NODE *node, rb_parse_state *parse_state)
{
    switch (nd_type(node)) {
      case NODE_NTH_REF:
        rb_compile_error(parse_state, "Can't set variable $%u", node->nd_nth);
        break;
      case NODE_BACK_REF:
        rb_compile_error(parse_state, "Can't set variable $%c", (int)node->nd_nth);
        break;
    }
}

static NODE *
arg_concat(rb_parse_state *parse_state, NODE *node1, NODE *node2)
{
    if (!node2) return node1;
    return NEW_ARGSCAT(node1, node2);
}

static NODE *
arg_add(rb_parse_state *parse_state, NODE *node1, NODE *node2)
{
    if (!node1) return NEW_LIST(node2);
    if (nd_type(node1) == NODE_ARRAY) {
        return list_append(parse_state, node1, node2);
    }
    else {
        return NEW_ARGSPUSH(node1, node2);
    }
}

static NODE*
node_assign(NODE *lhs, NODE *rhs, rb_parse_state *parse_state)
{
    if (!lhs) return 0;

    value_expr(rhs);
    switch (nd_type(lhs)) {
      case NODE_GASGN:
      case NODE_IASGN:
      case NODE_LASGN:
      case NODE_DASGN:
      case NODE_DASGN_CURR:
      case NODE_MASGN:
      case NODE_CDECL:
      case NODE_CVDECL:
      case NODE_CVASGN:
        lhs->nd_value = rhs;
        break;

      case NODE_ATTRASGN:
      case NODE_CALL:
        lhs->nd_args = arg_add(parse_state, lhs->nd_args, rhs);
        break;

      default:
        /* should not happen */
        break;
    }

    return lhs;
}

static int
value_expr0(NODE *node, rb_parse_state *parse_state)
{
    int cond = 0;

    while (node) {
        switch (nd_type(node)) {
          case NODE_DEFN:
          case NODE_DEFS:
            parser_warning(parse_state, node, "void value expression");
            return FALSE;

          case NODE_RETURN:
          case NODE_BREAK:
          case NODE_NEXT:
          case NODE_REDO:
          case NODE_RETRY:
            if (!cond) yyerror("void value expression");
            /* or "control never reach"? */
            return FALSE;

          case NODE_BLOCK:
            while (node->nd_next) {
                node = node->nd_next;
            }
            node = node->nd_head;
            break;

          case NODE_BEGIN:
            node = node->nd_body;
            break;

          case NODE_IF:
            if (!value_expr(node->nd_body)) return FALSE;
            node = node->nd_else;
            break;

          case NODE_AND:
          case NODE_OR:
            cond = 1;
            node = node->nd_2nd;
            break;

          case NODE_NEWLINE:
            node = node->nd_next;
            break;

          default:
            return TRUE;
        }
    }

    return TRUE;
}

static void
void_expr0(NODE *node, rb_parse_state *parse_state)
{
  const char *useless = NULL;

    if (!parse_state->verbose) return;

  again:
    if (!node) return;
    switch (nd_type(node)) {
      case NODE_NEWLINE:
        node = node->nd_next;
        goto again;

      case NODE_CALL:
        switch (node->nd_mid) {
          case '+':
          case '-':
          case '*':
          case '/':
          case '%':
          case tPOW:
          case tUPLUS:
          case tUMINUS:
          case '|':
          case '^':
          case '&':
          case tCMP:
          case '>':
          case tGEQ:
          case '<':
          case tLEQ:
          case tEQ:
          case tNEQ:
            useless = "";
            break;
        }
        break;

      case NODE_LVAR:
      case NODE_DVAR:
      case NODE_GVAR:
      case NODE_IVAR:
      case NODE_CVAR:
      case NODE_NTH_REF:
      case NODE_BACK_REF:
        useless = "a variable";
        break;
      case NODE_CONST:
      case NODE_CREF:
        useless = "a constant";
        break;
      case NODE_LIT:
      case NODE_STR:
      case NODE_DSTR:
      case NODE_DREGX:
      case NODE_DREGX_ONCE:
        useless = "a literal";
        break;
      case NODE_COLON2:
      case NODE_COLON3:
        useless = "::";
        break;
      case NODE_DOT2:
        useless = "..";
        break;
      case NODE_DOT3:
        useless = "...";
        break;
      case NODE_SELF:
        useless = "self";
        break;
      case NODE_NIL:
        useless = "nil";
        break;
      case NODE_TRUE:
        useless = "true";
        break;
      case NODE_FALSE:
        useless = "false";
        break;
      case NODE_DEFINED:
        useless = "defined?";
        break;
    }

    if (useless) {
        int line = ruby_sourceline;

        ruby_sourceline = nd_line(node);
        rb_warn("useless use of %s in void context", useless);
        ruby_sourceline = line;
    }
}

static void
void_stmts(NODE *node, rb_parse_state *parse_state)
{
    if (!parse_state->verbose) return;
    if (!node) return;
    if (nd_type(node) != NODE_BLOCK) return;

    for (;;) {
        if (!node->nd_next) return;
        void_expr(node->nd_head);
        node = node->nd_next;
    }
}

static NODE *
remove_begin(NODE *node, rb_parse_state *parse_state)
{
    NODE **n = &node;
    while (*n) {
        switch (nd_type(*n)) {
          case NODE_NEWLINE:
            n = &(*n)->nd_next;
            continue;
          case NODE_BEGIN:
            *n = (*n)->nd_body;
          default:
            return node;
        }
    }
    return node;
}

static int
assign_in_cond(NODE *node, rb_parse_state *parse_state)
{
    switch (nd_type(node)) {
      case NODE_MASGN:
        yyerror("multiple assignment in conditional");
        return 1;

      case NODE_LASGN:
      case NODE_DASGN:
      case NODE_GASGN:
      case NODE_IASGN:
        break;

      case NODE_NEWLINE:
      default:
        return 0;
    }

    switch (nd_type(node->nd_value)) {
      case NODE_LIT:
      case NODE_STR:
      case NODE_NIL:
      case NODE_TRUE:
      case NODE_FALSE:
        return 1;

      case NODE_DSTR:
      case NODE_XSTR:
      case NODE_DXSTR:
      case NODE_EVSTR:
      case NODE_DREGX:
      default:
        break;
    }
    return 1;
}

static int
e_option_supplied()
{
    if (strcmp(ruby_sourcefile, "-e") == 0)
        return TRUE;
    return FALSE;
}

static void
warn_unless_e_option(rb_parse_state *ps, NODE *node, const char *str)
{
    if (!e_option_supplied()) parser_warning(ps, node, str);
}

static NODE *cond0(NODE *node, rb_parse_state *parse_state);

static NODE*
range_op(NODE *node, rb_parse_state *parse_state)
{
    enum node_type type;

    if (!e_option_supplied()) return node;
    if (node == 0) return 0;

    value_expr(node);
    node = cond0(node, parse_state);
    type = (enum node_type)nd_type(node);
    if (type == NODE_NEWLINE) {
        node = node->nd_next;
        type = (enum node_type)nd_type(node);
    }
    if (type == NODE_LIT && FIXNUM_P(node->nd_lit)) {
        warn_unless_e_option(parse_state, node, "integer literal in conditional range");
        return call_op(node,tEQ,1,NEW_GVAR(rb_parser_sym("$.")), parse_state);
    }
    return node;
}

static int
literal_node(NODE *node)
{
    if (!node) return 1;        /* same as NODE_NIL */
    switch (nd_type(node)) {
      case NODE_LIT:
      case NODE_STR:
      case NODE_DSTR:
      case NODE_EVSTR:
      case NODE_DREGX:
      case NODE_DREGX_ONCE:
      case NODE_DSYM:
        return 2;
      case NODE_TRUE:
      case NODE_FALSE:
      case NODE_NIL:
        return 1;
    }
    return 0;
}

static NODE*
cond0(NODE *node, rb_parse_state *parse_state)
{
    if (node == 0) return 0;
    assign_in_cond(node, parse_state);

    switch (nd_type(node)) {
      case NODE_DSTR:
      case NODE_EVSTR:
      case NODE_STR:
        break;

      case NODE_DREGX:
      case NODE_DREGX_ONCE:
        local_cnt('_');
        local_cnt('~');
        return NEW_MATCH2(node, NEW_GVAR(rb_parser_sym("$_")));

      case NODE_AND:
      case NODE_OR:
        node->nd_1st = cond0(node->nd_1st, parse_state);
        node->nd_2nd = cond0(node->nd_2nd, parse_state);
        break;

      case NODE_DOT2:
      case NODE_DOT3:
        node->nd_beg = range_op(node->nd_beg, parse_state);
        node->nd_end = range_op(node->nd_end, parse_state);
        if (nd_type(node) == NODE_DOT2) nd_set_type(node,NODE_FLIP2);
        else if (nd_type(node) == NODE_DOT3) nd_set_type(node, NODE_FLIP3);
        if (!e_option_supplied()) {
            int b = literal_node(node->nd_beg);
            int e = literal_node(node->nd_end);
            if ((b == 1 && e == 1) || (b + e >= 2 && parse_state->verbose)) {
            }
        }
        break;

      case NODE_DSYM:
        break;

      case NODE_REGEX:
        nd_set_type(node, NODE_MATCH);
        local_cnt('_');
        local_cnt('~');
      default:
        break;
    }
    return node;
}

static NODE*
cond(NODE *node, rb_parse_state *parse_state)
{
    if (node == 0) return 0;
    value_expr(node);
    if (nd_type(node) == NODE_NEWLINE){
        node->nd_next = cond0(node->nd_next, parse_state);
        return node;
    }
    return cond0(node, parse_state);
}

static NODE*
logop(enum node_type type, NODE *left, NODE *right, rb_parse_state *parse_state)
{
    value_expr(left);
    if (left && nd_type(left) == type) {
        NODE *node = left, *second;
        while ((second = node->nd_2nd) != 0 && nd_type(second) == type) {
            node = second;
        }
        node->nd_2nd = NEW_NODE(type, second, right, 0);
        return left;
    }
    return NEW_NODE(type, left, right, 0);
}

static int
cond_negative(NODE **nodep)
{
    NODE *c = *nodep;

    if (!c) return 0;
    switch (nd_type(c)) {
      case NODE_NOT:
        *nodep = c->nd_body;
        return 1;
      case NODE_NEWLINE:
        if (c->nd_next && nd_type(c->nd_next) == NODE_NOT) {
            c->nd_next = c->nd_next->nd_body;
            return 1;
        }
    }
    return 0;
}

static void
no_blockarg(rb_parse_state *parse_state, NODE *node)
{
    if (node && nd_type(node) == NODE_BLOCK_PASS) {
        rb_compile_error(parse_state, "block argument should not be given");
    }
}

static NODE *
ret_args(rb_parse_state *parse_state, NODE *node)
{
    if (node) {
        no_blockarg(parse_state, node);
        if (nd_type(node) == NODE_ARRAY && node->nd_next == 0) {
            node = node->nd_head;
        }
        if (node && nd_type(node) == NODE_SPLAT) {
            node = NEW_SVALUE(node);
        }
    }
    return node;
}

static NODE *
new_yield(rb_parse_state *parse_state, NODE *node)
{
    VALUE state = Qtrue;

    if (node) {
        no_blockarg(parse_state, node);
        if (nd_type(node) == NODE_ARRAY && node->nd_next == 0) {
            node = node->nd_head;
            state = Qfalse;
        }
        if (node && nd_type(node) == NODE_SPLAT) {
            state = Qtrue;
        }
    }
    else {
        state = Qfalse;
    }
    return NEW_YIELD(node, state);
}

static NODE *
arg_blk_pass(NODE *node1, NODE *node2)
{
    if (node2) {
        node2->nd_head = node1;
        return node2;
    }
    return node1;
}

static NODE*
arg_prepend(rb_parse_state *parse_state, NODE *node1, NODE *node2)
{
    switch (nd_type(node2)) {
      case NODE_ARRAY:
        return list_concat(NEW_LIST(node1), node2);

      case NODE_SPLAT:
        return arg_concat(parse_state, node1, node2->nd_head);

      case NODE_BLOCK_PASS:
        node2->nd_body = arg_prepend(parse_state, node1, node2->nd_body);
        return node2;

      default:
        printf("unknown nodetype(%d) for arg_prepend", nd_type(node2));
        abort();
    }
    return 0;                   /* not reached */
}

static NODE*
new_call(rb_parse_state *parse_state,NODE *r,QUID m,NODE *a)
{
    if (a && nd_type(a) == NODE_BLOCK_PASS) {
        a->nd_iter = NEW_CALL(r,convert_op(m),a->nd_head);
        return a;
    }
    return NEW_CALL(r,convert_op(m),a);
}

static NODE*
new_fcall(rb_parse_state *parse_state,QUID m,NODE *a)
{
    if (a && nd_type(a) == NODE_BLOCK_PASS) {
        a->nd_iter = NEW_FCALL(m,a->nd_head);
        return a;
    }
    return NEW_FCALL(m,a);
}

static NODE*
new_super(rb_parse_state *parse_state,NODE *a)
{
    if (a && nd_type(a) == NODE_BLOCK_PASS) {
        a->nd_iter = NEW_SUPER(a->nd_head);
        return a;
    }
    return NEW_SUPER(a);
}


static void
mel_local_push(rb_parse_state *st, int top)
{
    st->variables = LocalState::push(st->variables);
}

static void
mel_local_pop(rb_parse_state *st)
{
    st->variables = LocalState::pop(st->variables);
}


static QUID*
mel_local_tbl(rb_parse_state *st)
{
    QUID *lcl_tbl;
    var_table tbl;
    int i, len;
    tbl = st->variables->variables;
    len = var_table_size(tbl);
    lcl_tbl = (QUID*)pt_allocate(st, sizeof(QUID) * (len + 3));
    lcl_tbl[0] = (QUID)len;
    lcl_tbl[1] = '_';
    lcl_tbl[2] = '~';
    for(i = 0; i < len; i++) {
        lcl_tbl[i + 3] = var_table_get(tbl, i);
    }
    return lcl_tbl;
}

static intptr_t
mel_local_cnt(rb_parse_state *st, QUID id)
{
    int idx;
    /* Leave these hardcoded here because they arne't REALLY ids at all. */
    if(id == '_') {
        return 0;
    } else if(id == '~') {
        return 1;
    }

    // if there are block variables, check to see if there is already
    // a local by this name. If not, create one in the top block_vars
    // table.
    if(st->variables->block_vars) {
      idx = var_table_find_chained(st->variables->block_vars, id);
      if(idx >= 0) {
        return idx;
      } else {
        return var_table_add(st->variables->block_vars, id);
      }
    }

    idx = var_table_find(st->variables->variables, id);
    if(idx >= 0) {
      return idx + 2;
    }

    return var_table_add(st->variables->variables, id);
}

static int
mel_local_id(rb_parse_state *st, QUID id)
{
    if(st->variables->block_vars) {
      if(var_table_find_chained(st->variables->block_vars, id) >= 0) return 1;
    }

    if(var_table_find(st->variables->variables, id) >= 0) return 1;
    return 0;
}

static QUID
rb_parser_sym(const char *name)
{
    const char *m = name;
    QUID id, pre, qrk, bef;
    int last;

    id = 0;
    last = strlen(name)-1;
    switch (*name) {
      case '$':
        id |= ID_GLOBAL;
        m++;
        if (!is_identchar(*m)) m++;
        break;
      case '@':
        if (name[1] == '@') {
            m++;
            id |= ID_CLASS;
        }
        else {
            id |= ID_INSTANCE;
        }
        m++;
        break;
      default:
        if (name[0] != '_' && !ISALPHA(name[0]) && !ismbchar(name[0])) {
            int i;

            for (i=0; op_tbl[i].token; i++) {
                if (*op_tbl[i].name == *name &&
                    strcmp(op_tbl[i].name, name) == 0) {
                    id = op_tbl[i].token;
                    return id;
                }
            }
        }

        if (name[last] == '=') {
            id = ID_ATTRSET;
        }
        else if (ISUPPER(name[0])) {
            id = ID_CONST;
        }
        else {
            id = ID_LOCAL;
        }
        break;
    }
    while (m <= name + last && is_identchar(*m)) {
        m += mbclen(*m);
    }
    if (*m) id = ID_JUNK;
    qrk = (QUID)quark_from_string(name);
    pre = qrk + tLAST_TOKEN;
    bef = id;
    id |= ( pre << ID_SCOPE_SHIFT );
    return id;
}

static unsigned long
scan_oct(const char *start, int len, int *retlen)
{
    register const char *s = start;
    register unsigned long retval = 0;

    while (len-- && *s >= '0' && *s <= '7') {
        retval <<= 3;
        retval |= *s++ - '0';
    }
    *retlen = s - start;
    return retval;
}

static unsigned long
scan_hex(const char *start, int len, int *retlen)
{
    static const char hexdigit[] = "0123456789abcdef0123456789ABCDEF";
    register const char *s = start;
    register unsigned long retval = 0;
    const char *tmp;

    while (len-- && *s && (tmp = strchr(hexdigit, *s))) {
        retval <<= 4;
        retval |= (tmp - hexdigit) & 15;
        s++;
    }
    *retlen = s - start;
    return retval;
}

const char *op_to_name(QUID id) {
  if(id < tLAST_TOKEN) {
    int i = 0;

    for (i=0; op_tbl[i].token; i++) {
        if (op_tbl[i].token == id)
            return op_tbl[i].name;
    }
  }
  return NULL;
}
quark id_to_quark(QUID id) {
  quark qrk;

  qrk = (quark)((id >> ID_SCOPE_SHIFT) - tLAST_TOKEN);
  return qrk;
}

}; // namespace melbourne

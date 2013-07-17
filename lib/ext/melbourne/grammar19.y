/***********************************************************************

  parse.y -

  $Author: matz $
  $Date: 2004/11/29 06:13:51 $
  created at: Fri May 28 18:02:42 JST 1993

  Copyright (C) 1993-2003 Yukihiro Matsumoto

**********************************************************************/

%{

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

#include "melbourne.hpp"
#include "grammar19.hpp"
#include "parser_state19.hpp"
#include "visitor19.hpp"
#include "symbols.hpp"

namespace melbourne {

namespace grammar19 {

#undef TRUE
#undef FALSE

#define TRUE  true
#define FALSE false

#define numberof(array) (int)(sizeof(array) / sizeof((array)[0]))

static void parser_prepare(rb_parser_state*);
static int parser_yyerror(rb_parser_state*, const char *);
#define yy_error(msg)   parser_yyerror(parser_state, msg)
#define yyerror         parser_yyerror

#define YYLEX_PARAM parser_state

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

static int yylex(void*, void *);

#define BITSTACK_PUSH(stack, n) (stack = (stack<<1)|((n)&1))
#define BITSTACK_POP(stack)     (stack >>= 1)
#define BITSTACK_LEXPOP(stack)  (stack = (stack >> 1) | (stack & 1))
#define BITSTACK_SET_P(stack)   (stack&1)

#define COND_PUSH(n)    BITSTACK_PUSH(cond_stack, n)
#define COND_POP()      BITSTACK_POP(cond_stack)
#define COND_LEXPOP()   BITSTACK_LEXPOP(cond_stack)
#define COND_P()        BITSTACK_SET_P(cond_stack)

#define CMDARG_PUSH(n)  BITSTACK_PUSH(cmdarg_stack, n)
#define CMDARG_POP()    BITSTACK_POP(cmdarg_stack)
#define CMDARG_LEXPOP() BITSTACK_LEXPOP(cmdarg_stack)
#define CMDARG_P()      BITSTACK_SET_P(cmdarg_stack)

static void parser_token_info_push(rb_parser_state*, const char *);
static void parser_token_info_pop(rb_parser_state*, const char *);
#define token_info_push(token)  (RTEST(ruby_verbose) \
                                 ? parser_token_info_push(parser_state, token) \
                                 : (void)0)
#define token_info_pop(token)   (RTEST(ruby_verbose) \
                                 ? parser_token_info_pop(parser_state, token) \
                                 : (void)0)

static NODE *parser_cond(rb_parser_state*, NODE*);
static NODE *parser_logop(rb_parser_state*, enum node_type, NODE*, NODE*);

static NODE *parser_newline_node(rb_parser_state*,NODE*);
static void fixpos(NODE*,NODE*);

static int parser_value_expr(rb_parser_state*, NODE*);
static void parser_void_expr0(rb_parser_state*, NODE*);
static NODE* remove_begin(NODE*);
static void parser_void_stmts(rb_parser_state*, NODE*);

static void parser_block_dup_check(rb_parser_state*, NODE*, NODE*);
static NODE *parser_block_append(rb_parser_state*, NODE*, NODE*);
static NODE *parser_list_append(rb_parser_state*, NODE*, NODE*);
static NODE *list_concat(NODE*,NODE*);
static NODE *parser_arg_append(rb_parser_state*, NODE*, NODE*);
static NODE *parser_arg_concat(rb_parser_state*, NODE*, NODE*);
static NODE *parser_literal_concat(rb_parser_state*, NODE*, NODE*);
static int parser_literal_concat0(rb_parser_state*, VALUE, VALUE);
static NODE *parser_new_evstr(rb_parser_state*, NODE*);
static NODE *parser_evstr2dstr(rb_parser_state*, NODE*);
static NODE *parser_call_bin_op(rb_parser_state*, NODE*, ID, NODE*);
static NODE *parser_call_uni_op(rb_parser_state*, NODE*, ID);
static NODE *parser_new_args(rb_parser_state*, NODE*, NODE*, ID, NODE*, ID);
static NODE *splat_array(NODE*);

static NODE *parser_negate_lit(rb_parser_state*, NODE*);
static NODE *parser_ret_args(rb_parser_state*, NODE*);
static NODE *arg_blk_pass(NODE*,NODE*);
static NODE *parser_new_yield(rb_parser_state*, NODE*);

static NODE *parser_gettable(rb_parser_state*,ID);
#define gettable(i) parser_gettable((rb_parser_state*)parser_state, i)
static NODE *parser_assignable(rb_parser_state*, ID, NODE*);
static ID parser_formal_argument(rb_parser_state*, ID);
static ID parser_shadowing_lvar(rb_parser_state*, ID);
static bool parser_lvar_defined(rb_parser_state*, ID);
static void parser_new_bv(rb_parser_state*, ID);
static const struct vtable* parser_bv_push(rb_parser_state*);
static void parser_bv_pop(rb_parser_state*, const struct vtable*);
static bool parser_in_block(rb_parser_state*);
static bool parser_bv_defined(rb_parser_state*, ID);
static int parser_bv_var(rb_parser_state*, ID);
static NODE *parser_aryset(rb_parser_state*, NODE*, NODE*);
static NODE *parser_attrset(rb_parser_state*, NODE*, ID);
static void rb_parser_backref_error(rb_parser_state*, NODE*);
static NODE *parser_node_assign(rb_parser_state*, NODE*, NODE*);

static NODE *parser_match_op(rb_parser_state*, NODE*, NODE*);
static int parser_arg_var(rb_parser_state*, ID);
static int parser_local_var(rb_parser_state*, ID);
static ID parser_internal_id(rb_parser_state*);

static void parser_local_push(rb_parser_state*, int cnt);
static void parser_local_pop(rb_parser_state*);
static bool parser_local_id(rb_parser_state*, ID);
static ID* parser_local_tbl(rb_parser_state*);
static ID convert_op(ID id);

rb_parser_state *parser_alloc_state() {
  rb_parser_state *parser_state = (rb_parser_state*)calloc(1, sizeof(rb_parser_state));

  lex_pbeg = 0;
  lex_p = 0;
  lex_pend = 0;
  parse_error = false;

  lex_io_buf = 0;
  lex_io_index = 0;
  lex_io_total = 0;
  lex_io_count = 0;

  eofp = false;
  command_start = true;
  class_nest = 0;
  in_single = 0;
  in_def = 0;
  compile_for_eval = 0;
  cur_mid = 0;
  tokenbuf = NULL;
  tokidx = 0;
  toksiz = 0;
  locals_table = 0;
  memory_cur = NULL;
  memory_last_addr = NULL;
  current_pool = 0;
  pool_size = 0;
  memory_size = 204800;
  memory_pools = NULL;
  emit_warnings = 0;
  verbose = RTEST(ruby_verbose);
  start_lines = new std::list<StartPosition>;

  processor = 0;
  references = rb_ary_new();

  parser_state->enc = parser_usascii_encoding();

  return parser_state;
}

void *pt_allocate(rb_parser_state *parser_state, int size) {
  void *cur;

  if(!memory_cur || ((memory_cur + size) >= memory_last_addr)) {
    if(memory_cur) current_pool++;

    if(current_pool == pool_size) {
      pool_size += 10;
      if(memory_pools) {
        memory_pools = (void**)realloc(memory_pools, sizeof(void*) * pool_size);
      } else {
        memory_pools = (void**)malloc(sizeof(void*) * pool_size);
      }
    }
    memory_pools[current_pool] = malloc(memory_size);
    memory_cur = (char*)memory_pools[current_pool];
    memory_last_addr = memory_cur + memory_size - 1;
  }

  cur = (void*)memory_cur;
  memory_cur = memory_cur + size;

  return cur;
}

void pt_free(rb_parser_state *parser_state) {
  int i;

  free(tokenbuf);
  local_vars_free(locals_table);

  delete start_lines;

  if(lex_io_buf) free(lex_io_buf);

  if(!memory_pools) return;

  for(i = 0; i <= current_pool; i++) {
    free(memory_pools[i]);
  }
  free(memory_pools);
}

#define SHOW_PARSER_WARNS 0

static void rb_compile_warn(const char *file, int line, const char *fmt, ...);
static void rb_compile_warning(const char *file, int line, const char *fmt, ...);

static int rb_compile_error(rb_parser_state* parser_state, const char *fmt, ...) {
  va_list ar;
  char msg[256];
  int count;

  va_start(ar, fmt);
  count = vsnprintf(msg, 256, fmt, ar);
  va_end(ar);

  yy_error(msg);

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

void push_start_line(rb_parser_state* parser_state, int line, const char* which) {
  start_lines->push_back(StartPosition(line, which));
}

#define PUSH_LINE(which) push_start_line((rb_parser_state*)parser_state, sourceline, which)

void pop_start_line(rb_parser_state* parser_state) {
  start_lines->pop_back();
}

#define POP_LINE() pop_start_line((rb_parser_state*)parser_state)

static ID rb_id_attrset(ID);

static int scan_oct(const char *start, size_t len, size_t *retlen);
static int scan_hex(const char *start, size_t len, size_t *retlen);

#define logop(t, a, b)            parser_logop(parser_state, t, a, b)
#define cond(n)                   parser_cond(parser_state, n)
#define newline_node(n)           parser_newline_node(parser_state, n)
#define void_stmts(n)             parser_void_stmts(parser_state, n)
#define block_dup_check(a, b)     parser_block_dup_check(parser_state, a, b)
#define block_append(a, b)        parser_block_append(parser_state, a, b)
#define arg_append(a, b)          parser_arg_append(parser_state, a, b)
#define arg_concat(a, b)          parser_arg_concat(parser_state, a, b)
#define list_append(l, i)         parser_list_append(parser_state, l, i)
#define node_assign(a, b)         parser_node_assign(parser_state, a, b)
#define call_bin_op(a, s, b)      parser_call_bin_op(parser_state, a, s, b)
#define call_uni_op(n, s)         parser_call_uni_op(parser_state, n, s)
#define new_args(f,o,r,p,b)       parser_new_args(parser_state, f, o, r, p, b)
#define negate_lit(n)             parser_negate_lit(parser_state, n)
#define ret_args(n)               parser_ret_args(parser_state, n)
#define assignable(a, b)          parser_assignable(parser_state, a, b)
#define formal_argument(n)        parser_formal_argument(parser_state, n)
#define lvar_defined(n)           parser_lvar_defined(parser_state, n)
#define shadowing_lvar(n)         parser_shadowing_lvar(parser_state, n)
#define new_bv(n)                 parser_new_bv(parser_state, n)
#define bv_push()                 parser_bv_push(parser_state)
#define bv_pop(l)                 parser_bv_pop(parser_state, l)
#define in_block()                parser_in_block(parser_state)
#define bv_defined(n)             parser_bv_defined(parser_state, n)
#define bv_var(n)                 parser_bv_var(parser_state, n)
#define aryset(a, b)              parser_aryset(parser_state, a, b)
#define attrset(a, b)             parser_attrset(parser_state, a, b)
#define match_op(a, b)            parser_match_op(parser_state, a, b)
#define new_yield(n)              parser_new_yield(parser_state, n)
#define evstr2dstr(n)             parser_evstr2dstr(parser_state, n)
#define literal_concat(a, b)      parser_literal_concat(parser_state, a, b)
#define literal_concat0(a, b)     parser_literal_concat0(parser_state, a, b)
#define new_evstr(n)              parser_new_evstr(parser_state, n)

#define value_expr(n)             parser_value_expr(parser_state, n)
#define void_expr0(n)             parser_void_expr0(parser_state, n)
#define void_expr(n)              void_expr0(((n) = remove_begin(n)))

#define local_tbl()               parser_local_tbl(parser_state)
#define arg_var(a)                parser_arg_var(parser_state, a)
#define local_var(a)              parser_local_var(parser_state, a)
#define internal_id()             parser_internal_id(parser_state)

#define compile_error(s)          rb_compile_error(parser_state, s)
#define rb_backref_error(s)       rb_parser_backref_error(parser_state, s)

#define get_id(id)                (id)
#define get_value(value)          (value)

#define local_push(cnt)           parser_local_push(parser_state, cnt)
#define local_pop()               parser_local_pop(parser_state)
#define local_id(i)               parser_local_id(parser_state, i)

#define rb_warn0(fmt)             rb_compile_warn(sourcefile, sourceline, fmt)
#define rb_warnI(fmt,a)           rb_compile_warn(sourcefile, sourceline, fmt, a)
#define rb_warnS(fmt,a)           rb_compile_warn(sourcefile, sourceline, fmt, a)
#define rb_warning0(fmt)          rb_compile_warning(sourcefile, sourceline, fmt)
#define rb_warningS(fmt,a)        rb_compile_warning(sourcefile, sourceline, fmt, a)


#ifndef RE_OPTION_IGNORECASE
#define RE_OPTION_IGNORECASE         (1)
#endif

#ifndef RE_OPTION_EXTENDED
#define RE_OPTION_EXTENDED           (2)
#endif

#ifndef RE_OPTION_MULTILINE
#define RE_OPTION_MULTILINE          (4)
#endif

/* Must match up with options/kcode definitions in regexp.rb and regexp.cpp */
#define RE_OPTION_DONT_CAPTURE_GROUP (128)
#define RE_OPTION_CAPTURE_GROUP      (256)
#define RE_KCODE_NONE                (1 << 9)
#define RE_KCODE_EUC                 (2 << 9)
#define RE_KCODE_SJIS                (3 << 9)
#define RE_KCODE_UTF8                (4 << 9)
#define RE_OPTION_ONCE               (8192)

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
#define nd_nest u3.cnt

#define UTF8_ENC()            (parser_state->utf8 ? parser_state->utf8 : \
                                (parser_state->utf8 = parser_utf8_encoding()))
#define STR_NEW(p,n)          parser_enc_str_new((p), (n), parser_state->enc)
#define STR_NEW0()            parser_enc_str_new(0, 0, parser_state->enc)
#define STR_NEW2(p)           parser_enc_str_new((p), strlen(p), parser_state->enc)
#define STR_NEW3(p,n,e,func)  parser_str_new(parser_state, (p), (n), (e), \
                                            (func), parser_state->enc)
#define ENC_SINGLE(cr)        ((cr)==ENC_CODERANGE_7BIT)
#define TOK_INTERN(mb)        parser_intern3(tok(), toklen(), parser_state->enc)

#define NEW_BLOCK_VAR(b, v) NEW_NODE(NODE_BLOCK_PASS, 0, b, v)

/* Older versions of Yacc set YYMAXDEPTH to a very low value by default (150,
   for instance).  This is too low for Ruby to parse some files, such as
   date/format.rb, therefore bump the value up to at least Bison's default. */
#ifdef OLD_YACC
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif
#endif

%}

%pure-parser
%parse-param {rb_parser_state* parser_state}

%union {
    VALUE val;
    NODE *node;
    ID id;
    int num;
    const struct vtable* vars;
}

%token
  keyword_class
  keyword_module
  keyword_def
  keyword_undef
  keyword_begin
  keyword_rescue
  keyword_ensure
  keyword_end
  keyword_if
  keyword_unless
  keyword_then
  keyword_elsif
  keyword_else
  keyword_case
  keyword_when
  keyword_while
  keyword_until
  keyword_for
  keyword_break
  keyword_next
  keyword_redo
  keyword_retry
  keyword_in
  keyword_do
  keyword_do_cond
  keyword_do_block
  keyword_do_LAMBDA
  keyword_return
  keyword_yield
  keyword_super
  keyword_self
  keyword_nil
  keyword_true
  keyword_false
  keyword_and
  keyword_or
  keyword_not
  modifier_if
  modifier_unless
  modifier_while
  modifier_until
  modifier_rescue
  keyword_alias
  keyword_defined
  keyword_BEGIN
  keyword_END
  keyword__LINE__
  keyword__FILE__
  keyword__ENCODING__

%token <id>   tIDENTIFIER tFID tGVAR tIVAR tCONSTANT tCVAR tLABEL
%token <node> tINTEGER tFLOAT tSTRING_CONTENT tCHAR
%token <node> tNTH_REF tBACK_REF
%token <num>  tREGEXP_END

%type <node> singleton strings string string1 xstring regexp
%type <node> string_contents xstring_contents regexp_contents string_content
%type <node> words qwords word_list qword_list word
%type <node> literal numeric dsym cpath
%type <node> top_compstmt top_stmts top_stmt
%type <node> bodystmt compstmt stmts stmt expr arg primary command command_call method_call
%type <node> expr_value arg_value primary_value
%type <node> if_tail opt_else case_body cases opt_rescue exc_list exc_var opt_ensure
%type <node> args call_args opt_call_args
%type <node> paren_args opt_paren_args
%type <node> command_args aref_args opt_block_arg block_arg var_ref var_lhs
%type <node> mrhs superclass block_call block_command
%type <node> f_block_optarg f_block_opt
%type <node> f_arglist f_args f_arg f_arg_item f_optarg f_marg f_marg_list f_margs
%type <node> assoc_list assocs assoc undef_list backref string_dvar for_var
%type <node> block_param opt_block_param block_param_def f_opt
%type <node> bv_decls opt_bv_decl bvar
%type <node> lambda f_larglist lambda_body
%type <node> brace_block cmd_brace_block do_block lhs none fitem
%type <node> mlhs mlhs_head mlhs_basic mlhs_item mlhs_node mlhs_post mlhs_inner
%type <id>   fsym variable sym symbol operation operation2 operation3
%type <id>   cname fname op f_rest_arg f_block_arg opt_f_block_arg f_norm_arg f_bad_arg

%token tUPLUS           /* unary+ */
%token tUMINUS          /* unary- */
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
%token tLAMBDA          /* -> */
%token tSYMBEG tSTRING_BEG tXSTRING_BEG tREGEXP_BEG tWORDS_BEG tQWORDS_BEG
%token tSTRING_DBEG tSTRING_DVAR tSTRING_END tLAMBEG

/*
 *      precedence table
 */

%nonassoc tLOWEST
%nonassoc tLBRACE_ARG

%nonassoc  modifier_if modifier_unless modifier_while modifier_until
%left  keyword_or keyword_and
%right keyword_not
%nonassoc keyword_defined
%right '=' tOP_ASGN
%left modifier_rescue
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
program         : {
                    lex_state = EXPR_BEG;
                    local_push(0);
                    class_nest = 0;
                  }
                  top_compstmt
                  {
                    if($2 && !compile_for_eval) {
                      /* last expression should not be void */
                      if(nd_type($2) != NODE_BLOCK) {
                        void_expr($2);
                      } else {
                        NODE *node = $2;
                        while(node->nd_next) {
                          node = node->nd_next;
                        }
                        void_expr(node->nd_head);
                      }
                    }
                    top_node = block_append(top_node, $2);
                    class_nest = 0;
                    local_pop();
                  }
                ;

top_compstmt    : top_stmts opt_terms
                  {
                    void_stmts($1);
                    $$ = $1;
                  }
                ;

top_stmts       : none
                  {
                    $$ = NEW_BEGIN(0);
                  }
                | top_stmt
                  {
                    $$ = newline_node($1);
                  }
                | top_stmts terms top_stmt
                  {
                    $$ = block_append($1, newline_node($3));
                  }
                | error top_stmt
                  {
                    $$ = remove_begin($2);
                  }
                ;

top_stmt        : stmt
                | keyword_BEGIN
                  {
                    if(in_def || in_single) {
                      yy_error("BEGIN in method");
                    }
                  }
                  '{' top_compstmt '}'
                  {
                    $$ = NEW_PREEXE($4);
                  }
                ;

bodystmt        : compstmt
                  opt_rescue
                  opt_else
                  opt_ensure
                  {
                    $$ = $1;
                    if($2) {
                      $$ = NEW_RESCUE($1, $2, $3);
                    } else if($3) {
                      rb_warn("else without rescue is useless");
                      $$ = block_append($$, $3);
                    }
                    if($4) {
                      if($$) {
                        $$ = NEW_ENSURE($$, $4);
                      } else {
                        $$ = block_append($4, NEW_NIL());
                      }
                    }
                    fixpos($$, $1);
                  }
                ;

compstmt        : stmts opt_terms
                  {
                    void_stmts($1);
                    $$ = $1;
                  }
                ;

stmts           : none
                  {
                    $$ = NEW_BEGIN(0);
                  }
                | stmt
                  {
                    $$ = newline_node($1);
                  }
                | stmts terms stmt
                  {
                    $$ = block_append($1, newline_node($3));
                  }
                | error stmt
                  {
                    $$ = remove_begin($2);
                  }
                ;

stmt            : keyword_alias fitem {lex_state = EXPR_FNAME;} fitem
                  {
                    $$ = NEW_ALIAS($2, $4);
                  }
                | keyword_alias tGVAR tGVAR
                  {
                    $$ = NEW_VALIAS($2, $3);
                  }
                | keyword_alias tGVAR tBACK_REF
                  {
                    char buf[2];
                    buf[0] = '$';
                    buf[1] = (char)$3->nd_nth;
                    $$ = NEW_VALIAS($2, parser_intern2(buf, 2));
                  }
                | keyword_alias tGVAR tNTH_REF
                  {
                    yy_error("can't make alias for the number variables");
                    $$ = NEW_BEGIN(0);
                  }
                | keyword_undef undef_list
                  {
                    $$ = $2;
                  }
                | stmt modifier_if expr_value
                  {
                    $$ = NEW_IF(cond($3), remove_begin($1), 0);
                    fixpos($$, $3);
                  }
                | stmt modifier_unless expr_value
                  {
                    $$ = NEW_UNLESS(cond($3), remove_begin($1), 0);
                    fixpos($$, $3);
                  }
                | stmt modifier_while expr_value
                  {
                    if($1 && nd_type($1) == NODE_BEGIN) {
                      $$ = NEW_WHILE(cond($3), $1->nd_body, 0);
                    } else {
                      $$ = NEW_WHILE(cond($3), $1, 1);
                    }
                  }
                | stmt modifier_until expr_value
                  {
                    if($1 && nd_type($1) == NODE_BEGIN) {
                      $$ = NEW_UNTIL(cond($3), $1->nd_body, 0);
                    } else {
                      $$ = NEW_UNTIL(cond($3), $1, 1);
                    }
                  }
                | stmt modifier_rescue stmt
                  {
                    NODE *resq = NEW_RESBODY(0, remove_begin($3), 0);
                    $$ = NEW_RESCUE(remove_begin($1), resq, 0);
                  }
                | keyword_END '{' compstmt '}'
                  {
                    if(in_def || in_single) {
                      rb_warn("END in method; use at_exit");
                    }

                    $$ = NEW_POSTEXE(NEW_NODE(NODE_SCOPE,
                            0 /* tbl */, $3 /* body */, 0 /* args */));
                  }
                | lhs '=' command_call
                  {
                    value_expr($3);
                    $$ = node_assign($1, $3);
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
                    if($1) {
                      ID vid = $1->nd_vid;
                      if($2 == tOROP) {
                        $1->nd_value = $3;
                        $$ = NEW_OP_ASGN_OR(gettable(vid), $1);
                        if(is_asgn_or_id(vid)) {
                          $$->nd_aid = vid;
                        }
                      } else if($2 == tANDOP) {
                        $1->nd_value = $3;
                        $$ = NEW_OP_ASGN_AND(gettable(vid), $1);
                      } else {
                        $$ = $1;
                        $$->nd_value = call_bin_op(gettable(vid), $2, $3);
                      }
                    } else {
                      $$ = NEW_BEGIN(0);
                    }
                  }
                | primary_value '[' opt_call_args rbracket tOP_ASGN command_call
                  {
                    NODE *args;

                    value_expr($6);
                    if(!$3) $3 = NEW_ZARRAY();
                    args = arg_concat($3, $6);
                    if($5 == tOROP) {
                      $5 = 0;
                    } else if($5 == tANDOP) {
                      $5 = 1;
                    } else {
                      $5 = convert_op($5);
                    }
                    $$ = NEW_OP_ASGN1($1, $5, args);
                    fixpos($$, $1);
                  }
                | primary_value '.' tIDENTIFIER tOP_ASGN command_call
                  {
                    value_expr($5);
                    if($4 == tOROP) {
                      $4 = 0;
                    } else if($4 == tANDOP) {
                      $4 = 1;
                    } else {
                      $4 = convert_op($4);
                    }
                    $$ = NEW_OP_ASGN2($1, $3, $4, $5);
                    fixpos($$, $1);
                  }
                | primary_value '.' tCONSTANT tOP_ASGN command_call
                  {
                    value_expr($5);
                    if($4 == tOROP) {
                      $4 = 0;
                    } else if($4 == tANDOP) {
                      $4 = 1;
                    } else {
                      $4 = convert_op($4);
                    }
                    $$ = NEW_OP_ASGN2($1, $3, $4, $5);
                    fixpos($$, $1);
                  }
                | primary_value tCOLON2 tCONSTANT tOP_ASGN command_call
                  {
                    yy_error("constant re-assignment");
                    $$ = 0;
                  }
                | primary_value tCOLON2 tIDENTIFIER tOP_ASGN command_call
                  {
                    value_expr($5);
                    if($4 == tOROP) {
                      $4 = 0;
                    } else if($4 == tANDOP) {
                      $4 = 1;
                    } else {
                      $4 = convert_op($4);
                    }
                    $$ = NEW_OP_ASGN2($1, $3, $4, $5);
                    fixpos($$, $1);
                  }
                | backref tOP_ASGN command_call
                  {
                    rb_backref_error($1);
                    $$ = 0;
                  }
                | lhs '=' mrhs
                  {
                    value_expr($3);
                    $$ = node_assign($1, $3);
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
                | expr keyword_and expr
                  {
                    $$ = logop(NODE_AND, $1, $3);
                  }
                | expr keyword_or expr
                  {
                    $$ = logop(NODE_OR, $1, $3);
                  }
                | keyword_not opt_nl expr
                  {
                    $$ = call_uni_op(cond($3), '!');
                  }
                | '!' command_call
                  {
                    $$ = call_uni_op(cond($2), '!');
                  }
                | arg
                ;

expr_value      : expr
                  {
                    value_expr($1);
                    $$ = $1;
                    if(!$$) $$ = NEW_NIL();
                  }
                ;

command_call    : command
                | block_command
                ;

block_command   : block_call
                | block_call '.' operation2 command_args
                  {
                    $$ = NEW_CALL($1, $3, $4);
                  }
                | block_call tCOLON2 operation2 command_args
                  {
                    $$ = NEW_CALL($1, $3, $4);
                  }
                ;

cmd_brace_block : tLBRACE_ARG
                  {
                    $<vars>1 = bv_push();
                    $<num>$ = sourceline;
                  }
                  opt_block_param
                  compstmt
                  '}'
                  {
                    $$ = NEW_ITER($3, $4);
                    nd_set_line($$, $<num>2);
                    bv_pop($<vars>1);
                  }
                ;

command         : operation command_args       %prec tLOWEST
                  {
                    $$ = NEW_FCALL($1, $2);
                    fixpos($$, $2);
                 }
                | operation command_args cmd_brace_block
                  {
                    block_dup_check($2, $3);
                    $3->nd_iter = NEW_FCALL($1, $2);
                    $$ = $3;
                    fixpos($$, $2);
                 }
                | primary_value '.' operation2 command_args     %prec tLOWEST
                  {
                    $$ = NEW_CALL($1, $3, $4);
                    fixpos($$, $1);
                  }
                | primary_value '.' operation2 command_args cmd_brace_block
                  {
                    block_dup_check($4, $5);
                    $5->nd_iter = NEW_CALL($1, $3, $4);
                    $$ = $5;
                    fixpos($$, $1);
                 }
                | primary_value tCOLON2 operation2 command_args %prec tLOWEST
                  {
                    $$ = NEW_CALL($1, $3, $4);
                    fixpos($$, $1);
                  }
                | primary_value tCOLON2 operation2 command_args cmd_brace_block
                  {
                    block_dup_check($4, $5);
                    $5->nd_iter = NEW_CALL($1, $3, $4);
                    $$ = $5;
                    fixpos($$, $1);
                  }
                | keyword_super command_args
                  {
                    $$ = NEW_SUPER($2);
                    fixpos($$, $2);
                  }
                | keyword_yield command_args
                  {
                    $$ = new_yield($2);
                    fixpos($$, $2);
                  }
                | keyword_return call_args
                  {
                    $$ = NEW_RETURN(ret_args($2));
                  }
                | keyword_break call_args
                  {
                    $$ = NEW_BREAK(ret_args($2));
                  }
                | keyword_next call_args
                  {
                    $$ = NEW_NEXT(ret_args($2));
                  }
                ;

mlhs            : mlhs_basic
                | tLPAREN mlhs_inner rparen
                  {
                    $$ = $2;
                  }
                ;

mlhs_inner      : mlhs_basic
                | tLPAREN mlhs_inner rparen
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
                    $$ = NEW_MASGN(list_append($1, $2), 0);
                  }
                | mlhs_head tSTAR mlhs_node
                  {
                    $$ = NEW_MASGN($1, $3);
                  }
                | mlhs_head tSTAR mlhs_node ',' mlhs_post
                  {
                    $$ = NEW_MASGN($1, NEW_POSTARG($3, $5));
                  }
                | mlhs_head tSTAR
                  {
                    $$ = NEW_MASGN($1, -1);
                  }
                | mlhs_head tSTAR ',' mlhs_post
                  {
                    $$ = NEW_MASGN($1, NEW_POSTARG(-1, $4));
                  }
                | tSTAR mlhs_node
                  {
                    $$ = NEW_MASGN(0, $2);
                  }
                | tSTAR mlhs_node ',' mlhs_post
                  {
                    $$ = NEW_MASGN(0, NEW_POSTARG($2, $4));
                  }
                | tSTAR
                  {
                    $$ = NEW_MASGN(0, -1);
                  }
                | tSTAR ',' mlhs_post
                  {
                    $$ = NEW_MASGN(0, NEW_POSTARG(-1, $3));
                  }
                ;

mlhs_item       : mlhs_node
                | tLPAREN mlhs_inner rparen
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
                    $$ = list_append($1, $2);
                  }
                ;

mlhs_post       : mlhs_item
                  {
                    $$ = NEW_LIST($1);
                  }
                | mlhs_post ',' mlhs_item
                  {
                    $$ = list_append($1, $3);
                  }
                ;

mlhs_node       : variable
                  {
                    $$ = assignable($1, 0);
                  }
                | primary_value '[' opt_call_args rbracket
                  {
                    $$ = aryset($1, $3);
                  }
                | primary_value '.' tIDENTIFIER
                  {
                    $$ = attrset($1, $3);
                  }
                | primary_value tCOLON2 tIDENTIFIER
                  {
                    $$ = attrset($1, $3);
                  }
                | primary_value '.' tCONSTANT
                  {
                    $$ = attrset($1, $3);
                  }
                | primary_value tCOLON2 tCONSTANT
                  {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    $$ = NEW_CDECL(0, 0, NEW_COLON2($1, $3));
                  }
                | tCOLON3 tCONSTANT
                  {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    $$ = NEW_CDECL(0, 0, NEW_COLON3($2));
                  }
                | backref
                  {
                    rb_backref_error($1);
                    $$ = NEW_BEGIN(0);
                  }
                ;

lhs             : variable
                  {
                    $$ = assignable($1, 0);
                    if(!$$) $$ = NEW_BEGIN(0);
                  }
                | primary_value '[' opt_call_args rbracket
                  {
                    $$ = aryset($1, $3);
                  }
                | primary_value '.' tIDENTIFIER
                  {
                    $$ = attrset($1, $3);
                  }
                | primary_value tCOLON2 tIDENTIFIER
                  {
                    $$ = attrset($1, $3);
                  }
                | primary_value '.' tCONSTANT
                  {
                    $$ = attrset($1, $3);
                  }
                | primary_value tCOLON2 tCONSTANT
                  {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    $$ = NEW_CDECL(0, 0, NEW_COLON2($1, $3));
                  }
                | tCOLON3 tCONSTANT
                  {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    $$ = NEW_CDECL(0, 0, NEW_COLON3($2));
                  }
                | backref
                  {
                    rb_backref_error($1);
                    $$ = NEW_BEGIN(0);
                  }
                ;

cname           : tIDENTIFIER
                  {
                    yy_error("class/module name must be CONSTANT");
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
                    lex_state = EXPR_ENDFN;
                    $$ = convert_op($1);
                  }
                | reswords
                  {
                    lex_state = EXPR_ENDFN;
                    $$ = $<id>1;
                  }
                ;

fsym            : fname
                | symbol
                ;

fitem           : fsym
                  {
                    $$ = NEW_LIT(ID2SYM($1));
                  }
                | dsym
                ;

undef_list      : fitem
                  {
                    $$ = NEW_UNDEF($1);
                  }
                | undef_list ',' {lex_state = EXPR_FNAME;} fitem
                  {
                    $$ = block_append($1, NEW_UNDEF($4));
                  }
                ;

op              : '|'           { $$ = '|'; }
                | '^'           { $$ = '^'; }
                | '&'           { $$ = '&'; }
                | tCMP          { $$ = tCMP; }
                | tEQ           { $$ = tEQ; }
                | tEQQ          { $$ = tEQQ; }
                | tMATCH        { $$ = tMATCH; }
                | tNMATCH       { $$ = tNMATCH; }
                | '>'           { $$ = '>'; }
                | tGEQ          { $$ = tGEQ; }
                | '<'           { $$ = '<'; }
                | tLEQ          { $$ = tLEQ; }
                | tNEQ          { $$ = tNEQ; }
                | tLSHFT        { $$ = tLSHFT; }
                | tRSHFT        { $$ = tRSHFT; }
                | '+'           { $$ = '+'; }
                | '-'           { $$ = '-'; }
                | '*'           { $$ = '*'; }
                | tSTAR         { $$ = '*'; }
                | '/'           { $$ = '/'; }
                | '%'           { $$ = '%'; }
                | tPOW          { $$ = tPOW; }
                | '!'           { $$ = '!'; }
                | '~'           { $$ = '~'; }
                | tUPLUS        { $$ = tUPLUS; }
                | tUMINUS       { $$ = tUMINUS; }
                | tAREF         { $$ = tAREF; }
                | tASET         { $$ = tASET; }
                | '`'           { $$ = '`'; }
                ;

reswords        : keyword__LINE__ | keyword__FILE__ | keyword__ENCODING__
                | keyword_BEGIN | keyword_END
                | keyword_alias | keyword_and | keyword_begin
                | keyword_break | keyword_case | keyword_class | keyword_def
                | keyword_defined | keyword_do | keyword_else | keyword_elsif
                | keyword_end | keyword_ensure | keyword_false
                | keyword_for | keyword_in | keyword_module | keyword_next
                | keyword_nil | keyword_not | keyword_or | keyword_redo
                | keyword_rescue | keyword_retry | keyword_return | keyword_self
                | keyword_super | keyword_then | keyword_true | keyword_undef
                | keyword_when | keyword_yield | keyword_if | keyword_unless
                | keyword_while | keyword_until
                ;

arg             : lhs '=' arg
                  {
                    value_expr($3);
                    $$ = node_assign($1, $3);
                  }
                | lhs '=' arg modifier_rescue arg
                  {
                    value_expr($3);
                    $3 = NEW_RESCUE($3, NEW_RESBODY(0, $5, 0), 0);
                    $$ = node_assign($1, $3);
                  }
                | var_lhs tOP_ASGN arg
                  {
                    value_expr($3);
                    if($1) {
                      ID vid = $1->nd_vid;
                      if($2 == tOROP) {
                        $1->nd_value = $3;
                        $$ = NEW_OP_ASGN_OR(gettable(vid), $1);
                        if(is_asgn_or_id(vid)) {
                          $$->nd_aid = vid;
                        }
                      } else if($2 == tANDOP) {
                        $1->nd_value = $3;
                        $$ = NEW_OP_ASGN_AND(gettable(vid), $1);
                      } else {
                        $$ = $1;
                        $$->nd_value = NEW_CALL(gettable(vid), $2, NEW_LIST($3));
                      }
                    } else {
                      $$ = NEW_BEGIN(0);
                    }
                  }
                | var_lhs tOP_ASGN arg modifier_rescue arg
                  {
                    value_expr($3);
                    $3 = NEW_RESCUE($3, NEW_RESBODY(0, $5, 0), 0);
                    if($1) {
                      ID vid = $1->nd_vid;
                      if($2 == tOROP) {
                        $1->nd_value = $3;
                        $$ = NEW_OP_ASGN_OR(gettable(vid), $1);
                        if(is_asgn_or_id(vid)) {
                          $$->nd_aid = vid;
                        }
                      } else if($2 == tANDOP) {
                        $1->nd_value = $3;
                        $$ = NEW_OP_ASGN_AND(gettable(vid), $1);
                      } else {
                        $$ = $1;
                        $$->nd_value = NEW_CALL(gettable(vid), $2, NEW_LIST($3));
                      }
                    } else {
                      $$ = NEW_BEGIN(0);
                    }
                  }
                | primary_value '[' opt_call_args rbracket tOP_ASGN arg
                  {
                    NODE *args;

                    value_expr($6);
                    if(!$3) $3 = NEW_ZARRAY();
                    if(nd_type($3) == NODE_BLOCK_PASS) {
                      args = NEW_ARGSCAT($3, $6);
                    } else {
                      args = arg_concat($3, $6);
                    }
                    if($5 == tOROP) {
                      $5 = 0;
                    } else if($5 == tANDOP) {
                      $5 = 1;
                    } else {
                      $5 = convert_op($5);
                    }
                    $$ = NEW_OP_ASGN1($1, $5, args);
                    fixpos($$, $1);
                  }
                | primary_value '.' tIDENTIFIER tOP_ASGN arg
                  {
                    value_expr($5);
                    if($4 == tOROP) {
                      $4 = 0;
                    } else if($4 == tANDOP) {
                      $4 = 1;
                    } else {
                      $4 = convert_op($4);
                    }
                    $$ = NEW_OP_ASGN2($1, $3, $4, $5);
                    fixpos($$, $1);
                  }
                | primary_value '.' tCONSTANT tOP_ASGN arg
                  {
                    value_expr($5);
                    if($4 == tOROP) {
                      $4 = 0;
                    } else if($4 == tANDOP) {
                      $4 = 1;
                    } else {
                      $4 = convert_op($4);
                    }
                    $$ = NEW_OP_ASGN2($1, $3, $4, $5);
                    fixpos($$, $1);
                  }
                | primary_value tCOLON2 tIDENTIFIER tOP_ASGN arg
                  {
                    value_expr($5);
                    if($4 == tOROP) {
                      $4 = 0;
                    } else if($4 == tANDOP) {
                      $4 = 1;
                    } else {
                      $4 = convert_op($4);
                    }
                    $$ = NEW_OP_ASGN2($1, $3, $4, $5);
                    fixpos($$, $1);
                  }
                | primary_value tCOLON2 tCONSTANT tOP_ASGN arg
                  {
                    yy_error("constant re-assignment");
                    $$ = NEW_BEGIN(0);
                  }
                | tCOLON3 tCONSTANT tOP_ASGN arg
                  {
                    yy_error("constant re-assignment");
                    $$ = NEW_BEGIN(0);
                  }
                | backref tOP_ASGN arg
                  {
                    rb_backref_error($1);
                    $$ = NEW_BEGIN(0);
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
                    $$ = call_bin_op($1, '+', $3);
                  }
                | arg '-' arg
                  {
                    $$ = call_bin_op($1, '-', $3);
                  }
                | arg '*' arg
                  {
                    $$ = call_bin_op($1, '*', $3);
                  }
                | arg '/' arg
                  {
                    $$ = call_bin_op($1, '/', $3);
                  }
                | arg '%' arg
                  {
                    $$ = call_bin_op($1, '%', $3);
                  }
                | arg tPOW arg
                  {
                    $$ = call_bin_op($1, tPOW, $3);
                  }
                | tUMINUS_NUM tINTEGER tPOW arg
                  {
                    $$ = NEW_CALL(call_bin_op($2, tPOW, $4), tUMINUS, 0);
                  }
                | tUMINUS_NUM tFLOAT tPOW arg
                  {
                    $$ = NEW_CALL(call_bin_op($2, tPOW, $4), tUMINUS, 0);
                  }
                | tUPLUS arg
                  {
                    $$ = call_uni_op($2, tUPLUS);
                  }
                | tUMINUS arg
                  {
                    $$ = call_uni_op($2, tUMINUS);
                  }
                | arg '|' arg
                  {
                    $$ = call_bin_op($1, '|', $3);
                  }
                | arg '^' arg
                  {
                    $$ = call_bin_op($1, '^', $3);
                  }
                | arg '&' arg
                  {
                    $$ = call_bin_op($1, '&', $3);
                  }
                | arg tCMP arg
                  {
                    $$ = call_bin_op($1, tCMP, $3);
                  }
                | arg '>' arg
                  {
                    $$ = call_bin_op($1, '>', $3);
                  }
                | arg tGEQ arg
                  {
                    $$ = call_bin_op($1, tGEQ, $3);
                  }
                | arg '<' arg
                  {
                    $$ = call_bin_op($1, '<', $3);
                  }
                | arg tLEQ arg
                  {
                    $$ = call_bin_op($1, tLEQ, $3);
                  }
                | arg tEQ arg
                  {
                    $$ = call_bin_op($1, tEQ, $3);
                  }
                | arg tEQQ arg
                  {
                    $$ = call_bin_op($1, tEQQ, $3);
                  }
                | arg tNEQ arg
                  {
                    $$ = call_bin_op($1, tNEQ, $3);
                  }
                | arg tMATCH arg
                  {
                    /* TODO */
                    $$ = match_op($1, $3);
                    /*
                    if(nd_type($1) == NODE_LIT && TYPE($1->nd_lit) == T_REGEXP) {
                      $$ = reg_named_capture_assign($1->nd_lit, $$);
                    }
                    */
                  }
                | arg tNMATCH arg
                  {
                    $$ = call_bin_op($1, tNMATCH, $3);
                  }
                | '!' arg
                  {
                    $$ = call_uni_op(cond($2), '!');
                  }
                | '~' arg
                  {
                    $$ = call_uni_op($2, '~');
                  }
                | arg tLSHFT arg
                  {
                    $$ = call_bin_op($1, tLSHFT, $3);
                  }
                | arg tRSHFT arg
                  {
                    $$ = call_bin_op($1, tRSHFT, $3);
                  }
                | arg tANDOP arg
                  {
                    $$ = logop(NODE_AND, $1, $3);
                  }
                | arg tOROP arg
                  {
                    $$ = logop(NODE_OR, $1, $3);
                  }
                | keyword_defined opt_nl {in_defined = 1;} arg
                  {
                    in_defined = 0;
                    $$ = NEW_DEFINED($4);
                  }
                | arg '?' arg opt_nl ':' arg
                  {
                    value_expr($1);
                    $$ = NEW_IF(cond($1), $3, $6);
                    fixpos($$, $1);
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
                    if(!$$) $$ = NEW_NIL();
                  }
                ;

aref_args       : none
                | args trailer
                  {
                    $$ = $1;
                  }
                | args ',' assocs trailer
                  {
                    $$ = arg_append($1, NEW_HASH($3));
                  }
                | assocs trailer
                  {
                    $$ = NEW_LIST(NEW_HASH($1));
                  }
                ;

paren_args      : '(' opt_call_args rparen
                  {
                    $$ = $2;
                  }
                ;

opt_paren_args  : none
                | paren_args
                ;

opt_call_args   : none
                | call_args
                ;

call_args       : command
                  {
                    value_expr($1);
                    $$ = NEW_LIST($1);
                  }
                | args opt_block_arg
                  {
                    $$ = arg_blk_pass($1, $2);
                  }
                | assocs opt_block_arg
                  {
                    $$ = NEW_LIST(NEW_HASH($1));
                    $$ = arg_blk_pass($$, $2);
                  }
                | args ',' assocs opt_block_arg
                  {
                    $$ = arg_append($1, NEW_HASH($3));
                    $$ = arg_blk_pass($$, $4);
                  }
                | block_arg
                ;

command_args    : {
                    $<val>$ = cmdarg_stack;
                    CMDARG_PUSH(1);
                  }
                  call_args
                  {
                    /* CMDARG_POP() */
                    cmdarg_stack = $<val>1;
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
                | ','
                  {
                    $$ = 0;
                  }
                | none
                  {
                    $$ = 0;
                  }
                ;

args            : arg_value
                  {
                    $$ = NEW_LIST($1);
                  }
                | tSTAR arg_value
                  {
                    $$ = NEW_SPLAT($2);
                  }
                | args ',' arg_value
                  {
                    NODE *n1;
                    if((n1 = splat_array($1)) != 0) {
                      $$ = list_append(n1, $3);
                    } else {
                      $$ = arg_append($1, $3);
                    }
                  }
                | args ',' tSTAR arg_value
                  {
                    NODE *n1;
                    if((nd_type($4) == NODE_ARRAY) && (n1 = splat_array($1)) != 0) {
                      $$ = list_concat(n1, $4);
                    } else {
                      $$ = arg_concat($1, $4);
                    }
                  }
                ;

mrhs            : args ',' arg_value
                  {
                    NODE *n1;
                    if((n1 = splat_array($1)) != 0) {
                      $$ = list_append(n1, $3);
                    } else {
                      $$ = arg_append($1, $3);
                    }
                  }
                | args ',' tSTAR arg_value
                  {
                    NODE *n1;
                    if(nd_type($4) == NODE_ARRAY && (n1 = splat_array($1)) != 0) {
                      $$ = list_concat(n1, $4);
                    } else {
                      $$ = arg_concat($1, $4);
                    }
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
                | k_begin
                  {
                    $<num>$ = sourceline;
                  }
                  bodystmt
                  k_end
                  {
                    if($3 == NULL) {
                      $$ = NEW_NIL();
                    } else {
                      if(nd_type($3) == NODE_RESCUE || nd_type($3) == NODE_ENSURE) {
                        nd_set_line($3, $<num>2);
                      }
                      $$ = NEW_BEGIN($3);
                    }
                    nd_set_line($$, $<num>2);
                  }
                | tLPAREN_ARG expr {lex_state = EXPR_ENDARG;} rparen
                  {
                    rb_warning("(...) interpreted as grouped expression");
                    $$ = $2;
                  }
                | tLPAREN compstmt ')'
                  {
                    $$ = $2;
                  }
                | primary_value tCOLON2 tCONSTANT
                  {
                    $$ = NEW_COLON2($1, $3);
                  }
                | tCOLON3 tCONSTANT
                  {
                    $$ = NEW_COLON3($2);
                  }
                | tLBRACK aref_args ']'
                  {
                    if($2 == 0) {
                      $$ = NEW_ZARRAY(); /* zero length array*/
                    } else {
                      $$ = $2;
                    }
                  }
                | tLBRACE assoc_list '}'
                  {
                    $$ = NEW_HASH($2);
                  }
                | keyword_return
                  {
                    $$ = NEW_RETURN(0);
                  }
                | keyword_yield '(' call_args rparen
                  {
                    $$ = new_yield($3);
                  }
                | keyword_yield '(' rparen
                  {
                    $$ = NEW_YIELD(0, Qfalse);
                  }
                | keyword_yield
                  {
                    $$ = NEW_YIELD(0, Qfalse);
                  }
                | keyword_defined opt_nl '(' {in_defined = 1;} expr rparen
                  {
                    in_defined = 0;
                    $$ = NEW_DEFINED($5);
                  }
                | keyword_not '(' expr rparen
                  {
                    $$ = call_uni_op(cond($3), '!');
                  }
                | keyword_not '(' rparen
                  {
                    $$ = call_uni_op(cond(NEW_NIL()), '!');
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
                    block_dup_check($1->nd_args, $2);
                    $2->nd_iter = $1;
                    $$ = $2;
                    fixpos($$, $1);
                  }
                | tLAMBDA lambda
                  {
                    $$ = $2;
                  }
                | k_if expr_value then
                  compstmt
                  if_tail
                  k_end
                  {
                    $$ = NEW_IF(cond($2), $4, $5);
                    fixpos($$, $2);
                  }
                | k_unless expr_value then
                  compstmt
                  opt_else
                  k_end
                  {
                    $$ = NEW_UNLESS(cond($2), $4, $5);
                    fixpos($$, $2);
                  }
                | k_while {COND_PUSH(1);} expr_value do {COND_POP();}
                  compstmt
                  k_end
                  {
                    $$ = NEW_WHILE(cond($3), $6, 1);
                    fixpos($$, $3);
                  }
                | k_until {COND_PUSH(1);} expr_value do {COND_POP();}
                  compstmt
                  k_end
                  {
                    $$ = NEW_UNTIL(cond($3), $6, 1);
                    fixpos($$, $3);
                  }
                | k_case expr_value opt_terms
                  case_body
                  k_end
                  {
                    $$ = NEW_CASE($2, $4);
                    fixpos($$, $2);
                  }
                | k_case opt_terms case_body k_end
                  {
                    $$ = NEW_CASE(0, $3);
                  }
                | k_for for_var keyword_in
                  {COND_PUSH(1);}
                  expr_value do
                  {COND_POP();}
                  compstmt
                  k_end
                  {
                    /*
                     *  for a, b, c in e
                     *  #=>
                     *  e.each{|*x| a, b, c = x
                     *
                     *  for a in e
                     *  #=>
                     *  e.each{|x| a, = x}
                     */
                    $$ = NEW_FOR($2, $5, $8);
                    fixpos($$, $2);
                  }
                | k_class cpath superclass
                  {
                    if(in_def || in_single)
                      yy_error("class definition in method body");
                    class_nest++;
                    local_push(0);
                    $<num>$ = sourceline;
                  }
                  bodystmt
                  k_end
                  {
                    $$ = NEW_CLASS($2, $5, $3);
                    nd_set_line($$, $<num>4);
                    local_pop();
                    class_nest--;
                  }
                | k_class tLSHFT expr
                  {
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
                  k_end
                  {
                    $$ = NEW_SCLASS($3, $7);
                    fixpos($$, $3);
                    local_pop();
                    class_nest--;
                    in_def = $<num>4;
                    in_single = $<num>6;
                  }
                | k_module cpath
                  {
                    if(in_def || in_single)
                      yy_error("module definition in method body");
                    class_nest++;
                    local_push(0);
                    $<num>$ = sourceline;
                  }
                  bodystmt
                  k_end
                  {
                    $$ = NEW_MODULE($2, $4);
                    nd_set_line($$, $<num>3);
                    local_pop();
                    class_nest--;
                  }
                | k_def fname
                  {
                    $<id>$ = cur_mid;
                    cur_mid = $2;
                    in_def++;
                    local_push(0);
                  }
                  f_arglist
                  bodystmt
                  k_end
                  {
                    NODE* body = remove_begin($5);
                    $$ = NEW_DEFN($2, $4, body, NOEX_PRIVATE);
                    nd_set_line($$, $<num>1);
                    local_pop();
                    in_def--;
                    cur_mid = $<id>3;
                  }
                | k_def singleton dot_or_colon {lex_state = EXPR_FNAME;} fname
                  {
                    in_single++;
                    lex_state = EXPR_ENDFN; /* force for args */
                    local_push(0);
                  }
                  f_arglist
                  bodystmt
                  k_end
                  {
                    NODE* body = remove_begin($8);
                    $$ = NEW_DEFS($2, $5, $7, body);
                    nd_set_line($$, $<num>1);
                    local_pop();
                    in_single--;
                  }
                | keyword_break
                  {
                    $$ = NEW_BREAK(0);
                  }
                | keyword_next
                  {
                    $$ = NEW_NEXT(0);
                  }
                | keyword_redo
                  {
                    $$ = NEW_REDO();
                  }
                | keyword_retry
                  {
                    $$ = NEW_RETRY();
                  }
                ;

primary_value   : primary
                  {
                    value_expr($1);
                    $$ = $1;
                    if(!$$) $$ = NEW_NIL();
                  }
                ;

k_begin         : keyword_begin
                  {
                    token_info_push("begin");
                  }
                ;

k_if            : keyword_if
                  {
                    token_info_push("if");
                  }
                ;

k_unless        : keyword_unless
                  {
                    token_info_push("unless");
                  }
                ;

k_while         : keyword_while
                  {
                    token_info_push("while");
                  }
                ;

k_until         : keyword_until
                  {
                    token_info_push("until");
                  }
                ;

k_case          : keyword_case
                  {
                    token_info_push("case");
                  }
                ;

k_for           : keyword_for
                  {
                    token_info_push("for");
                  }
                ;

k_class         : keyword_class
                  {
                    token_info_push("class");
                  }
                ;

k_module        : keyword_module
                  {
                    token_info_push("module");
                  }
                ;

k_def           : keyword_def
                  {
                    token_info_push("def");
                    $<num>$ = sourceline;
                  }
                ;

k_end           : keyword_end
                  {
                    token_info_pop("end");
                  }
                ;

then            : term
                | keyword_then
                | term keyword_then
                ;

do              : term
                | keyword_do_cond
                ;

if_tail         : opt_else
                | keyword_elsif expr_value then
                  compstmt
                  if_tail
                  {
                    $$ = NEW_IF(cond($2), $4, $5);
                    fixpos($$, $2);
                  }
                ;

opt_else        : none
                | keyword_else compstmt
                  {
                    $$ = $2;
                  }
                ;

for_var         : lhs
                | mlhs
                ;

f_marg          : f_norm_arg
                  {
                    $$ = assignable($1, 0);
                  }
                | tLPAREN f_margs rparen
                  {
                    $$ = $2;
                  }
                ;

f_marg_list     : f_marg
                  {
                    $$ = NEW_LIST($1);
                  }
                | f_marg_list ',' f_marg
                  {
                    $$ = list_append($1, $3);
                  }
                ;

f_margs         : f_marg_list
                  {
                    $$ = NEW_MASGN($1, 0);
                  }
                | f_marg_list ',' tSTAR f_norm_arg
                  {
                    $$ = assignable($4, 0);
                    $$ = NEW_MASGN($1, $$);
                  }
                | f_marg_list ',' tSTAR f_norm_arg ',' f_marg_list
                  {
                    $$ = assignable($4, 0);
                    $$ = NEW_MASGN($1, NEW_POSTARG($$, $6));
                  }
                | f_marg_list ',' tSTAR
                  {
                    $$ = NEW_MASGN($1, -1);
                  }
                | f_marg_list ',' tSTAR ',' f_marg_list
                  {
                    $$ = NEW_MASGN($1, NEW_POSTARG(-1, $5));
                  }
                | tSTAR f_norm_arg
                  {
                    $$ = assignable($2, 0);
                    $$ = NEW_MASGN(0, $$);
                  }
                | tSTAR f_norm_arg ',' f_marg_list
                  {
                    $$ = assignable($2, 0);
                    $$ = NEW_MASGN(0, NEW_POSTARG($$, $4));
                  }
                | tSTAR
                  {
                    $$ = NEW_MASGN(0, -1);
                  }
                | tSTAR ',' f_marg_list
                  {
                    $$ = NEW_MASGN(0, NEW_POSTARG(-1, $3));
                  }
                ;

block_param     : f_arg ',' f_block_optarg ',' f_rest_arg opt_f_block_arg
                  {
                    $$ = new_args($1, $3, $5, 0, $6);
                  }
                | f_arg ',' f_block_optarg ',' f_rest_arg ',' f_arg opt_f_block_arg
                  {
                    $$ = new_args($1, $3, $5, $7, $8);
                  }
                | f_arg ',' f_block_optarg opt_f_block_arg
                  {
                    $$ = new_args($1, $3, 0, 0, $4);
                  }
                | f_arg ',' f_block_optarg ',' f_arg opt_f_block_arg
                  {
                    $$ = new_args($1, $3, 0, $5, $6);
                  }
                | f_arg ',' f_rest_arg opt_f_block_arg
                  {
                    $$ = new_args($1, 0, $3, 0, $4);
                  }
                | f_arg ','
                  {
                    $$ = new_args($1, 0, 1, 0, 0);
                  }
                | f_arg ',' f_rest_arg ',' f_arg opt_f_block_arg
                  {
                    $$ = new_args($1, 0, $3, $5, $6);
                  }
                | f_arg opt_f_block_arg
                  {
                    $$ = new_args($1, 0, 0, 0, $2);
                  }
                | f_block_optarg ',' f_rest_arg opt_f_block_arg
                  {
                    $$ = new_args(0, $1, $3, 0, $4);
                  }
                | f_block_optarg ',' f_rest_arg ',' f_arg opt_f_block_arg
                  {
                    $$ = new_args(0, $1, $3, $5, $6);
                  }
                | f_block_optarg opt_f_block_arg
                  {
                    $$ = new_args(0, $1, 0, 0, $2);
                  }
                | f_block_optarg ',' f_arg opt_f_block_arg
                  {
                    $$ = new_args(0, $1, 0, $3, $4);
                  }
                | f_rest_arg opt_f_block_arg
                  {
                    $$ = new_args(0, 0, $1, 0, $2);
                  }
                | f_rest_arg ',' f_arg opt_f_block_arg
                  {
                    $$ = new_args(0, 0, $1, $3, $4);
                  }
                | f_block_arg
                  {
                    $$ = new_args(0, 0, 0, 0, $1);
                  }
                ;

opt_block_param : none
                | block_param_def
                  {
                    command_start = TRUE;
                  }
                ;

block_param_def : '|' opt_bv_decl '|'
                  {
                    $$ = $2 ? NEW_ARGS_AUX(0,$2) : 0;
                  }
                | tOROP
                  {
                    $$ = 0;
                  }
                | '|' block_param opt_bv_decl '|'
                  {
                    if($3) {
                      $$ = NEW_ARGS_AUX($2, $3);
                    } else {
                      $$ = $2;
                    }
                  }
                ;

opt_bv_decl     : none
                  {
                    $$ = 0;
                  }
                | ';' bv_decls
                  {
                    $$ = $2;
                  }
                ;

bv_decls        : bvar
                  {
                    $$ = NEW_LIST($1);
                  }
                | bv_decls ',' bvar
                  {
                    $$ = list_append($1, $3);
                  }
                ;

bvar            : tIDENTIFIER
                  {
                    new_bv(get_id($1));
                    $$ = NEW_LIT(ID2SYM(get_id($1)));
                  }
                | f_bad_arg
                  {
                    $$ = 0;
                  }
                ;

lambda          : {
                    $<vars>$ = bv_push();
                  }
                  {
                    $<num>$ = lpar_beg;
                    lpar_beg = ++paren_nest;
                  }
                  f_larglist
                  lambda_body
                  {
                    lpar_beg = $<num>2;
                    $$ = $3;
                    $$->nd_body = NEW_SCOPE($3->nd_head, $4);
                    bv_pop($<vars>1);
                  }
                ;

f_larglist      : '(' f_args opt_bv_decl rparen
                  {
                    $$ = NEW_LAMBDA($2);
                  }
                | f_args
                  {
                    $$ = NEW_LAMBDA($1);
                  }
                ;

lambda_body     : tLAMBEG compstmt '}'
                  {
                    $$ = $2;
                  }
                | keyword_do_LAMBDA compstmt keyword_end
                  {
                    $$ = $2;
                  }
                ;

do_block        : keyword_do_block
                  {
                    $<vars>1 = bv_push();
                    $<num>$ = sourceline;
                  }
                  opt_block_param
                  compstmt
                  keyword_end
                  {
                    $$ = NEW_ITER($3, $4);
                    nd_set_line($$, $<num>2);
                    bv_pop($<vars>1);
                  }
                ;

block_call      : command do_block
                  {
                    if(nd_type($1) == NODE_YIELD) {
                      compile_error("block given to yield");
                    } else {
                      block_dup_check($1->nd_args, $2);
                    }
                    $2->nd_iter = $1;
                    $$ = $2;
                    fixpos($$, $1);
                  }
                | block_call '.' operation2 opt_paren_args
                  {
                    $$ = NEW_CALL($1, $3, $4);
                  }
                | block_call tCOLON2 operation2 opt_paren_args
                  {
                    $$ = NEW_CALL($1, $3, $4);
                  }
                ;

method_call     : operation paren_args
                  {
                    $$ = NEW_FCALL($1, $2);
                    fixpos($$, $2);
                  }
                | primary_value '.' operation2 opt_paren_args
                  {
                    $$ = NEW_CALL($1, $3, $4);
                    fixpos($$, $1);
                  }
                | primary_value tCOLON2 operation2 paren_args
                  {
                    $$ = NEW_CALL($1, $3, $4);
                    fixpos($$, $1);
                  }
                | primary_value tCOLON2 operation3
                  {
                    $$ = NEW_CALL($1, $3, 0);
                  }
                | primary_value '.' paren_args
                  {
                    $$ = NEW_CALL($1, parser_intern("call"), $3);
                    fixpos($$, $1);
                  }
                | primary_value tCOLON2 paren_args
                  {
                    $$ = NEW_CALL($1, parser_intern("call"), $3);
                    fixpos($$, $1);
                  }
                | keyword_super paren_args
                  {
                    $$ = NEW_SUPER($2);
                  }
                | keyword_super
                  {
                    $$ = NEW_ZSUPER();
                  }
                | primary_value '[' opt_call_args rbracket
                  {
                    if($1 && nd_type($1) == NODE_SELF) {
                      $$ = NEW_FCALL(tAREF, $3);
                    } else {
                      $$ = NEW_CALL($1, tAREF, $3);
                    }
                    fixpos($$, $1);
                  }
                ;

brace_block     : '{'
                  {
                    $<vars>1 = bv_push();
                    $<num>$ = sourceline;
                  }
                  opt_block_param
                  compstmt '}'
                  {
                    $$ = NEW_ITER($3, $4);
                    nd_set_line($$, $<num>2);
                    bv_pop($<vars>1);
                  }
                | keyword_do
                  {
                    $<vars>1 = bv_push();
                    $<num>$ = sourceline;
                  }
                  opt_block_param
                  compstmt keyword_end
                  {
                    $$ = NEW_ITER($3, $4);
                    nd_set_line($$, $<num>2);
                    bv_pop($<vars>1);
                  }
                ;

case_body       : keyword_when args then
                  compstmt
                  cases
                  {
                    $$ = NEW_WHEN($2, $4, $5);
                  }
                ;

cases           : opt_else
                | case_body
                ;

opt_rescue      : keyword_rescue exc_list exc_var then
                  compstmt
                  opt_rescue
                  {
                    if($3) {
                      /* TODO NEW_ERRINFO() */
                      $3 = node_assign($3, NEW_GVAR(parser_intern("$!")));
                      $5 = block_append($3, $5);
                    }
                    $$ = NEW_RESBODY($2, $5, $6);
                    fixpos($$, $2 ? $2 : $5);
                  }
                | none
                ;

exc_list        : arg_value
                  {
                    $$ = NEW_LIST($1);
                  }
                | mrhs
                  {
                    if(!($$ = splat_array($1))) $$ = $1;
                  }
                | none
                ;

exc_var         : tASSOC lhs
                  {
                    $$ = $2;
                  }
                | none
                ;

opt_ensure      : keyword_ensure compstmt
                  {
                    $$ = $2;
                  }
                | none
                ;

literal         : numeric
                | symbol
                  {
                    $$ = NEW_LIT(ID2SYM($1));
                  }
                | dsym
                ;

strings         : string
                  {
                    NODE *node = $1;
                    if(!node) {
                      node = NEW_STR(STR_NEW0());
                    } else {
                      node = evstr2dstr(node);
                    }
                    $$ = node;
                  }
                ;

string          : tCHAR
                | string1
                | string string1
                  {
                    $$ = literal_concat($1, $2);
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
                    if(!node) {
                      node = NEW_XSTR(STR_NEW0());
                    } else {
                      switch(nd_type(node)) {
                      case NODE_STR:
                        nd_set_type(node, NODE_XSTR);
                        break;
                      case NODE_DSTR:
                        nd_set_type(node, NODE_DXSTR);
                        break;
                      default:
                        node = NEW_NODE(NODE_DXSTR, REF(STR_NEW0()), 1, NEW_LIST(node));
                        break;
                      }
                    }
                    $$ = node;
                  }
                ;

regexp          : tREGEXP_BEG regexp_contents tREGEXP_END
                  {
                    intptr_t options = $3;
                    NODE *node = $2;
                    NODE *list, *prev;

                    if(!node) {
                      node = NEW_REGEX(STR_NEW0(), options & ~RE_OPTION_ONCE);
                    } else {
                      switch(nd_type(node)) {
                      case NODE_STR:
                        {
                          nd_set_type(node, NODE_REGEX);
                          node->nd_cnt = options & ~RE_OPTION_ONCE;
                        }
                        break;
                      default:
                        node = NEW_NODE(NODE_DSTR, REF(STR_NEW0()), 1, NEW_LIST(node));
                      case NODE_DSTR:
                        if(options & RE_OPTION_ONCE) {
                          nd_set_type(node, NODE_DREGX_ONCE);
                        } else {
                          nd_set_type(node, NODE_DREGX);
                        }
                        node->nd_cflag = options & ~RE_OPTION_ONCE;
                        for(list = (prev = node)->nd_next; list; list = list->nd_next) {
                          if(nd_type(list->nd_head) == NODE_STR) {
                            VALUE tail = list->nd_head->nd_lit;
                            if(prev && !NIL_P(prev->nd_lit)) {
                              VALUE lit;
                              if(prev == node) {
                                lit = prev->nd_lit;
                              } else {
                                lit = prev->nd_head->nd_lit;
                              }
                              if(!literal_concat0(lit, tail)) {
                                node = 0;
                                break;
                              }
                              rb_str_resize(tail, 0);
                              prev->nd_next = list->nd_next;
                              list = prev;
                            } else {
                              prev = list;
                            }
                          } else {
                            prev = 0;
                          }
                        }
                        if(!node->nd_next) {
                          nd_set_type(node, NODE_REGEX);
                          node->nd_cnt = options & ~RE_OPTION_ONCE;
                        }
                        break;
                      }
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
                    $$ = list_append($1, evstr2dstr($2));
                  }
                ;

word            : string_content
                | word string_content
                  {
                    $$ = literal_concat($1, $2);
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
                    $$ = list_append($1, $2);
                  }
                ;

string_contents : /* none */
                  {
                    $$ = 0;
                  }
                | string_contents string_content
                  {
                    $$ = literal_concat($1, $2);
                  }
                ;

xstring_contents: /* none */
                  {
                    $$ = 0;
                  }
                | xstring_contents string_content
                  {
                    $$ = literal_concat($1, $2);
                  }
                ;

regexp_contents : /* none */
                  {
                    $$ = 0;
                  }
                | regexp_contents string_content
                  {
                    NODE *head = $1, *tail = $2;
                    if(!head) {
                      $$ = tail;
                    } else if(!tail) {
                      $$ = head;
                    } else {
                      switch(nd_type(head)) {
                      case NODE_STR:
                        nd_set_type(head, NODE_DSTR);
                        break;
                      case NODE_DSTR:
                        break;
                      default:
                        head = list_append(NEW_DSTR(STR_NEW0()), head);
                        break;
                      }
                      $$ = list_append(head, tail);
                    }
                  }
                ;

string_content  : tSTRING_CONTENT
                | tSTRING_DVAR
                  {
                    $<node>$ = lex_strterm;
                    lex_strterm = 0;
                    lex_state = EXPR_BEG;
                  }
                  string_dvar
                  {
                    lex_strterm = $<node>2;
                    $$ = NEW_EVSTR($3);
                  }
                | tSTRING_DBEG
                  {
                    $<val>1 = cond_stack;
                    $<val>$ = cmdarg_stack;
                    cond_stack = 0;
                    cmdarg_stack = 0;
                  }
                  {
                    $<node>$ = lex_strterm;
                    lex_strterm = 0;
                    lex_state = EXPR_BEG;
                  }
                  compstmt '}'
                  {
                    cond_stack = $<val>1;
                    cmdarg_stack = $<val>2;
                    lex_strterm = $<node>3;

                    if($4) $4->flags &= ~NODE_FL_NEWLINE;
                    $$ = new_evstr($4);
                  }
                ;

string_dvar     : tGVAR {$$ = NEW_GVAR($1);}
                | tIVAR {$$ = NEW_IVAR($1);}
                | tCVAR {$$ = NEW_CVAR($1);}
                | backref
                ;

symbol          : tSYMBEG sym
                  {
                    lex_state = EXPR_END;
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
                    lex_state = EXPR_END;
                    if(!($$ = $2)) {
                      $$ = NEW_LIT(ID2SYM(parser_intern("")));
                    } else {
                      VALUE lit;

                      switch(nd_type($$)) {
                      case NODE_DSTR:
                        nd_set_type($$, NODE_DSYM);
                        break;
                      case NODE_STR:
                        lit = $$->nd_lit;
                        $$->nd_lit = ID2SYM(parser_intern_str(lit));
                        nd_set_type($$, NODE_LIT);
                        break;
                      default:
                        $$ = NEW_NODE(NODE_DSYM, REF(STR_NEW0()), 1, NEW_LIST($$));
                        break;
                      }
                    }
                  }
                ;

numeric         : tINTEGER
                | tFLOAT
                | tUMINUS_NUM tINTEGER         %prec tLOWEST
                  {
                    $$ = negate_lit($2);
                  }
                | tUMINUS_NUM tFLOAT           %prec tLOWEST
                  {
                    $$ = negate_lit($2);
                  }
                ;

variable        : tIDENTIFIER
                | tIVAR
                | tGVAR
                | tCONSTANT
                | tCVAR
                | keyword_nil {$$ = keyword_nil;}
                | keyword_self {$$ = keyword_self;}
                | keyword_true {$$ = keyword_true;}
                | keyword_false {$$ = keyword_false;}
                | keyword__FILE__ {$$ = keyword__FILE__;}
                | keyword__LINE__ {$$ = keyword__LINE__;}
                | keyword__ENCODING__ {$$ = keyword__ENCODING__;}
                ;

var_ref         : variable
                  {
                    if(!($$ = gettable($1))) {
                      $$ = NEW_BEGIN(0);
                    }
                  }
                ;

var_lhs         : variable
                  {
                    $$ = assignable($1, 0);
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
                    lex_state = EXPR_BEG;
                  }
                  expr_value term
                  {
                    $$ = $3;
                  }
                | error term
                  {
                    yyerrok;
                    $$ = 0;
                  }
                ;

f_arglist       : '(' f_args rparen
                  {
                    $$ = $2;
                    lex_state = EXPR_BEG;
                    command_start = TRUE;
                  }
                | f_args term
                  {
                    $$ = $1;
                    lex_state = EXPR_BEG;
                    command_start = TRUE;
                  }
                ;

f_args          : f_arg ',' f_optarg ',' f_rest_arg opt_f_block_arg
                  {
                    $$ = new_args($1, $3, $5, 0, $6);
                  }
                | f_arg ',' f_optarg ',' f_rest_arg ',' f_arg opt_f_block_arg
                  {
                    $$ = new_args($1, $3, $5, $7, $8);
                  }
                | f_arg ',' f_optarg opt_f_block_arg
                  {
                    $$ = new_args($1, $3, 0, 0, $4);
                  }
                | f_arg ',' f_optarg ',' f_arg opt_f_block_arg
                  {
                    $$ = new_args($1, $3, 0, $5, $6);
                  }
                | f_arg ',' f_rest_arg opt_f_block_arg
                  {
                    $$ = new_args($1, 0, $3, 0, $4);
                  }
                | f_arg ',' f_rest_arg ',' f_arg opt_f_block_arg
                  {
                    $$ = new_args($1, 0, $3, $5, $6);
                  }
                | f_arg opt_f_block_arg
                  {
                    $$ = new_args($1, 0, 0, 0, $2);
                  }
                | f_optarg ',' f_rest_arg opt_f_block_arg
                  {
                    $$ = new_args(0, $1, $3, 0, $4);
                  }
                | f_optarg ',' f_rest_arg ',' f_arg opt_f_block_arg
                  {
                    $$ = new_args(0, $1, $3, $5, $6);
                  }
                | f_optarg opt_f_block_arg
                  {
                    $$ = new_args(0, $1, 0, 0, $2);
                  }
                | f_optarg ',' f_arg opt_f_block_arg
                  {
                    $$ = new_args(0, $1, 0, $3, $4);
                  }
                | f_rest_arg opt_f_block_arg
                  {
                    $$ = new_args(0, 0, $1, 0, $2);
                  }
                | f_rest_arg ',' f_arg opt_f_block_arg
                  {
                    $$ = new_args(0, 0, $1, $3, $4);
                  }
                | f_block_arg
                  {
                    $$ = new_args(0, 0, 0, 0, $1);
                  }
                | /* none */
                  {
                    $$ = new_args(0, 0, 0, 0, 0);
                  }
                ;

f_bad_arg       : tCONSTANT
                  {
                    yy_error("formal argument cannot be a constant");
                    $$ = 0;
                  }
                | tIVAR
                  {
                    yy_error("formal argument cannot be an instance variable");
                    $$ = 0;
                  }
                | tGVAR
                  {
                    yy_error("formal argument cannot be a global variable");
                    $$ = 0;
                  }
                | tCVAR
                  {
                    yy_error("formal argument cannot be a class variable");
                    $$ = 0;
                  }
                ;

f_norm_arg      : f_bad_arg
                | tIDENTIFIER
                  {
                    formal_argument(get_id($1));
                    $$ = $1;
                  }
                ;

f_arg_item      : f_norm_arg
                  {
                    arg_var(get_id($1));
                    $$ = NEW_ARGS_AUX($1, 1);
                  }
                | tLPAREN f_margs rparen
                  {
                    ID tid = internal_id();
                    arg_var(tid);
                    $2->nd_value = NEW_LVAR(tid);
                    $$ = NEW_ARGS_AUX(tid, 1);
                    $$->nd_next = $2;
                  }
                ;

f_arg           : f_arg_item
                | f_arg ',' f_arg_item
                  {
                    $$ = $1;
                    $$->nd_plen++;
                    $$->nd_next = block_append($$->nd_next, $3->nd_next);
                  }
                ;

f_opt           : tIDENTIFIER '=' arg_value
                  {
                    arg_var(formal_argument(get_id($1)));
                    $$ = assignable($1, $3);
                    $$ = NEW_OPT_ARG(0, $$);
                  }
                ;

f_block_opt     : tIDENTIFIER '=' primary_value
                  {
                    arg_var(formal_argument(get_id($1)));
                    $$ = assignable($1, $3);
                    $$ = NEW_OPT_ARG(0, $$);
                  }
                ;

f_block_optarg  : f_block_opt
                  {
                    $$ = $1;
                  }
                | f_block_optarg ',' f_block_opt
                  {
                    NODE *opts = $1;
                    while(opts->nd_next) {
                      opts = opts->nd_next;
                    }
                    opts->nd_next = $3;
                    $$ = $1;
                  }
                ;

f_optarg        : f_opt
                  {
                    $$ = $1;
                  }
                | f_optarg ',' f_opt
                  {
                    NODE *opts = $1;
                    while(opts->nd_next) {
                      opts = opts->nd_next;
                    }
                    opts->nd_next = $3;
                    $$ = $1;
                  }
                ;

restarg_mark    : '*'
                | tSTAR
                ;

f_rest_arg      : restarg_mark tIDENTIFIER
                  {
                    if(!is_local_id($2)) {
                      yy_error("rest argument must be local variable");
                    }
                    arg_var(shadowing_lvar(get_id($2)));
                    $$ = $2;
                  }
                | restarg_mark
                  {
                    $$ = internal_id();
                    arg_var($$);
                  }
                ;

blkarg_mark     : '&'
                | tAMPER
                ;

f_block_arg     : blkarg_mark tIDENTIFIER
                  {
                    if(!is_local_id($2))
                      yy_error("block argument must be local variable");
                    else if(!in_block() && local_id($2))
                      yy_error("duplicate block argument name");
                    arg_var(shadowing_lvar(get_id($2)));
                    $$ = $2;
                  }
                ;

opt_f_block_arg : ',' f_block_arg
                  {
                    $$ = $2;
                  }
                | none
                  {
                    $$ = 0;
                  }
                ;

singleton       : var_ref
                  {
                    value_expr($1);
                    $$ = $1;
                    if(!$$) $$ = NEW_NIL();
                  }
                | '(' {lex_state = EXPR_BEG;} expr rparen
                  {
                    if($3 == 0) {
                      yy_error("can't define singleton method for ().");
                    } else {
                      switch(nd_type($3)) {
                      case NODE_STR:
                      case NODE_DSTR:
                      case NODE_XSTR:
                      case NODE_DXSTR:
                      case NODE_DREGX:
                      case NODE_LIT:
                      case NODE_ARRAY:
                      case NODE_ZARRAY:
                        yy_error("can't define singleton method for literals");
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
                ;

assocs          : assoc
                | assocs ',' assoc
                  {
                    $$ = list_concat($1, $3);
                  }
                ;

assoc           : arg_value tASSOC arg_value
                  {
                    $$ = list_append(NEW_LIST($1), $3);
                  }
                | tLABEL arg_value
                  {
                    $$ = list_append(NEW_LIST(NEW_LIT(ID2SYM($1))), $2);
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

rparen          : opt_nl ')'
                ;

rbracket        : opt_nl ']'
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

#undef parser
#undef yylex
#undef yylval
#define yylval  (*((YYSTYPE*)(lval)))

static int parser_regx_options(rb_parser_state*);
static int parser_tokadd_string(rb_parser_state*, int, int, int, long*, rb_encoding**);
static void parser_tokaddmbc(rb_parser_state*, int c, rb_encoding *);
static int parser_parse_string(rb_parser_state*, NODE*);
static int parser_here_document(rb_parser_state*, NODE*);


#define nextc()                   parser_nextc(parser_state)
#define pushback(c)               parser_pushback(parser_state, c)
#define newtok()                  parser_newtok(parser_state)
#define tokspace(n)               parser_tokspace(parser_state, n)
#define tokadd(c)                 parser_tokadd(parser_state, c)
#define tok_hex(numlen)           parser_tok_hex(parser_state, numlen)
#define read_escape(flags,e)      parser_read_escape(parser_state, flags, e)
#define tokadd_escape(e)          parser_tokadd_escape(parser_state, e)
#define regx_options()            parser_regx_options(parser_state)
#define tokadd_string(f,t,p,n,e)  parser_tokadd_string(parser_state,f,t,p,n,e)
#define parse_string(n)           parser_parse_string(parser_state,n)
#define tokaddmbc(c, enc)         parser_tokaddmbc(parser_state, c, enc)
#define here_document(n)          parser_here_document(parser_state,n)
#define heredoc_identifier()      parser_heredoc_identifier(parser_state)
#define heredoc_restore(n)        parser_heredoc_restore(parser_state,n)
#define whole_match_p(e,l,i)      parser_whole_match_p(parser_state,e,l,i)

#define set_yylval_str(x)         yylval.node = NEW_STR(x)
#define set_yylval_num(x)         yylval.num = x
#define set_yylval_id(x)          yylval.id = x
#define set_yylval_name(x)        yylval.id = x
#define set_yylval_literal(x)     yylval.node = NEW_LIT(x)
#define set_yylval_number(x)      yylval.node = NEW_NUMBER(x)
#define set_yylval_float(x)       yylval.node = NEW_FLOAT(x)
#define set_yylval_node(x)        yylval.node = x
#define yylval_id()               yylval.id

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

#define parser_encoding_name()    (parser_state->enc->name)
#define parser_mbclen()           parser_enc_mbclen((lex_p-1),lex_pend,parser->enc)
#define parser_precise_mbclen()   parser_enc_precise_mbclen((lex_p-1),lex_pend,parser_state->enc)
#define is_identchar(p,e,enc)     (parser_enc_isalnum(*p,enc) || (*p) == '_' || !ISASCII(*p))
#define parser_is_identchar()     (!eofp && \
                                   is_identchar((lex_p-1),lex_pend,parser_state->enc))

#define parser_isascii() ISASCII(*(lex_p-1))

static void parser_token_info_push(rb_parser_state* parser_state, const char *token) {
  /* TODO */
}

static void parser_token_info_pop(rb_parser_state* parser_state, const char *token) {
  /* TODO */
}

static int
parser_yyerror(rb_parser_state* parser_state, const char *msg)
{
  create_error(parser_state, (char *)msg);

  return 1;
}

static int
yycompile(rb_parser_state* parser_state, char *f, int line)
{
  int n;
  /* Setup an initial empty scope. */
  heredoc_end = 0;
  lex_strterm = 0;
  ruby__end__seen = 0;
  sourcefile = f;
  command_start = TRUE;
  parser_prepare(parser_state);
  n = yyparse(parser_state);
  ruby_debug_lines = 0;
  compile_for_eval = 0;
  cond_stack = 0;
  cmdarg_stack = 0;
  command_start = TRUE;
  class_nest = 0;
  in_single = 0;
  in_def = 0;
  cur_mid = 0;

  return n;
}

static rb_encoding*
must_be_ascii_compatible(VALUE s)
{
  rb_encoding *enc = parser_enc_get(s);
  if(!parser_enc_asciicompat(enc)) {
    // TODO: handle this in a way that doesn't leak parser state
    // rb_raise(rb_eArgError, "invalid source encoding");
  }
  return enc;
}

static VALUE
lex_get_str(rb_parser_state* parser_state, VALUE s)
{
  const char *beg, *end, *pend;
  rb_encoding* enc = must_be_ascii_compatible(s);

  beg = RSTRING_PTR(s);
  if(lex_gets_ptr) {
    if(RSTRING_LEN(s) == lex_gets_ptr) return Qnil;
    beg += lex_gets_ptr;
  }
  pend = RSTRING_PTR(s) + RSTRING_LEN(s);
  end = beg;
  while(end < pend) {
    if(*end++ == '\n') break;
  }
  lex_gets_ptr = end - RSTRING_PTR(s);
  return REF(parser_enc_str_new(beg, end - beg, enc));
}

static VALUE
lex_getline(rb_parser_state* parser_state)
{
  VALUE line = (*lex_gets)(parser_state, lex_input);
  if(NIL_P(line)) return line;
  must_be_ascii_compatible(line);

  return line;
}

VALUE process_parse_tree(rb_parser_state*, VALUE, NODE*, ID*);

VALUE
string_to_ast(VALUE ptp, VALUE name, VALUE source, VALUE line)
{
  int l = FIX2INT(line);
  VALUE ret;
  rb_parser_state* parser_state = parser_alloc_state();

  lex_input = source;
  lex_gets = lex_get_str;
  lex_gets_ptr = 0;
  processor = ptp;
  rb_funcall(ptp, rb_intern("references="), 1, references);
  sourceline = l - 1;
  compile_for_eval = 1;

  yycompile(parser_state, RSTRING_PTR(name), l);

  if(!parse_error) {
    ret = process_parse_tree(parser_state, ptp, top_node, NULL);
  } else {
    ret = Qnil;
  }
  pt_free(parser_state);
  free(parser_state);
  return ret;
}

#define LEX_IO_BUFLEN  5120

static VALUE parse_io_gets(rb_parser_state* parser_state, VALUE s) {
  VALUE str = Qnil;
  char* p;
  ssize_t n;

  while(true) {
    if(lex_io_total == 0 || lex_io_index == lex_io_total) {
      lex_io_total = read(lex_io, lex_io_buf, LEX_IO_BUFLEN);

      if(lex_io_total == 0) {
        lex_io_total = 0;
        return str;
      } else if(lex_io_total < 0) {
        // TODO raise exception
        return str;
      } else {
        lex_io_index = 0;
      }
    }

    p = (char*)memchr(lex_io_buf + lex_io_index,
                      '\n', lex_io_total - lex_io_index);
    if(p) {
      n = p - lex_io_buf - lex_io_index + 1;
    } else {
      n = lex_io_total - lex_io_index;
    }

    if(str == Qnil) {
      str = REF(parser_enc_str_new(lex_io_buf + lex_io_index,
                                   n, parser_state->enc));
    } else {
      rb_str_cat(str, lex_io_buf + lex_io_index, n);
    }

    if(p) {
      lex_io_index += n;
      lex_io_count += n;
      return str;
    } else {
      lex_io_total = 0;
      lex_io_index = 0;
    }
  }

  return Qnil;
}

VALUE
file_to_ast(VALUE ptp, const char *f, int fd, int start)
{
  VALUE ret;
  rb_parser_state* parser_state = parser_alloc_state();

  lex_input = Qnil;
  lex_io = fd;
  lex_io_buf = (char*)malloc(LEX_IO_BUFLEN);
  lex_gets = parse_io_gets;
  processor = ptp;
  rb_funcall(ptp, rb_intern("references="), 1, references);
  sourceline = start - 1;

  yycompile(parser_state, (char*)f, start);

  if(!parse_error) {
    ret = process_parse_tree(parser_state, ptp, top_node, NULL);

    if(ruby__end__seen && lex_io) {
      rb_funcall(ptp, rb_sData, 1, ULONG2NUM(lex_io_count));
    }
  } else {
    ret = Qnil;
  }

  pt_free(parser_state);
  free(parser_state);
  return ret;
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

static VALUE
parser_str_new(rb_parser_state* parser_state, const char *p, long n,
               rb_encoding *enc, int func, rb_encoding *enc0)
{
  VALUE str;
  str = REF(parser_enc_str_new(p, n, enc));
  if(!(func & STR_FUNC_REGEXP) && parser_enc_asciicompat(enc)) {
    if(parser_enc_str_coderange(str) == ENC_CODERANGE_7BIT) {
      // Do nothing.
    } else if(enc0 == parser_usascii_encoding() && enc != parser_utf8_encoding()) {
      parser_enc_associate(str, parser_ascii8bit_encoding());
    }
  }

  return str;
}

#define lex_goto_eol(parser_state)  (lex_p = lex_pend)
#define lex_eol_p() (lex_p >= lex_pend)
#define peek(c) (lex_p < lex_pend && (c) == *lex_p)
#define peek_n(c,n) (lex_p+(n) < lex_pend && (c) == (unsigned char)lex_p[n])

static inline int
parser_nextc(rb_parser_state* parser_state)
{
  int c;

  if(lex_p == lex_pend) {
    VALUE v = lex_nextline;
    lex_nextline = 0;
    if(!v) {
      if(eofp)
        return -1;

      if(!lex_input || NIL_P(v = lex_getline(parser_state))) {
        eofp = true;
        lex_goto_eol(parser_state);
        return -1;
      }
    }

    if(heredoc_end > 0) {
      sourceline = heredoc_end;
      heredoc_end = 0;
    }

    sourceline++;
    line_count++;
    lex_pbeg = lex_p = RSTRING_PTR(v);
    lex_pend = lex_p + RSTRING_LEN(v);
    lex_lastline = v;
  }

  c = (unsigned char)*lex_p++;
  if(c == '\r' && peek('\n')) {
    lex_p++;
    c = '\n';
  }

  return c;
}

static void
parser_pushback(rb_parser_state* parser_state, int c)
{
  if(c == -1) return;
  lex_p--;
}

#define pushback(c)   parser_pushback(parser_state, c)

/* Indicates if we're currently at the beginning of a line. */
#define was_bol() (lex_p == lex_pbeg + 1)

/* The token buffer. It's just a global string that has
   functions to build up the string easily. */

#define tokfix() (tokenbuf[tokidx]='\0')
#define tok() tokenbuf
#define toklen() tokidx
#define toklast() (tokidx>0?tokenbuf[tokidx-1]:0)

static char*
parser_newtok(rb_parser_state* parser_state)
{
  tokidx = 0;
  if(!tokenbuf) {
    toksiz = 60;
    tokenbuf = ALLOC_N(char, 60);
  }
  if(toksiz > 4096) {
    toksiz = 60;
    REALLOC_N(tokenbuf, char, 60);
  }
  return tokenbuf;
}

static char *
parser_tokspace(rb_parser_state *parser_state, int n)
{
  tokidx += n;

  if(tokidx >= toksiz) {
    do {
      toksiz *= 2;
    } while(toksiz < tokidx);
    REALLOC_N(tokenbuf, char, toksiz);
  }
  return &tokenbuf[tokidx-n];
}


static void parser_tokadd(rb_parser_state* parser_state, char c)
{
  assert(tokidx < toksiz && tokidx >= 0);
  tokenbuf[tokidx++] = c;
  if(tokidx >= toksiz) {
    toksiz *= 2;
    REALLOC_N(tokenbuf, char, toksiz);
  }
}

static int
parser_tok_hex(rb_parser_state *parser_state, size_t *numlen)
{
  int c;

  c = scan_hex(lex_p, 2, numlen);
  if(!*numlen) {
    yy_error("invalid hex escape");
    return 0;
  }
  lex_p += *numlen;
  return c;
}

#define tokcopy(n) memcpy(tokspace(n), lex_p - (n), (n))

static int
parser_tokadd_utf8(rb_parser_state *parser_state, rb_encoding **encp,
                   int string_literal, int symbol_literal, int regexp_literal)
{
  /*
   * If string_literal is true, then we allow multiple codepoints
   * in \u{}, and add the codepoints to the current token.
   * Otherwise we're parsing a character literal and return a single
   * codepoint without adding it
   */

  int codepoint;
  size_t numlen;

  if(regexp_literal) {
    tokadd('\\'); tokadd('u');
  }

  if(peek('{')) {  /* handle \u{...} form */
    do {
      if(regexp_literal) tokadd(*lex_p);
      nextc();
      codepoint = scan_hex(lex_p, 6, &numlen);

      if(numlen == 0)  {
        yy_error("invalid Unicode escape");
        return 0;
      }
      if(codepoint > 0x10ffff) {
      yy_error("invalid Unicode codepoint (too large)");
      return 0;
      }

      lex_p += numlen;
      if(regexp_literal) {
        tokcopy((int)numlen);
      } else if(codepoint >= 0x80) {
        *encp = UTF8_ENC();
        if(string_literal) tokaddmbc(codepoint, *encp);
      } else if(string_literal) {
        tokadd(codepoint);
      }
    } while(string_literal && (peek(' ') || peek('\t')));

    if(!peek('}')) {
      yy_error("unterminated Unicode escape");
      return 0;
    }

    if(regexp_literal) tokadd('}');
    nextc();
  } else {			/* handle \uxxxx form */
    codepoint = scan_hex(lex_p, 4, &numlen);
    if(numlen < 4) {
      yy_error("invalid Unicode escape");
      return 0;
    }
    lex_p += 4;
    if(regexp_literal) {
      tokcopy(4);
    } else if(codepoint >= 0x80) {
      *encp = UTF8_ENC();
      if(string_literal) tokaddmbc(codepoint, *encp);
    } else if(string_literal) {
      tokadd(codepoint);
    }
  }

  return codepoint;
}

#define ESCAPE_CONTROL 1
#define ESCAPE_META    2

static int
parser_read_escape(rb_parser_state *parser_state, int flags, rb_encoding **encp)
{
  int c;
  size_t numlen;

  switch(c = nextc()) {
  case '\\':	    /* Backslash */
    return c;

  case 'n':	      /* newline */
    return '\n';

  case 't':	      /* horizontal tab */
    return '\t';

  case 'r':	      /* carriage-return */
    return '\r';

  case 'f':	      /* form-feed */
    return '\f';

  case 'v':	      /* vertical tab */
    return '\13';

  case 'a':	      /* alarm(bell) */
    return '\007';

  case 'e':	      /* escape */
    return 033;

  case '0': case '1': case '2': case '3': /* octal constant */
  case '4': case '5': case '6': case '7':
    if(flags & (ESCAPE_CONTROL|ESCAPE_META)) goto eof;
    pushback(c);
    c = scan_oct(lex_p, 3, &numlen);
    lex_p += numlen;
    return c;

  case 'x':	    /* hex constant */
    if(flags & (ESCAPE_CONTROL|ESCAPE_META)) goto eof;
    c = tok_hex(&numlen);
    if(numlen == 0) return 0;
    return c;

  case 'b':	    /* backspace */
    return '\010';

  case 's':	    /* space */
    return ' ';

  case 'M':
    if(flags & ESCAPE_META) goto eof;
    if((c = nextc()) != '-') {
      pushback(c);
      goto eof;
    }
    if((c = nextc()) == '\\') {
      if(peek('u')) goto eof;
      return read_escape(flags|ESCAPE_META, encp) | 0x80;
    } else if(c == -1 || !ISASCII(c)) {
      goto eof;
    } else {
      return ((c & 0xff) | 0x80);
    }

  case 'C':
    if((c = nextc()) != '-') {
      pushback(c);
      goto eof;
    }
  case 'c':
    if(flags & ESCAPE_CONTROL) goto eof;
    if((c = nextc())== '\\') {
      if(peek('u')) goto eof;
      c = read_escape(flags|ESCAPE_CONTROL, encp);
    } else if(c == '?') {
      return 0177;
    } else if(c == -1 || !ISASCII(c)) {
      goto eof;
    }
    return c & 0x9f;

  eof:
  case -1:
    yy_error("Invalid escape character syntax");
    return '\0';

  default:
    return c;
  }
}

static void
parser_tokaddmbc(rb_parser_state* parser_state, int c, rb_encoding *enc)
{
  int len = parser_enc_codelen(c, enc);
  parser_enc_mbcput(c, tokspace(len), enc);
}

static int
parser_tokadd_escape(rb_parser_state* parser_state, rb_encoding **encp)
{
  int c;
  int flags = 0;
  size_t numlen;

first:
  switch(c = nextc()) {
  case '\n':
    return 0;		/* just ignore */

  case '0': case '1': case '2': case '3': /* octal constant */
  case '4': case '5': case '6': case '7':
    if(flags & (ESCAPE_CONTROL|ESCAPE_META)) goto eof;
    {
      scan_oct(--lex_p, 3, &numlen);
      if(numlen == 0) goto eof;
      lex_p += numlen;
      tokcopy((int)numlen + 1);
    }
    return 0;

  case 'x':	/* hex constant */
    if(flags & (ESCAPE_CONTROL|ESCAPE_META)) goto eof;
    {
      tok_hex(&numlen);
      if(numlen == 0) goto eof;
      tokcopy((int)numlen + 2);
    }
    return 0;

  case 'M':
    if(flags & ESCAPE_META) goto eof;
    if((c = nextc()) != '-') {
      pushback(c);
      goto eof;
    }
    tokcopy(3);
    flags |= ESCAPE_META;
    goto escaped;

  case 'C':
    if(flags & ESCAPE_CONTROL) goto eof;
    if((c = nextc()) != '-') {
      pushback(c);
      goto eof;
    }
    tokcopy(3);
    goto escaped;

  case 'c':
    if(flags & ESCAPE_CONTROL) goto eof;
    tokcopy(2);
    flags |= ESCAPE_CONTROL;
escaped:
    if((c = nextc()) == '\\') {
      goto first;
    } else if(c == -1) goto eof;
    tokadd(c);
    return 0;

eof:
  case -1:
    yy_error("Invalid escape character syntax");
    return -1;

  default:
    tokadd('\\');
    tokadd(c);
  }

  return 0;
}

static int
parser_regx_options(rb_parser_state* parser_state)
{
    int kcode = 0;
    int options = 0;
    int c;

    /* TODO */
    newtok();
    while(c = nextc(), ISALPHA(c)) {
      switch(c) {
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
        kcode = RE_KCODE_NONE;
        break;
      case 'e':
        kcode = RE_KCODE_EUC;
        break;
      case 's':
        kcode = RE_KCODE_SJIS;
        break;
      case 'u':
        kcode = RE_KCODE_UTF8;
        break;
      default:
        tokadd((char)c);
        break;
      }
    }
    pushback(c);
    if(toklen()) {
      tokfix();
      rb_compile_error(parser_state, "unknown regexp option%s - %s",
                       toklen() > 1 ? "s" : "", tok());
    }
    return options | kcode;
}

static int
parser_tokadd_mbchar(rb_parser_state *parser_state, int c)
{
  int len = parser_precise_mbclen();
  if(!MBCLEN_CHARFOUND_P(len)) {
    rb_compile_error(parser_state, "invalid multibyte char (%s)", parser_encoding_name());
    return -1;
  }
  tokadd(c);
  lex_p += --len;
  if(len > 0) tokcopy(len);
  return c;
}

#define tokadd_mbchar(c) parser_tokadd_mbchar(parser_state, c)

static int
parser_tokadd_string(rb_parser_state *parser_state,
                     int func, int term, int paren, long *nest, rb_encoding **encp)
{
  int c;
  int has_nonascii = 0;
  rb_encoding *enc = *encp;
  char *errbuf = 0;
  static const char mixed_msg[] = "%s mixed within %s source";

#define mixed_error(enc1, enc2) if(!errbuf) {	\
    size_t len = sizeof(mixed_msg) - 4;	\
    len += strlen(parser_enc_name(enc1));	\
    len += strlen(parser_enc_name(enc2));	\
    errbuf = ALLOCA_N(char, len);		\
    snprintf(errbuf, len, mixed_msg, parser_enc_name(enc1), parser_enc_name(enc2));		\
    yy_error(errbuf);			\
  }

#define mixed_escape(beg, enc1, enc2) do {	\
    char *pos = lex_p;		\
    lex_p = beg;				\
    mixed_error(enc1, enc2);		\
    lex_p = pos;				\
  } while(0)

  while((c = nextc()) != -1) {
    if(paren && c == paren) {
      ++*nest;
    } else if(c == term) {
      if(!nest || !*nest) {
        pushback(c);
        break;
      }
      --*nest;
    } else if((func & STR_FUNC_EXPAND) && c == '#' && lex_p < lex_pend) {
      int c2 = *lex_p;
      if(c2 == '$' || c2 == '@' || c2 == '{') {
        pushback(c);
        break;
      }
    } else if(c == '\\') {
      char *beg = lex_p - 1;
      c = nextc();
      switch(c) {
      case '\n':
        if(func & STR_FUNC_QWORDS) break;
        if(func & STR_FUNC_EXPAND) continue;
        tokadd('\\');
        break;

      case '\\':
        if(func & STR_FUNC_ESCAPE) tokadd(c);
        break;

      case 'u':
        if((func & STR_FUNC_EXPAND) == 0) {
          tokadd('\\');
          break;
        }
        parser_tokadd_utf8(parser_state, &enc, 1, func & STR_FUNC_SYMBOL,
                           func & STR_FUNC_REGEXP);
        if(has_nonascii && enc != *encp) {
          mixed_escape(beg, enc, *encp);
        }
        continue;

      default:
        if(func & STR_FUNC_REGEXP) {
          pushback(c);
          if((c = tokadd_escape(&enc)) < 0)
          return -1;
          if(has_nonascii && enc != *encp) {
            mixed_escape(beg, enc, *encp);
          }
          continue;
        } else if(func & STR_FUNC_EXPAND) {
          pushback(c);
          if(func & STR_FUNC_ESCAPE) tokadd('\\');
          c = read_escape(0, &enc);
          if(!ISASCII(c)) {
            if(tokadd_mbchar(c) == -1) return -1;
            continue;
          }
        } else if((func & STR_FUNC_QWORDS) && ISSPACE(c)) {
          /* ignore backslashed spaces in %w */
        } else if(c != term && !(paren && c == paren)) {
          tokadd('\\');
          pushback(c);
          continue;
        }
      }
    } else if(!parser_isascii()) {
      has_nonascii = 1;
      if(enc != *encp) {
        mixed_error(enc, *encp);
        continue;
      }
      if(tokadd_mbchar(c) == -1) return -1;
      continue;
    } else if((func & STR_FUNC_QWORDS) && ISSPACE(c)) {
      pushback(c);
      break;
    }
    if(c & 0x80) {
      has_nonascii = 1;
      if(enc != *encp) {
        mixed_error(enc, *encp);
        continue;
      }
    }
    tokadd(c);
  }
  *encp = enc;
  return c;
}

#define NEW_STRTERM(func, term, paren) \
  node_newnode(NODE_STRTERM, (VALUE)(func), \
               (VALUE)((term) | ((paren) << (CHAR_BIT * 2))), 0)
#define pslval ((YYSTYPE *)lval)
static int
parser_parse_string(rb_parser_state* parser_state, NODE *quote)
{
  int func = (int)quote->nd_func;
  int term = nd_term(quote);
  int paren = nd_paren(quote);
  int c, space = 0;
  rb_encoding* enc = parser_state->enc;

  long start_line = sourceline;

  if(func == -1) return tSTRING_END;
  c = nextc();
  if((func & STR_FUNC_QWORDS) && ISSPACE(c)) {
    do {c = nextc();} while(ISSPACE(c));
    space = 1;
  }
  if(c == term && !quote->nd_nest) {
    if(func & STR_FUNC_QWORDS) {
      quote->nd_func = -1;
      return ' ';
    }
    if(!(func & STR_FUNC_REGEXP)) return tSTRING_END;
    set_yylval_num(regx_options());
    return tREGEXP_END;
  }
  if(space) {
    pushback(c);
    return ' ';
  }
  newtok();
  if((func & STR_FUNC_EXPAND) && c == '#') {
    switch(c = nextc()) {
    case '$':
    case '@':
      pushback(c);
      return tSTRING_DVAR;
    case '{':
      return tSTRING_DBEG;
    }
    tokadd('#');
  }
  pushback(c);
  if(tokadd_string(func, term, paren, &quote->nd_nest, &enc) == -1) {
    sourceline = nd_line(quote);
    if(func & STR_FUNC_REGEXP) {
      if(eofp)
        rb_compile_error(parser_state, "unterminated regexp meets end of file");
      return tREGEXP_END;
    } else {
      if(eofp)
        rb_compile_error(parser_state, "unterminated string meets end of file");
      return tSTRING_END;
    }
  }

  tokfix();
  set_yylval_str(STR_NEW3(tok(), toklen(), enc, func));
  nd_set_line(pslval->node, start_line);
  return tSTRING_CONTENT;
}

/* Called when the lexer detects a heredoc is beginning. This pulls
   in more characters and detects what kind of heredoc it is. */
static int
parser_heredoc_identifier(rb_parser_state* parser_state)
{
  int c = nextc(), term, func = 0;
  size_t len;

  if(c == '-') {
    c = nextc();
    func = STR_FUNC_INDENT;
  }
  switch(c) {
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

    newtok();
    tokadd(func);
    term = c;

    /* Where of where has the term gone.. */
    while((c = nextc()) != -1 && c != term) {
      if(tokadd_mbchar(c) == -1) return 0;
    }

    /* Ack! end of file or end of string. */
    if(c == -1) {
      rb_compile_error(parser_state, "unterminated here document identifier");
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

    if(!parser_is_identchar()) {
      pushback(c);
      if(func & STR_FUNC_INDENT) {
        pushback('-');
      }
      return 0;
    }

    /* Finally, setup the token buffer and begin to fill it. */
    newtok();
    term = '"';
    tokadd(func |= str_dquote);
    do {
      if(tokadd_mbchar(c) == -1) return 0;
    } while((c = nextc()) != -1 && parser_is_identchar());
    pushback(c);
    break;
  }


  /* Fixup the token buffer, ie set the last character to null. */
  tokfix();
  len = lex_p - lex_pbeg;
  lex_goto_eol(parser_state);

  /* Tell the lexer that we're inside a string now. nd_lit is
     the heredoc identifier that we watch the stream for to
     detect the end of the heredoc. */
  lex_strterm = node_newnode(NODE_HEREDOC,
                             REF(STR_NEW(tok(), toklen())), /* nd_lit */
                             (VALUE)len,                    /* nd_nth */
                             (VALUE)lex_lastline);          /* nd_orig */
  nd_set_line(lex_strterm, sourceline);
  return term == '`' ? tXSTRING_BEG : tSTRING_BEG;
}

static void
parser_heredoc_restore(rb_parser_state* parser_state, NODE *here)
{
  VALUE line;

  line = here->nd_orig;
  lex_lastline = line;
  lex_pbeg = RSTRING_PTR(line);
  lex_pend = lex_pbeg + RSTRING_LEN(line);
  lex_p = lex_pbeg + here->nd_nth;
  heredoc_end = sourceline;
  sourceline = nd_line(here);
}

static int
parser_whole_match_p(rb_parser_state* parser_state, const char *eos, ssize_t len, int indent)
{
  char *p = lex_pbeg;
  ssize_t n;

  if(indent) {
    while(*p && ISSPACE(*p)) p++;
  }
  n = lex_pend - (p + len);
  if(n < 0 || (n > 0 && p[len] != '\n' && p[len] != '\r')) return FALSE;
  if(strncmp(eos, p, len) == 0) return TRUE;
  return FALSE;
}

/* Called when the lexer knows it's inside a heredoc. This function
   is responsible for detecting an expandions (ie #{}) in the heredoc
   and emitting a lex token and also detecting the end of the heredoc. */

static int
parser_here_document(rb_parser_state* parser_state, NODE *here)
{
  int c, func, indent = 0;
  char *eos, *p, *pend;
  ssize_t len;
  VALUE str = 0;
  rb_encoding* enc = parser_state->enc;

  /* eos == the heredoc ident that we found when the heredoc started */
  eos = RSTRING_PTR(here->nd_lit);
  len = RSTRING_LEN(here->nd_lit) - 1;

  /* indicates if we should search for expansions. */
  indent = (func = *eos++) & STR_FUNC_INDENT;

  /* Ack! EOF or end of input string! */
  if((c = nextc()) == -1) {
  error:
    rb_compile_error(parser_state, "can't find string \"%s\" anywhere before EOF", eos);
  restore:
    heredoc_restore(lex_strterm);
    lex_strterm = 0;
    return 0;
  }
  /* Gr. not yet sure what was_bol() means other than it seems like
     it means only 1 character has been consumed. */

  if(was_bol() && whole_match_p(eos, len, indent)) {
    heredoc_restore(lex_strterm);
    return tSTRING_END;
  }

  /* If aren't doing expansions, we can just scan until
     we find the identifier. */

  if((func & STR_FUNC_EXPAND) == 0) {
    do {
      p = RSTRING_PTR(lex_lastline);
      pend = lex_pend;
      if(pend > p) {
        switch(pend[-1]) {
        case '\n':
          if(--pend == p || pend[-1] != '\r') {
            pend++;
            break;
          }
        case '\r':
          --pend;
        }
      }
      if(str) {
        rb_str_cat(str, p, pend - p);
      } else {
        str = REF(STR_NEW(p, pend - p));
      }
      if(pend < lex_pend) rb_str_cat(str, "\n", 1);
      lex_goto_eol(parser_state);
      if(nextc() == -1) {
        goto error;
      }
    } while(!whole_match_p(eos, len, indent));
  } else {
    newtok();
    if(c == '#') {
      switch(c = nextc()) {
      case '$':
      case '@':
        pushback(c);
        return tSTRING_DVAR;
      case '{':
        return tSTRING_DBEG;
      }
      tokadd('#');
    }

    /* Loop while we haven't found a the heredoc ident. */
    do {
      pushback(c);
      /* Scan up until a \n and fill in the token buffer. */
      if((c = tokadd_string(func, '\n', 0, NULL, &enc)) == -1) {
        if(eofp) goto error;
        goto restore;
      }

      /* We finished scanning, but didn't find a \n, so we setup the node
         and have the lexer file in more. */
      if(c != '\n') {
        set_yylval_str(STR_NEW3(tok(), toklen(), enc, func));
        return tSTRING_CONTENT;
      }

      /* I think this consumes the \n */
      tokadd(nextc());
      if((c = nextc()) == -1) goto error;
    } while(!whole_match_p(eos, len, indent));
    str = REF(STR_NEW3(tok(), toklen(), enc, func));
  }
  heredoc_restore(lex_strterm);
  lex_strterm = NEW_STRTERM(-1, 0, 0);
  set_yylval_str(str);
  return tSTRING_CONTENT;
}

#include "lex.c.blt"

static int
arg_ambiguous()
{
  rb_warning("ambiguous first argument; put parentheses or even spaces");

  return 1;
}

#define IS_ARG() (lex_state == EXPR_ARG || lex_state == EXPR_CMDARG)

static ID
parser_formal_argument(rb_parser_state* parser_state, ID lhs)
{
  if(!is_local_id(lhs)) {
    yy_error("formal argument must be local variable");
  }
  shadowing_lvar(lhs);
  return lhs;
}

static bool
parser_lvar_defined(rb_parser_state* parser_state, ID id) {
  return local_id(id);
}

static long
parser_encode_length(rb_parser_state* parser_state, const char *name, long len)
{
  long nlen;

  if(len > 5 && name[nlen = len - 5] == '-') {
    if(strncasecmp(name + nlen + 1, "unix", 4) == 0)
      return nlen;
  }
  if(len > 4 && name[nlen = len - 4] == '-') {
    if(strncasecmp(name + nlen + 1, "dos", 3) == 0)
      return nlen;
    if(strncasecmp(name + nlen + 1, "mac", 3) == 0 &&
        !(len == 8 && strncasecmp(name, "utf8-mac", len) == 0))
      /* exclude UTF8-MAC because the encoding named "UTF8" doesn't exist in Ruby */
      return nlen;
  }
  return len;
}

static void
parser_set_encode(rb_parser_state* parser_state, const char *name)
{
  int idx = parser_enc_find_index(name);
  rb_encoding *enc;

  if(idx < 0) {
    // TODO: handle this in a way that doesn't leak parser state
    // rb_raise(rb_eArgError, "unknown encoding name: %s", name);
    return;
  }

  enc = parser_enc_from_index(idx);
  if(!parser_enc_asciicompat(enc)) {
    // TODO: handle this in a way that doesn't leak parser state
    // rb_raise(rb_eArgError, "%s is not ASCII compatible", parser_enc_name(enc));
    return;
  }

  parser_state->enc = enc;
}

static int
comment_at_top(rb_parser_state* parser_state)
{
  const char *p = lex_pbeg, *pend = lex_p - 1;
  if(line_count != (has_shebang ? 2 : 1)) return FALSE;
  while(p < pend) {
    if (!ISSPACE(*p)) return FALSE;
    p++;
  }
  return TRUE;
}

typedef long (*rb_magic_comment_length_t)(rb_parser_state* parser_state, const char *name, long len);
typedef void (*rb_magic_comment_setter_t)(rb_parser_state* parser_state, const char *name, const char *val);

static void
magic_comment_encoding(rb_parser_state* parser_state, const char *name, const char *val)
{
  if(!comment_at_top(parser_state)) {
    return;
  }
  parser_set_encode(parser_state, val);
}

struct magic_comment {
    const char *name;
    rb_magic_comment_setter_t func;
    rb_magic_comment_length_t length;
};

static const struct magic_comment magic_comments[] = {
    {"coding", magic_comment_encoding, parser_encode_length},
    {"encoding", magic_comment_encoding, parser_encode_length},
};

static const char *
magic_comment_marker(const char *str, long len)
{
  long i = 2;

  while(i < len) {
    switch(str[i]) {
    case '-':
      if(str[i-1] == '*' && str[i-2] == '-') {
        return str + i + 1;
      }
      i += 2;
      break;
    case '*':
      if(i + 1 >= len) return 0;
      if(str[i+1] != '-') {
        i += 4;
      } else if(str[i-1] != '-') {
        i += 2;
      } else {
        return str + i + 2;
      }
      break;
    default:
      i += 3;
      break;
    }
  }
  return 0;
}

static int
parser_magic_comment(rb_parser_state* parser_state, const char *str, long len)
{
  VALUE name = 0, val = 0;
  const char *beg, *end, *vbeg, *vend;

#define str_copy(_s, _p, _n) ((_s) \
    ? (void)(rb_str_resize((_s), (_n)), \
    MEMCPY(RSTRING_PTR(_s), (_p), char, (_n)), (_s)) \
    : (void)((_s) = REF(STR_NEW((_p), (_n)))))

  if(len <= 7) return FALSE;
  if(!(beg = magic_comment_marker(str, len))) return FALSE;
  if(!(end = magic_comment_marker(beg, str + len - beg))) return FALSE;
  str = beg;
  len = end - beg - 3;

  /* %r"([^\\s\'\":;]+)\\s*:\\s*(\"(?:\\\\.|[^\"])*\"|[^\"\\s;]+)[\\s;]*" */
  while(len > 0) {
    const struct magic_comment *p = magic_comments;
    char *s;
    int i;
    long n = 0;

    for(; len > 0 && *str; str++, --len) {
      switch(*str) {
      case '\'': case '"': case ':': case ';':
        continue;
      }
      if(!ISSPACE(*str)) break;
    }
    for(beg = str; len > 0; str++, --len) {
      switch(*str) {
      case '\'': case '"': case ':': case ';':
        break;
      default:
        if(ISSPACE(*str)) break;
        continue;
      }
      break;
    }
    for(end = str; len > 0 && ISSPACE(*str); str++, --len) {
      // nothing
    }
    if(!len) break;
    if(*str != ':') continue;

    do str++; while(--len > 0 && ISSPACE(*str));
    if(!len) break;
    if(*str == '"') {
      for(vbeg = ++str; --len > 0 && *str != '"'; str++) {
        if(*str == '\\') {
          --len;
          ++str;
        }
      }
      vend = str;
      if(len) {
        --len;
        ++str;
      }
    } else {
      for(vbeg = str;
          len > 0 && *str != '"' && *str != ';' && !ISSPACE(*str);
          --len, str++) {
        // nothing
      }
      vend = str;
    }
    while(len > 0 && (*str == ';' || ISSPACE(*str))) --len, str++;

    n = end - beg;
    str_copy(name, beg, n);
    s = RSTRING_PTR(name);
    for(i = 0; i < n; ++i) {
      if(s[i] == '-') s[i] = '_';
    }
    do {
      if(strncasecmp(p->name, s, n) == 0) {
        n = vend - vbeg;
        if(p->length) {
          n = (*p->length)(parser_state, vbeg, n);
        }
        str_copy(val, vbeg, n);
        (*p->func)(parser_state, s, RSTRING_PTR(val));
        break;
      }
    } while(++p < magic_comments + numberof(magic_comments));
  }

  return TRUE;
}

static void
set_file_encoding(rb_parser_state* parser_state, const char *str, const char *send)
{
  int sep = 0;
  const char *beg = str;
  VALUE s;

  for(;;) {
    if(send - str <= 6) return;
    switch(str[6]) {
    case 'C': case 'c': str += 6; continue;
    case 'O': case 'o': str += 5; continue;
    case 'D': case 'd': str += 4; continue;
    case 'I': case 'i': str += 3; continue;
    case 'N': case 'n': str += 2; continue;
    case 'G': case 'g': str += 1; continue;
    case '=': case ':':
      sep = 1;
      str += 6;
      break;
    default:
      str += 6;
      if(ISSPACE(*str)) break;
      continue;
    }
    if(strncasecmp(str-6, "coding", 6) == 0) break;
  }

  for(;;) {
    do {
      if(++str >= send) return;
    } while(ISSPACE(*str));
    if(sep) break;
    if(*str != '=' && *str != ':') return;
    sep = 1;
    str++;
  }

  beg = str;
  while((*str == '-' || *str == '_' || ISALNUM(*str)) && ++str < send) {
    // nothing
  }
  s = REF(rb_str_new(beg, parser_encode_length(parser_state, beg, str - beg)));
  parser_set_encode(parser_state, RSTRING_PTR(s));
  rb_str_resize(s, 0);
}

static void
parser_prepare(rb_parser_state* parser_state)
{
  int c = nextc();
  switch(c) {
  case '#':
    if(peek('!')) has_shebang = 1;
    break;
  case 0xef:		/* UTF-8 BOM marker */
    if(lex_pend - lex_p >= 2 &&
        (unsigned char)lex_p[0] == 0xbb &&
        (unsigned char)lex_p[1] == 0xbf) {
      parser_state->enc = parser_utf8_encoding();
      lex_p += 2;
      lex_pbeg = lex_p;
      return;
    }
    break;
  case EOF:
    return;
  }
  pushback(c);
  parser_state->enc = parser_enc_get(lex_lastline);
}

#define IS_ARG()        (lex_state == EXPR_ARG \
                         || lex_state == EXPR_CMDARG)
#define IS_END()        (lex_state == EXPR_END \
                         || lex_state == EXPR_ENDARG \
                         || lex_state == EXPR_ENDFN)
#define IS_BEG()        (lex_state == EXPR_BEG \
                         || lex_state == EXPR_MID \
                         || lex_state == EXPR_VALUE \
                         || lex_state == EXPR_CLASS)
#define IS_SPCARG(c)    (IS_ARG() && space_seen && !ISSPACE(c))
#define IS_LABEL_POSSIBLE() ((lex_state == EXPR_BEG && !cmd_state) || IS_ARG())
#define IS_LABEL_SUFFIX(n) (peek_n(':',(n)) && !peek_n(':', (n)+1))

#define ambiguous_operator(op, syn) ( \
    rb_warning0("`"op"' after local variable is interpreted as binary operator"), \
    rb_warning0("even though it seems like "syn""))
#define warn_balanced(op, syn) \
    (last_state != EXPR_CLASS && last_state != EXPR_DOT && \
     last_state != EXPR_FNAME && last_state != EXPR_ENDFN && \
     last_state != EXPR_ENDARG && \
     space_seen && !ISSPACE(c) && \
     (ambiguous_operator(op, syn), 0))

static int
parser_yylex(rb_parser_state *parser_state)
{
  register int c;
  int space_seen = 0;
  int cmd_state;
  enum lex_state_e last_state;
  rb_encoding *enc;
  int mb;

  if(lex_strterm) {
    int token;
    if(nd_type(lex_strterm) == NODE_HEREDOC) {
      token = here_document(lex_strterm);
      if(token == tSTRING_END) {
        lex_strterm = 0;
        lex_state = EXPR_END;
      }
    } else {
      token = parse_string(lex_strterm);
      if(token == tSTRING_END || token == tREGEXP_END) {
        lex_strterm = 0;
        lex_state = EXPR_END;
      }
    }
    return token;
  }

  cmd_state = command_start;
  command_start = FALSE;
retry:
  last_state = lex_state;
  switch(c = nextc()) {
  case '\0':                /* NUL */
  case '\004':              /* ^D */
  case '\032':              /* ^Z */
  case -1:                  /* end of script. */
    return 0;

    /* white spaces */
  case ' ': case '\t': case '\f': case '\r':
  case '\13': /* '\v' */
    space_seen = 1;
    goto retry;

  case '#':         /* it's a comment */
    /* no magic_comment in shebang line */
    if(!parser_magic_comment(parser_state, lex_p, lex_pend - lex_p)) {
      if(comment_at_top(parser_state)) {
        set_file_encoding(parser_state, lex_p, lex_pend);
      }
    }

    lex_p = lex_pend;
    /* fall through */
  case '\n':
    switch(lex_state) {
    case EXPR_BEG:
    case EXPR_FNAME:
    case EXPR_DOT:
    case EXPR_CLASS:
    case EXPR_VALUE:
      goto retry;
    default:
      break;
    }

    while((c = nextc())) {
      switch(c) {
      case ' ': case '\t': case '\f': case '\r':
      case '\13': /* '\v' */
        space_seen = 1;
        break;
      case '.': {
        if((c = nextc()) != '.') {
          pushback(c);
          pushback('.');
          goto retry;
        }
      }
      default:
        --sourceline;
        lex_nextline = lex_lastline;
      case -1:         /* EOF no decrement*/
        lex_goto_eol(parser_state);
        goto normal_newline;
      }
    }

  normal_newline:
    command_start = TRUE;
    lex_state = EXPR_BEG;
    return '\n';

  case '*':
    if((c = nextc()) == '*') {
      if((c = nextc()) == '=') {
        set_yylval_id(tPOW);
        lex_state = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(c);
      c = tPOW;
    } else {
      if(c == '=') {
        set_yylval_id('*');
        lex_state = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(c);
      if(IS_SPCARG(c)){
        rb_warning("`*' interpreted as argument prefix");
        c = tSTAR;
      } else if(IS_BEG()) {
        c = tSTAR;
      } else {
        warn_balanced("*", "argument prefix");
        c = '*';
      }
    }
    switch(lex_state) {
    case EXPR_FNAME: case EXPR_DOT:
      lex_state = EXPR_ARG; break;
    default:
      lex_state = EXPR_BEG; break;
    }
    return c;

  case '!':
    c = nextc();
    if(lex_state == EXPR_FNAME || lex_state == EXPR_DOT) {
      lex_state = EXPR_ARG;
      if(c == '@') {
        return '!';
      }
    } else {
      lex_state = EXPR_BEG;
    }
    if(c == '=') {
      return tNEQ;
    }
    if(c == '~') {
      return tNMATCH;
    }
    pushback(c);
    return '!';

  case '=':
    if(was_bol()) {
      /* skip embedded rd document */
      if(strncmp(lex_p, "begin", 5) == 0 && ISSPACE(lex_p[5])) {
        for(;;) {
          lex_goto_eol(parser_state);
          c = nextc();
          if(c == -1) {
            rb_compile_error(parser_state, "embedded document meets end of file");
            return 0;
          }
          if(c != '=') continue;
          if(strncmp(lex_p, "end", 3) == 0 &&
              (lex_p + 3 == lex_pend || ISSPACE(lex_p[3]))) {
            break;
          }
        }
        lex_goto_eol(parser_state);
        goto retry;
      }
    }

    switch(lex_state) {
    case EXPR_FNAME: case EXPR_DOT:
      lex_state = EXPR_ARG; break;
    default:
      lex_state = EXPR_BEG; break;
    }
    if((c = nextc()) == '=') {
      if((c = nextc()) == '=') {
        return tEQQ;
      }
      pushback(c);
      return tEQ;
    }
    if(c == '~') {
      return tMATCH;
    } else if(c == '>') {
      return tASSOC;
    }
    pushback(c);
    return '=';

  case '<':
    last_state = lex_state;
    c = nextc();
    if(c == '<' &&
      lex_state != EXPR_DOT &&
      lex_state != EXPR_CLASS &&
      !IS_END() &&
      (!IS_ARG() || space_seen)) {
      int token = heredoc_identifier();
      if(token) return token;
    }
    switch(lex_state) {
    case EXPR_FNAME: case EXPR_DOT:
      lex_state = EXPR_ARG; break;
    default:
      lex_state = EXPR_BEG; break;
    }
    if(c == '=') {
      if((c = nextc()) == '>') {
        return tCMP;
      }
      pushback(c);
      return tLEQ;
    }
    if(c == '<') {
      if((c = nextc()) == '=') {
        set_yylval_id(tLSHFT);
        lex_state = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(c);
      warn_balanced("<<", "here document");
      return tLSHFT;
    }
    pushback(c);
    return '<';

  case '>':
    switch(lex_state) {
    case EXPR_FNAME: case EXPR_DOT:
      lex_state = EXPR_ARG; break;
    default:
      lex_state = EXPR_BEG; break;
    }
    if((c = nextc()) == '=') {
      return tGEQ;
    }
    if(c == '>') {
      if((c = nextc()) == '=') {
        set_yylval_id(tRSHFT);
        lex_state = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(c);
      return tRSHFT;
    }
    pushback(c);
    return '>';

  case '"':
    lex_strterm = NEW_STRTERM(str_dquote, '"', 0);
    return tSTRING_BEG;

  case '`':
    if(lex_state == EXPR_FNAME) {
      lex_state = EXPR_ENDFN;
      return c;
    }
    if(lex_state == EXPR_DOT) {
      if(cmd_state)
        lex_state = EXPR_CMDARG;
      else
        lex_state = EXPR_ARG;
      return c;
    }
    lex_strterm = NEW_STRTERM(str_xquote, '`', 0);
    return tXSTRING_BEG;

  case '\'':
    lex_strterm = NEW_STRTERM(str_squote, '\'', 0);
    return tSTRING_BEG;

  case '?':
    if(IS_END()) {
      lex_state = EXPR_VALUE;
      return '?';
    }
    c = nextc();
    if(c == -1) {
      rb_compile_error(parser_state, "incomplete character syntax");
      return 0;
    }
    if(parser_enc_isspace(c, parser_state->enc)) {
      if(!IS_ARG()){
        int c2 = 0;
        switch(c) {
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
        if(c2) {
          rb_warn("invalid character syntax; use ?\\%c", c2);
        }
      }
    ternary:
      pushback(c);
      lex_state = EXPR_VALUE;
      return '?';
    }

    newtok();
    enc = parser_state->enc;
    if(!parser_isascii()) {
      if(tokadd_mbchar(c) == -1) return 0;
    } else if((parser_enc_isalnum(c, parser_state->enc) || c == '_') &&
              lex_p < lex_pend && is_identchar(lex_p, lex_pend, parser_state->enc)) {
      goto ternary;
    } else if(c == '\\') {
      if(peek('u')) {
        nextc();
        c = parser_tokadd_utf8(parser_state, &enc, 0, 0, 0);
        if(0x80 <= c) {
          tokaddmbc(c, enc);
        } else {
          tokadd(c);
        }
      } else if(!lex_eol_p() && !(c = *lex_p, ISASCII(c))) {
        nextc();
        if(tokadd_mbchar(c) == -1) return 0;
      } else {
        c = read_escape(0, &enc);
        tokadd(c);
      }
    } else {
      tokadd(c);
    }
    tokfix();
    set_yylval_str(STR_NEW3(tok(), toklen(), enc, 0));
    lex_state = EXPR_END;
    return tCHAR;

  case '&':
    if((c = nextc()) == '&') {
      lex_state = EXPR_BEG;
      if((c = nextc()) == '=') {
        set_yylval_id(tANDOP);
        lex_state = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(c);
      return tANDOP;
    } else if(c == '=') {
      set_yylval_id('&');
      lex_state = EXPR_BEG;
      return tOP_ASGN;
    }
    pushback(c);
    if(IS_SPCARG(c)){
      rb_warning("`&' interpreted as argument prefix");
      c = tAMPER;
    } else if(IS_BEG()) {
      c = tAMPER;
    } else {
      warn_balanced("&", "argument prefix");
      c = '&';
    }
    switch(lex_state) {
    case EXPR_FNAME: case EXPR_DOT:
      lex_state = EXPR_ARG;
      break;
    default:
      lex_state = EXPR_BEG;
    }
    return c;

  case '|':
    if((c = nextc()) == '|') {
      lex_state = EXPR_BEG;
      if((c = nextc()) == '=') {
        set_yylval_id(tOROP);
        lex_state = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(c);
      return tOROP;
    }
    if(c == '=') {
      set_yylval_id('|');
      lex_state = EXPR_BEG;
      return tOP_ASGN;
    }
    if(lex_state == EXPR_FNAME || lex_state == EXPR_DOT) {
      lex_state = EXPR_ARG;
    } else {
      lex_state = EXPR_BEG;
    }
    pushback(c);
    return '|';

  case '+':
    c = nextc();
    if(lex_state == EXPR_FNAME || lex_state == EXPR_DOT) {
      lex_state = EXPR_ARG;
      if(c == '@') {
        return tUPLUS;
      }
      pushback(c);
      return '+';
    }
    if(c == '=') {
      set_yylval_id('+');
      lex_state = EXPR_BEG;
      return tOP_ASGN;
    }
    if(IS_BEG() || (IS_SPCARG(c) && arg_ambiguous())) {
      lex_state = EXPR_BEG;
      pushback(c);
      if(c != -1 && ISDIGIT(c)) {
        c = '+';
        goto start_num;
      }
      return tUPLUS;
    }
    lex_state = EXPR_BEG;
    pushback(c);
    warn_balanced("+", "unary operator");
    return '+';

  case '-':
    c = nextc();
    if(lex_state == EXPR_FNAME || lex_state == EXPR_DOT) {
      lex_state = EXPR_ARG;
      if(c == '@') {
        return tUMINUS;
      }
      pushback(c);
      return '-';
    }
    if(c == '=') {
      set_yylval_id('-');
      lex_state = EXPR_BEG;
      return tOP_ASGN;
    }
    if(c == '>') {
      lex_state = EXPR_ARG;
      return tLAMBDA;
    }
    if(IS_BEG() || (IS_SPCARG(c) && arg_ambiguous())) {
      lex_state = EXPR_BEG;
      pushback(c);
      if(c != -1 && ISDIGIT(c)) {
        return tUMINUS_NUM;
      }
      return tUMINUS;
    }
    lex_state = EXPR_BEG;
    pushback(c);
    warn_balanced("-", "unary operator");
    return '-';

  case '.':
    lex_state = EXPR_BEG;
    if((c = nextc()) == '.') {
      if((c = nextc()) == '.') {
        return tDOT3;
      }
      pushback(c);
      return tDOT2;
    }
    pushback(c);
    if(c != -1 && ISDIGIT(c)) {
      yy_error("no .<digit> floating literal anymore; put 0 before dot");
    }
    lex_state = EXPR_DOT;
    return '.';

  start_num:
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
    {
      int is_float, seen_point, seen_e, nondigit;

      is_float = seen_point = seen_e = nondigit = 0;
      lex_state = EXPR_END;
      newtok();
      if(c == '-' || c == '+') {
        tokadd(c);
        c = nextc();
      }
      if(c == '0') {
#define no_digits() do {yy_error("numeric literal without digits"); return 0;} while(0)
        int start = toklen();
        c = nextc();
        if(c == 'x' || c == 'X') {
          /* hexadecimal */
          c = nextc();
          if(c != -1 && ISXDIGIT(c)) {
            do {
              if(c == '_') {
                if(nondigit) break;
                nondigit = c;
                continue;
              }
              if(!ISXDIGIT(c)) break;
              nondigit = 0;
              tokadd(c);
            } while((c = nextc()) != -1);
          }
          pushback(c);
          tokfix();
          if(toklen() == start) {
            no_digits();
          } else if(nondigit) {
            goto trailing_uc;
          }
          set_yylval_number(rb_cstr_to_inum(tok(), 16, FALSE));
          return tINTEGER;
        }

        if(c == 'b' || c == 'B') {
          /* binary */
          c = nextc();
          if(c == '0' || c == '1') {
            do {
              if(c == '_') {
                if(nondigit) break;
                nondigit = c;
                continue;
              }
              if(c != '0' && c != '1') break;
              nondigit = 0;
              tokadd(c);
            } while((c = nextc()) != -1);
          }
          pushback(c);
          tokfix();
          if(toklen() == start) {
            no_digits();
          } else if(nondigit) {
            goto trailing_uc;
          }
          set_yylval_number(rb_cstr_to_inum(tok(), 2, FALSE));
          return tINTEGER;
        }

        if(c == 'd' || c == 'D') {
          /* decimal */
          c = nextc();
          if(c != -1 && ISDIGIT(c)) {
            do {
              if(c == '_') {
                if(nondigit) break;
                nondigit = c;
                continue;
              }
              if(!ISDIGIT(c)) break;
              nondigit = 0;
              tokadd(c);
            } while((c = nextc()) != -1);
          }
          pushback(c);
          tokfix();
          if(toklen() == start) {
            no_digits();
          } else if(nondigit) {
            goto trailing_uc;
          }
          set_yylval_number(rb_cstr_to_inum(tok(), 10, FALSE));
          return tINTEGER;
        }

        if(c == '_') {
          /* 0_0 */
          goto octal_number;
        }

        if(c == 'o' || c == 'O') {
          /* prefixed octal */
          c = nextc();
          if(c == -1 || c == '_' || !ISDIGIT(c)) {
            no_digits();
          }
        }

        if(c >= '0' && c <= '7') {
          /* octal */
          octal_number:
          do {
            if(c == '_') {
              if(nondigit) break;
              nondigit = c;
              continue;
            }
            if(c < '0' || c > '9') break;
            if(c > '7') goto invalid_octal;
            nondigit = 0;
            tokadd(c);
          } while((c = nextc()) != -1);

          if(toklen() > start) {
            pushback(c);
            tokfix();
            if(nondigit) goto trailing_uc;
            set_yylval_number(rb_cstr_to_inum(tok(), 8, FALSE));
            return tINTEGER;
          }
          if(nondigit) {
            pushback(c);
            goto trailing_uc;
          }
        }

        if(c > '7' && c <= '9') {
          invalid_octal:
          yy_error("Invalid octal digit");
        } else if(c == '.' || c == 'e' || c == 'E') {
          tokadd('0');
        } else {
          pushback(c);
          set_yylval_number(INT2FIX(0));
          return tINTEGER;
        }
      }

      for(;;) {
        switch(c) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          nondigit = 0;
          tokadd(c);
          break;

        case '.':
          if(nondigit) goto trailing_uc;
          if(seen_point || seen_e) {
            goto decode_num;
          } else {
            int c0 = nextc();
            if(c0 == -1 || !ISDIGIT(c0)) {
              pushback(c0);
              goto decode_num;
            }
            c = c0;
          }
          tokadd('.');
          tokadd(c);
          is_float++;
          seen_point++;
          nondigit = 0;
          break;

        case 'e':
        case 'E':
          if(nondigit) {
            pushback(c);
            c = nondigit;
            goto decode_num;
          }
          if(seen_e) {
            goto decode_num;
          }
          tokadd(c);
          seen_e++;
          is_float++;
          nondigit = c;
          c = nextc();
          if(c != '-' && c != '+') continue;
          tokadd(c);
          nondigit = c;
          break;

        case '_':      /* `_' in number just ignored */
          if(nondigit) goto decode_num;
          nondigit = c;
          break;

        default:
          goto decode_num;
      }
      c = nextc();
    }

  decode_num:
    pushback(c);
    if(nondigit) {
      char tmp[30];
      trailing_uc:
      snprintf(tmp, sizeof(tmp), "trailing `%c' in number", nondigit);
      yy_error(tmp);
    }
    tokfix();
    if(is_float) {
      double d = strtod(tok(), 0);
      if(errno == ERANGE) {
        rb_warningS("Float %s out of range", tok());
        errno = 0;
      }
      set_yylval_float(rb_float_new(d));
      return tFLOAT;
    }
    set_yylval_number(rb_cstr_to_inum(tok(), 10, FALSE));
    return tINTEGER;
  }

  case ')':
  case ']':
    paren_nest--;
  case '}':
    COND_LEXPOP();
    CMDARG_LEXPOP();
    if(c == ')') {
      lex_state = EXPR_ENDFN;
    } else {
      lex_state = EXPR_ENDARG;
    }
    return c;

  case ':':
    c = nextc();
    if(c == ':') {
      if(IS_BEG() || lex_state == EXPR_CLASS || IS_SPCARG(-1)) {
        lex_state = EXPR_BEG;
        return tCOLON3;
      }
      lex_state = EXPR_DOT;
      return tCOLON2;
    }
    if(IS_END() || ISSPACE(c)) {
      pushback(c);
      warn_balanced(":", "symbol literal");
      lex_state = EXPR_BEG;
      return ':';
    }
    switch(c) {
    case '\'':
      lex_strterm = NEW_STRTERM(str_ssym, c, 0);
      break;
    case '"':
      lex_strterm = NEW_STRTERM(str_dsym, c, 0);
      break;
    default:
      pushback(c);
      break;
    }
    lex_state = EXPR_FNAME;
    return tSYMBEG;

  case '/':
    if(IS_BEG()) {
      lex_strterm = NEW_STRTERM(str_regexp, '/', 0);
      return tREGEXP_BEG;
    }
    if((c = nextc()) == '=') {
      set_yylval_id('/');
      lex_state = EXPR_BEG;
      return tOP_ASGN;
    }
    pushback(c);
    if(IS_SPCARG(c)) {
      (void)arg_ambiguous();
      lex_strterm = NEW_STRTERM(str_regexp, '/', 0);
      return tREGEXP_BEG;
    }
    switch(lex_state) {
    case EXPR_FNAME: case EXPR_DOT:
      lex_state = EXPR_ARG;
      break;
    default:
      lex_state = EXPR_BEG;
      break;
    }
    warn_balanced("/", "regexp literal");
    return '/';

  case '^':
    if((c = nextc()) == '=') {
      set_yylval_id('^');
      lex_state = EXPR_BEG;
      return tOP_ASGN;
    }
    switch(lex_state) {
    case EXPR_FNAME: case EXPR_DOT:
      lex_state = EXPR_ARG;
      break;
    default:
      lex_state = EXPR_BEG;
      break;
    }
    pushback(c);
    return '^';

  case ';':
    lex_state = EXPR_BEG;
    command_start = TRUE;
    return ';';
  case ',':
    lex_state = EXPR_BEG;
    return ',';

  case '~':
    if(lex_state == EXPR_FNAME || lex_state == EXPR_DOT) {
      if((c = nextc()) != '@') {
        pushback(c);
      }
      lex_state = EXPR_ARG;
    } else {
      lex_state = EXPR_BEG;
    }
    return '~';

  case '(':
    if(IS_BEG()) {
      c = tLPAREN;
    } else if(IS_SPCARG(-1)) {
      c = tLPAREN_ARG;
    }
    paren_nest++;
    COND_PUSH(0);
    CMDARG_PUSH(0);
    lex_state = EXPR_BEG;
    return c;

  case '[':
    paren_nest++;
    if(lex_state == EXPR_FNAME || lex_state == EXPR_DOT) {
      lex_state = EXPR_ARG;
      if((c = nextc()) == ']') {
        if((c = nextc()) == '=') {
          return tASET;
        }
        pushback(c);
        return tAREF;
      }
      pushback(c);
      return '[';
    } else if(IS_BEG()) {
      c = tLBRACK;
    } else if(IS_ARG() && space_seen) {
      c = tLBRACK;
    }
    lex_state = EXPR_BEG;
    COND_PUSH(0);
    CMDARG_PUSH(0);
    return c;

  case '{':
    if(lpar_beg && lpar_beg == paren_nest) {
      lex_state = EXPR_BEG;
      lpar_beg = 0;
      --paren_nest;
      COND_PUSH(0);
      CMDARG_PUSH(0);
      return tLAMBEG;
    }
    if(IS_ARG() || lex_state == EXPR_END || lex_state == EXPR_ENDFN)
      c = '{';          /* block (primary) */
    else if(lex_state == EXPR_ENDARG)
      c = tLBRACE_ARG;  /* block (expr) */
    else
      c = tLBRACE;      /* hash */
    COND_PUSH(0);
    CMDARG_PUSH(0);
    lex_state = EXPR_BEG;
    if(c != tLBRACE) command_start = TRUE;
    return c;

  case '\\':
    c = nextc();
    if(c == '\n') {
      space_seen = 1;
      goto retry; /* skip \\n */
    }
    pushback(c);
    return '\\';

  case '%':
    if(IS_BEG()) {
      intptr_t term;
      intptr_t paren;

      c = nextc();
    quotation:
      if(c == -1 || !ISALNUM(c)) {
        term = c;
        c = 'Q';
      } else {
        term = nextc();
        if(parser_enc_isalnum((int)term, parser_state->enc) || !parser_isascii()) {
          yy_error("unknown type of % string");
          return 0;
        }
      }
      if(c == -1 || term == -1) {
        rb_compile_error(parser_state, "unterminated quoted string meets end of file");
        return 0;
      }
      paren = term;
      if(term == '(') term = ')';
      else if(term == '[') term = ']';
      else if(term == '{') term = '}';
      else if(term == '<') term = '>';
      else paren = 0;

      switch(c) {
      case 'Q':
        lex_strterm = NEW_STRTERM(str_dquote, term, paren);
        return tSTRING_BEG;

      case 'q':
        lex_strterm = NEW_STRTERM(str_squote, term, paren);
        return tSTRING_BEG;

      case 'W':
        lex_strterm = NEW_STRTERM(str_dword, term, paren);
        do {c = nextc();} while(ISSPACE(c));
        pushback(c);
        return tWORDS_BEG;

      case 'w':
        lex_strterm = NEW_STRTERM(str_sword, term, paren);
        do {c = nextc();} while(ISSPACE(c));
        pushback(c);
        return tQWORDS_BEG;

      case 'x':
        lex_strterm = NEW_STRTERM(str_xquote, term, paren);
        return tXSTRING_BEG;

      case 'r':
        lex_strterm = NEW_STRTERM(str_regexp, term, paren);
        return tREGEXP_BEG;

      case 's':
        lex_strterm = NEW_STRTERM(str_ssym, term, paren);
        lex_state = EXPR_FNAME;
        return tSYMBEG;

      default:
        yy_error("unknown type of % string");
        return 0;
      }
    }
    if((c = nextc()) == '=') {
      set_yylval_id('%');
      lex_state = EXPR_BEG;
      return tOP_ASGN;
    }
    if(IS_SPCARG(c)) {
      goto quotation;
    }
    switch(lex_state) {
    case EXPR_FNAME: case EXPR_DOT:
      lex_state = EXPR_ARG;
      break;
    default:
      lex_state = EXPR_BEG;
      break;
    }
    pushback(c);
    warn_balanced("%%", "string literal");
    return '%';

  case '$':
    lex_state = EXPR_END;
    newtok();
    c = nextc();
    switch(c) {
    case '_':             /* $_: last read line string */
      c = nextc();
      if(parser_is_identchar()) {
        tokadd('$');
        tokadd('_');
        break;
      }
      pushback(c);
      c = '_';
      /* fall through */
    case '~':   /* $~: match-data */
    case '*':   /* $*: argv */
    case '$':   /* $$: pid */
    case '?':   /* $?: last status */
    case '!':   /* $!: error string */
    case '@':   /* $@: error position */
    case '/':   /* $/: input record separator */
    case '\\':  /* $\: output record separator */
    case ';':   /* $;: field separator */
    case ',':   /* $,: output field separator */
    case '.':   /* $.: last read line number */
    case '=':   /* $=: ignorecase */
    case ':':   /* $:: load path */
    case '<':   /* $<: reading filename */
    case '>':   /* $>: default output handle */
    case '\"':  /* $": already loaded files */
      tokadd('$');
      tokadd(c);
      tokfix();
      set_yylval_name(parser_intern(tok()));
      return tGVAR;

    case '-':
      tokadd('$');
      tokadd(c);
      c = nextc();
      if(parser_is_identchar()) {
        if(tokadd_mbchar(c) == -1) return 0;
      } else {
        pushback(c);
      }
    gvar:
      tokfix();
      set_yylval_name(parser_intern(tok()));
      return tGVAR;

    case '&':             /* $&: last match */
    case '`':             /* $`: string before last match */
    case '\'':            /* $': string after last match */
    case '+':             /* $+: string matches last paren. */
      if(last_state == EXPR_FNAME) {
        tokadd('$');
        tokadd(c);
        goto gvar;
      }
      set_yylval_node(NEW_BACK_REF(c));
      return tBACK_REF;

    case '1': case '2': case '3':
    case '4': case '5': case '6':
    case '7': case '8': case '9':
      tokadd('$');
      do {
        tokadd(c);
        c = nextc();
      } while(c != -1 && ISDIGIT(c));
      pushback(c);
      if(last_state == EXPR_FNAME) goto gvar;
      tokfix();
      set_yylval_node(NEW_NTH_REF(atoi(tok()+1)));
      return tNTH_REF;

    default:
      if(!parser_is_identchar()) {
        pushback(c);
        return '$';
      }
    case '0':
      tokadd('$');
    }
    break;

  case '@':
    c = nextc();
    newtok();
    tokadd('@');
    if(c == '@') {
      tokadd('@');
      c = nextc();
    }
    if(c != -1 && ISDIGIT(c)) {
      if(tokidx == 1) {
        rb_compile_error(parser_state,
                         "`@%c' is not allowed as an instance variable name", c);
      } else {
        rb_compile_error(parser_state,
                         "`@@%c' is not allowed as a class variable name", c);
      }
      return 0;
    }
    if(!parser_is_identchar()) {
      pushback(c);
      return '@';
    }
    break;

  case '_':
    if(was_bol() && whole_match_p("__END__", 7, 0)) {
      ruby__end__seen = 1;
      eofp = true;
      return -1;
    }
    newtok();
    break;

  default:
    if(!parser_is_identchar()) {
      rb_compile_error(parser_state, "Invalid char `\\x%02X' in expression", c);
      goto retry;
    }

    newtok();
    break;
  }

  mb = ENC_CODERANGE_7BIT;
  do {
    if(!ISASCII(c)) mb = ENC_CODERANGE_UNKNOWN;
    if(tokadd_mbchar(c) == -1) return 0;
    c = nextc();
  } while(parser_is_identchar());
  switch(tok()[0]) {
  case '@': case '$':
    pushback(c);
    break;
  default:
    if((c == '!' || c == '?') && !peek('=')) {
      tokadd(c);
    } else {
      pushback(c);
    }
  }
  tokfix();
  {
    int result = 0;

    last_state = lex_state;
    switch(tok()[0]) {
    case '$':
      lex_state = EXPR_END;
      result = tGVAR;
      break;
    case '@':
      lex_state = EXPR_END;
      if(tok()[1] == '@') {
        result = tCVAR;
      } else {
        result = tIVAR;
      }
      break;
    default:
      if(toklast() == '!' || toklast() == '?') {
        result = tFID;
      } else {
        if(lex_state == EXPR_FNAME) {
          if((c = nextc()) == '=' && !peek('~') && !peek('>') &&
              (!peek('=') || (peek_n('>', 1)))) {
            result = tIDENTIFIER;
            tokadd(c);
            tokfix();
          } else {
            pushback(c);
          }
        }
        if(result == 0 && ISUPPER(tok()[0])) {
          result = tCONSTANT;
        } else {
          result = tIDENTIFIER;
        }
      }

      if(IS_LABEL_POSSIBLE()) {
        if(IS_LABEL_SUFFIX(0)) {
          lex_state = EXPR_BEG;
          nextc();
          set_yylval_name(TOK_INTERN(!ENC_SINGLE(mb)));
          return tLABEL;
        }
      }
      if(mb == ENC_CODERANGE_7BIT && lex_state != EXPR_DOT) {
        const struct kwtable *kw;

        /* See if it is a reserved word.  */
        kw = reserved_word(tok(), toklen());
        if(kw) {
          enum lex_state_e state = lex_state;
          lex_state = kw->state;
          if(state == EXPR_FNAME) {
            set_yylval_name(parser_intern(kw->name));
            return kw->id[0];
          }
          if(kw->id[0] == keyword_do) {
            command_start = TRUE;
            if(lpar_beg && lpar_beg == paren_nest) {
              lpar_beg = 0;
              --paren_nest;
              return keyword_do_LAMBDA;
            }
            if(COND_P()) return keyword_do_cond;
            if(CMDARG_P() && state != EXPR_CMDARG)
              return keyword_do_block;
            if(state == EXPR_ENDARG || state == EXPR_BEG)
              return keyword_do_block;
            return keyword_do;
          }
          if(state == EXPR_BEG || state == EXPR_VALUE)
            return kw->id[0];
          else {
            if(kw->id[0] != kw->id[1])
              lex_state = EXPR_BEG;
            return kw->id[1];
          }
        }
      }

      if(IS_BEG() ||
          lex_state == EXPR_DOT ||
          IS_ARG()) {
        if(cmd_state) {
          lex_state = EXPR_CMDARG;
        } else {
          lex_state = EXPR_ARG;
        }
      } else if(lex_state == EXPR_FNAME) {
        lex_state = EXPR_ENDFN;
      } else {
        lex_state = EXPR_END;
      }
    }
    {
      ID ident = TOK_INTERN(!ENC_SINGLE(mb));

      set_yylval_name(ident);
      if(last_state != EXPR_DOT && last_state != EXPR_FNAME &&
          is_local_id(ident) && lvar_defined(ident)) {
        lex_state = EXPR_END;
      }
    }

    return result;
  }
}

#if YYPURE
static int
yylex(void *l, void *p)
#else
yylex(void *p)
#endif
{
  rb_parser_state* parser_state = (rb_parser_state*)p;

  lval = l;
  ((YYSTYPE*)lval)->val = Qundef;

  return parser_yylex(parser_state);
}

VALUE
parser_add_reference(rb_parser_state* parser_state, VALUE obj)
{
  rb_ary_push(references, obj);

  return obj;
}

NODE*
parser_node_newnode(rb_parser_state* parser_state, enum node_type type,
                 VALUE a0, VALUE a1, VALUE a2)
{
  NODE *n = (NODE*)pt_allocate(parser_state, sizeof(NODE));

  n->flags = 0;
  nd_set_type(n, type);
  nd_set_line(n, sourceline);
  n->nd_file = sourcefile;

  n->u1.value = a0;
  n->u2.value = a1;
  n->u3.value = a2;

  return n;
}

enum node_type
nodetype(NODE *node) {  /* for debug */
  return (enum node_type)nd_type(node);
}

int
nodeline(NODE *node) {
  return nd_line(node);
}

static NODE*
parser_newline_node(rb_parser_state* parser_state, NODE *node)
{
  if(node) {
    node = remove_begin(node);
    node->flags |= NODE_FL_NEWLINE;
  }
  return node;
}

static void
fixpos(NODE *node, NODE *orig)
{
  if(!node) return;
  if(!orig) return;
  if(orig == (NODE*)1) return;
  nd_set_line(node, nd_line(orig));
}

/*
static void
parser_warning(rb_parser_state* parser_state, NODE *node, const char *mesg)
{
  int line = ruby_sourceline;
  if(emit_warnings) {
    ruby_sourceline = nd_line(node);
    printf("%s:%li: warning: %s\n", ruby_sourcefile, ruby_sourceline, mesg);
    ruby_sourceline = line;
  }
}
*/

static void
rb_compile_warn(const char *file, int line, const char *fmt, ...)
{
  // TODO
  return;

  char buf[BUFSIZ];
  va_list args;

  if(NIL_P(ruby_verbose)) return;

  snprintf(buf, BUFSIZ, "warning: %s", fmt);

  va_start(args, fmt);
  // compile_warn_print(file, line, buf, args);
  va_end(args);
}

/* rb_compile_warning() reports only in verbose mode */
void
rb_compile_warning(const char *file, int line, const char *fmt, ...)
{
  // TODO
  return;

  char buf[BUFSIZ];
  va_list args;

  snprintf(buf, BUFSIZ, "warning: %s", fmt);

  va_start(args, fmt);
  // compile_warn_print(file, line, buf, args);
  va_end(args);
}

static void
parser_warning(rb_parser_state* parser_state, NODE *node, const char *mesg)
{
  rb_compile_warning(sourcefile, nd_line(node), "%s", mesg);
}
#define parser_warning(node, mesg) parser_warning(parser_state, node, mesg)

static void
parser_warn(rb_parser_state* parser_state, NODE *node, const char *mesg)
{
  rb_compile_warn(sourcefile, nd_line(node), "%s", mesg);
}
#define parser_warn(node, mesg) parser_warn(parser_state, node, mesg)

static NODE*
parser_block_append(rb_parser_state* parser_state, NODE *head, NODE *tail)
{
  NODE *end, *h = head, *nd;

  if(tail == 0) return head;

  if(h == 0) return tail;
  switch(nd_type(h)) {
  case NODE_STR:
  case NODE_LIT:
  case NODE_SELF:
  case NODE_TRUE:
  case NODE_FALSE:
  case NODE_NIL:
    parser_warning(h, "unused literal ignored");
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

  nd = end->nd_head;
  switch(nd_type(nd)) {
  case NODE_RETURN:
  case NODE_BREAK:
  case NODE_NEXT:
  case NODE_REDO:
  case NODE_RETRY:
    if(verbose) {
      parser_warning(nd, "statement not reached");
    }
    break;

  default:
    break;
  }

  if(nd_type(tail) != NODE_BLOCK) {
    tail = NEW_BLOCK(tail);
    tail->nd_end = tail;
  }
  end->nd_next = tail;
  h->nd_end = tail->nd_end;
  return head;
}

/* append item to the list */
static NODE*
parser_list_append(rb_parser_state* parser_state, NODE *list, NODE *item)
{
  NODE *last;

  if(list == 0) return NEW_LIST(item);
  if(list->nd_next) {
    last = list->nd_next->nd_end;
  } else {
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

  if(head->nd_next) {
    last = head->nd_next->nd_end;
  } else {
    last = head;
  }

  head->nd_alen += tail->nd_alen;
  last->nd_next = tail;
  if(tail->nd_next) {
    head->nd_next->nd_end = tail->nd_next->nd_end;
  } else {
    head->nd_next->nd_end = tail;
  }

  return head;
}

static int
parser_literal_concat0(rb_parser_state* parser_state, VALUE head, VALUE tail)
{
  if(NIL_P(tail)) return 1;
  if(!parser_enc_compatible(head, tail)) {
    rb_compile_error(parser_state, "string literal encodings differ (%s / %s)",
    parser_enc_name(parser_enc_get(head)),
    parser_enc_name(parser_enc_get(tail)));
    rb_str_resize(head, 0);
    rb_str_resize(tail, 0);
    return 0;
  }
  rb_str_buf_append(head, tail);
  return 1;
}

/* concat two string literals */
static NODE *
parser_literal_concat(rb_parser_state* parser_state, NODE *head, NODE *tail)
{
  enum node_type htype;

  if(!head) return tail;
  if(!tail) return head;

  htype = (enum node_type)nd_type(head);
  if(htype == NODE_EVSTR) {
    NODE *node = NEW_DSTR(STR_NEW0());
    head = list_append(node, head);
  }
  switch(nd_type(tail)) {
  case NODE_STR:
    if(htype == NODE_STR) {
      if(!literal_concat0(head->nd_lit, tail->nd_lit)) {
      error:
        return 0;
      }
    } else {
      list_append(head, tail);
    }
    break;

  case NODE_DSTR:
    if(htype == NODE_STR) {
      if(!literal_concat0(head->nd_lit, tail->nd_lit))
        goto error;
      tail->nd_lit = head->nd_lit;
      head = tail;
    } else if(NIL_P(tail->nd_lit)) {
      head->nd_alen += tail->nd_alen - 1;
      head->nd_next->nd_end->nd_next = tail->nd_next;
      head->nd_next->nd_end = tail->nd_next->nd_end;
    } else {
      nd_set_type(tail, NODE_ARRAY);
      tail->nd_head = NEW_STR(tail->nd_lit);
      list_concat(head, tail);
    }
    break;

  case NODE_EVSTR:
    if(htype == NODE_STR) {
      nd_set_type(head, NODE_DSTR);
      head->nd_alen = 1;
    }
    list_append(head, tail);
    break;
  }
  return head;
}

static NODE *
parser_evstr2dstr(rb_parser_state* parser_state, NODE *node)
{
  if(nd_type(node) == NODE_EVSTR) {
    node = list_append(NEW_DSTR(STR_NEW0()), node);
  }
  return node;
}

static NODE *
parser_new_evstr(rb_parser_state* parser_state, NODE *node)
{
  NODE *head = node;

  if(node) {
    switch(nd_type(node)) {
    case NODE_STR: case NODE_DSTR: case NODE_EVSTR:
      return node;
    }
  }
  return NEW_EVSTR(head);
}

static NODE *
parser_call_bin_op(rb_parser_state* parser_state, NODE *recv, ID id, NODE *arg1)
{
  value_expr(recv);
  value_expr(arg1);
  return NEW_CALL(recv, convert_op(id), NEW_LIST(arg1));
}

static NODE *
parser_call_uni_op(rb_parser_state* parser_state, NODE *recv, ID id)
{
  value_expr(recv);
  return NEW_CALL(recv, convert_op(id), 0);
}

static const struct {
  ID token;
  const char *name;
} op_tbl[] = {
  {tDOT2,	  ".."},
  {tDOT3,	  "..."},
  {'+',	    "+(binary)"},
  {'-',	    "-(binary)"},
  {tPOW,	  "**"},
  {tUPLUS,	"+@"},
  {tUMINUS,	"-@"},
  {tCMP,	  "<=>"},
  {tGEQ,	  ">="},
  {tLEQ,	  "<="},
  {tEQ,	    "=="},
  {tEQQ,	  "==="},
  {tNEQ,	  "!="},
  {tMATCH,	"=~"},
  {tNMATCH,	"!~"},
  {tAREF,	  "[]"},
  {tASET,	  "[]="},
  {tLSHFT,	"<<"},
  {tRSHFT,	">>"},
  {tCOLON2,	"::"},

  // Added for Rubinius
  {'!',     "!"},
  {'%',     "%"},
  {'&',     "&"},
  {'*',     "*"},
  {'/',     "/"},
  {'<',     "<"},
  {'>',     ">"},
  {'^',     "^"},
  {'`',     "`"},
  {'|',     "|"},
  {'~',     "~"},
};

#define op_tbl_count numberof(op_tbl)

static ID convert_op(ID id) {
  for(int i = 0; i < op_tbl_count; i++) {
    if(op_tbl[i].token == id) {
      return parser_intern(op_tbl[i].name);
    }
  }
  return id;
}

static NODE*
parser_match_op(rb_parser_state* parser_state, NODE *node1, NODE *node2)
{
  value_expr(node1);
  value_expr(node2);
  if(node1) {
    switch(nd_type(node1)) {
    case NODE_DREGX:
    case NODE_DREGX_ONCE:
      return NEW_MATCH2(node1, node2);

    case NODE_REGEX:
        return NEW_MATCH2(node1, node2);
    }
  }

  if(node2) {
    switch(nd_type(node2)) {
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
parser_gettable(rb_parser_state* parser_state, ID id)
{
  if(id == keyword_self) {
    return NEW_SELF();
  } else if(id == keyword_nil) {
    return NEW_NIL();
  } else if(id == keyword_true) {
    return NEW_TRUE();
  } else if(id == keyword_false) {
    return NEW_FALSE();
  } else if(id == keyword__FILE__) {
    return NEW_FILE();
  } else if(id == keyword__LINE__) {
    return NEW_NUMBER(INT2FIX(sourceline));
  } else if(id == keyword__ENCODING__) {
    return NEW_ENCODING(STR_NEW2(parser_enc_name(parser_state->enc)));
  } else if(is_local_id(id)) {
    if((in_block() && bv_defined(id)) || local_id(id)) {
      return NEW_LVAR(id);
    }
    /* method call without arguments */
    return NEW_VCALL(id);
  } else if(is_global_id(id)) {
    return NEW_GVAR(id);
  } else if(is_instance_id(id)) {
    return NEW_IVAR(id);
  } else if(is_const_id(id)) {
    return NEW_CONST(id);
  } else if(is_class_id(id)) {
    return NEW_CVAR(id);
  }
  rb_compile_error(parser_state, "identifier %s is not valid", parser_id2name(id));
  return 0;
}

static NODE*
parser_assignable(rb_parser_state* parser_state, ID id, NODE *val)
{
  if(!id) return 0;
  if(id == keyword_self) {
    yy_error("Can't change the value of self");
  } else if(id == keyword_nil) {
    yy_error("Can't assign to nil");
  } else if(id == keyword_true) {
    yy_error("Can't assign to true");
  } else if(id == keyword_false) {
    yy_error("Can't assign to false");
  } else if(id == keyword__FILE__) {
    yy_error("Can't assign to __FILE__");
  } else if(id == keyword__LINE__) {
    yy_error("Can't assign to __LINE__");
  } else if(id == keyword__ENCODING__) {
    yy_error("Can't assign to __ENCODING__");
  } else if(is_local_id(id)) {
    if(!local_id(id)) {
      local_var(id);
    }
    return NEW_LASGN(id, val);
  } else if(is_global_id(id)) {
    return NEW_GASGN(id, val);
  } else if(is_instance_id(id)) {
    return NEW_IASGN(id, val);
  } else if(is_const_id(id)) {
    if(!in_def && !in_single)
      return NEW_CDECL(id, val, 0);
    yy_error("dynamic constant assignment");
  } else if(is_class_id(id)) {
    return NEW_CVASGN(id, val);
  } else {
    rb_compile_error(parser_state, "identifier %s is not valid to set", parser_id2name(id));
  }
  return 0;
}

static ID
parser_shadowing_lvar(rb_parser_state* parser_state, ID name)
{
  if(parser_intern("_") == name) return name;

  if(in_block()) {
    if(bv_var(name)) {
      yy_error("duplicate argument name");
    } else if(bv_defined(name) || local_id(name)) {
      rb_warningS("shadowing outer local variable - %s", parser_id2name(name));
      vtable_add(locals_table->vars, name);
    }
  } else {
    if(local_id(name)) {
      yy_error("duplicate argument name");
    }
  }

  return name;
}

static void
parser_new_bv(rb_parser_state* parser_state, ID name)
{
  if(!name) return;
  if(!is_local_id(name)) {
    rb_compile_error(parser_state, "invalid local variable - %s", parser_id2name(name));
    return;
  }
  shadowing_lvar(name);
  local_var(name);
}

static const struct vtable*
parser_bv_push(rb_parser_state* parser_state) {
  locals_table->args = vtable_alloc(locals_table->args);
  locals_table->vars = vtable_alloc(locals_table->vars);
  return locals_table->args;
}

static void
bv_pop_tables(rb_parser_state* parser_state)
{
  struct vtable *tmp;

  tmp = locals_table->args;
  locals_table->args = locals_table->args->prev;
  vtable_free(tmp);

  tmp = locals_table->vars;
  locals_table->vars = locals_table->vars->prev;
  vtable_free(tmp);
}

static void
parser_bv_pop(rb_parser_state* parser_state, const struct vtable* args) {
  while(locals_table->args != args) {
    bv_pop_tables(parser_state);
    if(!locals_table->args) {
      struct local_vars *local = locals_table->prev;
      free(locals_table);
      locals_table = local;
    }
  }
  bv_pop_tables(parser_state);
}

static bool
parser_in_block(rb_parser_state* parser_state) {
  return locals_table->vars && locals_table->vars->prev;
}

static bool
parser_bv_defined(rb_parser_state* parser_state, ID id)
{
  struct vtable *vars, *args;

  args = locals_table->args;
  vars = locals_table->vars;

  while(vars) {
    if(vtable_included(args, id)) {
      return true;
    }
    if(vtable_included(vars, id)) {
      return true;
    }
    args = args->prev;
    vars = vars->prev;
  }

  return false;
}

static int
parser_bv_var(rb_parser_state* parser_state, ID id)
{
  return vtable_included(locals_table->args, id) ||
            vtable_included(locals_table->vars, id);
}

static NODE *
parser_aryset(rb_parser_state* parser_state, NODE *recv, NODE *idx)
{
  if(recv && nd_type(recv) == NODE_SELF) {
    recv = (NODE *)1;
  } else {
    value_expr(recv);
  }
  return NEW_ATTRASGN(recv, convert_op(tASET), idx);
}

static void
parser_block_dup_check(rb_parser_state* parser_state, NODE *node1, NODE *node2)
{
  if(node2 && node1 && nd_type(node1) == NODE_BLOCK_PASS) {
    compile_error("both block arg and actual block given");
  }
}

static ID
rb_id_attrset(ID id)
{
  id &= ~ID_SCOPE_MASK;
  id |= ID_ATTRSET;
  return id;
}

static NODE *
parser_attrset(rb_parser_state* parser_state, NODE *recv, ID id)
{
  if(recv && nd_type(recv) == NODE_SELF) {
    recv = (NODE *)1;
  }
  return NEW_ATTRASGN(recv, rb_id_attrset(id), 0);
}

static void
rb_parser_backref_error(rb_parser_state* parser_state, NODE *node)
{
  switch(nd_type(node)) {
  case NODE_NTH_REF:
    rb_compile_error(parser_state, "Can't set variable $%u", node->nd_nth);
    break;
  case NODE_BACK_REF:
    rb_compile_error(parser_state, "Can't set variable $%c", (int)node->nd_nth);
    break;
  }
}

static NODE *
parser_arg_concat(rb_parser_state* parser_state, NODE *node1, NODE *node2)
{
  if(!node2) return node1;
  return NEW_ARGSCAT(node1, node2);
}

static NODE *
parser_arg_append(rb_parser_state* parser_state, NODE *node1, NODE *node2)
{
  if(!node1) return NEW_LIST(node2);
  switch(nd_type(node1))  {
  case NODE_ARRAY:
    return list_append(node1, node2);
  case NODE_BLOCK_PASS:
    node1->nd_head = arg_append(node1->nd_head, node2);
    return node1;
  case NODE_ARGSPUSH:
    node1->nd_body = list_append(NEW_LIST(node1->nd_body), node2);
    nd_set_type(node1, NODE_ARGSCAT);
    return node1;
  }
  return NEW_ARGSPUSH(node1, node2);
}

static NODE *
arg_add(rb_parser_state* parser_state, NODE *node1, NODE *node2)
{
  if(!node1) return NEW_LIST(node2);
  if(nd_type(node1) == NODE_ARRAY) {
    return list_append(node1, node2);
  }
  else {
    return NEW_ARGSPUSH(node1, node2);
  }
}

static NODE *
splat_array(NODE* node)
{
  if(nd_type(node) == NODE_SPLAT) node = node->nd_head;
  if(nd_type(node) == NODE_ARRAY) return node;
  return 0;
}

static NODE*
parser_node_assign(rb_parser_state* parser_state, NODE *lhs, NODE *rhs)
{
  if(!lhs) return 0;

  value_expr(rhs);
  switch(nd_type(lhs)) {
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
    lhs->nd_args = arg_add(parser_state, lhs->nd_args, rhs);
    break;

  default:
    /* should not happen */
    break;
  }

  return lhs;
}

static int
parser_value_expr(rb_parser_state* parser_state, NODE *node)
{
  int cond = 0;

  if(!node) {
    rb_warning("empty expression");
  }

  while(node) {
    switch(nd_type(node)) {
    case NODE_DEFN:
    case NODE_DEFS:
      parser_warning(node, "void value expression");
      return FALSE;

    case NODE_RETURN:
    case NODE_BREAK:
    case NODE_NEXT:
    case NODE_REDO:
    case NODE_RETRY:
      if(!cond) yy_error("void value expression");
      /* or "control never reach"? */
      return FALSE;

    case NODE_BLOCK:
      while(node->nd_next) {
        node = node->nd_next;
      }
      node = node->nd_head;
      break;

    case NODE_BEGIN:
      node = node->nd_body;
      break;

    case NODE_IF:
      if(!node->nd_body) {
        node = node->nd_else;
        break;
      } else if(!node->nd_else) {
        node = node->nd_body;
        break;
      }
      if(!value_expr(node->nd_body)) return FALSE;
      node = node->nd_else;
      break;

    case NODE_AND:
    case NODE_OR:
      cond = 1;
      node = node->nd_2nd;
      break;

    default:
      return TRUE;
    }
  }

  return TRUE;
}

static void
parser_void_stmts(rb_parser_state* parser_state, NODE *node)
{
  if(!RTEST(ruby_verbose)) return;
  if(!node) return;
  if(nd_type(node) != NODE_BLOCK) return;

  for(;;) {
    if(!node->nd_next) return;
    void_expr0(node->nd_head);
    node = node->nd_next;
  }
}

static void
parser_void_expr0(rb_parser_state* parser_state, NODE *node)
{
  const char *useless = NULL;

  if(!verbose) return;

  if(!node) return;
  switch(nd_type(node)) {
  case NODE_CALL:
    switch(node->nd_mid) {
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

  if(useless) {
    int line = sourceline;

    sourceline = nd_line(node);
    rb_warn("useless use of %s in void context", useless);
    sourceline = line;
  }
}

static NODE *
remove_begin(NODE *node)
{
  NODE **n = &node, *n1 = node;
  while(n1 && nd_type(n1) == NODE_BEGIN && n1->nd_body) {
    *n = n1 = n1->nd_body;
  }
  return node;
}

static int
assign_in_cond(NODE *node, rb_parser_state* parser_state)
{
  switch(nd_type(node)) {
  case NODE_MASGN:
    yy_error("multiple assignment in conditional");
    return 1;

  case NODE_LASGN:
  case NODE_DASGN:
  case NODE_DASGN_CURR:
  case NODE_GASGN:
  case NODE_IASGN:
    break;

  default:
    return 0;
  }

  if(!node->nd_value) return 1;
  switch(nd_type(node->nd_value)) {
  case NODE_LIT:
  case NODE_STR:
  case NODE_NIL:
  case NODE_TRUE:
  case NODE_FALSE:
    // always warn
    // TODO
    // parser_warn(node->nd_value, "found = in conditional, should be ==");
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

#define e_option_supplied()   parser_e_option_supplied(parser_state)

static bool
parser_e_option_supplied(rb_parser_state* parser_state)
{
  return strcmp(sourcefile, "-e") == 0;
}

static void
warn_unless_e_option(rb_parser_state* parser_state, NODE *node, const char *str)
{
  if(!e_option_supplied()) parser_warning(node, str);
}

static void
warning_unless_e_option(rb_parser_state* parser_state, NODE *node, const char *str)
{
  if(!e_option_supplied()) parser_warning(node, str);
}

static NODE *cond0(rb_parser_state* parser_state, NODE *node);

static NODE*
range_op(rb_parser_state* parser_state, NODE *node)
{
  if(node == 0) return 0;

  value_expr(node);
  if(nd_type(node) == NODE_LIT && FIXNUM_P(node->nd_lit)) {
    warn_unless_e_option(parser_state, node, "integer literal in conditional range");
    return NEW_CALL(node, tEQ, NEW_LIST(NEW_GVAR(parser_intern("$."))));
  }
  return cond0(parser_state, node);
}

static int
literal_node(NODE *node)
{
  if(!node) return 1;        /* same as NODE_NIL */
  switch(nd_type(node)) {
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
cond0(rb_parser_state* parser_state, NODE *node)
{
  if(node == 0) return 0;
  assign_in_cond(node, parser_state);

  switch(nd_type(node)) {
  case NODE_DSTR:
  case NODE_EVSTR:
  case NODE_STR:
    rb_warn0("string literal in condition");
    break;

  case NODE_DREGX:
  case NODE_DREGX_ONCE:
    warning_unless_e_option(parser_state, node, "regex literal in condition");
    return NEW_MATCH2(node, NEW_GVAR(parser_intern("$_")));

  case NODE_AND:
  case NODE_OR:
    node->nd_1st = cond0(parser_state, node->nd_1st);
    node->nd_2nd = cond0(parser_state, node->nd_2nd);
    break;

  case NODE_DOT2:
  case NODE_DOT3:
    node->nd_beg = range_op(parser_state, node->nd_beg);
    node->nd_end = range_op(parser_state, node->nd_end);
    if(nd_type(node) == NODE_DOT2) nd_set_type(node,NODE_FLIP2);
    else if(nd_type(node) == NODE_DOT3) nd_set_type(node, NODE_FLIP3);
    if(!e_option_supplied()) {
      int b = literal_node(node->nd_beg);
      int e = literal_node(node->nd_end);
      if((b == 1 && e == 1) || (b + e >= 2 && verbose)) {
        parser_warn(node, "range literal in condition");
      }
    }
    break;

  case NODE_DSYM:
    parser_warning(node, "literal in condition");
    break;

  case NODE_LIT:
    parser_warning(node, "literal in condition");
    break;

  case NODE_REGEX:
    warn_unless_e_option(parser_state, node, "regex literal in condition");
    nd_set_type(node, NODE_MATCH);
  default:
    break;
  }
  return node;
}

static NODE*
parser_cond(rb_parser_state* parser_state, NODE *node)
{
  if(node == 0) return 0;
  return cond0(parser_state, node);
}

static NODE*
parser_logop(rb_parser_state* parser_state, enum node_type type, NODE *left, NODE *right)
{
  value_expr(left);
  if(left && nd_type(left) == type) {
    NODE *node = left, *second;
    while((second = node->nd_2nd) != 0 && nd_type(second) == type) {
      node = second;
    }
    node->nd_2nd = NEW_NODE(type, second, right, 0);
    return left;
  }
  return NEW_NODE(type, left, right, 0);
}

static void
no_blockarg(rb_parser_state* parser_state, NODE *node)
{
  if(node && nd_type(node) == NODE_BLOCK_PASS) {
    rb_compile_error(parser_state, "block argument should not be given");
  }
}

static NODE *
parser_ret_args(rb_parser_state* parser_state, NODE *node)
{
  if(node) {
    no_blockarg(parser_state, node);
    if(nd_type(node) == NODE_ARRAY) {
      if(node->nd_next == 0) {
        node = node->nd_head;
      } else {
        nd_set_type(node, NODE_VALUES);
      }
    }
  }
  return node;
}

static NODE *
parser_new_yield(rb_parser_state* parser_state, NODE *node)
{
  VALUE state = Qtrue;

  if(node) {
    no_blockarg(parser_state, node);
    if(node && nd_type(node) == NODE_SPLAT) {
      state = Qtrue;
    }
  } else {
    state = Qfalse;
  }
  return NEW_YIELD(node, state);
}

static NODE*
parser_negate_lit(rb_parser_state* parser_state, NODE *node)
{
  switch(TYPE(node->nd_lit)) {
  case T_FIXNUM:
    node->nd_lit = LONG2FIX(-FIX2LONG(node->nd_lit));
    break;
  case T_BIGNUM:
    node->nd_lit = REF(rb_funcall(node->nd_lit, rb_intern("-@"), 0, 0));
    break;
  case T_FLOAT:
    node->nd_lit = REF(rb_float_new(-NUM2DBL(node->nd_lit)));
    break;
  default:
    break;
  }
  return node;
}

static NODE *
arg_blk_pass(NODE *node1, NODE *node2)
{
  if(node2) {
    node2->nd_head = node1;
    return node2;
  }
  return node1;
}

static NODE*
parser_new_args(rb_parser_state* parser_state, NODE *m, NODE *o, ID r, NODE *p, ID b)
{
  int saved_line = sourceline;
  NODE *node;
  NODE *i1, *i2 = 0;

  node = NEW_ARGS(m ? m->nd_plen : 0, o);
  i1 = m ? m->nd_next : 0;
  node->nd_next = NEW_ARGS_AUX(r, b);

  if(p) {
    i2 = p->nd_next;
    node->nd_next->nd_next = NEW_ARGS_AUX(p->nd_pid, p->nd_plen);
  } else if(i1) {
    node->nd_next->nd_next = NEW_ARGS_AUX(0, 0);
  }
  if(i1 || i2) {
    node->nd_next->nd_next->nd_next = NEW_NODE(NODE_AND, i1, i2, 0);
  }
  sourceline = saved_line;
  return node;
}

static int
parser_arg_var(rb_parser_state* parser_state, ID id)
{
  vtable_add(locals_table->args, id);
  return vtable_size(locals_table->args) - 1;
}

static int
parser_local_var(rb_parser_state* parser_state, ID id)
{
  vtable_add(locals_table->vars, id);
  return vtable_size(locals_table->vars) - 1;
}

static bool
parser_local_id(rb_parser_state* parser_state, ID id)
{
  struct vtable *vars, *args;

  vars = locals_table->vars;
  args = locals_table->args;

  while(vars && vars->prev) {
    vars = vars->prev;
    args = args->prev;
  }

  return (vtable_included(args, id) || vtable_included(vars, id));
}

static void
parser_local_push(rb_parser_state* parser_state, int top)
{
  struct local_vars *local;

  local = ALLOC(struct local_vars);
  local->prev = locals_table;
  local->args = vtable_alloc(0);
  local->vars = vtable_alloc(0);
  locals_table = local;
}

static void
parser_local_pop(rb_parser_state* parser_state)
{
  struct local_vars *local = locals_table->prev;
  vtable_free(locals_table->args);
  vtable_free(locals_table->vars);
  xfree(locals_table);
  locals_table = local;
}

static ID*
vtable_tblcpy(ID *buf, const struct vtable *src)
{
  int cnt = vtable_size(src);

  if(cnt > 0) {
    buf[0] = cnt;
    for(int i = 0; i < cnt; i++) {
      buf[i] = src->tbl[i];
    }
    return buf;
  }
  return 0;
}

static ID*
parser_local_tbl(rb_parser_state* parser_state)
{
  int arg_cnt = vtable_size(locals_table->args);
  int cnt = arg_cnt + vtable_size(locals_table->vars);
  ID *buf;

  if(cnt <= 0) return 0;
  buf = (ID*)pt_allocate(parser_state, (int)(sizeof(ID) * (cnt + 1)));
  vtable_tblcpy(buf + 1, locals_table->args);
  vtable_tblcpy(buf + arg_cnt + 1, locals_table->vars);
  buf[0] = cnt;
  return buf;
}

VALUE
rb_symbol_new(const char* name, long len, rb_encoding* enc)
{
  VALUE str = rb_str_new(name, len);
  return rb_funcall(str, rb_intern("to_sym"), 0);
}

ID
parser_intern3(const char* name, long len, rb_encoding* enc)
{
  const char *m = name;
  unsigned char c;
  long last;

  VALUE sym = rb_symbol_new(name, len, enc);
  ID id = (SYM2ID(sym) << ID_SCOPE_SHIFT) & ~ID_INTERNAL;

  last = len-1;
  switch(*m) {
  case '$':
    id |= ID_GLOBAL;
    break;
  case '@':
    if(m[1] == '@') {
      m++;
      id |= ID_CLASS;
    } else {
      id |= ID_INSTANCE;
    }
    m++;
    break;
  default:
    c = m[0];
    if(len > 1 && c != '_' && parser_enc_isascii(c, enc) && parser_enc_ispunct(c, enc)) {
      /* operators */
      for(int i = 0; i < op_tbl_count; i++) {
        if(*op_tbl[i].name == *m && strcmp(op_tbl[i].name, m) == 0) {
          return id;
        }
      }
    }

    if(m[last] == '=') {
      id |= ID_ATTRSET;
    } else if(parser_enc_isupper(m[0], enc)) {
      id |= ID_CONST;
    } else {
      id |= ID_LOCAL;
    }
    break;
  }

  return id;
}

ID
parser_intern2(const char* name, long len)
{
  return parser_intern3(name, len, parser_usascii_encoding());
}

ID
parser_intern(const char* name)
{
  return parser_intern2(name, strlen(name));
}

ID
parser_intern_str(VALUE str)
{
  return parser_intern2(RSTRING_PTR(str), RSTRING_LEN(str));
}

char*
parser_id2name(ID id)
{
  VALUE str = rb_funcall(ID2SYM(id), rb_intern("to_s"), 0);
  return RSTRING_PTR(str);
}

static int
scan_oct(const char *start, size_t len, size_t *retlen)
{
  register const char *s = start;
  register int retval = 0;

  while(len-- && *s >= '0' && *s <= '7') {
    retval <<= 3;
    retval |= *s++ - '0';
  }
  *retlen = s - start;
  return retval;
}

static int
scan_hex(const char *start, size_t len, size_t *retlen)
{
  static const char hexdigit[] = "0123456789abcdef0123456789ABCDEF";
  register const char *s = start;
  register int retval = 0;
  const char *tmp;

  while(len-- && *s && (tmp = strchr(hexdigit, *s))) {
    retval <<= 4;
    retval |= (int)((tmp - hexdigit) & 15);
    s++;
  }
  *retlen = s - start;
  return retval;
}

static ID
parser_internal_id(rb_parser_state *parser_state)
{
  ID id = (ID)vtable_size(locals_table->args) + (ID)vtable_size(locals_table->vars);
  id += ((tLAST_TOKEN - ID_INTERNAL) >> ID_SCOPE_SHIFT) + 1;
  return ID_INTERNAL | (id << ID_SCOPE_SHIFT);
}

}; // namespace grammar19
}; // namespace melbourne

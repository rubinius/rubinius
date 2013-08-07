/* A Bison parser, made by GNU Bison 3.0.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 13 "grammar19.y" /* yacc.c:339  */


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


#line 448 "grammar19.cpp" /* yacc.c:339  */

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    keyword_class = 258,
    keyword_module = 259,
    keyword_def = 260,
    keyword_undef = 261,
    keyword_begin = 262,
    keyword_rescue = 263,
    keyword_ensure = 264,
    keyword_end = 265,
    keyword_if = 266,
    keyword_unless = 267,
    keyword_then = 268,
    keyword_elsif = 269,
    keyword_else = 270,
    keyword_case = 271,
    keyword_when = 272,
    keyword_while = 273,
    keyword_until = 274,
    keyword_for = 275,
    keyword_break = 276,
    keyword_next = 277,
    keyword_redo = 278,
    keyword_retry = 279,
    keyword_in = 280,
    keyword_do = 281,
    keyword_do_cond = 282,
    keyword_do_block = 283,
    keyword_do_LAMBDA = 284,
    keyword_return = 285,
    keyword_yield = 286,
    keyword_super = 287,
    keyword_self = 288,
    keyword_nil = 289,
    keyword_true = 290,
    keyword_false = 291,
    keyword_and = 292,
    keyword_or = 293,
    keyword_not = 294,
    modifier_if = 295,
    modifier_unless = 296,
    modifier_while = 297,
    modifier_until = 298,
    modifier_rescue = 299,
    keyword_alias = 300,
    keyword_defined = 301,
    keyword_BEGIN = 302,
    keyword_END = 303,
    keyword__LINE__ = 304,
    keyword__FILE__ = 305,
    keyword__ENCODING__ = 306,
    tIDENTIFIER = 307,
    tFID = 308,
    tGVAR = 309,
    tIVAR = 310,
    tCONSTANT = 311,
    tCVAR = 312,
    tLABEL = 313,
    tINTEGER = 314,
    tFLOAT = 315,
    tSTRING_CONTENT = 316,
    tCHAR = 317,
    tNTH_REF = 318,
    tBACK_REF = 319,
    tREGEXP_END = 320,
    tUPLUS = 321,
    tUMINUS = 322,
    tPOW = 323,
    tCMP = 324,
    tEQ = 325,
    tEQQ = 326,
    tNEQ = 327,
    tGEQ = 328,
    tLEQ = 329,
    tANDOP = 330,
    tOROP = 331,
    tMATCH = 332,
    tNMATCH = 333,
    tDOT2 = 334,
    tDOT3 = 335,
    tAREF = 336,
    tASET = 337,
    tLSHFT = 338,
    tRSHFT = 339,
    tCOLON2 = 340,
    tCOLON3 = 341,
    tOP_ASGN = 342,
    tASSOC = 343,
    tLPAREN = 344,
    tLPAREN_ARG = 345,
    tRPAREN = 346,
    tLBRACK = 347,
    tLBRACE = 348,
    tLBRACE_ARG = 349,
    tSTAR = 350,
    tAMPER = 351,
    tLAMBDA = 352,
    tSYMBEG = 353,
    tSTRING_BEG = 354,
    tXSTRING_BEG = 355,
    tREGEXP_BEG = 356,
    tWORDS_BEG = 357,
    tQWORDS_BEG = 358,
    tSTRING_DBEG = 359,
    tSTRING_DVAR = 360,
    tSTRING_END = 361,
    tLAMBEG = 362,
    tLOWEST = 363,
    tUMINUS_NUM = 364,
    tLAST_TOKEN = 365
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 399 "grammar19.y" /* yacc.c:355  */

    VALUE val;
    NODE *node;
    ID id;
    int num;
    const struct vtable* vars;

#line 604 "grammar19.cpp" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (rb_parser_state* parser_state);



/* Copy the second part of user declarations.  */

#line 618 "grammar19.cpp" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   10094

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  138
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  172
/* YYNRULES -- Number of rules.  */
#define YYNRULES  565
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  975

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   365

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     137,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,   136,   123,     2,     2,     2,   121,   116,     2,
     132,   133,   119,   117,   130,   118,   129,   120,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   111,   135,
     113,   109,   112,   110,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   128,     2,   134,   115,     2,   131,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   126,   114,   127,   124,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   122,   125
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   547,   547,   547,   572,   579,   583,   587,   591,   597,
     599,   598,   610,   633,   640,   644,   648,   652,   658,   658,
     662,   666,   673,   678,   682,   687,   692,   700,   708,   713,
     722,   727,   733,   755,   772,   785,   798,   803,   816,   821,
     826,   831,   836,   839,   840,   844,   848,   852,   856,   859,
     867,   868,   871,   872,   876,   883,   882,   897,   902,   909,
     914,   921,   926,   933,   938,   943,   947,   951,   957,   958,
     964,   965,   971,   975,   979,   983,   987,   991,   995,   999,
    1003,  1007,  1013,  1014,  1020,  1024,  1030,  1034,  1040,  1044,
    1048,  1052,  1056,  1060,  1066,  1072,  1079,  1084,  1088,  1092,
    1096,  1100,  1106,  1112,  1119,  1123,  1126,  1130,  1134,  1140,
    1141,  1142,  1143,  1148,  1155,  1156,  1159,  1163,  1166,  1170,
    1170,  1176,  1177,  1178,  1179,  1180,  1181,  1182,  1183,  1184,
    1185,  1186,  1187,  1188,  1189,  1190,  1191,  1192,  1193,  1194,
    1195,  1196,  1197,  1198,  1199,  1200,  1201,  1202,  1203,  1204,
    1207,  1207,  1207,  1208,  1208,  1209,  1209,  1209,  1210,  1210,
    1210,  1210,  1211,  1211,  1211,  1211,  1212,  1212,  1212,  1213,
    1213,  1213,  1213,  1214,  1214,  1214,  1214,  1215,  1215,  1215,
    1215,  1216,  1216,  1216,  1216,  1217,  1217,  1217,  1217,  1218,
    1218,  1221,  1226,  1232,  1254,  1277,  1298,  1311,  1324,  1337,
    1342,  1347,  1352,  1358,  1364,  1368,  1372,  1376,  1380,  1384,
    1388,  1392,  1396,  1400,  1404,  1408,  1412,  1416,  1420,  1424,
    1428,  1432,  1436,  1440,  1444,  1448,  1458,  1462,  1466,  1470,
    1474,  1478,  1482,  1486,  1486,  1491,  1497,  1503,  1511,  1512,
    1516,  1520,  1526,  1532,  1533,  1536,  1537,  1540,  1545,  1549,
    1554,  1559,  1562,  1562,  1574,  1580,  1584,  1588,  1594,  1598,
    1602,  1611,  1622,  1631,  1640,  1646,  1647,  1648,  1649,  1650,
    1651,  1652,  1653,  1654,  1659,  1658,  1675,  1675,  1680,  1684,
    1688,  1692,  1700,  1704,  1708,  1712,  1716,  1720,  1720,  1725,
    1729,  1733,  1739,  1740,  1747,  1751,  1759,  1767,  1767,  1767,
    1774,  1774,  1774,  1781,  1788,  1793,  1795,  1792,  1812,  1811,
    1828,  1833,  1827,  1850,  1849,  1866,  1865,  1883,  1884,  1883,
    1899,  1903,  1907,  1911,  1917,  1925,  1931,  1937,  1943,  1949,
    1955,  1961,  1967,  1973,  1979,  1986,  1992,  1993,  1994,  1997,
    1998,  2001,  2002,  2011,  2012,  2018,  2019,  2022,  2026,  2032,
    2036,  2042,  2046,  2051,  2056,  2060,  2064,  2069,  2074,  2078,
    2084,  2088,  2092,  2096,  2100,  2104,  2108,  2112,  2116,  2120,
    2124,  2128,  2132,  2136,  2140,  2146,  2147,  2153,  2157,  2161,
    2171,  2175,  2181,  2185,  2191,  2196,  2202,  2205,  2202,  2219,
    2223,  2229,  2233,  2240,  2239,  2254,  2265,  2269,  2275,  2280,
    2285,  2290,  2294,  2299,  2304,  2308,  2312,  2324,  2323,  2336,
    2335,  2349,  2357,  2358,  2361,  2373,  2376,  2380,  2384,  2387,
    2391,  2394,  2398,  2401,  2402,  2406,  2409,  2421,  2422,  2423,
    2429,  2435,  2457,  2517,  2521,  2528,  2531,  2537,  2538,  2544,
    2548,  2555,  2558,  2565,  2568,  2575,  2578,  2585,  2588,  2611,
    2613,  2612,  2624,  2630,  2623,  2646,  2647,  2648,  2649,  2652,
    2659,  2660,  2661,  2662,  2665,  2690,  2691,  2692,  2696,  2702,
    2703,  2704,  2705,  2706,  2707,  2708,  2709,  2710,  2711,  2712,
    2713,  2716,  2724,  2730,  2731,  2734,  2739,  2738,  2746,  2753,
    2759,  2767,  2771,  2775,  2779,  2783,  2787,  2791,  2795,  2799,
    2803,  2807,  2811,  2815,  2819,  2824,  2829,  2834,  2839,  2844,
    2851,  2852,  2859,  2864,  2874,  2875,  2883,  2891,  2899,  2903,
    2914,  2918,  2929,  2930,  2933,  2941,  2948,  2949,  2952,  2963,
    2967,  2973,  2979,  2979,  3003,  3004,  3010,  3011,  3017,  3021,
    3027,  3028,  3029,  3032,  3033,  3034,  3035,  3038,  3039,  3040,
    3043,  3044,  3047,  3048,  3051,  3052,  3055,  3058,  3061,  3062,
    3063,  3066,  3067,  3070,  3071,  3074
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "keyword_class", "keyword_module",
  "keyword_def", "keyword_undef", "keyword_begin", "keyword_rescue",
  "keyword_ensure", "keyword_end", "keyword_if", "keyword_unless",
  "keyword_then", "keyword_elsif", "keyword_else", "keyword_case",
  "keyword_when", "keyword_while", "keyword_until", "keyword_for",
  "keyword_break", "keyword_next", "keyword_redo", "keyword_retry",
  "keyword_in", "keyword_do", "keyword_do_cond", "keyword_do_block",
  "keyword_do_LAMBDA", "keyword_return", "keyword_yield", "keyword_super",
  "keyword_self", "keyword_nil", "keyword_true", "keyword_false",
  "keyword_and", "keyword_or", "keyword_not", "modifier_if",
  "modifier_unless", "modifier_while", "modifier_until", "modifier_rescue",
  "keyword_alias", "keyword_defined", "keyword_BEGIN", "keyword_END",
  "keyword__LINE__", "keyword__FILE__", "keyword__ENCODING__",
  "tIDENTIFIER", "tFID", "tGVAR", "tIVAR", "tCONSTANT", "tCVAR", "tLABEL",
  "tINTEGER", "tFLOAT", "tSTRING_CONTENT", "tCHAR", "tNTH_REF",
  "tBACK_REF", "tREGEXP_END", "tUPLUS", "tUMINUS", "tPOW", "tCMP", "tEQ",
  "tEQQ", "tNEQ", "tGEQ", "tLEQ", "tANDOP", "tOROP", "tMATCH", "tNMATCH",
  "tDOT2", "tDOT3", "tAREF", "tASET", "tLSHFT", "tRSHFT", "tCOLON2",
  "tCOLON3", "tOP_ASGN", "tASSOC", "tLPAREN", "tLPAREN_ARG", "tRPAREN",
  "tLBRACK", "tLBRACE", "tLBRACE_ARG", "tSTAR", "tAMPER", "tLAMBDA",
  "tSYMBEG", "tSTRING_BEG", "tXSTRING_BEG", "tREGEXP_BEG", "tWORDS_BEG",
  "tQWORDS_BEG", "tSTRING_DBEG", "tSTRING_DVAR", "tSTRING_END", "tLAMBEG",
  "tLOWEST", "'='", "'?'", "':'", "'>'", "'<'", "'|'", "'^'", "'&'", "'+'",
  "'-'", "'*'", "'/'", "'%'", "tUMINUS_NUM", "'!'", "'~'", "tLAST_TOKEN",
  "'{'", "'}'", "'['", "'.'", "','", "'`'", "'('", "')'", "']'", "';'",
  "' '", "'\\n'", "$accept", "program", "$@1", "top_compstmt", "top_stmts",
  "top_stmt", "$@2", "bodystmt", "compstmt", "stmts", "stmt", "$@3",
  "expr", "expr_value", "command_call", "block_command", "cmd_brace_block",
  "@4", "command", "mlhs", "mlhs_inner", "mlhs_basic", "mlhs_item",
  "mlhs_head", "mlhs_post", "mlhs_node", "lhs", "cname", "cpath", "fname",
  "fsym", "fitem", "undef_list", "$@5", "op", "reswords", "arg", "$@6",
  "arg_value", "aref_args", "paren_args", "opt_paren_args",
  "opt_call_args", "call_args", "command_args", "@7", "block_arg",
  "opt_block_arg", "args", "mrhs", "primary", "@8", "$@9", "$@10", "$@11",
  "$@12", "$@13", "$@14", "$@15", "$@16", "@17", "@18", "@19", "@20",
  "@21", "$@22", "$@23", "primary_value", "k_begin", "k_if", "k_unless",
  "k_while", "k_until", "k_case", "k_for", "k_class", "k_module", "k_def",
  "k_end", "then", "do", "if_tail", "opt_else", "for_var", "f_marg",
  "f_marg_list", "f_margs", "block_param", "opt_block_param",
  "block_param_def", "opt_bv_decl", "bv_decls", "bvar", "lambda", "@24",
  "@25", "f_larglist", "lambda_body", "do_block", "@26", "block_call",
  "method_call", "brace_block", "@27", "@28", "case_body", "cases",
  "opt_rescue", "exc_list", "exc_var", "opt_ensure", "literal", "strings",
  "string", "string1", "xstring", "regexp", "words", "word_list", "word",
  "qwords", "qword_list", "string_contents", "xstring_contents",
  "regexp_contents", "string_content", "@29", "@30", "@31", "string_dvar",
  "symbol", "sym", "dsym", "numeric", "variable", "var_ref", "var_lhs",
  "backref", "superclass", "$@32", "f_arglist", "f_args", "f_bad_arg",
  "f_norm_arg", "f_arg_item", "f_arg", "f_opt", "f_block_opt",
  "f_block_optarg", "f_optarg", "restarg_mark", "f_rest_arg",
  "blkarg_mark", "f_block_arg", "opt_f_block_arg", "singleton", "$@33",
  "assoc_list", "assocs", "assoc", "operation", "operation2", "operation3",
  "dot_or_colon", "opt_terms", "opt_nl", "rparen", "rbracket", "trailer",
  "term", "terms", "none", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,    61,
      63,    58,    62,    60,   124,    94,    38,    43,    45,    42,
      47,    37,   364,    33,   126,   365,   123,   125,    91,    46,
      44,    96,    40,    41,    93,    59,    32,    10
};
# endif

#define YYPACT_NINF -747

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-747)))

#define YYTABLE_NINF -566

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-566)))

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -747,    93,  2394,  -747,  6642,  -747,  -747,  -747,  6181,  -747,
    -747,  -747,  -747,  -747,  -747,  -747,  6860,  6860,  -747,  -747,
    6860,  3098,  2705,  -747,  -747,  -747,  -747,   220,  6052,   -62,
    -747,   -23,  -747,  -747,  -747,  5301,  2836,  -747,  -747,  5425,
    -747,  -747,  -747,  -747,  -747,  -747,  8059,  8059,    56,  4096,
    8168,  7078,  7405,  6432,  -747,  5923,  -747,  -747,  -747,   -19,
      24,   130,  8277,  8059,  -747,   -14,  -747,   954,   399,  -747,
    -747,   142,    69,  -747,    90,  8386,  -747,   140,  2819,   376,
     393,  -747,  8168,  8168,  -747,  -747,  4719,  8491,  8596,  8701,
    5179,    14,    18,  -747,  -747,   154,  -747,  -747,  -747,  -747,
    -747,  -747,  -747,  -747,   463,  -747,   224,   536,    85,  -747,
    -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,
    -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,
    -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,
    -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,
    -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,
    -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,
    -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,
    -747,  -747,  -747,  -747,  -747,  -747,  -747,   195,  -747,  -747,
    -747,  -747,   209,  8059,   288,  4231,  8059,  8059,  8059,  -747,
     263,  2819,   336,  -747,  -747,   300,   413,   178,   353,   239,
     313,  -747,  -747,  -747,  4610,  -747,  6860,  6860,  -747,  -747,
    4828,  -747,  8168,   422,  -747,   317,   334,  4366,  -747,  -747,
    -747,   331,   342,  -747,   432,    85,   415,   614,  6751,  4096,
     360,   -14,   954,   -62,   410,  -747,   399,   368,   136,   147,
    -747,   336,   398,   147,  -747,   -62,   479,   430,  8806,   408,
    -747,   439,   448,   534,  -747,  -747,  -747,  -747,  -747,  -747,
      25,  -747,   407,   426,   308,   443,   552,   445,    36,   485,
     489,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  4937,  8168,
    8168,  8168,  8168,  6751,  8168,  8168,  -747,  -747,  7514,  -747,
    4096,  6537,   452,  7514,  8059,  8059,  8059,  8059,  8059,  8059,
    8059,  8059,  8059,  8059,  8059,  8059,  8059,  8059,  8059,  8059,
    8059,  8059,  8059,  8059,  8059,  8059,  8059,  8059,  8059,  8059,
    1712,  6860,  2214,  3364,   399,    50,    50,  8168,  8168,   -14,
     569,   453,   535,  -747,  -747,   563,   572,    75,    82,   306,
     325,  8168,    84,  -747,    58,   565,  -747,  -747,  -747,    28,
      40,    47,    67,   103,   191,   221,   260,   292,  -747,  -747,
    -747,   310,  9876,  9876,  -747,  -747,  -747,  -747,  8277,  8277,
    -747,   496,  -747,  -747,  -747,   249,  -747,  -747,  8059,  8059,
    6969,  -747,  -747,  9073,  6860,  9146,  8059,  8059,  7187,  -747,
     -62,   467,  -747,  -747,   -62,  -747,  -747,   272,  -747,  -747,
    -747,  -747,  -747,  6181,  -747,  8059,  3743,   475,  9073,  9146,
    8059,   954,   -62,  -747,  -747,  5046,   477,   -62,  -747,  7296,
    -747,  -747,  7405,  -747,  -747,  -747,   317,   574,  -747,  -747,
    -747,   478,  8806,  9219,  6860,  9292,   993,  -747,  -747,  -747,
    -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,   367,
    -747,  -747,   480,  -747,  8059,  8059,  -747,  -747,  -747,  -747,
    -747,  -747,  -747,  -747,  -747,    34,  8059,  -747,   488,   490,
    -747,   -62,  8806,   495,  -747,  -747,  1576,  -747,  -747,   415,
    1512,  1512,  1512,  1512,   765,   765,  1879,  3081,  1512,  1512,
    2950,  2950,   287,   287,  9957,   765,   765,   747,   747,   790,
     387,   387,   415,   415,   415,  2089,  5671,  3229,  5793,  -747,
     342,  -747,   -62,   625,  -747,   629,  -747,  -747,  2967,   617,
     630,  -747,  3499,   634,  3878,    42,    42,   569,  7623,   617,
      83,  9365,  6860,  9438,  -747,   399,  -747,   574,  -747,   -14,
    -747,  -747,  -747,  9511,  6860,  9584,  3364,  8168,  1316,  -747,
    -747,  -747,  -747,  -747,  1208,  1208,    34,    34,  -747,  3207,
    -747,  2819,  -747,  -747,  6181,  9940,  -747,  8059,   336,  -747,
     313,  5549,  2541,   -62,   250,   324,  -747,  -747,  -747,  -747,
    -747,  -747,  8168,  2819,   522,  -747,   342,   342,  2819,   289,
     954,  -747,   147,  8806,   478,   330,   258,   -62,   233,   238,
     543,  -747,  -747,  -747,  -747,   845,  -747,  -747,  -747,  -747,
    1115,    65,  -747,  -747,  -747,  -747,   523,  -747,   542,   632,
     553,   633,  -747,  -747,   722,  -747,  -747,  -747,   415,   415,
    -747,   904,  4501,  -747,  -747,   557,  7732,  -747,   478,  8806,
    8059,   588,  8277,  8277,  -747,   496,   566,   631,  8277,  8277,
    -747,  -747,   496,  -747,  -747,  -747,  7841,   694,  -747,   482,
    -747,   694,  -747,  -747,  -747,  -747,   617,    45,  -747,   101,
     120,   -62,    95,   109,  8168,   -14,  -747,  8168,  3364,   330,
     258,  -747,   -62,   617,   272,  1115,  3364,   -14,  6310,  -747,
    -747,  -747,  -747,  4501,  4366,  8059,    34,  -747,  -747,  -747,
    8059,  8059,   361,  8059,  8059,   272,  -747,  -747,  -747,   338,
    8059,  -747,   845,   425,  -747,   581,   -62,  -747,   578,  4501,
    4366,  -747,  1115,  -747,  -747,  1115,  -747,  -747,   579,  -747,
    -747,  4366,  -747,  -747,  -747,  -747,  -747,   605,   869,   578,
     606,   589,  -747,   598,   599,  -747,  -747,   725,  8059,   609,
     478,  2819,  8059,  -747,  2819,  -747,  2819,  -747,  -747,  8277,
    -747,  2819,  -747,  2819,  -747,   488,  -747,   653,  -747,  3987,
     733,  -747,  8168,   617,  -747,   617,  4501,  4501,  -747,  7950,
    3634,   112,    42,  -747,   -14,   617,  -747,  -747,  -747,   -62,
     617,  -747,  -747,   735,   623,  2819,  4366,  8059,  -747,  -747,
     -62,   932,   636,   923,  -747,   -62,   753,   640,  -747,   639,
     642,  -747,   644,  -747,   650,   644,   643,  8911,  -747,   657,
    -747,  -747,   669,  -747,  1423,  -747,  1423,  -747,   579,  -747,
    -747,   661,  2819,  -747,  2819,  9016,    50,  -747,  -747,  4501,
    -747,  -747,    50,  -747,  -747,   617,   617,  -747,   499,  -747,
    3364,  -747,  -747,  -747,  -747,  1316,  -747,  -747,   666,  -747,
     667,   932,   491,  -747,  -747,  -747,  -747,  1115,  -747,   579,
    -747,   579,  -747,   579,  -747,  -747,  -747,   740,   593,   869,
    -747,   668,   671,   644,  -747,   672,   644,   751,  -747,   596,
     448,   534,  3364,  -747,  3499,  -747,  -747,  -747,  -747,  -747,
    4501,   617,  3364,  -747,   932,   667,   932,   682,   644,   683,
     644,   644,  -747,  9657,  -747,  1423,  -747,   579,  -747,  -747,
     579,  -747,  -747,   574,  9730,  6860,  9803,   630,   482,   617,
    -747,   617,   667,   932,  -747,   579,  -747,  -747,  -747,   687,
     689,   644,   695,   644,   644,    91,   258,   -62,   134,   137,
    -747,  -747,  -747,  -747,   667,   644,  -747,   579,  -747,  -747,
    -747,   151,  -747,   644,  -747
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     0,     1,     0,   332,   333,   334,     0,   325,
     326,   327,   330,   328,   329,   331,   320,   321,   322,   323,
     283,   252,   252,   475,   474,   476,   477,   554,     0,   554,
      10,     0,   479,   478,   480,   469,   542,   471,   470,   472,
     473,   465,   466,   427,   483,   484,     0,     0,     0,     0,
       0,   565,   565,    80,   386,   445,   443,   445,   447,   435,
     441,     0,     0,     0,     3,   552,     6,     9,    42,    43,
      51,    50,     0,    68,     0,    72,    82,     0,    48,   236,
       0,   274,     0,     0,   297,   300,   552,     0,     0,     0,
       0,    52,   292,   265,   266,   426,   428,   267,   268,   269,
     270,   424,   425,   423,   481,   271,     0,   272,   252,     5,
       8,   160,   171,   161,   184,   157,   177,   167,   166,   187,
     188,   182,   165,   164,   159,   185,   189,   190,   169,   158,
     172,   176,   178,   170,   163,   179,   186,   181,   180,   173,
     183,   168,   156,   175,   174,   155,   162,   153,   154,   150,
     151,   152,   109,   111,   110,   145,   146,   142,   124,   125,
     126,   133,   130,   132,   127,   128,   147,   148,   134,   135,
     139,   129,   131,   121,   122,   123,   136,   137,   138,   140,
     141,   143,   144,   149,   114,   116,   118,    23,   112,   113,
     115,   117,     0,     0,     0,     0,     0,     0,     0,   247,
       0,   237,   258,    66,   251,   565,     0,   481,     0,   272,
     565,   536,    67,    65,   554,    64,     0,   565,   404,    63,
     554,   555,     0,     0,    18,   233,     0,     0,   320,   321,
     283,   286,   405,   212,     0,     0,   213,   280,     0,     0,
       0,   552,    15,   554,    70,    14,   276,     0,   558,   558,
     238,     0,     0,   558,   534,   554,     0,     0,     0,    78,
     324,     0,    88,    95,   294,   387,   462,   461,   463,   460,
       0,   459,     0,     0,     0,     0,     0,     0,     0,   467,
     468,    47,   227,   228,   561,   562,     4,   563,   553,     0,
       0,     0,     0,     0,     0,     0,   393,   395,     0,    84,
       0,    76,    73,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   565,     0,     0,    49,     0,     0,     0,     0,   552,
       0,   553,     0,   346,   345,     0,     0,   481,   272,   104,
     105,     0,     0,   107,     0,     0,   481,   272,   313,   180,
     173,   183,   168,   150,   151,   152,   109,   110,   532,   315,
     531,     0,     0,     0,   409,   407,   293,   429,     0,     0,
     398,    57,   291,   119,   539,   280,   259,   254,     0,     0,
     256,   248,   257,     0,   565,     0,     0,     0,   256,   249,
     554,     0,   285,   253,   554,   246,   245,   554,   290,    46,
      20,    22,    21,     0,   287,     0,     0,     0,     0,     0,
       0,    17,   554,   278,    13,   553,    69,   554,   281,   560,
     559,   239,   560,   241,   282,   535,     0,    94,   467,   468,
      86,    81,     0,     0,   565,     0,   505,   449,   452,   450,
     464,   446,   430,   444,   431,   432,   448,   433,   434,     0,
     437,   439,     0,   440,     0,     0,   564,     7,    24,    25,
      26,    27,    28,    44,    45,   565,     0,    31,    40,     0,
      41,   554,     0,    74,    85,    30,   191,   258,    39,   209,
     217,   222,   223,   224,   219,   221,   231,   232,   225,   226,
     202,   203,   229,   230,   554,   218,   220,   214,   215,   216,
     204,   205,   206,   207,   208,   543,   548,   544,   549,   403,
     252,   401,   554,   543,   545,   544,   546,   402,   252,     0,
     565,   337,     0,   336,     0,     0,     0,     0,     0,     0,
     280,     0,   565,     0,   305,   310,   104,   105,   106,     0,
     486,   308,   485,     0,   565,     0,     0,     0,   505,   551,
     550,   317,   543,   544,   252,   252,   565,   565,    32,   193,
      38,   201,    55,    58,     0,   191,   538,     0,   260,   255,
     565,   547,   544,   554,   543,   544,   537,   284,   556,   242,
     289,    19,     0,   234,     0,    29,     0,   565,   200,    71,
      16,   277,   558,     0,    79,    91,    93,   554,   543,   544,
     511,   508,   507,   506,   509,     0,   523,   527,   526,   522,
     505,     0,   390,   510,   512,   514,   565,   520,   565,   525,
     565,     0,   504,   453,     0,   436,   438,   442,   210,   211,
     378,   565,     0,   376,   375,   264,     0,    83,    77,     0,
       0,     0,     0,     0,   400,    61,     0,   406,     0,     0,
     244,   399,    59,   243,   335,   275,   565,   565,   415,   565,
     338,   565,   340,   298,   339,   301,     0,     0,   304,   547,
     279,   554,   543,   544,     0,     0,   488,     0,     0,   104,
     105,   108,   554,     0,   554,   505,     0,     0,     0,   397,
      54,   396,    53,     0,     0,     0,   565,   120,   261,   250,
       0,     0,   406,     0,     0,   554,    11,   240,    87,    89,
       0,   511,     0,   358,   349,   351,   554,   347,   565,     0,
       0,   388,     0,   497,   530,     0,   500,   524,     0,   502,
     528,     0,   455,   456,   457,   451,   458,   511,     0,   565,
       0,   565,   518,   565,   565,   374,   380,     0,     0,   262,
      75,   192,     0,    37,   198,    36,   199,    62,   557,     0,
      34,   196,    35,   197,    60,   416,   417,   565,   418,     0,
     565,   343,     0,     0,   341,     0,     0,     0,   303,     0,
       0,   406,     0,   311,     0,     0,   406,   314,   533,   554,
       0,   490,   318,     0,     0,   194,     0,     0,   288,   516,
     554,     0,   356,     0,   513,   554,     0,     0,   515,   565,
     565,   529,   565,   521,   565,   565,     0,     0,   384,   381,
     382,   385,     0,   377,   365,   367,     0,   370,     0,   372,
     394,   263,   235,    33,   195,     0,     0,   420,   344,     0,
      12,   422,     0,   295,   296,     0,     0,   260,   565,   306,
       0,   487,   309,   489,   316,   505,   410,   408,     0,   348,
     359,     0,   354,   350,   389,   392,   391,     0,   493,     0,
     495,     0,   501,     0,   498,   503,   454,     0,   517,     0,
     379,   565,   565,   565,   519,   565,   565,     0,   419,     0,
      96,   103,     0,   421,     0,   299,   302,   412,   413,   411,
       0,     0,     0,    56,     0,   357,     0,   352,   565,   565,
     565,   565,   280,     0,   383,     0,   362,     0,   364,   371,
       0,   368,   373,   102,     0,   565,     0,   565,   565,     0,
     312,     0,   355,     0,   494,     0,   491,   496,   499,   547,
     279,   565,   565,   565,   565,   547,   101,   554,   543,   544,
     414,   342,   307,   319,   353,   565,   363,     0,   360,   366,
     369,   406,   492,   565,   361
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -747,  -747,  -747,   411,  -747,    33,  -747,  -530,   -33,  -747,
     159,  -747,    23,   -55,    21,  -747,  -328,  -747,   -15,   739,
    -167,    -1,   -66,  -747,  -403,   -26,  1181,  -306,   743,   -52,
    -747,   -20,  -747,  -747,    32,  -747,   748,  -747,   540,  -747,
      46,     6,  -298,    54,    76,  -747,  -239,  -196,   -44,  -283,
      27,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,
    -747,  -747,  -747,  -747,  -747,  -747,  -747,     2,  -747,  -747,
    -747,  -747,  -747,  -747,  -747,  -747,  -747,  -747,   298,  -323,
    -512,  -110,  -610,  -747,  -746,  -716,   113,  -747,  -485,  -747,
    -327,  -747,   -53,  -747,  -747,  -747,  -747,  -747,  -747,  -747,
    -747,  -747,   742,  -747,  -747,  -520,  -747,   -98,  -747,  -747,
    -747,  -747,  -747,  -747,   745,  -747,  -747,  -747,  -747,  -747,
    -747,  -747,  -747,   784,  -747,  -229,  -747,  -747,  -747,  -747,
       7,  -747,    13,  -747,  1031,   752,  1198,  1109,  -747,  -747,
     -21,  -416,  -706,  -549,  -667,  -121,  -679,  -744,    11,   118,
    -747,  -579,  -747,  -434,   531,  -747,  -747,  -747,   -41,  -297,
    1927,  -254,  -747,  -747,   -32,    -4,    88,  -554,  -217,    63,
     -31,    -2
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    64,    65,    66,   226,   529,   530,   241,
     242,   413,    68,   335,    69,    70,   573,   706,    71,    72,
     243,    73,    74,    75,   441,    76,   200,   353,   354,   184,
     185,   186,   187,   574,   526,   189,    78,   415,   202,   247,
     519,   661,   404,   405,   215,   216,   204,   391,   205,   480,
      79,   333,   427,   592,   337,   786,   338,   787,   684,   910,
     688,   685,   860,   556,   558,   698,   865,   234,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,   665,   532,
     673,   783,   784,   346,   724,   725,   726,   749,   642,   643,
     750,   829,   830,   264,   265,   446,   621,   731,   297,   475,
      91,    92,   382,   567,   566,   539,   909,   667,   777,   846,
     850,    93,    94,    95,    96,    97,    98,    99,   276,   459,
     100,   278,   272,   270,   274,   451,   634,   633,   741,   745,
     101,   271,   102,   103,   207,   105,   208,   209,   551,   687,
     696,   697,   623,   624,   625,   626,   627,   752,   753,   628,
     629,   630,   631,   821,   733,   371,   557,   252,   210,   211,
     108,   596,   521,   561,   286,   401,   402,   657,   431,   533,
     341,   245
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     109,   199,   199,   269,    80,   199,    80,   248,   224,   302,
     249,   253,   632,   534,   399,   190,   240,   676,   206,   206,
     488,   191,   206,   222,   675,   225,   693,   259,   336,   712,
     622,   339,   433,   522,   288,   190,   435,   110,   369,   604,
     188,   191,   831,   453,   374,   456,   548,   460,   244,   250,
     254,    80,   206,   719,   340,   261,   823,   780,   531,   549,
     188,   785,   754,   531,   206,   818,   727,   873,   218,   672,
     203,   212,   422,   246,   213,   221,   520,   261,   528,   648,
     260,   703,   704,   281,   206,   206,   447,   188,   206,   345,
     355,   355,   894,     3,   729,   870,   583,   462,   219,   372,
     -96,   586,   260,   227,   -99,   334,   334,  -103,  -102,   334,
     640,   374,   237,  -475,   260,   260,   260,   275,   564,   565,
     -98,   284,   188,   285,   632,  -474,   -99,   791,   287,   448,
     449,   450,  -476,   481,  -100,   586,   546,   -97,   796,   520,
     547,   528,   463,   373,   375,  -101,   607,   -98,   641,   287,
    -100,   579,  -477,   820,   380,   915,   824,  -475,   795,   579,
     277,    67,   240,    67,   -97,   597,   800,   818,   873,  -474,
     296,   550,   730,   727,   812,   789,  -476,   284,   298,   285,
     284,   894,   285,   831,   381,   284,   632,   285,  -479,   279,
     280,   597,   440,   284,   417,   285,  -477,    80,   823,   199,
     942,   199,   199,   392,   728,   -88,   240,   755,   392,   424,
     425,   375,   -95,   -94,   818,   406,   206,   217,   206,   206,
     299,   806,   206,  -543,   206,   -90,   -99,   964,   -99,    80,
     636,   -91,  -479,  -543,   468,   469,   470,   471,   244,   -92,
      80,    80,   -89,   407,   681,   409,   760,   691,   907,   303,
     -93,   436,  -544,    56,   479,   892,   692,   895,   -90,   479,
     261,   632,   727,   -92,   727,  -482,   429,   240,   400,   -98,
     403,   -98,  -100,   430,  -100,   483,  -478,   432,   218,   799,
     859,   380,   535,   536,   430,   260,   -97,   -96,   -97,   597,
      80,   206,   206,   206,   206,    80,   206,   206,   919,   244,
     206,   597,    80,   261,   287,   206,  -480,   537,   408,   294,
     295,   378,   334,   334,   334,   334,   199,   473,   474,   477,
    -478,   467,   727,   917,   485,   383,   397,   767,   260,   406,
     911,   426,  -540,   206,   774,    80,   420,   713,   908,   206,
     206,   220,   -90,  -279,   385,  -469,   952,   -92,  -103,   580,
    -480,  -541,   220,   206,   790,   304,  -547,   221,  -102,   -98,
     334,   334,   518,   -90,  -406,   727,   -90,   727,   -92,   447,
     -90,   -92,   388,   455,   545,   -92,   440,  -472,   527,   199,
     206,   206,   941,   776,   709,   717,  -279,  -279,   602,  -469,
    -544,  -469,   406,   591,   727,   559,   206,   421,   -69,   568,
     570,   815,   287,   971,   325,   326,   327,   328,   329,   221,
    -472,   714,   448,   449,   109,  -547,   440,   552,    80,   -83,
     190,  -472,   832,  -406,   389,   518,   191,    80,   447,   199,
     390,   632,  -540,  -100,  -469,  -469,   294,   295,  -540,   560,
     396,   527,   406,   398,   261,   188,   206,    67,   807,   414,
     518,  -541,   472,  -472,  -472,   304,  -547,  -541,  -547,  -547,
     416,  -324,  -543,   214,  -406,   527,  -406,  -406,   447,   260,
     -97,   448,   449,   644,   217,   518,   410,   721,   330,   611,
     612,   613,   614,   304,   261,   411,   412,   447,   587,   438,
     439,   527,   589,   423,   677,   590,   782,   779,   393,   669,
     651,   671,   428,   635,  -324,  -324,   327,   328,   329,   260,
     599,   448,   449,   452,   779,   601,   538,   418,   656,   -68,
     751,   331,   332,   902,   443,   434,   663,   199,   668,   904,
     448,   449,   454,  -481,    80,   437,    80,   718,   442,   199,
     406,   394,   395,   721,   206,   611,   612,   613,   614,   457,
    -482,   461,   406,   464,   707,   811,   206,   465,    80,   206,
     394,   419,   663,   663,   644,   644,   654,   444,   445,   647,
     699,   701,   -96,   518,   660,    67,  -481,  -481,   392,   656,
     694,   190,   484,   440,   600,   518,   538,   191,   466,   527,
     572,   540,   251,   -88,   206,   663,   655,   544,   674,   674,
     588,   527,   595,   656,   662,   261,   188,   -83,   603,   757,
     660,   660,   686,   447,   822,   715,   637,   825,  -258,  -272,
     646,   916,   479,   379,   734,   649,   734,   664,   734,   792,
     260,   721,   794,   611,   612,   613,   614,   957,   666,   756,
     700,   702,   654,   660,    80,  -103,   802,   670,   541,   716,
     553,   261,   720,   732,   206,   206,   448,   449,   458,  -280,
     206,   206,  -272,  -272,   778,   781,   -95,   781,   615,   781,
     803,   804,   735,   763,   765,   617,   260,   656,   923,   770,
     772,   934,   597,   738,   737,   740,   206,  -259,   656,   206,
      80,   542,   543,   554,   555,   618,   816,   817,    80,   762,
     768,   420,  -280,  -280,   644,    80,    80,   334,   826,   779,
     334,   813,   658,   748,   827,   893,   659,   896,   769,   834,
     833,   554,   555,  -102,   935,   936,   756,   852,   836,   838,
     188,    80,    80,   384,   -98,   840,   386,   387,  -100,  -260,
     -97,   845,   849,    80,   -94,   866,   848,   756,   793,   734,
     867,   734,   734,   855,   856,   -90,   918,   858,   920,   -92,
     801,   -89,   921,   875,   201,   201,   871,   876,   201,   877,
     886,   206,   879,   868,   881,   847,   742,   743,   851,   744,
     883,    80,   798,   890,   206,    44,    45,   889,    80,    80,
     843,  -261,    80,   913,   233,   236,   922,   914,   925,   201,
     201,   927,   930,   808,   951,   334,   953,   933,    80,   954,
     282,   283,   943,   945,   814,   304,   903,   734,   734,  -543,
     734,  -544,   734,   734,   965,   967,   343,   594,   961,   888,
     317,   318,   358,   304,   376,   810,   924,   678,   478,   960,
     377,   273,   370,   487,   912,   891,   973,   899,   317,   318,
     819,    80,     0,     0,   260,   674,   781,   861,   304,     0,
       0,     0,    80,   324,   325,   326,   327,   328,   329,   937,
       0,   938,   260,   317,   318,     0,     0,   939,     0,   322,
     323,   324,   325,   326,   327,   328,   329,   863,     0,   734,
     734,   734,     0,   734,   734,     0,     0,   721,   869,   611,
     612,   613,   614,   874,    80,     0,    80,   325,   326,   327,
     328,   329,    80,     0,    80,     0,   734,   734,   734,   734,
     199,   828,     0,   611,   612,   613,   614,     0,     0,   576,
     578,     0,     0,   406,   722,   668,   781,   206,   251,     0,
     723,   201,     0,     0,   201,   201,   282,     0,     0,   734,
     734,   734,   734,   656,     0,   518,   747,     0,   611,   612,
     613,   614,   201,   734,   201,   201,   518,     0,     0,   578,
       0,   734,   251,     0,   788,   721,     0,   611,   612,   613,
     614,     0,   527,     0,   721,     0,   611,   612,   613,   614,
       0,   797,     0,   615,   289,   290,   291,   292,   293,   616,
     617,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   722,     0,     0,     0,   645,     0,   872,     0,
     618,   722,     0,   619,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   104,     0,   104,     0,     0,     0,   748,
       0,     0,     0,     0,     0,   610,   201,   611,   612,   613,
     614,   486,   489,   490,   491,   492,   493,   494,   495,   496,
     497,   498,   499,   500,   501,   502,   503,   504,   505,   506,
     507,   508,   509,   510,   511,   512,   513,   514,   487,   201,
     104,   853,   615,   854,   262,     0,     0,     0,   616,   617,
       0,     0,     0,   862,     0,     0,     0,     0,   864,     0,
       0,     0,     0,     0,     0,     0,   262,     0,     0,   618,
       0,   107,   619,   107,     0,     0,     0,   708,   347,   356,
     356,   356,     0,     0,     0,   620,   569,   571,     0,     0,
       0,     0,     0,     0,     0,     0,   575,   201,   201,     0,
       0,     0,   201,     0,   569,   571,   201,     0,     0,     0,
       0,     0,     0,   905,   906,     0,     0,     0,   107,   736,
       0,   739,   263,   593,     0,     0,     0,   610,   598,   611,
     612,   613,   614,     0,     0,     0,     0,   201,     0,     0,
     201,     0,     0,    77,   263,    77,   759,     0,     0,     0,
       0,     0,   201,     0,     0,     0,   348,   357,   357,     0,
     106,     0,   106,     0,   615,     0,   775,     0,  -565,   940,
     616,   617,   638,   639,     0,     0,  -565,  -565,  -565,     0,
       0,  -565,  -565,  -565,   201,  -565,   104,     0,     0,     0,
      77,   618,     0,     0,   619,  -565,     0,   962,     0,   963,
       0,     0,     0,     0,     0,  -565,  -565,   106,  -565,  -565,
    -565,  -565,  -565,     0,     0,     0,     0,     0,   104,     0,
     809,     0,     0,     0,     0,     0,     0,     0,   344,   104,
     104,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   835,     0,   837,   839,   201,     0,     0,   262,
     201,     0,     0,  -565,     0,     0,     0,     0,   841,     0,
       0,     0,   201,     0,   107,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   104,
       0,     0,     0,     0,   104,   201,     0,     0,     0,   857,
       0,   104,   262,     0,     0,  -565,   107,  -565,     0,     0,
     217,  -565,     0,  -565,     0,  -565,     0,   107,   107,     0,
     878,   880,     0,   882,     0,   884,   885,     0,     0,     0,
       0,     0,     0,     0,   104,     0,     0,   263,   610,     0,
     611,   612,   613,   614,     0,     0,    77,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   106,   201,     0,     0,   107,   761,     0,
     764,   766,   107,     0,     0,   615,   771,   773,    77,   107,
     263,   616,   617,     0,   201,     0,     0,     0,     0,    77,
      77,     0,   926,   928,   929,   106,   931,   932,     0,     0,
       0,     0,   618,     0,     0,   619,   106,   106,     0,     0,
       0,     0,   107,     0,     0,     0,     0,   104,   695,   944,
     946,   947,   948,   805,     0,     0,   104,     0,   764,   766,
       0,   771,   773,     0,     0,     0,     0,     0,   201,    77,
       0,     0,     0,   262,    77,   747,     0,   611,   612,   613,
     614,    77,   966,   968,   969,   970,   106,     0,     0,     0,
       0,   106,     0,     0,     0,     0,   972,     0,   106,     0,
       0,     0,     0,     0,   974,     0,   201,     0,     0,     0,
     842,     0,   615,   262,    77,     0,     0,   844,   616,   617,
       0,     0,     0,     0,     0,   107,     0,     0,     0,     0,
       0,   106,     0,     0,   107,     0,     0,   201,     0,   618,
       0,     0,   619,     0,     0,     0,     0,     0,     0,     0,
       0,   263,     0,     0,     0,   844,     0,     0,     0,     0,
       0,     0,     0,   104,     0,   104,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     304,  -566,  -566,  -566,  -566,   309,   310,   104,     0,  -566,
    -566,   263,     0,     0,     0,   317,   318,    77,     0,     0,
       0,     0,     0,     0,     0,     0,    77,     0,     0,     0,
       0,     0,     0,     0,   106,     0,     0,     0,     0,     0,
     650,     0,     0,   106,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   262,     0,     0,     0,     0,     0,
       0,   107,     0,   107,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,     0,     0,   317,
     318,     0,     0,     0,     0,   107,     0,     0,     0,     0,
       0,     0,     0,   104,     0,     0,     0,     0,     0,     0,
     262,     0,     0,   201,     0,     0,   319,     0,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,     0,     0,
       0,     0,     0,     0,     0,     0,  -237,     0,     0,     0,
       0,     0,   263,    77,     0,    77,     0,     0,     0,   104,
       0,     0,     0,     0,     0,     0,     0,   104,     0,     0,
     106,     0,   106,     0,   104,   104,     0,    77,     0,     0,
       0,     0,     0,   746,     0,     0,     0,     0,     0,     0,
       0,   107,     0,     0,   106,     0,     0,     0,   263,     0,
     104,   104,     0,     0,   515,   516,     0,     0,   517,     0,
       0,     0,   104,     0,     0,     0,     0,     0,   155,   156,
     157,   158,   159,   160,   161,   162,   163,     0,     0,   164,
     165,     0,     0,   166,   167,   168,   169,   107,     0,     0,
       0,     0,     0,     0,     0,   107,     0,   170,     0,     0,
     104,     0,   107,   107,     0,     0,     0,   104,   104,     0,
       0,   104,     0,    77,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,     0,   181,   182,   104,   107,   107,
     106,     0,     0,   183,   217,     0,     0,     0,     0,     0,
     107,     0,     0,     0,     0,     0,     0,     0,   356,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    77,
       0,     0,     0,     0,     0,     0,   900,    77,     0,     0,
     104,     0,     0,     0,    77,    77,   106,     0,   107,     0,
       0,   104,     0,     0,   106,   107,   107,     0,     0,   107,
       0,   106,   106,     0,     0,     0,     0,     0,     0,     0,
      77,    77,     0,     0,     0,   107,     0,     0,     0,     0,
       0,     0,    77,     0,     0,     0,     0,   106,   106,     0,
       0,     0,     0,   104,     0,   104,   357,     0,     0,   106,
       0,   104,     0,   104,     0,     0,     0,   304,   305,   306,
     307,   308,   309,   310,   901,     0,   313,   314,   107,     0,
      77,     0,   317,   318,     0,     0,     0,    77,    77,   107,
       0,    77,     0,   235,   235,     0,     0,   106,   235,   235,
     235,     0,     0,     0,   106,   106,     0,    77,   106,     0,
     235,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,     0,   235,     0,   106,     0,     0,     0,     0,     0,
       0,   107,     0,   107,   235,   235,   235,     0,     0,   107,
       0,   107,     0,     0,     0,     0,   898,     0,     0,     0,
      77,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    77,     0,     0,     0,     0,     0,   106,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   106,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    77,     0,    77,     0,     0,     0,  -547,
       0,    77,     0,    77,     0,     0,     0,  -547,  -547,  -547,
     106,     0,   106,  -547,  -547,     0,  -547,     0,   106,     0,
     106,     0,     0,     0,     0,  -547,     0,     0,     0,     0,
     235,     0,     0,   235,   235,   235,  -547,  -547,     0,  -547,
    -547,  -547,  -547,  -547,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -547,  -547,  -547,
    -547,  -547,  -547,  -547,  -547,  -547,  -547,  -547,  -547,  -547,
       0,     0,  -547,  -547,  -547,     0,   652,     0,     0,     0,
       0,     0,     0,     0,     0,   235,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   -99,  -547,
       0,  -547,  -547,  -547,  -547,  -547,  -547,  -547,  -547,  -547,
    -547,     0,     0,     0,     0,  -547,  -547,  -547,  -547,   -91,
       0,     0,  -547,     0,  -547,     0,  -547,     0,   235,     0,
       0,   235,   235,   235,   235,   235,   235,   235,   235,   235,
     235,   235,   235,   235,   235,   235,   235,   235,   235,   235,
     235,   235,   235,   235,   235,   235,   235,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   523,   524,     0,     0,
     525,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     155,   156,   157,   158,   159,   160,   161,   162,   163,     0,
       0,   164,   165,     0,     0,   166,   167,   168,   169,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   170,
       0,     0,     0,     0,     0,   235,   235,   235,     0,     0,
       0,     0,     0,   235,   235,   235,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,     0,   181,   182,     0,
       0,     0,   235,     0,     0,   183,   217,   235,     0,     0,
       0,     0,     0,     0,     0,     0,   235,     0,     0,   235,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   235,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   235,   235,     0,  -565,     4,     0,     5,     6,     7,
       8,     9,     0,   235,     0,    10,    11,     0,     0,   235,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
      46,    47,     0,     0,     0,   235,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,   235,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    61,    62,    63,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -565,
     235,  -565,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -279,     0,     0,     0,     0,     0,     0,     0,  -279,
    -279,  -279,     0,     0,  -279,  -279,  -279,     0,  -279,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -279,  -279,
    -279,     0,     0,   235,     0,     0,   235,   235,  -279,  -279,
       0,  -279,  -279,  -279,  -279,  -279,     0,     0,     0,     0,
       0,     0,     0,   235,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -279,
    -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,
    -279,  -279,     0,     0,  -279,  -279,  -279,     0,   711,  -279,
       0,     0,   235,     0,     0,  -279,     0,   235,   235,     0,
     235,   235,     0,     0,     0,     0,     0,   235,  -279,     0,
    -101,  -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,
    -279,  -279,  -279,     0,     0,     0,     0,     0,  -279,  -279,
    -279,  -279,     0,     0,  -279,  -279,  -279,     0,  -279,     0,
       0,     0,     0,     0,     0,   235,     0,     0,     0,   235,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -405,     0,     0,     0,     0,
       0,     0,     0,  -405,  -405,  -405,   235,     0,  -405,  -405,
    -405,     0,  -405,     0,     0,     0,     0,     0,     0,     0,
       0,  -405,  -405,  -405,   235,     0,     0,     0,     0,     0,
       0,     0,  -405,  -405,     0,  -405,  -405,  -405,  -405,  -405,
       0,     0,     0,     0,   235,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   235,  -405,  -405,  -405,  -405,  -405,  -405,  -405,
    -405,  -405,  -405,  -405,  -405,  -405,     0,     0,  -405,  -405,
    -405,     0,     0,  -405,     0,     0,     0,     0,     0,  -405,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -405,     0,  -405,  -405,  -405,
    -405,  -405,  -405,  -405,  -405,  -405,  -405,     0,     0,     0,
       0,  -405,  -405,  -405,  -405,  -405,  -273,   217,  -405,  -405,
    -405,     0,  -405,     0,  -273,  -273,  -273,     0,     0,  -273,
    -273,  -273,     0,  -273,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -273,  -273,  -273,     0,     0,     0,     0,
       0,     0,     0,  -273,  -273,     0,  -273,  -273,  -273,  -273,
    -273,     0,     0,     0,     0,     0,     0,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
       0,     0,   317,   318,  -273,  -273,  -273,  -273,  -273,  -273,
    -273,  -273,  -273,  -273,  -273,  -273,  -273,     0,     0,  -273,
    -273,  -273,     0,     0,  -273,     0,     0,     0,     0,   319,
    -273,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,     0,     0,  -273,     0,     0,  -273,  -273,  -273,  -273,
    -273,  -273,  -273,  -273,  -273,  -273,  -273,  -273,     0,     0,
       0,     0,     0,  -273,  -273,  -273,  -273,  -565,     0,  -273,
    -273,  -273,     0,  -273,     0,  -565,  -565,  -565,     0,     0,
    -565,  -565,  -565,     0,  -565,     0,     0,     0,     0,     0,
       0,     0,     0,  -565,  -565,  -565,     0,     0,     0,     0,
       0,     0,     0,     0,  -565,  -565,     0,  -565,  -565,  -565,
    -565,  -565,     0,     0,     0,     0,     0,     0,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,  -566,
    -566,     0,     0,   317,   318,  -565,  -565,  -565,  -565,  -565,
    -565,  -565,  -565,  -565,  -565,  -565,  -565,  -565,     0,     0,
    -565,  -565,  -565,     0,     0,  -565,     0,     0,     0,     0,
       0,  -565,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,     0,     0,     0,     0,     0,  -565,     0,  -565,
    -565,  -565,  -565,  -565,  -565,  -565,  -565,  -565,  -565,     0,
       0,     0,     0,  -565,  -565,  -565,  -565,  -565,  -286,   217,
    -565,  -565,  -565,     0,  -565,     0,  -286,  -286,  -286,     0,
       0,  -286,  -286,  -286,     0,  -286,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -286,  -286,     0,     0,     0,
       0,     0,     0,     0,     0,  -286,  -286,     0,  -286,  -286,
    -286,  -286,  -286,     0,     0,     0,     0,     0,     0,   304,
     305,   306,   307,   308,   309,   310,   311,     0,   313,   314,
       0,     0,     0,     0,   317,   318,  -286,  -286,  -286,  -286,
    -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,     0,
       0,  -286,  -286,  -286,     0,     0,  -286,     0,     0,     0,
       0,     0,  -286,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,     0,     0,     0,     0,     0,  -286,     0,
    -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,
       0,     0,     0,     0,     0,  -286,  -286,  -286,  -286,  -279,
     214,  -286,  -286,  -286,     0,  -286,     0,  -279,  -279,  -279,
       0,     0,     0,  -279,  -279,     0,  -279,     0,     0,     0,
       0,   705,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -279,  -279,     0,  -279,
    -279,  -279,  -279,  -279,     0,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,     0,     0,
     317,   318,     0,     0,     0,     0,     0,  -279,  -279,  -279,
    -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,
       0,     0,  -279,  -279,  -279,     0,   653,   319,     0,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -101,  -279,
       0,  -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,  -279,
    -279,     0,     0,     0,     0,     0,  -279,  -279,  -279,   -93,
       0,     0,  -279,     0,  -279,   238,  -279,     5,     6,     7,
       8,     9,  -565,  -565,  -565,    10,    11,     0,     0,  -565,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,    28,
      29,     0,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    61,    62,    63,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -565,
     238,  -565,     5,     6,     7,     8,     9,     0,     0,  -565,
      10,    11,     0,  -565,  -565,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,    28,    29,     0,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
       0,    43,    44,    45,     0,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    48,     0,     0,    49,    50,
       0,    51,    52,     0,    53,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    61,    62,    63,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -565,   238,  -565,     5,     6,     7,
       8,     9,     0,     0,  -565,    10,    11,     0,     0,  -565,
      12,  -565,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,    28,
      29,     0,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     4,     0,     5,     6,     7,     8,
       9,     0,     0,     0,    10,    11,    61,    62,    63,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,  -565,
       0,  -565,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,     0,    41,    42,     0,    43,    44,    45,     0,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,    49,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    61,    62,    63,     0,     0,
    -565,     0,     0,     0,     0,     0,     0,     0,  -565,   238,
    -565,     5,     6,     7,     8,     9,     0,     0,  -565,    10,
      11,     0,     0,  -565,    12,     0,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,    28,    29,     0,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
      43,    44,    45,     0,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,     0,    49,    50,     0,
      51,    52,     0,    53,     0,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,     0,     0,   238,     0,
       5,     6,     7,     8,     9,     0,  -565,  -565,    10,    11,
      61,    62,    63,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,  -565,     0,  -565,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,    27,     0,     0,     0,
       0,     0,    28,    29,     0,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,     0,    41,    42,     0,    43,
      44,    45,     0,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,     0,     0,    49,    50,     0,    51,
      52,     0,    53,     0,    54,    55,    56,    57,    58,    59,
      60,     0,     0,     0,     0,     0,     0,   238,     0,     5,
       6,     7,     8,     9,     0,     0,     0,    10,    11,    61,
      62,    63,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,  -565,     0,  -565,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,    28,    29,     0,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,     0,    41,    42,     0,    43,    44,
      45,     0,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,     0,     0,   239,    50,     0,    51,    52,
       0,    53,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    61,    62,
      63,     0,     0,     0,     0,     0,     0,     0,     0,  -565,
       0,  -565,   238,  -565,     5,     6,     7,     8,     9,     0,
       0,     0,    10,    11,     0,     0,     0,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,    29,     0,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
      41,    42,     0,    43,    44,    45,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    48,     0,     0,
      49,    50,     0,    51,    52,     0,    53,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    61,    62,    63,     0,     0,     0,     0,
       0,     0,     0,     0,  -565,     0,  -565,   238,  -565,     5,
       6,     7,     8,     9,     0,     0,     0,    10,    11,     0,
       0,     0,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,    28,    29,     0,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,     0,    41,    42,     0,    43,    44,
      45,     0,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,     0,     0,    49,    50,     0,    51,    52,
       0,    53,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    61,    62,
      63,     0,     0,  -565,     0,     0,     0,     0,     0,     0,
       0,  -565,   238,  -565,     5,     6,     7,     8,     9,     0,
       0,  -565,    10,    11,     0,     0,     0,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,    29,     0,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
      41,    42,     0,    43,    44,    45,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    48,     0,     0,
      49,    50,     0,    51,    52,     0,    53,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    61,    62,    63,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,  -565,     0,  -565,     0,
      20,    21,    22,    23,    24,    25,    26,     0,     0,   192,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,   193,    41,
      42,     0,    43,    44,    45,     0,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   194,     0,     0,   195,
      50,     0,    51,    52,     0,   196,   197,    54,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    61,   198,    63,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,   221,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
       0,    43,    44,    45,     0,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   194,     0,     0,   195,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    61,    62,    63,    12,     0,    13,    14,    15,    16,
      17,    18,    19,     0,   284,     0,   285,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
      43,    44,    45,     0,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   194,     0,     0,   195,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     8,     9,     0,     0,     0,    10,    11,
      61,    62,    63,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,   221,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,    27,     0,     0,     0,
       0,     0,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,     0,    41,    42,     0,    43,
      44,    45,     0,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,     0,     0,    49,    50,     0,    51,
      52,     0,    53,     0,    54,    55,    56,    57,    58,    59,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     8,     9,     0,     0,     0,    10,    11,    61,
      62,    63,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,   466,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,    28,    29,     0,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,     0,    41,    42,     0,    43,    44,
      45,     0,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,     0,     0,    49,    50,     0,    51,    52,
       0,    53,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    61,    62,
      63,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   466,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,     0,     0,     0,   135,
     136,   137,   359,   360,   361,   362,   142,   143,   144,     0,
       0,     0,     0,     0,   145,   146,   147,   148,   363,   364,
     365,   366,   153,    37,    38,   367,    40,     0,     0,     0,
       0,     0,     0,     0,     0,   155,   156,   157,   158,   159,
     160,   161,   162,   163,     0,     0,   164,   165,     0,     0,
     166,   167,   168,   169,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   170,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,     0,   181,   182,  -540,  -540,  -540,     0,  -540,     0,
     183,   368,  -540,  -540,     0,     0,     0,  -540,     0,  -540,
    -540,  -540,  -540,  -540,  -540,  -540,     0,  -540,     0,     0,
       0,  -540,  -540,  -540,  -540,  -540,  -540,  -540,     0,     0,
    -540,     0,     0,     0,     0,     0,     0,  -540,     0,     0,
    -540,  -540,  -540,  -540,  -540,  -540,  -540,  -540,  -540,  -540,
    -540,  -540,     0,  -540,  -540,  -540,     0,  -540,  -540,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -540,     0,     0,
    -540,  -540,     0,  -540,  -540,     0,  -540,  -540,  -540,  -540,
    -540,  -540,  -540,  -540,  -540,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -540,  -540,  -540,     0,  -540,  -541,  -541,
    -541,     0,  -541,  -540,     0,     0,  -541,  -541,     0,     0,
       0,  -541,     0,  -541,  -541,  -541,  -541,  -541,  -541,  -541,
       0,  -541,     0,     0,     0,  -541,  -541,  -541,  -541,  -541,
    -541,  -541,     0,     0,  -541,     0,     0,     0,     0,     0,
       0,  -541,     0,     0,  -541,  -541,  -541,  -541,  -541,  -541,
    -541,  -541,  -541,  -541,  -541,  -541,     0,  -541,  -541,  -541,
       0,  -541,  -541,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -541,     0,     0,  -541,  -541,     0,  -541,  -541,     0,
    -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -541,  -541,  -541,
       0,  -541,  -543,  -543,  -543,     0,  -543,  -541,     0,     0,
    -543,  -543,     0,     0,     0,  -543,     0,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,     0,     0,     0,     0,     0,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,     0,     0,  -543,     0,
       0,     0,     0,     0,     0,  -543,     0,     0,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
       0,  -543,  -543,  -543,     0,  -543,  -543,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -543,   710,     0,  -543,  -543,
       0,  -543,  -543,     0,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,     0,     0,     0,     0,     0,   -99,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -543,  -543,  -543,  -545,  -545,  -545,     0,  -545,     0,
       0,  -543,  -545,  -545,     0,     0,     0,  -545,     0,  -545,
    -545,  -545,  -545,  -545,  -545,  -545,     0,     0,     0,     0,
       0,  -545,  -545,  -545,  -545,  -545,  -545,  -545,     0,     0,
    -545,     0,     0,     0,     0,     0,     0,  -545,     0,     0,
    -545,  -545,  -545,  -545,  -545,  -545,  -545,  -545,  -545,  -545,
    -545,  -545,     0,  -545,  -545,  -545,     0,  -545,  -545,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -545,     0,     0,
    -545,  -545,     0,  -545,  -545,     0,  -545,  -545,  -545,  -545,
    -545,  -545,  -545,  -545,  -545,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -545,  -545,  -545,  -546,  -546,  -546,     0,
    -546,     0,     0,  -545,  -546,  -546,     0,     0,     0,  -546,
       0,  -546,  -546,  -546,  -546,  -546,  -546,  -546,     0,     0,
       0,     0,     0,  -546,  -546,  -546,  -546,  -546,  -546,  -546,
       0,     0,  -546,     0,     0,     0,     0,     0,     0,  -546,
       0,     0,  -546,  -546,  -546,  -546,  -546,  -546,  -546,  -546,
    -546,  -546,  -546,  -546,     0,  -546,  -546,  -546,     0,  -546,
    -546,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -546,
       0,     0,  -546,  -546,     0,  -546,  -546,     0,  -546,  -546,
    -546,  -546,  -546,  -546,  -546,  -546,  -546,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -546,  -546,  -546,     0,     0,
       0,     0,     0,     0,     0,  -546,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
       0,     0,     0,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,     0,     0,     0,     0,     0,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   266,   267,   154,
     268,     0,     0,     0,     0,     0,     0,     0,     0,   155,
     156,   157,   158,   159,   160,   161,   162,   163,     0,     0,
     164,   165,     0,     0,   166,   167,   168,   169,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   170,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,     0,   181,   182,     0,     0,
       0,     0,     0,     0,   183,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,     0,
       0,     0,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,     0,     0,     0,     0,     0,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   223,     0,   154,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   155,   156,
     157,   158,   159,   160,   161,   162,   163,     0,     0,   164,
     165,     0,     0,   166,   167,   168,   169,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   170,     0,     0,
      55,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,     0,   181,   182,     0,     0,     0,
       0,     0,     0,   183,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,     0,     0,
       0,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,     0,     0,     0,     0,     0,   145,   146,   147,   148,
     149,   150,   151,   152,   153,     0,     0,   154,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   155,   156,   157,
     158,   159,   160,   161,   162,   163,     0,     0,   164,   165,
       0,     0,   166,   167,   168,   169,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   170,     0,     0,    55,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,     0,   181,   182,     0,     0,     0,     0,
       0,     0,   183,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,     0,     0,     0,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
       0,     0,     0,     0,     0,   145,   146,   147,   148,   149,
     150,   151,   152,   153,     0,     0,   154,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   155,   156,   157,   158,
     159,   160,   161,   162,   163,     0,     0,   164,   165,     0,
       0,   166,   167,   168,   169,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   170,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,     0,   181,   182,     5,     6,     7,     0,     9,
       0,   183,     0,    10,    11,     0,     0,     0,    12,     0,
      13,    14,    15,   228,   229,    18,    19,     0,     0,     0,
       0,     0,   230,   231,   232,    23,    24,    25,    26,     0,
       0,   192,     0,     0,     0,     0,     0,     0,   255,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
       0,    41,    42,     0,    43,    44,    45,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   256,     0,
       0,   195,    50,     0,    51,    52,     0,     0,     0,    54,
      55,    56,    57,    58,    59,    60,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
       0,     0,     0,    12,   257,    13,    14,    15,   228,   229,
      18,    19,   258,     0,     0,     0,     0,   230,   231,   232,
      23,    24,    25,    26,     0,     0,   192,     0,     0,     0,
       0,     0,     0,   255,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,     0,    41,    42,     0,    43,
      44,    45,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   256,     0,     0,   195,    50,     0,    51,
      52,     0,     0,     0,    54,    55,    56,    57,    58,    59,
      60,     0,     0,     0,     0,     5,     6,     7,     8,     9,
       0,     0,     0,    10,    11,     0,     0,     0,    12,   257,
      13,    14,    15,    16,    17,    18,    19,   482,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
       0,    41,    42,     0,    43,    44,    45,     0,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,     0,
       0,    49,    50,     0,    51,    52,     0,    53,     0,    54,
      55,    56,    57,    58,    59,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     8,     9,     0,
       0,     0,    10,    11,    61,    62,    63,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,    29,     0,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
      41,    42,     0,    43,    44,    45,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    48,     0,     0,
      49,    50,     0,    51,    52,     0,    53,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    61,    62,    63,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,     0,
      20,    21,    22,    23,    24,    25,    26,     0,     0,   192,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,   193,    41,
      42,     0,    43,    44,    45,     0,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   194,     0,     0,   195,
      50,     0,    51,    52,     0,   196,   197,    54,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    61,   198,    63,    12,     0,    13,    14,    15,
     228,   229,    18,    19,     0,     0,     0,     0,     0,   230,
     231,   232,    23,    24,    25,    26,     0,     0,   192,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,   193,    41,    42,
       0,    43,    44,    45,     0,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   194,     0,     0,   195,    50,
       0,    51,    52,     0,   577,   197,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    61,   198,    63,    12,     0,    13,    14,    15,   228,
     229,    18,    19,     0,     0,     0,     0,     0,   230,   231,
     232,    23,    24,    25,    26,     0,     0,   192,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,   193,    41,    42,     0,
      43,    44,    45,     0,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   194,     0,     0,   195,    50,     0,
      51,    52,     0,   196,     0,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      61,   198,    63,    12,     0,    13,    14,    15,   228,   229,
      18,    19,     0,     0,     0,     0,     0,   230,   231,   232,
      23,    24,    25,    26,     0,     0,   192,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,   193,    41,    42,     0,    43,
      44,    45,     0,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   194,     0,     0,   195,    50,     0,    51,
      52,     0,     0,   197,    54,    55,    56,    57,    58,    59,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    61,
     198,    63,    12,     0,    13,    14,    15,   228,   229,    18,
      19,     0,     0,     0,     0,     0,   230,   231,   232,    23,
      24,    25,    26,     0,     0,   192,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,   193,    41,    42,     0,    43,    44,
      45,     0,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   194,     0,     0,   195,    50,     0,    51,    52,
       0,   577,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    61,   198,
      63,    12,     0,    13,    14,    15,   228,   229,    18,    19,
       0,     0,     0,     0,     0,   230,   231,   232,    23,    24,
      25,    26,     0,     0,   192,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,   193,    41,    42,     0,    43,    44,    45,
       0,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   194,     0,     0,   195,    50,     0,    51,    52,     0,
       0,     0,    54,    55,    56,    57,    58,    59,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    61,   198,    63,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,   192,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     194,     0,     0,   195,    50,     0,    51,    52,     0,   476,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    61,   198,    63,    12,
       0,    13,    14,    15,   228,   229,    18,    19,     0,     0,
       0,     0,     0,   230,   231,   232,    23,    24,    25,    26,
       0,     0,   192,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,     0,    41,    42,     0,    43,    44,    45,     0,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   194,
       0,     0,   195,    50,     0,    51,    52,     0,   196,     0,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    61,   198,    63,    12,     0,
      13,    14,    15,   228,   229,    18,    19,     0,     0,     0,
       0,     0,   230,   231,   232,    23,    24,    25,    26,     0,
       0,   192,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
       0,    41,    42,     0,    43,    44,    45,     0,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   194,     0,
       0,   195,    50,     0,    51,    52,     0,   758,     0,    54,
      55,    56,    57,    58,    59,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    61,   198,    63,    12,     0,    13,
      14,    15,   228,   229,    18,    19,     0,     0,     0,     0,
       0,   230,   231,   232,    23,    24,    25,    26,     0,     0,
     192,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
      41,    42,     0,    43,    44,    45,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   194,     0,     0,
     195,    50,     0,    51,    52,     0,   476,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    61,   198,    63,    12,     0,    13,    14,
      15,   228,   229,    18,    19,     0,     0,     0,     0,     0,
     230,   231,   232,    23,    24,    25,    26,     0,     0,   192,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,     0,    41,
      42,     0,    43,    44,    45,     0,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   194,     0,     0,   195,
      50,     0,    51,    52,     0,   577,     0,    54,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    61,   198,    63,    12,     0,    13,    14,    15,
     228,   229,    18,    19,     0,     0,     0,     0,     0,   230,
     231,   232,    23,    24,    25,    26,     0,     0,   192,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
       0,    43,    44,    45,     0,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   194,     0,     0,   195,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    61,   198,    63,    12,     0,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
      43,    44,    45,     0,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   194,     0,     0,   195,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      61,    62,    63,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,   192,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,     0,    41,    42,     0,    43,
      44,    45,     0,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   194,     0,     0,   195,    50,     0,    51,
      52,     0,     0,     0,    54,    55,    56,    57,    58,    59,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    61,
     198,    63,    12,     0,    13,    14,    15,   228,   229,    18,
      19,     0,     0,     0,     0,     0,   230,   231,   232,    23,
      24,    25,    26,     0,     0,   192,     0,     0,     0,     0,
       0,     0,   255,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,     0,    41,    42,     0,    43,    44,
      45,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   256,     0,     0,   300,    50,     0,    51,    52,
       0,   301,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,     0,     0,     0,    12,   257,    13,
      14,    15,   228,   229,    18,    19,     0,     0,     0,     0,
       0,   230,   231,   232,    23,    24,    25,    26,     0,     0,
     192,     0,     0,     0,     0,     0,     0,   255,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
      41,    42,     0,    43,    44,    45,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   342,     0,     0,
      49,    50,     0,    51,    52,     0,    53,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,     0,
       0,     0,    12,   257,    13,    14,    15,   228,   229,    18,
      19,     0,     0,     0,     0,     0,   230,   231,   232,    23,
      24,    25,    26,     0,     0,   192,     0,     0,     0,     0,
       0,     0,   255,     0,     0,    32,    33,    34,   349,    36,
      37,    38,   350,    40,     0,    41,    42,     0,    43,    44,
      45,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   351,
       0,     0,   352,     0,     0,   195,    50,     0,    51,    52,
       0,     0,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,     0,     0,     0,    12,   257,    13,
      14,    15,   228,   229,    18,    19,     0,     0,     0,     0,
       0,   230,   231,   232,    23,    24,    25,    26,     0,     0,
     192,     0,     0,     0,     0,     0,     0,   255,     0,     0,
      32,    33,    34,   349,    36,    37,    38,   350,    40,     0,
      41,    42,     0,    43,    44,    45,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   352,     0,     0,
     195,    50,     0,    51,    52,     0,     0,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,     0,
       0,     0,    12,   257,    13,    14,    15,   228,   229,    18,
      19,     0,     0,     0,     0,     0,   230,   231,   232,    23,
      24,    25,    26,     0,     0,   192,     0,     0,     0,     0,
       0,     0,   255,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,     0,    41,    42,     0,    43,    44,
      45,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   256,     0,     0,   300,    50,     0,    51,    52,
       0,     0,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,     0,     0,     0,    12,   257,    13,
      14,    15,   228,   229,    18,    19,     0,     0,     0,     0,
       0,   230,   231,   232,    23,    24,    25,    26,     0,     0,
     192,     0,     0,     0,     0,     0,     0,   255,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
      41,    42,     0,    43,    44,    45,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   887,     0,     0,
     195,    50,     0,    51,    52,     0,     0,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,     0,
       0,     0,    12,   257,    13,    14,    15,   228,   229,    18,
      19,     0,     0,     0,     0,     0,   230,   231,   232,    23,
      24,    25,    26,     0,     0,   192,     0,     0,     0,     0,
       0,     0,   255,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,     0,    41,    42,     0,    43,    44,
      45,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   897,     0,     0,   195,    50,     0,    51,    52,
       0,     0,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     0,   581,   516,     0,     0,   582,
       0,     0,     0,     0,     0,     0,     0,     0,   257,   155,
     156,   157,   158,   159,   160,   161,   162,   163,     0,     0,
     164,   165,     0,     0,   166,   167,   168,   169,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   170,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,     0,   181,   182,   584,   524,
       0,     0,   585,     0,   183,   217,     0,     0,     0,     0,
       0,     0,   155,   156,   157,   158,   159,   160,   161,   162,
     163,     0,     0,   164,   165,     0,     0,   166,   167,   168,
     169,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   170,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,     0,   181,
     182,   605,   516,     0,     0,   606,     0,   183,   217,     0,
       0,     0,     0,     0,     0,   155,   156,   157,   158,   159,
     160,   161,   162,   163,     0,     0,   164,   165,     0,     0,
     166,   167,   168,   169,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   170,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,     0,   181,   182,   608,   524,     0,     0,   609,     0,
     183,   217,     0,     0,     0,     0,     0,     0,   155,   156,
     157,   158,   159,   160,   161,   162,   163,     0,     0,   164,
     165,     0,     0,   166,   167,   168,   169,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   170,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,     0,   181,   182,   679,   516,     0,
       0,   680,     0,   183,   217,     0,     0,     0,     0,     0,
       0,   155,   156,   157,   158,   159,   160,   161,   162,   163,
       0,     0,   164,   165,     0,     0,   166,   167,   168,   169,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     170,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,     0,   181,   182,
     682,   524,     0,     0,   683,     0,   183,   217,     0,     0,
       0,     0,     0,     0,   155,   156,   157,   158,   159,   160,
     161,   162,   163,     0,     0,   164,   165,     0,     0,   166,
     167,   168,   169,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   170,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
       0,   181,   182,   689,   516,     0,     0,   690,     0,   183,
     217,     0,     0,     0,     0,     0,     0,   155,   156,   157,
     158,   159,   160,   161,   162,   163,     0,     0,   164,   165,
       0,     0,   166,   167,   168,   169,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   170,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,     0,   181,   182,   562,   524,     0,     0,
     563,     0,   183,   217,     0,     0,     0,     0,     0,     0,
     155,   156,   157,   158,   159,   160,   161,   162,   163,     0,
       0,   164,   165,     0,     0,   166,   167,   168,   169,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   170,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,     0,   181,   182,   949,
     516,     0,     0,   950,     0,   183,   217,     0,     0,     0,
       0,     0,     0,   155,   156,   157,   158,   159,   160,   161,
     162,   163,     0,     0,   164,   165,     0,     0,   166,   167,
     168,   169,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   170,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,     0,
     181,   182,   955,   516,     0,     0,   956,     0,   183,   217,
       0,     0,     0,     0,     0,     0,   155,   156,   157,   158,
     159,   160,   161,   162,   163,     0,     0,   164,   165,     0,
       0,   166,   167,   168,   169,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   170,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,     0,   181,   182,   958,   524,     0,     0,   959,
       0,   183,   217,     0,     0,     0,     0,     0,     0,   155,
     156,   157,   158,   159,   160,   161,   162,   163,     0,     0,
     164,   165,     0,     0,   166,   167,   168,   169,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   170,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,     0,   181,   182,   562,   524,
       0,     0,   563,     0,   183,   217,     0,     0,     0,     0,
       0,     0,   155,   156,   157,   158,   159,   160,   161,   162,
     163,     0,     0,   164,   165,     0,     0,   166,   167,   168,
     169,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   170,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   650,     0,     0,     0,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,     0,   181,
     182,     0,     0,     0,     0,     0,     0,   183,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,     0,     0,   317,   318,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,     0,     0,
     317,   318,     0,     0,     0,     0,     0,     0,     0,     0,
     319,     0,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,     0,     0,     0,     0,     0,   319,     0,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   221
};

static const yytype_int16 yycheck[] =
{
       2,    16,    17,    55,     2,    20,     4,    51,    28,    75,
      51,    52,   446,   336,   210,     8,    49,   537,    16,    17,
     303,     8,    20,    27,   536,    29,   556,    53,    83,   583,
     446,    86,   249,   331,    65,    28,   253,     4,    90,   442,
       8,    28,   748,   272,    26,   274,   352,   276,    49,    51,
      52,    49,    50,   607,    86,    53,   735,   667,    13,     1,
      28,   671,   641,    13,    62,   732,   615,   813,    22,    27,
      16,    17,   239,    50,    20,   137,   330,    75,   332,   482,
      53,   566,   567,    62,    82,    83,    61,    55,    86,    87,
      88,    89,   836,     0,    29,   811,   394,    61,    22,    85,
      25,   398,    75,   126,    13,    82,    83,    25,    25,    86,
      76,    26,    56,    85,    87,    88,    89,   136,   372,   373,
      25,   135,    90,   137,   558,    85,    25,   681,    65,   104,
     105,   106,    85,   300,    25,   432,    52,    25,   692,   393,
      56,   395,   106,   129,   126,    25,   444,    13,   114,    86,
      13,   390,    85,   732,   108,   871,   735,   129,   688,   398,
     136,     2,   195,     4,    13,   419,   696,   834,   914,   129,
      28,   113,   107,   722,   723,   130,   129,   135,   109,   137,
     135,   925,   137,   889,   108,   135,   620,   137,    85,    59,
      60,   445,   258,   135,   227,   137,   129,   195,   877,   214,
     916,   216,   217,   205,   620,   130,   239,   641,   210,   241,
     241,   126,   130,   130,   881,   217,   214,   132,   216,   217,
     130,   706,   220,   132,   222,   130,   135,   943,   137,   227,
     459,   130,   129,   132,   289,   290,   291,   292,   239,   130,
     238,   239,   130,   220,   542,   222,   649,   553,   858,   109,
     130,   255,   132,    99,   298,   834,   554,   836,    25,   303,
     258,   695,   811,    25,   813,    87,   130,   300,   214,   135,
     216,   137,   135,   137,   137,   301,    85,   130,   232,   695,
     792,   235,   337,   338,   137,   258,   135,   109,   137,   543,
     288,   289,   290,   291,   292,   293,   294,   295,   877,   300,
     298,   555,   300,   301,   241,   303,    85,   339,   220,    37,
      38,    87,   289,   290,   291,   292,   331,   294,   295,   298,
     129,   288,   871,   872,   303,   130,    87,   655,   301,   331,
     860,   243,    26,   331,   662,   333,    87,    87,   858,   337,
     338,   132,   109,    85,    56,    85,   925,   109,   109,   390,
     129,    26,   132,   351,   677,    68,    26,   137,   109,   109,
     337,   338,   330,   130,    26,   914,   133,   916,   130,    61,
     137,   133,   109,    65,   351,   137,   442,    85,   332,   394,
     378,   379,   912,   666,   580,   602,   128,   129,   429,   129,
     132,    85,   394,   413,   943,    85,   394,   238,   109,   378,
     379,   728,   339,   957,   117,   118,   119,   120,   121,   137,
      85,    87,   104,   105,   416,    85,   482,   354,   416,   130,
     413,   129,   749,    85,    88,   393,   413,   425,    61,   444,
     130,   865,   126,   109,   128,   129,    37,    38,   132,   129,
      87,   395,   444,   130,   442,   413,   444,   288,    87,   132,
     418,   126,   293,   128,   129,    68,   126,   132,   128,   129,
     126,    85,   132,   132,   126,   419,   128,   129,    61,   442,
     109,   104,   105,   475,   132,   443,    54,    52,    85,    54,
      55,    56,    57,    68,   482,    63,    64,    61,   400,    59,
      60,   445,   404,   133,   538,   407,    14,    15,    85,   532,
     504,   534,   134,   136,   128,   129,   119,   120,   121,   482,
     422,   104,   105,   106,    15,   427,    17,    85,   522,   109,
     641,   128,   129,   846,    85,   127,   528,   542,   530,   852,
     104,   105,   106,    85,   532,    56,   534,   603,   130,   554,
     542,   128,   129,    52,   542,    54,    55,    56,    57,   106,
      87,   106,   554,    68,   574,   130,   554,    68,   556,   557,
     128,   129,   564,   565,   566,   567,   520,   128,   129,   481,
     564,   565,   109,   541,   528,   416,   128,   129,   580,   583,
     557,   574,   130,   649,   425,   553,    17,   574,   135,   543,
      94,    56,    52,   130,   592,   597,   520,    25,   535,   536,
     133,   555,   127,   607,   528,   603,   574,   130,   130,   642,
     564,   565,   549,    61,   735,   592,   136,   738,   130,    85,
     130,   130,   666,    87,   626,   130,   628,    10,   630,   684,
     603,    52,   687,    54,    55,    56,    57,   935,     8,   641,
     564,   565,   596,   597,   642,   109,   698,    13,    85,   127,
      85,   649,   109,   130,   652,   653,   104,   105,   106,    85,
     658,   659,   128,   129,   666,   667,   130,   669,    89,   671,
     703,   704,   130,   652,   653,    96,   649,   681,    85,   658,
     659,    85,   936,   130,    52,    52,   684,   130,   692,   687,
     688,   128,   129,   128,   129,   116,   729,   730,   696,   111,
     134,    87,   128,   129,   706,   703,   704,   684,   741,    15,
     687,   130,    87,   135,   109,   836,    87,   838,    87,   130,
     114,   128,   129,   109,   128,   129,   728,   782,   130,   130,
     698,   729,   730,   193,   109,    10,   196,   197,   109,   130,
     109,    88,     9,   741,   130,    10,   779,   749,   685,   751,
     127,   753,   754,   786,   787,   130,   877,   790,   879,   130,
     697,   130,   883,    10,    16,    17,   130,   127,    20,   130,
     127,   769,   130,   806,   130,   777,    54,    55,   780,    57,
     130,   779,   694,   114,   782,    63,    64,   130,   786,   787,
     769,   130,   790,   127,    46,    47,    56,   130,   130,    51,
      52,   130,   130,   715,   925,   782,   927,    56,   806,   930,
      62,    63,   130,   130,   726,    68,   849,   819,   820,   132,
     822,   132,   824,   825,   945,   130,    87,   416,   938,   827,
      83,    84,    89,    68,    92,   722,   889,   539,   298,   937,
      95,    57,    90,   303,   865,   834,   967,   845,    83,    84,
     732,   849,    -1,    -1,   827,   792,   858,   794,    68,    -1,
      -1,    -1,   860,   116,   117,   118,   119,   120,   121,   902,
      -1,   904,   845,    83,    84,    -1,    -1,   910,    -1,   114,
     115,   116,   117,   118,   119,   120,   121,   799,    -1,   891,
     892,   893,    -1,   895,   896,    -1,    -1,    52,   810,    54,
      55,    56,    57,   815,   902,    -1,   904,   117,   118,   119,
     120,   121,   910,    -1,   912,    -1,   918,   919,   920,   921,
     935,    52,    -1,    54,    55,    56,    57,    -1,    -1,   389,
     390,    -1,    -1,   935,    89,   937,   938,   935,   398,    -1,
      95,   193,    -1,    -1,   196,   197,   198,    -1,    -1,   951,
     952,   953,   954,   957,    -1,   923,    52,    -1,    54,    55,
      56,    57,   214,   965,   216,   217,   934,    -1,    -1,   429,
      -1,   973,   432,    -1,   676,    52,    -1,    54,    55,    56,
      57,    -1,   936,    -1,    52,    -1,    54,    55,    56,    57,
      -1,   693,    -1,    89,    40,    41,    42,    43,    44,    95,
      96,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    -1,    -1,    -1,   476,    -1,    95,    -1,
     116,    89,    -1,   119,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     2,    -1,     4,    -1,    -1,    -1,   135,
      -1,    -1,    -1,    -1,    -1,    52,   298,    54,    55,    56,
      57,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   538,   331,
      49,   783,    89,   785,    53,    -1,    -1,    -1,    95,    96,
      -1,    -1,    -1,   795,    -1,    -1,    -1,    -1,   800,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    75,    -1,    -1,   116,
      -1,     2,   119,     4,    -1,    -1,    -1,   577,    87,    88,
      89,    90,    -1,    -1,    -1,   132,   378,   379,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   388,   389,   390,    -1,
      -1,    -1,   394,    -1,   396,   397,   398,    -1,    -1,    -1,
      -1,    -1,    -1,   855,   856,    -1,    -1,    -1,    49,   628,
      -1,   630,    53,   415,    -1,    -1,    -1,    52,   420,    54,
      55,    56,    57,    -1,    -1,    -1,    -1,   429,    -1,    -1,
     432,    -1,    -1,     2,    75,     4,   646,    -1,    -1,    -1,
      -1,    -1,   444,    -1,    -1,    -1,    87,    88,    89,    -1,
       2,    -1,     4,    -1,    89,    -1,   666,    -1,     0,   911,
      95,    96,   464,   465,    -1,    -1,     8,     9,    10,    -1,
      -1,    13,    14,    15,   476,    17,   195,    -1,    -1,    -1,
      49,   116,    -1,    -1,   119,    27,    -1,   939,    -1,   941,
      -1,    -1,    -1,    -1,    -1,    37,    38,    49,    40,    41,
      42,    43,    44,    -1,    -1,    -1,    -1,    -1,   227,    -1,
     720,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,   238,
     239,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   751,    -1,   753,   754,   538,    -1,    -1,   258,
     542,    -1,    -1,    85,    -1,    -1,    -1,    -1,   758,    -1,
      -1,    -1,   554,    -1,   195,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   288,
      -1,    -1,    -1,    -1,   293,   577,    -1,    -1,    -1,   789,
      -1,   300,   301,    -1,    -1,   127,   227,   129,    -1,    -1,
     132,   133,    -1,   135,    -1,   137,    -1,   238,   239,    -1,
     819,   820,    -1,   822,    -1,   824,   825,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   333,    -1,    -1,   258,    52,    -1,
      54,    55,    56,    57,    -1,    -1,   195,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   195,   646,    -1,    -1,   288,   650,    -1,
     652,   653,   293,    -1,    -1,    89,   658,   659,   227,   300,
     301,    95,    96,    -1,   666,    -1,    -1,    -1,    -1,   238,
     239,    -1,   891,   892,   893,   227,   895,   896,    -1,    -1,
      -1,    -1,   116,    -1,    -1,   119,   238,   239,    -1,    -1,
      -1,    -1,   333,    -1,    -1,    -1,    -1,   416,   132,   918,
     919,   920,   921,   705,    -1,    -1,   425,    -1,   710,   711,
      -1,   713,   714,    -1,    -1,    -1,    -1,    -1,   720,   288,
      -1,    -1,    -1,   442,   293,    52,    -1,    54,    55,    56,
      57,   300,   951,   952,   953,   954,   288,    -1,    -1,    -1,
      -1,   293,    -1,    -1,    -1,    -1,   965,    -1,   300,    -1,
      -1,    -1,    -1,    -1,   973,    -1,   758,    -1,    -1,    -1,
     762,    -1,    89,   482,   333,    -1,    -1,   769,    95,    96,
      -1,    -1,    -1,    -1,    -1,   416,    -1,    -1,    -1,    -1,
      -1,   333,    -1,    -1,   425,    -1,    -1,   789,    -1,   116,
      -1,    -1,   119,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   442,    -1,    -1,    -1,   807,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   532,    -1,   534,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    69,    70,    71,    72,    73,    74,   556,    -1,    77,
      78,   482,    -1,    -1,    -1,    83,    84,   416,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   425,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   416,    -1,    -1,    -1,    -1,    -1,
      44,    -1,    -1,   425,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   603,    -1,    -1,    -1,    -1,    -1,
      -1,   532,    -1,   534,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    -1,    -1,    83,
      84,    -1,    -1,    -1,    -1,   556,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   642,    -1,    -1,    -1,    -1,    -1,    -1,
     649,    -1,    -1,   935,    -1,    -1,   110,    -1,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   130,    -1,    -1,    -1,
      -1,    -1,   603,   532,    -1,   534,    -1,    -1,    -1,   688,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   696,    -1,    -1,
     532,    -1,   534,    -1,   703,   704,    -1,   556,    -1,    -1,
      -1,    -1,    -1,   634,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   642,    -1,    -1,   556,    -1,    -1,    -1,   649,    -1,
     729,   730,    -1,    -1,    52,    53,    -1,    -1,    56,    -1,
      -1,    -1,   741,    -1,    -1,    -1,    -1,    -1,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    -1,    -1,    77,
      78,    -1,    -1,    81,    82,    83,    84,   688,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   696,    -1,    95,    -1,    -1,
     779,    -1,   703,   704,    -1,    -1,    -1,   786,   787,    -1,
      -1,   790,    -1,   642,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,    -1,   123,   124,   806,   729,   730,
     642,    -1,    -1,   131,   132,    -1,    -1,    -1,    -1,    -1,
     741,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   827,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   688,
      -1,    -1,    -1,    -1,    -1,    -1,   845,   696,    -1,    -1,
     849,    -1,    -1,    -1,   703,   704,   688,    -1,   779,    -1,
      -1,   860,    -1,    -1,   696,   786,   787,    -1,    -1,   790,
      -1,   703,   704,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     729,   730,    -1,    -1,    -1,   806,    -1,    -1,    -1,    -1,
      -1,    -1,   741,    -1,    -1,    -1,    -1,   729,   730,    -1,
      -1,    -1,    -1,   902,    -1,   904,   827,    -1,    -1,   741,
      -1,   910,    -1,   912,    -1,    -1,    -1,    68,    69,    70,
      71,    72,    73,    74,   845,    -1,    77,    78,   849,    -1,
     779,    -1,    83,    84,    -1,    -1,    -1,   786,   787,   860,
      -1,   790,    -1,    46,    47,    -1,    -1,   779,    51,    52,
      53,    -1,    -1,    -1,   786,   787,    -1,   806,   790,    -1,
      63,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,    75,    -1,   806,    -1,    -1,    -1,    -1,    -1,
      -1,   902,    -1,   904,    87,    88,    89,    -1,    -1,   910,
      -1,   912,    -1,    -1,    -1,    -1,   845,    -1,    -1,    -1,
     849,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   860,    -1,    -1,    -1,    -1,    -1,   849,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   860,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   902,    -1,   904,    -1,    -1,    -1,     0,
      -1,   910,    -1,   912,    -1,    -1,    -1,     8,     9,    10,
     902,    -1,   904,    14,    15,    -1,    17,    -1,   910,    -1,
     912,    -1,    -1,    -1,    -1,    26,    -1,    -1,    -1,    -1,
     193,    -1,    -1,   196,   197,   198,    37,    38,    -1,    40,
      41,    42,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      -1,    -1,    83,    84,    85,    -1,    87,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   258,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,    -1,    -1,    -1,   126,   127,   128,   129,   130,
      -1,    -1,   133,    -1,   135,    -1,   137,    -1,   301,    -1,
      -1,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    52,    53,    -1,    -1,
      56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    -1,
      -1,    77,    78,    -1,    -1,    81,    82,    83,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      -1,    -1,    -1,    -1,    -1,   388,   389,   390,    -1,    -1,
      -1,    -1,    -1,   396,   397,   398,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,   123,   124,    -1,
      -1,    -1,   415,    -1,    -1,   131,   132,   420,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   429,    -1,    -1,   432,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   442,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   464,   465,    -1,     0,     1,    -1,     3,     4,     5,
       6,     7,    -1,   476,    -1,    11,    12,    -1,    -1,   482,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    64,    -1,
      66,    67,    -1,    -1,    -1,   538,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      -1,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,    -1,    -1,   577,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   122,   123,   124,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   135,
     603,   137,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     0,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     8,
       9,    10,    -1,    -1,    13,    14,    15,    -1,    17,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    28,
      29,    -1,    -1,   646,    -1,    -1,   649,   650,    37,    38,
      -1,    40,    41,    42,    43,    44,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   666,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    -1,    -1,    83,    84,    85,    -1,    87,    88,
      -1,    -1,   705,    -1,    -1,    94,    -1,   710,   711,    -1,
     713,   714,    -1,    -1,    -1,    -1,    -1,   720,   107,    -1,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,    -1,    -1,    -1,    -1,    -1,   127,   128,
     129,   130,    -1,    -1,   133,   134,   135,    -1,   137,    -1,
      -1,    -1,    -1,    -1,    -1,   758,    -1,    -1,    -1,   762,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     0,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     8,     9,    10,   789,    -1,    13,    14,
      15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    26,    27,    28,   807,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    37,    38,    -1,    40,    41,    42,    43,    44,
      -1,    -1,    -1,    -1,   827,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   845,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    -1,    -1,    83,    84,
      85,    -1,    -1,    88,    -1,    -1,    -1,    -1,    -1,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   110,    -1,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,    -1,    -1,    -1,
      -1,   126,   127,   128,   129,   130,     0,   132,   133,   134,
     135,    -1,   137,    -1,     8,     9,    10,    -1,    -1,    13,
      14,    15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    28,    29,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    37,    38,    -1,    40,    41,    42,    43,
      44,    -1,    -1,    -1,    -1,    -1,    -1,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      -1,    -1,    83,    84,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    -1,    -1,    83,
      84,    85,    -1,    -1,    88,    -1,    -1,    -1,    -1,   110,
      94,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,    -1,   107,    -1,    -1,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    -1,    -1,
      -1,    -1,    -1,   127,   128,   129,   130,     0,    -1,   133,
     134,   135,    -1,   137,    -1,     8,     9,    10,    -1,    -1,
      13,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    26,    27,    28,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    37,    38,    -1,    40,    41,    42,
      43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    -1,    -1,    83,    84,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    85,    -1,    -1,    88,    -1,    -1,    -1,    -1,
      -1,    94,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,    -1,    -1,    -1,    -1,    -1,   110,    -1,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,    -1,
      -1,    -1,    -1,   126,   127,   128,   129,   130,     0,   132,
     133,   134,   135,    -1,   137,    -1,     8,     9,    10,    -1,
      -1,    13,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    28,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    37,    38,    -1,    40,    41,
      42,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    68,
      69,    70,    71,    72,    73,    74,    75,    -1,    77,    78,
      -1,    -1,    -1,    -1,    83,    84,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    -1,
      -1,    83,    84,    85,    -1,    -1,    88,    -1,    -1,    -1,
      -1,    -1,    94,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,    -1,    -1,    -1,    -1,    -1,   110,    -1,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
      -1,    -1,    -1,    -1,    -1,   127,   128,   129,   130,     0,
     132,   133,   134,   135,    -1,   137,    -1,     8,     9,    10,
      -1,    -1,    -1,    14,    15,    -1,    17,    -1,    -1,    -1,
      -1,    44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    37,    38,    -1,    40,
      41,    42,    43,    44,    -1,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    -1,    -1,    -1,    -1,    -1,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      -1,    -1,    83,    84,    85,    -1,    87,   110,    -1,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,    -1,    -1,    -1,    -1,   127,   128,   129,   130,
      -1,    -1,   133,    -1,   135,     1,   137,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    -1,    -1,    15,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,
      46,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    64,    -1,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      -1,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   122,   123,   124,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   135,
       1,   137,     3,     4,     5,     6,     7,    -1,    -1,    10,
      11,    12,    -1,    14,    15,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    45,    46,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      -1,    62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    95,    -1,    97,    98,    99,   100,
     101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   122,   123,   124,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   135,     1,   137,     3,     4,     5,
       6,     7,    -1,    -1,    10,    11,    12,    -1,    -1,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,
      46,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    64,    -1,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      -1,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,
       7,    -1,    -1,    -1,    11,    12,   122,   123,   124,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,   135,
      -1,   137,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,    63,    64,    -1,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,
      97,    98,    99,   100,   101,   102,   103,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   122,   123,   124,    -1,    -1,
     127,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   135,     1,
     137,     3,     4,     5,     6,     7,    -1,    -1,    10,    11,
      12,    -1,    -1,    15,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    45,    46,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    -1,
      62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    -1,    97,    98,    99,   100,   101,
     102,   103,    -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,
       3,     4,     5,     6,     7,    -1,     9,    10,    11,    12,
     122,   123,   124,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,   135,    -1,   137,    -1,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    45,    46,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,    -1,    62,
      63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    -1,    97,    98,    99,   100,   101,   102,
     103,    -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,     3,
       4,     5,     6,     7,    -1,    -1,    -1,    11,    12,   122,
     123,   124,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,   135,    -1,   137,    -1,    30,    31,    32,    33,
      34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      -1,    45,    46,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
      64,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    -1,    97,    98,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,   123,
     124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   133,
      -1,   135,     1,   137,     3,     4,     5,     6,     7,    -1,
      -1,    -1,    11,    12,    -1,    -1,    -1,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    45,    46,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,    63,    64,    -1,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    -1,    97,    98,
      99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   122,   123,   124,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   133,    -1,   135,     1,   137,     3,
       4,     5,     6,     7,    -1,    -1,    -1,    11,    12,    -1,
      -1,    -1,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      -1,    45,    46,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
      64,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    -1,    97,    98,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,   123,
     124,    -1,    -1,   127,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   135,     1,   137,     3,     4,     5,     6,     7,    -1,
      -1,    10,    11,    12,    -1,    -1,    -1,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    45,    46,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,    63,    64,    -1,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    -1,    97,    98,
      99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   122,   123,   124,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,   135,    -1,   137,    -1,
      30,    31,    32,    33,    34,    35,    36,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    -1,    62,    63,    64,    -1,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    96,    97,    98,    99,
     100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   122,   123,   124,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,   137,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      -1,    62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    -1,    -1,    97,    98,    99,   100,
     101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   122,   123,   124,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,   135,    -1,   137,    -1,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    -1,
      62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    -1,    -1,    97,    98,    99,   100,   101,
     102,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,    -1,    -1,    -1,    11,    12,
     122,   123,   124,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,   137,    -1,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,    -1,    62,
      63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    -1,    97,    98,    99,   100,   101,   102,
     103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,     7,    -1,    -1,    -1,    11,    12,   122,
     123,   124,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,   135,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      -1,    45,    46,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
      64,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    -1,    97,    98,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,   123,
     124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   135,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    -1,
      -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,
      81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,   124,     3,     4,     5,    -1,     7,    -1,
     131,   132,    11,    12,    -1,    -1,    -1,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    26,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    -1,    62,    63,    64,    -1,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    96,    97,    98,
      99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   122,   123,   124,    -1,   126,     3,     4,
       5,    -1,     7,   132,    -1,    -1,    11,    12,    -1,    -1,
      -1,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    26,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    -1,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    -1,    62,    63,    64,
      -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      95,    96,    97,    98,    99,   100,   101,   102,   103,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,   123,   124,
      -1,   126,     3,     4,     5,    -1,     7,   132,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      -1,    62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    -1,    89,    90,
      -1,    92,    93,    -1,    95,    96,    97,    98,    99,   100,
     101,   102,   103,    -1,    -1,    -1,    -1,    -1,   109,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   122,   123,   124,     3,     4,     5,    -1,     7,    -1,
      -1,   132,    11,    12,    -1,    -1,    -1,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    -1,    62,    63,    64,    -1,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    96,    97,    98,
      99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   122,   123,   124,     3,     4,     5,    -1,
       7,    -1,    -1,   132,    11,    12,    -1,    -1,    -1,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    -1,    62,    63,    64,    -1,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    96,
      97,    98,    99,   100,   101,   102,   103,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   122,   123,   124,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   132,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    -1,    -1,    -1,    -1,    -1,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    -1,    -1,
      77,    78,    -1,    -1,    81,    82,    83,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,   123,   124,    -1,    -1,
      -1,    -1,    -1,    -1,   131,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    -1,    -1,    -1,    -1,    -1,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    -1,    56,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    -1,    -1,    77,
      78,    -1,    -1,    81,    82,    83,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,
      98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,    -1,   123,   124,    -1,    -1,    -1,
      -1,    -1,    -1,   131,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    56,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    -1,    -1,    77,    78,
      -1,    -1,    81,    82,    83,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,    98,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,    -1,   123,   124,    -1,    -1,    -1,    -1,
      -1,    -1,   131,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    -1,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    -1,    -1,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    -1,    -1,    77,    78,    -1,
      -1,    81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,    -1,   123,   124,     3,     4,     5,    -1,     7,
      -1,   131,    -1,    11,    12,    -1,    -1,    -1,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    -1,
      -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,
      -1,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    -1,    62,    63,    64,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    -1,    92,    93,    -1,    -1,    -1,    97,
      98,    99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
      -1,    -1,    -1,    16,   122,    18,    19,    20,    21,    22,
      23,    24,   130,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    -1,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,    -1,    62,
      63,    64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    -1,    -1,    97,    98,    99,   100,   101,   102,
     103,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
      -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,   122,
      18,    19,    20,    21,    22,    23,    24,   130,    -1,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    -1,
      -1,    39,    -1,    -1,    -1,    -1,    -1,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    -1,    62,    63,    64,    -1,    66,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,    97,
      98,    99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,    -1,
      -1,    -1,    11,    12,   122,   123,   124,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    45,    46,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,    63,    64,    -1,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    -1,    97,    98,
      99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   122,   123,   124,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    -1,    62,    63,    64,    -1,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    96,    97,    98,    99,
     100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   122,   123,   124,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      -1,    62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    95,    96,    97,    98,    99,   100,
     101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   122,   123,   124,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
      62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    -1,    97,    98,    99,   100,   101,
     102,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     122,   123,   124,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    -1,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    -1,    62,
      63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    -1,    96,    97,    98,    99,   100,   101,   102,
     103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   122,
     123,   124,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    -1,    62,    63,
      64,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    -1,    97,    98,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   122,   123,
     124,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    -1,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    -1,    62,    63,    64,
      -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      -1,    -1,    97,    98,    99,   100,   101,   102,   103,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   122,   123,   124,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    64,    -1,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      -1,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   122,   123,   124,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,    63,    64,    -1,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,
      97,    98,    99,   100,   101,   102,   103,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   122,   123,   124,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    -1,
      -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,
      -1,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    -1,    62,    63,    64,    -1,    66,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,    97,
      98,    99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   122,   123,   124,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,    63,    64,    -1,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    -1,    97,    98,
      99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   122,   123,   124,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    59,
      60,    -1,    62,    63,    64,    -1,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    -1,    97,    98,    99,
     100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   122,   123,   124,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      -1,    62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    -1,    -1,    97,    98,    99,   100,
     101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   122,   123,   124,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    -1,
      62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    -1,    -1,    97,    98,    99,   100,   101,
     102,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     122,   123,   124,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    -1,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,    -1,    62,
      63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    -1,    -1,    97,    98,    99,   100,   101,   102,
     103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   122,
     123,   124,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
      64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    -1,    97,    98,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,    -1,    -1,    -1,    16,   122,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,    63,    64,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    -1,    97,    98,
      99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,    -1,
      -1,    -1,    16,   122,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
      64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    -1,    -1,    97,    98,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,    -1,    -1,    -1,    16,   122,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,    63,    64,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    -1,    -1,    97,    98,
      99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,    -1,
      -1,    -1,    16,   122,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
      64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    -1,    -1,    97,    98,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,    -1,    -1,    -1,    16,   122,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,    63,    64,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    -1,    -1,    97,    98,
      99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,    -1,
      -1,    -1,    16,   122,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
      64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    -1,    -1,    97,    98,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,    -1,    52,    53,    -1,    -1,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    -1,    -1,
      77,    78,    -1,    -1,    81,    82,    83,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,   123,   124,    52,    53,
      -1,    -1,    56,    -1,   131,   132,    -1,    -1,    -1,    -1,
      -1,    -1,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    -1,    -1,    77,    78,    -1,    -1,    81,    82,    83,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    -1,   123,
     124,    52,    53,    -1,    -1,    56,    -1,   131,   132,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,
      81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,   124,    52,    53,    -1,    -1,    56,    -1,
     131,   132,    -1,    -1,    -1,    -1,    -1,    -1,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    -1,    -1,    77,
      78,    -1,    -1,    81,    82,    83,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,    -1,   123,   124,    52,    53,    -1,
      -1,    56,    -1,   131,   132,    -1,    -1,    -1,    -1,    -1,
      -1,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      -1,    -1,    77,    78,    -1,    -1,    81,    82,    83,    84,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,    -1,   123,   124,
      52,    53,    -1,    -1,    56,    -1,   131,   132,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,    81,
      82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
      -1,   123,   124,    52,    53,    -1,    -1,    56,    -1,   131,
     132,    -1,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    -1,    -1,    77,    78,
      -1,    -1,    81,    82,    83,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,    -1,   123,   124,    52,    53,    -1,    -1,
      56,    -1,   131,   132,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    -1,
      -1,    77,    78,    -1,    -1,    81,    82,    83,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,   123,   124,    52,
      53,    -1,    -1,    56,    -1,   131,   132,    -1,    -1,    -1,
      -1,    -1,    -1,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    -1,    -1,    77,    78,    -1,    -1,    81,    82,
      83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,    -1,
     123,   124,    52,    53,    -1,    -1,    56,    -1,   131,   132,
      -1,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    -1,    -1,    77,    78,    -1,
      -1,    81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,    -1,   123,   124,    52,    53,    -1,    -1,    56,
      -1,   131,   132,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    -1,    -1,
      77,    78,    -1,    -1,    81,    82,    83,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,   123,   124,    52,    53,
      -1,    -1,    56,    -1,   131,   132,    -1,    -1,    -1,    -1,
      -1,    -1,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    -1,    -1,    77,    78,    -1,    -1,    81,    82,    83,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    -1,    -1,    -1,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    -1,   123,
     124,    -1,    -1,    -1,    -1,    -1,    -1,   131,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    -1,    -1,    83,    84,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     110,    -1,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,    -1,    -1,    -1,    -1,    -1,   110,    -1,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   137
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   139,   140,     0,     1,     3,     4,     5,     6,     7,
      11,    12,    16,    18,    19,    20,    21,    22,    23,    24,
      30,    31,    32,    33,    34,    35,    36,    39,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    59,    60,    62,    63,    64,    66,    67,    86,    89,
      90,    92,    93,    95,    97,    98,    99,   100,   101,   102,
     103,   122,   123,   124,   141,   142,   143,   148,   150,   152,
     153,   156,   157,   159,   160,   161,   163,   164,   174,   188,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   238,   239,   249,   250,   251,   252,   253,   254,   255,
     258,   268,   270,   271,   272,   273,   274,   275,   298,   309,
     143,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    56,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    77,    78,    81,    82,    83,    84,
      95,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   123,   124,   131,   167,   168,   169,   170,   172,   173,
     268,   270,    39,    58,    86,    89,    95,    96,   123,   156,
     164,   174,   176,   181,   184,   186,   205,   272,   274,   275,
     296,   297,   181,   181,   132,   182,   183,   132,   178,   182,
     132,   137,   303,    54,   169,   303,   144,   126,    21,    22,
      30,    31,    32,   174,   205,   298,   174,    56,     1,    89,
     146,   147,   148,   158,   159,   309,   150,   177,   186,   296,
     309,   176,   295,   296,   309,    46,    86,   122,   130,   163,
     188,   205,   272,   275,   231,   232,    54,    55,    57,   167,
     261,   269,   260,   261,   262,   136,   256,   136,   259,    59,
      60,   152,   174,   174,   135,   137,   302,   307,   308,    40,
      41,    42,    43,    44,    37,    38,    28,   236,   109,   130,
      89,    95,   160,   109,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    83,    84,   110,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
      85,   128,   129,   189,   150,   151,   151,   192,   194,   151,
     302,   308,    86,   157,   164,   205,   221,   272,   275,    52,
      56,    83,    86,   165,   166,   205,   272,   275,   166,    33,
      34,    35,    36,    49,    50,    51,    52,    56,   132,   167,
     273,   293,    85,   129,    26,   126,   240,   252,    87,    87,
     178,   182,   240,   130,   176,    56,   176,   176,   109,    88,
     130,   185,   309,    85,   128,   129,    87,    87,   130,   185,
     181,   303,   304,   181,   180,   181,   309,   150,   304,   150,
      54,    63,    64,   149,   132,   175,   126,   146,    85,   129,
      87,   148,   158,   133,   302,   308,   304,   190,   134,   130,
     137,   306,   130,   306,   127,   306,   303,    56,    59,    60,
     160,   162,   130,    85,   128,   129,   233,    61,   104,   105,
     106,   263,   106,   263,   106,    65,   263,   106,   106,   257,
     263,   106,    61,   106,    68,    68,   135,   143,   151,   151,
     151,   151,   148,   150,   150,   237,    95,   152,   176,   186,
     187,   158,   130,   163,   130,   152,   174,   176,   187,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,    52,    53,    56,   172,   178,
     299,   300,   180,    52,    53,    56,   172,   178,   299,   145,
     146,    13,   217,   307,   217,   151,   151,   302,    17,   243,
      56,    85,   128,   129,    25,   150,    52,    56,   165,     1,
     113,   276,   307,    85,   128,   129,   201,   294,   202,    85,
     129,   301,    52,    56,   299,   299,   242,   241,   152,   174,
     152,   174,    94,   154,   171,   174,   176,    95,   176,   184,
     296,    52,    56,   180,    52,    56,   297,   304,   133,   304,
     304,   169,   191,   174,   141,   127,   299,   299,   174,   304,
     148,   304,   296,   130,   162,    52,    56,   180,    52,    56,
      52,    54,    55,    56,    57,    89,    95,    96,   116,   119,
     132,   234,   279,   280,   281,   282,   283,   284,   287,   288,
     289,   290,   291,   265,   264,   136,   263,   136,   174,   174,
      76,   114,   226,   227,   309,   176,   130,   304,   162,   130,
      44,   303,    87,    87,   178,   182,   303,   305,    87,    87,
     178,   179,   182,   309,    10,   216,     8,   245,   309,   146,
      13,   146,    27,   218,   307,   218,   243,   186,   216,    52,
      56,   180,    52,    56,   196,   199,   307,   277,   198,    52,
      56,   165,   180,   145,   150,   132,   278,   279,   203,   179,
     182,   179,   182,   226,   226,    44,   155,   169,   176,   185,
      87,    87,   305,    87,    87,   150,   127,   306,   160,   305,
     109,    52,    89,    95,   222,   223,   224,   281,   279,    29,
     107,   235,   130,   292,   309,   130,   292,    52,   130,   292,
      52,   266,    54,    55,    57,   267,   275,    52,   135,   225,
     228,   283,   285,   286,   289,   291,   309,   146,    95,   176,
     162,   174,   111,   152,   174,   152,   174,   154,   134,    87,
     152,   174,   152,   174,   154,   176,   187,   246,   309,    15,
     220,   309,    14,   219,   220,   220,   193,   195,   216,   130,
     217,   305,   151,   307,   151,   145,   305,   216,   304,   279,
     145,   307,   167,   146,   146,   174,   226,    87,   304,   176,
     224,   130,   281,   130,   304,   228,   146,   146,   282,   287,
     289,   291,   283,   284,   289,   283,   146,   109,    52,   229,
     230,   280,   228,   114,   130,   292,   130,   292,   130,   292,
      10,   176,   174,   152,   174,    88,   247,   309,   146,     9,
     248,   309,   151,   216,   216,   146,   146,   176,   146,   218,
     200,   307,   216,   304,   216,   204,    10,   127,   146,   304,
     223,   130,    95,   222,   304,    10,   127,   130,   292,   130,
     292,   130,   292,   130,   292,   292,   127,    86,   205,   130,
     114,   286,   289,   283,   285,   289,   283,    86,   164,   205,
     272,   275,   217,   146,   217,   216,   216,   220,   243,   244,
     197,   145,   278,   127,   130,   223,   130,   281,   283,   289,
     283,   283,    56,    85,   230,   130,   292,   130,   292,   292,
     130,   292,   292,    56,    85,   128,   129,   146,   146,   146,
     216,   145,   223,   130,   292,   130,   292,   292,   292,    52,
      56,   283,   289,   283,   283,    52,    56,   180,    52,    56,
     245,   219,   216,   216,   223,   283,   292,   130,   292,   292,
     292,   305,   292,   283,   292
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   138,   140,   139,   141,   142,   142,   142,   142,   143,
     144,   143,   145,   146,   147,   147,   147,   147,   149,   148,
     148,   148,   148,   148,   148,   148,   148,   148,   148,   148,
     148,   148,   148,   148,   148,   148,   148,   148,   148,   148,
     148,   148,   148,   150,   150,   150,   150,   150,   150,   151,
     152,   152,   153,   153,   153,   155,   154,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   157,   157,
     158,   158,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   160,   160,   161,   161,   162,   162,   163,   163,
     163,   163,   163,   163,   163,   163,   164,   164,   164,   164,
     164,   164,   164,   164,   165,   165,   166,   166,   166,   167,
     167,   167,   167,   167,   168,   168,   169,   169,   170,   171,
     170,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   175,   174,   174,   174,   176,   177,   177,
     177,   177,   178,   179,   179,   180,   180,   181,   181,   181,
     181,   181,   183,   182,   184,   185,   185,   185,   186,   186,
     186,   186,   187,   187,   187,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   189,   188,   190,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   191,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   192,   193,   188,
     194,   195,   188,   188,   188,   196,   197,   188,   198,   188,
     199,   200,   188,   201,   188,   202,   188,   203,   204,   188,
     188,   188,   188,   188,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   217,   217,   218,
     218,   219,   219,   220,   220,   221,   221,   222,   222,   223,
     223,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   226,   226,   227,   227,   227,
     228,   228,   229,   229,   230,   230,   232,   233,   231,   234,
     234,   235,   235,   237,   236,   238,   238,   238,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   241,   240,   242,
     240,   243,   244,   244,   245,   245,   246,   246,   246,   247,
     247,   248,   248,   249,   249,   249,   250,   251,   251,   251,
     252,   253,   254,   255,   255,   256,   256,   257,   257,   258,
     258,   259,   259,   260,   260,   261,   261,   262,   262,   263,
     264,   263,   265,   266,   263,   267,   267,   267,   267,   268,
     269,   269,   269,   269,   270,   271,   271,   271,   271,   272,
     272,   272,   272,   272,   272,   272,   272,   272,   272,   272,
     272,   273,   274,   275,   275,   276,   277,   276,   276,   278,
     278,   279,   279,   279,   279,   279,   279,   279,   279,   279,
     279,   279,   279,   279,   279,   279,   280,   280,   280,   280,
     281,   281,   282,   282,   283,   283,   284,   285,   286,   286,
     287,   287,   288,   288,   289,   289,   290,   290,   291,   292,
     292,   293,   294,   293,   295,   295,   296,   296,   297,   297,
     298,   298,   298,   299,   299,   299,   299,   300,   300,   300,
     301,   301,   302,   302,   303,   303,   304,   305,   306,   306,
     306,   307,   307,   308,   308,   309
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     1,     1,     3,     2,     1,
       0,     5,     4,     2,     1,     1,     3,     2,     0,     4,
       3,     3,     3,     2,     3,     3,     3,     3,     3,     4,
       3,     3,     3,     6,     5,     5,     5,     5,     3,     3,
       3,     3,     1,     1,     3,     3,     3,     2,     1,     1,
       1,     1,     1,     4,     4,     0,     5,     2,     3,     4,
       5,     4,     5,     2,     2,     2,     2,     2,     1,     3,
       1,     3,     1,     2,     3,     5,     2,     4,     2,     4,
       1,     3,     1,     3,     2,     3,     1,     3,     1,     4,
       3,     3,     3,     3,     2,     1,     1,     4,     3,     3,
       3,     3,     2,     1,     1,     1,     2,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       4,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     5,     3,     5,     6,     5,     5,     5,     5,
       4,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       4,     4,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     3,
       3,     3,     3,     0,     4,     6,     1,     1,     1,     2,
       4,     2,     3,     1,     1,     1,     1,     1,     2,     2,
       4,     1,     0,     2,     2,     2,     1,     1,     1,     2,
       3,     4,     3,     4,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     4,     0,     4,     3,     3,
       2,     3,     3,     1,     4,     3,     1,     0,     6,     4,
       3,     2,     1,     2,     2,     6,     6,     0,     0,     7,
       0,     0,     7,     5,     4,     0,     0,     9,     0,     6,
       0,     0,     8,     0,     5,     0,     6,     0,     0,     9,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       1,     1,     5,     1,     2,     1,     1,     1,     3,     1,
       3,     1,     4,     6,     3,     5,     2,     4,     1,     3,
       6,     8,     4,     6,     4,     2,     6,     2,     4,     6,
       2,     4,     2,     4,     1,     1,     1,     3,     1,     4,
       1,     2,     1,     3,     1,     1,     0,     0,     4,     4,
       1,     3,     3,     0,     5,     2,     4,     4,     2,     4,
       4,     3,     3,     3,     2,     1,     4,     0,     5,     0,
       5,     5,     1,     1,     6,     1,     1,     1,     1,     2,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     2,
       3,     3,     3,     3,     3,     0,     3,     1,     2,     3,
       3,     0,     3,     0,     2,     0,     2,     0,     2,     1,
       0,     3,     0,     0,     5,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     3,     1,     1,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     4,     2,     3,
       2,     6,     8,     4,     6,     4,     6,     2,     4,     6,
       2,     4,     2,     4,     1,     0,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     3,     3,     3,     1,     3,
       1,     3,     1,     1,     2,     1,     1,     1,     2,     2,
       1,     1,     0,     4,     1,     2,     1,     3,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     0,     1,     2,     2,     0,     1,
       1,     1,     1,     1,     2,     0
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (parser_state, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, parser_state); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, rb_parser_state* parser_state)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (parser_state);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, rb_parser_state* parser_state)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, parser_state);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, rb_parser_state* parser_state)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , parser_state);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, parser_state); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, rb_parser_state* parser_state)
{
  YYUSE (yyvaluep);
  YYUSE (parser_state);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (rb_parser_state* parser_state)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, parser_state);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 547 "grammar19.y" /* yacc.c:1646  */
    {
                    lex_state = EXPR_BEG;
                    local_push(0);
                    class_nest = 0;
                  }
#line 4261 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 3:
#line 553 "grammar19.y" /* yacc.c:1646  */
    {
                    if((yyvsp[0].node) && !compile_for_eval) {
                      /* last expression should not be void */
                      if(nd_type((yyvsp[0].node)) != NODE_BLOCK) {
                        void_expr((yyvsp[0].node));
                      } else {
                        NODE *node = (yyvsp[0].node);
                        while(node->nd_next) {
                          node = node->nd_next;
                        }
                        void_expr(node->nd_head);
                      }
                    }
                    top_node = block_append(top_node, (yyvsp[0].node));
                    class_nest = 0;
                    local_pop();
                  }
#line 4283 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 573 "grammar19.y" /* yacc.c:1646  */
    {
                    void_stmts((yyvsp[-1].node));
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 4292 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 5:
#line 580 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_BEGIN(0);
                  }
#line 4300 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 584 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = newline_node((yyvsp[0].node));
                  }
#line 4308 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 588 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = block_append((yyvsp[-2].node), newline_node((yyvsp[0].node)));
                  }
#line 4316 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 592 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = remove_begin((yyvsp[0].node));
                  }
#line 4324 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 599 "grammar19.y" /* yacc.c:1646  */
    {
                    if(in_def || in_single) {
                      yy_error("BEGIN in method");
                    }
                  }
#line 4334 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 605 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_PREEXE((yyvsp[-1].node));
                  }
#line 4342 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 614 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-3].node);
                    if((yyvsp[-2].node)) {
                      (yyval.node) = NEW_RESCUE((yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[-1].node));
                    } else if((yyvsp[-1].node)) {
                      rb_warn("else without rescue is useless");
                      (yyval.node) = block_append((yyval.node), (yyvsp[-1].node));
                    }
                    if((yyvsp[0].node)) {
                      if((yyval.node)) {
                        (yyval.node) = NEW_ENSURE((yyval.node), (yyvsp[0].node));
                      } else {
                        (yyval.node) = block_append((yyvsp[0].node), NEW_NIL());
                      }
                    }
                    fixpos((yyval.node), (yyvsp[-3].node));
                  }
#line 4364 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 634 "grammar19.y" /* yacc.c:1646  */
    {
                    void_stmts((yyvsp[-1].node));
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 4373 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 641 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_BEGIN(0);
                  }
#line 4381 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 645 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = newline_node((yyvsp[0].node));
                  }
#line 4389 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 649 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = block_append((yyvsp[-2].node), newline_node((yyvsp[0].node)));
                  }
#line 4397 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 653 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = remove_begin((yyvsp[0].node));
                  }
#line 4405 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 658 "grammar19.y" /* yacc.c:1646  */
    {lex_state = EXPR_FNAME;}
#line 4411 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 659 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_ALIAS((yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 4419 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 663 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_VALIAS((yyvsp[-1].id), (yyvsp[0].id));
                  }
#line 4427 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 667 "grammar19.y" /* yacc.c:1646  */
    {
                    char buf[2];
                    buf[0] = '$';
                    buf[1] = (char)(yyvsp[0].node)->nd_nth;
                    (yyval.node) = NEW_VALIAS((yyvsp[-1].id), parser_intern2(buf, 2));
                  }
#line 4438 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 674 "grammar19.y" /* yacc.c:1646  */
    {
                    yy_error("can't make alias for the number variables");
                    (yyval.node) = NEW_BEGIN(0);
                  }
#line 4447 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 679 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[0].node);
                  }
#line 4455 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 683 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_IF(cond((yyvsp[0].node)), remove_begin((yyvsp[-2].node)), 0);
                    fixpos((yyval.node), (yyvsp[0].node));
                  }
#line 4464 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 688 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_UNLESS(cond((yyvsp[0].node)), remove_begin((yyvsp[-2].node)), 0);
                    fixpos((yyval.node), (yyvsp[0].node));
                  }
#line 4473 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 693 "grammar19.y" /* yacc.c:1646  */
    {
                    if((yyvsp[-2].node) && nd_type((yyvsp[-2].node)) == NODE_BEGIN) {
                      (yyval.node) = NEW_WHILE(cond((yyvsp[0].node)), (yyvsp[-2].node)->nd_body, 0);
                    } else {
                      (yyval.node) = NEW_WHILE(cond((yyvsp[0].node)), (yyvsp[-2].node), 1);
                    }
                  }
#line 4485 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 701 "grammar19.y" /* yacc.c:1646  */
    {
                    if((yyvsp[-2].node) && nd_type((yyvsp[-2].node)) == NODE_BEGIN) {
                      (yyval.node) = NEW_UNTIL(cond((yyvsp[0].node)), (yyvsp[-2].node)->nd_body, 0);
                    } else {
                      (yyval.node) = NEW_UNTIL(cond((yyvsp[0].node)), (yyvsp[-2].node), 1);
                    }
                  }
#line 4497 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 709 "grammar19.y" /* yacc.c:1646  */
    {
                    NODE *resq = NEW_RESBODY(0, remove_begin((yyvsp[0].node)), 0);
                    (yyval.node) = NEW_RESCUE(remove_begin((yyvsp[-2].node)), resq, 0);
                  }
#line 4506 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 714 "grammar19.y" /* yacc.c:1646  */
    {
                    if(in_def || in_single) {
                      rb_warn("END in method; use at_exit");
                    }

                    (yyval.node) = NEW_POSTEXE(NEW_NODE(NODE_SCOPE,
                            0 /* tbl */, (yyvsp[-1].node) /* body */, 0 /* args */));
                  }
#line 4519 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 723 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    (yyval.node) = node_assign((yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 4528 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 728 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    (yyvsp[-2].node)->nd_value = ((yyvsp[-2].node)->nd_head) ? NEW_TO_ARY((yyvsp[0].node)) : NEW_ARRAY((yyvsp[0].node));
                    (yyval.node) = (yyvsp[-2].node);
                  }
#line 4538 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 734 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    if((yyvsp[-2].node)) {
                      ID vid = (yyvsp[-2].node)->nd_vid;
                      if((yyvsp[-1].id) == tOROP) {
                        (yyvsp[-2].node)->nd_value = (yyvsp[0].node);
                        (yyval.node) = NEW_OP_ASGN_OR(gettable(vid), (yyvsp[-2].node));
                        if(is_asgn_or_id(vid)) {
                          (yyval.node)->nd_aid = vid;
                        }
                      } else if((yyvsp[-1].id) == tANDOP) {
                        (yyvsp[-2].node)->nd_value = (yyvsp[0].node);
                        (yyval.node) = NEW_OP_ASGN_AND(gettable(vid), (yyvsp[-2].node));
                      } else {
                        (yyval.node) = (yyvsp[-2].node);
                        (yyval.node)->nd_value = call_bin_op(gettable(vid), (yyvsp[-1].id), (yyvsp[0].node));
                      }
                    } else {
                      (yyval.node) = NEW_BEGIN(0);
                    }
                  }
#line 4564 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 756 "grammar19.y" /* yacc.c:1646  */
    {
                    NODE *args;

                    value_expr((yyvsp[0].node));
                    if(!(yyvsp[-3].node)) (yyvsp[-3].node) = NEW_ZARRAY();
                    args = arg_concat((yyvsp[-3].node), (yyvsp[0].node));
                    if((yyvsp[-1].id) == tOROP) {
                      (yyvsp[-1].id) = 0;
                    } else if((yyvsp[-1].id) == tANDOP) {
                      (yyvsp[-1].id) = 1;
                    } else {
                      (yyvsp[-1].id) = convert_op((yyvsp[-1].id));
                    }
                    (yyval.node) = NEW_OP_ASGN1((yyvsp[-5].node), (yyvsp[-1].id), args);
                    fixpos((yyval.node), (yyvsp[-5].node));
                  }
#line 4585 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 773 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    if((yyvsp[-1].id) == tOROP) {
                      (yyvsp[-1].id) = 0;
                    } else if((yyvsp[-1].id) == tANDOP) {
                      (yyvsp[-1].id) = 1;
                    } else {
                      (yyvsp[-1].id) = convert_op((yyvsp[-1].id));
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[-4].node), (yyvsp[-2].id), (yyvsp[-1].id), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-4].node));
                  }
#line 4602 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 786 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    if((yyvsp[-1].id) == tOROP) {
                      (yyvsp[-1].id) = 0;
                    } else if((yyvsp[-1].id) == tANDOP) {
                      (yyvsp[-1].id) = 1;
                    } else {
                      (yyvsp[-1].id) = convert_op((yyvsp[-1].id));
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[-4].node), (yyvsp[-2].id), (yyvsp[-1].id), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-4].node));
                  }
#line 4619 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 799 "grammar19.y" /* yacc.c:1646  */
    {
                    yy_error("constant re-assignment");
                    (yyval.node) = 0;
                  }
#line 4628 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 804 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    if((yyvsp[-1].id) == tOROP) {
                      (yyvsp[-1].id) = 0;
                    } else if((yyvsp[-1].id) == tANDOP) {
                      (yyvsp[-1].id) = 1;
                    } else {
                      (yyvsp[-1].id) = convert_op((yyvsp[-1].id));
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[-4].node), (yyvsp[-2].id), (yyvsp[-1].id), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-4].node));
                  }
#line 4645 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 817 "grammar19.y" /* yacc.c:1646  */
    {
                    rb_backref_error((yyvsp[-2].node));
                    (yyval.node) = 0;
                  }
#line 4654 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 822 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    (yyval.node) = node_assign((yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 4663 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 827 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyvsp[-2].node)->nd_value = ((yyvsp[-2].node)->nd_head) ? NEW_TO_ARY((yyvsp[0].node)) : NEW_ARRAY((yyvsp[0].node));
                    (yyval.node) = (yyvsp[-2].node);
                  }
#line 4672 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 832 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyvsp[-2].node)->nd_value = (yyvsp[0].node);
                    (yyval.node) = (yyvsp[-2].node);
                  }
#line 4681 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 841 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = logop(NODE_AND, (yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 4689 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 845 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = logop(NODE_OR, (yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 4697 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 849 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[0].node)), '!');
                  }
#line 4705 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 853 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[0].node)), '!');
                  }
#line 4713 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 860 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    (yyval.node) = (yyvsp[0].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  }
#line 4723 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 873 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CALL((yyvsp[-3].node), (yyvsp[-1].id), (yyvsp[0].node));
                  }
#line 4731 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 877 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CALL((yyvsp[-3].node), (yyvsp[-1].id), (yyvsp[0].node));
                  }
#line 4739 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 883 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyvsp[0].vars) = bv_push();
                    (yyval.num) = sourceline;
                  }
#line 4748 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 890 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_ITER((yyvsp[-2].node), (yyvsp[-1].node));
                    nd_set_line((yyval.node), (yyvsp[-3].num));
                    bv_pop((yyvsp[-4].vars));
                  }
#line 4758 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 898 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_FCALL((yyvsp[-1].id), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[0].node));
                 }
#line 4767 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 903 "grammar19.y" /* yacc.c:1646  */
    {
                    block_dup_check((yyvsp[-1].node), (yyvsp[0].node));
                    (yyvsp[0].node)->nd_iter = NEW_FCALL((yyvsp[-2].id), (yyvsp[-1].node));
                    (yyval.node) = (yyvsp[0].node);
                    fixpos((yyval.node), (yyvsp[-1].node));
                 }
#line 4778 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 59:
#line 910 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CALL((yyvsp[-3].node), (yyvsp[-1].id), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-3].node));
                  }
#line 4787 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 915 "grammar19.y" /* yacc.c:1646  */
    {
                    block_dup_check((yyvsp[-1].node), (yyvsp[0].node));
                    (yyvsp[0].node)->nd_iter = NEW_CALL((yyvsp[-4].node), (yyvsp[-2].id), (yyvsp[-1].node));
                    (yyval.node) = (yyvsp[0].node);
                    fixpos((yyval.node), (yyvsp[-4].node));
                 }
#line 4798 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 922 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CALL((yyvsp[-3].node), (yyvsp[-1].id), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-3].node));
                  }
#line 4807 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 927 "grammar19.y" /* yacc.c:1646  */
    {
                    block_dup_check((yyvsp[-1].node), (yyvsp[0].node));
                    (yyvsp[0].node)->nd_iter = NEW_CALL((yyvsp[-4].node), (yyvsp[-2].id), (yyvsp[-1].node));
                    (yyval.node) = (yyvsp[0].node);
                    fixpos((yyval.node), (yyvsp[-4].node));
                  }
#line 4818 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 934 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_SUPER((yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[0].node));
                  }
#line 4827 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 939 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_yield((yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[0].node));
                  }
#line 4836 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 944 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_RETURN(ret_args((yyvsp[0].node)));
                  }
#line 4844 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 948 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_BREAK(ret_args((yyvsp[0].node)));
                  }
#line 4852 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 952 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_NEXT(ret_args((yyvsp[0].node)));
                  }
#line 4860 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 959 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 4868 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 966 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN(NEW_LIST((yyvsp[-1].node)), 0);
                  }
#line 4876 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 972 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN((yyvsp[0].node), 0);
                  }
#line 4884 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 976 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN(list_append((yyvsp[-1].node), (yyvsp[0].node)), 0);
                  }
#line 4892 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 980 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN((yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 4900 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 984 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN((yyvsp[-4].node), NEW_POSTARG((yyvsp[-2].node), (yyvsp[0].node)));
                  }
#line 4908 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 988 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN((yyvsp[-1].node), -1);
                  }
#line 4916 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 992 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN((yyvsp[-3].node), NEW_POSTARG(-1, (yyvsp[0].node)));
                  }
#line 4924 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 996 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN(0, (yyvsp[0].node));
                  }
#line 4932 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 1000 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG((yyvsp[-2].node), (yyvsp[0].node)));
                  }
#line 4940 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 1004 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN(0, -1);
                  }
#line 4948 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 1008 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG(-1, (yyvsp[0].node)));
                  }
#line 4956 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 1015 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 4964 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 1021 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_LIST((yyvsp[-1].node));
                  }
#line 4972 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 1025 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = list_append((yyvsp[-2].node), (yyvsp[-1].node));
                  }
#line 4980 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 1031 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_LIST((yyvsp[0].node));
                  }
#line 4988 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 1035 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = list_append((yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 4996 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 1041 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = assignable((yyvsp[0].id), 0);
                  }
#line 5004 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 1045 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = aryset((yyvsp[-3].node), (yyvsp[-1].node));
                  }
#line 5012 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 1049 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = attrset((yyvsp[-2].node), (yyvsp[0].id));
                  }
#line 5020 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 1053 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = attrset((yyvsp[-2].node), (yyvsp[0].id));
                  }
#line 5028 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 1057 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = attrset((yyvsp[-2].node), (yyvsp[0].id));
                  }
#line 5036 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 1061 "grammar19.y" /* yacc.c:1646  */
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[-2].node), (yyvsp[0].id)));
                  }
#line 5046 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 1067 "grammar19.y" /* yacc.c:1646  */
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[0].id)));
                  }
#line 5056 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 1073 "grammar19.y" /* yacc.c:1646  */
    {
                    rb_backref_error((yyvsp[0].node));
                    (yyval.node) = NEW_BEGIN(0);
                  }
#line 5065 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 1080 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = assignable((yyvsp[0].id), 0);
                    if(!(yyval.node)) (yyval.node) = NEW_BEGIN(0);
                  }
#line 5074 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 1085 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = aryset((yyvsp[-3].node), (yyvsp[-1].node));
                  }
#line 5082 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 1089 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = attrset((yyvsp[-2].node), (yyvsp[0].id));
                  }
#line 5090 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 1093 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = attrset((yyvsp[-2].node), (yyvsp[0].id));
                  }
#line 5098 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 1097 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = attrset((yyvsp[-2].node), (yyvsp[0].id));
                  }
#line 5106 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 101:
#line 1101 "grammar19.y" /* yacc.c:1646  */
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[-2].node), (yyvsp[0].id)));
                  }
#line 5116 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 102:
#line 1107 "grammar19.y" /* yacc.c:1646  */
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[0].id)));
                  }
#line 5126 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 103:
#line 1113 "grammar19.y" /* yacc.c:1646  */
    {
                    rb_backref_error((yyvsp[0].node));
                    (yyval.node) = NEW_BEGIN(0);
                  }
#line 5135 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 104:
#line 1120 "grammar19.y" /* yacc.c:1646  */
    {
                    yy_error("class/module name must be CONSTANT");
                  }
#line 5143 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 106:
#line 1127 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_COLON3((yyvsp[0].id));
                  }
#line 5151 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 107:
#line 1131 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_COLON2(0, (yyval.node));
                  }
#line 5159 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 108:
#line 1135 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_COLON2((yyvsp[-2].node), (yyvsp[0].id));
                  }
#line 5167 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 112:
#line 1144 "grammar19.y" /* yacc.c:1646  */
    {
                    lex_state = EXPR_ENDFN;
                    (yyval.id) = convert_op((yyvsp[0].id));
                  }
#line 5176 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 113:
#line 1149 "grammar19.y" /* yacc.c:1646  */
    {
                    lex_state = EXPR_ENDFN;
                    (yyval.id) = (yyvsp[0].id);
                  }
#line 5185 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 116:
#line 1160 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_LIT(ID2SYM((yyvsp[0].id)));
                  }
#line 5193 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 118:
#line 1167 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_UNDEF((yyvsp[0].node));
                  }
#line 5201 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 119:
#line 1170 "grammar19.y" /* yacc.c:1646  */
    {lex_state = EXPR_FNAME;}
#line 5207 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 120:
#line 1171 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = block_append((yyvsp[-3].node), NEW_UNDEF((yyvsp[0].node)));
                  }
#line 5215 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 121:
#line 1176 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = '|'; }
#line 5221 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 122:
#line 1177 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = '^'; }
#line 5227 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 123:
#line 1178 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = '&'; }
#line 5233 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 124:
#line 1179 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tCMP; }
#line 5239 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 125:
#line 1180 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tEQ; }
#line 5245 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 126:
#line 1181 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tEQQ; }
#line 5251 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 127:
#line 1182 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tMATCH; }
#line 5257 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 128:
#line 1183 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tNMATCH; }
#line 5263 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 129:
#line 1184 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = '>'; }
#line 5269 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 130:
#line 1185 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tGEQ; }
#line 5275 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 131:
#line 1186 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = '<'; }
#line 5281 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 132:
#line 1187 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tLEQ; }
#line 5287 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 133:
#line 1188 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tNEQ; }
#line 5293 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 134:
#line 1189 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tLSHFT; }
#line 5299 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 135:
#line 1190 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tRSHFT; }
#line 5305 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 136:
#line 1191 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = '+'; }
#line 5311 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 137:
#line 1192 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = '-'; }
#line 5317 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 138:
#line 1193 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = '*'; }
#line 5323 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 139:
#line 1194 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = '*'; }
#line 5329 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 140:
#line 1195 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = '/'; }
#line 5335 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 141:
#line 1196 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = '%'; }
#line 5341 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 142:
#line 1197 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tPOW; }
#line 5347 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 143:
#line 1198 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = '!'; }
#line 5353 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 144:
#line 1199 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = '~'; }
#line 5359 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 145:
#line 1200 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tUPLUS; }
#line 5365 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 146:
#line 1201 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tUMINUS; }
#line 5371 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 147:
#line 1202 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tAREF; }
#line 5377 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 148:
#line 1203 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = tASET; }
#line 5383 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 149:
#line 1204 "grammar19.y" /* yacc.c:1646  */
    { (yyval.id) = '`'; }
#line 5389 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 191:
#line 1222 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    (yyval.node) = node_assign((yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 5398 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 192:
#line 1227 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[-2].node));
                    (yyvsp[-2].node) = NEW_RESCUE((yyvsp[-2].node), NEW_RESBODY(0, (yyvsp[0].node), 0), 0);
                    (yyval.node) = node_assign((yyvsp[-4].node), (yyvsp[-2].node));
                  }
#line 5408 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 193:
#line 1233 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    if((yyvsp[-2].node)) {
                      ID vid = (yyvsp[-2].node)->nd_vid;
                      if((yyvsp[-1].id) == tOROP) {
                        (yyvsp[-2].node)->nd_value = (yyvsp[0].node);
                        (yyval.node) = NEW_OP_ASGN_OR(gettable(vid), (yyvsp[-2].node));
                        if(is_asgn_or_id(vid)) {
                          (yyval.node)->nd_aid = vid;
                        }
                      } else if((yyvsp[-1].id) == tANDOP) {
                        (yyvsp[-2].node)->nd_value = (yyvsp[0].node);
                        (yyval.node) = NEW_OP_ASGN_AND(gettable(vid), (yyvsp[-2].node));
                      } else {
                        (yyval.node) = (yyvsp[-2].node);
                        (yyval.node)->nd_value = NEW_CALL(gettable(vid), (yyvsp[-1].id), NEW_LIST((yyvsp[0].node)));
                      }
                    } else {
                      (yyval.node) = NEW_BEGIN(0);
                    }
                  }
#line 5434 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 194:
#line 1255 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[-2].node));
                    (yyvsp[-2].node) = NEW_RESCUE((yyvsp[-2].node), NEW_RESBODY(0, (yyvsp[0].node), 0), 0);
                    if((yyvsp[-4].node)) {
                      ID vid = (yyvsp[-4].node)->nd_vid;
                      if((yyvsp[-3].id) == tOROP) {
                        (yyvsp[-4].node)->nd_value = (yyvsp[-2].node);
                        (yyval.node) = NEW_OP_ASGN_OR(gettable(vid), (yyvsp[-4].node));
                        if(is_asgn_or_id(vid)) {
                          (yyval.node)->nd_aid = vid;
                        }
                      } else if((yyvsp[-3].id) == tANDOP) {
                        (yyvsp[-4].node)->nd_value = (yyvsp[-2].node);
                        (yyval.node) = NEW_OP_ASGN_AND(gettable(vid), (yyvsp[-4].node));
                      } else {
                        (yyval.node) = (yyvsp[-4].node);
                        (yyval.node)->nd_value = NEW_CALL(gettable(vid), (yyvsp[-3].id), NEW_LIST((yyvsp[-2].node)));
                      }
                    } else {
                      (yyval.node) = NEW_BEGIN(0);
                    }
                  }
#line 5461 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 195:
#line 1278 "grammar19.y" /* yacc.c:1646  */
    {
                    NODE *args;

                    value_expr((yyvsp[0].node));
                    if(!(yyvsp[-3].node)) (yyvsp[-3].node) = NEW_ZARRAY();
                    if(nd_type((yyvsp[-3].node)) == NODE_BLOCK_PASS) {
                      args = NEW_ARGSCAT((yyvsp[-3].node), (yyvsp[0].node));
                    } else {
                      args = arg_concat((yyvsp[-3].node), (yyvsp[0].node));
                    }
                    if((yyvsp[-1].id) == tOROP) {
                      (yyvsp[-1].id) = 0;
                    } else if((yyvsp[-1].id) == tANDOP) {
                      (yyvsp[-1].id) = 1;
                    } else {
                      (yyvsp[-1].id) = convert_op((yyvsp[-1].id));
                    }
                    (yyval.node) = NEW_OP_ASGN1((yyvsp[-5].node), (yyvsp[-1].id), args);
                    fixpos((yyval.node), (yyvsp[-5].node));
                  }
#line 5486 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 196:
#line 1299 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    if((yyvsp[-1].id) == tOROP) {
                      (yyvsp[-1].id) = 0;
                    } else if((yyvsp[-1].id) == tANDOP) {
                      (yyvsp[-1].id) = 1;
                    } else {
                      (yyvsp[-1].id) = convert_op((yyvsp[-1].id));
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[-4].node), (yyvsp[-2].id), (yyvsp[-1].id), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-4].node));
                  }
#line 5503 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 197:
#line 1312 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    if((yyvsp[-1].id) == tOROP) {
                      (yyvsp[-1].id) = 0;
                    } else if((yyvsp[-1].id) == tANDOP) {
                      (yyvsp[-1].id) = 1;
                    } else {
                      (yyvsp[-1].id) = convert_op((yyvsp[-1].id));
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[-4].node), (yyvsp[-2].id), (yyvsp[-1].id), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-4].node));
                  }
#line 5520 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 198:
#line 1325 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    if((yyvsp[-1].id) == tOROP) {
                      (yyvsp[-1].id) = 0;
                    } else if((yyvsp[-1].id) == tANDOP) {
                      (yyvsp[-1].id) = 1;
                    } else {
                      (yyvsp[-1].id) = convert_op((yyvsp[-1].id));
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[-4].node), (yyvsp[-2].id), (yyvsp[-1].id), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-4].node));
                  }
#line 5537 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 199:
#line 1338 "grammar19.y" /* yacc.c:1646  */
    {
                    yy_error("constant re-assignment");
                    (yyval.node) = NEW_BEGIN(0);
                  }
#line 5546 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 200:
#line 1343 "grammar19.y" /* yacc.c:1646  */
    {
                    yy_error("constant re-assignment");
                    (yyval.node) = NEW_BEGIN(0);
                  }
#line 5555 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 201:
#line 1348 "grammar19.y" /* yacc.c:1646  */
    {
                    rb_backref_error((yyvsp[-2].node));
                    (yyval.node) = NEW_BEGIN(0);
                  }
#line 5564 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 202:
#line 1353 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[-2].node));
                    value_expr((yyvsp[0].node));
                    (yyval.node) = NEW_DOT2((yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 5574 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 203:
#line 1359 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[-2].node));
                    value_expr((yyvsp[0].node));
                    (yyval.node) = NEW_DOT3((yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 5584 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 204:
#line 1365 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), '+', (yyvsp[0].node));
                  }
#line 5592 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 205:
#line 1369 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), '-', (yyvsp[0].node));
                  }
#line 5600 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 206:
#line 1373 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), '*', (yyvsp[0].node));
                  }
#line 5608 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 207:
#line 1377 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), '/', (yyvsp[0].node));
                  }
#line 5616 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 208:
#line 1381 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), '%', (yyvsp[0].node));
                  }
#line 5624 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 209:
#line 1385 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), tPOW, (yyvsp[0].node));
                  }
#line 5632 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 210:
#line 1389 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CALL(call_bin_op((yyvsp[-2].node), tPOW, (yyvsp[0].node)), tUMINUS, 0);
                  }
#line 5640 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 211:
#line 1393 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CALL(call_bin_op((yyvsp[-2].node), tPOW, (yyvsp[0].node)), tUMINUS, 0);
                  }
#line 5648 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 212:
#line 1397 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_uni_op((yyvsp[0].node), tUPLUS);
                  }
#line 5656 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 213:
#line 1401 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_uni_op((yyvsp[0].node), tUMINUS);
                  }
#line 5664 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 214:
#line 1405 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), '|', (yyvsp[0].node));
                  }
#line 5672 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 215:
#line 1409 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), '^', (yyvsp[0].node));
                  }
#line 5680 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 216:
#line 1413 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), '&', (yyvsp[0].node));
                  }
#line 5688 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 217:
#line 1417 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), tCMP, (yyvsp[0].node));
                  }
#line 5696 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 218:
#line 1421 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), '>', (yyvsp[0].node));
                  }
#line 5704 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 219:
#line 1425 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), tGEQ, (yyvsp[0].node));
                  }
#line 5712 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 220:
#line 1429 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), '<', (yyvsp[0].node));
                  }
#line 5720 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 221:
#line 1433 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), tLEQ, (yyvsp[0].node));
                  }
#line 5728 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 222:
#line 1437 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), tEQ, (yyvsp[0].node));
                  }
#line 5736 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 223:
#line 1441 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), tEQQ, (yyvsp[0].node));
                  }
#line 5744 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 224:
#line 1445 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), tNEQ, (yyvsp[0].node));
                  }
#line 5752 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 225:
#line 1449 "grammar19.y" /* yacc.c:1646  */
    {
                    /* TODO */
                    (yyval.node) = match_op((yyvsp[-2].node), (yyvsp[0].node));
                    /*
                    if(nd_type($1) == NODE_LIT && TYPE($1->nd_lit) == T_REGEXP) {
                      $$ = reg_named_capture_assign($1->nd_lit, $$);
                    }
                    */
                  }
#line 5766 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 226:
#line 1459 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), tNMATCH, (yyvsp[0].node));
                  }
#line 5774 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 227:
#line 1463 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[0].node)), '!');
                  }
#line 5782 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 228:
#line 1467 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_uni_op((yyvsp[0].node), '~');
                  }
#line 5790 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 229:
#line 1471 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), tLSHFT, (yyvsp[0].node));
                  }
#line 5798 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 230:
#line 1475 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_bin_op((yyvsp[-2].node), tRSHFT, (yyvsp[0].node));
                  }
#line 5806 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 231:
#line 1479 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = logop(NODE_AND, (yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 5814 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 232:
#line 1483 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = logop(NODE_OR, (yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 5822 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 233:
#line 1486 "grammar19.y" /* yacc.c:1646  */
    {in_defined = 1;}
#line 5828 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 234:
#line 1487 "grammar19.y" /* yacc.c:1646  */
    {
                    in_defined = 0;
                    (yyval.node) = NEW_DEFINED((yyvsp[0].node));
                  }
#line 5837 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 235:
#line 1492 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[-5].node));
                    (yyval.node) = NEW_IF(cond((yyvsp[-5].node)), (yyvsp[-3].node), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-5].node));
                  }
#line 5847 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 236:
#line 1498 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[0].node);
                  }
#line 5855 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 237:
#line 1504 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    (yyval.node) = (yyvsp[0].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  }
#line 5865 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 239:
#line 1513 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 5873 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 240:
#line 1517 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = arg_append((yyvsp[-3].node), NEW_HASH((yyvsp[-1].node)));
                  }
#line 5881 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 241:
#line 1521 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[-1].node)));
                  }
#line 5889 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 242:
#line 1527 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 5897 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 247:
#line 1541 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    (yyval.node) = NEW_LIST((yyvsp[0].node));
                  }
#line 5906 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 248:
#line 1546 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = arg_blk_pass((yyvsp[-1].node), (yyvsp[0].node));
                  }
#line 5914 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 249:
#line 1550 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[-1].node)));
                    (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[0].node));
                  }
#line 5923 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 250:
#line 1555 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = arg_append((yyvsp[-3].node), NEW_HASH((yyvsp[-1].node)));
                    (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[0].node));
                  }
#line 5932 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 252:
#line 1562 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.val) = cmdarg_stack;
                    CMDARG_PUSH(1);
                  }
#line 5941 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 253:
#line 1567 "grammar19.y" /* yacc.c:1646  */
    {
                    /* CMDARG_POP() */
                    cmdarg_stack = (yyvsp[-1].val);
                    (yyval.node) = (yyvsp[0].node);
                  }
#line 5951 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 254:
#line 1575 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_BLOCK_PASS((yyvsp[0].node));
                  }
#line 5959 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 255:
#line 1581 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[0].node);
                  }
#line 5967 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 256:
#line 1585 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = 0;
                  }
#line 5975 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 257:
#line 1589 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = 0;
                  }
#line 5983 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 258:
#line 1595 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_LIST((yyvsp[0].node));
                  }
#line 5991 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 259:
#line 1599 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_SPLAT((yyvsp[0].node));
                  }
#line 5999 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 260:
#line 1603 "grammar19.y" /* yacc.c:1646  */
    {
                    NODE *n1;
                    if((n1 = splat_array((yyvsp[-2].node))) != 0) {
                      (yyval.node) = list_append(n1, (yyvsp[0].node));
                    } else {
                      (yyval.node) = arg_append((yyvsp[-2].node), (yyvsp[0].node));
                    }
                  }
#line 6012 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 261:
#line 1612 "grammar19.y" /* yacc.c:1646  */
    {
                    NODE *n1;
                    if((nd_type((yyvsp[0].node)) == NODE_ARRAY) && (n1 = splat_array((yyvsp[-3].node))) != 0) {
                      (yyval.node) = list_concat(n1, (yyvsp[0].node));
                    } else {
                      (yyval.node) = arg_concat((yyvsp[-3].node), (yyvsp[0].node));
                    }
                  }
#line 6025 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 262:
#line 1623 "grammar19.y" /* yacc.c:1646  */
    {
                    NODE *n1;
                    if((n1 = splat_array((yyvsp[-2].node))) != 0) {
                      (yyval.node) = list_append(n1, (yyvsp[0].node));
                    } else {
                      (yyval.node) = arg_append((yyvsp[-2].node), (yyvsp[0].node));
                    }
                  }
#line 6038 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 263:
#line 1632 "grammar19.y" /* yacc.c:1646  */
    {
                    NODE *n1;
                    if(nd_type((yyvsp[0].node)) == NODE_ARRAY && (n1 = splat_array((yyvsp[-3].node))) != 0) {
                      (yyval.node) = list_concat(n1, (yyvsp[0].node));
                    } else {
                      (yyval.node) = arg_concat((yyvsp[-3].node), (yyvsp[0].node));
                    }
                  }
#line 6051 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 264:
#line 1641 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_SPLAT((yyvsp[0].node));
                  }
#line 6059 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 273:
#line 1655 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_FCALL((yyvsp[0].id), 0);
                  }
#line 6067 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 274:
#line 1659 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.num) = sourceline;
                  }
#line 6075 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 275:
#line 1664 "grammar19.y" /* yacc.c:1646  */
    {
                    if((yyvsp[-1].node) == NULL) {
                      (yyval.node) = NEW_NIL();
                    } else {
                      if(nd_type((yyvsp[-1].node)) == NODE_RESCUE || nd_type((yyvsp[-1].node)) == NODE_ENSURE) {
                        nd_set_line((yyvsp[-1].node), (yyvsp[-2].num));
                      }
                      (yyval.node) = NEW_BEGIN((yyvsp[-1].node));
                    }
                    nd_set_line((yyval.node), (yyvsp[-2].num));
                  }
#line 6091 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 276:
#line 1675 "grammar19.y" /* yacc.c:1646  */
    {lex_state = EXPR_ENDARG;}
#line 6097 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 277:
#line 1676 "grammar19.y" /* yacc.c:1646  */
    {
                    rb_warning("(...) interpreted as grouped expression");
                    (yyval.node) = (yyvsp[-2].node);
                  }
#line 6106 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 278:
#line 1681 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 6114 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 279:
#line 1685 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_COLON2((yyvsp[-2].node), (yyvsp[0].id));
                  }
#line 6122 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 280:
#line 1689 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_COLON3((yyvsp[0].id));
                  }
#line 6130 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 281:
#line 1693 "grammar19.y" /* yacc.c:1646  */
    {
                    if((yyvsp[-1].node) == 0) {
                      (yyval.node) = NEW_ZARRAY(); /* zero length array*/
                    } else {
                      (yyval.node) = (yyvsp[-1].node);
                    }
                  }
#line 6142 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 282:
#line 1701 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_HASH((yyvsp[-1].node));
                  }
#line 6150 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 283:
#line 1705 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_RETURN(0);
                  }
#line 6158 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 284:
#line 1709 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_yield((yyvsp[-1].node));
                  }
#line 6166 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 285:
#line 1713 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_YIELD(0, Qfalse);
                  }
#line 6174 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 286:
#line 1717 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_YIELD(0, Qfalse);
                  }
#line 6182 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 287:
#line 1720 "grammar19.y" /* yacc.c:1646  */
    {in_defined = 1;}
#line 6188 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 288:
#line 1721 "grammar19.y" /* yacc.c:1646  */
    {
                    in_defined = 0;
                    (yyval.node) = NEW_DEFINED((yyvsp[-1].node));
                  }
#line 6197 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 289:
#line 1726 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[-1].node)), '!');
                  }
#line 6205 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 290:
#line 1730 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = call_uni_op(cond(NEW_NIL()), '!');
                  }
#line 6213 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 291:
#line 1734 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyvsp[0].node)->nd_iter = NEW_FCALL((yyvsp[-1].id), 0);
                    (yyval.node) = (yyvsp[0].node);
                    fixpos((yyvsp[0].node)->nd_iter, (yyvsp[0].node));
                  }
#line 6223 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 293:
#line 1741 "grammar19.y" /* yacc.c:1646  */
    {
                    block_dup_check((yyvsp[-1].node)->nd_args, (yyvsp[0].node));
                    (yyvsp[0].node)->nd_iter = (yyvsp[-1].node);
                    (yyval.node) = (yyvsp[0].node);
                    fixpos((yyval.node), (yyvsp[-1].node));
                  }
#line 6234 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 294:
#line 1748 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[0].node);
                  }
#line 6242 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 295:
#line 1755 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_IF(cond((yyvsp[-4].node)), (yyvsp[-2].node), (yyvsp[-1].node));
                    fixpos((yyval.node), (yyvsp[-4].node));
                  }
#line 6251 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 296:
#line 1763 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_UNLESS(cond((yyvsp[-4].node)), (yyvsp[-2].node), (yyvsp[-1].node));
                    fixpos((yyval.node), (yyvsp[-4].node));
                  }
#line 6260 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 297:
#line 1767 "grammar19.y" /* yacc.c:1646  */
    {COND_PUSH(1);}
#line 6266 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 298:
#line 1767 "grammar19.y" /* yacc.c:1646  */
    {COND_POP();}
#line 6272 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 299:
#line 1770 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_WHILE(cond((yyvsp[-4].node)), (yyvsp[-1].node), 1);
                    fixpos((yyval.node), (yyvsp[-4].node));
                  }
#line 6281 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 300:
#line 1774 "grammar19.y" /* yacc.c:1646  */
    {COND_PUSH(1);}
#line 6287 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 301:
#line 1774 "grammar19.y" /* yacc.c:1646  */
    {COND_POP();}
#line 6293 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 302:
#line 1777 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_UNTIL(cond((yyvsp[-4].node)), (yyvsp[-1].node), 1);
                    fixpos((yyval.node), (yyvsp[-4].node));
                  }
#line 6302 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 303:
#line 1784 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CASE((yyvsp[-3].node), (yyvsp[-1].node));
                    fixpos((yyval.node), (yyvsp[-3].node));
                  }
#line 6311 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 304:
#line 1789 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CASE(0, (yyvsp[-1].node));
                  }
#line 6319 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 305:
#line 1793 "grammar19.y" /* yacc.c:1646  */
    {COND_PUSH(1);}
#line 6325 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 306:
#line 1795 "grammar19.y" /* yacc.c:1646  */
    {COND_POP();}
#line 6331 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 307:
#line 1798 "grammar19.y" /* yacc.c:1646  */
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
                    (yyval.node) = NEW_FOR((yyvsp[-7].node), (yyvsp[-4].node), (yyvsp[-1].node));
                    fixpos((yyval.node), (yyvsp[-7].node));
                  }
#line 6349 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 308:
#line 1812 "grammar19.y" /* yacc.c:1646  */
    {
                    if(in_def || in_single)
                      yy_error("class definition in method body");
                    class_nest++;
                    local_push(0);
                    (yyval.num) = sourceline;
                  }
#line 6361 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 309:
#line 1821 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CLASS((yyvsp[-4].node), (yyvsp[-1].node), (yyvsp[-3].node));
                    nd_set_line((yyval.node), (yyvsp[-2].num));
                    local_pop();
                    class_nest--;
                  }
#line 6372 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 310:
#line 1828 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.num) = in_def;
                    in_def = 0;
                  }
#line 6381 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 311:
#line 1833 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.num) = in_single;
                    in_single = 0;
                    class_nest++;
                    local_push(0);
                  }
#line 6392 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 312:
#line 1841 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_SCLASS((yyvsp[-5].node), (yyvsp[-1].node));
                    fixpos((yyval.node), (yyvsp[-5].node));
                    local_pop();
                    class_nest--;
                    in_def = (yyvsp[-4].num);
                    in_single = (yyvsp[-2].num);
                  }
#line 6405 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 313:
#line 1850 "grammar19.y" /* yacc.c:1646  */
    {
                    if(in_def || in_single)
                      yy_error("module definition in method body");
                    class_nest++;
                    local_push(0);
                    (yyval.num) = sourceline;
                  }
#line 6417 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 314:
#line 1859 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MODULE((yyvsp[-3].node), (yyvsp[-1].node));
                    nd_set_line((yyval.node), (yyvsp[-2].num));
                    local_pop();
                    class_nest--;
                  }
#line 6428 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 315:
#line 1866 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.id) = cur_mid;
                    cur_mid = (yyvsp[0].id);
                    in_def++;
                    local_push(0);
                  }
#line 6439 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 316:
#line 1875 "grammar19.y" /* yacc.c:1646  */
    {
                    NODE* body = remove_begin((yyvsp[-1].node));
                    (yyval.node) = NEW_DEFN((yyvsp[-4].id), (yyvsp[-2].node), body, NOEX_PRIVATE);
                    nd_set_line((yyval.node), (yyvsp[-5].num));
                    local_pop();
                    in_def--;
                    cur_mid = (yyvsp[-3].id);
                  }
#line 6452 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 317:
#line 1883 "grammar19.y" /* yacc.c:1646  */
    {lex_state = EXPR_FNAME;}
#line 6458 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 318:
#line 1884 "grammar19.y" /* yacc.c:1646  */
    {
                    in_single++;
                    lex_state = EXPR_ENDFN; /* force for args */
                    local_push(0);
                  }
#line 6468 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 319:
#line 1892 "grammar19.y" /* yacc.c:1646  */
    {
                    NODE* body = remove_begin((yyvsp[-1].node));
                    (yyval.node) = NEW_DEFS((yyvsp[-7].node), (yyvsp[-4].id), (yyvsp[-2].node), body);
                    nd_set_line((yyval.node), (yyvsp[-8].num));
                    local_pop();
                    in_single--;
                  }
#line 6480 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 320:
#line 1900 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_BREAK(0);
                  }
#line 6488 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 321:
#line 1904 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_NEXT(0);
                  }
#line 6496 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 322:
#line 1908 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_REDO();
                  }
#line 6504 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 323:
#line 1912 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_RETRY();
                  }
#line 6512 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 324:
#line 1918 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    (yyval.node) = (yyvsp[0].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  }
#line 6522 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 325:
#line 1926 "grammar19.y" /* yacc.c:1646  */
    {
                    token_info_push("begin");
                  }
#line 6530 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 326:
#line 1932 "grammar19.y" /* yacc.c:1646  */
    {
                    token_info_push("if");
                  }
#line 6538 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 327:
#line 1938 "grammar19.y" /* yacc.c:1646  */
    {
                    token_info_push("unless");
                  }
#line 6546 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 328:
#line 1944 "grammar19.y" /* yacc.c:1646  */
    {
                    token_info_push("while");
                  }
#line 6554 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 329:
#line 1950 "grammar19.y" /* yacc.c:1646  */
    {
                    token_info_push("until");
                  }
#line 6562 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 330:
#line 1956 "grammar19.y" /* yacc.c:1646  */
    {
                    token_info_push("case");
                  }
#line 6570 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 331:
#line 1962 "grammar19.y" /* yacc.c:1646  */
    {
                    token_info_push("for");
                  }
#line 6578 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 332:
#line 1968 "grammar19.y" /* yacc.c:1646  */
    {
                    token_info_push("class");
                  }
#line 6586 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 333:
#line 1974 "grammar19.y" /* yacc.c:1646  */
    {
                    token_info_push("module");
                  }
#line 6594 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 334:
#line 1980 "grammar19.y" /* yacc.c:1646  */
    {
                    token_info_push("def");
                    (yyval.num) = sourceline;
                  }
#line 6603 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 335:
#line 1987 "grammar19.y" /* yacc.c:1646  */
    {
                    token_info_pop("end");
                  }
#line 6611 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 342:
#line 2005 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_IF(cond((yyvsp[-3].node)), (yyvsp[-1].node), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-3].node));
                  }
#line 6620 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 344:
#line 2013 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[0].node);
                  }
#line 6628 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 347:
#line 2023 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = assignable((yyvsp[0].id), 0);
                  }
#line 6636 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 348:
#line 2027 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 6644 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 349:
#line 2033 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_LIST((yyvsp[0].node));
                  }
#line 6652 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 350:
#line 2037 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = list_append((yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 6660 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 351:
#line 2043 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN((yyvsp[0].node), 0);
                  }
#line 6668 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 352:
#line 2047 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = assignable((yyvsp[0].id), 0);
                    (yyval.node) = NEW_MASGN((yyvsp[-3].node), (yyval.node));
                  }
#line 6677 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 353:
#line 2052 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = assignable((yyvsp[-2].id), 0);
                    (yyval.node) = NEW_MASGN((yyvsp[-5].node), NEW_POSTARG((yyval.node), (yyvsp[0].node)));
                  }
#line 6686 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 354:
#line 2057 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN((yyvsp[-2].node), -1);
                  }
#line 6694 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 355:
#line 2061 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN((yyvsp[-4].node), NEW_POSTARG(-1, (yyvsp[0].node)));
                  }
#line 6702 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 356:
#line 2065 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = assignable((yyvsp[0].id), 0);
                    (yyval.node) = NEW_MASGN(0, (yyval.node));
                  }
#line 6711 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 357:
#line 2070 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = assignable((yyvsp[-2].id), 0);
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG((yyval.node), (yyvsp[0].node)));
                  }
#line 6720 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 358:
#line 2075 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN(0, -1);
                  }
#line 6728 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 359:
#line 2079 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG(-1, (yyvsp[0].node)));
                  }
#line 6736 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 360:
#line 2085 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].id), 0, (yyvsp[0].id));
                  }
#line 6744 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 361:
#line 2089 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].id), (yyvsp[-1].node), (yyvsp[0].id));
                  }
#line 6752 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 362:
#line 2093 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-3].node), (yyvsp[-1].node), 0, 0, (yyvsp[0].id));
                  }
#line 6760 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 363:
#line 2097 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-5].node), (yyvsp[-3].node), 0, (yyvsp[-1].node), (yyvsp[0].id));
                  }
#line 6768 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 364:
#line 2101 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-3].node), 0, (yyvsp[-1].id), 0, (yyvsp[0].id));
                  }
#line 6776 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 365:
#line 2105 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-1].node), 0, 1, 0, 0);
                  }
#line 6784 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 366:
#line 2109 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-5].node), 0, (yyvsp[-3].id), (yyvsp[-1].node), (yyvsp[0].id));
                  }
#line 6792 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 367:
#line 2113 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-1].node), 0, 0, 0, (yyvsp[0].id));
                  }
#line 6800 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 368:
#line 2117 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, (yyvsp[-3].node), (yyvsp[-1].id), 0, (yyvsp[0].id));
                  }
#line 6808 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 369:
#line 2121 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, (yyvsp[-5].node), (yyvsp[-3].id), (yyvsp[-1].node), (yyvsp[0].id));
                  }
#line 6816 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 370:
#line 2125 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, (yyvsp[-1].node), 0, 0, (yyvsp[0].id));
                  }
#line 6824 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 371:
#line 2129 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, (yyvsp[-3].node), 0, (yyvsp[-1].node), (yyvsp[0].id));
                  }
#line 6832 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 372:
#line 2133 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[-1].id), 0, (yyvsp[0].id));
                  }
#line 6840 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 373:
#line 2137 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[-3].id), (yyvsp[-1].node), (yyvsp[0].id));
                  }
#line 6848 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 374:
#line 2141 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, 0, 0, 0, (yyvsp[0].id));
                  }
#line 6856 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 376:
#line 2148 "grammar19.y" /* yacc.c:1646  */
    {
                    command_start = TRUE;
                  }
#line 6864 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 377:
#line 2154 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node) ? NEW_ARGS_AUX(0,(yyvsp[-1].node)) : 0;
                  }
#line 6872 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 378:
#line 2158 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = 0;
                  }
#line 6880 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 379:
#line 2162 "grammar19.y" /* yacc.c:1646  */
    {
                    if((yyvsp[-1].node)) {
                      (yyval.node) = NEW_ARGS_AUX((yyvsp[-2].node), (yyvsp[-1].node));
                    } else {
                      (yyval.node) = (yyvsp[-2].node);
                    }
                  }
#line 6892 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 380:
#line 2172 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = 0;
                  }
#line 6900 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 381:
#line 2176 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[0].node);
                  }
#line 6908 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 382:
#line 2182 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_LIST((yyvsp[0].node));
                  }
#line 6916 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 383:
#line 2186 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = list_append((yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 6924 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 384:
#line 2192 "grammar19.y" /* yacc.c:1646  */
    {
                    new_bv(get_id((yyvsp[0].id)));
                    (yyval.node) = NEW_LIT(ID2SYM(get_id((yyvsp[0].id))));
                  }
#line 6933 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 385:
#line 2197 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = 0;
                  }
#line 6941 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 386:
#line 2202 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.vars) = bv_push();
                  }
#line 6949 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 387:
#line 2205 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.num) = lpar_beg;
                    lpar_beg = ++paren_nest;
                  }
#line 6958 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 388:
#line 2211 "grammar19.y" /* yacc.c:1646  */
    {
                    lpar_beg = (yyvsp[-2].num);
                    (yyval.node) = (yyvsp[-1].node);
                    (yyval.node)->nd_body = NEW_SCOPE((yyvsp[-1].node)->nd_head, (yyvsp[0].node));
                    bv_pop((yyvsp[-3].vars));
                  }
#line 6969 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 389:
#line 2220 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_LAMBDA((yyvsp[-2].node));
                  }
#line 6977 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 390:
#line 2224 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_LAMBDA((yyvsp[0].node));
                  }
#line 6985 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 391:
#line 2230 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 6993 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 392:
#line 2234 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 7001 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 393:
#line 2240 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyvsp[0].vars) = bv_push();
                    (yyval.num) = sourceline;
                  }
#line 7010 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 394:
#line 2247 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_ITER((yyvsp[-2].node), (yyvsp[-1].node));
                    nd_set_line((yyval.node), (yyvsp[-3].num));
                    bv_pop((yyvsp[-4].vars));
                  }
#line 7020 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 395:
#line 2255 "grammar19.y" /* yacc.c:1646  */
    {
                    if(nd_type((yyvsp[-1].node)) == NODE_YIELD) {
                      compile_error("block given to yield");
                    } else {
                      block_dup_check((yyvsp[-1].node)->nd_args, (yyvsp[0].node));
                    }
                    (yyvsp[0].node)->nd_iter = (yyvsp[-1].node);
                    (yyval.node) = (yyvsp[0].node);
                    fixpos((yyval.node), (yyvsp[-1].node));
                  }
#line 7035 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 396:
#line 2266 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CALL((yyvsp[-3].node), (yyvsp[-1].id), (yyvsp[0].node));
                  }
#line 7043 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 397:
#line 2270 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CALL((yyvsp[-3].node), (yyvsp[-1].id), (yyvsp[0].node));
                  }
#line 7051 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 398:
#line 2276 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_FCALL((yyvsp[-1].id), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[0].node));
                  }
#line 7060 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 399:
#line 2281 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CALL((yyvsp[-3].node), (yyvsp[-1].id), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-3].node));
                  }
#line 7069 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 400:
#line 2286 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CALL((yyvsp[-3].node), (yyvsp[-1].id), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-3].node));
                  }
#line 7078 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 401:
#line 2291 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CALL((yyvsp[-2].node), (yyvsp[0].id), 0);
                  }
#line 7086 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 402:
#line 2295 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CALL((yyvsp[-2].node), parser_intern("call"), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-2].node));
                  }
#line 7095 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 403:
#line 2300 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_CALL((yyvsp[-2].node), parser_intern("call"), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-2].node));
                  }
#line 7104 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 404:
#line 2305 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_SUPER((yyvsp[0].node));
                  }
#line 7112 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 405:
#line 2309 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_ZSUPER();
                  }
#line 7120 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 406:
#line 2313 "grammar19.y" /* yacc.c:1646  */
    {
                    if((yyvsp[-3].node) && nd_type((yyvsp[-3].node)) == NODE_SELF) {
                      (yyval.node) = NEW_FCALL(tAREF, (yyvsp[-1].node));
                    } else {
                      (yyval.node) = NEW_CALL((yyvsp[-3].node), tAREF, (yyvsp[-1].node));
                    }
                    fixpos((yyval.node), (yyvsp[-3].node));
                  }
#line 7133 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 407:
#line 2324 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyvsp[0].vars) = bv_push();
                    (yyval.num) = sourceline;
                  }
#line 7142 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 408:
#line 2330 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_ITER((yyvsp[-2].node), (yyvsp[-1].node));
                    nd_set_line((yyval.node), (yyvsp[-3].num));
                    bv_pop((yyvsp[-4].vars));
                  }
#line 7152 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 409:
#line 2336 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyvsp[0].vars) = bv_push();
                    (yyval.num) = sourceline;
                  }
#line 7161 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 410:
#line 2342 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_ITER((yyvsp[-2].node), (yyvsp[-1].node));
                    nd_set_line((yyval.node), (yyvsp[-3].num));
                    bv_pop((yyvsp[-4].vars));
                  }
#line 7171 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 411:
#line 2352 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_WHEN((yyvsp[-3].node), (yyvsp[-1].node), (yyvsp[0].node));
                  }
#line 7179 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 414:
#line 2364 "grammar19.y" /* yacc.c:1646  */
    {
                    if((yyvsp[-3].node)) {
                      /* TODO NEW_ERRINFO() */
                      (yyvsp[-3].node) = node_assign((yyvsp[-3].node), NEW_GVAR(parser_intern("$!")));
                      (yyvsp[-1].node) = block_append((yyvsp[-3].node), (yyvsp[-1].node));
                    }
                    (yyval.node) = NEW_RESBODY((yyvsp[-4].node), (yyvsp[-1].node), (yyvsp[0].node));
                    fixpos((yyval.node), (yyvsp[-4].node) ? (yyvsp[-4].node) : (yyvsp[-1].node));
                  }
#line 7193 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 416:
#line 2377 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_LIST((yyvsp[0].node));
                  }
#line 7201 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 417:
#line 2381 "grammar19.y" /* yacc.c:1646  */
    {
                    if(!((yyval.node) = splat_array((yyvsp[0].node)))) (yyval.node) = (yyvsp[0].node);
                  }
#line 7209 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 419:
#line 2388 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[0].node);
                  }
#line 7217 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 421:
#line 2395 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[0].node);
                  }
#line 7225 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 424:
#line 2403 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_LIT(ID2SYM((yyvsp[0].id)));
                  }
#line 7233 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 426:
#line 2410 "grammar19.y" /* yacc.c:1646  */
    {
                    NODE *node = (yyvsp[0].node);
                    if(!node) {
                      node = NEW_STR(STR_NEW0());
                    } else {
                      node = evstr2dstr(node);
                    }
                    (yyval.node) = node;
                  }
#line 7247 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 429:
#line 2424 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = literal_concat((yyvsp[-1].node), (yyvsp[0].node));
                  }
#line 7255 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 430:
#line 2430 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 7263 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 431:
#line 2436 "grammar19.y" /* yacc.c:1646  */
    {
                    NODE *node = (yyvsp[-1].node);
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
                    (yyval.node) = node;
                  }
#line 7287 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 432:
#line 2458 "grammar19.y" /* yacc.c:1646  */
    {
                    intptr_t options = (yyvsp[0].num);
                    NODE *node = (yyvsp[-1].node);
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
                    (yyval.node) = node;
                  }
#line 7349 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 433:
#line 2518 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_ZARRAY();
                  }
#line 7357 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 434:
#line 2522 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 7365 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 435:
#line 2528 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = 0;
                  }
#line 7373 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 436:
#line 2532 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = list_append((yyvsp[-2].node), evstr2dstr((yyvsp[-1].node)));
                  }
#line 7381 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 438:
#line 2539 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = literal_concat((yyvsp[-1].node), (yyvsp[0].node));
                  }
#line 7389 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 439:
#line 2545 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = NEW_ZARRAY();
                  }
#line 7397 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 440:
#line 2549 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 7405 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 441:
#line 2555 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = 0;
                  }
#line 7413 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 442:
#line 2559 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = list_append((yyvsp[-2].node), (yyvsp[-1].node));
                  }
#line 7421 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 443:
#line 2565 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = 0;
                  }
#line 7429 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 444:
#line 2569 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = literal_concat((yyvsp[-1].node), (yyvsp[0].node));
                  }
#line 7437 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 445:
#line 2575 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = 0;
                  }
#line 7445 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 446:
#line 2579 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = literal_concat((yyvsp[-1].node), (yyvsp[0].node));
                  }
#line 7453 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 447:
#line 2585 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = 0;
                  }
#line 7461 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 448:
#line 2589 "grammar19.y" /* yacc.c:1646  */
    {
                    NODE *head = (yyvsp[-1].node), *tail = (yyvsp[0].node);
                    if(!head) {
                      (yyval.node) = tail;
                    } else if(!tail) {
                      (yyval.node) = head;
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
                      (yyval.node) = list_append(head, tail);
                    }
                  }
#line 7486 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 450:
#line 2613 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = lex_strterm;
                    lex_strterm = 0;
                    lex_state = EXPR_BEG;
                  }
#line 7496 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 451:
#line 2619 "grammar19.y" /* yacc.c:1646  */
    {
                    lex_strterm = (yyvsp[-1].node);
                    (yyval.node) = NEW_EVSTR((yyvsp[0].node));
                  }
#line 7505 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 452:
#line 2624 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyvsp[0].val) = cond_stack;
                    (yyval.val) = cmdarg_stack;
                    cond_stack = 0;
                    cmdarg_stack = 0;
                  }
#line 7516 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 453:
#line 2630 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = lex_strterm;
                    lex_strterm = 0;
                    lex_state = EXPR_BEG;
                  }
#line 7526 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 454:
#line 2636 "grammar19.y" /* yacc.c:1646  */
    {
                    cond_stack = (yyvsp[-4].val);
                    cmdarg_stack = (yyvsp[-3].val);
                    lex_strterm = (yyvsp[-2].node);

                    if((yyvsp[-1].node)) (yyvsp[-1].node)->flags &= ~NODE_FL_NEWLINE;
                    (yyval.node) = new_evstr((yyvsp[-1].node));
                  }
#line 7539 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 455:
#line 2646 "grammar19.y" /* yacc.c:1646  */
    {(yyval.node) = NEW_GVAR((yyvsp[0].id));}
#line 7545 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 456:
#line 2647 "grammar19.y" /* yacc.c:1646  */
    {(yyval.node) = NEW_IVAR((yyvsp[0].id));}
#line 7551 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 457:
#line 2648 "grammar19.y" /* yacc.c:1646  */
    {(yyval.node) = NEW_CVAR((yyvsp[0].id));}
#line 7557 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 459:
#line 2653 "grammar19.y" /* yacc.c:1646  */
    {
                    lex_state = EXPR_END;
                    (yyval.id) = (yyvsp[0].id);
                  }
#line 7566 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 464:
#line 2666 "grammar19.y" /* yacc.c:1646  */
    {
                    lex_state = EXPR_END;
                    if(!((yyval.node) = (yyvsp[-1].node))) {
                      (yyval.node) = NEW_LIT(ID2SYM(parser_intern("")));
                    } else {
                      VALUE lit;

                      switch(nd_type((yyval.node))) {
                      case NODE_DSTR:
                        nd_set_type((yyval.node), NODE_DSYM);
                        break;
                      case NODE_STR:
                        lit = (yyval.node)->nd_lit;
                        (yyval.node)->nd_lit = ID2SYM(parser_intern_str(lit));
                        nd_set_type((yyval.node), NODE_LIT);
                        break;
                      default:
                        (yyval.node) = NEW_NODE(NODE_DSYM, REF(STR_NEW0()), 1, NEW_LIST((yyval.node)));
                        break;
                      }
                    }
                  }
#line 7593 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 467:
#line 2693 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = negate_lit((yyvsp[0].node));
                  }
#line 7601 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 468:
#line 2697 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = negate_lit((yyvsp[0].node));
                  }
#line 7609 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 474:
#line 2707 "grammar19.y" /* yacc.c:1646  */
    {(yyval.id) = keyword_nil;}
#line 7615 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 475:
#line 2708 "grammar19.y" /* yacc.c:1646  */
    {(yyval.id) = keyword_self;}
#line 7621 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 476:
#line 2709 "grammar19.y" /* yacc.c:1646  */
    {(yyval.id) = keyword_true;}
#line 7627 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 477:
#line 2710 "grammar19.y" /* yacc.c:1646  */
    {(yyval.id) = keyword_false;}
#line 7633 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 478:
#line 2711 "grammar19.y" /* yacc.c:1646  */
    {(yyval.id) = keyword__FILE__;}
#line 7639 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 479:
#line 2712 "grammar19.y" /* yacc.c:1646  */
    {(yyval.id) = keyword__LINE__;}
#line 7645 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 480:
#line 2713 "grammar19.y" /* yacc.c:1646  */
    {(yyval.id) = keyword__ENCODING__;}
#line 7651 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 481:
#line 2717 "grammar19.y" /* yacc.c:1646  */
    {
                    if(!((yyval.node) = gettable((yyvsp[0].id)))) {
                      (yyval.node) = NEW_BEGIN(0);
                    }
                  }
#line 7661 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 482:
#line 2725 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = assignable((yyvsp[0].id), 0);
                  }
#line 7669 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 485:
#line 2735 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = 0;
                  }
#line 7677 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 486:
#line 2739 "grammar19.y" /* yacc.c:1646  */
    {
                    lex_state = EXPR_BEG;
                  }
#line 7685 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 487:
#line 2743 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 7693 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 488:
#line 2747 "grammar19.y" /* yacc.c:1646  */
    {
                    yyerrok;
                    (yyval.node) = 0;
                  }
#line 7702 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 489:
#line 2754 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                    lex_state = EXPR_BEG;
                    command_start = TRUE;
                  }
#line 7712 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 490:
#line 2760 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                    lex_state = EXPR_BEG;
                    command_start = TRUE;
                  }
#line 7722 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 491:
#line 2768 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].id), 0, (yyvsp[0].id));
                  }
#line 7730 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 492:
#line 2772 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].id), (yyvsp[-1].node), (yyvsp[0].id));
                  }
#line 7738 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 493:
#line 2776 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-3].node), (yyvsp[-1].node), 0, 0, (yyvsp[0].id));
                  }
#line 7746 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 494:
#line 2780 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-5].node), (yyvsp[-3].node), 0, (yyvsp[-1].node), (yyvsp[0].id));
                  }
#line 7754 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 495:
#line 2784 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-3].node), 0, (yyvsp[-1].id), 0, (yyvsp[0].id));
                  }
#line 7762 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 496:
#line 2788 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-5].node), 0, (yyvsp[-3].id), (yyvsp[-1].node), (yyvsp[0].id));
                  }
#line 7770 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 497:
#line 2792 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args((yyvsp[-1].node), 0, 0, 0, (yyvsp[0].id));
                  }
#line 7778 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 498:
#line 2796 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, (yyvsp[-3].node), (yyvsp[-1].id), 0, (yyvsp[0].id));
                  }
#line 7786 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 499:
#line 2800 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, (yyvsp[-5].node), (yyvsp[-3].id), (yyvsp[-1].node), (yyvsp[0].id));
                  }
#line 7794 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 500:
#line 2804 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, (yyvsp[-1].node), 0, 0, (yyvsp[0].id));
                  }
#line 7802 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 501:
#line 2808 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, (yyvsp[-3].node), 0, (yyvsp[-1].node), (yyvsp[0].id));
                  }
#line 7810 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 502:
#line 2812 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[-1].id), 0, (yyvsp[0].id));
                  }
#line 7818 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 503:
#line 2816 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[-3].id), (yyvsp[-1].node), (yyvsp[0].id));
                  }
#line 7826 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 504:
#line 2820 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, 0, 0, 0, (yyvsp[0].id));
                  }
#line 7834 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 505:
#line 2824 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = new_args(0, 0, 0, 0, 0);
                  }
#line 7842 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 506:
#line 2830 "grammar19.y" /* yacc.c:1646  */
    {
                    yy_error("formal argument cannot be a constant");
                    (yyval.id) = 0;
                  }
#line 7851 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 507:
#line 2835 "grammar19.y" /* yacc.c:1646  */
    {
                    yy_error("formal argument cannot be an instance variable");
                    (yyval.id) = 0;
                  }
#line 7860 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 508:
#line 2840 "grammar19.y" /* yacc.c:1646  */
    {
                    yy_error("formal argument cannot be a global variable");
                    (yyval.id) = 0;
                  }
#line 7869 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 509:
#line 2845 "grammar19.y" /* yacc.c:1646  */
    {
                    yy_error("formal argument cannot be a class variable");
                    (yyval.id) = 0;
                  }
#line 7878 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 511:
#line 2853 "grammar19.y" /* yacc.c:1646  */
    {
                    formal_argument(get_id((yyvsp[0].id)));
                    (yyval.id) = (yyvsp[0].id);
                  }
#line 7887 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 512:
#line 2860 "grammar19.y" /* yacc.c:1646  */
    {
                    arg_var(get_id((yyvsp[0].id)));
                    (yyval.node) = NEW_ARGS_AUX((yyvsp[0].id), 1);
                  }
#line 7896 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 513:
#line 2865 "grammar19.y" /* yacc.c:1646  */
    {
                    ID tid = internal_id();
                    arg_var(tid);
                    (yyvsp[-1].node)->nd_value = NEW_LVAR(tid);
                    (yyval.node) = NEW_ARGS_AUX(tid, 1);
                    (yyval.node)->nd_next = (yyvsp[-1].node);
                  }
#line 7908 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 515:
#line 2876 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-2].node);
                    (yyval.node)->nd_plen++;
                    (yyval.node)->nd_next = block_append((yyval.node)->nd_next, (yyvsp[0].node)->nd_next);
                  }
#line 7918 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 516:
#line 2884 "grammar19.y" /* yacc.c:1646  */
    {
                    arg_var(formal_argument(get_id((yyvsp[-2].id))));
                    (yyval.node) = assignable((yyvsp[-2].id), (yyvsp[0].node));
                    (yyval.node) = NEW_OPT_ARG(0, (yyval.node));
                  }
#line 7928 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 517:
#line 2892 "grammar19.y" /* yacc.c:1646  */
    {
                    arg_var(formal_argument(get_id((yyvsp[-2].id))));
                    (yyval.node) = assignable((yyvsp[-2].id), (yyvsp[0].node));
                    (yyval.node) = NEW_OPT_ARG(0, (yyval.node));
                  }
#line 7938 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 518:
#line 2900 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[0].node);
                  }
#line 7946 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 519:
#line 2904 "grammar19.y" /* yacc.c:1646  */
    {
                    NODE *opts = (yyvsp[-2].node);
                    while(opts->nd_next) {
                      opts = opts->nd_next;
                    }
                    opts->nd_next = (yyvsp[0].node);
                    (yyval.node) = (yyvsp[-2].node);
                  }
#line 7959 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 520:
#line 2915 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[0].node);
                  }
#line 7967 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 521:
#line 2919 "grammar19.y" /* yacc.c:1646  */
    {
                    NODE *opts = (yyvsp[-2].node);
                    while(opts->nd_next) {
                      opts = opts->nd_next;
                    }
                    opts->nd_next = (yyvsp[0].node);
                    (yyval.node) = (yyvsp[-2].node);
                  }
#line 7980 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 524:
#line 2934 "grammar19.y" /* yacc.c:1646  */
    {
                    if(!is_local_id((yyvsp[0].id))) {
                      yy_error("rest argument must be local variable");
                    }
                    arg_var(shadowing_lvar(get_id((yyvsp[0].id))));
                    (yyval.id) = (yyvsp[0].id);
                  }
#line 7992 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 525:
#line 2942 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.id) = internal_id();
                    arg_var((yyval.id));
                  }
#line 8001 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 528:
#line 2953 "grammar19.y" /* yacc.c:1646  */
    {
                    if(!is_local_id((yyvsp[0].id)))
                      yy_error("block argument must be local variable");
                    else if(!in_block() && local_id((yyvsp[0].id)))
                      yy_error("duplicate block argument name");
                    arg_var(shadowing_lvar(get_id((yyvsp[0].id))));
                    (yyval.id) = (yyvsp[0].id);
                  }
#line 8014 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 529:
#line 2964 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.id) = (yyvsp[0].id);
                  }
#line 8022 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 530:
#line 2968 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.id) = 0;
                  }
#line 8030 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 531:
#line 2974 "grammar19.y" /* yacc.c:1646  */
    {
                    value_expr((yyvsp[0].node));
                    (yyval.node) = (yyvsp[0].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  }
#line 8040 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 532:
#line 2979 "grammar19.y" /* yacc.c:1646  */
    {lex_state = EXPR_BEG;}
#line 8046 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 533:
#line 2980 "grammar19.y" /* yacc.c:1646  */
    {
                    if((yyvsp[-1].node) == 0) {
                      yy_error("can't define singleton method for ().");
                    } else {
                      switch(nd_type((yyvsp[-1].node))) {
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
                        value_expr((yyvsp[-1].node));
                        break;
                      }
                    }
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 8072 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 535:
#line 3005 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = (yyvsp[-1].node);
                  }
#line 8080 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 537:
#line 3012 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = list_concat((yyvsp[-2].node), (yyvsp[0].node));
                  }
#line 8088 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 538:
#line 3018 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = list_append(NEW_LIST((yyvsp[-2].node)), (yyvsp[0].node));
                  }
#line 8096 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 539:
#line 3022 "grammar19.y" /* yacc.c:1646  */
    {
                    (yyval.node) = list_append(NEW_LIST(NEW_LIT(ID2SYM((yyvsp[-1].id)))), (yyvsp[0].node));
                  }
#line 8104 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 561:
#line 3066 "grammar19.y" /* yacc.c:1646  */
    {yyerrok;}
#line 8110 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 564:
#line 3071 "grammar19.y" /* yacc.c:1646  */
    {yyerrok;}
#line 8116 "grammar19.cpp" /* yacc.c:1646  */
    break;

  case 565:
#line 3074 "grammar19.y" /* yacc.c:1646  */
    {(yyval.node) = 0;}
#line 8122 "grammar19.cpp" /* yacc.c:1646  */
    break;


#line 8126 "grammar19.cpp" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (parser_state, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (parser_state, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, parser_state);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, parser_state);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (parser_state, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, parser_state);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, parser_state);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 3076 "grammar19.y" /* yacc.c:1906  */


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
  return (in_block() && bv_defined(id)) || local_id(id);
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
      return -1;
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

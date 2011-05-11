/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
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
/* Tokens.  */
#define keyword_class 258
#define keyword_module 259
#define keyword_def 260
#define keyword_undef 261
#define keyword_begin 262
#define keyword_rescue 263
#define keyword_ensure 264
#define keyword_end 265
#define keyword_if 266
#define keyword_unless 267
#define keyword_then 268
#define keyword_elsif 269
#define keyword_else 270
#define keyword_case 271
#define keyword_when 272
#define keyword_while 273
#define keyword_until 274
#define keyword_for 275
#define keyword_break 276
#define keyword_next 277
#define keyword_redo 278
#define keyword_retry 279
#define keyword_in 280
#define keyword_do 281
#define keyword_do_cond 282
#define keyword_do_block 283
#define keyword_do_LAMBDA 284
#define keyword_return 285
#define keyword_yield 286
#define keyword_super 287
#define keyword_self 288
#define keyword_nil 289
#define keyword_true 290
#define keyword_false 291
#define keyword_and 292
#define keyword_or 293
#define keyword_not 294
#define modifier_if 295
#define modifier_unless 296
#define modifier_while 297
#define modifier_until 298
#define modifier_rescue 299
#define keyword_alias 300
#define keyword_defined 301
#define keyword_BEGIN 302
#define keyword_END 303
#define keyword__LINE__ 304
#define keyword__FILE__ 305
#define keyword__ENCODING__ 306
#define tIDENTIFIER 307
#define tFID 308
#define tGVAR 309
#define tIVAR 310
#define tCONSTANT 311
#define tCVAR 312
#define tLABEL 313
#define tINTEGER 314
#define tFLOAT 315
#define tSTRING_CONTENT 316
#define tCHAR 317
#define tNTH_REF 318
#define tBACK_REF 319
#define tREGEXP_END 320
#define tUPLUS 321
#define tUMINUS 322
#define tPOW 323
#define tCMP 324
#define tEQ 325
#define tEQQ 326
#define tNEQ 327
#define tGEQ 328
#define tLEQ 329
#define tANDOP 330
#define tOROP 331
#define tMATCH 332
#define tNMATCH 333
#define tDOT2 334
#define tDOT3 335
#define tAREF 336
#define tASET 337
#define tLSHFT 338
#define tRSHFT 339
#define tCOLON2 340
#define tCOLON3 341
#define tOP_ASGN 342
#define tASSOC 343
#define tLPAREN 344
#define tLPAREN_ARG 345
#define tRPAREN 346
#define tLBRACK 347
#define tLBRACE 348
#define tLBRACE_ARG 349
#define tSTAR 350
#define tAMPER 351
#define tLAMBDA 352
#define tSYMBEG 353
#define tSTRING_BEG 354
#define tXSTRING_BEG 355
#define tREGEXP_BEG 356
#define tWORDS_BEG 357
#define tQWORDS_BEG 358
#define tSTRING_DBEG 359
#define tSTRING_DVAR 360
#define tSTRING_END 361
#define tLAMBEG 362
#define tLOWEST 363
#define tUMINUS_NUM 364
#define tLAST_TOKEN 365




/* Copy the first part of user declarations.  */
#line 13 "grammar19.y"


#define YYDEBUG 1
#define YYERROR_VERBOSE 1

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "ruby.h"

#define RBX_GRAMMAR_19  1

#include "internal.hpp"
#include "visitor.hpp"
#include "symbols.hpp"
#include "local_state.hpp"

namespace melbourne {

namespace grammar19 {

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

#define ismbchar(c) (0)
#define mbclen(c) (1)

#define string_new(ptr, len) blk2bstr(ptr, len)
#define string_new2(ptr) cstr2bstr(ptr)

long mel_sourceline;
static char *mel_sourcefile;

#define ruby_sourceline mel_sourceline
#define ruby_sourcefile mel_sourcefile

static void parser_prepare(rb_parser_state*);
static int parser_yyerror(rb_parser_state*, const char *);
#define yy_error(msg)   parser_yyerror(parser_state, msg)
#define yyerror         parser_yyerror

#define YYLEX_PARAM parser_state

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
static NODE *parser_new_evstr(rb_parser_state*, NODE*);
static NODE *parser_evstr2dstr(rb_parser_state*, NODE*);
static NODE *parser_call_bin_op(rb_parser_state*, NODE*, QUID, NODE*);
static NODE *parser_call_uni_op(rb_parser_state*, NODE*, QUID);
static NODE *parser_new_args(rb_parser_state*, NODE*, NODE*, QUID, NODE*, QUID);
static NODE *splat_array(NODE*);

/* static NODE *negate_lit(NODE*); */
static NODE *parser_ret_args(rb_parser_state*, NODE*);
static NODE *arg_blk_pass(NODE*,NODE*);
static NODE *new_call(rb_parser_state*,NODE*,QUID,NODE*);
static NODE *new_fcall(rb_parser_state*,QUID,NODE*);
static NODE *parser_new_super(rb_parser_state*, NODE*);
static NODE *parser_new_yield(rb_parser_state*, NODE*);

static NODE *mel_gettable(rb_parser_state*,QUID);
#define gettable(i) mel_gettable((rb_parser_state*)parser_state, i)
static NODE *parser_assignable(rb_parser_state*, QUID, NODE*);
static QUID parser_formal_argument(rb_parser_state*, QUID);
static QUID parser_shadowing_lvar(rb_parser_state*, QUID);
static int parser_lvar_defined(rb_parser_state*, QUID);
static void parser_new_bv(rb_parser_state*, QUID);
static NODE *parser_aryset(rb_parser_state*, NODE*, NODE*);
static NODE *parser_attrset(rb_parser_state*, NODE*, QUID);
static void rb_parser_backref_error(rb_parser_state*, NODE*);
static NODE *parser_node_assign(rb_parser_state*, NODE*, NODE*);

static NODE *parser_match_op(rb_parser_state*, NODE*, NODE*);
static int parser_arg_var(rb_parser_state*, QUID);
static int parser_local_var(rb_parser_state*, QUID);
static QUID parser_internal_id(rb_parser_state*);

static void parser_local_push(rb_parser_state*, int cnt);
static void parser_local_pop(rb_parser_state*);
static intptr_t parser_local_cnt(rb_parser_state*, QUID);
static int  parser_local_id(rb_parser_state*, QUID);
static QUID  *parser_local_tbl(rb_parser_state*);
static QUID   convert_op(QUID id);

#define QUID2SYM(x)   (x)

#define SHOW_PARSER_WARNS 0

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

static void rb_compile_warning(rb_parser_state* parser_state, const char *fmt, ...) {
  // TODO: only in verbose mode.
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

#define PUSH_LINE(which) push_start_line((rb_parser_state*)parser_state, ruby_sourceline, which)

void pop_start_line(rb_parser_state* parser_state) {
  start_lines->pop_back();
}

#define POP_LINE() pop_start_line((rb_parser_state*)parser_state)

static QUID rb_parser_sym(const char *name);
static QUID rb_id_attrset(QUID);

static int scan_oct(const char *start, size_t len, size_t *retlen);
static int scan_hex(const char *start, size_t len, size_t *retlen);

static void parser_reset_block(rb_parser_state* parser_state);
static NODE *parser_extract_block_vars(rb_parser_state* parser_state, NODE* node, var_table vars);

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
#define reset_block()             parser_reset_block(parser_state)
#define extract_block_vars(a, b)  parser_extract_block_vars(parser_state, a, b)
#define ret_args(n)               parser_ret_args(parser_state, n)
#define assignable(a, b)          parser_assignable(parser_state, a, b)
#define formal_argument(n)        parser_formal_argument(parser_state, n)
#define lvar_defined(n)           parser_lvar_defined(parser_state, n)
#define shadowing_lvar(n)         parser_shadowing_lvar(parser_state, n)
#define new_bv(n)                 parser_new_bv(parser_state, n)
#define aryset(a, b)              parser_aryset(parser_state, a, b)
#define attrset(a, b)             parser_attrset(parser_state, a, b)
#define match_op(a, b)            parser_match_op(parser_state, a, b)
#define new_yield(n)              parser_new_yield(parser_state, n)
#define new_super(n)              parser_new_super(parser_state, n)
#define evstr2dstr(n)             parser_evstr2dstr(parser_state, n)
#define literal_concat(a, b)      parser_literal_concat(parser_state, a, b)
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
#define local_cnt(i)              parser_local_cnt(parser_state, i)

#define rb_warning0(fmt)          rb_compile_warning(parser_state, fmt)
#define rb_warningS(fmt,a)        rb_compile_warning(parser_state, fmt, a)

/* TODO */
#define CONST_ID(x, y)            ((void)0)

#ifndef RE_OPTION_IGNORECASE
#define RE_OPTION_IGNORECASE         (1L)
#endif

#ifndef RE_OPTION_EXTENDED
#define RE_OPTION_EXTENDED           (2L)
#endif

#ifndef RE_OPTION_MULTILINE
#define RE_OPTION_MULTILINE          (4L)
#endif

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
#define nd_nest u3.cnt

#define UTF8_ENC()            (parser_state->utf8 ? parser_state->utf8 : \
                                (parser_state->utf8 = rb_utf8_encoding()))
#define STR_NEW0()            rb_str_new(0, 0)
#define STR_NEW3(p,n,e,func)  parser_str_new((p), (n), (e), (func), parser_state->enc)
#define ENC_SINGLE(cr)        ((cr)==ENC_CODERANGE_7BIT)
#define TOK_INTERN(mb)        rb_parser_sym(tok())

#define NEW_BLOCK_VAR(b, v) NEW_NODE(NODE_BLOCK_PASS, 0, b, v)

/* Older versions of Yacc set YYMAXDEPTH to a very low value by default (150,
   for instance).  This is too low for Ruby to parse some files, such as
   date/format.rb, therefore bump the value up to at least Bison's default. */
#ifdef OLD_YACC
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif
#endif



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 348 "grammar19.y"
{
    VALUE val;
    NODE *node;
    QUID id;
    int num;
    var_table vars;
}
/* Line 193 of yacc.c.  */
#line 656 "grammar19.cpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 669 "grammar19.cpp"

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
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
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
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   10155

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  138
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  175
/* YYNRULES -- Number of rules.  */
#define YYNRULES  568
/* YYNRULES -- Number of states.  */
#define YYNSTATES  978

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   365

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
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
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    12,    14,    18,    21,
      23,    24,    30,    35,    38,    40,    42,    46,    49,    50,
      55,    59,    63,    67,    70,    74,    78,    82,    86,    90,
      95,    99,   103,   107,   114,   120,   126,   132,   138,   142,
     146,   150,   154,   156,   158,   162,   166,   170,   173,   175,
     177,   179,   181,   183,   188,   193,   194,   195,   202,   205,
     209,   214,   220,   225,   231,   234,   237,   240,   243,   246,
     248,   252,   254,   258,   260,   263,   267,   273,   276,   281,
     284,   289,   291,   295,   297,   301,   304,   308,   310,   314,
     316,   321,   325,   329,   333,   337,   340,   342,   344,   349,
     353,   357,   361,   365,   368,   370,   372,   374,   377,   379,
     383,   385,   387,   389,   391,   393,   395,   397,   399,   401,
     403,   404,   409,   411,   413,   415,   417,   419,   421,   423,
     425,   427,   429,   431,   433,   435,   437,   439,   441,   443,
     445,   447,   449,   451,   453,   455,   457,   459,   461,   463,
     465,   467,   469,   471,   473,   475,   477,   479,   481,   483,
     485,   487,   489,   491,   493,   495,   497,   499,   501,   503,
     505,   507,   509,   511,   513,   515,   517,   519,   521,   523,
     525,   527,   529,   531,   533,   535,   537,   539,   541,   543,
     545,   547,   549,   553,   559,   563,   569,   576,   582,   588,
     594,   600,   605,   609,   613,   617,   621,   625,   629,   633,
     637,   641,   646,   651,   654,   657,   661,   665,   669,   673,
     677,   681,   685,   689,   693,   697,   701,   705,   709,   712,
     715,   719,   723,   727,   731,   732,   737,   744,   746,   748,
     750,   753,   758,   761,   765,   767,   769,   771,   773,   775,
     778,   781,   786,   788,   789,   792,   795,   798,   800,   802,
     804,   807,   811,   816,   820,   825,   828,   830,   832,   834,
     836,   838,   840,   842,   844,   846,   847,   852,   853,   858,
     862,   866,   869,   873,   877,   879,   884,   888,   890,   891,
     898,   903,   907,   910,   912,   915,   918,   925,   932,   933,
     934,   942,   943,   944,   952,   958,   963,   964,   965,   975,
     976,   983,   984,   985,   994,   995,  1001,  1002,  1009,  1010,
    1011,  1021,  1023,  1025,  1027,  1029,  1031,  1033,  1035,  1037,
    1039,  1041,  1043,  1045,  1047,  1049,  1051,  1053,  1055,  1057,
    1060,  1062,  1064,  1066,  1072,  1074,  1077,  1079,  1081,  1083,
    1087,  1089,  1093,  1095,  1100,  1107,  1111,  1117,  1120,  1125,
    1127,  1131,  1138,  1147,  1152,  1159,  1164,  1167,  1174,  1177,
    1182,  1189,  1192,  1197,  1200,  1205,  1207,  1209,  1211,  1215,
    1217,  1222,  1224,  1227,  1229,  1233,  1235,  1237,  1238,  1239,
    1244,  1249,  1251,  1255,  1259,  1260,  1261,  1268,  1271,  1276,
    1281,  1284,  1289,  1294,  1298,  1302,  1306,  1309,  1311,  1316,
    1317,  1318,  1325,  1326,  1327,  1334,  1340,  1342,  1344,  1351,
    1353,  1355,  1357,  1359,  1362,  1364,  1367,  1369,  1371,  1373,
    1375,  1377,  1379,  1381,  1384,  1388,  1392,  1396,  1400,  1404,
    1405,  1409,  1411,  1414,  1418,  1422,  1423,  1427,  1428,  1431,
    1432,  1435,  1436,  1439,  1441,  1442,  1446,  1447,  1452,  1454,
    1456,  1458,  1460,  1463,  1465,  1467,  1469,  1471,  1475,  1477,
    1479,  1482,  1485,  1487,  1489,  1491,  1493,  1495,  1497,  1499,
    1501,  1503,  1505,  1507,  1509,  1511,  1513,  1515,  1517,  1519,
    1520,  1525,  1528,  1532,  1535,  1542,  1551,  1556,  1563,  1568,
    1575,  1578,  1583,  1590,  1593,  1598,  1601,  1606,  1608,  1609,
    1611,  1613,  1615,  1617,  1619,  1621,  1623,  1627,  1629,  1633,
    1637,  1641,  1643,  1647,  1649,  1653,  1655,  1657,  1660,  1662,
    1664,  1666,  1669,  1672,  1674,  1676,  1677,  1682,  1684,  1687,
    1689,  1693,  1697,  1700,  1702,  1704,  1706,  1708,  1710,  1712,
    1714,  1716,  1718,  1720,  1722,  1724,  1725,  1727,  1728,  1730,
    1733,  1736,  1737,  1739,  1741,  1743,  1745,  1747,  1750
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     139,     0,    -1,    -1,   140,   141,    -1,   142,   305,    -1,
     312,    -1,   143,    -1,   142,   311,   143,    -1,     1,   143,
      -1,   148,    -1,    -1,    47,   144,   126,   141,   127,    -1,
     146,   249,   221,   252,    -1,   147,   305,    -1,   312,    -1,
     148,    -1,   147,   311,   148,    -1,     1,   148,    -1,    -1,
      45,   170,   149,   170,    -1,    45,    54,    54,    -1,    45,
      54,    64,    -1,    45,    54,    63,    -1,     6,   171,    -1,
     148,    40,   151,    -1,   148,    41,   151,    -1,   148,    42,
     151,    -1,   148,    43,   151,    -1,   148,    44,   148,    -1,
      48,   126,   146,   127,    -1,   165,   109,   152,    -1,   158,
     109,   152,    -1,   277,    87,   152,    -1,   206,   128,   181,
     308,    87,   152,    -1,   206,   129,    52,    87,   152,    -1,
     206,   129,    56,    87,   152,    -1,   206,    85,    56,    87,
     152,    -1,   206,    85,    52,    87,   152,    -1,   278,    87,
     152,    -1,   165,   109,   188,    -1,   158,   109,   177,    -1,
     158,   109,   188,    -1,   150,    -1,   152,    -1,   150,    37,
     150,    -1,   150,    38,   150,    -1,    39,   306,   150,    -1,
     123,   152,    -1,   175,    -1,   150,    -1,   157,    -1,   153,
      -1,   240,    -1,   240,   129,   302,   183,    -1,   240,    85,
     302,   183,    -1,    -1,    -1,    94,   155,   227,   156,   146,
     127,    -1,   301,   183,    -1,   301,   183,   154,    -1,   206,
     129,   302,   183,    -1,   206,   129,   302,   183,   154,    -1,
     206,    85,   302,   183,    -1,   206,    85,   302,   183,   154,
      -1,    32,   183,    -1,    31,   183,    -1,    30,   182,    -1,
      21,   182,    -1,    22,   182,    -1,   160,    -1,    89,   159,
     307,    -1,   160,    -1,    89,   159,   307,    -1,   162,    -1,
     162,   161,    -1,   162,    95,   164,    -1,   162,    95,   164,
     130,   163,    -1,   162,    95,    -1,   162,    95,   130,   163,
      -1,    95,   164,    -1,    95,   164,   130,   163,    -1,    95,
      -1,    95,   130,   163,    -1,   164,    -1,    89,   159,   307,
      -1,   161,   130,    -1,   162,   161,   130,    -1,   161,    -1,
     163,   130,   161,    -1,   275,    -1,   206,   128,   181,   308,
      -1,   206,   129,    52,    -1,   206,    85,    52,    -1,   206,
     129,    56,    -1,   206,    85,    56,    -1,    86,    56,    -1,
     278,    -1,   275,    -1,   206,   128,   181,   308,    -1,   206,
     129,    52,    -1,   206,    85,    52,    -1,   206,   129,    56,
      -1,   206,    85,    56,    -1,    86,    56,    -1,   278,    -1,
      52,    -1,    56,    -1,    86,   166,    -1,   166,    -1,   206,
      85,   166,    -1,    52,    -1,    56,    -1,    53,    -1,   173,
      -1,   174,    -1,   168,    -1,   271,    -1,   169,    -1,   273,
      -1,   170,    -1,    -1,   171,   130,   172,   170,    -1,   114,
      -1,   115,    -1,   116,    -1,    69,    -1,    70,    -1,    71,
      -1,    77,    -1,    78,    -1,   112,    -1,    73,    -1,   113,
      -1,    74,    -1,    72,    -1,    83,    -1,    84,    -1,   117,
      -1,   118,    -1,   119,    -1,    95,    -1,   120,    -1,   121,
      -1,    68,    -1,   123,    -1,   124,    -1,    66,    -1,    67,
      -1,    81,    -1,    82,    -1,   131,    -1,    49,    -1,    50,
      -1,    51,    -1,    47,    -1,    48,    -1,    45,    -1,    37,
      -1,     7,    -1,    21,    -1,    16,    -1,     3,    -1,     5,
      -1,    46,    -1,    26,    -1,    15,    -1,    14,    -1,    10,
      -1,     9,    -1,    36,    -1,    20,    -1,    25,    -1,     4,
      -1,    22,    -1,    34,    -1,    39,    -1,    38,    -1,    23,
      -1,     8,    -1,    24,    -1,    30,    -1,    33,    -1,    32,
      -1,    13,    -1,    35,    -1,     6,    -1,    17,    -1,    31,
      -1,    11,    -1,    12,    -1,    18,    -1,    19,    -1,   165,
     109,   175,    -1,   165,   109,   175,    44,   175,    -1,   277,
      87,   175,    -1,   277,    87,   175,    44,   175,    -1,   206,
     128,   181,   308,    87,   175,    -1,   206,   129,    52,    87,
     175,    -1,   206,   129,    56,    87,   175,    -1,   206,    85,
      52,    87,   175,    -1,   206,    85,    56,    87,   175,    -1,
      86,    56,    87,   175,    -1,   278,    87,   175,    -1,   175,
      79,   175,    -1,   175,    80,   175,    -1,   175,   117,   175,
      -1,   175,   118,   175,    -1,   175,   119,   175,    -1,   175,
     120,   175,    -1,   175,   121,   175,    -1,   175,    68,   175,
      -1,   122,    59,    68,   175,    -1,   122,    60,    68,   175,
      -1,    66,   175,    -1,    67,   175,    -1,   175,   114,   175,
      -1,   175,   115,   175,    -1,   175,   116,   175,    -1,   175,
      69,   175,    -1,   175,   112,   175,    -1,   175,    73,   175,
      -1,   175,   113,   175,    -1,   175,    74,   175,    -1,   175,
      70,   175,    -1,   175,    71,   175,    -1,   175,    72,   175,
      -1,   175,    77,   175,    -1,   175,    78,   175,    -1,   123,
     175,    -1,   124,   175,    -1,   175,    83,   175,    -1,   175,
      84,   175,    -1,   175,    75,   175,    -1,   175,    76,   175,
      -1,    -1,    46,   306,   176,   175,    -1,   175,   110,   175,
     306,   111,   175,    -1,   189,    -1,   175,    -1,   312,    -1,
     187,   309,    -1,   187,   130,   299,   309,    -1,   299,   309,
      -1,   132,   181,   307,    -1,   312,    -1,   179,    -1,   312,
      -1,   182,    -1,   157,    -1,   187,   186,    -1,   299,   186,
      -1,   187,   130,   299,   186,    -1,   185,    -1,    -1,   184,
     182,    -1,    96,   177,    -1,   130,   185,    -1,   130,    -1,
     312,    -1,   177,    -1,    95,   177,    -1,   187,   130,   177,
      -1,   187,   130,    95,   177,    -1,   187,   130,   177,    -1,
     187,   130,    95,   177,    -1,    95,   177,    -1,   253,    -1,
     254,    -1,   257,    -1,   258,    -1,   259,    -1,   262,    -1,
     276,    -1,   278,    -1,    53,    -1,    -1,   207,   190,   145,
     217,    -1,    -1,    90,   150,   191,   307,    -1,    89,   146,
     133,    -1,   206,    85,    56,    -1,    86,    56,    -1,    92,
     178,   134,    -1,    93,   298,   127,    -1,    30,    -1,    31,
     132,   182,   307,    -1,    31,   132,   307,    -1,    31,    -1,
      -1,    46,   306,   132,   192,   150,   307,    -1,    39,   132,
     150,   307,    -1,    39,   132,   307,    -1,   301,   242,    -1,
     241,    -1,   241,   242,    -1,    97,   232,    -1,   208,   151,
     218,   146,   220,   217,    -1,   209,   151,   218,   146,   221,
     217,    -1,    -1,    -1,   210,   193,   151,   219,   194,   146,
     217,    -1,    -1,    -1,   211,   195,   151,   219,   196,   146,
     217,    -1,   212,   151,   305,   247,   217,    -1,   212,   305,
     247,   217,    -1,    -1,    -1,   213,   222,    25,   197,   151,
     219,   198,   146,   217,    -1,    -1,   214,   167,   279,   199,
     145,   217,    -1,    -1,    -1,   214,    83,   150,   200,   310,
     201,   145,   217,    -1,    -1,   215,   167,   202,   145,   217,
      -1,    -1,   216,   168,   203,   281,   145,   217,    -1,    -1,
      -1,   216,   296,   304,   204,   168,   205,   281,   145,   217,
      -1,    21,    -1,    22,    -1,    23,    -1,    24,    -1,   189,
      -1,     7,    -1,    11,    -1,    12,    -1,    18,    -1,    19,
      -1,    16,    -1,    20,    -1,     3,    -1,     4,    -1,     5,
      -1,    10,    -1,   310,    -1,    13,    -1,   310,    13,    -1,
     310,    -1,    27,    -1,   221,    -1,    14,   151,   218,   146,
     220,    -1,   312,    -1,    15,   146,    -1,   165,    -1,   158,
      -1,   284,    -1,    89,   225,   307,    -1,   223,    -1,   224,
     130,   223,    -1,   224,    -1,   224,   130,    95,   284,    -1,
     224,   130,    95,   284,   130,   224,    -1,   224,   130,    95,
      -1,   224,   130,    95,   130,   224,    -1,    95,   284,    -1,
      95,   284,   130,   224,    -1,    95,    -1,    95,   130,   224,
      -1,   286,   130,   289,   130,   292,   295,    -1,   286,   130,
     289,   130,   292,   130,   286,   295,    -1,   286,   130,   289,
     295,    -1,   286,   130,   289,   130,   286,   295,    -1,   286,
     130,   292,   295,    -1,   286,   130,    -1,   286,   130,   292,
     130,   286,   295,    -1,   286,   295,    -1,   289,   130,   292,
     295,    -1,   289,   130,   292,   130,   286,   295,    -1,   289,
     295,    -1,   289,   130,   286,   295,    -1,   292,   295,    -1,
     292,   130,   286,   295,    -1,   294,    -1,   312,    -1,   228,
      -1,   114,   229,   114,    -1,    76,    -1,   114,   226,   229,
     114,    -1,   312,    -1,   135,   230,    -1,   231,    -1,   230,
     130,   231,    -1,    52,    -1,   283,    -1,    -1,    -1,   233,
     234,   235,   236,    -1,   132,   282,   229,   307,    -1,   282,
      -1,   107,   146,   127,    -1,    29,   146,    10,    -1,    -1,
      -1,    28,   238,   227,   239,   146,    10,    -1,   157,   237,
      -1,   240,   129,   302,   180,    -1,   240,    85,   302,   180,
      -1,   301,   179,    -1,   206,   129,   302,   180,    -1,   206,
      85,   302,   179,    -1,   206,    85,   303,    -1,   206,   129,
     179,    -1,   206,    85,   179,    -1,    32,   179,    -1,    32,
      -1,   206,   128,   181,   308,    -1,    -1,    -1,   126,   243,
     227,   244,   146,   127,    -1,    -1,    -1,    26,   245,   227,
     246,   146,    10,    -1,    17,   187,   218,   146,   248,    -1,
     221,    -1,   247,    -1,     8,   250,   251,   218,   146,   249,
      -1,   312,    -1,   177,    -1,   188,    -1,   312,    -1,    88,
     165,    -1,   312,    -1,     9,   146,    -1,   312,    -1,   274,
      -1,   271,    -1,   273,    -1,   255,    -1,    62,    -1,   256,
      -1,   255,   256,    -1,    99,   264,   106,    -1,   100,   265,
     106,    -1,   101,   266,    65,    -1,   102,   136,   106,    -1,
     102,   260,   106,    -1,    -1,   260,   261,   136,    -1,   267,
      -1,   261,   267,    -1,   103,   136,   106,    -1,   103,   263,
     106,    -1,    -1,   263,    61,   136,    -1,    -1,   264,   267,
      -1,    -1,   265,   267,    -1,    -1,   266,   267,    -1,    61,
      -1,    -1,   105,   268,   270,    -1,    -1,   104,   269,   146,
     127,    -1,    54,    -1,    55,    -1,    57,    -1,   278,    -1,
      98,   272,    -1,   168,    -1,    55,    -1,    54,    -1,    57,
      -1,    98,   265,   106,    -1,    59,    -1,    60,    -1,   122,
      59,    -1,   122,    60,    -1,    52,    -1,    55,    -1,    54,
      -1,    56,    -1,    57,    -1,    34,    -1,    33,    -1,    35,
      -1,    36,    -1,    50,    -1,    49,    -1,    51,    -1,   275,
      -1,   275,    -1,    63,    -1,    64,    -1,   310,    -1,    -1,
     113,   280,   151,   310,    -1,     1,   310,    -1,   132,   282,
     307,    -1,   282,   310,    -1,   286,   130,   290,   130,   292,
     295,    -1,   286,   130,   290,   130,   292,   130,   286,   295,
      -1,   286,   130,   290,   295,    -1,   286,   130,   290,   130,
     286,   295,    -1,   286,   130,   292,   295,    -1,   286,   130,
     292,   130,   286,   295,    -1,   286,   295,    -1,   290,   130,
     292,   295,    -1,   290,   130,   292,   130,   286,   295,    -1,
     290,   295,    -1,   290,   130,   286,   295,    -1,   292,   295,
      -1,   292,   130,   286,   295,    -1,   294,    -1,    -1,    56,
      -1,    55,    -1,    54,    -1,    57,    -1,   283,    -1,    52,
      -1,   284,    -1,    89,   225,   307,    -1,   285,    -1,   286,
     130,   285,    -1,    52,   109,   177,    -1,    52,   109,   206,
      -1,   288,    -1,   289,   130,   288,    -1,   287,    -1,   290,
     130,   287,    -1,   119,    -1,    95,    -1,   291,    52,    -1,
     291,    -1,   116,    -1,    96,    -1,   293,    52,    -1,   130,
     294,    -1,   312,    -1,   276,    -1,    -1,   132,   297,   150,
     307,    -1,   312,    -1,   299,   309,    -1,   300,    -1,   299,
     130,   300,    -1,   177,    88,   177,    -1,    58,   177,    -1,
      52,    -1,    56,    -1,    53,    -1,    52,    -1,    56,    -1,
      53,    -1,   173,    -1,    52,    -1,    53,    -1,   173,    -1,
     129,    -1,    85,    -1,    -1,   311,    -1,    -1,   137,    -1,
     306,   133,    -1,   306,   134,    -1,    -1,   137,    -1,   130,
      -1,   135,    -1,   137,    -1,   310,    -1,   311,   135,    -1,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   496,   496,   496,   520,   527,   531,   535,   539,   545,
     547,   546,   561,   584,   591,   595,   599,   603,   609,   609,
     613,   617,   624,   629,   633,   638,   643,   651,   659,   664,
     673,   678,   684,   706,   721,   732,   743,   748,   759,   764,
     769,   774,   779,   782,   783,   787,   791,   795,   799,   802,
     810,   811,   814,   815,   819,   826,   831,   825,   841,   846,
     853,   858,   865,   870,   877,   882,   887,   891,   895,   901,
     902,   908,   909,   915,   919,   923,   927,   931,   935,   939,
     943,   947,   951,   957,   958,   964,   968,   974,   978,   984,
     988,   992,   996,  1000,  1004,  1010,  1016,  1023,  1028,  1032,
    1036,  1040,  1044,  1050,  1056,  1063,  1067,  1070,  1074,  1078,
    1084,  1085,  1086,  1087,  1092,  1099,  1100,  1103,  1107,  1110,
    1114,  1114,  1120,  1121,  1122,  1123,  1124,  1125,  1126,  1127,
    1128,  1129,  1130,  1131,  1132,  1133,  1134,  1135,  1136,  1137,
    1138,  1139,  1140,  1141,  1142,  1143,  1144,  1145,  1146,  1147,
    1148,  1151,  1151,  1151,  1152,  1152,  1153,  1153,  1153,  1154,
    1154,  1154,  1154,  1155,  1155,  1155,  1155,  1156,  1156,  1156,
    1157,  1157,  1157,  1157,  1158,  1158,  1158,  1158,  1159,  1159,
    1159,  1159,  1160,  1160,  1160,  1160,  1161,  1161,  1161,  1161,
    1162,  1162,  1165,  1170,  1176,  1198,  1221,  1240,  1251,  1262,
    1273,  1278,  1283,  1288,  1294,  1300,  1304,  1308,  1312,  1316,
    1320,  1324,  1328,  1332,  1336,  1340,  1344,  1348,  1352,  1356,
    1360,  1364,  1368,  1372,  1376,  1380,  1384,  1394,  1398,  1402,
    1406,  1410,  1414,  1418,  1422,  1422,  1427,  1433,  1439,  1447,
    1448,  1452,  1456,  1462,  1468,  1469,  1472,  1473,  1476,  1481,
    1485,  1490,  1495,  1498,  1498,  1510,  1516,  1520,  1524,  1530,
    1534,  1538,  1547,  1558,  1567,  1576,  1582,  1583,  1584,  1585,
    1586,  1587,  1588,  1589,  1590,  1595,  1594,  1611,  1611,  1616,
    1620,  1624,  1628,  1636,  1640,  1644,  1648,  1652,  1656,  1656,
    1661,  1665,  1669,  1675,  1676,  1683,  1687,  1695,  1703,  1703,
    1703,  1710,  1710,  1710,  1717,  1724,  1729,  1731,  1728,  1748,
    1747,  1764,  1769,  1763,  1786,  1785,  1802,  1801,  1820,  1821,
    1820,  1835,  1839,  1843,  1847,  1853,  1861,  1867,  1873,  1879,
    1885,  1891,  1897,  1903,  1909,  1915,  1922,  1928,  1929,  1930,
    1933,  1934,  1937,  1938,  1947,  1948,  1954,  1955,  1958,  1962,
    1968,  1972,  1978,  1982,  1987,  1992,  1996,  2000,  2005,  2010,
    2014,  2020,  2024,  2028,  2032,  2036,  2040,  2044,  2048,  2052,
    2056,  2060,  2064,  2068,  2072,  2076,  2082,  2083,  2089,  2093,
    2097,  2103,  2104,  2110,  2111,  2114,  2118,  2124,  2127,  2124,
    2140,  2144,  2150,  2154,  2161,  2166,  2160,  2178,  2189,  2193,
    2199,  2204,  2209,  2214,  2218,  2223,  2228,  2232,  2236,  2248,
    2252,  2247,  2260,  2264,  2259,  2273,  2281,  2282,  2285,  2297,
    2300,  2304,  2308,  2311,  2315,  2318,  2322,  2325,  2326,  2330,
    2333,  2345,  2346,  2347,  2353,  2359,  2381,  2411,  2415,  2422,
    2425,  2431,  2432,  2438,  2442,  2449,  2452,  2459,  2462,  2469,
    2472,  2479,  2482,  2505,  2507,  2506,  2518,  2517,  2540,  2541,
    2542,  2543,  2546,  2553,  2554,  2555,  2556,  2559,  2589,  2590,
    2591,  2595,  2601,  2602,  2603,  2604,  2605,  2606,  2607,  2608,
    2609,  2610,  2611,  2612,  2615,  2623,  2629,  2630,  2633,  2638,
    2637,  2645,  2652,  2658,  2664,  2668,  2672,  2676,  2680,  2684,
    2688,  2692,  2696,  2700,  2704,  2708,  2712,  2716,  2721,  2726,
    2731,  2736,  2741,  2748,  2749,  2756,  2761,  2770,  2771,  2779,
    2787,  2795,  2799,  2810,  2814,  2825,  2826,  2829,  2837,  2844,
    2845,  2848,  2859,  2863,  2869,  2875,  2875,  2899,  2900,  2906,
    2907,  2913,  2917,  2923,  2924,  2925,  2928,  2929,  2930,  2931,
    2934,  2935,  2936,  2939,  2940,  2943,  2944,  2947,  2948,  2951,
    2954,  2957,  2958,  2959,  2962,  2963,  2966,  2967,  2970
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
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
  "' '", "'\\n'", "$accept", "program", "@1", "top_compstmt", "top_stmts",
  "top_stmt", "@2", "bodystmt", "compstmt", "stmts", "stmt", "@3", "expr",
  "expr_value", "command_call", "block_command", "cmd_brace_block", "@4",
  "@5", "command", "mlhs", "mlhs_inner", "mlhs_basic", "mlhs_item",
  "mlhs_head", "mlhs_post", "mlhs_node", "lhs", "cname", "cpath", "fname",
  "fsym", "fitem", "undef_list", "@6", "op", "reswords", "arg", "@7",
  "arg_value", "aref_args", "paren_args", "opt_paren_args",
  "opt_call_args", "call_args", "command_args", "@8", "block_arg",
  "opt_block_arg", "args", "mrhs", "primary", "@9", "@10", "@11", "@12",
  "@13", "@14", "@15", "@16", "@17", "@18", "@19", "@20", "@21", "@22",
  "@23", "@24", "primary_value", "k_begin", "k_if", "k_unless", "k_while",
  "k_until", "k_case", "k_for", "k_class", "k_module", "k_def", "k_end",
  "then", "do", "if_tail", "opt_else", "for_var", "f_marg", "f_marg_list",
  "f_margs", "block_param", "opt_block_param", "block_param_def",
  "opt_bv_decl", "bv_decls", "bvar", "lambda", "@25", "@26", "f_larglist",
  "lambda_body", "do_block", "@27", "@28", "block_call", "method_call",
  "brace_block", "@29", "@30", "@31", "@32", "case_body", "cases",
  "opt_rescue", "exc_list", "exc_var", "opt_ensure", "literal", "strings",
  "string", "string1", "xstring", "regexp", "words", "word_list", "word",
  "qwords", "qword_list", "string_contents", "xstring_contents",
  "regexp_contents", "string_content", "@33", "@34", "string_dvar",
  "symbol", "sym", "dsym", "numeric", "variable", "var_ref", "var_lhs",
  "backref", "superclass", "@35", "f_arglist", "f_args", "f_bad_arg",
  "f_norm_arg", "f_arg_item", "f_arg", "f_opt", "f_block_opt",
  "f_block_optarg", "f_optarg", "restarg_mark", "f_rest_arg",
  "blkarg_mark", "f_block_arg", "opt_f_block_arg", "singleton", "@36",
  "assoc_list", "assocs", "assoc", "operation", "operation2", "operation3",
  "dot_or_colon", "opt_terms", "opt_nl", "rparen", "rbracket", "trailer",
  "term", "terms", "none", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
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

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   138,   140,   139,   141,   142,   142,   142,   142,   143,
     144,   143,   145,   146,   147,   147,   147,   147,   149,   148,
     148,   148,   148,   148,   148,   148,   148,   148,   148,   148,
     148,   148,   148,   148,   148,   148,   148,   148,   148,   148,
     148,   148,   148,   150,   150,   150,   150,   150,   150,   151,
     152,   152,   153,   153,   153,   155,   156,   154,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   158,
     158,   159,   159,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   161,   161,   162,   162,   163,   163,   164,
     164,   164,   164,   164,   164,   164,   164,   165,   165,   165,
     165,   165,   165,   165,   165,   166,   166,   167,   167,   167,
     168,   168,   168,   168,   168,   169,   169,   170,   170,   171,
     172,   171,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   176,   175,   175,   175,   177,   178,
     178,   178,   178,   179,   180,   180,   181,   181,   182,   182,
     182,   182,   182,   184,   183,   185,   186,   186,   186,   187,
     187,   187,   187,   188,   188,   188,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   190,   189,   191,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   192,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   193,   194,
     189,   195,   196,   189,   189,   189,   197,   198,   189,   199,
     189,   200,   201,   189,   202,   189,   203,   189,   204,   205,
     189,   189,   189,   189,   189,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   218,   218,
     219,   219,   220,   220,   221,   221,   222,   222,   223,   223,
     224,   224,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   226,   226,   226,   226,   226,   226,   226,   226,   226,
     226,   226,   226,   226,   226,   226,   227,   227,   228,   228,
     228,   229,   229,   230,   230,   231,   231,   233,   234,   232,
     235,   235,   236,   236,   238,   239,   237,   240,   240,   240,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   243,
     244,   242,   245,   246,   242,   247,   248,   248,   249,   249,
     250,   250,   250,   251,   251,   252,   252,   253,   253,   253,
     254,   255,   255,   255,   256,   257,   258,   259,   259,   260,
     260,   261,   261,   262,   262,   263,   263,   264,   264,   265,
     265,   266,   266,   267,   268,   267,   269,   267,   270,   270,
     270,   270,   271,   272,   272,   272,   272,   273,   274,   274,
     274,   274,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   276,   277,   278,   278,   279,   280,
     279,   279,   281,   281,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,   282,   282,   282,   283,
     283,   283,   283,   284,   284,   285,   285,   286,   286,   287,
     288,   289,   289,   290,   290,   291,   291,   292,   292,   293,
     293,   294,   295,   295,   296,   297,   296,   298,   298,   299,
     299,   300,   300,   301,   301,   301,   302,   302,   302,   302,
     303,   303,   303,   304,   304,   305,   305,   306,   306,   307,
     308,   309,   309,   309,   310,   310,   311,   311,   312
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     1,     1,     3,     2,     1,
       0,     5,     4,     2,     1,     1,     3,     2,     0,     4,
       3,     3,     3,     2,     3,     3,     3,     3,     3,     4,
       3,     3,     3,     6,     5,     5,     5,     5,     3,     3,
       3,     3,     1,     1,     3,     3,     3,     2,     1,     1,
       1,     1,     1,     4,     4,     0,     0,     6,     2,     3,
       4,     5,     4,     5,     2,     2,     2,     2,     2,     1,
       3,     1,     3,     1,     2,     3,     5,     2,     4,     2,
       4,     1,     3,     1,     3,     2,     3,     1,     3,     1,
       4,     3,     3,     3,     3,     2,     1,     1,     4,     3,
       3,     3,     3,     2,     1,     1,     1,     2,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     4,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     5,     3,     5,     6,     5,     5,     5,
       5,     4,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     4,     4,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       3,     3,     3,     3,     0,     4,     6,     1,     1,     1,
       2,     4,     2,     3,     1,     1,     1,     1,     1,     2,
       2,     4,     1,     0,     2,     2,     2,     1,     1,     1,
       2,     3,     4,     3,     4,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     4,     0,     4,     3,
       3,     2,     3,     3,     1,     4,     3,     1,     0,     6,
       4,     3,     2,     1,     2,     2,     6,     6,     0,     0,
       7,     0,     0,     7,     5,     4,     0,     0,     9,     0,
       6,     0,     0,     8,     0,     5,     0,     6,     0,     0,
       9,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     5,     1,     2,     1,     1,     1,     3,
       1,     3,     1,     4,     6,     3,     5,     2,     4,     1,
       3,     6,     8,     4,     6,     4,     2,     6,     2,     4,
       6,     2,     4,     2,     4,     1,     1,     1,     3,     1,
       4,     1,     2,     1,     3,     1,     1,     0,     0,     4,
       4,     1,     3,     3,     0,     0,     6,     2,     4,     4,
       2,     4,     4,     3,     3,     3,     2,     1,     4,     0,
       0,     6,     0,     0,     6,     5,     1,     1,     6,     1,
       1,     1,     1,     2,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     3,     3,     3,     0,
       3,     1,     2,     3,     3,     0,     3,     0,     2,     0,
       2,     0,     2,     1,     0,     3,     0,     4,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     3,     1,     1,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       4,     2,     3,     2,     6,     8,     4,     6,     4,     6,
       2,     4,     6,     2,     4,     2,     4,     1,     0,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     3,     3,
       3,     1,     3,     1,     3,     1,     1,     2,     1,     1,
       1,     2,     2,     1,     1,     0,     4,     1,     2,     1,
       3,     3,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     1,     0,     1,     2,
       2,     0,     1,     1,     1,     1,     1,     2,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     0,     1,     0,   333,   334,   335,     0,   326,
     327,   328,   331,   329,   330,   332,   321,   322,   323,   324,
     284,   253,   253,   478,   477,   479,   480,   557,     0,   557,
      10,     0,   482,   481,   483,   472,   545,   474,   473,   475,
     476,   468,   469,   431,   486,   487,     0,     0,     0,     0,
       0,   568,   568,    81,   387,   449,   447,   449,   451,   439,
     445,     0,     0,     0,     3,   555,     6,     9,    42,    43,
      51,    50,     0,    69,     0,    73,    83,     0,    48,   237,
       0,   275,     0,     0,   298,   301,   555,     0,     0,     0,
       0,    52,   293,   266,   267,   430,   432,   268,   269,   270,
     271,   428,   429,   427,   484,   272,     0,   273,   253,     5,
       8,   161,   172,   162,   185,   158,   178,   168,   167,   188,
     189,   183,   166,   165,   160,   186,   190,   191,   170,   159,
     173,   177,   179,   171,   164,   180,   187,   182,   181,   174,
     184,   169,   157,   176,   175,   156,   163,   154,   155,   151,
     152,   153,   110,   112,   111,   146,   147,   143,   125,   126,
     127,   134,   131,   133,   128,   129,   148,   149,   135,   136,
     140,   130,   132,   122,   123,   124,   137,   138,   139,   141,
     142,   144,   145,   150,   115,   117,   119,    23,   113,   114,
     116,   118,     0,     0,     0,     0,     0,     0,     0,   248,
       0,   238,   259,    67,   252,   568,     0,   484,     0,   273,
     568,   539,    68,    66,   557,    65,     0,   568,   406,    64,
     557,   558,     0,     0,    18,   234,     0,     0,   321,   322,
     284,   287,   407,   213,     0,     0,   214,   281,     0,     0,
       0,   555,    15,   557,    71,    14,   277,     0,   561,   561,
     239,     0,     0,   561,   537,   557,     0,     0,     0,    79,
     325,     0,    89,    96,   295,   388,   465,   464,   466,   463,
       0,   462,     0,     0,     0,     0,     0,     0,     0,   470,
     471,    47,   228,   229,   564,   565,     4,   566,   556,     0,
       0,     0,     0,     0,     0,     0,   394,   397,     0,    85,
       0,    77,    74,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   568,     0,     0,    49,     0,     0,     0,     0,   555,
       0,   556,     0,   347,   346,     0,     0,   484,   273,   105,
     106,     0,     0,   108,     0,     0,   484,   273,   314,   181,
     174,   184,   169,   151,   152,   153,   110,   111,   535,   316,
     534,     0,     0,     0,   412,   409,   294,   433,     0,     0,
     400,    58,   292,   120,   542,   281,   260,   255,     0,     0,
     257,   249,   258,     0,   568,     0,     0,     0,   257,   250,
     557,     0,   286,   254,   557,   247,   246,   557,   291,    46,
      20,    22,    21,     0,   288,     0,     0,     0,     0,     0,
       0,    17,   557,   279,    13,   556,    70,   557,   282,   563,
     562,   240,   563,   242,   283,   538,     0,    95,   470,   471,
      87,    82,     0,     0,   568,     0,   508,   453,   456,   454,
     467,   450,   434,   448,   435,   436,   452,   437,   438,     0,
     441,   443,     0,   444,     0,     0,   567,     7,    24,    25,
      26,    27,    28,    44,    45,   568,     0,    31,    40,     0,
      41,   557,     0,    75,    86,    30,   192,   259,    39,   210,
     218,   223,   224,   225,   220,   222,   232,   233,   226,   227,
     203,   204,   230,   231,   557,   219,   221,   215,   216,   217,
     205,   206,   207,   208,   209,   546,   551,   547,   552,   405,
     253,   403,   557,   546,   548,   547,   549,   404,   253,     0,
     568,   338,     0,   337,     0,     0,     0,     0,     0,     0,
     281,     0,   568,     0,   306,   311,   105,   106,   107,     0,
     489,   309,   488,     0,   568,     0,     0,     0,   508,   554,
     553,   318,   546,   547,   253,   253,   568,   568,    32,   194,
      38,   202,    55,    59,     0,   192,   541,     0,   261,   256,
     568,   550,   547,   557,   546,   547,   540,   285,   559,   243,
     290,    19,     0,   235,     0,    29,     0,   568,   201,    72,
      16,   278,   561,     0,    80,    92,    94,   557,   546,   547,
     514,   511,   510,   509,   512,     0,   526,   530,   529,   525,
     508,     0,   391,   513,   515,   517,   568,   523,   568,   528,
     568,     0,   507,     0,     0,   440,   442,   446,   211,   212,
     379,   568,   395,   377,   376,   265,     0,    84,    78,     0,
       0,     0,     0,     0,   402,    62,     0,   408,     0,     0,
     245,   401,    60,   244,   336,   276,   568,   568,   419,   568,
     339,   568,   341,   299,   340,   302,     0,     0,   305,   550,
     280,   557,   546,   547,     0,     0,   491,     0,     0,   105,
     106,   109,   557,     0,   557,   508,     0,     0,     0,   399,
      54,   398,    53,   413,   410,     0,   568,   121,   262,   251,
       0,     0,   408,     0,     0,   557,    11,   241,    88,    90,
       0,   514,     0,   359,   350,   352,   557,   348,   568,     0,
       0,   389,     0,   500,   533,     0,   503,   527,     0,   505,
     531,     0,   458,   459,   460,   455,   461,   514,     0,   568,
       0,   568,   521,   568,   568,   375,   381,     0,     0,   263,
      76,   193,     0,    37,   199,    36,   200,    63,   560,     0,
      34,   197,    35,   198,    61,   420,   421,   568,   422,     0,
     568,   344,     0,     0,   342,     0,     0,     0,   304,     0,
       0,   408,     0,   312,     0,     0,   408,   315,   536,   557,
       0,   493,   319,     0,     0,   195,    56,     0,   289,   519,
     557,     0,   357,     0,   516,   557,     0,     0,   518,   568,
     568,   532,   568,   524,   568,   568,   457,     0,   385,   382,
     383,   386,     0,   378,   366,   368,     0,   371,     0,   373,
       0,   264,   236,    33,   196,     0,     0,   424,   345,     0,
      12,   426,     0,   296,   297,     0,     0,   261,   568,   307,
       0,   490,   310,   492,   317,   508,     0,     0,     0,   349,
     360,     0,   355,   351,   390,   393,   392,     0,   496,     0,
     498,     0,   504,     0,   501,   506,     0,   520,     0,   380,
     568,   568,   568,   522,   568,   568,   396,     0,   423,     0,
      97,   104,     0,   425,     0,   300,   303,   416,   417,   415,
       0,     0,     0,   414,   411,     0,     0,   358,     0,   353,
     568,   568,   568,   568,   281,     0,   384,     0,   363,     0,
     365,   372,     0,   369,   374,   103,     0,   568,     0,   568,
     568,     0,   313,     0,    57,   356,     0,   497,     0,   494,
     499,   502,   550,   280,   568,   568,   568,   568,   550,   102,
     557,   546,   547,   418,   343,   308,   320,   354,   568,   364,
       0,   361,   367,   370,   408,   495,   568,   362
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    64,    65,    66,   226,   529,   530,   241,
     242,   413,    68,   335,    69,    70,   573,   706,   868,    71,
      72,   243,    73,    74,    75,   441,    76,   200,   353,   354,
     184,   185,   186,   187,   574,   526,   189,    78,   415,   202,
     247,   519,   661,   404,   405,   215,   216,   204,   391,   205,
     480,    79,   333,   427,   592,   337,   786,   338,   787,   684,
     910,   688,   685,   860,   556,   558,   698,   865,   234,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,   665,
     532,   673,   783,   784,   346,   724,   725,   726,   749,   642,
     643,   750,   829,   830,   264,   265,   446,   621,   731,   297,
     475,   757,    91,    92,   382,   567,   804,   566,   803,   539,
     909,   667,   777,   846,   850,    93,    94,    95,    96,    97,
      98,    99,   276,   459,   100,   278,   272,   270,   274,   451,
     634,   633,   745,   101,   271,   102,   103,   207,   105,   208,
     209,   551,   687,   696,   697,   623,   624,   625,   626,   627,
     752,   753,   628,   629,   630,   631,   821,   733,   371,   557,
     252,   210,   211,   108,   596,   521,   561,   286,   401,   402,
     657,   431,   533,   341,   245
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -744
static const yytype_int16 yypact[] =
{
    -744,    86,  2356,  -744,  6735,  -744,  -744,  -744,  6274,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  6953,  6953,  -744,  -744,
    6953,  3060,  2667,  -744,  -744,  -744,  -744,   178,  6145,   -42,
    -744,   -25,  -744,  -744,  -744,  5394,  2798,  -744,  -744,  5518,
    -744,  -744,  -744,  -744,  -744,  -744,  8152,  8152,    77,  4189,
    8261,  7171,  7498,  6525,  -744,  6016,  -744,  -744,  -744,    68,
     143,   284,  8370,  8152,  -744,   280,  -744,   791,   466,  -744,
    -744,   184,   173,  -744,   155,  8479,  -744,   189,  2781,   223,
     257,  -744,  8261,  8261,  -744,  -744,  4812,  8584,  8689,  8794,
    5272,    11,   105,  -744,  -744,   219,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,   315,  -744,   251,   405,    61,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,   218,  -744,  -744,
    -744,  -744,   243,  8152,   327,  4324,  8152,  8152,  8152,  -744,
     289,  2781,   323,  -744,  -744,   273,   338,    71,   341,   406,
     313,  -744,  -744,  -744,  4703,  -744,  6953,  6953,  -744,  -744,
    4921,  -744,  8261,   658,  -744,   302,   326,  4459,  -744,  -744,
    -744,   332,   348,  -744,   377,    61,   417,   497,  6844,  4189,
     364,   280,   791,   -42,   407,  -744,   466,   388,    29,   150,
    -744,   323,   403,   150,  -744,   -42,   491,   527,  8899,   421,
    -744,   448,   485,   512,  -744,  -744,  -744,  -744,  -744,  -744,
     334,  -744,   365,   415,   488,   461,   437,   465,    50,   493,
     507,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  5030,  8261,
    8261,  8261,  8261,  6844,  8261,  8261,  -744,  -744,  7607,  -744,
    4189,  6630,   458,  7607,  8152,  8152,  8152,  8152,  8152,  8152,
    8152,  8152,  8152,  8152,  8152,  8152,  8152,  8152,  8152,  8152,
    8152,  8152,  8152,  8152,  8152,  8152,  8152,  8152,  8152,  8152,
    2176,  6953,  9166,  3457,   466,    91,    91,  8261,  8261,   280,
     574,   467,   540,  -744,  -744,   524,   582,    56,    84,   262,
     281,  8261,   113,  -744,    47,   530,  -744,  -744,  -744,    32,
      39,    65,   135,   221,   234,   236,   276,   396,  -744,  -744,
    -744,   409,  1780,  1780,  -744,  -744,  -744,  -744,  8370,  8370,
    -744,   518,  -744,  -744,  -744,   408,  -744,  -744,  8152,  8152,
    7062,  -744,  -744,  9239,  6953,  9312,  8152,  8152,  7280,  -744,
     -42,   484,  -744,  -744,   -42,  -744,  -744,    76,  -744,  -744,
    -744,  -744,  -744,  6274,  -744,  8152,  3836,   494,  9239,  9312,
    8152,   791,   -42,  -744,  -744,  5139,   504,   -42,  -744,  7389,
    -744,  -744,  7498,  -744,  -744,  -744,   302,   534,  -744,  -744,
    -744,   520,  8899,  9385,  6953,  9458,   741,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,    74,
    -744,  -744,   487,  -744,  8152,  8152,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,    22,  8152,  -744,   526,   536,
    -744,   -42,  8899,   548,  -744,  -744,  1664,  -744,  -744,   417,
    3043,  3043,  3043,  3043,  1168,  1168, 10034,  1242,  3043,  3043,
    2912,  2912,   369,   369,  2042,  1168,  1168,  1174,  1174,   949,
      25,    25,   417,   417,   417,  3191,  5764,  3322,  5886,  -744,
     348,  -744,   -42,   584,  -744,   597,  -744,  -744,  2929,   638,
     649,  -744,  3592,   663,  3971,    72,    72,   574,  7716,   638,
      95,  9531,  6953,  9604,  -744,   466,  -744,   534,  -744,   280,
    -744,  -744,  -744,  9677,  6953,  9750,  3457,  8261,   969,  -744,
    -744,  -744,  -744,  -744,  1544,  1544,    22,    22,  -744,  3169,
    -744,  2781,  -744,  -744,  6274,  3300,  -744,  8152,   323,  -744,
     313,  5642,  2503,   -42,   463,   495,  -744,  -744,  -744,  -744,
    -744,  -744,  8261,  2781,   552,  -744,   348,   348,  2781,   202,
     791,  -744,   150,  8899,   520,   346,   319,   -42,    34,   227,
     571,  -744,  -744,  -744,  -744,   874,  -744,  -744,  -744,  -744,
    1062,    98,  -744,  -744,  -744,  -744,   553,  -744,   555,   630,
     557,   639,  -744,  4459,   709,  -744,  -744,  -744,   417,   417,
    -744,  1037,  -744,  -744,  -744,   564,  7825,  -744,   520,  8899,
    8152,   586,  8370,  8370,  -744,   518,   573,   628,  8370,  8370,
    -744,  -744,   518,  -744,  -744,  -744,  7934,   693,  -744,   585,
    -744,   693,  -744,  -744,  -744,  -744,   638,    97,  -744,    44,
      51,   -42,   116,   126,  8261,   280,  -744,  8261,  3457,   346,
     319,  -744,   -42,   638,    76,  1062,  3457,   280,  6403,  -744,
    -744,  -744,  -744,  -744,  -744,  8152,    22,  -744,  -744,  -744,
    8152,  8152,   511,  8152,  8152,    76,  -744,  -744,  -744,   304,
    8152,  -744,   874,   401,  -744,   580,   -42,  -744,   576,  4594,
    4459,  -744,  1062,  -744,  -744,  1062,  -744,  -744,   786,  -744,
    -744,   591,  -744,  -744,  -744,  -744,  -744,   604,   811,   576,
     609,   594,  -744,   595,   603,  -744,  -744,  4594,  8152,   608,
     520,  2781,  8152,  -744,  2781,  -744,  2781,  -744,  -744,  8370,
    -744,  2781,  -744,  2781,  -744,   526,  -744,   652,  -744,  4080,
     733,  -744,  8261,   638,  -744,   638,  4594,  4594,  -744,  8043,
    3727,   145,    72,  -744,   280,   638,  -744,  -744,  -744,   -42,
     638,  -744,  -744,  4594,  4459,  2781,  -744,  8152,  -744,  -744,
     -42,  1108,   613,  1128,  -744,   -42,   734,   621,  -744,   620,
     623,  -744,   625,  -744,   626,   625,  -744,  9004,  -744,   627,
    -744,  -744,   646,  -744,  1114,  -744,  1114,  -744,   786,  -744,
     751,   632,  2781,  -744,  2781,  9109,    91,  -744,  -744,  4594,
    -744,  -744,    91,  -744,  -744,   638,   638,  -744,   462,  -744,
    3457,  -744,  -744,  -744,  -744,   969,   757,   641,  4459,  -744,
     644,  1108,   455,  -744,  -744,  -744,  -744,  1062,  -744,   786,
    -744,   786,  -744,   786,  -744,  -744,   713,   544,   811,  -744,
     647,   650,   625,  -744,   653,   625,  -744,   720,  -744,   546,
     485,   512,  3457,  -744,  3592,  -744,  -744,  -744,  -744,  -744,
    4594,   638,  3457,  -744,  -744,   655,  1108,   644,  1108,   656,
     625,   657,   625,   625,  -744,  9823,  -744,  1114,  -744,   786,
    -744,  -744,   786,  -744,  -744,   534,  9896,  6953,  9969,   649,
     585,   638,  -744,   638,  -744,   644,  1108,  -744,   786,  -744,
    -744,  -744,   659,   662,   625,   670,   625,   625,   112,   319,
     -42,   108,   124,  -744,  -744,  -744,  -744,   644,   625,  -744,
     786,  -744,  -744,  -744,   134,  -744,   625,  -744
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -744,  -744,  -744,   363,  -744,    24,  -744,  -511,   318,  -744,
      38,  -744,    33,   -54,    43,  -744,  -500,  -744,  -744,   -15,
     698,  -147,     9,   -66,  -744,  -379,     3,  1194,  -311,   714,
     -52,  -744,   -20,  -744,  -744,     7,  -744,   683,  -744,    70,
    -744,    46,    73,  -321,    58,     4,  -744,  -241,  -193,   -30,
    -292,    19,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,     2,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,    16,
    -323,  -493,  -138,  -600,  -744,  -743,  -729,    82,  -744,  -506,
    -744,  -415,  -744,   -81,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,   716,  -744,  -744,  -744,  -744,  -513,
    -744,  -130,  -744,  -744,  -744,  -744,  -744,  -744,   715,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,   756,  -744,  -142,
    -744,  -744,  -744,    -1,  -744,     6,  -744,   966,   724,  1274,
    1126,  -744,  -744,   -46,  -381,  -682,   104,  -686,   207,  -681,
    -697,   -13,    92,  -744,  -478,  -744,  -430,   710,  -744,  -744,
    -744,   -21,  -260,  1889,  -293,  -744,  -744,   -50,    -4,   -45,
    -563,  -196,   -53,   -18,    -2
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -569
static const yytype_int16 yytable[] =
{
     109,   199,   199,   269,    80,   199,    80,   190,   224,   302,
     522,   488,   287,   534,   191,   188,   632,   399,   206,   206,
     712,   248,   206,   222,   676,   225,   219,   190,   110,   336,
     249,   253,   339,   287,   191,   188,   340,   520,   369,   528,
      67,   548,    67,   675,   719,   693,   818,   288,   549,   250,
     254,    80,   206,   433,   823,   261,   259,   435,   244,   -91,
     703,   704,   188,   604,   206,   622,   831,   780,   218,  -100,
     873,   785,   260,   583,   203,   212,  -102,   261,   213,   564,
     565,   -97,   870,   246,   206,   206,     3,   374,   206,   345,
     355,   355,   422,   304,   260,   221,   372,   188,   640,   672,
     520,   227,   528,   648,   531,   281,   260,   260,   260,  -104,
     531,   462,   381,   294,   295,   334,   334,  -478,   791,   334,
    -103,   -99,   251,   607,  -477,  -100,   597,   729,   632,   796,
     453,   374,   456,   237,   460,   447,   641,  -101,   586,   893,
     373,   -99,   917,   -91,   327,   328,   329,   -98,   818,   579,
    -479,  -101,   597,   481,   380,   767,   463,   579,  -485,   429,
     550,  -478,   774,   754,   -91,   546,   430,   -91,  -477,   547,
     -98,   -91,   586,   873,   -92,   408,  -546,   795,   448,   449,
     -97,   -94,   284,  -547,   285,   800,   -89,   375,   287,   945,
     632,   424,   440,   217,  -479,   818,   823,    80,   426,   199,
     806,   199,   199,   392,   275,   730,   831,   284,   392,   285,
     635,   755,   296,   221,   -96,   406,   206,   967,   206,   206,
    -480,   681,   206,   425,   206,   -95,   284,   789,   285,    80,
     893,   375,   284,   692,   285,   468,   469,   470,   471,   728,
      80,    80,   691,   -99,  -546,   -99,   -91,  -100,   244,  -100,
     597,   436,   -93,   407,   820,   409,   -93,   824,   907,  -101,
     261,  -101,   597,   384,  -480,   632,   386,   387,   479,   -98,
     760,   -98,   400,   479,   403,   -90,   421,   260,   218,   277,
     432,   380,   298,   535,   536,   299,   287,   430,  -543,   537,
      80,   206,   206,   206,   206,    80,   206,   206,   303,   859,
     206,   552,    80,   261,   483,   206,  -482,  -544,  -325,   244,
     220,   -70,   467,   815,   799,   221,   199,   636,    56,  -481,
     260,  -483,   334,   334,   334,   334,    67,   473,   474,   406,
    -408,   472,   -84,   206,   832,    80,   -93,   518,   378,   206,
     206,   477,   330,   279,   280,   908,   485,  -472,   383,   911,
    -482,  -325,  -325,   206,   790,   587,   891,   -93,   894,   589,
     -93,  -472,   590,  -481,   -93,  -483,  -475,   240,   478,   580,
     334,   334,  -550,   487,   776,   220,   440,   599,   527,   199,
     206,   206,   601,   385,   545,   331,   332,   709,  -543,  -408,
    -472,  -472,   406,   591,  -543,   447,   206,   974,   388,   921,
     518,   943,  -485,   390,  -280,  -472,   717,  -544,   602,  -475,
    -475,   389,   190,  -544,   109,   284,   440,   285,    80,   191,
     188,   568,   570,   393,   -97,   518,   447,    80,   396,   199,
    -408,  -550,  -408,  -408,   414,   632,   647,   304,   448,   449,
     450,   527,   406,   398,   261,   -89,   206,  -280,  -280,   955,
     518,  -547,   416,   721,    67,   611,   612,   613,   614,   576,
     578,   260,   418,   600,   214,   527,   394,   395,   251,   448,
     449,   452,  -550,   644,  -550,  -550,   447,   779,  -546,   538,
     217,  -475,   674,   674,   261,   304,   325,   326,   327,   328,
     329,   527,   379,   397,   559,   420,   686,   423,   447,   578,
     651,   260,   251,   294,   295,   394,   419,   721,   677,   611,
     612,   613,   614,   240,  -104,  -104,   -69,  -103,   656,   448,
     449,   454,   428,   902,   655,  -475,   663,   199,   668,   904,
     434,   811,   662,   443,    80,   -96,    80,   718,   560,   199,
     406,   448,   449,   458,   206,   417,   645,   437,   518,   447,
     713,   442,   406,   455,   707,   678,   206,   240,    80,   206,
     518,   464,   663,   663,   644,   644,   654,   457,   700,   702,
    -484,   461,   -99,   190,   660,   465,   444,   445,   392,   656,
     191,   188,   714,   440,   420,   918,   438,   439,   484,   527,
     694,   538,   448,   449,   206,   663,   540,  -273,   807,   782,
     779,   527,   466,   656,  -101,   261,  -103,   544,   487,   541,
     660,   660,   572,  -484,  -484,   553,   960,   588,   240,  -281,
     -98,   595,   260,   637,   734,   715,   734,   -95,   734,   925,
     792,   936,   793,   794,   -84,    80,   479,   699,   701,   756,
    -273,  -273,   654,   660,   801,   597,   802,   708,   664,   798,
     603,   261,   542,   543,   206,   206,  -259,   666,   554,   555,
     206,   206,  -281,  -281,   778,   781,   646,   781,   260,   781,
     808,   658,   554,   555,   937,   938,   670,   656,   649,   716,
     720,   814,   737,   732,   659,   735,   206,   738,   656,   206,
      80,   740,   788,   -99,  -260,   763,   765,   762,    80,   201,
     201,   770,   772,   201,   644,   188,  -101,   768,   779,   797,
     813,   748,   410,   827,   -91,   769,   759,   334,   826,   727,
     334,   411,   412,   833,   834,   836,   756,   -93,   852,   233,
     236,    80,    80,   838,   201,   201,   775,   -98,  -261,   674,
     845,   861,   849,   871,   875,   282,   283,   756,   876,   734,
     877,   734,   734,   879,   863,   881,   883,   888,   -90,    80,
     889,   896,  -262,   742,   743,   869,   744,   913,   914,   924,
     874,   206,    44,    45,   916,   847,   935,   927,   851,   594,
     929,    80,   944,   932,   206,   343,   946,   948,    80,    80,
     809,  -546,    80,   610,  -547,   611,   612,   613,   614,   853,
     970,   854,   964,   358,   810,    80,    80,   926,   376,   963,
     377,   862,   843,   273,   370,   334,   864,   734,   734,   912,
     734,   890,   734,   734,   819,     0,   727,   812,   841,   887,
     615,   289,   290,   291,   292,   293,   616,   617,   721,     0,
     611,   612,   613,   614,     0,     0,   260,   899,   751,     0,
     669,    80,   671,     0,     0,     0,   781,   618,     0,   857,
     619,     0,    80,   828,   260,   611,   612,   613,   614,     0,
      80,   905,   906,   620,     0,   615,   201,     0,     0,   201,
     201,   282,   617,     0,     0,     0,     0,     0,   734,   734,
     734,     0,   734,   734,     0,     0,     0,   201,     0,   201,
     201,     0,   618,     0,    80,     0,    80,     0,     0,     0,
       0,     0,    80,     0,    80,   727,     0,   727,   734,   734,
     734,   734,   199,     0,     0,     0,   721,   942,   611,   612,
     613,   614,   518,     0,     0,   406,     0,   668,   781,   206,
       0,     0,   822,   518,     0,   825,     0,     0,     0,     0,
       0,   741,   734,   734,   734,   734,   656,   965,     0,   966,
       0,     0,     0,   722,     0,     0,   734,     0,   104,   723,
     104,     0,     0,     0,   734,   727,   919,     0,     0,     0,
       0,   201,     0,     0,   527,     0,   486,   489,   490,   491,
     492,   493,   494,   495,   496,   497,   498,   499,   500,   501,
     502,   503,   504,   505,   506,   507,   508,   509,   510,   511,
     512,   513,   514,     0,   201,   104,     0,   304,     0,   262,
     727,   610,   727,   611,   612,   613,   614,     0,     0,     0,
       0,     0,   317,   318,     0,     0,     0,     0,     0,     0,
       0,   262,     0,   892,     0,   895,     0,   816,   817,     0,
     727,     0,     0,   347,   356,   356,   356,     0,   615,     0,
       0,   569,   571,     0,   616,   617,   325,   326,   327,   328,
     329,   575,   201,   201,     0,   840,     0,   201,     0,   569,
     571,   201,     0,     0,   920,   618,   922,     0,   619,   747,
     923,   611,   612,   613,   614,     0,     0,   848,   593,     0,
       0,   695,     0,   598,   855,   856,     0,     0,   858,     0,
       0,     0,   201,     0,   610,   201,   611,   612,   613,   614,
       0,   866,   867,     0,     0,     0,   615,   201,   107,     0,
     107,     0,   616,   617,   954,     0,   956,     0,     0,   957,
       0,     0,     0,     0,     0,     0,     0,   638,   639,     0,
       0,   615,     0,   618,     0,   968,   619,   616,   617,   201,
     721,   104,   611,   612,   613,   614,   747,   903,   611,   612,
     613,   614,   748,     0,     0,   107,     0,   976,   618,   263,
     721,   619,   611,   612,   613,   614,   915,     0,     0,     0,
       0,     0,     0,   104,     0,     0,    77,   722,    77,     0,
       0,   263,     0,   615,   104,   104,     0,     0,     0,   616,
     617,     0,     0,   348,   357,   357,     0,   722,     0,     0,
     939,   201,   940,   872,   262,   201,     0,     0,   941,     0,
     618,     0,     0,   619,     0,     0,   304,   201,     0,     0,
       0,     0,   304,    77,     0,     0,     0,     0,     0,     0,
       0,   317,   318,     0,   104,     0,     0,   317,   318,   104,
     201,     0,     0,     0,     0,     0,   104,   262,     0,     0,
       0,     0,     0,     0,     0,     0,   106,     0,   106,     0,
       0,   344,   322,   323,   324,   325,   326,   327,   328,   329,
     324,   325,   326,   327,   328,   329,     0,     0,     0,   104,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     304,   305,   306,   307,   308,   309,   310,   311,     0,   313,
     314,   107,     0,   106,     0,   317,   318,     0,     0,   201,
       0,     0,     0,   761,     0,   764,   766,     0,   736,     0,
     739,   771,   773,     0,     0,     0,     0,     0,     0,   201,
       0,     0,     0,   107,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   107,   107,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   104,     0,   263,     0,     0,     0,   805,    77,
       0,   104,     0,   764,   766,     0,   771,   773,     0,     0,
       0,     0,     0,   201,     0,     0,     0,     0,   262,     0,
       0,     0,     0,     0,   107,     0,     0,     0,     0,   107,
       0,    77,     0,     0,     0,     0,   107,   263,     0,     0,
       0,     0,    77,    77,     0,     0,     0,     0,     0,     0,
       0,   201,     0,     0,     0,   842,     0,     0,   262,     0,
       0,     0,   844,     0,     0,     0,     0,     0,     0,   107,
       0,   835,     0,   837,   839,     0,     0,     0,     0,   106,
       0,     0,   201,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    77,     0,     0,     0,     0,    77,     0,     0,
     844,     0,     0,     0,    77,     0,     0,     0,   104,     0,
     104,   106,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   106,   106,     0,     0,     0,     0,     0,     0,
       0,     0,   104,     0,     0,     0,     0,    77,     0,   878,
     880,     0,   882,     0,   884,   885,     0,     0,     0,     0,
       0,     0,   107,     0,  -568,     0,     0,     0,     0,     0,
       0,   107,  -568,  -568,  -568,     0,     0,  -568,  -568,  -568,
       0,  -568,   106,     0,     0,     0,     0,   106,   263,   262,
       0,  -568,     0,     0,   106,     0,     0,     0,     0,     0,
       0,  -568,  -568,     0,  -568,  -568,  -568,  -568,  -568,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   104,
     928,   930,   931,     0,   933,   934,     0,   106,   263,     0,
      77,     0,     0,     0,     0,   262,     0,     0,     0,    77,
     201,     0,     0,     0,     0,     0,     0,     0,     0,  -568,
     947,   949,   950,   951,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   104,     0,     0,     0,   107,     0,
     107,     0,   104,     0,   969,   971,   972,   973,     0,     0,
       0,  -568,     0,  -568,     0,     0,   217,  -568,   975,  -568,
       0,  -568,   107,     0,     0,     0,   977,     0,     0,     0,
     106,     0,     0,     0,     0,   104,   104,     0,     0,   106,
       0,     0,     0,     0,     0,     0,     0,     0,   650,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   104,     0,     0,    77,     0,    77,   263,
       0,     0,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   104,     0,   317,   318,     0,
      77,     0,   104,   104,     0,     0,   104,     0,     0,   107,
     746,     0,     0,     0,     0,     0,     0,     0,     0,   104,
     104,     0,     0,     0,   319,   263,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,     0,     0,     0,     0,
       0,     0,     0,   356,  -238,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   106,     0,   106,     0,
       0,   900,     0,     0,   107,   104,     0,     0,     0,     0,
       0,     0,   107,     0,     0,     0,   104,    77,     0,     0,
     106,     0,   562,   524,   104,     0,   563,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   107,   107,   164,   165,     0,
       0,   166,   167,   168,   169,     0,     0,     0,   104,     0,
     104,     0,     0,     0,     0,   170,   104,     0,   104,     0,
       0,     0,    77,   107,     0,     0,     0,     0,     0,     0,
      77,     0,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,     0,   181,   182,   107,     0,   106,     0,     0,
       0,   183,   107,   107,     0,     0,   107,     0,     0,     0,
       0,     0,     0,    77,    77,     0,     0,     0,     0,   107,
     107,     0,     0,     0,     0,   235,   235,     0,     0,     0,
     235,   235,   235,     0,     0,     0,     0,     0,     0,     0,
       0,    77,   235,   357,     0,     0,     0,     0,     0,     0,
       0,     0,   106,     0,   235,     0,     0,     0,     0,     0,
     106,   901,     0,    77,     0,   107,   235,   235,   235,     0,
      77,    77,     0,     0,    77,     0,   107,     0,     0,     0,
       0,     0,     0,     0,   107,     0,     0,    77,    77,     0,
       0,     0,     0,   106,   106,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   107,     0,
     107,   106,     0,     0,     0,     0,   107,     0,   107,   898,
       0,     0,     0,    77,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   106,    77,     0,     0,     0,     0,     0,
     106,   106,    77,     0,   106,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   106,   106,     0,
       0,     0,   235,     0,     0,   235,   235,   235,     0,     0,
       0,     0,     0,     0,     0,     0,    77,     0,    77,     0,
       0,     0,     0,     0,    77,     0,    77,     0,     0,     0,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   106,     0,   317,   318,     0,     0,     0,
       0,     0,     0,     0,   106,     0,     0,     0,     0,     0,
       0,     0,   106,     0,     0,     0,     0,   235,     0,     0,
       0,     0,   319,     0,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   106,     0,   106,   221,
       0,     0,     0,     0,   106,     0,   106,     0,     0,     0,
     235,     0,     0,   235,   235,   235,   235,   235,   235,   235,
     235,   235,   235,   235,   235,   235,   235,   235,   235,   235,
     235,   235,   235,   235,   235,   235,   235,   235,   235,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   515,   516,
       0,     0,   517,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   155,   156,   157,   158,   159,   160,   161,   162,
     163,     0,     0,   164,   165,     0,     0,   166,   167,   168,
     169,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   170,     0,     0,     0,     0,     0,   235,   235,   235,
       0,     0,     0,     0,     0,   235,   235,   235,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,     0,   181,
     182,     0,     0,     0,   235,     0,     0,   183,   217,   235,
       0,     0,     0,     0,     0,     0,     0,     0,   235,     0,
       0,   235,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   235,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   235,   235,     0,  -568,     4,     0,     5,
       6,     7,     8,     9,     0,   235,     0,    10,    11,     0,
       0,   235,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,     0,    41,    42,     0,    43,    44,
      45,     0,    46,    47,     0,     0,     0,   235,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,     0,     0,    49,    50,     0,    51,    52,
       0,    53,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     0,     0,   235,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    61,    62,
      63,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -568,   235,  -568,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -280,     0,     0,     0,     0,     0,     0,
       0,  -280,  -280,  -280,     0,     0,  -280,  -280,  -280,     0,
    -280,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -280,  -280,  -280,     0,     0,   235,     0,     0,   235,   235,
    -280,  -280,     0,  -280,  -280,  -280,  -280,  -280,     0,     0,
       0,     0,     0,     0,     0,   235,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -280,  -280,  -280,  -280,  -280,  -280,  -280,  -280,  -280,
    -280,  -280,  -280,  -280,     0,     0,  -280,  -280,  -280,     0,
     711,  -280,     0,     0,   235,     0,     0,  -280,     0,   235,
     235,     0,   235,   235,     0,     0,     0,     0,     0,   235,
    -280,     0,  -102,  -280,  -280,  -280,  -280,  -280,  -280,  -280,
    -280,  -280,  -280,  -280,  -280,     0,     0,     0,     0,     0,
    -280,  -280,  -280,  -280,     0,     0,  -280,  -280,  -280,     0,
    -280,     0,     0,     0,     0,     0,     0,   235,     0,     0,
       0,   235,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -407,     0,     0,
       0,     0,     0,     0,     0,  -407,  -407,  -407,   235,     0,
    -407,  -407,  -407,     0,  -407,     0,     0,     0,     0,     0,
       0,     0,     0,  -407,  -407,  -407,   235,     0,     0,     0,
       0,     0,     0,     0,  -407,  -407,     0,  -407,  -407,  -407,
    -407,  -407,     0,     0,     0,     0,   235,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   235,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,     0,     0,
    -407,  -407,  -407,     0,     0,  -407,     0,     0,     0,     0,
       0,  -407,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -407,     0,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,     0,
       0,     0,     0,  -407,  -407,  -407,  -407,  -407,  -274,   217,
    -407,  -407,  -407,     0,  -407,     0,  -274,  -274,  -274,     0,
       0,  -274,  -274,  -274,     0,  -274,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -274,  -274,  -274,     0,     0,
       0,     0,     0,     0,     0,  -274,  -274,     0,  -274,  -274,
    -274,  -274,  -274,     0,     0,     0,     0,     0,     0,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,     0,     0,   317,   318,  -274,  -274,  -274,  -274,
    -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,     0,
       0,  -274,  -274,  -274,     0,     0,  -274,     0,     0,     0,
       0,   319,  -274,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,     0,     0,  -274,     0,     0,  -274,  -274,
    -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,
       0,     0,     0,     0,     0,  -274,  -274,  -274,  -274,  -568,
       0,  -274,  -274,  -274,     0,  -274,     0,  -568,  -568,  -568,
       0,     0,  -568,  -568,  -568,     0,  -568,     0,     0,     0,
       0,     0,     0,     0,     0,  -568,  -568,  -568,     0,     0,
       0,     0,     0,     0,     0,     0,  -568,  -568,     0,  -568,
    -568,  -568,  -568,  -568,     0,     0,     0,     0,     0,     0,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,  -569,  -569,     0,     0,   317,   318,  -568,  -568,  -568,
    -568,  -568,  -568,  -568,  -568,  -568,  -568,  -568,  -568,  -568,
       0,     0,  -568,  -568,  -568,     0,     0,  -568,     0,     0,
       0,     0,     0,  -568,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,     0,     0,     0,     0,     0,  -568,
       0,  -568,  -568,  -568,  -568,  -568,  -568,  -568,  -568,  -568,
    -568,     0,     0,     0,     0,  -568,  -568,  -568,  -568,  -568,
    -287,   217,  -568,  -568,  -568,     0,  -568,     0,  -287,  -287,
    -287,     0,     0,  -287,  -287,  -287,     0,  -287,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -287,  -287,     0,
       0,     0,     0,     0,     0,     0,     0,  -287,  -287,     0,
    -287,  -287,  -287,  -287,  -287,     0,     0,     0,     0,     0,
       0,   304,  -569,  -569,  -569,  -569,   309,   310,     0,     0,
    -569,  -569,     0,     0,     0,     0,   317,   318,  -287,  -287,
    -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,
    -287,     0,     0,  -287,  -287,  -287,     0,     0,  -287,     0,
       0,     0,     0,     0,  -287,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,     0,     0,     0,     0,     0,
    -287,     0,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,
    -287,  -287,     0,     0,     0,     0,     0,  -287,  -287,  -287,
    -287,  -550,   214,  -287,  -287,  -287,     0,  -287,     0,  -550,
    -550,  -550,     0,     0,     0,  -550,  -550,     0,  -550,     0,
       0,     0,     0,   705,     0,     0,     0,  -550,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -550,  -550,
       0,  -550,  -550,  -550,  -550,  -550,     0,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
       0,     0,   317,   318,     0,     0,     0,     0,     0,  -550,
    -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,
    -550,  -550,     0,     0,  -550,  -550,  -550,     0,   652,   319,
       0,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -100,  -550,     0,  -550,  -550,  -550,  -550,  -550,  -550,  -550,
    -550,  -550,  -550,     0,     0,     0,     0,  -550,  -550,  -550,
    -550,   -92,  -280,     0,  -550,     0,  -550,     0,  -550,     0,
    -280,  -280,  -280,     0,     0,     0,  -280,  -280,     0,  -280,
       0,     0,     0,     0,   650,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -280,
    -280,     0,  -280,  -280,  -280,  -280,  -280,     0,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,     0,     0,   317,   318,     0,     0,     0,     0,     0,
    -280,  -280,  -280,  -280,  -280,  -280,  -280,  -280,  -280,  -280,
    -280,  -280,  -280,     0,     0,  -280,  -280,  -280,     0,   653,
     319,     0,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -102,  -280,     0,  -280,  -280,  -280,  -280,  -280,  -280,
    -280,  -280,  -280,  -280,     0,     0,     0,     0,     0,  -280,
    -280,  -280,   -94,     0,     0,  -280,     0,  -280,   238,  -280,
       5,     6,     7,     8,     9,  -568,  -568,  -568,    10,    11,
       0,     0,  -568,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,    27,     0,     0,     0,
       0,     0,    28,    29,     0,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,     0,    41,    42,     0,    43,
      44,    45,     0,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,     0,     0,    49,    50,     0,    51,
      52,     0,    53,     0,    54,    55,    56,    57,    58,    59,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    61,
      62,    63,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -568,   238,  -568,     5,     6,     7,     8,     9,
       0,     0,  -568,    10,    11,     0,  -568,  -568,    12,     0,
      13,    14,    15,    16,    17,    18,    19,     0,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,     0,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
       0,    41,    42,     0,    43,    44,    45,     0,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,     0,
       0,    49,    50,     0,    51,    52,     0,    53,     0,    54,
      55,    56,    57,    58,    59,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    61,    62,    63,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -568,   238,  -568,
       5,     6,     7,     8,     9,     0,     0,  -568,    10,    11,
       0,     0,  -568,    12,  -568,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,    27,     0,     0,     0,
       0,     0,    28,    29,     0,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,     0,    41,    42,     0,    43,
      44,    45,     0,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,     0,     0,    49,    50,     0,    51,
      52,     0,    53,     0,    54,    55,    56,    57,    58,    59,
      60,     0,     0,     0,     0,     0,     0,     4,     0,     5,
       6,     7,     8,     9,     0,     0,     0,    10,    11,    61,
      62,    63,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,  -568,     0,  -568,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,     0,    41,    42,     0,    43,    44,
      45,     0,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,     0,     0,    49,    50,     0,    51,    52,
       0,    53,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    61,    62,
      63,     0,     0,  -568,     0,     0,     0,     0,     0,     0,
       0,  -568,   238,  -568,     5,     6,     7,     8,     9,     0,
       0,  -568,    10,    11,     0,     0,  -568,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,    29,     0,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
      41,    42,     0,    43,    44,    45,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    48,     0,     0,
      49,    50,     0,    51,    52,     0,    53,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     0,
       0,   238,     0,     5,     6,     7,     8,     9,     0,  -568,
    -568,    10,    11,    61,    62,    63,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,  -568,     0,  -568,     0,
      20,    21,    22,    23,    24,    25,    26,     0,     0,    27,
       0,     0,     0,     0,     0,    28,    29,     0,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,     0,    41,
      42,     0,    43,    44,    45,     0,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,     0,     0,    49,
      50,     0,    51,    52,     0,    53,     0,    54,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     0,     0,
     238,     0,     5,     6,     7,     8,     9,     0,     0,     0,
      10,    11,    61,    62,    63,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,  -568,     0,  -568,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,    28,    29,     0,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
       0,    43,    44,    45,     0,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    48,     0,     0,   239,    50,
       0,    51,    52,     0,    53,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    61,    62,    63,     0,     0,     0,     0,     0,     0,
       0,     0,  -568,     0,  -568,   238,  -568,     5,     6,     7,
       8,     9,     0,     0,     0,    10,    11,     0,     0,     0,
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
       0,     0,     0,     0,     0,     0,     0,  -568,     0,  -568,
     238,  -568,     5,     6,     7,     8,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,     0,    13,    14,    15,
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
       0,    61,    62,    63,     0,     0,  -568,     0,     0,     0,
       0,     0,     0,     0,  -568,   238,  -568,     5,     6,     7,
       8,     9,     0,     0,  -568,    10,    11,     0,     0,     0,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,    28,
      29,     0,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    61,    62,    63,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,  -568,
       0,  -568,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,   192,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,   193,    41,    42,     0,    43,    44,    45,     0,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   194,
       0,     0,   195,    50,     0,    51,    52,     0,   196,   197,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    61,   198,    63,    12,     0,
      13,    14,    15,    16,    17,    18,    19,     0,     0,     0,
     221,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
       0,    41,    42,     0,    43,    44,    45,     0,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   194,     0,
       0,   195,    50,     0,    51,    52,     0,     0,     0,    54,
      55,    56,    57,    58,    59,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    61,    62,    63,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,   284,     0,   285,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
      41,    42,     0,    43,    44,    45,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   194,     0,     0,
     195,    50,     0,    51,    52,     0,     0,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     8,     9,     0,     0,
       0,    10,    11,    61,    62,    63,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,   221,     0,
      20,    21,    22,    23,    24,    25,    26,     0,     0,    27,
       0,     0,     0,     0,     0,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,     0,    41,
      42,     0,    43,    44,    45,     0,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,     0,     0,    49,
      50,     0,    51,    52,     0,    53,     0,    54,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     8,     9,     0,     0,     0,
      10,    11,    61,    62,    63,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,   466,     0,     0,     0,    20,
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
       0,     0,     0,     0,   466,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,     0,
       0,     0,   135,   136,   137,   359,   360,   361,   362,   142,
     143,   144,     0,     0,     0,     0,     0,   145,   146,   147,
     148,   363,   364,   365,   366,   153,    37,    38,   367,    40,
       0,     0,     0,     0,     0,     0,     0,     0,   155,   156,
     157,   158,   159,   160,   161,   162,   163,     0,     0,   164,
     165,     0,     0,   166,   167,   168,   169,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   170,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,     0,   181,   182,  -543,  -543,  -543,
       0,  -543,     0,   183,   368,  -543,  -543,     0,     0,     0,
    -543,     0,  -543,  -543,  -543,  -543,  -543,  -543,  -543,     0,
    -543,     0,     0,     0,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,     0,     0,  -543,     0,     0,     0,     0,     0,     0,
    -543,     0,     0,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,     0,  -543,  -543,  -543,     0,
    -543,  -543,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -543,     0,     0,  -543,  -543,     0,  -543,  -543,     0,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -543,  -543,  -543,     0,
    -543,  -544,  -544,  -544,     0,  -544,  -543,     0,     0,  -544,
    -544,     0,     0,     0,  -544,     0,  -544,  -544,  -544,  -544,
    -544,  -544,  -544,     0,  -544,     0,     0,     0,  -544,  -544,
    -544,  -544,  -544,  -544,  -544,     0,     0,  -544,     0,     0,
       0,     0,     0,     0,  -544,     0,     0,  -544,  -544,  -544,
    -544,  -544,  -544,  -544,  -544,  -544,  -544,  -544,  -544,     0,
    -544,  -544,  -544,     0,  -544,  -544,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -544,     0,     0,  -544,  -544,     0,
    -544,  -544,     0,  -544,  -544,  -544,  -544,  -544,  -544,  -544,
    -544,  -544,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -544,  -544,  -544,     0,  -544,  -546,  -546,  -546,     0,  -546,
    -544,     0,     0,  -546,  -546,     0,     0,     0,  -546,     0,
    -546,  -546,  -546,  -546,  -546,  -546,  -546,     0,     0,     0,
       0,     0,  -546,  -546,  -546,  -546,  -546,  -546,  -546,     0,
       0,  -546,     0,     0,     0,     0,     0,     0,  -546,     0,
       0,  -546,  -546,  -546,  -546,  -546,  -546,  -546,  -546,  -546,
    -546,  -546,  -546,     0,  -546,  -546,  -546,     0,  -546,  -546,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -546,   710,
       0,  -546,  -546,     0,  -546,  -546,     0,  -546,  -546,  -546,
    -546,  -546,  -546,  -546,  -546,  -546,     0,     0,     0,     0,
       0,  -100,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -546,  -546,  -546,  -548,  -548,  -548,
       0,  -548,     0,     0,  -546,  -548,  -548,     0,     0,     0,
    -548,     0,  -548,  -548,  -548,  -548,  -548,  -548,  -548,     0,
       0,     0,     0,     0,  -548,  -548,  -548,  -548,  -548,  -548,
    -548,     0,     0,  -548,     0,     0,     0,     0,     0,     0,
    -548,     0,     0,  -548,  -548,  -548,  -548,  -548,  -548,  -548,
    -548,  -548,  -548,  -548,  -548,     0,  -548,  -548,  -548,     0,
    -548,  -548,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -548,     0,     0,  -548,  -548,     0,  -548,  -548,     0,  -548,
    -548,  -548,  -548,  -548,  -548,  -548,  -548,  -548,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -548,  -548,  -548,  -549,
    -549,  -549,     0,  -549,     0,     0,  -548,  -549,  -549,     0,
       0,     0,  -549,     0,  -549,  -549,  -549,  -549,  -549,  -549,
    -549,     0,     0,     0,     0,     0,  -549,  -549,  -549,  -549,
    -549,  -549,  -549,     0,     0,  -549,     0,     0,     0,     0,
       0,     0,  -549,     0,     0,  -549,  -549,  -549,  -549,  -549,
    -549,  -549,  -549,  -549,  -549,  -549,  -549,     0,  -549,  -549,
    -549,     0,  -549,  -549,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -549,     0,     0,  -549,  -549,     0,  -549,  -549,
       0,  -549,  -549,  -549,  -549,  -549,  -549,  -549,  -549,  -549,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -549,  -549,
    -549,     0,     0,     0,     0,     0,     0,     0,  -549,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,     0,     0,     0,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,     0,     0,     0,     0,
       0,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     266,   267,   154,   268,     0,     0,     0,     0,     0,     0,
       0,     0,   155,   156,   157,   158,   159,   160,   161,   162,
     163,     0,     0,   164,   165,     0,     0,   166,   167,   168,
     169,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   170,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,     0,   181,
     182,     0,     0,     0,     0,     0,     0,   183,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,     0,     0,     0,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,     0,     0,     0,     0,     0,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   223,
       0,   154,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   155,   156,   157,   158,   159,   160,   161,   162,   163,
       0,     0,   164,   165,     0,     0,   166,   167,   168,   169,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     170,     0,     0,    55,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,     0,   181,   182,
       0,     0,     0,     0,     0,     0,   183,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,     0,     0,     0,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,     0,     0,     0,     0,     0,   145,
     146,   147,   148,   149,   150,   151,   152,   153,     0,     0,
     154,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     155,   156,   157,   158,   159,   160,   161,   162,   163,     0,
       0,   164,   165,     0,     0,   166,   167,   168,   169,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   170,
       0,     0,    55,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,     0,   181,   182,     0,
       0,     0,     0,     0,     0,   183,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
       0,     0,     0,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,     0,     0,     0,     0,     0,   145,   146,
     147,   148,   149,   150,   151,   152,   153,     0,     0,   154,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   155,
     156,   157,   158,   159,   160,   161,   162,   163,     0,     0,
     164,   165,     0,     0,   166,   167,   168,   169,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   170,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,     0,   181,   182,     5,     6,
       7,     0,     9,     0,   183,     0,    10,    11,     0,     0,
       0,    12,     0,    13,    14,    15,   228,   229,    18,    19,
       0,     0,     0,     0,     0,   230,   231,   232,    23,    24,
      25,    26,     0,     0,   192,     0,     0,     0,     0,     0,
       0,   255,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,     0,    41,    42,     0,    43,    44,    45,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   256,     0,     0,   195,    50,     0,    51,    52,     0,
       0,     0,    54,    55,    56,    57,    58,    59,    60,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,     0,     0,     0,    12,   257,    13,    14,
      15,   228,   229,    18,    19,   258,     0,     0,     0,     0,
     230,   231,   232,    23,    24,    25,    26,     0,     0,   192,
       0,     0,     0,     0,     0,     0,   255,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,     0,    41,
      42,     0,    43,    44,    45,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   256,     0,     0,   195,
      50,     0,    51,    52,     0,     0,     0,    54,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     5,     6,
       7,     8,     9,     0,     0,     0,    10,    11,     0,     0,
       0,    12,   257,    13,    14,    15,    16,    17,    18,    19,
     482,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,     0,    41,    42,     0,    43,    44,    45,
       0,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    48,     0,     0,    49,    50,     0,    51,    52,     0,
      53,     0,    54,    55,    56,    57,    58,    59,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       8,     9,     0,     0,     0,    10,    11,    61,    62,    63,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,    28,
      29,     0,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    61,    62,    63,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,   192,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,   193,    41,    42,     0,    43,    44,    45,     0,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   194,
       0,     0,   195,    50,     0,    51,    52,     0,   196,   197,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    61,   198,    63,    12,     0,
      13,    14,    15,   228,   229,    18,    19,     0,     0,     0,
       0,     0,   230,   231,   232,    23,    24,    25,    26,     0,
       0,   192,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
     193,    41,    42,     0,    43,    44,    45,     0,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   194,     0,
       0,   195,    50,     0,    51,    52,     0,   577,   197,    54,
      55,    56,    57,    58,    59,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    61,   198,    63,    12,     0,    13,
      14,    15,   228,   229,    18,    19,     0,     0,     0,     0,
       0,   230,   231,   232,    23,    24,    25,    26,     0,     0,
     192,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,   193,
      41,    42,     0,    43,    44,    45,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   194,     0,     0,
     195,    50,     0,    51,    52,     0,   196,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    61,   198,    63,    12,     0,    13,    14,
      15,   228,   229,    18,    19,     0,     0,     0,     0,     0,
     230,   231,   232,    23,    24,    25,    26,     0,     0,   192,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,   193,    41,
      42,     0,    43,    44,    45,     0,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   194,     0,     0,   195,
      50,     0,    51,    52,     0,     0,   197,    54,    55,    56,
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
       0,    51,    52,     0,   577,     0,    54,    55,    56,    57,
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
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      61,   198,    63,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,   192,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,     0,    41,    42,     0,    43,
      44,    45,     0,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   194,     0,     0,   195,    50,     0,    51,
      52,     0,   476,     0,    54,    55,    56,    57,    58,    59,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    61,
     198,    63,    12,     0,    13,    14,    15,   228,   229,    18,
      19,     0,     0,     0,     0,     0,   230,   231,   232,    23,
      24,    25,    26,     0,     0,   192,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,     0,    41,    42,     0,    43,    44,
      45,     0,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   194,     0,     0,   195,    50,     0,    51,    52,
       0,   196,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    61,   198,
      63,    12,     0,    13,    14,    15,   228,   229,    18,    19,
       0,     0,     0,     0,     0,   230,   231,   232,    23,    24,
      25,    26,     0,     0,   192,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,     0,    41,    42,     0,    43,    44,    45,
       0,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   194,     0,     0,   195,    50,     0,    51,    52,     0,
     758,     0,    54,    55,    56,    57,    58,    59,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    61,   198,    63,
      12,     0,    13,    14,    15,   228,   229,    18,    19,     0,
       0,     0,     0,     0,   230,   231,   232,    23,    24,    25,
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
       0,     0,   195,    50,     0,    51,    52,     0,   577,     0,
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
       0,   195,    50,     0,    51,    52,     0,     0,     0,    54,
      55,    56,    57,    58,    59,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    61,   198,    63,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
      41,    42,     0,    43,    44,    45,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   194,     0,     0,
     195,    50,     0,    51,    52,     0,     0,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    61,    62,    63,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,     0,
      20,    21,    22,    23,    24,    25,    26,     0,     0,   192,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,     0,    41,
      42,     0,    43,    44,    45,     0,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   194,     0,     0,   195,
      50,     0,    51,    52,     0,     0,     0,    54,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    61,   198,    63,    12,     0,    13,    14,    15,
     228,   229,    18,    19,     0,     0,     0,     0,     0,   230,
     231,   232,    23,    24,    25,    26,     0,     0,   192,     0,
       0,     0,     0,     0,     0,   255,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
       0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   256,     0,     0,   300,    50,
       0,    51,    52,     0,   301,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,     0,     0,     0,
      12,   257,    13,    14,    15,   228,   229,    18,    19,     0,
       0,     0,     0,     0,   230,   231,   232,    23,    24,    25,
      26,     0,     0,   192,     0,     0,     0,     0,     0,     0,
     255,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     342,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   257,    13,    14,    15,
     228,   229,    18,    19,     0,     0,     0,     0,     0,   230,
     231,   232,    23,    24,    25,    26,     0,     0,   192,     0,
       0,     0,     0,     0,     0,   255,     0,     0,    32,    33,
      34,   349,    36,    37,    38,   350,    40,     0,    41,    42,
       0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   351,     0,     0,   352,     0,     0,   195,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,     0,     0,     0,
      12,   257,    13,    14,    15,   228,   229,    18,    19,     0,
       0,     0,     0,     0,   230,   231,   232,    23,    24,    25,
      26,     0,     0,   192,     0,     0,     0,     0,     0,     0,
     255,     0,     0,    32,    33,    34,   349,    36,    37,    38,
     350,    40,     0,    41,    42,     0,    43,    44,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     352,     0,     0,   195,    50,     0,    51,    52,     0,     0,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   257,    13,    14,    15,
     228,   229,    18,    19,     0,     0,     0,     0,     0,   230,
     231,   232,    23,    24,    25,    26,     0,     0,   192,     0,
       0,     0,     0,     0,     0,   255,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
       0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   256,     0,     0,   300,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,     0,     0,     0,
      12,   257,    13,    14,    15,   228,   229,    18,    19,     0,
       0,     0,     0,     0,   230,   231,   232,    23,    24,    25,
      26,     0,     0,   192,     0,     0,     0,     0,     0,     0,
     255,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     886,     0,     0,   195,    50,     0,    51,    52,     0,     0,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   257,    13,    14,    15,
     228,   229,    18,    19,     0,     0,     0,     0,     0,   230,
     231,   232,    23,    24,    25,    26,     0,     0,   192,     0,
       0,     0,     0,     0,     0,   255,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
       0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   897,     0,     0,   195,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,   523,   524,
       0,     0,   525,     0,     0,     0,     0,     0,     0,     0,
       0,   257,   155,   156,   157,   158,   159,   160,   161,   162,
     163,     0,     0,   164,   165,     0,     0,   166,   167,   168,
     169,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   170,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,     0,   181,
     182,   581,   516,     0,     0,   582,     0,   183,   217,     0,
       0,     0,     0,     0,     0,   155,   156,   157,   158,   159,
     160,   161,   162,   163,     0,     0,   164,   165,     0,     0,
     166,   167,   168,   169,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   170,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,     0,   181,   182,   584,   524,     0,     0,   585,     0,
     183,   217,     0,     0,     0,     0,     0,     0,   155,   156,
     157,   158,   159,   160,   161,   162,   163,     0,     0,   164,
     165,     0,     0,   166,   167,   168,   169,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   170,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,     0,   181,   182,   605,   516,     0,
       0,   606,     0,   183,   217,     0,     0,     0,     0,     0,
       0,   155,   156,   157,   158,   159,   160,   161,   162,   163,
       0,     0,   164,   165,     0,     0,   166,   167,   168,   169,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     170,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,     0,   181,   182,
     608,   524,     0,     0,   609,     0,   183,   217,     0,     0,
       0,     0,     0,     0,   155,   156,   157,   158,   159,   160,
     161,   162,   163,     0,     0,   164,   165,     0,     0,   166,
     167,   168,   169,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   170,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
       0,   181,   182,   679,   516,     0,     0,   680,     0,   183,
     217,     0,     0,     0,     0,     0,     0,   155,   156,   157,
     158,   159,   160,   161,   162,   163,     0,     0,   164,   165,
       0,     0,   166,   167,   168,   169,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   170,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,     0,   181,   182,   682,   524,     0,     0,
     683,     0,   183,   217,     0,     0,     0,     0,     0,     0,
     155,   156,   157,   158,   159,   160,   161,   162,   163,     0,
       0,   164,   165,     0,     0,   166,   167,   168,   169,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   170,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,     0,   181,   182,   689,
     516,     0,     0,   690,     0,   183,   217,     0,     0,     0,
       0,     0,     0,   155,   156,   157,   158,   159,   160,   161,
     162,   163,     0,     0,   164,   165,     0,     0,   166,   167,
     168,   169,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   170,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,     0,
     181,   182,   562,   524,     0,     0,   563,     0,   183,   217,
       0,     0,     0,     0,     0,     0,   155,   156,   157,   158,
     159,   160,   161,   162,   163,     0,     0,   164,   165,     0,
       0,   166,   167,   168,   169,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   170,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,     0,   181,   182,   952,   516,     0,     0,   953,
       0,   183,   217,     0,     0,     0,     0,     0,     0,   155,
     156,   157,   158,   159,   160,   161,   162,   163,     0,     0,
     164,   165,     0,     0,   166,   167,   168,   169,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   170,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,     0,   181,   182,   958,   516,
       0,     0,   959,     0,   183,   217,     0,     0,     0,     0,
       0,     0,   155,   156,   157,   158,   159,   160,   161,   162,
     163,     0,     0,   164,   165,     0,     0,   166,   167,   168,
     169,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   170,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,     0,   181,
     182,   961,   524,     0,     0,   962,     0,   183,   217,     0,
       0,     0,     0,     0,     0,   155,   156,   157,   158,   159,
     160,   161,   162,   163,     0,     0,   164,   165,     0,     0,
     166,   167,   168,   169,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   170,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,     0,   181,   182,     0,     0,     0,     0,     0,     0,
     183,   217,   304,   305,   306,   307,   308,   309,   310,     0,
       0,   313,   314,     0,     0,     0,     0,   317,   318,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329
};

static const yytype_int16 yycheck[] =
{
       2,    16,    17,    55,     2,    20,     4,     8,    28,    75,
     331,   303,    65,   336,     8,     8,   446,   210,    16,    17,
     583,    51,    20,    27,   537,    29,    22,    28,     4,    83,
      51,    52,    86,    86,    28,    28,    86,   330,    90,   332,
       2,   352,     4,   536,   607,   556,   732,    65,     1,    51,
      52,    49,    50,   249,   735,    53,    53,   253,    49,    25,
     566,   567,    55,   442,    62,   446,   748,   667,    22,    25,
     813,   671,    53,   394,    16,    17,    25,    75,    20,   372,
     373,    25,   811,    50,    82,    83,     0,    26,    86,    87,
      88,    89,   239,    68,    75,   137,    85,    90,    76,    27,
     393,   126,   395,   482,    13,    62,    87,    88,    89,    25,
      13,    61,   108,    37,    38,    82,    83,    85,   681,    86,
      25,    13,    52,   444,    85,    13,   419,    29,   558,   692,
     272,    26,   274,    56,   276,    61,   114,    13,   398,   836,
     129,    25,   871,   109,   119,   120,   121,    13,   834,   390,
      85,    25,   445,   300,   108,   655,   106,   398,    87,   130,
     113,   129,   662,   641,   130,    52,   137,   133,   129,    56,
      25,   137,   432,   916,   130,   220,   132,   688,   104,   105,
     109,   130,   135,   132,   137,   696,   130,   126,   241,   918,
     620,   241,   258,   132,   129,   881,   877,   195,   243,   214,
     706,   216,   217,   205,   136,   107,   888,   135,   210,   137,
     136,   641,    28,   137,   130,   217,   214,   946,   216,   217,
      85,   542,   220,   241,   222,   130,   135,   130,   137,   227,
     927,   126,   135,   554,   137,   289,   290,   291,   292,   620,
     238,   239,   553,   135,   132,   137,   130,   135,   239,   137,
     543,   255,    25,   220,   732,   222,   130,   735,   858,   135,
     258,   137,   555,   193,   129,   695,   196,   197,   298,   135,
     649,   137,   214,   303,   216,   130,   238,   258,   232,   136,
     130,   235,   109,   337,   338,   130,   339,   137,    26,   339,
     288,   289,   290,   291,   292,   293,   294,   295,   109,   792,
     298,   354,   300,   301,   301,   303,    85,    26,    85,   300,
     132,   109,   288,   728,   695,   137,   331,   459,    99,    85,
     301,    85,   289,   290,   291,   292,   288,   294,   295,   331,
      26,   293,   130,   331,   749,   333,   109,   330,    87,   337,
     338,   298,    85,    59,    60,   858,   303,    85,   130,   860,
     129,   128,   129,   351,   677,   400,   834,   130,   836,   404,
     133,    85,   407,   129,   137,   129,    85,    49,   298,   390,
     337,   338,    26,   303,   666,   132,   442,   422,   332,   394,
     378,   379,   427,    56,   351,   128,   129,   580,   126,    85,
     128,   129,   394,   413,   132,    61,   394,   960,   109,   877,
     393,   912,    87,   130,    85,   129,   602,   126,   429,   128,
     129,    88,   413,   132,   416,   135,   482,   137,   416,   413,
     413,   378,   379,    85,   109,   418,    61,   425,    87,   444,
     126,    85,   128,   129,   132,   865,   481,    68,   104,   105,
     106,   395,   444,   130,   442,   130,   444,   128,   129,   927,
     443,   132,   126,    52,   416,    54,    55,    56,    57,   389,
     390,   442,    85,   425,   132,   419,   128,   129,   398,   104,
     105,   106,   126,   475,   128,   129,    61,    15,   132,    17,
     132,    85,   535,   536,   482,    68,   117,   118,   119,   120,
     121,   445,    87,    87,    85,    87,   549,   133,    61,   429,
     504,   482,   432,    37,    38,   128,   129,    52,   538,    54,
      55,    56,    57,   195,   109,   109,   109,   109,   522,   104,
     105,   106,   134,   846,   520,   129,   528,   542,   530,   852,
     127,   130,   528,    85,   532,   130,   534,   603,   129,   554,
     542,   104,   105,   106,   542,   227,   476,    56,   541,    61,
      87,   130,   554,    65,   574,   539,   554,   239,   556,   557,
     553,    68,   564,   565,   566,   567,   520,   106,   564,   565,
      85,   106,   109,   574,   528,    68,   128,   129,   580,   583,
     574,   574,    87,   649,    87,   130,    59,    60,   130,   543,
     557,    17,   104,   105,   592,   597,    56,    85,    87,    14,
      15,   555,   135,   607,   109,   603,   109,    25,   538,    85,
     564,   565,    94,   128,   129,    85,   937,   133,   300,    85,
     109,   127,   603,   136,   626,   592,   628,   130,   630,    85,
     684,    85,   685,   687,   130,   633,   666,   564,   565,   641,
     128,   129,   596,   597,   697,   938,   698,   577,    10,   694,
     130,   649,   128,   129,   652,   653,   130,     8,   128,   129,
     658,   659,   128,   129,   666,   667,   130,   669,   649,   671,
     715,    87,   128,   129,   128,   129,    13,   681,   130,   127,
     109,   726,    52,   130,    87,   130,   684,   130,   692,   687,
     688,    52,   676,   109,   130,   652,   653,   111,   696,    16,
      17,   658,   659,    20,   706,   698,   109,   134,    15,   693,
     130,   135,    54,   109,   130,    87,   646,   684,   127,   615,
     687,    63,    64,   114,   130,   130,   728,   130,   782,    46,
      47,   729,   730,   130,    51,    52,   666,   109,   130,   792,
      88,   794,     9,   130,    10,    62,    63,   749,   127,   751,
     130,   753,   754,   130,   799,   130,   130,   130,   130,   757,
     114,    10,   130,    54,    55,   810,    57,    10,   127,    56,
     815,   769,    63,    64,   130,   777,    56,   130,   780,   416,
     130,   779,   127,   130,   782,    87,   130,   130,   786,   787,
     720,   132,   790,    52,   132,    54,    55,    56,    57,   783,
     130,   785,   940,    89,   722,   803,   804,   888,    92,   939,
      95,   795,   769,    57,    90,   782,   800,   819,   820,   865,
     822,   834,   824,   825,   732,    -1,   722,   723,   758,   827,
      89,    40,    41,    42,    43,    44,    95,    96,    52,    -1,
      54,    55,    56,    57,    -1,    -1,   827,   845,   641,    -1,
     532,   849,   534,    -1,    -1,    -1,   858,   116,    -1,   789,
     119,    -1,   860,    52,   845,    54,    55,    56,    57,    -1,
     868,   855,   856,   132,    -1,    89,   193,    -1,    -1,   196,
     197,   198,    96,    -1,    -1,    -1,    -1,    -1,   890,   891,
     892,    -1,   894,   895,    -1,    -1,    -1,   214,    -1,   216,
     217,    -1,   116,    -1,   902,    -1,   904,    -1,    -1,    -1,
      -1,    -1,   910,    -1,   912,   811,    -1,   813,   920,   921,
     922,   923,   937,    -1,    -1,    -1,    52,   911,    54,    55,
      56,    57,   925,    -1,    -1,   937,    -1,   939,   940,   937,
      -1,    -1,   735,   936,    -1,   738,    -1,    -1,    -1,    -1,
      -1,   633,   954,   955,   956,   957,   960,   941,    -1,   943,
      -1,    -1,    -1,    89,    -1,    -1,   968,    -1,     2,    95,
       4,    -1,    -1,    -1,   976,   871,   872,    -1,    -1,    -1,
      -1,   298,    -1,    -1,   938,    -1,   303,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,    -1,   331,    49,    -1,    68,    -1,    53,
     916,    52,   918,    54,    55,    56,    57,    -1,    -1,    -1,
      -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    75,    -1,   836,    -1,   838,    -1,   729,   730,    -1,
     946,    -1,    -1,    87,    88,    89,    90,    -1,    89,    -1,
      -1,   378,   379,    -1,    95,    96,   117,   118,   119,   120,
     121,   388,   389,   390,    -1,   757,    -1,   394,    -1,   396,
     397,   398,    -1,    -1,   877,   116,   879,    -1,   119,    52,
     883,    54,    55,    56,    57,    -1,    -1,   779,   415,    -1,
      -1,   132,    -1,   420,   786,   787,    -1,    -1,   790,    -1,
      -1,    -1,   429,    -1,    52,   432,    54,    55,    56,    57,
      -1,   803,   804,    -1,    -1,    -1,    89,   444,     2,    -1,
       4,    -1,    95,    96,   927,    -1,   929,    -1,    -1,   932,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   464,   465,    -1,
      -1,    89,    -1,   116,    -1,   948,   119,    95,    96,   476,
      52,   195,    54,    55,    56,    57,    52,   849,    54,    55,
      56,    57,   135,    -1,    -1,    49,    -1,   970,   116,    53,
      52,   119,    54,    55,    56,    57,   868,    -1,    -1,    -1,
      -1,    -1,    -1,   227,    -1,    -1,     2,    89,     4,    -1,
      -1,    75,    -1,    89,   238,   239,    -1,    -1,    -1,    95,
      96,    -1,    -1,    87,    88,    89,    -1,    89,    -1,    -1,
     902,   538,   904,    95,   258,   542,    -1,    -1,   910,    -1,
     116,    -1,    -1,   119,    -1,    -1,    68,   554,    -1,    -1,
      -1,    -1,    68,    49,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    84,    -1,   288,    -1,    -1,    83,    84,   293,
     577,    -1,    -1,    -1,    -1,    -1,   300,   301,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     2,    -1,     4,    -1,
      -1,    87,   114,   115,   116,   117,   118,   119,   120,   121,
     116,   117,   118,   119,   120,   121,    -1,    -1,    -1,   333,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    69,    70,    71,    72,    73,    74,    75,    -1,    77,
      78,   195,    -1,    49,    -1,    83,    84,    -1,    -1,   646,
      -1,    -1,    -1,   650,    -1,   652,   653,    -1,   628,    -1,
     630,   658,   659,    -1,    -1,    -1,    -1,    -1,    -1,   666,
      -1,    -1,    -1,   227,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   238,   239,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   416,    -1,   258,    -1,    -1,    -1,   705,   195,
      -1,   425,    -1,   710,   711,    -1,   713,   714,    -1,    -1,
      -1,    -1,    -1,   720,    -1,    -1,    -1,    -1,   442,    -1,
      -1,    -1,    -1,    -1,   288,    -1,    -1,    -1,    -1,   293,
      -1,   227,    -1,    -1,    -1,    -1,   300,   301,    -1,    -1,
      -1,    -1,   238,   239,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   758,    -1,    -1,    -1,   762,    -1,    -1,   482,    -1,
      -1,    -1,   769,    -1,    -1,    -1,    -1,    -1,    -1,   333,
      -1,   751,    -1,   753,   754,    -1,    -1,    -1,    -1,   195,
      -1,    -1,   789,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   288,    -1,    -1,    -1,    -1,   293,    -1,    -1,
     807,    -1,    -1,    -1,   300,    -1,    -1,    -1,   532,    -1,
     534,   227,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   238,   239,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   556,    -1,    -1,    -1,    -1,   333,    -1,   819,
     820,    -1,   822,    -1,   824,   825,    -1,    -1,    -1,    -1,
      -1,    -1,   416,    -1,     0,    -1,    -1,    -1,    -1,    -1,
      -1,   425,     8,     9,    10,    -1,    -1,    13,    14,    15,
      -1,    17,   288,    -1,    -1,    -1,    -1,   293,   442,   603,
      -1,    27,    -1,    -1,   300,    -1,    -1,    -1,    -1,    -1,
      -1,    37,    38,    -1,    40,    41,    42,    43,    44,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   633,
     890,   891,   892,    -1,   894,   895,    -1,   333,   482,    -1,
     416,    -1,    -1,    -1,    -1,   649,    -1,    -1,    -1,   425,
     937,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
     920,   921,   922,   923,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   688,    -1,    -1,    -1,   532,    -1,
     534,    -1,   696,    -1,   954,   955,   956,   957,    -1,    -1,
      -1,   127,    -1,   129,    -1,    -1,   132,   133,   968,   135,
      -1,   137,   556,    -1,    -1,    -1,   976,    -1,    -1,    -1,
     416,    -1,    -1,    -1,    -1,   729,   730,    -1,    -1,   425,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   757,    -1,    -1,   532,    -1,   534,   603,
      -1,    -1,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,   779,    -1,    83,    84,    -1,
     556,    -1,   786,   787,    -1,    -1,   790,    -1,    -1,   633,
     634,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   803,
     804,    -1,    -1,    -1,   110,   649,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   827,   130,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   532,    -1,   534,    -1,
      -1,   845,    -1,    -1,   688,   849,    -1,    -1,    -1,    -1,
      -1,    -1,   696,    -1,    -1,    -1,   860,   633,    -1,    -1,
     556,    -1,    52,    53,   868,    -1,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,    69,
      70,    71,    72,    73,    74,   729,   730,    77,    78,    -1,
      -1,    81,    82,    83,    84,    -1,    -1,    -1,   902,    -1,
     904,    -1,    -1,    -1,    -1,    95,   910,    -1,   912,    -1,
      -1,    -1,   688,   757,    -1,    -1,    -1,    -1,    -1,    -1,
     696,    -1,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,    -1,   123,   124,   779,    -1,   633,    -1,    -1,
      -1,   131,   786,   787,    -1,    -1,   790,    -1,    -1,    -1,
      -1,    -1,    -1,   729,   730,    -1,    -1,    -1,    -1,   803,
     804,    -1,    -1,    -1,    -1,    46,    47,    -1,    -1,    -1,
      51,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   757,    63,   827,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   688,    -1,    75,    -1,    -1,    -1,    -1,    -1,
     696,   845,    -1,   779,    -1,   849,    87,    88,    89,    -1,
     786,   787,    -1,    -1,   790,    -1,   860,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   868,    -1,    -1,   803,   804,    -1,
      -1,    -1,    -1,   729,   730,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   902,    -1,
     904,   757,    -1,    -1,    -1,    -1,   910,    -1,   912,   845,
      -1,    -1,    -1,   849,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   779,   860,    -1,    -1,    -1,    -1,    -1,
     786,   787,   868,    -1,   790,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   803,   804,    -1,
      -1,    -1,   193,    -1,    -1,   196,   197,   198,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   902,    -1,   904,    -1,
      -1,    -1,    -1,    -1,   910,    -1,   912,    -1,    -1,    -1,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,   849,    -1,    83,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   860,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   868,    -1,    -1,    -1,    -1,   258,    -1,    -1,
      -1,    -1,   110,    -1,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   902,    -1,   904,   137,
      -1,    -1,    -1,    -1,   910,    -1,   912,    -1,    -1,    -1,
     301,    -1,    -1,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    53,
      -1,    -1,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    -1,    -1,    77,    78,    -1,    -1,    81,    82,    83,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    -1,    -1,    -1,    -1,    -1,   388,   389,   390,
      -1,    -1,    -1,    -1,    -1,   396,   397,   398,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    -1,   123,
     124,    -1,    -1,    -1,   415,    -1,    -1,   131,   132,   420,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   429,    -1,
      -1,   432,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   442,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   464,   465,    -1,     0,     1,    -1,     3,
       4,     5,     6,     7,    -1,   476,    -1,    11,    12,    -1,
      -1,   482,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      -1,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
      64,    -1,    66,    67,    -1,    -1,    -1,   538,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    -1,    97,    98,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,    -1,    -1,   577,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,   123,
     124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   135,   603,   137,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     0,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     8,     9,    10,    -1,    -1,    13,    14,    15,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    28,    29,    -1,    -1,   646,    -1,    -1,   649,   650,
      37,    38,    -1,    40,    41,    42,    43,    44,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   666,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    -1,    -1,    83,    84,    85,    -1,
      87,    88,    -1,    -1,   705,    -1,    -1,    94,    -1,   710,
     711,    -1,   713,   714,    -1,    -1,    -1,    -1,    -1,   720,
     107,    -1,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,    -1,
     127,   128,   129,   130,    -1,    -1,   133,   134,   135,    -1,
     137,    -1,    -1,    -1,    -1,    -1,    -1,   758,    -1,    -1,
      -1,   762,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     8,     9,    10,   789,    -1,
      13,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    26,    27,    28,   807,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    37,    38,    -1,    40,    41,    42,
      43,    44,    -1,    -1,    -1,    -1,   827,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   845,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    85,    -1,    -1,    88,    -1,    -1,    -1,    -1,
      -1,    94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,    -1,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,    -1,
      -1,    -1,    -1,   126,   127,   128,   129,   130,     0,   132,
     133,   134,   135,    -1,   137,    -1,     8,     9,    10,    -1,
      -1,    13,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    28,    29,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    37,    38,    -1,    40,    41,
      42,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    -1,    -1,    83,    84,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    -1,
      -1,    83,    84,    85,    -1,    -1,    88,    -1,    -1,    -1,
      -1,   110,    94,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,    -1,    -1,   107,    -1,    -1,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
      -1,    -1,    -1,    -1,    -1,   127,   128,   129,   130,     0,
      -1,   133,   134,   135,    -1,   137,    -1,     8,     9,    10,
      -1,    -1,    13,    14,    15,    -1,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    26,    27,    28,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    37,    38,    -1,    40,
      41,    42,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      -1,    -1,    83,    84,    85,    -1,    -1,    88,    -1,    -1,
      -1,    -1,    -1,    94,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,    -1,    -1,    -1,    -1,    -1,   110,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,    -1,    -1,    -1,   126,   127,   128,   129,   130,
       0,   132,   133,   134,   135,    -1,   137,    -1,     8,     9,
      10,    -1,    -1,    13,    14,    15,    -1,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    28,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,    38,    -1,
      40,    41,    42,    43,    44,    -1,    -1,    -1,    -1,    -1,
      -1,    68,    69,    70,    71,    72,    73,    74,    -1,    -1,
      77,    78,    -1,    -1,    -1,    -1,    83,    84,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    -1,    -1,    83,    84,    85,    -1,    -1,    88,    -1,
      -1,    -1,    -1,    -1,    94,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,    -1,
     110,    -1,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,    -1,    -1,    -1,    -1,    -1,   127,   128,   129,
     130,     0,   132,   133,   134,   135,    -1,   137,    -1,     8,
       9,    10,    -1,    -1,    -1,    14,    15,    -1,    17,    -1,
      -1,    -1,    -1,    44,    -1,    -1,    -1,    26,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,    38,
      -1,    40,    41,    42,    43,    44,    -1,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    -1,    -1,    83,    84,    85,    -1,    87,   110,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     109,   110,    -1,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,    -1,    -1,    -1,    -1,   126,   127,   128,
     129,   130,     0,    -1,   133,    -1,   135,    -1,   137,    -1,
       8,     9,    10,    -1,    -1,    -1,    14,    15,    -1,    17,
      -1,    -1,    -1,    -1,    44,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,
      38,    -1,    40,    41,    42,    43,    44,    -1,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    85,    -1,    87,
     110,    -1,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   109,   110,    -1,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,    -1,    -1,    -1,    -1,    -1,   127,
     128,   129,   130,    -1,    -1,   133,    -1,   135,     1,   137,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      -1,    -1,    15,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    45,    46,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,    -1,    62,
      63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    -1,    97,    98,    99,   100,   101,   102,
     103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,
     123,   124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   135,     1,   137,     3,     4,     5,     6,     7,
      -1,    -1,    10,    11,    12,    -1,    14,    15,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    -1,
      -1,    39,    -1,    -1,    -1,    -1,    -1,    45,    46,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    -1,    62,    63,    64,    -1,    66,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,    97,
      98,    99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   122,   123,   124,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   135,     1,   137,
       3,     4,     5,     6,     7,    -1,    -1,    10,    11,    12,
      -1,    -1,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,
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
      -1,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
      64,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    -1,    97,    98,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,   123,
     124,    -1,    -1,   127,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   135,     1,   137,     3,     4,     5,     6,     7,    -1,
      -1,    10,    11,    12,    -1,    -1,    15,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    45,    46,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,    63,    64,    -1,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    -1,    97,    98,
      99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,
      -1,     1,    -1,     3,     4,     5,     6,     7,    -1,     9,
      10,    11,    12,   122,   123,   124,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,   135,    -1,   137,    -1,
      30,    31,    32,    33,    34,    35,    36,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    45,    46,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    59,
      60,    -1,    62,    63,    64,    -1,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    -1,    97,    98,    99,
     100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,
       1,    -1,     3,     4,     5,     6,     7,    -1,    -1,    -1,
      11,    12,   122,   123,   124,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,   135,    -1,   137,    -1,    30,
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
      -1,    -1,   133,    -1,   135,     1,   137,     3,     4,     5,
       6,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,
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
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   133,    -1,   135,
       1,   137,     3,     4,     5,     6,     7,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,
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
      -1,   122,   123,   124,    -1,    -1,   127,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   135,     1,   137,     3,     4,     5,
       6,     7,    -1,    -1,    10,    11,    12,    -1,    -1,    -1,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,
      46,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    64,    -1,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      -1,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   122,   123,   124,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,   135,
      -1,   137,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    -1,    62,    63,    64,    -1,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    96,
      97,    98,    99,   100,   101,   102,   103,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   122,   123,   124,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
     137,    -1,    30,    31,    32,    33,    34,    35,    36,    -1,
      -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,
      -1,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    -1,    62,    63,    64,    -1,    66,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    -1,    92,    93,    -1,    -1,    -1,    97,
      98,    99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   122,   123,   124,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,   135,    -1,   137,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,    63,    64,    -1,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    -1,    -1,    97,    98,
      99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,     7,    -1,    -1,
      -1,    11,    12,   122,   123,   124,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,   137,    -1,
      30,    31,    32,    33,    34,    35,    36,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    59,
      60,    -1,    62,    63,    64,    -1,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    -1,    97,    98,    99,
     100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,    -1,    -1,    -1,
      11,    12,   122,   123,   124,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,   135,    -1,    -1,    -1,    30,
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
      -1,    -1,    -1,    -1,   135,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    -1,    -1,    -1,    -1,    -1,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    -1,    -1,    77,
      78,    -1,    -1,    81,    82,    83,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,    -1,   123,   124,     3,     4,     5,
      -1,     7,    -1,   131,   132,    11,    12,    -1,    -1,    -1,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      26,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    -1,    62,    63,    64,    -1,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      96,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   122,   123,   124,    -1,
     126,     3,     4,     5,    -1,     7,   132,    -1,    -1,    11,
      12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    26,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
      62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    96,    97,    98,    99,   100,   101,
     102,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     122,   123,   124,    -1,   126,     3,     4,     5,    -1,     7,
     132,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    -1,
      -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,
      -1,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    62,    63,    64,    -1,    66,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,
      -1,    89,    90,    -1,    92,    93,    -1,    95,    96,    97,
      98,    99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,
      -1,   109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   122,   123,   124,     3,     4,     5,
      -1,     7,    -1,    -1,   132,    11,    12,    -1,    -1,    -1,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    -1,    62,    63,    64,    -1,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      96,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   122,   123,   124,     3,
       4,     5,    -1,     7,    -1,    -1,   132,    11,    12,    -1,
      -1,    -1,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    -1,    62,    63,
      64,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    96,    97,    98,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,   123,
     124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   132,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    -1,    -1,    -1,    -1,
      -1,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    -1,    -1,    77,    78,    -1,    -1,    81,    82,    83,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    -1,   123,
     124,    -1,    -1,    -1,    -1,    -1,    -1,   131,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    -1,    -1,    -1,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      -1,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      -1,    -1,    77,    78,    -1,    -1,    81,    82,    83,    84,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      95,    -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,    -1,   123,   124,
      -1,    -1,    -1,    -1,    -1,    -1,   131,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    -1,    -1,    -1,    -1,    -1,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    -1,    -1,
      56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    -1,
      -1,    77,    78,    -1,    -1,    81,    82,    83,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,   123,   124,    -1,
      -1,    -1,    -1,    -1,    -1,   131,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    -1,    -1,    -1,    -1,    -1,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    -1,    -1,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    -1,    -1,
      77,    78,    -1,    -1,    81,    82,    83,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,   123,   124,     3,     4,
       5,    -1,     7,    -1,   131,    -1,    11,    12,    -1,    -1,
      -1,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    -1,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,    -1,    62,    63,    64,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      -1,    -1,    97,    98,    99,   100,   101,   102,   103,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,    -1,    -1,    -1,    16,   122,    18,    19,
      20,    21,    22,    23,    24,   130,    -1,    -1,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    59,
      60,    -1,    62,    63,    64,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    -1,    -1,    97,    98,    99,
     100,   101,   102,   103,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,
      -1,    16,   122,    18,    19,    20,    21,    22,    23,    24,
     130,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,    -1,    62,    63,    64,
      -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      95,    -1,    97,    98,    99,   100,   101,   102,   103,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,    -1,    -1,    -1,    11,    12,   122,   123,   124,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,
      46,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
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
      57,    58,    59,    60,    -1,    62,    63,    64,    -1,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    96,
      97,    98,    99,   100,   101,   102,   103,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   122,   123,   124,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    -1,
      -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,
      -1,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    62,    63,    64,    -1,    66,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    -1,    92,    93,    -1,    95,    96,    97,
      98,    99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   122,   123,   124,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
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
      90,    -1,    92,    93,    -1,    -1,    96,    97,    98,    99,
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
      -1,    92,    93,    -1,    95,    -1,    97,    98,    99,   100,
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
      93,    -1,    95,    -1,    97,    98,    99,   100,   101,   102,
     103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   122,
     123,   124,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
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
      55,    56,    57,    -1,    59,    60,    -1,    62,    63,    64,
      -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      95,    -1,    97,    98,    99,   100,   101,   102,   103,    -1,
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
      -1,    89,    90,    -1,    92,    93,    -1,    -1,    -1,    97,
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
      89,    90,    -1,    92,    93,    -1,    -1,    -1,    97,    98,
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
      90,    -1,    92,    93,    -1,    -1,    -1,    97,    98,    99,
     100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   122,   123,   124,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      -1,    62,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    95,    -1,    97,    98,    99,   100,
     101,   102,   103,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,
      16,   122,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    64,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      -1,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,   122,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      -1,    62,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    83,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    -1,    -1,    97,    98,    99,   100,
     101,   102,   103,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,
      16,   122,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    64,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    -1,
      -1,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,   122,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      -1,    62,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    -1,    -1,    97,    98,    99,   100,
     101,   102,   103,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,
      16,   122,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    64,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    -1,
      -1,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,   122,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      -1,    62,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    -1,    -1,    97,    98,    99,   100,
     101,   102,   103,    -1,    -1,    -1,    -1,    -1,    52,    53,
      -1,    -1,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   122,    66,    67,    68,    69,    70,    71,    72,    73,
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
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    -1,   123,
     124,    52,    53,    -1,    -1,    56,    -1,   131,   132,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,
      81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,   124,    -1,    -1,    -1,    -1,    -1,    -1,
     131,   132,    68,    69,    70,    71,    72,    73,    74,    -1,
      -1,    77,    78,    -1,    -1,    -1,    -1,    83,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121
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
     153,   157,   158,   160,   161,   162,   164,   165,   175,   189,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   240,   241,   253,   254,   255,   256,   257,   258,   259,
     262,   271,   273,   274,   275,   276,   277,   278,   301,   312,
     143,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    56,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    77,    78,    81,    82,    83,    84,
      95,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   123,   124,   131,   168,   169,   170,   171,   173,   174,
     271,   273,    39,    58,    86,    89,    95,    96,   123,   157,
     165,   175,   177,   182,   185,   187,   206,   275,   277,   278,
     299,   300,   182,   182,   132,   183,   184,   132,   179,   183,
     132,   137,   306,    54,   170,   306,   144,   126,    21,    22,
      30,    31,    32,   175,   206,   301,   175,    56,     1,    89,
     146,   147,   148,   159,   160,   312,   150,   178,   187,   299,
     312,   177,   298,   299,   312,    46,    86,   122,   130,   164,
     189,   206,   275,   278,   232,   233,    54,    55,    57,   168,
     265,   272,   264,   265,   266,   136,   260,   136,   263,    59,
      60,   152,   175,   175,   135,   137,   305,   310,   311,    40,
      41,    42,    43,    44,    37,    38,    28,   237,   109,   130,
      89,    95,   161,   109,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    83,    84,   110,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
      85,   128,   129,   190,   150,   151,   151,   193,   195,   151,
     305,   311,    86,   158,   165,   206,   222,   275,   278,    52,
      56,    83,    86,   166,   167,   206,   275,   278,   167,    33,
      34,    35,    36,    49,    50,    51,    52,    56,   132,   168,
     276,   296,    85,   129,    26,   126,   242,   256,    87,    87,
     179,   183,   242,   130,   177,    56,   177,   177,   109,    88,
     130,   186,   312,    85,   128,   129,    87,    87,   130,   186,
     182,   306,   307,   182,   181,   182,   312,   150,   307,   150,
      54,    63,    64,   149,   132,   176,   126,   146,    85,   129,
      87,   148,   159,   133,   305,   311,   307,   191,   134,   130,
     137,   309,   130,   309,   127,   309,   306,    56,    59,    60,
     161,   163,   130,    85,   128,   129,   234,    61,   104,   105,
     106,   267,   106,   267,   106,    65,   267,   106,   106,   261,
     267,   106,    61,   106,    68,    68,   135,   143,   151,   151,
     151,   151,   148,   150,   150,   238,    95,   152,   177,   187,
     188,   159,   130,   164,   130,   152,   175,   177,   188,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,    52,    53,    56,   173,   179,
     302,   303,   181,    52,    53,    56,   173,   179,   302,   145,
     146,    13,   218,   310,   218,   151,   151,   305,    17,   247,
      56,    85,   128,   129,    25,   150,    52,    56,   166,     1,
     113,   279,   310,    85,   128,   129,   202,   297,   203,    85,
     129,   304,    52,    56,   302,   302,   245,   243,   152,   175,
     152,   175,    94,   154,   172,   175,   177,    95,   177,   185,
     299,    52,    56,   181,    52,    56,   300,   307,   133,   307,
     307,   170,   192,   175,   141,   127,   302,   302,   175,   307,
     148,   307,   299,   130,   163,    52,    56,   181,    52,    56,
      52,    54,    55,    56,    57,    89,    95,    96,   116,   119,
     132,   235,   282,   283,   284,   285,   286,   287,   290,   291,
     292,   293,   294,   269,   268,   136,   267,   136,   175,   175,
      76,   114,   227,   228,   312,   177,   130,   307,   163,   130,
      44,   306,    87,    87,   179,   183,   306,   308,    87,    87,
     179,   180,   183,   312,    10,   217,     8,   249,   312,   146,
      13,   146,    27,   219,   310,   219,   247,   187,   217,    52,
      56,   181,    52,    56,   197,   200,   310,   280,   199,    52,
      56,   166,   181,   145,   150,   132,   281,   282,   204,   180,
     183,   180,   183,   227,   227,    44,   155,   170,   177,   186,
      87,    87,   308,    87,    87,   150,   127,   309,   161,   308,
     109,    52,    89,    95,   223,   224,   225,   284,   282,    29,
     107,   236,   130,   295,   312,   130,   295,    52,   130,   295,
      52,   146,    54,    55,    57,   270,   278,    52,   135,   226,
     229,   286,   288,   289,   292,   294,   312,   239,    95,   177,
     163,   175,   111,   152,   175,   152,   175,   154,   134,    87,
     152,   175,   152,   175,   154,   177,   188,   250,   312,    15,
     221,   312,    14,   220,   221,   221,   194,   196,   217,   130,
     218,   308,   151,   310,   151,   145,   308,   217,   307,   282,
     145,   310,   168,   246,   244,   175,   227,    87,   307,   177,
     225,   130,   284,   130,   307,   229,   146,   146,   285,   290,
     292,   294,   286,   287,   292,   286,   127,   109,    52,   230,
     231,   283,   229,   114,   130,   295,   130,   295,   130,   295,
     146,   177,   175,   152,   175,    88,   251,   312,   146,     9,
     252,   312,   151,   217,   217,   146,   146,   177,   146,   219,
     201,   310,   217,   307,   217,   205,   146,   146,   156,   307,
     224,   130,    95,   223,   307,    10,   127,   130,   295,   130,
     295,   130,   295,   130,   295,   295,    86,   206,   130,   114,
     289,   292,   286,   288,   292,   286,    10,    86,   165,   206,
     275,   278,   218,   146,   218,   217,   217,   221,   247,   248,
     198,   145,   281,    10,   127,   146,   130,   224,   130,   284,
     286,   292,   286,   286,    56,    85,   231,   130,   295,   130,
     295,   295,   130,   295,   295,    56,    85,   128,   129,   146,
     146,   146,   217,   145,   127,   224,   130,   295,   130,   295,
     295,   295,    52,    56,   286,   292,   286,   286,    52,    56,
     181,    52,    56,   249,   220,   217,   217,   224,   286,   295,
     130,   295,   295,   295,   308,   295,   286,   295
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (parser_state, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, parser_state); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, rb_parser_state* parser_state)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, parser_state)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    rb_parser_state* parser_state;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (parser_state);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, rb_parser_state* parser_state)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, parser_state)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    rb_parser_state* parser_state;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, parser_state);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, rb_parser_state* parser_state)
#else
static void
yy_reduce_print (yyvsp, yyrule, parser_state)
    YYSTYPE *yyvsp;
    int yyrule;
    rb_parser_state* parser_state;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , parser_state);
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, parser_state); \
} while (YYID (0))

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
#ifndef	YYINITDEPTH
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, rb_parser_state* parser_state)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, parser_state)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    rb_parser_state* parser_state;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (parser_state);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (rb_parser_state* parser_state);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (rb_parser_state* parser_state)
#else
int
yyparse (parser_state)
    rb_parser_state* parser_state;
#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

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
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
     `$$ = $1'.

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
#line 496 "grammar19.y"
    {
                    lex_state = EXPR_BEG;
                    variables = new LocalState(0);
                    class_nest = 0;
                  ;}
    break;

  case 3:
#line 502 "grammar19.y"
    {
                    if((yyvsp[(2) - (2)].node) && !compile_for_eval) {
                      /* last expression should not be void */
                      if(nd_type((yyvsp[(2) - (2)].node)) != NODE_BLOCK) {
                        void_expr((yyvsp[(2) - (2)].node));
                      } else {
                        NODE *node = (yyvsp[(2) - (2)].node);
                        while(node->nd_next) {
                          node = node->nd_next;
                        }
                        void_expr(node->nd_head);
                      }
                    }
                    top_node = block_append(top_node, (yyvsp[(2) - (2)].node));
                    class_nest = 0;
                  ;}
    break;

  case 4:
#line 521 "grammar19.y"
    {
                    void_stmts((yyvsp[(1) - (2)].node));
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                  ;}
    break;

  case 5:
#line 528 "grammar19.y"
    {
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 6:
#line 532 "grammar19.y"
    {
                    (yyval.node) = newline_node((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 7:
#line 536 "grammar19.y"
    {
                    (yyval.node) = block_append((yyvsp[(1) - (3)].node), newline_node((yyvsp[(3) - (3)].node)));
                  ;}
    break;

  case 8:
#line 540 "grammar19.y"
    {
                    (yyval.node) = remove_begin((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 10:
#line 547 "grammar19.y"
    {
                    if(in_def || in_single) {
                      yy_error("BEGIN in method");
                    }
                  ;}
    break;

  case 11:
#line 553 "grammar19.y"
    {
                    /* TODO
                    block_append( , $4);
                    */
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 12:
#line 565 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (4)].node);
                    if((yyvsp[(2) - (4)].node)) {
                      (yyval.node) = NEW_RESCUE((yyvsp[(1) - (4)].node), (yyvsp[(2) - (4)].node), (yyvsp[(3) - (4)].node));
                    } else if((yyvsp[(3) - (4)].node)) {
                      rb_warn("else without rescue is useless");
                      (yyval.node) = block_append((yyval.node), (yyvsp[(3) - (4)].node));
                    }
                    if((yyvsp[(4) - (4)].node)) {
                      if((yyval.node)) {
                        (yyval.node) = NEW_ENSURE((yyval.node), (yyvsp[(4) - (4)].node));
                      } else {
                        (yyval.node) = block_append((yyvsp[(4) - (4)].node), NEW_NIL());
                      }
                    }
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 13:
#line 585 "grammar19.y"
    {
                    void_stmts((yyvsp[(1) - (2)].node));
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                  ;}
    break;

  case 14:
#line 592 "grammar19.y"
    {
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 15:
#line 596 "grammar19.y"
    {
                    (yyval.node) = newline_node((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 16:
#line 600 "grammar19.y"
    {
                    (yyval.node) = block_append((yyvsp[(1) - (3)].node), newline_node((yyvsp[(3) - (3)].node)));
                  ;}
    break;

  case 17:
#line 604 "grammar19.y"
    {
                    (yyval.node) = remove_begin((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 18:
#line 609 "grammar19.y"
    {lex_state = EXPR_FNAME;;}
    break;

  case 19:
#line 610 "grammar19.y"
    {
                    (yyval.node) = NEW_ALIAS((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 20:
#line 614 "grammar19.y"
    {
                    (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 21:
#line 618 "grammar19.y"
    {
                    char buf[3];

                    snprintf(buf, sizeof(buf), "$%c", (char)(yyvsp[(3) - (3)].node)->nd_nth);
                    (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), rb_parser_sym(buf));
                  ;}
    break;

  case 22:
#line 625 "grammar19.y"
    {
                    yy_error("can't make alias for the number variables");
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 23:
#line 630 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 24:
#line 634 "grammar19.y"
    {
                    (yyval.node) = NEW_IF(cond((yyvsp[(3) - (3)].node)), remove_begin((yyvsp[(1) - (3)].node)), 0);
                    fixpos((yyval.node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 25:
#line 639 "grammar19.y"
    {
                    (yyval.node) = NEW_UNLESS(cond((yyvsp[(3) - (3)].node)), remove_begin((yyvsp[(1) - (3)].node)), 0);
                    fixpos((yyval.node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 26:
#line 644 "grammar19.y"
    {
                    if((yyvsp[(1) - (3)].node) && nd_type((yyvsp[(1) - (3)].node)) == NODE_BEGIN) {
                      (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (3)].node)), (yyvsp[(1) - (3)].node)->nd_body, 0);
                    } else {
                      (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (3)].node)), (yyvsp[(1) - (3)].node), 1);
                    }
                  ;}
    break;

  case 27:
#line 652 "grammar19.y"
    {
                    if((yyvsp[(1) - (3)].node) && nd_type((yyvsp[(1) - (3)].node)) == NODE_BEGIN) {
                      (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (3)].node)), (yyvsp[(1) - (3)].node)->nd_body, 0);
                    } else {
                      (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (3)].node)), (yyvsp[(1) - (3)].node), 1);
                    }
                  ;}
    break;

  case 28:
#line 660 "grammar19.y"
    {
                    NODE *resq = NEW_RESBODY(0, remove_begin((yyvsp[(3) - (3)].node)), 0);
                    (yyval.node) = NEW_RESCUE(remove_begin((yyvsp[(1) - (3)].node)), resq, 0);
                  ;}
    break;

  case 29:
#line 665 "grammar19.y"
    {
                    if(in_def || in_single) {
                      rb_warn("END in method; use at_exit");
                    }

                    (yyval.node) = NEW_POSTEXE(NEW_NODE(NODE_SCOPE,
                            0 /* tbl */, (yyvsp[(3) - (4)].node) /* body */, 0 /* args */));
                  ;}
    break;

  case 30:
#line 674 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 31:
#line 679 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 32:
#line 685 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    if((yyvsp[(1) - (3)].node)) {
                      QUID vid = (yyvsp[(1) - (3)].node)->nd_vid;
                      if((yyvsp[(2) - (3)].id) == tOROP) {
                        (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                        (yyval.node) = NEW_OP_ASGN_OR(gettable(vid), (yyvsp[(1) - (3)].node));
                        if(is_asgn_or_id(vid)) {
                          (yyval.node)->nd_aid = vid;
                        }
                      } else if((yyvsp[(2) - (3)].id) == tANDOP) {
                        (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                        (yyval.node) = NEW_OP_ASGN_AND(gettable(vid), (yyvsp[(1) - (3)].node));
                      } else {
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                        (yyval.node)->nd_value = call_bin_op(gettable(vid), (yyvsp[(2) - (3)].id), (yyvsp[(3) - (3)].node));
                      }
                    } else {
                      (yyval.node) = NEW_BEGIN(0);
                    }
                  ;}
    break;

  case 33:
#line 707 "grammar19.y"
    {
                    NODE *args;

                    value_expr((yyvsp[(6) - (6)].node));
                    if(!(yyvsp[(3) - (6)].node)) (yyvsp[(3) - (6)].node) = NEW_ZARRAY();
                    args = arg_concat((yyvsp[(3) - (6)].node), (yyvsp[(6) - (6)].node));
                    if((yyvsp[(5) - (6)].id) == tOROP) {
                      (yyvsp[(5) - (6)].id) = 0;
                    } else if((yyvsp[(5) - (6)].id) == tANDOP) {
                      (yyvsp[(5) - (6)].id) = 1;
                    }
                    (yyval.node) = NEW_OP_ASGN1((yyvsp[(1) - (6)].node), (yyvsp[(5) - (6)].id), args);
                    fixpos((yyval.node), (yyvsp[(1) - (6)].node));
                  ;}
    break;

  case 34:
#line 722 "grammar19.y"
    {
                    value_expr((yyvsp[(5) - (5)].node));
                    if((yyvsp[(4) - (5)].id) == tOROP) {
                      (yyvsp[(4) - (5)].id) = 0;
                    } else if((yyvsp[(4) - (5)].id) == tANDOP) {
                      (yyvsp[(4) - (5)].id) = 1;
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 35:
#line 733 "grammar19.y"
    {
                    value_expr((yyvsp[(5) - (5)].node));
                    if((yyvsp[(4) - (5)].id) == tOROP) {
                      (yyvsp[(4) - (5)].id) = 0;
                    } else if((yyvsp[(4) - (5)].id) == tANDOP) {
                      (yyvsp[(4) - (5)].id) = 1;
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 36:
#line 744 "grammar19.y"
    {
                    yy_error("constant re-assignment");
                    (yyval.node) = 0;
                  ;}
    break;

  case 37:
#line 749 "grammar19.y"
    {
                    value_expr((yyvsp[(5) - (5)].node));
                    if((yyvsp[(4) - (5)].id) == tOROP) {
                      (yyvsp[(4) - (5)].id) = 0;
                    } else if((yyvsp[(4) - (5)].id) == tANDOP) {
                      (yyvsp[(4) - (5)].id) = 1;
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 38:
#line 760 "grammar19.y"
    {
                    rb_backref_error((yyvsp[(1) - (3)].node));
                    (yyval.node) = 0;
                  ;}
    break;

  case 39:
#line 765 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 40:
#line 770 "grammar19.y"
    {
                    (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 41:
#line 775 "grammar19.y"
    {
                    (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 44:
#line 784 "grammar19.y"
    {
                    (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 45:
#line 788 "grammar19.y"
    {
                    (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 46:
#line 792 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[(3) - (3)].node)), '!');
                  ;}
    break;

  case 47:
#line 796 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[(2) - (2)].node)), '!');
                  ;}
    break;

  case 49:
#line 803 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  ;}
    break;

  case 53:
#line 816 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 54:
#line 820 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 55:
#line 826 "grammar19.y"
    {
                    /* TODO */
                    (yyvsp[(1) - (1)].num) = ruby_sourceline;
                    reset_block();
                  ;}
    break;

  case 56:
#line 831 "grammar19.y"
    { (yyval.vars) = variables->block_vars; ;}
    break;

  case 57:
#line 834 "grammar19.y"
    {
                    /* TODO $$ = NEW_ITER($3, 0, extract_block_vars($5, * $<vars>4)); */
                    (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), extract_block_vars((yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                    nd_set_line((yyval.node), (yyvsp[(2) - (6)].num));
                  ;}
    break;

  case 58:
#line 842 "grammar19.y"
    {
                    (yyval.node) = NEW_FCALL((yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                 ;}
    break;

  case 59:
#line 847 "grammar19.y"
    {
                    block_dup_check((yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].node));
                    (yyvsp[(3) - (3)].node)->nd_iter = NEW_FCALL((yyvsp[(1) - (3)].id), (yyvsp[(2) - (3)].node));
                    (yyval.node) = (yyvsp[(3) - (3)].node);
                    fixpos((yyval.node), (yyvsp[(2) - (3)].node));
                 ;}
    break;

  case 60:
#line 854 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 61:
#line 859 "grammar19.y"
    {
                    block_dup_check((yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                    (yyvsp[(5) - (5)].node)->nd_iter = NEW_CALL((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].node));
                    (yyval.node) = (yyvsp[(5) - (5)].node);
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                 ;}
    break;

  case 62:
#line 866 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 63:
#line 871 "grammar19.y"
    {
                    block_dup_check((yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                    (yyvsp[(5) - (5)].node)->nd_iter = NEW_CALL((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].node));
                    (yyval.node) = (yyvsp[(5) - (5)].node);
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 64:
#line 878 "grammar19.y"
    {
                    (yyval.node) = NEW_SUPER((yyvsp[(2) - (2)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 65:
#line 883 "grammar19.y"
    {
                    (yyval.node) = new_yield((yyvsp[(2) - (2)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 66:
#line 888 "grammar19.y"
    {
                    (yyval.node) = NEW_RETURN(ret_args((yyvsp[(2) - (2)].node)));
                  ;}
    break;

  case 67:
#line 892 "grammar19.y"
    {
                    (yyval.node) = NEW_BREAK(ret_args((yyvsp[(2) - (2)].node)));
                  ;}
    break;

  case 68:
#line 896 "grammar19.y"
    {
                    (yyval.node) = NEW_NEXT(ret_args((yyvsp[(2) - (2)].node)));
                  ;}
    break;

  case 70:
#line 903 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 72:
#line 910 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(NEW_LIST((yyvsp[(2) - (3)].node)), 0);
                  ;}
    break;

  case 73:
#line 916 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (1)].node), 0);
                  ;}
    break;

  case 74:
#line 920 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(list_append((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node)), 0);
                  ;}
    break;

  case 75:
#line 924 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 76:
#line 928 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (5)].node), NEW_POSTARG((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)));
                  ;}
    break;

  case 77:
#line 932 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (2)].node), -1);
                  ;}
    break;

  case 78:
#line 936 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (4)].node), NEW_POSTARG(-1, (yyvsp[(4) - (4)].node)));
                  ;}
    break;

  case 79:
#line 940 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 80:
#line 944 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].node)));
                  ;}
    break;

  case 81:
#line 948 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, -1);
                  ;}
    break;

  case 82:
#line 952 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG(-1, (yyvsp[(3) - (3)].node)));
                  ;}
    break;

  case 84:
#line 959 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 85:
#line 965 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (2)].node));
                  ;}
    break;

  case 86:
#line 969 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                  ;}
    break;

  case 87:
#line 975 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 88:
#line 979 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 89:
#line 985 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                  ;}
    break;

  case 90:
#line 989 "grammar19.y"
    {
                    (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
                  ;}
    break;

  case 91:
#line 993 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 92:
#line 997 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 93:
#line 1001 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 94:
#line 1005 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                  ;}
    break;

  case 95:
#line 1011 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                  ;}
    break;

  case 96:
#line 1017 "grammar19.y"
    {
                    rb_backref_error((yyvsp[(1) - (1)].node));
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 97:
#line 1024 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                    if(!(yyval.node)) (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 98:
#line 1029 "grammar19.y"
    {
                    (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
                  ;}
    break;

  case 99:
#line 1033 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 100:
#line 1037 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 101:
#line 1041 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 102:
#line 1045 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                  ;}
    break;

  case 103:
#line 1051 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                  ;}
    break;

  case 104:
#line 1057 "grammar19.y"
    {
                    rb_backref_error((yyvsp[(1) - (1)].node));
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 105:
#line 1064 "grammar19.y"
    {
                    yy_error("class/module name must be CONSTANT");
                  ;}
    break;

  case 107:
#line 1071 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 108:
#line 1075 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON2(0, (yyval.node));
                  ;}
    break;

  case 109:
#line 1079 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 113:
#line 1088 "grammar19.y"
    {
                    lex_state = EXPR_ENDFN;
                    (yyval.id) = convert_op((yyvsp[(1) - (1)].id));
                  ;}
    break;

  case 114:
#line 1093 "grammar19.y"
    {
                    lex_state = EXPR_ENDFN;
                    (yyval.id) = (yyvsp[(1) - (1)].id);
                  ;}
    break;

  case 117:
#line 1104 "grammar19.y"
    {
                    (yyval.node) = NEW_LIT(QUID2SYM((yyvsp[(1) - (1)].id)));
                  ;}
    break;

  case 119:
#line 1111 "grammar19.y"
    {
                    (yyval.node) = NEW_UNDEF((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 120:
#line 1114 "grammar19.y"
    {lex_state = EXPR_FNAME;;}
    break;

  case 121:
#line 1115 "grammar19.y"
    {
                    (yyval.node) = block_append((yyvsp[(1) - (4)].node), NEW_UNDEF((yyvsp[(4) - (4)].node)));
                  ;}
    break;

  case 122:
#line 1120 "grammar19.y"
    { (yyval.id) = '|'; ;}
    break;

  case 123:
#line 1121 "grammar19.y"
    { (yyval.id) = '^'; ;}
    break;

  case 124:
#line 1122 "grammar19.y"
    { (yyval.id) = '&'; ;}
    break;

  case 125:
#line 1123 "grammar19.y"
    { (yyval.id) = tCMP; ;}
    break;

  case 126:
#line 1124 "grammar19.y"
    { (yyval.id) = tEQ; ;}
    break;

  case 127:
#line 1125 "grammar19.y"
    { (yyval.id) = tEQQ; ;}
    break;

  case 128:
#line 1126 "grammar19.y"
    { (yyval.id) = tMATCH; ;}
    break;

  case 129:
#line 1127 "grammar19.y"
    { (yyval.id) = tNMATCH; ;}
    break;

  case 130:
#line 1128 "grammar19.y"
    { (yyval.id) = '>'; ;}
    break;

  case 131:
#line 1129 "grammar19.y"
    { (yyval.id) = tGEQ; ;}
    break;

  case 132:
#line 1130 "grammar19.y"
    { (yyval.id) = '<'; ;}
    break;

  case 133:
#line 1131 "grammar19.y"
    { (yyval.id) = tLEQ; ;}
    break;

  case 134:
#line 1132 "grammar19.y"
    { (yyval.id) = tNEQ; ;}
    break;

  case 135:
#line 1133 "grammar19.y"
    { (yyval.id) = tLSHFT; ;}
    break;

  case 136:
#line 1134 "grammar19.y"
    { (yyval.id) = tRSHFT; ;}
    break;

  case 137:
#line 1135 "grammar19.y"
    { (yyval.id) = '+'; ;}
    break;

  case 138:
#line 1136 "grammar19.y"
    { (yyval.id) = '-'; ;}
    break;

  case 139:
#line 1137 "grammar19.y"
    { (yyval.id) = '*'; ;}
    break;

  case 140:
#line 1138 "grammar19.y"
    { (yyval.id) = '*'; ;}
    break;

  case 141:
#line 1139 "grammar19.y"
    { (yyval.id) = '/'; ;}
    break;

  case 142:
#line 1140 "grammar19.y"
    { (yyval.id) = '%'; ;}
    break;

  case 143:
#line 1141 "grammar19.y"
    { (yyval.id) = tPOW; ;}
    break;

  case 144:
#line 1142 "grammar19.y"
    { (yyval.id) = '!'; ;}
    break;

  case 145:
#line 1143 "grammar19.y"
    { (yyval.id) = '~'; ;}
    break;

  case 146:
#line 1144 "grammar19.y"
    { (yyval.id) = tUPLUS; ;}
    break;

  case 147:
#line 1145 "grammar19.y"
    { (yyval.id) = tUMINUS; ;}
    break;

  case 148:
#line 1146 "grammar19.y"
    { (yyval.id) = tAREF; ;}
    break;

  case 149:
#line 1147 "grammar19.y"
    { (yyval.id) = tASET; ;}
    break;

  case 150:
#line 1148 "grammar19.y"
    { (yyval.id) = '`'; ;}
    break;

  case 192:
#line 1166 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 193:
#line 1171 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (5)].node));
                    (yyvsp[(3) - (5)].node) = NEW_RESCUE((yyvsp[(3) - (5)].node), NEW_RESBODY(0, (yyvsp[(5) - (5)].node), 0), 0);
                    (yyval.node) = node_assign((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node));
                  ;}
    break;

  case 194:
#line 1177 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    if((yyvsp[(1) - (3)].node)) {
                      QUID vid = (yyvsp[(1) - (3)].node)->nd_vid;
                      if((yyvsp[(2) - (3)].id) == tOROP) {
                        (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                        (yyval.node) = NEW_OP_ASGN_OR(gettable(vid), (yyvsp[(1) - (3)].node));
                        if(is_asgn_or_id(vid)) {
                          (yyval.node)->nd_aid = vid;
                        }
                      } else if((yyvsp[(2) - (3)].id) == tANDOP) {
                        (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                        (yyval.node) = NEW_OP_ASGN_AND(gettable(vid), (yyvsp[(1) - (3)].node));
                      } else {
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                        (yyval.node)->nd_value = NEW_CALL(gettable(vid), (yyvsp[(2) - (3)].id), NEW_LIST((yyvsp[(3) - (3)].node)));
                      }
                    } else {
                      (yyval.node) = NEW_BEGIN(0);
                    }
                  ;}
    break;

  case 195:
#line 1199 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (5)].node));
                    (yyvsp[(3) - (5)].node) = NEW_RESCUE((yyvsp[(3) - (5)].node), NEW_RESBODY(0, (yyvsp[(5) - (5)].node), 0), 0);
                    if((yyvsp[(1) - (5)].node)) {
                      QUID vid = (yyvsp[(1) - (5)].node)->nd_vid;
                      if((yyvsp[(2) - (5)].id) == tOROP) {
                        (yyvsp[(1) - (5)].node)->nd_value = (yyvsp[(3) - (5)].node);
                        (yyval.node) = NEW_OP_ASGN_OR(gettable(vid), (yyvsp[(1) - (5)].node));
                        if(is_asgn_or_id(vid)) {
                          (yyval.node)->nd_aid = vid;
                        }
                      } else if((yyvsp[(2) - (5)].id) == tANDOP) {
                        (yyvsp[(1) - (5)].node)->nd_value = (yyvsp[(3) - (5)].node);
                        (yyval.node) = NEW_OP_ASGN_AND(gettable(vid), (yyvsp[(1) - (5)].node));
                      } else {
                        (yyval.node) = (yyvsp[(1) - (5)].node);
                        (yyval.node)->nd_value = NEW_CALL(gettable(vid), (yyvsp[(2) - (5)].id), NEW_LIST((yyvsp[(3) - (5)].node)));
                      }
                    } else {
                      (yyval.node) = NEW_BEGIN(0);
                    }
                  ;}
    break;

  case 196:
#line 1222 "grammar19.y"
    {
                    NODE *args;

                    value_expr((yyvsp[(6) - (6)].node));
                    if(!(yyvsp[(3) - (6)].node)) (yyvsp[(3) - (6)].node) = NEW_ZARRAY();
                    if(nd_type((yyvsp[(3) - (6)].node)) == NODE_BLOCK_PASS) {
                      args = NEW_ARGSCAT((yyvsp[(3) - (6)].node), (yyvsp[(6) - (6)].node));
                    } else {
                      args = arg_concat((yyvsp[(3) - (6)].node), (yyvsp[(6) - (6)].node));
                    }
                    if((yyvsp[(5) - (6)].id) == tOROP) {
                      (yyvsp[(5) - (6)].id) = 0;
                    } else if((yyvsp[(5) - (6)].id) == tANDOP) {
                      (yyvsp[(5) - (6)].id) = 1;
                    }
                    (yyval.node) = NEW_OP_ASGN1((yyvsp[(1) - (6)].node), (yyvsp[(5) - (6)].id), args);
                    fixpos((yyval.node), (yyvsp[(1) - (6)].node));
                  ;}
    break;

  case 197:
#line 1241 "grammar19.y"
    {
                    value_expr((yyvsp[(5) - (5)].node));
                    if((yyvsp[(4) - (5)].id) == tOROP) {
                      (yyvsp[(4) - (5)].id) = 0;
                    } else if((yyvsp[(4) - (5)].id) == tANDOP) {
                      (yyvsp[(4) - (5)].id) = 1;
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 198:
#line 1252 "grammar19.y"
    {
                    value_expr((yyvsp[(5) - (5)].node));
                    if((yyvsp[(4) - (5)].id) == tOROP) {
                      (yyvsp[(4) - (5)].id) = 0;
                    } else if((yyvsp[(4) - (5)].id) == tANDOP) {
                      (yyvsp[(4) - (5)].id) = 1;
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 199:
#line 1263 "grammar19.y"
    {
                    value_expr((yyvsp[(5) - (5)].node));
                    if((yyvsp[(4) - (5)].id) == tOROP) {
                      (yyvsp[(4) - (5)].id) = 0;
                    } else if((yyvsp[(4) - (5)].id) == tANDOP) {
                      (yyvsp[(4) - (5)].id) = 1;
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 200:
#line 1274 "grammar19.y"
    {
                    yy_error("constant re-assignment");
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 201:
#line 1279 "grammar19.y"
    {
                    yy_error("constant re-assignment");
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 202:
#line 1284 "grammar19.y"
    {
                    rb_backref_error((yyvsp[(1) - (3)].node));
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 203:
#line 1289 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (3)].node));
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = NEW_DOT2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 204:
#line 1295 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (3)].node));
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = NEW_DOT3((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 205:
#line 1301 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '+', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 206:
#line 1305 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '-', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 207:
#line 1309 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '*', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 208:
#line 1313 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '/', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 209:
#line 1317 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '%', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 210:
#line 1321 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tPOW, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 211:
#line 1325 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL(call_bin_op((yyvsp[(2) - (4)].node), tPOW, (yyvsp[(4) - (4)].node)), tUMINUS, 0);
                  ;}
    break;

  case 212:
#line 1329 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL(call_bin_op((yyvsp[(2) - (4)].node), tPOW, (yyvsp[(4) - (4)].node)), tUMINUS, 0);
                  ;}
    break;

  case 213:
#line 1333 "grammar19.y"
    {
                    (yyval.node) = call_uni_op((yyvsp[(2) - (2)].node), tUPLUS);
                  ;}
    break;

  case 214:
#line 1337 "grammar19.y"
    {
                    (yyval.node) = call_uni_op((yyvsp[(2) - (2)].node), tUMINUS);
                  ;}
    break;

  case 215:
#line 1341 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '|', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 216:
#line 1345 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '^', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 217:
#line 1349 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '&', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 218:
#line 1353 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tCMP, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 219:
#line 1357 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '>', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 220:
#line 1361 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tGEQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 221:
#line 1365 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '<', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 222:
#line 1369 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tLEQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 223:
#line 1373 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tEQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 224:
#line 1377 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tEQQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 225:
#line 1381 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tNEQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 226:
#line 1385 "grammar19.y"
    {
                    /* TODO */
                    (yyval.node) = match_op((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    /*
                    if(nd_type($1) == NODE_LIT && TYPE($1->nd_lit) == T_REGEXP) {
                      $$ = reg_named_capture_assign($1->nd_lit, $$);
                    }
                    */
                  ;}
    break;

  case 227:
#line 1395 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tNMATCH, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 228:
#line 1399 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[(2) - (2)].node)), '!');
                  ;}
    break;

  case 229:
#line 1403 "grammar19.y"
    {
                    (yyval.node) = call_uni_op((yyvsp[(2) - (2)].node), '~');
                  ;}
    break;

  case 230:
#line 1407 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tLSHFT, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 231:
#line 1411 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tRSHFT, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 232:
#line 1415 "grammar19.y"
    {
                    (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 233:
#line 1419 "grammar19.y"
    {
                    (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 234:
#line 1422 "grammar19.y"
    {in_defined = 1;;}
    break;

  case 235:
#line 1423 "grammar19.y"
    {
                    in_defined = 0;
                    (yyval.node) = NEW_DEFINED((yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 236:
#line 1428 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (6)].node));
                    (yyval.node) = NEW_IF(cond((yyvsp[(1) - (6)].node)), (yyvsp[(3) - (6)].node), (yyvsp[(6) - (6)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (6)].node));
                  ;}
    break;

  case 237:
#line 1434 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                  ;}
    break;

  case 238:
#line 1440 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  ;}
    break;

  case 240:
#line 1449 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                  ;}
    break;

  case 241:
#line 1453 "grammar19.y"
    {
                    (yyval.node) = arg_append((yyvsp[(1) - (4)].node), NEW_HASH((yyvsp[(3) - (4)].node)));
                  ;}
    break;

  case 242:
#line 1457 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                  ;}
    break;

  case 243:
#line 1463 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 248:
#line 1477 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 249:
#line 1482 "grammar19.y"
    {
                    (yyval.node) = arg_blk_pass((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 250:
#line 1486 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                    (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 251:
#line 1491 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (4)].node), NEW_HASH((yyvsp[(3) - (4)].node)));
                    (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 253:
#line 1498 "grammar19.y"
    {
                    (yyval.val) = cmdarg_stack;
                    CMDARG_PUSH(1);
                  ;}
    break;

  case 254:
#line 1503 "grammar19.y"
    {
                    /* CMDARG_POP() */
                    cmdarg_stack = (yyvsp[(1) - (2)].val);
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 255:
#line 1511 "grammar19.y"
    {
                    (yyval.node) = NEW_BLOCK_PASS((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 256:
#line 1517 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 257:
#line 1521 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 258:
#line 1525 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 259:
#line 1531 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 260:
#line 1535 "grammar19.y"
    {
                    (yyval.node) = NEW_SPLAT((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 261:
#line 1539 "grammar19.y"
    {
                    NODE *n1;
                    if((n1 = splat_array((yyvsp[(1) - (3)].node))) != 0) {
                      (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    } else {
                      (yyval.node) = arg_append((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    }
                  ;}
    break;

  case 262:
#line 1548 "grammar19.y"
    {
                    NODE *n1;
                    if((nd_type((yyvsp[(4) - (4)].node)) == NODE_ARRAY) && (n1 = splat_array((yyvsp[(1) - (4)].node))) != 0) {
                      (yyval.node) = list_concat(n1, (yyvsp[(4) - (4)].node));
                    } else {
                      (yyval.node) = arg_concat((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node));
                    }
                  ;}
    break;

  case 263:
#line 1559 "grammar19.y"
    {
                    NODE *n1;
                    if((n1 = splat_array((yyvsp[(1) - (3)].node))) != 0) {
                      (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    } else {
                      (yyval.node) = arg_append((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    }
                  ;}
    break;

  case 264:
#line 1568 "grammar19.y"
    {
                    NODE *n1;
                    if(nd_type((yyvsp[(4) - (4)].node)) == NODE_ARRAY && (n1 = splat_array((yyvsp[(1) - (4)].node))) != 0) {
                      (yyval.node) = list_concat(n1, (yyvsp[(4) - (4)].node));
                    } else {
                      (yyval.node) = arg_concat((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node));
                    }
                  ;}
    break;

  case 265:
#line 1577 "grammar19.y"
    {
                    (yyval.node) = NEW_SPLAT((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 274:
#line 1591 "grammar19.y"
    {
                    (yyval.node) = NEW_FCALL((yyvsp[(1) - (1)].id), 0);
                  ;}
    break;

  case 275:
#line 1595 "grammar19.y"
    {
                    (yyval.num) = ruby_sourceline;
                  ;}
    break;

  case 276:
#line 1600 "grammar19.y"
    {
                    if((yyvsp[(3) - (4)].node) == NULL) {
                      (yyval.node) = NEW_NIL();
                    } else {
                      if(nd_type((yyvsp[(3) - (4)].node)) == NODE_RESCUE || nd_type((yyvsp[(3) - (4)].node)) == NODE_ENSURE) {
                        nd_set_line((yyvsp[(3) - (4)].node), (yyvsp[(2) - (4)].num));
                      }
                      (yyval.node) = NEW_BEGIN((yyvsp[(3) - (4)].node));
                    }
                    nd_set_line((yyval.node), (yyvsp[(2) - (4)].num));
                  ;}
    break;

  case 277:
#line 1611 "grammar19.y"
    {lex_state = EXPR_ENDARG;;}
    break;

  case 278:
#line 1612 "grammar19.y"
    {
                    rb_warning("(...) interpreted as grouped expression");
                    (yyval.node) = (yyvsp[(2) - (4)].node);
                  ;}
    break;

  case 279:
#line 1617 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 280:
#line 1621 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 281:
#line 1625 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 282:
#line 1629 "grammar19.y"
    {
                    if((yyvsp[(2) - (3)].node) == 0) {
                      (yyval.node) = NEW_ZARRAY(); /* zero length array*/
                    } else {
                      (yyval.node) = (yyvsp[(2) - (3)].node);
                    }
                  ;}
    break;

  case 283:
#line 1637 "grammar19.y"
    {
                    (yyval.node) = NEW_HASH((yyvsp[(2) - (3)].node));
                  ;}
    break;

  case 284:
#line 1641 "grammar19.y"
    {
                    (yyval.node) = NEW_RETURN(0);
                  ;}
    break;

  case 285:
#line 1645 "grammar19.y"
    {
                    (yyval.node) = new_yield((yyvsp[(3) - (4)].node));
                  ;}
    break;

  case 286:
#line 1649 "grammar19.y"
    {
                    (yyval.node) = NEW_YIELD(0, Qfalse);
                  ;}
    break;

  case 287:
#line 1653 "grammar19.y"
    {
                    (yyval.node) = NEW_YIELD(0, Qfalse);
                  ;}
    break;

  case 288:
#line 1656 "grammar19.y"
    {in_defined = 1;;}
    break;

  case 289:
#line 1657 "grammar19.y"
    {
                    in_defined = 0;
                    (yyval.node) = NEW_DEFINED((yyvsp[(5) - (6)].node));
                  ;}
    break;

  case 290:
#line 1662 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[(3) - (4)].node)), '!');
                  ;}
    break;

  case 291:
#line 1666 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond(NEW_NIL()), '!');
                  ;}
    break;

  case 292:
#line 1670 "grammar19.y"
    {
                    (yyvsp[(2) - (2)].node)->nd_iter = NEW_FCALL((yyvsp[(1) - (2)].id), 0);
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                    fixpos((yyvsp[(2) - (2)].node)->nd_iter, (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 294:
#line 1677 "grammar19.y"
    {
                    block_dup_check((yyvsp[(1) - (2)].node)->nd_args, (yyvsp[(2) - (2)].node));
                    (yyvsp[(2) - (2)].node)->nd_iter = (yyvsp[(1) - (2)].node);
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                    fixpos((yyval.node), (yyvsp[(1) - (2)].node));
                  ;}
    break;

  case 295:
#line 1684 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 296:
#line 1691 "grammar19.y"
    {
                    (yyval.node) = NEW_IF(cond((yyvsp[(2) - (6)].node)), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (6)].node));
                  ;}
    break;

  case 297:
#line 1699 "grammar19.y"
    {
                    (yyval.node) = NEW_UNLESS(cond((yyvsp[(2) - (6)].node)), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (6)].node));
                  ;}
    break;

  case 298:
#line 1703 "grammar19.y"
    {COND_PUSH(1);;}
    break;

  case 299:
#line 1703 "grammar19.y"
    {COND_POP();;}
    break;

  case 300:
#line 1706 "grammar19.y"
    {
                    (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (7)].node)), (yyvsp[(6) - (7)].node), 1);
                    fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                  ;}
    break;

  case 301:
#line 1710 "grammar19.y"
    {COND_PUSH(1);;}
    break;

  case 302:
#line 1710 "grammar19.y"
    {COND_POP();;}
    break;

  case 303:
#line 1713 "grammar19.y"
    {
                    (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (7)].node)), (yyvsp[(6) - (7)].node), 1);
                    fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                  ;}
    break;

  case 304:
#line 1720 "grammar19.y"
    {
                    (yyval.node) = NEW_CASE((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (5)].node));
                  ;}
    break;

  case 305:
#line 1725 "grammar19.y"
    {
                    (yyval.node) = NEW_CASE(0, (yyvsp[(3) - (4)].node));
                  ;}
    break;

  case 306:
#line 1729 "grammar19.y"
    {COND_PUSH(1);;}
    break;

  case 307:
#line 1731 "grammar19.y"
    {COND_POP();;}
    break;

  case 308:
#line 1734 "grammar19.y"
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
                    (yyval.node) = 0; /* TODO NEW_FOR(0, $5, scope); */
                    fixpos((yyval.node), (yyvsp[(2) - (9)].node));
                  ;}
    break;

  case 309:
#line 1748 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("class definition in method body");
                    class_nest++;
                    local_push(0);
                    (yyval.num) = ruby_sourceline;
                  ;}
    break;

  case 310:
#line 1757 "grammar19.y"
    {
                    (yyval.node) = NEW_CLASS((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(3) - (6)].node));
                    nd_set_line((yyval.node), (yyvsp[(4) - (6)].num));
                    local_pop();
                    class_nest--;
                  ;}
    break;

  case 311:
#line 1764 "grammar19.y"
    {
                    (yyval.num) = in_def;
                    in_def = 0;
                  ;}
    break;

  case 312:
#line 1769 "grammar19.y"
    {
                    (yyval.num) = in_single;
                    in_single = 0;
                    class_nest++;
                    local_push(0);
                  ;}
    break;

  case 313:
#line 1777 "grammar19.y"
    {
                    (yyval.node) = NEW_SCLASS((yyvsp[(3) - (8)].node), (yyvsp[(7) - (8)].node));
                    fixpos((yyval.node), (yyvsp[(3) - (8)].node));
                    local_pop();
                    class_nest--;
                    in_def = (yyvsp[(4) - (8)].num);
                    in_single = (yyvsp[(6) - (8)].num);
                  ;}
    break;

  case 314:
#line 1786 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("module definition in method body");
                    class_nest++;
                    local_push(0);
                    (yyval.num) = ruby_sourceline;
                  ;}
    break;

  case 315:
#line 1795 "grammar19.y"
    {
                    (yyval.node) = NEW_MODULE((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
                    nd_set_line((yyval.node), (yyvsp[(3) - (5)].num));
                    local_pop();
                    class_nest--;
                  ;}
    break;

  case 316:
#line 1802 "grammar19.y"
    {
                    (yyval.id) = cur_mid;
                    cur_mid = (yyvsp[(2) - (2)].id);
                    in_def++;
                    local_push(0);
                  ;}
    break;

  case 317:
#line 1811 "grammar19.y"
    {
                    /* TODO */
                    if(!(yyvsp[(5) - (6)].node)) (yyvsp[(5) - (6)].node) = NEW_NIL();
                    (yyval.node) = NEW_DEFN((yyvsp[(2) - (6)].id), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node), NOEX_PRIVATE);
                    nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    local_pop();
                    in_def--;
                    cur_mid = (yyvsp[(3) - (6)].id);
                  ;}
    break;

  case 318:
#line 1820 "grammar19.y"
    {lex_state = EXPR_FNAME;;}
    break;

  case 319:
#line 1821 "grammar19.y"
    {
                    in_single++;
                    lex_state = EXPR_ENDFN; /* force for args */
                    local_push(0);
                  ;}
    break;

  case 320:
#line 1829 "grammar19.y"
    {
                    (yyval.node) = NEW_DEFS((yyvsp[(2) - (9)].node), (yyvsp[(5) - (9)].id), (yyvsp[(7) - (9)].node), (yyvsp[(8) - (9)].node));
                    nd_set_line((yyval.node), (yyvsp[(1) - (9)].num));
                    local_pop();
                    in_single--;
                  ;}
    break;

  case 321:
#line 1836 "grammar19.y"
    {
                    (yyval.node) = NEW_BREAK(0);
                  ;}
    break;

  case 322:
#line 1840 "grammar19.y"
    {
                    (yyval.node) = NEW_NEXT(0);
                  ;}
    break;

  case 323:
#line 1844 "grammar19.y"
    {
                    (yyval.node) = NEW_REDO();
                  ;}
    break;

  case 324:
#line 1848 "grammar19.y"
    {
                    (yyval.node) = NEW_RETRY();
                  ;}
    break;

  case 325:
#line 1854 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  ;}
    break;

  case 326:
#line 1862 "grammar19.y"
    {
                    token_info_push("begin");
                  ;}
    break;

  case 327:
#line 1868 "grammar19.y"
    {
                    token_info_push("if");
                  ;}
    break;

  case 328:
#line 1874 "grammar19.y"
    {
                    token_info_push("unless");
                  ;}
    break;

  case 329:
#line 1880 "grammar19.y"
    {
                    token_info_push("while");
                  ;}
    break;

  case 330:
#line 1886 "grammar19.y"
    {
                    token_info_push("until");
                  ;}
    break;

  case 331:
#line 1892 "grammar19.y"
    {
                    token_info_push("case");
                  ;}
    break;

  case 332:
#line 1898 "grammar19.y"
    {
                    token_info_push("for");
                  ;}
    break;

  case 333:
#line 1904 "grammar19.y"
    {
                    token_info_push("class");
                  ;}
    break;

  case 334:
#line 1910 "grammar19.y"
    {
                    token_info_push("module");
                  ;}
    break;

  case 335:
#line 1916 "grammar19.y"
    {
                    token_info_push("def");
                    (yyval.num) = ruby_sourceline;
                  ;}
    break;

  case 336:
#line 1923 "grammar19.y"
    {
                    token_info_pop("end");
                  ;}
    break;

  case 343:
#line 1941 "grammar19.y"
    {
                    (yyval.node) = NEW_IF(cond((yyvsp[(2) - (5)].node)), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (5)].node));
                  ;}
    break;

  case 345:
#line 1949 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 348:
#line 1959 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                  ;}
    break;

  case 349:
#line 1963 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 350:
#line 1969 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 351:
#line 1973 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 352:
#line 1979 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (1)].node), 0);
                  ;}
    break;

  case 353:
#line 1983 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(4) - (4)].id), 0);
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (4)].node), (yyval.node));
                  ;}
    break;

  case 354:
#line 1988 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(4) - (6)].id), 0);
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (6)].node), NEW_POSTARG((yyval.node), (yyvsp[(6) - (6)].node)));
                  ;}
    break;

  case 355:
#line 1993 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (3)].node), -1);
                  ;}
    break;

  case 356:
#line 1997 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (5)].node), NEW_POSTARG(-1, (yyvsp[(5) - (5)].node)));
                  ;}
    break;

  case 357:
#line 2001 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(2) - (2)].id), 0);
                    (yyval.node) = NEW_MASGN(0, (yyval.node));
                  ;}
    break;

  case 358:
#line 2006 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(2) - (4)].id), 0);
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG((yyval.node), (yyvsp[(4) - (4)].node)));
                  ;}
    break;

  case 359:
#line 2011 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, -1);
                  ;}
    break;

  case 360:
#line 2015 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG(-1, (yyvsp[(3) - (3)].node)));
                  ;}
    break;

  case 361:
#line 2021 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].id), 0, (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 362:
#line 2025 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (8)].node), (yyvsp[(3) - (8)].node), (yyvsp[(5) - (8)].id), (yyvsp[(7) - (8)].node), (yyvsp[(8) - (8)].id));
                  ;}
    break;

  case 363:
#line 2029 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), 0, 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 364:
#line 2033 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].node), 0, (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 365:
#line 2037 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (4)].node), 0, (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 366:
#line 2041 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (2)].node), 0, 1, 0, 0);
                  ;}
    break;

  case 367:
#line 2045 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), 0, (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 368:
#line 2049 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (2)].node), 0, 0, 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 369:
#line 2053 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 370:
#line 2057 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 371:
#line 2061 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (2)].node), 0, 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 372:
#line 2065 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (4)].node), 0, (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 373:
#line 2069 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[(1) - (2)].id), 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 374:
#line 2073 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[(1) - (4)].id), (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 375:
#line 2077 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, 0, 0, (yyvsp[(1) - (1)].id));
                  ;}
    break;

  case 377:
#line 2084 "grammar19.y"
    {
                    command_start = TRUE;
                  ;}
    break;

  case 378:
#line 2090 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 379:
#line 2094 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 380:
#line 2098 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (4)].node);
                  ;}
    break;

  case 382:
#line 2105 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 385:
#line 2115 "grammar19.y"
    {
                    new_bv(get_id((yyvsp[(1) - (1)].id)));
                  ;}
    break;

  case 386:
#line 2119 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 387:
#line 2124 "grammar19.y"
    {
                    /* TODO */
                  ;}
    break;

  case 388:
#line 2127 "grammar19.y"
    {
                    (yyval.num) = lpar_beg;
                    lpar_beg = ++paren_nest;
                  ;}
    break;

  case 389:
#line 2133 "grammar19.y"
    {
                    lpar_beg = (yyvsp[(2) - (4)].num);
                    (yyval.node) = (yyvsp[(3) - (4)].node);
                    (yyval.node)->nd_body = NEW_SCOPE((yyvsp[(3) - (4)].node)->nd_head, (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 390:
#line 2141 "grammar19.y"
    {
                    (yyval.node) = NEW_LAMBDA((yyvsp[(2) - (4)].node));
                  ;}
    break;

  case 391:
#line 2145 "grammar19.y"
    {
                    (yyval.node) = NEW_LAMBDA((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 392:
#line 2151 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 393:
#line 2155 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 394:
#line 2161 "grammar19.y"
    {
                    (yyval.num) = ruby_sourceline;
                    reset_block();
                  ;}
    break;

  case 395:
#line 2166 "grammar19.y"
    {
                    (yyval.vars) = variables->block_vars;
                  ;}
    break;

  case 396:
#line 2171 "grammar19.y"
    {
                    /* TODO $$ = NEW_ITER($3, 0, extract_block_vars($5, * $<vars>4)); */
                    (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), extract_block_vars((yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                    nd_set_line((yyval.node), (yyvsp[(2) - (6)].num));
                  ;}
    break;

  case 397:
#line 2179 "grammar19.y"
    {
                    if(nd_type((yyvsp[(1) - (2)].node)) == NODE_YIELD) {
                      compile_error("block given to yield");
                    } else {
                      block_dup_check((yyvsp[(1) - (2)].node)->nd_args, (yyvsp[(2) - (2)].node));
                    }
                    (yyvsp[(2) - (2)].node)->nd_iter = (yyvsp[(1) - (2)].node);
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                    fixpos((yyval.node), (yyvsp[(1) - (2)].node));
                  ;}
    break;

  case 398:
#line 2190 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 399:
#line 2194 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 400:
#line 2200 "grammar19.y"
    {
                    (yyval.node) = NEW_FCALL((yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 401:
#line 2205 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 402:
#line 2210 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 403:
#line 2215 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), 0);
                  ;}
    break;

  case 404:
#line 2219 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (3)].node), rb_parser_sym("call"), (yyvsp[(3) - (3)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (3)].node));
                  ;}
    break;

  case 405:
#line 2224 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (3)].node), rb_parser_sym("call"), (yyvsp[(3) - (3)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (3)].node));
                  ;}
    break;

  case 406:
#line 2229 "grammar19.y"
    {
                    (yyval.node) = NEW_SUPER((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 407:
#line 2233 "grammar19.y"
    {
                    (yyval.node) = NEW_ZSUPER();
                  ;}
    break;

  case 408:
#line 2237 "grammar19.y"
    {
                    if((yyvsp[(1) - (4)].node) && nd_type((yyvsp[(1) - (4)].node)) == NODE_SELF) {
                      (yyval.node) = NEW_FCALL(tAREF, (yyvsp[(3) - (4)].node));
                    } else {
                      (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), tAREF, (yyvsp[(3) - (4)].node));
                    }
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 409:
#line 2248 "grammar19.y"
    {
                    (yyval.num) = ruby_sourceline;
                    reset_block();
                  ;}
    break;

  case 410:
#line 2252 "grammar19.y"
    { (yyval.vars) = variables->block_vars; ;}
    break;

  case 411:
#line 2254 "grammar19.y"
    {
                    /* TODO $$ = NEW_ITER($3, 0, extract_block_vars($5, * $<vars>4)); */
                    (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), extract_block_vars((yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                    nd_set_line((yyval.node), (yyvsp[(2) - (6)].num));
                  ;}
    break;

  case 412:
#line 2260 "grammar19.y"
    {
                    (yyval.num) = ruby_sourceline;
                    reset_block();
                  ;}
    break;

  case 413:
#line 2264 "grammar19.y"
    { (yyval.vars) = variables->block_vars; ;}
    break;

  case 414:
#line 2266 "grammar19.y"
    {
                    /* TODO $$ = NEW_ITER($3, 0, extract_block_vars($5, * $<vars>4)); */
                    (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), extract_block_vars((yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                    nd_set_line((yyval.node), (yyvsp[(2) - (6)].num));
                  ;}
    break;

  case 415:
#line 2276 "grammar19.y"
    {
                    (yyval.node) = NEW_WHEN((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                  ;}
    break;

  case 418:
#line 2288 "grammar19.y"
    {
                    if((yyvsp[(3) - (6)].node)) {
                      /* TODO NEW_ERRINFO() */
                      (yyvsp[(3) - (6)].node) = node_assign((yyvsp[(3) - (6)].node), NEW_GVAR(rb_parser_sym("$!")));
                      (yyvsp[(5) - (6)].node) = block_append((yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node));
                    }
                    (yyval.node) = NEW_RESBODY((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (6)].node) ? (yyvsp[(2) - (6)].node) : (yyvsp[(5) - (6)].node));
                  ;}
    break;

  case 420:
#line 2301 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 421:
#line 2305 "grammar19.y"
    {
                    if(!((yyval.node) = splat_array((yyvsp[(1) - (1)].node)))) (yyval.node) = (yyvsp[(1) - (1)].node);
                  ;}
    break;

  case 423:
#line 2312 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 425:
#line 2319 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 428:
#line 2327 "grammar19.y"
    {
                    (yyval.node) = NEW_LIT(QUID2SYM((yyvsp[(1) - (1)].id)));
                  ;}
    break;

  case 430:
#line 2334 "grammar19.y"
    {
                    NODE *node = (yyvsp[(1) - (1)].node);
                    if(!node) {
                      node = NEW_STR(STR_NEW0());
                    } else {
                      node = evstr2dstr(node);
                    }
                    (yyval.node) = node;
                  ;}
    break;

  case 433:
#line 2348 "grammar19.y"
    {
                    (yyval.node) = literal_concat((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 434:
#line 2354 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 435:
#line 2360 "grammar19.y"
    {
                    NODE *node = (yyvsp[(2) - (3)].node);
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
                        node = NEW_NODE(NODE_DXSTR, STR_NEW0(), 1, NEW_LIST(node));
                        break;
                      }
                    }
                    (yyval.node) = node;
                  ;}
    break;

  case 436:
#line 2382 "grammar19.y"
    {
                    intptr_t options = (yyvsp[(3) - (3)].num);
                    NODE *node = (yyvsp[(2) - (3)].node);
                    if(!node) {
                      node = NEW_REGEX(string_new2(""), options & ~RE_OPTION_ONCE);
                    } else {
                      switch(nd_type(node)) {
                      case NODE_STR:
                        {
                          nd_set_type(node, NODE_REGEX);
                          node->nd_cnt = options & ~RE_OPTION_ONCE;
                        }
                        break;
                      default:
                        node = NEW_NODE(NODE_DSTR, STR_NEW0(), 1, NEW_LIST(node));
                      case NODE_DSTR:
                        if(options & RE_OPTION_ONCE) {
                          nd_set_type(node, NODE_DREGX_ONCE);
                        } else {
                          nd_set_type(node, NODE_DREGX);
                        }
                        node->nd_cflag = options & ~RE_OPTION_ONCE;
                        break;
                      }
                    }
                    (yyval.node) = node;
                  ;}
    break;

  case 437:
#line 2412 "grammar19.y"
    {
                    (yyval.node) = NEW_ZARRAY();
                  ;}
    break;

  case 438:
#line 2416 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 439:
#line 2422 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 440:
#line 2426 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), evstr2dstr((yyvsp[(2) - (3)].node)));
                  ;}
    break;

  case 442:
#line 2433 "grammar19.y"
    {
                    (yyval.node) = literal_concat((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 443:
#line 2439 "grammar19.y"
    {
                    (yyval.node) = NEW_ZARRAY();
                  ;}
    break;

  case 444:
#line 2443 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 445:
#line 2449 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 446:
#line 2453 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                  ;}
    break;

  case 447:
#line 2459 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 448:
#line 2463 "grammar19.y"
    {
                    (yyval.node) = literal_concat((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 449:
#line 2469 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 450:
#line 2473 "grammar19.y"
    {
                    (yyval.node) = literal_concat((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 451:
#line 2479 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 452:
#line 2483 "grammar19.y"
    {
                    NODE *head = (yyvsp[(1) - (2)].node), *tail = (yyvsp[(2) - (2)].node);
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
                        head = list_append(NEW_DSTR(Qnil), head);
                        break;
                      }
                      (yyval.node) = list_append(head, tail);
                    }
                  ;}
    break;

  case 454:
#line 2507 "grammar19.y"
    {
                    (yyval.node) = lex_strterm;
                    lex_strterm = 0;
                    lex_state = EXPR_BEG;
                  ;}
    break;

  case 455:
#line 2513 "grammar19.y"
    {
                    lex_strterm = (yyvsp[(2) - (3)].node);
                    (yyval.node) = NEW_EVSTR((yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 456:
#line 2518 "grammar19.y"
    {
                    (yyvsp[(1) - (1)].val) = cond_stack;
                    (yyval.val) = cmdarg_stack;
                    cond_stack = 0;
                    cmdarg_stack = 0;
                    (yyval.node) = lex_strterm;
                    lex_strterm = 0;
                    lex_state = EXPR_BEG;
                  ;}
    break;

  case 457:
#line 2528 "grammar19.y"
    {
                    cond_stack = (yyvsp[(1) - (4)].val);
                    cmdarg_stack = (yyvsp[(2) - (4)].val);
                    lex_strterm = (yyvsp[(3) - (4)].node);
                    /* TODO */
                    if(((yyval.node) = (yyvsp[(3) - (4)].node)) && nd_type((yyval.node)) == NODE_NEWLINE) {
                      (yyval.node) = (yyval.node)->nd_next;
                    }
                    (yyval.node) = new_evstr((yyval.node));
                  ;}
    break;

  case 458:
#line 2540 "grammar19.y"
    {(yyval.node) = NEW_GVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 459:
#line 2541 "grammar19.y"
    {(yyval.node) = NEW_IVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 460:
#line 2542 "grammar19.y"
    {(yyval.node) = NEW_CVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 462:
#line 2547 "grammar19.y"
    {
                    lex_state = EXPR_END;
                    (yyval.id) = (yyvsp[(2) - (2)].id);
                  ;}
    break;

  case 467:
#line 2560 "grammar19.y"
    {
                    lex_state = EXPR_END;
                    if(!((yyval.node) = (yyvsp[(2) - (3)].node))) {
                      (yyval.node) = NEW_LIT(QUID2SYM(rb_parser_sym("")));
                    } else {
                      switch(nd_type((yyval.node))) {
                      case NODE_DSTR:
                        nd_set_type((yyval.node), NODE_DSYM);
                        break;
                      case NODE_STR:
                        /* TODO: this line should never fail unless nd_str is binary */
                        if(strlen(bdatae((yyval.node)->nd_str,"")) == (size_t)blength((yyval.node)->nd_str)) {
                          QUID tmp = rb_parser_sym(bdata((yyval.node)->nd_str));
                          bdestroy((yyval.node)->nd_str);
                          (yyval.node)->nd_lit = QUID2SYM(tmp);
                          nd_set_type((yyval.node), NODE_LIT);
                          break;
                        } else {
                          bdestroy((yyval.node)->nd_str);
                        }
                        /* fall through */
                      default:
                        (yyval.node) = NEW_NODE(NODE_DSYM, STR_NEW0(), 1, NEW_LIST((yyval.node)));
                        break;
                      }
                    }
                  ;}
    break;

  case 470:
#line 2592 "grammar19.y"
    {
                    (yyval.node) = NEW_NEGATE((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 471:
#line 2596 "grammar19.y"
    {
                    (yyval.node) = NEW_NEGATE((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 477:
#line 2606 "grammar19.y"
    {(yyval.id) = keyword_nil;;}
    break;

  case 478:
#line 2607 "grammar19.y"
    {(yyval.id) = keyword_self;;}
    break;

  case 479:
#line 2608 "grammar19.y"
    {(yyval.id) = keyword_true;;}
    break;

  case 480:
#line 2609 "grammar19.y"
    {(yyval.id) = keyword_false;;}
    break;

  case 481:
#line 2610 "grammar19.y"
    {(yyval.id) = keyword__FILE__;;}
    break;

  case 482:
#line 2611 "grammar19.y"
    {(yyval.id) = keyword__LINE__;;}
    break;

  case 483:
#line 2612 "grammar19.y"
    {(yyval.id) = keyword__ENCODING__;;}
    break;

  case 484:
#line 2616 "grammar19.y"
    {
                    if(!((yyval.node) = gettable((yyvsp[(1) - (1)].id)))) {
                      (yyval.node) = NEW_BEGIN(0);
                    }
                  ;}
    break;

  case 485:
#line 2624 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                  ;}
    break;

  case 488:
#line 2634 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 489:
#line 2638 "grammar19.y"
    {
                    lex_state = EXPR_BEG;
                  ;}
    break;

  case 490:
#line 2642 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(3) - (4)].node);
                  ;}
    break;

  case 491:
#line 2646 "grammar19.y"
    {
                    yyerrok;
                    (yyval.node) = 0;
                  ;}
    break;

  case 492:
#line 2653 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                    lex_state = EXPR_BEG;
                    command_start = TRUE;
                  ;}
    break;

  case 493:
#line 2659 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                  ;}
    break;

  case 494:
#line 2665 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].id), 0, (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 495:
#line 2669 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (8)].node), (yyvsp[(3) - (8)].node), (yyvsp[(5) - (8)].id), (yyvsp[(7) - (8)].node), (yyvsp[(8) - (8)].id));
                  ;}
    break;

  case 496:
#line 2673 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), 0, 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 497:
#line 2677 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].node), 0, (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 498:
#line 2681 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (4)].node), 0, (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 499:
#line 2685 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), 0, (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 500:
#line 2689 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (2)].node), 0, 0, 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 501:
#line 2693 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 502:
#line 2697 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 503:
#line 2701 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (2)].node), 0, 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 504:
#line 2705 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (4)].node), 0, (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 505:
#line 2709 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[(1) - (2)].id), 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 506:
#line 2713 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[(1) - (4)].id), (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 507:
#line 2717 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, 0, 0, (yyvsp[(1) - (1)].id));
                  ;}
    break;

  case 508:
#line 2721 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, 0, 0, 0);
                  ;}
    break;

  case 509:
#line 2727 "grammar19.y"
    {
                    yy_error("formal argument cannot be a constant");
                    (yyval.id) = 0;
                  ;}
    break;

  case 510:
#line 2732 "grammar19.y"
    {
                    yy_error("formal argument cannot be an instance variable");
                    (yyval.id) = 0;
                  ;}
    break;

  case 511:
#line 2737 "grammar19.y"
    {
                    yy_error("formal argument cannot be a global variable");
                    (yyval.id) = 0;
                  ;}
    break;

  case 512:
#line 2742 "grammar19.y"
    {
                    yy_error("formal argument cannot be a class variable");
                    (yyval.id) = 0;
                  ;}
    break;

  case 514:
#line 2750 "grammar19.y"
    {
                    formal_argument(get_id((yyvsp[(1) - (1)].id)));
                    (yyval.id) = (yyvsp[(1) - (1)].id);
                  ;}
    break;

  case 515:
#line 2757 "grammar19.y"
    {
                    arg_var(get_id((yyvsp[(1) - (1)].id)));
                    (yyval.node) = NEW_ARGS_AUX((yyvsp[(1) - (1)].id), 1);
                  ;}
    break;

  case 516:
#line 2762 "grammar19.y"
    {
                    /* TODO */
                    QUID tid = internal_id();
                    (yyval.node) = NEW_ARGS_AUX(tid, 1);
                    (yyval.node)->nd_next = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 518:
#line 2772 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                    (yyval.node)->nd_plen++;
                    (yyval.node)->nd_next = block_append((yyval.node)->nd_next, (yyvsp[(3) - (3)].node)->nd_next);
                  ;}
    break;

  case 519:
#line 2780 "grammar19.y"
    {
                    arg_var(formal_argument(get_id((yyvsp[(1) - (3)].id))));
                    (yyval.node) = assignable((yyvsp[(1) - (3)].id), (yyvsp[(3) - (3)].node));
                    (yyval.node) = NEW_OPT_ARG(0, (yyval.node));
                  ;}
    break;

  case 520:
#line 2788 "grammar19.y"
    {
                    arg_var(formal_argument(get_id((yyvsp[(1) - (3)].id))));
                    (yyval.node) = assignable((yyvsp[(1) - (3)].id), (yyvsp[(3) - (3)].node));
                    (yyval.node) = NEW_OPT_ARG(0, (yyval.node));
                  ;}
    break;

  case 521:
#line 2796 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                  ;}
    break;

  case 522:
#line 2800 "grammar19.y"
    {
                    NODE *opts = (yyvsp[(1) - (3)].node);
                    while(opts->nd_next) {
                      opts = opts->nd_next;
                    }
                    opts->nd_next = (yyvsp[(3) - (3)].node);
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 523:
#line 2811 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                  ;}
    break;

  case 524:
#line 2815 "grammar19.y"
    {
                    NODE *opts = (yyvsp[(1) - (3)].node);
                    while(opts->nd_next) {
                      opts = opts->nd_next;
                    }
                    opts->nd_next = (yyvsp[(3) - (3)].node);
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 527:
#line 2830 "grammar19.y"
    {
                    if(!is_local_id((yyvsp[(2) - (2)].id))) {
                      yy_error("rest argument must be local variable");
                    }
                    arg_var(shadowing_lvar(get_id((yyvsp[(2) - (2)].id))));
                    (yyval.id) = (yyvsp[(2) - (2)].id);
                  ;}
    break;

  case 528:
#line 2838 "grammar19.y"
    {
                    (yyval.id) = internal_id();
                    arg_var((yyval.id));
                  ;}
    break;

  case 531:
#line 2849 "grammar19.y"
    {
                    if(!is_local_id((yyvsp[(2) - (2)].id)))
                      yy_error("block argument must be local variable");
                    else if(local_id((yyvsp[(2) - (2)].id)))
                      yy_error("duplicate block argument name");
                    arg_var(shadowing_lvar(get_id((yyvsp[(2) - (2)].id))));
                    (yyval.id) = (yyvsp[(2) - (2)].id);
                  ;}
    break;

  case 532:
#line 2860 "grammar19.y"
    {
                    (yyval.id) = (yyvsp[(2) - (2)].id);
                  ;}
    break;

  case 533:
#line 2864 "grammar19.y"
    {
                    (yyval.id) = 0;
                  ;}
    break;

  case 534:
#line 2870 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  ;}
    break;

  case 535:
#line 2875 "grammar19.y"
    {lex_state = EXPR_BEG;;}
    break;

  case 536:
#line 2876 "grammar19.y"
    {
                    if((yyvsp[(3) - (4)].node) == 0) {
                      yy_error("can't define singleton method for ().");
                    } else {
                      switch(nd_type((yyvsp[(3) - (4)].node))) {
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
                        value_expr((yyvsp[(3) - (4)].node));
                        break;
                      }
                    }
                    (yyval.node) = (yyvsp[(3) - (4)].node);
                  ;}
    break;

  case 538:
#line 2901 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                  ;}
    break;

  case 540:
#line 2908 "grammar19.y"
    {
                    (yyval.node) = list_concat((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 541:
#line 2914 "grammar19.y"
    {
                    (yyval.node) = list_append(NEW_LIST((yyvsp[(1) - (3)].node)), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 542:
#line 2918 "grammar19.y"
    {
                    (yyval.node) = list_append(NEW_LIST(NEW_LIT(QUID2SYM((yyvsp[(1) - (2)].id)))), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 564:
#line 2962 "grammar19.y"
    {yyerrok;;}
    break;

  case 567:
#line 2967 "grammar19.y"
    {yyerrok;;}
    break;

  case 568:
#line 2970 "grammar19.y"
    {(yyval.node) = 0;;}
    break;


/* Line 1267 of yacc.c.  */
#line 8081 "grammar19.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (parser_state, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (parser_state, yymsg);
	  }
	else
	  {
	    yyerror (parser_state, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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

  /* Else will try to reuse look-ahead token after shifting the error
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

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
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

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (parser_state, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, parser_state);
  /* Do not reclaim the symbols of the rule which action triggered
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 2972 "grammar19.y"


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
#define parser_mbclen()           mbclen((lex_p-1),lex_pend,parser->enc)
#define parser_precise_mbclen()   rb_enc_precise_mbclen((lex_p-1),lex_pend,parser_state->enc)
#define is_identchar(p,e,enc)     (rb_enc_isalnum(*p,enc) || (*p) == '_' || !ISASCII(*p))
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
  end_seen = 0;
  ruby_sourcefile = f;
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

  lex_strterm = 0;

  return n;
}

static bool
lex_get_str(rb_parser_state* parser_state)
{
  const char *str;
  const char *beg, *end, *pend;
  int sz;

  str = bdata(lex_string);
  beg = str;

  if(lex_str_used) {
    if(blength(lex_string) == lex_str_used) {
      return false;
    }

    beg += lex_str_used;
  }

  pend = str + blength(lex_string);
  end = beg;

  while(end < pend) {
    if(*end++ == '\n') break;
  }

  sz = end - beg;
  bcatblk(line_buffer, beg, sz);
  lex_str_used += sz;

  return TRUE;
}

static bool
lex_getline(rb_parser_state* parser_state)
{
  if(!line_buffer) {
    line_buffer = cstr2bstr("");
  } else {
    btrunc(line_buffer, 0);
  }

  return lex_gets(parser_state);
}

VALUE process_parse_tree(rb_parser_state*, VALUE, NODE*, QUID*);

VALUE
string_to_ast(VALUE ptp, const char *f, bstring s, int line)
{
  int n;
  VALUE ret;
  rb_parser_state* parser_state = parser_alloc_state();

  lex_string = s;
  lex_gets = lex_get_str;
  processor = ptp;
  ruby_sourceline = line - 1;
  compile_for_eval = 1;

  n = yycompile(parser_state, (char*)f, line);

  if(!parse_error) {
    for(std::vector<bstring>::iterator i = magic_comments->begin();
        i != magic_comments->end();
        i++) {
      rb_funcall(ptp, rb_intern("add_magic_comment"), 1,
        rb_str_new((const char*)(*i)->data, (*i)->slen));
    }
    ret = process_parse_tree(parser_state, ptp, top_node, NULL);
  } else {
    ret = Qnil;
  }
  pt_free(parser_state);
  free(parser_state);
  return ret;
}

static bool parse_io_gets(rb_parser_state* parser_state) {
  if(feof(lex_io)) {
    return false;
  }

  while(TRUE) {
    char *ptr, buf[1024];
    int read;

    ptr = fgets(buf, sizeof(buf), lex_io);
    if(!ptr) {
      return false;
    }

    read = strlen(ptr);
    bcatblk(line_buffer, ptr, read);

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
  rb_parser_state* parser_state = parser_alloc_state();

  lex_io = file;
  lex_gets = parse_io_gets;
  processor = ptp;
  ruby_sourceline = start - 1;

  n = yycompile(parser_state, (char*)f, start);

  if(!parse_error) {
    for(std::vector<bstring>::iterator i = magic_comments->begin();
        i != magic_comments->end();
        i++) {
      rb_funcall(ptp, rb_intern("add_magic_comment"), 1,
        rb_str_new((const char*)(*i)->data, (*i)->slen));
    }
      ret = process_parse_tree(parser_state, ptp, top_node, NULL);

      if(end_seen && lex_io) {
        rb_funcall(ptp, rb_sData, 1, ULONG2NUM(ftell(lex_io)));
      }
  } else {
    ret = Qnil;
  }

  pt_free(parser_state);
  free(parser_state);
  return ret;
}

static VALUE
parser_str_new(const char *p, long n, rb_encoding *enc, int func, rb_encoding *enc0)
{
  VALUE str;

/*
  str = rb_enc_str_new(p, n, enc);
  if(!(func & STR_FUNC_REGEXP) && rb_enc_asciicompat(enc)) {
    if(rb_enc_str_coderange(str) == ENC_CODERANGE_7BIT) {
      // ?
    } else if(enc0 == rb_usascii_encoding() && enc != rb_utf8_encoding()) {
      rb_enc_associate(str, rb_ascii8bit_encoding());
    }
  }
*/

  return str;
}

static inline int
parser_nextc(rb_parser_state* parser_state)
{
  int c;

  if(lex_p == lex_pend) {
      bstring v;

      if(!lex_getline(parser_state)) return -1;
      v = line_buffer;

      if(heredoc_end > 0) {
        ruby_sourceline = heredoc_end;
        heredoc_end = 0;
      }
      ruby_sourceline++;

      /* This code is setup so that lex_pend can be compared to
         the data in lex_lastline. Thats important, otherwise
         the heredoc code breaks. */
      if(lex_lastline) {
        bassign(lex_lastline, v);
      } else {
        lex_lastline = bstrcpy(v);
      }

      v = lex_lastline;

      lex_pbeg = lex_p = bdata(v);
      lex_pend = lex_p + blength(v);
  }
  c = (unsigned char)*(lex_p++);
  if(c == '\r' && lex_p < lex_pend && *(lex_p) == '\n') {
    lex_p++;
    c = '\n';
    column = 0;
  } else if(c == '\n') {
    column = 0;
  } else {
    column++;
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

#define lex_goto_eol()  (lex_p = lex_pend)
#define peek(c) (lex_p < lex_pend && (c) == *lex_p)

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
  int len = rb_enc_codelen(c, enc);
  rb_enc_mbcput(c, tokspace(len), enc);
}

static int
parser_tokadd_escape(rb_parser_state* parser_state, rb_encoding **encp)
{
  int c;
  int flags = 0;
  size_t numlen;

first:
  switch (c = nextc()) {
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
    if (flags & ESCAPE_CONTROL) goto eof;
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
    char kcode = 0;
    int options = 0;
    int c;

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

#define mixed_error(enc1, enc2) if (!errbuf) {	\
    size_t len = sizeof(mixed_msg) - 4;	\
    len += strlen(rb_enc_name(enc1));	\
    len += strlen(rb_enc_name(enc2));	\
    errbuf = ALLOCA_N(char, len);		\
    snprintf(errbuf, len, mixed_msg, rb_enc_name(enc1), rb_enc_name(enc2));		\
    yy_error(errbuf);			\
  }

#define mixed_escape(beg, enc1, enc2) do {	\
    char *pos = lex_p;		\
    lex_p = beg;				\
    mixed_error(enc1, enc2);		\
    lex_p = pos;				\
  } while (0)

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
      switch (c) {
      case '\n':
        if(func & STR_FUNC_QWORDS) break;
        if(func & STR_FUNC_EXPAND) continue;
        tokadd('\\');
        break;

      case '\\':
        if(func & STR_FUNC_ESCAPE) tokadd(c);
        break;

      case 'u':
        if ((func & STR_FUNC_EXPAND) == 0) {
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
      if (tokadd_mbchar(c) == -1) return -1;
      continue;
    } else if((func & STR_FUNC_QWORDS) && ISSPACE(c)) {
      pushback(c);
      break;
    }
    if (c & 0x80) {
      has_nonascii = 1;
      if (enc != *encp) {
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
               (VALUE)((term) | ((paren) << (CHAR_BIT * 2))), NULL)
#define pslval ((YYSTYPE *)lval)
static int
parser_parse_string(rb_parser_state* parser_state, NODE *quote)
{
  int func = quote->nd_func;
  int term = nd_term(quote);
  int paren = nd_paren(quote);
  int c, space = 0;
  rb_encoding* enc = parser_state->enc;

  long start_line = ruby_sourceline;

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
    pslval->num = regx_options();
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
    ruby_sourceline = nd_line(quote);
    if (func & STR_FUNC_REGEXP) {
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
    tokadd((char)func);
    term = c;

    /* Where of where has the term gone.. */
    while((c = nextc()) != -1 && c != term) {
      len = mbclen(c);
      do {
        tokadd((char)c);
      } while(--len > 0 && (c = nextc()) != -1);
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
    tokadd((char)(func |= str_dquote));
    do {
      len = mbclen(c);
      do { tokadd((char)c); } while(--len > 0 && (c = nextc()) != -1);
    } while((c = nextc()) != -1 && parser_is_identchar());
    pushback(c);
    break;
  }


  /* Fixup the token buffer, ie set the last character to null. */
  tokfix();
  len = lex_p - lex_pbeg;
  lex_p = lex_pend;
  pslval->id = 0;

  /* Tell the lexer that we're inside a string now. nd_lit is
     the heredoc identifier that we watch the stream for to
     detect the end of the heredoc. */
  bstring str = bstrcpy(lex_lastline);
  lex_strterm = node_newnode( NODE_HEREDOC,
                             (VALUE)string_new(tok(), toklen()),  /* nd_lit */
                             (VALUE)len,                          /* nd_nth */
                             (VALUE)str);                         /* nd_orig */
  return term == '`' ? tXSTRING_BEG : tSTRING_BEG;
}

static void
parser_heredoc_restore(rb_parser_state* parser_state, NODE *here)
{
  bstring line = here->nd_orig;

  bdestroy(lex_lastline);

  lex_lastline = line;
  lex_pbeg = bdata(line);
  lex_pend = lex_pbeg + blength(line);
  lex_p = lex_pbeg + here->nd_nth;
  heredoc_end = ruby_sourceline;
  ruby_sourceline = nd_line(here);
  bdestroy((bstring)here->nd_lit);
}

static int
parser_whole_match_p(rb_parser_state* parser_state, const char *eos, int len, int indent)
{
  char *p = lex_pbeg;
  int n;

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
  long len;
  bstring str = NULL;
  rb_encoding* enc = rb_usascii_encoding();

  /* eos == the heredoc ident that we found when the heredoc started */
  eos = bdata(here->nd_str);
  len = blength(here->nd_str) - 1;

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
      p = bdata(lex_lastline);
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
        bcatblk(str, p, pend - p);
      } else {
        str = blk2bstr(p, pend - p);
      }
      if(pend < lex_pend) bcatblk(str, "\n", 1);
      lex_p = lex_pend;
      if(nextc() == -1) {
        if(str) bdestroy(str);
        goto error;
      }
    } while(!whole_match_p(eos, len, indent));
  }
  else {
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
        pslval->node = NEW_STR(string_new(tok(), toklen()));
        return tSTRING_CONTENT;
      }

      /* I think this consumes the \n */
      tokadd((char)nextc());
      if((c = nextc()) == -1) goto error;
    } while(!whole_match_p(eos, len, indent));
    str = string_new(tok(), toklen());
  }
  heredoc_restore(lex_strterm);
  lex_strterm = NEW_STRTERM(-1, 0, 0);
  pslval->node = NEW_STR(str);
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

static QUID
parser_formal_argument(rb_parser_state* parser_state, QUID lhs)
{
  if(!is_local_id(lhs))
    yy_error("formal argument must be local variable");
  shadowing_lvar(lhs);
  return lhs;
}

static int
parser_lvar_defined(rb_parser_state* parser_state, QUID id) {
  return local_id(id);
}

static char*
parse_comment(rb_parser_state* parser_state) {
  int len = lex_pend - lex_p;

  char* str = lex_p;
  while(len-- > 0 && ISSPACE(str[0])) str++;
  if(len <= 2) return NULL;

  if(str[0] == '-' && str[1] == '*' && str[2] == '-') return str;

  return NULL;
}

static void
parser_prepare(rb_parser_state* parser_state)
{
  int c = nextc();
  switch (c) {
  case '#':
    if (peek('!')) parser_state->has_shebang = 1;
    break;
  case 0xef:		/* UTF-8 BOM marker */
    if (lex_pend - lex_p >= 2 &&
        (unsigned char)lex_p[0] == 0xbb &&
        (unsigned char)lex_p[1] == 0xbf) {
      parser_state->enc = rb_utf8_encoding();
      lex_p += 2;
      lex_pbeg = lex_p;
      return;
    }
    break;
  case EOF:
    return;
  }
  pushback(c);
  // TODO: lex_lastline into a String
  // parser_state->enc = rb_enc_get(lex_lastline);
  parser_state->enc = rb_usascii_encoding();
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
  bstring cur_line;
  enum lex_state_e last_state;
  rb_encoding *enc;
  int mb;

  /*
  c = nextc();
  printf("lex char: %c\n", c);
  pushback(c, parser_state);
  */

  if(lex_strterm) {
    int token;
    if(nd_type(lex_strterm) == NODE_HEREDOC) {
      token = here_document(lex_strterm);
      if(token == tSTRING_END) {
        lex_strterm = 0;
        lex_state = EXPR_END;
      }
    }
    else {
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
    if(char* str = parse_comment(parser_state)) {
        int len = lex_pend - str - 1; // - 1 for the \n
        cur_line = blk2bstr(str, len);
        magic_comments->push_back(cur_line);
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
        --ruby_sourceline;
        lex_nextline = lex_lastline;
      case -1:		/* EOF no decrement*/
        lex_goto_eol();
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
        pslval->id = tPOW;
        lex_state = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(c);
      c = tPOW;
    } else {
      if(c == '=') {
        pslval->id = '*';
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
    lex_state = EXPR_BEG;
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
        for (;;) {
          lex_goto_eol();
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
        lex_goto_eol();
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
    }
    else if(c == '>') {
      return tASSOC;
    }
    pushback(c);
    return '=';

  case '<':
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
        pslval->id = tLSHFT;
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
        pslval->id = tRSHFT;
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
    pslval->id = 0; /* so that xstring gets used normally */
    return tXSTRING_BEG;

  case '\'':
    lex_strterm = NEW_STRTERM(str_squote, '\'', 0);
    pslval->id = 0; /* so that xstring gets used normally */
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
    if(rb_enc_isspace(c, parser_state->enc)) {
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
    } else if((rb_enc_isalnum(c, parser_state->enc) || c == '_') &&
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
        pslval->id = tANDOP;
        lex_state = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(c);
      return tANDOP;
    } else if(c == '=') {
      pslval->id = '&';
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
      lex_state = EXPR_ARG; break;
    default:
      lex_state = EXPR_BEG;
    }
    return c;

  case '|':
    if((c = nextc()) == '|') {
      lex_state = EXPR_BEG;
      if((c = nextc()) == '=') {
        pslval->id = tOROP;
        lex_state = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(c);
      return tOROP;
    }
    if(c == '=') {
      pslval->id = '|';
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
      pslval->id = '+';
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
      pslval->id = '-';
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
        tokadd((char)c);
        c = nextc();
      }
	    if(c == '0') {
#define no_digits() do {yy_error("numeric literal without digits"); return 0;} while (0)
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
          set_yylval_literal(rb_cstr_to_inum(tok(), 16, FALSE));
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
          set_yylval_literal(rb_cstr_to_inum(tok(), 2, FALSE));
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
          set_yylval_literal(rb_cstr_to_inum(tok(), 10, FALSE));
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
            set_yylval_literal(rb_cstr_to_inum(tok(), 8, FALSE));
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
          set_yylval_literal(INT2FIX(0));
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

        case '_':	/* `_' in number just ignored */
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
      set_yylval_literal(rb_float_new(d));
      return tFLOAT;
    }
    set_yylval_literal(rb_cstr_to_inum(tok(), 10, FALSE));
    return tINTEGER;
	}

  case ']':
  case '}':
    paren_nest--;
  case ')':
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
      lex_strterm = NEW_STRTERM(str_ssym, (intptr_t)c, 0);
      break;
    case '"':
      lex_strterm = NEW_STRTERM(str_dsym, (intptr_t)c, 0);
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
      arg_ambiguous();
      lex_strterm = NEW_STRTERM(str_regexp, '/', 0);
      return tREGEXP_BEG;
    }
    switch(lex_state) {
    case EXPR_FNAME: case EXPR_DOT:
      lex_state = EXPR_ARG; break;
    default:
      lex_state = EXPR_BEG; break;
    }
    return '/';

  case '^':
    if((c = nextc()) == '=') {
      set_yylval_id('^');
      lex_state = EXPR_BEG;
      return tOP_ASGN;
    }
    switch(lex_state) {
    case EXPR_FNAME: case EXPR_DOT:
      lex_state = EXPR_ARG; break;
    default:
      lex_state = EXPR_BEG; break;
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
    lex_state = EXPR_DOT;
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
        if(rb_enc_isalnum(term, parser_state->enc) || !parser_isascii()) {
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
      lex_state = EXPR_ARG; break;
    default:
      lex_state = EXPR_BEG; break;
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
    case '~':             /* $~: match-data */
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
      tokadd('$');
      tokadd(c);
      tokfix();
      set_yylval_name(rb_intern(tok()));
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
      set_yylval_name(rb_intern(tok()));
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
    }
    if(!parser_is_identchar()) {
      pushback(c);
      return '@';
    }
    break;

  case '_':
    if(was_bol() && whole_match_p("__END__", 7, 0)) {
      end_seen = 1;
      eofp = true;
      return -1;
    }
    newtok();
    break;

  default:
    if(!parser_is_identchar()) {
      rb_compile_error(parser_state, "Invalid char `\\%03o' in expression", c);
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
              (!peek('=') || (lex_p + 1 < lex_pend && lex_p[1] == '>'))) {
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
      if((lex_state == EXPR_BEG && !cmd_state) || IS_ARG()) {
        if(peek(':') && !(lex_p + 1 < lex_pend && lex_p[1] == ':')) {
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
            set_yylval_name(rb_intern(kw->name));
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
      } else if (lex_state == EXPR_FNAME) {
        lex_state = EXPR_ENDFN;
      } else {
        lex_state = EXPR_END;
      }
    }
    {
      QUID ident = TOK_INTERN(!ENC_SINGLE(mb));

      set_yylval_name(ident);
      if(last_state != EXPR_DOT && is_local_id(ident) && lvar_defined(ident)) {
        lex_state = EXPR_END;
      }
    }

/*  if(is_local_id(pslval->id) && local_id(pslval->id)) { */
/*      lex_state = EXPR_END; */
/*  } */

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



NODE*
parser_node_newnode(rb_parser_state* parser_state, enum node_type type,
                 VALUE a0, VALUE a1, VALUE a2)
{
  NODE *n = (NODE*)pt_allocate(parser_state, sizeof(NODE));

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
parser_newline_node(rb_parser_state* parser_state, NODE *node)
{
  NODE *nl = 0;
  if(node) {
    if(nd_type(node) == NODE_NEWLINE) return node;
    nl = NEW_NEWLINE(node);
    fixpos(nl, node);
    nl->nd_nth = nd_line(node);
  }
  return nl;
}

static void
fixpos(NODE *node, NODE *orig)
{
  if(!node) return;
  if(!orig) return;
  if(orig == (NODE*)1) return;
  node->nd_file = orig->nd_file;
  nd_set_line(node, nd_line(orig));
}

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

static NODE*
parser_block_append(rb_parser_state* parser_state, NODE *head, NODE *tail)
{
  NODE *end, *h = head;

  if(tail == 0) return head;

again:
  if(h == 0) return tail;
  switch(nd_type(h)) {
  case NODE_NEWLINE:
    h = h->nd_next;
    goto again;
  case NODE_STR:
  case NODE_LIT:
    parser_warning(parser_state, h, "unused literal ignored");
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

  if(verbose) {
    NODE *nd = end->nd_head;
  newline:
    switch(nd_type(nd)) {
    case NODE_RETURN:
    case NODE_BREAK:
    case NODE_NEXT:
    case NODE_REDO:
    case NODE_RETRY:
      parser_warning(parser_state, nd, "statement not reached");
      break;

    case NODE_NEWLINE:
      nd = nd->nd_next;
      goto newline;

    default:
      break;
    }
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

/* concat two string literals */
static NODE *
parser_literal_concat(rb_parser_state* parser_state, NODE *head, NODE *tail)
{
  enum node_type htype;

  if(!head) return tail;
  if(!tail) return head;

  htype = (enum node_type)nd_type(head);
  if(htype == NODE_EVSTR) {
    NODE *node = NEW_DSTR(string_new(0, 0));
    head = list_append(node, head);
  }
  switch(nd_type(tail)) {
  case NODE_STR:
    if(htype == NODE_STR) {
      if(head->nd_str) {
        bconcat(head->nd_str, tail->nd_str);
        bdestroy(tail->nd_str);
      } else {
        head = tail;
      }
    }
    else {
      list_append(head, tail);
    }
    break;

  case NODE_DSTR:
    if(htype == NODE_STR) {
      bconcat(head->nd_str, tail->nd_str);
      bdestroy(tail->nd_str);

      tail->nd_lit = head->nd_lit;
      head = tail;
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
    node = list_append(NEW_DSTR(string_new(0, 0)), node);
  }
  return node;
}

static NODE *
parser_new_evstr(rb_parser_state* parser_state, NODE *node)
{
  NODE *head = node;

again:
  if(node) {
    switch(nd_type(node)) {
    case NODE_STR: case NODE_DSTR: case NODE_EVSTR:
      return node;
    case NODE_NEWLINE:
      node = node->nd_next;
      goto again;
    }
  }
  return NEW_EVSTR(head);
}

static NODE *
parser_call_bin_op(rb_parser_state* parser_state, NODE *recv, QUID id, NODE *arg1)
{
    value_expr(recv);
    value_expr(arg1);
    return NEW_CALL(recv, id, NEW_LIST(arg1));
}

static NODE *
parser_call_uni_op(rb_parser_state* parser_state, NODE *recv, QUID id)
{
    value_expr(recv);
    return NEW_CALL(recv, id, 0);
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
call_op(NODE *recv, QUID id, int narg, NODE *arg1, rb_parser_state* parser_state)
{
  value_expr(recv);
  if(narg == 1) {
    value_expr(arg1);
    arg1 = NEW_LIST(arg1);
  } else {
    arg1 = 0;
  }

  id = convert_op(id);

  NODE* n = NEW_CALL(recv, id, arg1);

  fixpos(n, recv);

  return n;
}

static NODE*
parser_match_op(rb_parser_state* parser_state, NODE *node1, NODE *node2)
{
  local_cnt('~');

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
mel_gettable(rb_parser_state* parser_state, QUID id)
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
    return NEW_FIXNUM(ruby_sourceline);
  } else if(is_local_id(id)) {
    if(local_id(id)) return NEW_LVAR(id);
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
  /* FIXME: indicate which identifier. */
  rb_compile_error(parser_state, "identifier is not valid 1\n");
  return 0;
}

static NODE*
parser_assignable(rb_parser_state* parser_state, QUID id, NODE *val)
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
    rb_compile_error(parser_state, "identifier %s is not valid to set", rb_id2name(id));
  }
  return 0;
}

static QUID
parser_shadowing_lvar(rb_parser_state* parser_state, QUID name)
{
  QUID uscore;

  CONST_ID(uscore, "_");
  if(uscore == name) return name;
  /* TODO shadowing variables */
#if 0
  if(dyna_in_block()) {
    if(dvar_curr(name)) {
      yyerror("duplicated argument name");
    } else if(dvar_defined(name) || local_id(name)) {
      rb_warningS("shadowing outer local variable - %s", rb_id2name(name));
      vtable_add(lvtbl->vars, name);
    }
  } else {
    if(local_id(name)) {
      yyerror("duplicated argument name");
    }
  }
#endif
  return name;
}

static void
parser_reset_block(rb_parser_state* parser_state) {
  if(!variables->block_vars) {
    variables->block_vars = var_table_create();
  } else {
    variables->block_vars = var_table_push(variables->block_vars);
  }
}

static NODE *
parser_extract_block_vars(rb_parser_state* parser_state, NODE* node, var_table vars)
{
  int i;
  NODE *var, *out = node;

  // we don't create any DASGN_CURR nodes
  goto out;

  if(!node) goto out;
  if(var_table_size(vars) == 0) goto out;

  var = NULL;
  for(i = 0; i < var_table_size(vars); i++) {
    var = NEW_DASGN_CURR(var_table_get(vars, i), var);
  }
  out = block_append(var, node);

out:
  variables->block_vars = var_table_pop(variables->block_vars);

  return out;
}

static NODE*
parser_assignable(QUID id, NODE *val, rb_parser_state* parser_state)
{
  value_expr(val);
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
  } else if(is_local_id(id)) {
    if(variables->block_vars) {
      var_table_add(variables->block_vars, id);
    }
    return NEW_LASGN(id, val);
  } else if(is_global_id(id)) {
    return NEW_GASGN(id, val);
  } else if(is_instance_id(id)) {
    return NEW_IASGN(id, val);
  } else if(is_const_id(id)) {
    if(in_def || in_single)
      yy_error("dynamic constant assignment");
    return NEW_CDECL(id, val, 0);
  } else if(is_class_id(id)) {
    if(in_def || in_single) return NEW_CVASGN(id, val);
    return NEW_CVDECL(id, val);
  } else {
    /* FIXME: indicate which identifier. */
    rb_compile_error(parser_state, "identifier is not valid 2 (%d)\n", id);
  }
  return 0;
}

static void
parser_new_bv(rb_parser_state* parser_state, QUID name)
{
  if(!name) return;
  if(!is_local_id(name)) {
    rb_compile_error(parser_state, "invalid local variable - %s", rb_id2name(name));
    return;
  }
  shadowing_lvar(name);
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

static QUID
rb_id_attrset(QUID id)
{
  id &= ~ID_SCOPE_MASK;
  id |= ID_ATTRSET;
  return id;
}

static NODE *
parser_attrset(rb_parser_state* parser_state, NODE *recv, QUID id)
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
    switch (nd_type(node)) {
    case NODE_DEFN:
    case NODE_DEFS:
      parser_warning(parser_state, node, "void value expression");
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

static int
value_expr0(NODE *node, rb_parser_state* parser_state)
{
  int cond = 0;

  while(node) {
    switch(nd_type(node)) {
    case NODE_DEFN:
    case NODE_DEFS:
      parser_warning(parser_state, node, "void value expression");
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
      if(!value_expr(node->nd_body)) return FALSE;
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
parser_void_expr0(rb_parser_state* parser_state, NODE *node)
{
  const char *useless = NULL;

  if(!verbose) return;

again:
  if(!node) return;
  switch(nd_type(node)) {
  case NODE_NEWLINE:
    node = node->nd_next;
    goto again;

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

  if(useless) {
    int line = ruby_sourceline;

    ruby_sourceline = nd_line(node);
    rb_warn("useless use of %s in void context", useless);
    ruby_sourceline = line;
  }
}

static void
parser_void_stmts(NODE *node, rb_parser_state* parser_state)
{
  if(!verbose) return;
  if(!node) return;
  if(nd_type(node) != NODE_BLOCK) return;

  for (;;) {
    if(!node->nd_next) return;
    void_expr(node->nd_head);
    node = node->nd_next;
  }
}

static NODE *
remove_begin(NODE *node)
{
  NODE **n = &node;
  while(*n) {
    switch(nd_type(*n)) {
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
assign_in_cond(NODE *node, rb_parser_state* parser_state)
{
  switch(nd_type(node)) {
  case NODE_MASGN:
    yy_error("multiple assignment in conditional");
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

  switch(nd_type(node->nd_value)) {
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
  if(strcmp(ruby_sourcefile, "-e") == 0)
    return TRUE;
  return FALSE;
}

static void
warn_unless_e_option(rb_parser_state* parser_state, NODE *node, const char *str)
{
  if(!e_option_supplied()) parser_warning(parser_state, node, str);
}

static NODE *cond0(NODE *node, rb_parser_state* parser_state);

static NODE*
range_op(NODE *node, rb_parser_state* parser_state)
{
  enum node_type type;

  if(!e_option_supplied()) return node;
  if(node == 0) return 0;

  value_expr(node);
  node = cond0(node, parser_state);
  type = (enum node_type)nd_type(node);
  if(type == NODE_NEWLINE) {
    node = node->nd_next;
    type = (enum node_type)nd_type(node);
  }
  if(type == NODE_LIT && FIXNUM_P(node->nd_lit)) {
    warn_unless_e_option(parser_state, node, "integer literal in conditional range");
    return call_op(node,tEQ,1,NEW_GVAR(rb_parser_sym("$.")), parser_state);
  }
  return node;
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
cond0(NODE *node, rb_parser_state* parser_state)
{
  if(node == 0) return 0;
  assign_in_cond(node, parser_state);

  switch(nd_type(node)) {
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
    node->nd_1st = cond0(node->nd_1st, parser_state);
    node->nd_2nd = cond0(node->nd_2nd, parser_state);
    break;

  case NODE_DOT2:
  case NODE_DOT3:
    node->nd_beg = range_op(node->nd_beg, parser_state);
    node->nd_end = range_op(node->nd_end, parser_state);
    if(nd_type(node) == NODE_DOT2) nd_set_type(node,NODE_FLIP2);
    else if(nd_type(node) == NODE_DOT3) nd_set_type(node, NODE_FLIP3);
    if(!e_option_supplied()) {
      int b = literal_node(node->nd_beg);
      int e = literal_node(node->nd_end);
      if((b == 1 && e == 1) || (b + e >= 2 && verbose)) {
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
parser_cond(rb_parser_state* parser_state, NODE *node)
{
  if(node == 0) return 0;
  value_expr(node);
  if(nd_type(node) == NODE_NEWLINE){
    node->nd_next = cond0(node->nd_next, parser_state);
    return node;
  }
  return cond0(node, parser_state);
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
    if(nd_type(node) == NODE_ARRAY && node->nd_next == 0) {
      node = node->nd_head;
      state = Qfalse;
    }
    if(node && nd_type(node) == NODE_SPLAT) {
      state = Qtrue;
    }
  } else {
    state = Qfalse;
  }
  return NEW_YIELD(node, state);
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
parser_new_args(rb_parser_state* parser_state, NODE *m, NODE *o, QUID r, NODE *p, QUID b)
{
  int saved_line = ruby_sourceline;
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
  ruby_sourceline = saved_line;
  return node;
}
static NODE*
new_call(rb_parser_state* parser_state,NODE *r,QUID m,NODE *a)
{
  if(a && nd_type(a) == NODE_BLOCK_PASS) {
    a->nd_iter = NEW_CALL(r,convert_op(m),a->nd_head);
    return a;
  }
  return NEW_CALL(r,convert_op(m),a);
}

static NODE*
new_fcall(rb_parser_state* parser_state,QUID m,NODE *a)
{
  if(a && nd_type(a) == NODE_BLOCK_PASS) {
    a->nd_iter = NEW_FCALL(m,a->nd_head);
    return a;
  }
  return NEW_FCALL(m,a);
}

static NODE*
parser_new_super(rb_parser_state* parser_state,NODE *a)
{
  if(a && nd_type(a) == NODE_BLOCK_PASS) {
    a->nd_iter = NEW_SUPER(a->nd_head);
    return a;
  }
  return NEW_SUPER(a);
}

static int
parser_local_var(rb_parser_state* parser_state, QUID id)
{
  /* TODO */
  return 1;
}

static int
parser_local_id(rb_parser_state* parser_state, QUID id)
{
  /* TODO */
#if 0
  struct vtable *vars, *args;

  vars = lvtbl->vars;
  args = lvtbl->args;

  while(vars && POINTER_P(vars->prev)) {
    vars = vars->prev;
    args = args->prev;
  }

  if(vars && vars->prev == DVARS_INHERIT) {
    return rb_local_defined(id);
  } else {
    return (vtable_included(args, id) || vtable_included(vars, id));
  }
#endif
  return 1;
}

static int
parser_arg_var(rb_parser_state* parser_state, QUID id)
{
  /* TODO */
  return 1;
}

static void
parser_local_push(rb_parser_state* parser_state, int top)
{
  variables = LocalState::push(variables);
}

static void
parser_local_pop(rb_parser_state* parser_state)
{
  variables = LocalState::pop(variables);
}


static QUID*
parser_local_tbl(rb_parser_state* parser_state)
{
  QUID *lcl_tbl;
  var_table tbl;
  int i, len;
  tbl = variables->local_vars;
  len = var_table_size(tbl);
  lcl_tbl = (QUID*)pt_allocate(parser_state, sizeof(QUID) * (len + 3));
  lcl_tbl[0] = (QUID)len;
  lcl_tbl[1] = '_';
  lcl_tbl[2] = '~';
  for(i = 0; i < len; i++) {
    lcl_tbl[i + 3] = var_table_get(tbl, i);
  }
  return lcl_tbl;
}

static intptr_t
parser_local_cnt(rb_parser_state* parser_state, QUID id)
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
  if(variables->block_vars) {
    idx = var_table_find_chained(variables->block_vars, id);
    if(idx >= 0) {
      return idx;
    } else {
      return var_table_add(variables->block_vars, id);
    }
  }

  idx = var_table_find(variables->local_vars, id);
  if(idx >= 0) {
    return idx + 2;
  }

  return var_table_add(variables->local_vars, id);
}

static int
mel_local_id(rb_parser_state* parser_state, QUID id)
{
  if(variables->block_vars) {
    if(var_table_find_chained(variables->block_vars, id) >= 0) return 1;
  }

  if(var_table_find(variables->local_vars, id) >= 0) return 1;
  return 0;
}

// TODO: encoding support, rb_usascii_encoding(), see rb_intern3
static QUID
rb_parser_sym(const char *name)
{
  const char *m = name;
  const char *e = m + strlen(name);
  rb_encoding *enc = rb_usascii_encoding();
  QUID id, pre, qrk, bef;
  int last;

  id = 0;
  last = strlen(name)-1;
  switch(*name) {
  case '$':
    id |= ID_GLOBAL;
    m++;
    if(!is_identchar(m, e, enc)) m++;
    break;
  case '@':
    if(name[1] == '@') {
      m++;
      id |= ID_CLASS;
    } else {
      id |= ID_INSTANCE;
    }
    m++;
    break;
  default:
    if(name[0] != '_' && !ISALPHA(name[0]) && !ismbchar(name[0])) {
      int i;

      for (i=0; op_tbl[i].token; i++) {
        if(*op_tbl[i].name == *name &&
          strcmp(op_tbl[i].name, name) == 0) {
          id = op_tbl[i].token;
          return id;
        }
      }
    }

    if(name[last] == '=') {
      id = ID_ATTRSET;
    } else if(ISUPPER(name[0])) {
      id = ID_CONST;
    } else {
      id = ID_LOCAL;
    }
    break;
  }
  while(m <= name + last && is_identchar(m, e, enc)) {
    m += mbclen(*m);
  }
  if(*m) id = ID_JUNK;
  qrk = (QUID)quark_from_string(name);
  pre = qrk + tLAST_TOKEN;
  bef = id;
  id |= ( pre << ID_SCOPE_SHIFT );
  return id;
}

static int
scan_oct(const char *start, size_t len, size_t *retlen)
{
  register const char *s = start;
  register unsigned long retval = 0;

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
  register unsigned long retval = 0;
  const char *tmp;

  while(len-- && *s && (tmp = strchr(hexdigit, *s))) {
    retval <<= 4;
    retval |= (tmp - hexdigit) & 15;
    s++;
  }
  *retlen = s - start;
  return retval;
}

static QUID
parser_internal_id(rb_parser_state *parser_state)
{
  /* TODO */
  return 1;
}

const char *op_to_name(QUID id) {
  if(id < tLAST_TOKEN) {
    int i = 0;

    for (i=0; op_tbl[i].token; i++) {
      if(op_tbl[i].token == id)
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

}; // namespace grammar18
}; // namespace melbourne


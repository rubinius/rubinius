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
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "ruby.h"

#include "grammar19.hpp"
#include "parser_state19.hpp"
#include "visitor19.hpp"
#include "symbols.hpp"

namespace melbourne {

namespace grammar19 {

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

long mel_sourceline;
static char *mel_sourcefile;

#define ruby_sourceline mel_sourceline
#define ruby_sourcefile mel_sourcefile

static void parser_prepare(rb_parser_state*);
static int parser_yyerror(rb_parser_state*, const char *);
#define yy_error(msg)   parser_yyerror(parser_state, msg)
#define yyerror         parser_yyerror

#define YYLEX_PARAM parser_state

#define ID_SCOPE_SHIFT  3
#define ID_SCOPE_MASK   0x07
#define ID_LOCAL        0x00
#define ID_INSTANCE     0x01
#define ID_GLOBAL       0x03
#define ID_ATTRSET      0x04
#define ID_CONST        0x05
#define ID_CLASS        0x06
#define ID_JUNK         0x07
#define ID_INTERNAL     ID_JUNK

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

static NODE *negate_lit(NODE*);
static NODE *parser_ret_args(rb_parser_state*, NODE*);
static NODE *arg_blk_pass(NODE*,NODE*);
static NODE *new_call(rb_parser_state*,NODE*,QUID,NODE*);
static NODE *new_fcall(rb_parser_state*,QUID,NODE*);
static NODE *parser_new_super(rb_parser_state*, NODE*);
static NODE *parser_new_yield(rb_parser_state*, NODE*);

static NODE *parser_gettable(rb_parser_state*,QUID);
#define gettable(i) parser_gettable((rb_parser_state*)parser_state, i)
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

#define QUID2SYM(x)   quark_to_symbol(x)

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
  memory_cur = NULL;
  memory_last_addr = NULL;
  current_pool = 0;
  pool_size = 0;
  memory_size = 204800;
  memory_pools = NULL;
  emit_warnings = 0;
  verbose = RTEST(ruby_verbose);
  magic_comments = new std::vector<bstring>;
  start_lines = new std::list<StartPosition>;

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
  delete variables;

  for(std::vector<bstring>::iterator i = magic_comments->begin();
      i != magic_comments->end();
      i++) {
    bdestroy(*i);
  }

  delete magic_comments;
  delete start_lines;

  if(lex_io_buf) free(lex_io_buf);

  if(!memory_pools) return;

  for(i = 0; i <= current_pool; i++) {
    free(memory_pools[i]);
  }
  free(memory_pools);

}

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
#define STR_NEW(p,n)          rb_enc_str_new((p),(n),parser_state->enc)
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
#line 432 "grammar19.y"
{
    VALUE val;
    NODE *node;
    QUID id;
    int num;
    var_table vars;
}
/* Line 193 of yacc.c.  */
#line 740 "grammar19.cpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 753 "grammar19.cpp"

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
#define YYLAST   10110

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  138
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  176
/* YYNRULES -- Number of rules.  */
#define YYNRULES  569
/* YYNRULES -- Number of states.  */
#define YYNSTATES  979

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
    1432,  1435,  1436,  1439,  1441,  1442,  1446,  1447,  1448,  1454,
    1456,  1458,  1460,  1462,  1465,  1467,  1469,  1471,  1473,  1477,
    1479,  1481,  1484,  1487,  1489,  1491,  1493,  1495,  1497,  1499,
    1501,  1503,  1505,  1507,  1509,  1511,  1513,  1515,  1517,  1519,
    1521,  1522,  1527,  1530,  1534,  1537,  1544,  1553,  1558,  1565,
    1570,  1577,  1580,  1585,  1592,  1595,  1600,  1603,  1608,  1610,
    1611,  1613,  1615,  1617,  1619,  1621,  1623,  1625,  1629,  1631,
    1635,  1639,  1643,  1645,  1649,  1651,  1655,  1657,  1659,  1662,
    1664,  1666,  1668,  1671,  1674,  1676,  1678,  1679,  1684,  1686,
    1689,  1691,  1695,  1699,  1702,  1704,  1706,  1708,  1710,  1712,
    1714,  1716,  1718,  1720,  1722,  1724,  1726,  1727,  1729,  1730,
    1732,  1735,  1738,  1739,  1741,  1743,  1745,  1747,  1749,  1752
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     139,     0,    -1,    -1,   140,   141,    -1,   142,   306,    -1,
     313,    -1,   143,    -1,   142,   312,   143,    -1,     1,   143,
      -1,   148,    -1,    -1,    47,   144,   126,   141,   127,    -1,
     146,   249,   221,   252,    -1,   147,   306,    -1,   313,    -1,
     148,    -1,   147,   312,   148,    -1,     1,   148,    -1,    -1,
      45,   170,   149,   170,    -1,    45,    54,    54,    -1,    45,
      54,    64,    -1,    45,    54,    63,    -1,     6,   171,    -1,
     148,    40,   151,    -1,   148,    41,   151,    -1,   148,    42,
     151,    -1,   148,    43,   151,    -1,   148,    44,   148,    -1,
      48,   126,   146,   127,    -1,   165,   109,   152,    -1,   158,
     109,   152,    -1,   278,    87,   152,    -1,   206,   128,   181,
     309,    87,   152,    -1,   206,   129,    52,    87,   152,    -1,
     206,   129,    56,    87,   152,    -1,   206,    85,    56,    87,
     152,    -1,   206,    85,    52,    87,   152,    -1,   279,    87,
     152,    -1,   165,   109,   188,    -1,   158,   109,   177,    -1,
     158,   109,   188,    -1,   150,    -1,   152,    -1,   150,    37,
     150,    -1,   150,    38,   150,    -1,    39,   307,   150,    -1,
     123,   152,    -1,   175,    -1,   150,    -1,   157,    -1,   153,
      -1,   240,    -1,   240,   129,   303,   183,    -1,   240,    85,
     303,   183,    -1,    -1,    -1,    94,   155,   227,   156,   146,
     127,    -1,   302,   183,    -1,   302,   183,   154,    -1,   206,
     129,   303,   183,    -1,   206,   129,   303,   183,   154,    -1,
     206,    85,   303,   183,    -1,   206,    85,   303,   183,   154,
      -1,    32,   183,    -1,    31,   183,    -1,    30,   182,    -1,
      21,   182,    -1,    22,   182,    -1,   160,    -1,    89,   159,
     308,    -1,   160,    -1,    89,   159,   308,    -1,   162,    -1,
     162,   161,    -1,   162,    95,   164,    -1,   162,    95,   164,
     130,   163,    -1,   162,    95,    -1,   162,    95,   130,   163,
      -1,    95,   164,    -1,    95,   164,   130,   163,    -1,    95,
      -1,    95,   130,   163,    -1,   164,    -1,    89,   159,   308,
      -1,   161,   130,    -1,   162,   161,   130,    -1,   161,    -1,
     163,   130,   161,    -1,   276,    -1,   206,   128,   181,   309,
      -1,   206,   129,    52,    -1,   206,    85,    52,    -1,   206,
     129,    56,    -1,   206,    85,    56,    -1,    86,    56,    -1,
     279,    -1,   276,    -1,   206,   128,   181,   309,    -1,   206,
     129,    52,    -1,   206,    85,    52,    -1,   206,   129,    56,
      -1,   206,    85,    56,    -1,    86,    56,    -1,   279,    -1,
      52,    -1,    56,    -1,    86,   166,    -1,   166,    -1,   206,
      85,   166,    -1,    52,    -1,    56,    -1,    53,    -1,   173,
      -1,   174,    -1,   168,    -1,   272,    -1,   169,    -1,   274,
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
     109,   175,    -1,   165,   109,   175,    44,   175,    -1,   278,
      87,   175,    -1,   278,    87,   175,    44,   175,    -1,   206,
     128,   181,   309,    87,   175,    -1,   206,   129,    52,    87,
     175,    -1,   206,   129,    56,    87,   175,    -1,   206,    85,
      52,    87,   175,    -1,   206,    85,    56,    87,   175,    -1,
      86,    56,    87,   175,    -1,   279,    87,   175,    -1,   175,
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
      -1,    -1,    46,   307,   176,   175,    -1,   175,   110,   175,
     307,   111,   175,    -1,   189,    -1,   175,    -1,   313,    -1,
     187,   310,    -1,   187,   130,   300,   310,    -1,   300,   310,
      -1,   132,   181,   308,    -1,   313,    -1,   179,    -1,   313,
      -1,   182,    -1,   157,    -1,   187,   186,    -1,   300,   186,
      -1,   187,   130,   300,   186,    -1,   185,    -1,    -1,   184,
     182,    -1,    96,   177,    -1,   130,   185,    -1,   130,    -1,
     313,    -1,   177,    -1,    95,   177,    -1,   187,   130,   177,
      -1,   187,   130,    95,   177,    -1,   187,   130,   177,    -1,
     187,   130,    95,   177,    -1,    95,   177,    -1,   253,    -1,
     254,    -1,   257,    -1,   258,    -1,   259,    -1,   262,    -1,
     277,    -1,   279,    -1,    53,    -1,    -1,   207,   190,   145,
     217,    -1,    -1,    90,   150,   191,   308,    -1,    89,   146,
     133,    -1,   206,    85,    56,    -1,    86,    56,    -1,    92,
     178,   134,    -1,    93,   299,   127,    -1,    30,    -1,    31,
     132,   182,   308,    -1,    31,   132,   308,    -1,    31,    -1,
      -1,    46,   307,   132,   192,   150,   308,    -1,    39,   132,
     150,   308,    -1,    39,   132,   308,    -1,   302,   242,    -1,
     241,    -1,   241,   242,    -1,    97,   232,    -1,   208,   151,
     218,   146,   220,   217,    -1,   209,   151,   218,   146,   221,
     217,    -1,    -1,    -1,   210,   193,   151,   219,   194,   146,
     217,    -1,    -1,    -1,   211,   195,   151,   219,   196,   146,
     217,    -1,   212,   151,   306,   247,   217,    -1,   212,   306,
     247,   217,    -1,    -1,    -1,   213,   222,    25,   197,   151,
     219,   198,   146,   217,    -1,    -1,   214,   167,   280,   199,
     145,   217,    -1,    -1,    -1,   214,    83,   150,   200,   311,
     201,   145,   217,    -1,    -1,   215,   167,   202,   145,   217,
      -1,    -1,   216,   168,   203,   282,   145,   217,    -1,    -1,
      -1,   216,   297,   305,   204,   168,   205,   282,   145,   217,
      -1,    21,    -1,    22,    -1,    23,    -1,    24,    -1,   189,
      -1,     7,    -1,    11,    -1,    12,    -1,    18,    -1,    19,
      -1,    16,    -1,    20,    -1,     3,    -1,     4,    -1,     5,
      -1,    10,    -1,   311,    -1,    13,    -1,   311,    13,    -1,
     311,    -1,    27,    -1,   221,    -1,    14,   151,   218,   146,
     220,    -1,   313,    -1,    15,   146,    -1,   165,    -1,   158,
      -1,   285,    -1,    89,   225,   308,    -1,   223,    -1,   224,
     130,   223,    -1,   224,    -1,   224,   130,    95,   285,    -1,
     224,   130,    95,   285,   130,   224,    -1,   224,   130,    95,
      -1,   224,   130,    95,   130,   224,    -1,    95,   285,    -1,
      95,   285,   130,   224,    -1,    95,    -1,    95,   130,   224,
      -1,   287,   130,   290,   130,   293,   296,    -1,   287,   130,
     290,   130,   293,   130,   287,   296,    -1,   287,   130,   290,
     296,    -1,   287,   130,   290,   130,   287,   296,    -1,   287,
     130,   293,   296,    -1,   287,   130,    -1,   287,   130,   293,
     130,   287,   296,    -1,   287,   296,    -1,   290,   130,   293,
     296,    -1,   290,   130,   293,   130,   287,   296,    -1,   290,
     296,    -1,   290,   130,   287,   296,    -1,   293,   296,    -1,
     293,   130,   287,   296,    -1,   295,    -1,   313,    -1,   228,
      -1,   114,   229,   114,    -1,    76,    -1,   114,   226,   229,
     114,    -1,   313,    -1,   135,   230,    -1,   231,    -1,   230,
     130,   231,    -1,    52,    -1,   284,    -1,    -1,    -1,   233,
     234,   235,   236,    -1,   132,   283,   229,   308,    -1,   283,
      -1,   107,   146,   127,    -1,    29,   146,    10,    -1,    -1,
      -1,    28,   238,   227,   239,   146,    10,    -1,   157,   237,
      -1,   240,   129,   303,   180,    -1,   240,    85,   303,   180,
      -1,   302,   179,    -1,   206,   129,   303,   180,    -1,   206,
      85,   303,   179,    -1,   206,    85,   304,    -1,   206,   129,
     179,    -1,   206,    85,   179,    -1,    32,   179,    -1,    32,
      -1,   206,   128,   181,   309,    -1,    -1,    -1,   126,   243,
     227,   244,   146,   127,    -1,    -1,    -1,    26,   245,   227,
     246,   146,    10,    -1,    17,   187,   218,   146,   248,    -1,
     221,    -1,   247,    -1,     8,   250,   251,   218,   146,   249,
      -1,   313,    -1,   177,    -1,   188,    -1,   313,    -1,    88,
     165,    -1,   313,    -1,     9,   146,    -1,   313,    -1,   275,
      -1,   272,    -1,   274,    -1,   255,    -1,    62,    -1,   256,
      -1,   255,   256,    -1,    99,   264,   106,    -1,   100,   265,
     106,    -1,   101,   266,    65,    -1,   102,   136,   106,    -1,
     102,   260,   106,    -1,    -1,   260,   261,   136,    -1,   267,
      -1,   261,   267,    -1,   103,   136,   106,    -1,   103,   263,
     106,    -1,    -1,   263,    61,   136,    -1,    -1,   264,   267,
      -1,    -1,   265,   267,    -1,    -1,   266,   267,    -1,    61,
      -1,    -1,   105,   268,   271,    -1,    -1,    -1,   104,   269,
     270,   146,   127,    -1,    54,    -1,    55,    -1,    57,    -1,
     279,    -1,    98,   273,    -1,   168,    -1,    55,    -1,    54,
      -1,    57,    -1,    98,   265,   106,    -1,    59,    -1,    60,
      -1,   122,    59,    -1,   122,    60,    -1,    52,    -1,    55,
      -1,    54,    -1,    56,    -1,    57,    -1,    34,    -1,    33,
      -1,    35,    -1,    36,    -1,    50,    -1,    49,    -1,    51,
      -1,   276,    -1,   276,    -1,    63,    -1,    64,    -1,   311,
      -1,    -1,   113,   281,   151,   311,    -1,     1,   311,    -1,
     132,   283,   308,    -1,   283,   311,    -1,   287,   130,   291,
     130,   293,   296,    -1,   287,   130,   291,   130,   293,   130,
     287,   296,    -1,   287,   130,   291,   296,    -1,   287,   130,
     291,   130,   287,   296,    -1,   287,   130,   293,   296,    -1,
     287,   130,   293,   130,   287,   296,    -1,   287,   296,    -1,
     291,   130,   293,   296,    -1,   291,   130,   293,   130,   287,
     296,    -1,   291,   296,    -1,   291,   130,   287,   296,    -1,
     293,   296,    -1,   293,   130,   287,   296,    -1,   295,    -1,
      -1,    56,    -1,    55,    -1,    54,    -1,    57,    -1,   284,
      -1,    52,    -1,   285,    -1,    89,   225,   308,    -1,   286,
      -1,   287,   130,   286,    -1,    52,   109,   177,    -1,    52,
     109,   206,    -1,   289,    -1,   290,   130,   289,    -1,   288,
      -1,   291,   130,   288,    -1,   119,    -1,    95,    -1,   292,
      52,    -1,   292,    -1,   116,    -1,    96,    -1,   294,    52,
      -1,   130,   295,    -1,   313,    -1,   277,    -1,    -1,   132,
     298,   150,   308,    -1,   313,    -1,   300,   310,    -1,   301,
      -1,   300,   130,   301,    -1,   177,    88,   177,    -1,    58,
     177,    -1,    52,    -1,    56,    -1,    53,    -1,    52,    -1,
      56,    -1,    53,    -1,   173,    -1,    52,    -1,    53,    -1,
     173,    -1,   129,    -1,    85,    -1,    -1,   312,    -1,    -1,
     137,    -1,   307,   133,    -1,   307,   134,    -1,    -1,   137,
      -1,   130,    -1,   135,    -1,   137,    -1,   311,    -1,   312,
     135,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   580,   580,   580,   604,   611,   615,   619,   623,   629,
     631,   630,   645,   668,   675,   679,   683,   687,   693,   693,
     697,   701,   708,   713,   717,   722,   727,   735,   743,   748,
     757,   762,   768,   790,   805,   816,   827,   832,   843,   848,
     853,   858,   863,   866,   867,   871,   875,   879,   883,   886,
     894,   895,   898,   899,   903,   910,   915,   909,   925,   930,
     937,   942,   949,   954,   961,   966,   971,   975,   979,   985,
     986,   992,   993,   999,  1003,  1007,  1011,  1015,  1019,  1023,
    1027,  1031,  1035,  1041,  1042,  1048,  1052,  1058,  1062,  1068,
    1072,  1076,  1080,  1084,  1088,  1094,  1100,  1107,  1112,  1116,
    1120,  1124,  1128,  1134,  1140,  1147,  1151,  1154,  1158,  1162,
    1168,  1169,  1170,  1171,  1176,  1183,  1184,  1187,  1191,  1194,
    1198,  1198,  1204,  1205,  1206,  1207,  1208,  1209,  1210,  1211,
    1212,  1213,  1214,  1215,  1216,  1217,  1218,  1219,  1220,  1221,
    1222,  1223,  1224,  1225,  1226,  1227,  1228,  1229,  1230,  1231,
    1232,  1235,  1235,  1235,  1236,  1236,  1237,  1237,  1237,  1238,
    1238,  1238,  1238,  1239,  1239,  1239,  1239,  1240,  1240,  1240,
    1241,  1241,  1241,  1241,  1242,  1242,  1242,  1242,  1243,  1243,
    1243,  1243,  1244,  1244,  1244,  1244,  1245,  1245,  1245,  1245,
    1246,  1246,  1249,  1254,  1260,  1282,  1305,  1324,  1335,  1346,
    1357,  1362,  1367,  1372,  1378,  1384,  1388,  1392,  1396,  1400,
    1404,  1408,  1412,  1416,  1420,  1424,  1428,  1432,  1436,  1440,
    1444,  1448,  1452,  1456,  1460,  1464,  1468,  1478,  1482,  1486,
    1490,  1494,  1498,  1502,  1506,  1506,  1511,  1517,  1523,  1531,
    1532,  1536,  1540,  1546,  1552,  1553,  1556,  1557,  1560,  1565,
    1569,  1574,  1579,  1582,  1582,  1594,  1600,  1604,  1608,  1614,
    1618,  1622,  1631,  1642,  1651,  1660,  1666,  1667,  1668,  1669,
    1670,  1671,  1672,  1673,  1674,  1679,  1678,  1695,  1695,  1700,
    1704,  1708,  1712,  1720,  1724,  1728,  1732,  1736,  1740,  1740,
    1745,  1749,  1753,  1759,  1760,  1767,  1771,  1779,  1787,  1787,
    1787,  1794,  1794,  1794,  1801,  1808,  1813,  1815,  1812,  1832,
    1831,  1848,  1853,  1847,  1870,  1869,  1886,  1885,  1903,  1904,
    1903,  1919,  1923,  1927,  1931,  1937,  1945,  1951,  1957,  1963,
    1969,  1975,  1981,  1987,  1993,  1999,  2006,  2012,  2013,  2014,
    2017,  2018,  2021,  2022,  2031,  2032,  2038,  2039,  2042,  2046,
    2052,  2056,  2062,  2066,  2071,  2076,  2080,  2084,  2089,  2094,
    2098,  2104,  2108,  2112,  2116,  2120,  2124,  2128,  2132,  2136,
    2140,  2144,  2148,  2152,  2156,  2160,  2166,  2167,  2173,  2177,
    2181,  2187,  2188,  2194,  2195,  2198,  2202,  2208,  2211,  2208,
    2224,  2228,  2234,  2238,  2245,  2250,  2244,  2262,  2273,  2277,
    2283,  2288,  2293,  2298,  2302,  2307,  2312,  2316,  2320,  2332,
    2336,  2331,  2344,  2348,  2343,  2357,  2365,  2366,  2369,  2381,
    2384,  2388,  2392,  2395,  2399,  2402,  2406,  2409,  2410,  2414,
    2417,  2429,  2430,  2431,  2437,  2443,  2465,  2496,  2500,  2507,
    2510,  2516,  2517,  2523,  2527,  2534,  2537,  2544,  2547,  2554,
    2557,  2564,  2567,  2590,  2592,  2591,  2603,  2609,  2602,  2625,
    2626,  2627,  2628,  2631,  2638,  2639,  2640,  2641,  2644,  2671,
    2672,  2673,  2677,  2683,  2684,  2685,  2686,  2687,  2688,  2689,
    2690,  2691,  2692,  2693,  2694,  2697,  2705,  2711,  2712,  2715,
    2720,  2719,  2727,  2734,  2740,  2746,  2750,  2754,  2758,  2762,
    2766,  2770,  2774,  2778,  2782,  2786,  2790,  2794,  2798,  2803,
    2808,  2813,  2818,  2823,  2830,  2831,  2838,  2843,  2854,  2855,
    2863,  2871,  2879,  2883,  2894,  2898,  2909,  2910,  2913,  2921,
    2928,  2929,  2932,  2943,  2947,  2953,  2959,  2959,  2983,  2984,
    2990,  2991,  2997,  3001,  3007,  3008,  3009,  3012,  3013,  3014,
    3015,  3018,  3019,  3020,  3023,  3024,  3027,  3028,  3031,  3032,
    3035,  3038,  3041,  3042,  3043,  3046,  3047,  3050,  3051,  3054
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
  "regexp_contents", "string_content", "@33", "@34", "@35", "string_dvar",
  "symbol", "sym", "dsym", "numeric", "variable", "var_ref", "var_lhs",
  "backref", "superclass", "@36", "f_arglist", "f_args", "f_bad_arg",
  "f_norm_arg", "f_arg_item", "f_arg", "f_opt", "f_block_opt",
  "f_block_optarg", "f_optarg", "restarg_mark", "f_rest_arg",
  "blkarg_mark", "f_block_arg", "opt_f_block_arg", "singleton", "@37",
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
     265,   266,   266,   267,   268,   267,   269,   270,   267,   271,
     271,   271,   271,   272,   273,   273,   273,   273,   274,   275,
     275,   275,   275,   276,   276,   276,   276,   276,   276,   276,
     276,   276,   276,   276,   276,   277,   278,   279,   279,   280,
     281,   280,   280,   282,   282,   283,   283,   283,   283,   283,
     283,   283,   283,   283,   283,   283,   283,   283,   283,   283,
     284,   284,   284,   284,   285,   285,   286,   286,   287,   287,
     288,   289,   290,   290,   291,   291,   292,   292,   293,   293,
     294,   294,   295,   296,   296,   297,   298,   297,   299,   299,
     300,   300,   301,   301,   302,   302,   302,   303,   303,   303,
     303,   304,   304,   304,   305,   305,   306,   306,   307,   307,
     308,   309,   310,   310,   310,   311,   311,   312,   312,   313
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
       2,     0,     2,     1,     0,     3,     0,     0,     5,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     3,     1,
       1,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     4,     2,     3,     2,     6,     8,     4,     6,     4,
       6,     2,     4,     6,     2,     4,     2,     4,     1,     0,
       1,     1,     1,     1,     1,     1,     1,     3,     1,     3,
       3,     3,     1,     3,     1,     3,     1,     1,     2,     1,
       1,     1,     2,     2,     1,     1,     0,     4,     1,     2,
       1,     3,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     1,     0,     1,
       2,     2,     0,     1,     1,     1,     1,     1,     2,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     0,     1,     0,   333,   334,   335,     0,   326,
     327,   328,   331,   329,   330,   332,   321,   322,   323,   324,
     284,   253,   253,   479,   478,   480,   481,   558,     0,   558,
      10,     0,   483,   482,   484,   473,   546,   475,   474,   476,
     477,   469,   470,   431,   487,   488,     0,     0,     0,     0,
       0,   569,   569,    81,   387,   449,   447,   449,   451,   439,
     445,     0,     0,     0,     3,   556,     6,     9,    42,    43,
      51,    50,     0,    69,     0,    73,    83,     0,    48,   237,
       0,   275,     0,     0,   298,   301,   556,     0,     0,     0,
       0,    52,   293,   266,   267,   430,   432,   268,   269,   270,
     271,   428,   429,   427,   485,   272,     0,   273,   253,     5,
       8,   161,   172,   162,   185,   158,   178,   168,   167,   188,
     189,   183,   166,   165,   160,   186,   190,   191,   170,   159,
     173,   177,   179,   171,   164,   180,   187,   182,   181,   174,
     184,   169,   157,   176,   175,   156,   163,   154,   155,   151,
     152,   153,   110,   112,   111,   146,   147,   143,   125,   126,
     127,   134,   131,   133,   128,   129,   148,   149,   135,   136,
     140,   130,   132,   122,   123,   124,   137,   138,   139,   141,
     142,   144,   145,   150,   115,   117,   119,    23,   113,   114,
     116,   118,     0,     0,     0,     0,     0,     0,     0,   248,
       0,   238,   259,    67,   252,   569,     0,   485,     0,   273,
     569,   540,    68,    66,   558,    65,     0,   569,   406,    64,
     558,   559,     0,     0,    18,   234,     0,     0,   321,   322,
     284,   287,   407,   213,     0,     0,   214,   281,     0,     0,
       0,   556,    15,   558,    71,    14,   277,     0,   562,   562,
     239,     0,     0,   562,   538,   558,     0,     0,     0,    79,
     325,     0,    89,    96,   295,   388,   466,   465,   467,   464,
       0,   463,     0,     0,     0,     0,     0,     0,     0,   471,
     472,    47,   228,   229,   565,   566,     4,   567,   557,     0,
       0,     0,     0,     0,     0,     0,   394,   397,     0,    85,
       0,    77,    74,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   569,     0,     0,    49,     0,     0,     0,     0,   556,
       0,   557,     0,   347,   346,     0,     0,   485,   273,   105,
     106,     0,     0,   108,     0,     0,   485,   273,   314,   181,
     174,   184,   169,   151,   152,   153,   110,   111,   536,   316,
     535,     0,     0,     0,   412,   409,   294,   433,     0,     0,
     400,    58,   292,   120,   543,   281,   260,   255,     0,     0,
     257,   249,   258,     0,   569,     0,     0,     0,   257,   250,
     558,     0,   286,   254,   558,   247,   246,   558,   291,    46,
      20,    22,    21,     0,   288,     0,     0,     0,     0,     0,
       0,    17,   558,   279,    13,   557,    70,   558,   282,   564,
     563,   240,   564,   242,   283,   539,     0,    95,   471,   472,
      87,    82,     0,     0,   569,     0,   509,   453,   456,   454,
     468,   450,   434,   448,   435,   436,   452,   437,   438,     0,
     441,   443,     0,   444,     0,     0,   568,     7,    24,    25,
      26,    27,    28,    44,    45,   569,     0,    31,    40,     0,
      41,   558,     0,    75,    86,    30,   192,   259,    39,   210,
     218,   223,   224,   225,   220,   222,   232,   233,   226,   227,
     203,   204,   230,   231,   558,   219,   221,   215,   216,   217,
     205,   206,   207,   208,   209,   547,   552,   548,   553,   405,
     253,   403,   558,   547,   549,   548,   550,   404,   253,     0,
     569,   338,     0,   337,     0,     0,     0,     0,     0,     0,
     281,     0,   569,     0,   306,   311,   105,   106,   107,     0,
     490,   309,   489,     0,   569,     0,     0,     0,   509,   555,
     554,   318,   547,   548,   253,   253,   569,   569,    32,   194,
      38,   202,    55,    59,     0,   192,   542,     0,   261,   256,
     569,   551,   548,   558,   547,   548,   541,   285,   560,   243,
     290,    19,     0,   235,     0,    29,     0,   569,   201,    72,
      16,   278,   562,     0,    80,    92,    94,   558,   547,   548,
     515,   512,   511,   510,   513,     0,   527,   531,   530,   526,
     509,     0,   391,   514,   516,   518,   569,   524,   569,   529,
     569,     0,   508,   457,     0,   440,   442,   446,   211,   212,
     379,   569,   395,   377,   376,   265,     0,    84,    78,     0,
       0,     0,     0,     0,   402,    62,     0,   408,     0,     0,
     245,   401,    60,   244,   336,   276,   569,   569,   419,   569,
     339,   569,   341,   299,   340,   302,     0,     0,   305,   551,
     280,   558,   547,   548,     0,     0,   492,     0,     0,   105,
     106,   109,   558,     0,   558,   509,     0,     0,     0,   399,
      54,   398,    53,   413,   410,     0,   569,   121,   262,   251,
       0,     0,   408,     0,     0,   558,    11,   241,    88,    90,
       0,   515,     0,   359,   350,   352,   558,   348,   569,     0,
       0,   389,     0,   501,   534,     0,   504,   528,     0,   506,
     532,     0,   459,   460,   461,   455,   462,   515,     0,   569,
       0,   569,   522,   569,   569,   375,   381,     0,     0,   263,
      76,   193,     0,    37,   199,    36,   200,    63,   561,     0,
      34,   197,    35,   198,    61,   420,   421,   569,   422,     0,
     569,   344,     0,     0,   342,     0,     0,     0,   304,     0,
       0,   408,     0,   312,     0,     0,   408,   315,   537,   558,
       0,   494,   319,     0,     0,   195,    56,     0,   289,   520,
     558,     0,   357,     0,   517,   558,     0,     0,   519,   569,
     569,   533,   569,   525,   569,   569,     0,     0,   385,   382,
     383,   386,     0,   378,   366,   368,     0,   371,     0,   373,
       0,   264,   236,    33,   196,     0,     0,   424,   345,     0,
      12,   426,     0,   296,   297,     0,     0,   261,   569,   307,
       0,   491,   310,   493,   317,   509,     0,     0,     0,   349,
     360,     0,   355,   351,   390,   393,   392,     0,   497,     0,
     499,     0,   505,     0,   502,   507,   458,     0,   521,     0,
     380,   569,   569,   569,   523,   569,   569,   396,     0,   423,
       0,    97,   104,     0,   425,     0,   300,   303,   416,   417,
     415,     0,     0,     0,   414,   411,     0,     0,   358,     0,
     353,   569,   569,   569,   569,   281,     0,   384,     0,   363,
       0,   365,   372,     0,   369,   374,   103,     0,   569,     0,
     569,   569,     0,   313,     0,    57,   356,     0,   498,     0,
     495,   500,   503,   551,   280,   569,   569,   569,   569,   551,
     102,   558,   547,   548,   418,   343,   308,   320,   354,   569,
     364,     0,   361,   367,   370,   408,   496,   569,   362
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
     911,   688,   685,   860,   556,   558,   698,   865,   234,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,   665,
     532,   673,   783,   784,   346,   724,   725,   726,   749,   642,
     643,   750,   829,   830,   264,   265,   446,   621,   731,   297,
     475,   757,    91,    92,   382,   567,   804,   566,   803,   539,
     910,   667,   777,   846,   850,    93,    94,    95,    96,    97,
      98,    99,   276,   459,   100,   278,   272,   270,   274,   451,
     634,   633,   741,   745,   101,   271,   102,   103,   207,   105,
     208,   209,   551,   687,   696,   697,   623,   624,   625,   626,
     627,   752,   753,   628,   629,   630,   631,   821,   733,   371,
     557,   252,   210,   211,   108,   596,   521,   561,   286,   401,
     402,   657,   431,   533,   341,   245
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -734
static const yytype_int16 yypact[] =
{
    -734,   141,  2210,  -734,  6589,  -734,  -734,  -734,  6128,  -734,
    -734,  -734,  -734,  -734,  -734,  -734,  6807,  6807,  -734,  -734,
    6807,  2914,  2521,  -734,  -734,  -734,  -734,   265,  5999,    10,
    -734,    24,  -734,  -734,  -734,  5248,  2652,  -734,  -734,  5372,
    -734,  -734,  -734,  -734,  -734,  -734,  8006,  8006,    97,  4043,
    8115,  7025,  7352,  6379,  -734,  5870,  -734,  -734,  -734,    20,
      31,    57,  8224,  8006,  -734,   297,  -734,   784,   322,  -734,
    -734,   145,    95,  -734,   107,  8333,  -734,   137,  2635,   402,
     491,  -734,  8115,  8115,  -734,  -734,  4666,  8438,  8543,  8648,
    5126,    43,    69,  -734,  -734,   163,  -734,  -734,  -734,  -734,
    -734,  -734,  -734,  -734,   149,  -734,   188,   243,    66,  -734,
    -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,
    -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,
    -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,
    -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,
    -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,
    -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,
    -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,
    -734,  -734,  -734,  -734,  -734,  -734,  -734,   192,  -734,  -734,
    -734,  -734,   217,  8006,   332,  4178,  8006,  8006,  8006,  -734,
     282,  2635,   313,  -734,  -734,   277,   506,    40,   337,    81,
     323,  -734,  -734,  -734,  4557,  -734,  6807,  6807,  -734,  -734,
    4775,  -734,  8115,    79,  -734,   339,   363,  4313,  -734,  -734,
    -734,   370,   389,  -734,   519,    66,   449,   307,  6698,  4043,
     392,   297,   784,    10,   434,  -734,   322,   401,   231,   269,
    -734,   313,   424,   269,  -734,    10,   497,   487,  8753,   427,
    -734,   556,   564,   567,  -734,  -734,  -734,  -734,  -734,  -734,
      71,  -734,   343,   391,   510,   455,   399,   461,    42,   505,
     509,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  4884,  8115,
    8115,  8115,  8115,  6698,  8115,  8115,  -734,  -734,  7461,  -734,
    4043,  6484,   452,  7461,  8006,  8006,  8006,  8006,  8006,  8006,
    8006,  8006,  8006,  8006,  8006,  8006,  8006,  8006,  8006,  8006,
    8006,  8006,  8006,  8006,  8006,  8006,  8006,  8006,  8006,  8006,
    9020,  6807,  9093,  3311,   322,    74,    74,  8115,  8115,   297,
     569,   453,   543,  -734,  -734,   573,   575,    53,    61,   330,
     384,  8115,   262,  -734,   136,   596,  -734,  -734,  -734,    59,
     195,   236,   253,   326,   328,   350,   382,   385,  -734,  -734,
    -734,   408,  9969,  9969,  -734,  -734,  -734,  -734,  8224,  8224,
    -734,   508,  -734,  -734,  -734,   321,  -734,  -734,  8006,  8006,
    6916,  -734,  -734,  9166,  6807,  9239,  8006,  8006,  7134,  -734,
      10,   474,  -734,  -734,    10,  -734,  -734,   102,  -734,  -734,
    -734,  -734,  -734,  6128,  -734,  8006,  3690,   481,  9166,  9239,
    8006,   784,    10,  -734,  -734,  4993,   488,    10,  -734,  7243,
    -734,  -734,  7352,  -734,  -734,  -734,   339,   609,  -734,  -734,
    -734,   495,  8753,  9312,  6807,  9385,   741,  -734,  -734,  -734,
    -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,   259,
    -734,  -734,   494,  -734,  8006,  8006,  -734,  -734,  -734,  -734,
    -734,  -734,  -734,  -734,  -734,    49,  8006,  -734,   503,   520,
    -734,    10,  8753,   523,  -734,  -734,  1172,  -734,  -734,   449,
    3153,  3153,  3153,  3153,   595,   595,  9989,  2897,  3153,  3153,
    2766,  2766,   553,   553,  1774,   595,   595,   842,   842,  1023,
     264,   264,   449,   449,   449,  3045,  5618,  3176,  5740,  -734,
     389,  -734,    10,   379,  -734,   536,  -734,  -734,  2783,   607,
     619,  -734,  3446,   623,  3825,    44,    44,   569,  7570,   607,
      75,  9458,  6807,  9531,  -734,   322,  -734,   609,  -734,   297,
    -734,  -734,  -734,  9604,  6807,  9677,  3311,  8115,  1062,  -734,
    -734,  -734,  -734,  -734,  1516,  1516,    49,    49,  -734,  2032,
    -734,  2635,  -734,  -734,  6128,  3023,  -734,  8006,   313,  -734,
     323,  5496,  2357,    10,   352,   377,  -734,  -734,  -734,  -734,
    -734,  -734,  8115,  2635,   530,  -734,   389,   389,  2635,    27,
     784,  -734,   269,  8753,   495,   464,    23,    10,    80,   303,
     571,  -734,  -734,  -734,  -734,   809,  -734,  -734,  -734,  -734,
     759,    64,  -734,  -734,  -734,  -734,   546,  -734,   552,   631,
     557,   639,  -734,  -734,   745,  -734,  -734,  -734,   449,   449,
    -734,   666,  -734,  -734,  -734,   576,  7679,  -734,   495,  8753,
    8006,   597,  8224,  8224,  -734,   508,   585,   657,  8224,  8224,
    -734,  -734,   508,  -734,  -734,  -734,  7788,   692,  -734,   583,
    -734,   692,  -734,  -734,  -734,  -734,   607,    50,  -734,    29,
      98,    10,    84,   101,  8115,   297,  -734,  8115,  3311,   464,
      23,  -734,    10,   607,   102,   759,  3311,   297,  6257,  -734,
    -734,  -734,  -734,  -734,  -734,  8006,    49,  -734,  -734,  -734,
    8006,  8006,   432,  8006,  8006,   102,  -734,  -734,  -734,   222,
    8006,  -734,   809,   315,  -734,   587,    10,  -734,   592,  4448,
    4313,  -734,   759,  -734,  -734,   759,  -734,  -734,   426,  -734,
    -734,  4313,  -734,  -734,  -734,  -734,  -734,   633,   830,   592,
     614,   620,  -734,   626,   628,  -734,  -734,  4448,  8006,   630,
     495,  2635,  8006,  -734,  2635,  -734,  2635,  -734,  -734,  8224,
    -734,  2635,  -734,  2635,  -734,   503,  -734,   665,  -734,  3934,
     755,  -734,  8115,   607,  -734,   607,  4448,  4448,  -734,  7897,
    3581,   105,    44,  -734,   297,   607,  -734,  -734,  -734,    10,
     607,  -734,  -734,  4448,  4313,  2635,  -734,  8006,  -734,  -734,
      10,   895,   637,  1013,  -734,    10,   744,   641,  -734,   640,
     642,  -734,   643,  -734,   644,   643,   652,  8858,  -734,   650,
    -734,  -734,   655,  -734,  1306,  -734,  1306,  -734,   426,  -734,
     773,   660,  2635,  -734,  2635,  8963,    74,  -734,  -734,  4448,
    -734,  -734,    74,  -734,  -734,   607,   607,  -734,   555,  -734,
    3311,  -734,  -734,  -734,  -734,  1062,   781,   667,  4313,  -734,
     673,   895,   420,  -734,  -734,  -734,  -734,   759,  -734,   426,
    -734,   426,  -734,   426,  -734,  -734,  -734,   748,   612,   830,
    -734,   677,   680,   643,  -734,   682,   643,  -734,   763,  -734,
     648,   564,   567,  3311,  -734,  3446,  -734,  -734,  -734,  -734,
    -734,  4448,   607,  3311,  -734,  -734,   694,   895,   673,   895,
     701,   643,   702,   643,   643,  -734,  9750,  -734,  1306,  -734,
     426,  -734,  -734,   426,  -734,  -734,   609,  9823,  6807,  9896,
     619,   583,   607,  -734,   607,  -734,   673,   895,  -734,   426,
    -734,  -734,  -734,   703,   707,   643,   711,   643,   643,   151,
      23,    10,   108,   118,  -734,  -734,  -734,  -734,   673,   643,
    -734,   426,  -734,  -734,  -734,   152,  -734,   643,  -734
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -734,  -734,  -734,   417,  -734,    35,  -734,  -526,   306,  -734,
      38,  -734,    52,   -25,   -21,  -734,  -170,  -734,  -734,   -15,
     756,  -140,    -1,   -65,  -734,  -399,   -32,  1178,  -306,   753,
     -52,  -734,   -20,  -734,  -734,     7,  -734,   683,  -734,   734,
    -734,    46,    48,  -322,    90,     4,  -734,   -89,  -194,   -44,
    -292,    26,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,
    -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,     2,  -734,
    -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -734,  -357,
    -323,  -507,   -96,  -601,  -734,  -733,  -713,   124,  -734,  -485,
    -734,  -486,  -734,   -40,  -734,  -734,  -734,  -734,  -734,  -734,
    -734,  -734,  -734,  -734,   758,  -734,  -734,  -734,  -734,  -513,
    -734,   -88,  -734,  -734,  -734,  -734,  -734,  -734,   764,  -734,
    -734,  -734,  -734,  -734,  -734,  -734,  -734,   801,  -734,  -207,
    -734,  -734,  -734,  -734,     6,  -734,     9,  -734,   966,   778,
     899,  1126,  -734,  -734,    12,  -387,  -695,  -496,  -656,   398,
    -703,  -732,    54,   140,  -734,  -566,  -734,  -434,   742,  -734,
    -734,  -734,   -24,  -287,  1743,  -299,  -734,  -734,   -41,    -4,
     624,  -563,  -193,   -29,   -18,    -2
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -570
static const yytype_int16 yytable[] =
{
     109,   199,   199,   269,    80,   199,    80,   248,   224,   522,
     302,   488,   632,   534,   190,   188,   399,   191,   206,   206,
     712,   259,   206,   222,   676,   225,   219,   249,   253,   675,
     693,   520,   823,   528,   190,   188,   287,   191,   369,   110,
      67,   281,    67,   604,   719,   340,   548,   288,   244,   250,
     254,    80,   206,   831,  -100,   261,   433,   287,   336,   622,
     435,   339,   188,   531,   206,   453,   780,   456,   218,   460,
     785,   672,   583,   564,   565,   754,   818,   261,   -97,   260,
     873,   703,   704,   648,   206,   206,  -104,   531,   206,   345,
     355,   355,   374,   729,   520,   374,   528,   188,   870,   422,
    -103,   260,   246,   462,   894,   -91,   203,   212,  -280,   -99,
     213,   586,   381,   260,   260,   260,   279,   280,   791,   727,
     597,   -99,   607,  -102,   632,   640,  -101,  -486,   372,   796,
     -98,  -101,   447,   410,   334,   334,   -70,   549,   334,   294,
     295,     3,   411,   412,  -479,   586,   597,   221,   463,   -97,
     227,  -280,  -280,   237,   380,  -548,   275,   -84,   918,   -92,
     481,  -547,   795,   641,  -100,   -98,   820,   277,   397,   824,
     800,   730,   373,   296,   823,   448,   449,   450,   818,   284,
     789,   285,   678,   -89,   873,   284,   632,   285,  -479,   -91,
    -104,   -96,   375,   440,   831,   375,   894,    80,   217,   199,
     424,   199,   199,   392,   298,   -95,   946,   755,   392,   284,
     -91,   285,   287,   -91,   -91,   406,   206,   -91,   206,   206,
     681,   806,   206,   425,   206,   818,   727,   812,   -94,    80,
    -548,   -93,   692,   728,   968,   -90,  -486,   299,   244,   221,
      80,    80,   815,   -99,   597,   -99,   303,   691,  -408,   550,
     760,   436,   636,  -101,   479,  -101,   597,   908,   -97,   479,
     261,   632,    56,   832,   468,   469,   470,   471,   892,   483,
     895,   284,   407,   285,   409,   378,   421,   477,   218,   -89,
    -478,   380,   485,  -547,   260,   859,  -100,   -98,  -100,   -98,
      80,   206,   206,   206,   206,    80,   206,   206,   537,   244,
     206,   579,    80,   261,   400,   206,   403,  -408,   799,   579,
     287,   922,   535,   536,   546,   727,   199,   727,   547,   788,
     447,  -480,   383,   467,  -478,   552,    67,   260,   -93,   406,
     379,   472,   304,   206,   912,    80,   797,   518,  -481,   206,
     206,   334,   334,   334,   334,   909,   473,   474,  -408,   220,
    -408,  -408,  -104,   206,   790,   240,  -544,   568,   570,   294,
     295,   429,   956,   448,   449,  -480,   580,   721,   430,   611,
     612,   613,   614,   -96,   776,   727,   920,   440,   527,   199,
     206,   206,  -481,   327,   328,   329,   709,   944,   385,   334,
     334,   388,   406,   591,   420,   635,   206,   220,   975,   432,
     518,   389,   221,   545,   447,   602,   430,   390,   420,   717,
    -545,  -483,   -93,  -482,   109,  -473,  -103,   440,    80,   190,
     188,   727,   191,   727,   396,   518,   853,    80,   854,   199,
    -103,   632,   284,   -93,   285,  -484,   -93,   -95,   862,   713,
     -93,   527,   406,   864,   261,   811,   206,   448,   449,   452,
     518,   727,   447,   398,    67,  -483,  -544,  -482,  -473,  -473,
     447,   -99,  -544,   600,   714,   527,   658,  -473,   260,  -476,
    -476,   414,   721,   644,   611,   612,   613,   614,   721,  -484,
     611,   612,   613,   614,   261,   767,  -101,  -325,   -99,   416,
    -551,   527,   774,   559,   677,   448,   449,   454,   906,   907,
     651,   240,   214,   448,   449,   458,   674,   674,   260,   -91,
    -545,  -473,  -476,  -476,  -476,   615,  -545,   304,   656,   807,
     686,   217,   617,   903,   655,   423,   663,   199,   668,   905,
    -325,  -325,   662,   417,    80,   428,    80,   560,   718,   199,
     406,   -98,   618,   -69,   206,   240,   438,   439,   518,  -551,
     919,   434,   406,   437,   707,   943,   206,   442,    80,   206,
     518,   457,   663,   663,   644,   644,   654,   461,   700,   702,
     779,   447,   538,   464,   660,   455,   330,   465,   392,   656,
     190,   188,   484,   191,   440,   966,   538,   967,   466,   527,
    -551,   393,  -551,  -551,   206,   663,  -547,   782,   779,   540,
     544,   527,   572,   656,   418,   261,   240,   588,   595,   694,
     660,   660,   699,   701,   448,   449,   961,   664,   -84,   331,
     332,   304,   479,   659,   734,   603,   734,   666,   734,   260,
     637,   763,   765,  -259,   394,   395,   670,   770,   772,   756,
     597,   443,   654,   660,   715,  -101,   802,   394,   419,  -485,
     646,   261,  -273,   649,   206,   206,   793,   716,   541,   792,
     206,   206,   794,   304,   778,   781,   -93,   781,   801,   781,
     325,   326,   327,   328,   329,   260,   732,   656,   317,   318,
     720,   553,   735,   737,   444,   445,   206,   738,   656,   206,
      80,   740,  -485,  -485,  -281,  -273,  -273,   926,    80,   201,
     201,   542,   543,   201,   644,   188,  -260,   779,   762,   322,
     323,   324,   325,   326,   327,   328,   329,   813,   747,   768,
     611,   612,   613,   614,   554,   555,   756,   748,   833,   233,
     236,    80,    80,   937,   201,   201,   334,  -281,  -281,   334,
     554,   555,   827,    80,   769,   282,   283,   756,   843,   734,
     834,   734,   734,   845,   875,   615,   836,   852,   838,    80,
    -261,   616,   617,   674,   849,   861,   -98,   871,   876,   890,
     877,   206,   879,   881,   883,   847,   938,   939,   851,   886,
     889,    80,   618,   897,   206,   619,   251,   -90,    80,    80,
    -262,   914,    80,   610,   915,   611,   612,   613,   614,   742,
     743,   748,   744,   917,   925,    80,    80,   928,    44,    45,
     930,   610,   933,   611,   612,   613,   614,   734,   734,   936,
     734,   945,   734,   734,   289,   290,   291,   292,   293,   888,
     615,   947,   949,   594,   334,  -547,   616,   617,   669,  -548,
     671,   971,   358,   343,   408,   965,   810,   900,   615,   927,
     376,    80,   964,   260,   616,   617,   781,   618,   273,   377,
     619,   721,    80,   611,   612,   613,   614,   426,   370,     0,
      80,   260,   819,   620,     0,   618,   201,   913,   619,   201,
     201,   282,   828,     0,   611,   612,   613,   614,   891,   734,
     734,   734,     0,   734,   734,     0,     0,   201,   722,   201,
     201,   106,     0,   106,   723,    80,     0,    80,     0,     0,
     304,     0,     0,    80,     0,    80,     0,     0,     0,   734,
     734,   734,   734,   199,     0,   317,   318,   384,     0,     0,
     386,   387,     0,   518,     0,     0,   406,     0,   668,   781,
     206,     0,     0,     0,   518,     0,     0,   721,   106,   611,
     612,   613,   614,   734,   734,   734,   734,   656,   324,   325,
     326,   327,   328,   329,     0,     0,     0,   734,   104,     0,
     104,     0,     0,     0,     0,   734,     0,     0,     0,     0,
       0,   201,     0,     0,   722,   527,   486,   489,   490,   491,
     492,   493,   494,   495,   496,   497,   498,   499,   500,   501,
     502,   503,   504,   505,   506,   507,   508,   509,   510,   511,
     512,   513,   514,     0,   201,   104,     0,     0,     0,   262,
       0,     0,     0,     0,   587,     0,     0,     0,   589,     0,
       0,   590,   478,     0,     0,   816,   817,   487,     0,   751,
       0,   262,     0,     0,     0,     0,   599,   826,     0,     0,
       0,   601,     0,   347,   356,   356,   356,     0,     0,     0,
       0,   569,   571,   840,     0,   721,     0,   611,   612,   613,
     614,   575,   201,   201,     0,     0,     0,   201,     0,   569,
     571,   201,     0,     0,     0,   848,     0,     0,     0,     0,
       0,   304,   855,   856,   106,     0,   858,     0,   593,     0,
       0,     0,   722,   598,     0,   647,   317,   318,   872,   866,
     867,     0,   201,     0,   610,   201,   611,   612,   613,   614,
       0,     0,     0,   576,   578,     0,   106,   201,   107,     0,
     107,     0,   251,   822,     0,     0,   825,   106,   106,     0,
     325,   326,   327,   328,   329,     0,     0,   638,   639,     0,
       0,   615,     0,     0,     0,   904,     0,   616,   617,   201,
       0,   104,     0,   578,     0,     0,   251,     0,     0,     0,
       0,     0,     0,     0,   916,   107,     0,     0,   618,   263,
      77,   619,    77,     0,     0,     0,     0,   106,     0,     0,
       0,     0,   106,   104,   695,     0,     0,     0,     0,   106,
       0,   263,     0,     0,   104,   104,     0,     0,     0,   940,
     645,   941,     0,   348,   357,   357,   650,   942,     0,     0,
       0,   201,     0,     0,   262,   201,     0,    77,     0,     0,
       0,     0,   106,     0,   893,     0,   896,   201,     0,     0,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,     0,   104,   317,   318,     0,     0,   104,
     201,     0,     0,     0,     0,   344,   104,   262,     0,     0,
       0,     0,   487,     0,     0,   921,     0,   923,     0,     0,
       0,   924,   319,     0,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,     0,     0,     0,     0,     0,   104,
       0,     0,  -238,     0,     0,     0,     0,     0,     0,     0,
       0,   708,     0,     0,     0,   106,     0,     0,   798,     0,
       0,   107,     0,     0,   106,     0,   955,     0,   957,   201,
       0,   958,     0,   761,     0,   764,   766,     0,     0,   808,
       0,   771,   773,     0,     0,     0,     0,   969,     0,   201,
     814,     0,     0,   107,     0,     0,     0,     0,   747,     0,
     611,   612,   613,   614,   107,   107,     0,     0,     0,   977,
     736,     0,   739,    77,     0,     0,     0,     0,     0,     0,
     759,     0,   104,     0,   263,     0,     0,     0,   805,     0,
       0,   104,     0,   764,   766,   615,   771,   773,     0,     0,
     775,   616,   617,   201,     0,    77,     0,     0,   262,     0,
       0,     0,     0,     0,   107,     0,    77,    77,     0,   107,
       0,     0,   618,   863,     0,   619,   107,   263,     0,     0,
       0,   106,     0,   106,   869,     0,     0,     0,     0,   874,
       0,   201,     0,     0,     0,   842,     0,     0,   262,     0,
       0,     0,   844,     0,   809,   106,     0,     0,     0,   107,
       0,     0,     0,     0,     0,     0,    77,     0,     0,     0,
       0,    77,   201,     0,     0,     0,     0,     0,    77,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     844,     0,   841,   835,     0,   837,   839,     0,   104,     0,
     104,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    77,     0,     0,     0,     0,  -569,     0,     0,     0,
       0,     0,   104,   857,  -569,  -569,  -569,     0,     0,  -569,
    -569,  -569,     0,  -569,     0,     0,     0,     0,     0,     0,
       0,     0,   107,  -569,     0,     0,     0,     0,     0,     0,
       0,   107,     0,  -569,  -569,     0,  -569,  -569,  -569,  -569,
    -569,   878,   880,     0,   882,     0,   884,   885,   263,   262,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   106,     0,     0,
       0,     0,     0,     0,    77,   106,     0,     0,     0,     0,
       0,  -569,     0,    77,     0,     0,     0,     0,   263,     0,
       0,     0,     0,     0,     0,   262,     0,     0,     0,     0,
       0,   201,     0,     0,     0,     0,     0,     0,   106,   106,
       0,     0,     0,   929,   931,   932,     0,   934,   935,     0,
     106,     0,     0,  -569,     0,  -569,     0,     0,   217,  -569,
       0,  -569,     0,  -569,   104,     0,   106,     0,   107,     0,
     107,     0,   104,   948,   950,   951,   952,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   106,     0,
       0,     0,   107,     0,     0,   106,   106,     0,     0,   106,
       0,     0,     0,     0,     0,   104,   104,   970,   972,   973,
     974,     0,   106,   106,     0,     0,     0,   104,     0,     0,
      77,   976,    77,     0,     0,     0,     0,     0,     0,   978,
       0,     0,     0,   104,     0,     0,     0,     0,     0,   263,
       0,     0,     0,     0,    77,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   104,     0,     0,   106,     0,
       0,     0,   104,   104,     0,     0,   104,     0,     0,   106,
     746,     0,     0,     0,     0,     0,     0,   106,     0,   104,
     104,     0,     0,     0,     0,   263,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   235,
     235,     0,     0,   356,   235,   235,   235,     0,     0,     0,
       0,     0,   106,     0,   106,     0,   235,     0,     0,     0,
     106,   901,   106,     0,   107,   104,     0,     0,   235,     0,
       0,     0,   107,     0,     0,     0,   104,     0,     0,     0,
     235,   235,   235,     0,   104,     0,     0,     0,     0,     0,
       0,     0,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   107,   107,   317,   318,     0,
       0,     0,     0,     0,     0,     0,    77,   107,     0,   104,
       0,   104,     0,     0,    77,     0,     0,   104,     0,   104,
       0,     0,     0,   107,   319,     0,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   107,     0,    77,    77,     0,
       0,   221,   107,   107,     0,     0,   107,     0,     0,    77,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   107,
     107,     0,     0,     0,     0,    77,   235,     0,     0,   235,
     235,   235,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   357,     0,     0,     0,    77,     0,     0,
       0,     0,     0,     0,    77,    77,     0,     0,    77,     0,
       0,   902,     0,     0,     0,   107,     0,     0,     0,     0,
       0,    77,    77,     0,     0,     0,   107,     0,     0,     0,
       0,     0,     0,     0,   107,     0,     0,     0,     0,     0,
       0,   235,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   899,     0,     0,     0,    77,     0,   107,
       0,   107,     0,     0,     0,     0,     0,   107,    77,   107,
       0,     0,     0,     0,   235,     0,    77,   235,   235,   235,
     235,   235,   235,   235,   235,   235,   235,   235,   235,   235,
     235,   235,   235,   235,   235,   235,   235,   235,   235,   235,
     235,   235,   235,     0,     0,     0,   705,     0,     0,     0,
       0,    77,     0,    77,     0,     0,     0,     0,     0,    77,
       0,    77,     0,     0,     0,     0,     0,     0,     0,     0,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,     0,     0,   317,   318,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   235,   235,   235,     0,     0,     0,     0,     0,   235,
     235,   235,   319,     0,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,     0,     0,     0,     0,   235,     0,
       0,     0,     0,   235,     0,     0,     0,     0,     0,     0,
       0,     0,   235,     0,     0,   235,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   235,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   235,   235,     0,
    -569,     4,     0,     5,     6,     7,     8,     9,     0,   235,
       0,    10,    11,     0,     0,   235,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,     0,
      20,    21,    22,    23,    24,    25,    26,     0,     0,    27,
       0,     0,     0,     0,     0,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,     0,    41,
      42,     0,    43,    44,    45,     0,    46,    47,     0,     0,
       0,   235,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,     0,     0,    49,
      50,     0,    51,    52,     0,    53,     0,    54,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     0,     0,
     235,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    61,    62,    63,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -569,   235,  -569,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -280,     0,     0,
       0,     0,     0,     0,     0,  -280,  -280,  -280,     0,     0,
    -280,  -280,  -280,     0,  -280,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -280,  -280,  -280,     0,     0,   235,
       0,     0,   235,   235,  -280,  -280,     0,  -280,  -280,  -280,
    -280,  -280,     0,     0,     0,     0,     0,     0,     0,   235,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -280,  -280,  -280,  -280,  -280,
    -280,  -280,  -280,  -280,  -280,  -280,  -280,  -280,     0,     0,
    -280,  -280,  -280,     0,   711,  -280,     0,     0,   235,     0,
       0,  -280,     0,   235,   235,     0,   235,   235,     0,     0,
       0,     0,     0,   235,  -280,     0,  -102,  -280,  -280,  -280,
    -280,  -280,  -280,  -280,  -280,  -280,  -280,  -280,  -280,     0,
       0,     0,     0,     0,  -280,  -280,  -280,  -280,     0,     0,
    -280,  -280,  -280,     0,  -280,     0,     0,     0,     0,     0,
       0,   235,     0,     0,     0,   235,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -407,     0,     0,     0,     0,     0,     0,     0,  -407,
    -407,  -407,   235,     0,  -407,  -407,  -407,     0,  -407,     0,
       0,     0,     0,     0,     0,     0,     0,  -407,  -407,  -407,
     235,     0,     0,     0,     0,     0,     0,     0,  -407,  -407,
       0,  -407,  -407,  -407,  -407,  -407,     0,     0,     0,     0,
     235,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   235,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,     0,     0,  -407,  -407,  -407,     0,     0,  -407,
       0,     0,     0,     0,     0,  -407,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -407,     0,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,     0,     0,     0,     0,  -407,  -407,  -407,
    -407,  -407,  -274,   217,  -407,  -407,  -407,     0,  -407,     0,
    -274,  -274,  -274,     0,     0,  -274,  -274,  -274,     0,  -274,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -274,
    -274,  -274,     0,     0,     0,     0,     0,     0,     0,  -274,
    -274,     0,  -274,  -274,  -274,  -274,  -274,     0,     0,     0,
       0,     0,     0,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,     0,     0,   317,   318,
    -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,
    -274,  -274,  -274,     0,     0,  -274,  -274,  -274,     0,     0,
    -274,     0,     0,     0,     0,   319,  -274,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,     0,     0,  -274,
       0,     0,  -274,  -274,  -274,  -274,  -274,  -274,  -274,  -274,
    -274,  -274,  -274,  -274,     0,     0,     0,     0,     0,  -274,
    -274,  -274,  -274,  -569,     0,  -274,  -274,  -274,     0,  -274,
       0,  -569,  -569,  -569,     0,     0,  -569,  -569,  -569,     0,
    -569,     0,     0,     0,     0,     0,     0,     0,     0,  -569,
    -569,  -569,     0,     0,     0,     0,     0,     0,     0,     0,
    -569,  -569,     0,  -569,  -569,  -569,  -569,  -569,     0,     0,
       0,     0,     0,     0,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,  -570,  -570,     0,     0,   317,
     318,  -569,  -569,  -569,  -569,  -569,  -569,  -569,  -569,  -569,
    -569,  -569,  -569,  -569,     0,     0,  -569,  -569,  -569,     0,
       0,  -569,     0,     0,     0,     0,     0,  -569,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,     0,     0,
       0,     0,     0,  -569,     0,  -569,  -569,  -569,  -569,  -569,
    -569,  -569,  -569,  -569,  -569,     0,     0,     0,     0,  -569,
    -569,  -569,  -569,  -569,  -287,   217,  -569,  -569,  -569,     0,
    -569,     0,  -287,  -287,  -287,     0,     0,  -287,  -287,  -287,
       0,  -287,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -287,  -287,     0,     0,     0,     0,     0,     0,     0,
       0,  -287,  -287,     0,  -287,  -287,  -287,  -287,  -287,     0,
       0,     0,     0,     0,     0,   304,   305,   306,   307,   308,
     309,   310,   311,     0,   313,   314,     0,     0,     0,     0,
     317,   318,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,
    -287,  -287,  -287,  -287,  -287,     0,     0,  -287,  -287,  -287,
       0,     0,  -287,     0,     0,     0,     0,     0,  -287,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,     0,
       0,     0,     0,     0,  -287,     0,  -287,  -287,  -287,  -287,
    -287,  -287,  -287,  -287,  -287,  -287,     0,     0,     0,     0,
       0,  -287,  -287,  -287,  -287,  -551,   214,  -287,  -287,  -287,
       0,  -287,     0,  -551,  -551,  -551,     0,     0,     0,  -551,
    -551,     0,  -551,     0,     0,     0,     0,   650,     0,     0,
       0,  -551,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -551,  -551,     0,  -551,  -551,  -551,  -551,  -551,
       0,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,     0,     0,   317,   318,     0,     0,
       0,     0,     0,  -551,  -551,  -551,  -551,  -551,  -551,  -551,
    -551,  -551,  -551,  -551,  -551,  -551,     0,     0,  -551,  -551,
    -551,     0,   652,   319,     0,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -100,  -551,     0,  -551,  -551,  -551,
    -551,  -551,  -551,  -551,  -551,  -551,  -551,     0,     0,     0,
       0,  -551,  -551,  -551,  -551,   -92,  -280,     0,  -551,     0,
    -551,     0,  -551,     0,  -280,  -280,  -280,     0,     0,     0,
    -280,  -280,     0,  -280,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -280,  -280,     0,  -280,  -280,  -280,  -280,
    -280,   304,  -570,  -570,  -570,  -570,   309,   310,     0,     0,
    -570,  -570,     0,     0,     0,     0,   317,   318,     0,     0,
       0,     0,     0,     0,  -280,  -280,  -280,  -280,  -280,  -280,
    -280,  -280,  -280,  -280,  -280,  -280,  -280,     0,     0,  -280,
    -280,  -280,     0,   653,     0,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -102,  -280,     0,  -280,  -280,
    -280,  -280,  -280,  -280,  -280,  -280,  -280,  -280,     0,     0,
       0,     0,     0,  -280,  -280,  -280,   -94,     0,     0,  -280,
       0,  -280,   238,  -280,     5,     6,     7,     8,     9,  -569,
    -569,  -569,    10,    11,     0,     0,  -569,    12,     0,    13,
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
       0,     0,     0,     0,     0,     0,  -569,   238,  -569,     5,
       6,     7,     8,     9,     0,     0,  -569,    10,    11,     0,
    -569,  -569,    12,     0,    13,    14,    15,    16,    17,    18,
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
      63,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -569,   238,  -569,     5,     6,     7,     8,     9,     0,
       0,  -569,    10,    11,     0,     0,  -569,    12,  -569,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,    29,     0,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
      41,    42,     0,    43,    44,    45,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    48,     0,     0,
      49,    50,     0,    51,    52,     0,    53,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     0,
       0,     4,     0,     5,     6,     7,     8,     9,     0,     0,
       0,    10,    11,    61,    62,    63,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,  -569,     0,  -569,     0,
      20,    21,    22,    23,    24,    25,    26,     0,     0,    27,
       0,     0,     0,     0,     0,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,     0,    41,
      42,     0,    43,    44,    45,     0,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,     0,     0,    49,
      50,     0,    51,    52,     0,    53,     0,    54,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    61,    62,    63,     0,     0,  -569,     0,     0,
       0,     0,     0,     0,     0,  -569,   238,  -569,     5,     6,
       7,     8,     9,     0,     0,  -569,    10,    11,     0,     0,
    -569,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
      28,    29,     0,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,     0,    41,    42,     0,    43,    44,    45,
       0,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    48,     0,     0,    49,    50,     0,    51,    52,     0,
      53,     0,    54,    55,    56,    57,    58,    59,    60,     0,
       0,     0,     0,     0,     0,   238,     0,     5,     6,     7,
       8,     9,     0,  -569,  -569,    10,    11,    61,    62,    63,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
    -569,     0,  -569,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,    28,
      29,     0,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,   238,     0,     5,     6,     7,     8,
       9,     0,     0,     0,    10,    11,    61,    62,    63,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,  -569,
       0,  -569,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
       0,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,     0,    41,    42,     0,    43,    44,    45,     0,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,   239,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    61,    62,    63,     0,     0,
       0,     0,     0,     0,     0,     0,  -569,     0,  -569,   238,
    -569,     5,     6,     7,     8,     9,     0,     0,     0,    10,
      11,     0,     0,     0,    12,     0,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,    28,    29,     0,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
      43,    44,    45,     0,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,     0,    49,    50,     0,
      51,    52,     0,    53,     0,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      61,    62,    63,     0,     0,     0,     0,     0,     0,     0,
       0,  -569,     0,  -569,   238,  -569,     5,     6,     7,     8,
       9,     0,     0,     0,    10,    11,     0,     0,     0,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
       0,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,     0,    41,    42,     0,    43,    44,    45,     0,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,    49,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    61,    62,    63,     0,     0,
    -569,     0,     0,     0,     0,     0,     0,     0,  -569,   238,
    -569,     5,     6,     7,     8,     9,     0,     0,  -569,    10,
      11,     0,     0,     0,    12,     0,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,    28,    29,     0,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
      43,    44,    45,     0,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,     0,    49,    50,     0,
      51,    52,     0,    53,     0,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      61,    62,    63,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,  -569,     0,  -569,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,   192,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,   193,    41,    42,     0,    43,
      44,    45,     0,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   194,     0,     0,   195,    50,     0,    51,
      52,     0,   196,   197,    54,    55,    56,    57,    58,    59,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    61,
     198,    63,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,   221,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,     0,    41,    42,     0,    43,    44,
      45,     0,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   194,     0,     0,   195,    50,     0,    51,    52,
       0,     0,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    61,    62,
      63,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,   284,     0,   285,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,     0,    41,    42,     0,    43,    44,    45,
       0,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   194,     0,     0,   195,    50,     0,    51,    52,     0,
       0,     0,    54,    55,    56,    57,    58,    59,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       8,     9,     0,     0,     0,    10,    11,    61,    62,    63,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,   221,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     8,
       9,     0,     0,     0,    10,    11,    61,    62,    63,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,   466,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
       0,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,     0,    41,    42,     0,    43,    44,    45,     0,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,    49,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    61,    62,    63,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   466,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,     0,     0,     0,   135,   136,   137,   359,
     360,   361,   362,   142,   143,   144,     0,     0,     0,     0,
       0,   145,   146,   147,   148,   363,   364,   365,   366,   153,
      37,    38,   367,    40,     0,     0,     0,     0,     0,     0,
       0,     0,   155,   156,   157,   158,   159,   160,   161,   162,
     163,     0,     0,   164,   165,     0,     0,   166,   167,   168,
     169,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   170,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,     0,   181,
     182,  -544,  -544,  -544,     0,  -544,     0,   183,   368,  -544,
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
    -544,  -544,  -544,     0,  -544,  -545,  -545,  -545,     0,  -545,
    -544,     0,     0,  -545,  -545,     0,     0,     0,  -545,     0,
    -545,  -545,  -545,  -545,  -545,  -545,  -545,     0,  -545,     0,
       0,     0,  -545,  -545,  -545,  -545,  -545,  -545,  -545,     0,
       0,  -545,     0,     0,     0,     0,     0,     0,  -545,     0,
       0,  -545,  -545,  -545,  -545,  -545,  -545,  -545,  -545,  -545,
    -545,  -545,  -545,     0,  -545,  -545,  -545,     0,  -545,  -545,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -545,     0,
       0,  -545,  -545,     0,  -545,  -545,     0,  -545,  -545,  -545,
    -545,  -545,  -545,  -545,  -545,  -545,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -545,  -545,  -545,     0,  -545,  -547,
    -547,  -547,     0,  -547,  -545,     0,     0,  -547,  -547,     0,
       0,     0,  -547,     0,  -547,  -547,  -547,  -547,  -547,  -547,
    -547,     0,     0,     0,     0,     0,  -547,  -547,  -547,  -547,
    -547,  -547,  -547,     0,     0,  -547,     0,     0,     0,     0,
       0,     0,  -547,     0,     0,  -547,  -547,  -547,  -547,  -547,
    -547,  -547,  -547,  -547,  -547,  -547,  -547,     0,  -547,  -547,
    -547,     0,  -547,  -547,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -547,   710,     0,  -547,  -547,     0,  -547,  -547,
       0,  -547,  -547,  -547,  -547,  -547,  -547,  -547,  -547,  -547,
       0,     0,     0,     0,     0,  -100,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -547,  -547,
    -547,  -549,  -549,  -549,     0,  -549,     0,     0,  -547,  -549,
    -549,     0,     0,     0,  -549,     0,  -549,  -549,  -549,  -549,
    -549,  -549,  -549,     0,     0,     0,     0,     0,  -549,  -549,
    -549,  -549,  -549,  -549,  -549,     0,     0,  -549,     0,     0,
       0,     0,     0,     0,  -549,     0,     0,  -549,  -549,  -549,
    -549,  -549,  -549,  -549,  -549,  -549,  -549,  -549,  -549,     0,
    -549,  -549,  -549,     0,  -549,  -549,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -549,     0,     0,  -549,  -549,     0,
    -549,  -549,     0,  -549,  -549,  -549,  -549,  -549,  -549,  -549,
    -549,  -549,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -549,  -549,  -549,  -550,  -550,  -550,     0,  -550,     0,     0,
    -549,  -550,  -550,     0,     0,     0,  -550,     0,  -550,  -550,
    -550,  -550,  -550,  -550,  -550,     0,     0,     0,     0,     0,
    -550,  -550,  -550,  -550,  -550,  -550,  -550,     0,     0,  -550,
       0,     0,     0,     0,     0,     0,  -550,     0,     0,  -550,
    -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,
    -550,     0,  -550,  -550,  -550,     0,  -550,  -550,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -550,     0,     0,  -550,
    -550,     0,  -550,  -550,     0,  -550,  -550,  -550,  -550,  -550,
    -550,  -550,  -550,  -550,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -550,  -550,  -550,     0,     0,     0,     0,     0,
       0,     0,  -550,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,     0,     0,     0,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
       0,     0,     0,     0,     0,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   266,   267,   154,   268,     0,     0,
       0,     0,     0,     0,     0,     0,   155,   156,   157,   158,
     159,   160,   161,   162,   163,     0,     0,   164,   165,     0,
       0,   166,   167,   168,   169,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   170,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,     0,   181,   182,     0,     0,     0,     0,     0,
       0,   183,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,     0,     0,     0,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,     0,
       0,     0,     0,     0,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   223,     0,   154,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   155,   156,   157,   158,   159,
     160,   161,   162,   163,     0,     0,   164,   165,     0,     0,
     166,   167,   168,   169,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   170,     0,     0,    55,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,     0,   181,   182,     0,     0,     0,     0,     0,     0,
     183,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,     0,     0,     0,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,     0,     0,
       0,     0,     0,   145,   146,   147,   148,   149,   150,   151,
     152,   153,     0,     0,   154,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   155,   156,   157,   158,   159,   160,
     161,   162,   163,     0,     0,   164,   165,     0,     0,   166,
     167,   168,   169,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   170,     0,     0,    55,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
       0,   181,   182,     0,     0,     0,     0,     0,     0,   183,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,     0,     0,     0,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,   145,   146,   147,   148,   149,   150,   151,   152,
     153,     0,     0,   154,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   155,   156,   157,   158,   159,   160,   161,
     162,   163,     0,     0,   164,   165,     0,     0,   166,   167,
     168,   169,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   170,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,     0,
     181,   182,     5,     6,     7,     0,     9,     0,   183,     0,
      10,    11,     0,     0,     0,    12,     0,    13,    14,    15,
     228,   229,    18,    19,     0,     0,     0,     0,     0,   230,
     231,   232,    23,    24,    25,    26,     0,     0,   192,     0,
       0,     0,     0,     0,     0,   255,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
       0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   256,     0,     0,   195,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,     0,     0,     0,
      12,   257,    13,    14,    15,   228,   229,    18,    19,   258,
       0,     0,     0,     0,   230,   231,   232,    23,    24,    25,
      26,     0,     0,   192,     0,     0,     0,     0,     0,     0,
     255,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     256,     0,     0,   195,    50,     0,    51,    52,     0,     0,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     5,     6,     7,     8,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   257,    13,    14,    15,
      16,    17,    18,    19,   482,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
       0,    43,    44,    45,     0,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    48,     0,     0,    49,    50,
       0,    51,    52,     0,    53,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     8,     9,     0,     0,     0,    10,
      11,    61,    62,    63,    12,     0,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,    28,    29,     0,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
      43,    44,    45,     0,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,     0,    49,    50,     0,
      51,    52,     0,    53,     0,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      61,    62,    63,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,   192,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,   193,    41,    42,     0,    43,
      44,    45,     0,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   194,     0,     0,   195,    50,     0,    51,
      52,     0,   196,   197,    54,    55,    56,    57,    58,    59,
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
       0,   577,   197,    54,    55,    56,    57,    58,    59,    60,
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
     196,     0,    54,    55,    56,    57,    58,    59,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    61,   198,    63,
      12,     0,    13,    14,    15,   228,   229,    18,    19,     0,
       0,     0,     0,     0,   230,   231,   232,    23,    24,    25,
      26,     0,     0,   192,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,   193,    41,    42,     0,    43,    44,    45,     0,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     194,     0,     0,   195,    50,     0,    51,    52,     0,     0,
     197,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    61,   198,    63,    12,
       0,    13,    14,    15,   228,   229,    18,    19,     0,     0,
       0,     0,     0,   230,   231,   232,    23,    24,    25,    26,
       0,     0,   192,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,   193,    41,    42,     0,    43,    44,    45,     0,    46,
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
     193,    41,    42,     0,    43,    44,    45,     0,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   194,     0,
       0,   195,    50,     0,    51,    52,     0,     0,     0,    54,
      55,    56,    57,    58,    59,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    61,   198,    63,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
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
      50,     0,    51,    52,     0,   196,     0,    54,    55,    56,
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
       0,    51,    52,     0,   758,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    61,   198,    63,    12,     0,    13,    14,    15,   228,
     229,    18,    19,     0,     0,     0,     0,     0,   230,   231,
     232,    23,    24,    25,    26,     0,     0,   192,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
      43,    44,    45,     0,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   194,     0,     0,   195,    50,     0,
      51,    52,     0,   476,     0,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      61,   198,    63,    12,     0,    13,    14,    15,   228,   229,
      18,    19,     0,     0,     0,     0,     0,   230,   231,   232,
      23,    24,    25,    26,     0,     0,   192,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,     0,    41,    42,     0,    43,
      44,    45,     0,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   194,     0,     0,   195,    50,     0,    51,
      52,     0,   577,     0,    54,    55,    56,    57,    58,    59,
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
       0,     0,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    61,   198,
      63,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,     0,    41,    42,     0,    43,    44,    45,
       0,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   194,     0,     0,   195,    50,     0,    51,    52,     0,
       0,     0,    54,    55,    56,    57,    58,    59,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    61,    62,    63,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,   192,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     194,     0,     0,   195,    50,     0,    51,    52,     0,     0,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    61,   198,    63,    12,
       0,    13,    14,    15,   228,   229,    18,    19,     0,     0,
       0,     0,     0,   230,   231,   232,    23,    24,    25,    26,
       0,     0,   192,     0,     0,     0,     0,     0,     0,   255,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,     0,    41,    42,     0,    43,    44,    45,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   256,
       0,     0,   300,    50,     0,    51,    52,     0,   301,     0,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,     0,     0,     0,    12,   257,    13,    14,    15,   228,
     229,    18,    19,     0,     0,     0,     0,     0,   230,   231,
     232,    23,    24,    25,    26,     0,     0,   192,     0,     0,
       0,     0,     0,     0,   255,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
      43,    44,    45,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   342,     0,     0,    49,    50,     0,
      51,    52,     0,    53,     0,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,     0,     0,     0,    12,
     257,    13,    14,    15,   228,   229,    18,    19,     0,     0,
       0,     0,     0,   230,   231,   232,    23,    24,    25,    26,
       0,     0,   192,     0,     0,     0,     0,     0,     0,   255,
       0,     0,    32,    33,    34,   349,    36,    37,    38,   350,
      40,     0,    41,    42,     0,    43,    44,    45,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   351,     0,     0,   352,
       0,     0,   195,    50,     0,    51,    52,     0,     0,     0,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,     0,     0,     0,    12,   257,    13,    14,    15,   228,
     229,    18,    19,     0,     0,     0,     0,     0,   230,   231,
     232,    23,    24,    25,    26,     0,     0,   192,     0,     0,
       0,     0,     0,     0,   255,     0,     0,    32,    33,    34,
     349,    36,    37,    38,   350,    40,     0,    41,    42,     0,
      43,    44,    45,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   352,     0,     0,   195,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,     0,     0,     0,    12,
     257,    13,    14,    15,   228,   229,    18,    19,     0,     0,
       0,     0,     0,   230,   231,   232,    23,    24,    25,    26,
       0,     0,   192,     0,     0,     0,     0,     0,     0,   255,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,     0,    41,    42,     0,    43,    44,    45,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   256,
       0,     0,   300,    50,     0,    51,    52,     0,     0,     0,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,     0,     0,     0,    12,   257,    13,    14,    15,   228,
     229,    18,    19,     0,     0,     0,     0,     0,   230,   231,
     232,    23,    24,    25,    26,     0,     0,   192,     0,     0,
       0,     0,     0,     0,   255,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
      43,    44,    45,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   887,     0,     0,   195,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,     0,     0,     0,    12,
     257,    13,    14,    15,   228,   229,    18,    19,     0,     0,
       0,     0,     0,   230,   231,   232,    23,    24,    25,    26,
       0,     0,   192,     0,     0,     0,     0,     0,     0,   255,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,     0,    41,    42,     0,    43,    44,    45,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   898,
       0,     0,   195,    50,     0,    51,    52,     0,     0,     0,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     0,   515,   516,     0,     0,   517,     0,     0,     0,
       0,     0,     0,     0,     0,   257,   155,   156,   157,   158,
     159,   160,   161,   162,   163,     0,     0,   164,   165,     0,
       0,   166,   167,   168,   169,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   170,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,     0,   181,   182,   523,   524,     0,     0,   525,
       0,   183,   217,     0,     0,     0,     0,     0,     0,   155,
     156,   157,   158,   159,   160,   161,   162,   163,     0,     0,
     164,   165,     0,     0,   166,   167,   168,   169,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   170,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,     0,   181,   182,   581,   516,
       0,     0,   582,     0,   183,   217,     0,     0,     0,     0,
       0,     0,   155,   156,   157,   158,   159,   160,   161,   162,
     163,     0,     0,   164,   165,     0,     0,   166,   167,   168,
     169,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   170,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,     0,   181,
     182,   584,   524,     0,     0,   585,     0,   183,   217,     0,
       0,     0,     0,     0,     0,   155,   156,   157,   158,   159,
     160,   161,   162,   163,     0,     0,   164,   165,     0,     0,
     166,   167,   168,   169,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   170,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,     0,   181,   182,   605,   516,     0,     0,   606,     0,
     183,   217,     0,     0,     0,     0,     0,     0,   155,   156,
     157,   158,   159,   160,   161,   162,   163,     0,     0,   164,
     165,     0,     0,   166,   167,   168,   169,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   170,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,     0,   181,   182,   608,   524,     0,
       0,   609,     0,   183,   217,     0,     0,     0,     0,     0,
       0,   155,   156,   157,   158,   159,   160,   161,   162,   163,
       0,     0,   164,   165,     0,     0,   166,   167,   168,   169,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     170,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,     0,   181,   182,
     679,   516,     0,     0,   680,     0,   183,   217,     0,     0,
       0,     0,     0,     0,   155,   156,   157,   158,   159,   160,
     161,   162,   163,     0,     0,   164,   165,     0,     0,   166,
     167,   168,   169,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   170,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
       0,   181,   182,   682,   524,     0,     0,   683,     0,   183,
     217,     0,     0,     0,     0,     0,     0,   155,   156,   157,
     158,   159,   160,   161,   162,   163,     0,     0,   164,   165,
       0,     0,   166,   167,   168,   169,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   170,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,     0,   181,   182,   689,   516,     0,     0,
     690,     0,   183,   217,     0,     0,     0,     0,     0,     0,
     155,   156,   157,   158,   159,   160,   161,   162,   163,     0,
       0,   164,   165,     0,     0,   166,   167,   168,   169,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   170,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,     0,   181,   182,   562,
     524,     0,     0,   563,     0,   183,   217,     0,     0,     0,
       0,     0,     0,   155,   156,   157,   158,   159,   160,   161,
     162,   163,     0,     0,   164,   165,     0,     0,   166,   167,
     168,   169,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   170,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,     0,
     181,   182,   953,   516,     0,     0,   954,     0,   183,   217,
       0,     0,     0,     0,     0,     0,   155,   156,   157,   158,
     159,   160,   161,   162,   163,     0,     0,   164,   165,     0,
       0,   166,   167,   168,   169,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   170,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,     0,   181,   182,   959,   516,     0,     0,   960,
       0,   183,   217,     0,     0,     0,     0,     0,     0,   155,
     156,   157,   158,   159,   160,   161,   162,   163,     0,     0,
     164,   165,     0,     0,   166,   167,   168,   169,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   170,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,     0,   181,   182,   962,   524,
       0,     0,   963,     0,   183,   217,     0,     0,     0,     0,
       0,     0,   155,   156,   157,   158,   159,   160,   161,   162,
     163,     0,     0,   164,   165,     0,     0,   166,   167,   168,
     169,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   170,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,     0,   181,
     182,   562,   524,     0,     0,   563,     0,   183,   217,     0,
       0,     0,     0,     0,     0,   155,   156,   157,   158,   159,
     160,   161,   162,   163,     0,     0,   164,   165,     0,     0,
     166,   167,   168,   169,     0,     0,     0,   304,   305,   306,
     307,   308,   309,   310,   170,     0,   313,   314,     0,     0,
       0,     0,   317,   318,     0,     0,     0,     0,     0,     0,
       0,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,     0,   181,   182,     0,     0,     0,     0,     0,     0,
     183,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329
};

static const yytype_int16 yycheck[] =
{
       2,    16,    17,    55,     2,    20,     4,    51,    28,   331,
      75,   303,   446,   336,     8,     8,   210,     8,    16,    17,
     583,    53,    20,    27,   537,    29,    22,    51,    52,   536,
     556,   330,   735,   332,    28,    28,    65,    28,    90,     4,
       2,    62,     4,   442,   607,    86,   352,    65,    49,    51,
      52,    49,    50,   748,    25,    53,   249,    86,    83,   446,
     253,    86,    55,    13,    62,   272,   667,   274,    22,   276,
     671,    27,   394,   372,   373,   641,   732,    75,    25,    53,
     813,   566,   567,   482,    82,    83,    25,    13,    86,    87,
      88,    89,    26,    29,   393,    26,   395,    90,   811,   239,
      25,    75,    50,    61,   836,    25,    16,    17,    85,    25,
      20,   398,   108,    87,    88,    89,    59,    60,   681,   615,
     419,    13,   444,    25,   558,    76,    25,    87,    85,   692,
      25,    13,    61,    54,    82,    83,   109,     1,    86,    37,
      38,     0,    63,    64,    85,   432,   445,   137,   106,   109,
     126,   128,   129,    56,   108,   132,   136,   130,   871,   130,
     300,   132,   688,   114,    13,    13,   732,   136,    87,   735,
     696,   107,   129,    28,   877,   104,   105,   106,   834,   135,
     130,   137,   539,   130,   917,   135,   620,   137,   129,   109,
     109,   130,   126,   258,   889,   126,   928,   195,   132,   214,
     241,   216,   217,   205,   109,   130,   919,   641,   210,   135,
     130,   137,   241,   133,   130,   217,   214,   137,   216,   217,
     542,   706,   220,   241,   222,   881,   722,   723,   130,   227,
     132,   130,   554,   620,   947,   130,    87,   130,   239,   137,
     238,   239,   728,   135,   543,   137,   109,   553,    26,   113,
     649,   255,   459,   135,   298,   137,   555,   858,   109,   303,
     258,   695,    99,   749,   289,   290,   291,   292,   834,   301,
     836,   135,   220,   137,   222,    87,   238,   298,   232,   130,
      85,   235,   303,   132,   258,   792,   135,   135,   137,   137,
     288,   289,   290,   291,   292,   293,   294,   295,   339,   300,
     298,   390,   300,   301,   214,   303,   216,    85,   695,   398,
     339,   877,   337,   338,    52,   811,   331,   813,    56,   676,
      61,    85,   130,   288,   129,   354,   288,   301,    25,   331,
      87,   293,    68,   331,   860,   333,   693,   330,    85,   337,
     338,   289,   290,   291,   292,   858,   294,   295,   126,   132,
     128,   129,   109,   351,   677,    49,    26,   378,   379,    37,
      38,   130,   928,   104,   105,   129,   390,    52,   137,    54,
      55,    56,    57,   130,   666,   871,   872,   442,   332,   394,
     378,   379,   129,   119,   120,   121,   580,   913,    56,   337,
     338,   109,   394,   413,    87,   136,   394,   132,   961,   130,
     393,    88,   137,   351,    61,   429,   137,   130,    87,   602,
      26,    85,   109,    85,   416,    85,   109,   482,   416,   413,
     413,   917,   413,   919,    87,   418,   783,   425,   785,   444,
     109,   865,   135,   130,   137,    85,   133,   130,   795,    87,
     137,   395,   444,   800,   442,   130,   444,   104,   105,   106,
     443,   947,    61,   130,   416,   129,   126,   129,   128,   129,
      61,   109,   132,   425,    87,   419,    87,    85,   442,    85,
      85,   132,    52,   475,    54,    55,    56,    57,    52,   129,
      54,    55,    56,    57,   482,   655,   109,    85,   109,   126,
      26,   445,   662,    85,   538,   104,   105,   106,   855,   856,
     504,   195,   132,   104,   105,   106,   535,   536,   482,   130,
     126,   129,   128,   129,   129,    89,   132,    68,   522,    87,
     549,   132,    96,   846,   520,   133,   528,   542,   530,   852,
     128,   129,   528,   227,   532,   134,   534,   129,   603,   554,
     542,   109,   116,   109,   542,   239,    59,    60,   541,    85,
     130,   127,   554,    56,   574,   912,   554,   130,   556,   557,
     553,   106,   564,   565,   566,   567,   520,   106,   564,   565,
      15,    61,    17,    68,   528,    65,    85,    68,   580,   583,
     574,   574,   130,   574,   649,   942,    17,   944,   135,   543,
     126,    85,   128,   129,   592,   597,   132,    14,    15,    56,
      25,   555,    94,   607,    85,   603,   300,   133,   127,   557,
     564,   565,   564,   565,   104,   105,   938,    10,   130,   128,
     129,    68,   666,    87,   626,   130,   628,     8,   630,   603,
     136,   652,   653,   130,   128,   129,    13,   658,   659,   641,
     939,    85,   596,   597,   592,   109,   698,   128,   129,    85,
     130,   649,    85,   130,   652,   653,   685,   127,    85,   684,
     658,   659,   687,    68,   666,   667,   130,   669,   697,   671,
     117,   118,   119,   120,   121,   649,   130,   681,    83,    84,
     109,    85,   130,    52,   128,   129,   684,   130,   692,   687,
     688,    52,   128,   129,    85,   128,   129,    85,   696,    16,
      17,   128,   129,    20,   706,   698,   130,    15,   111,   114,
     115,   116,   117,   118,   119,   120,   121,   130,    52,   134,
      54,    55,    56,    57,   128,   129,   728,   135,   114,    46,
      47,   729,   730,    85,    51,    52,   684,   128,   129,   687,
     128,   129,   109,   741,    87,    62,    63,   749,   769,   751,
     130,   753,   754,    88,    10,    89,   130,   782,   130,   757,
     130,    95,    96,   792,     9,   794,   109,   130,   127,   114,
     130,   769,   130,   130,   130,   777,   128,   129,   780,   127,
     130,   779,   116,    10,   782,   119,    52,   130,   786,   787,
     130,    10,   790,    52,   127,    54,    55,    56,    57,    54,
      55,   135,    57,   130,    56,   803,   804,   130,    63,    64,
     130,    52,   130,    54,    55,    56,    57,   819,   820,    56,
     822,   127,   824,   825,    40,    41,    42,    43,    44,   827,
      89,   130,   130,   416,   782,   132,    95,    96,   532,   132,
     534,   130,    89,    87,   220,   941,   722,   845,    89,   889,
      92,   849,   940,   827,    95,    96,   858,   116,    57,    95,
     119,    52,   860,    54,    55,    56,    57,   243,    90,    -1,
     868,   845,   732,   132,    -1,   116,   193,   865,   119,   196,
     197,   198,    52,    -1,    54,    55,    56,    57,   834,   891,
     892,   893,    -1,   895,   896,    -1,    -1,   214,    89,   216,
     217,     2,    -1,     4,    95,   903,    -1,   905,    -1,    -1,
      68,    -1,    -1,   911,    -1,   913,    -1,    -1,    -1,   921,
     922,   923,   924,   938,    -1,    83,    84,   193,    -1,    -1,
     196,   197,    -1,   926,    -1,    -1,   938,    -1,   940,   941,
     938,    -1,    -1,    -1,   937,    -1,    -1,    52,    49,    54,
      55,    56,    57,   955,   956,   957,   958,   961,   116,   117,
     118,   119,   120,   121,    -1,    -1,    -1,   969,     2,    -1,
       4,    -1,    -1,    -1,    -1,   977,    -1,    -1,    -1,    -1,
      -1,   298,    -1,    -1,    89,   939,   303,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,    -1,   331,    49,    -1,    -1,    -1,    53,
      -1,    -1,    -1,    -1,   400,    -1,    -1,    -1,   404,    -1,
      -1,   407,   298,    -1,    -1,   729,   730,   303,    -1,   641,
      -1,    75,    -1,    -1,    -1,    -1,   422,   741,    -1,    -1,
      -1,   427,    -1,    87,    88,    89,    90,    -1,    -1,    -1,
      -1,   378,   379,   757,    -1,    52,    -1,    54,    55,    56,
      57,   388,   389,   390,    -1,    -1,    -1,   394,    -1,   396,
     397,   398,    -1,    -1,    -1,   779,    -1,    -1,    -1,    -1,
      -1,    68,   786,   787,   195,    -1,   790,    -1,   415,    -1,
      -1,    -1,    89,   420,    -1,   481,    83,    84,    95,   803,
     804,    -1,   429,    -1,    52,   432,    54,    55,    56,    57,
      -1,    -1,    -1,   389,   390,    -1,   227,   444,     2,    -1,
       4,    -1,   398,   735,    -1,    -1,   738,   238,   239,    -1,
     117,   118,   119,   120,   121,    -1,    -1,   464,   465,    -1,
      -1,    89,    -1,    -1,    -1,   849,    -1,    95,    96,   476,
      -1,   195,    -1,   429,    -1,    -1,   432,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   868,    49,    -1,    -1,   116,    53,
       2,   119,     4,    -1,    -1,    -1,    -1,   288,    -1,    -1,
      -1,    -1,   293,   227,   132,    -1,    -1,    -1,    -1,   300,
      -1,    75,    -1,    -1,   238,   239,    -1,    -1,    -1,   903,
     476,   905,    -1,    87,    88,    89,    44,   911,    -1,    -1,
      -1,   538,    -1,    -1,   258,   542,    -1,    49,    -1,    -1,
      -1,    -1,   333,    -1,   836,    -1,   838,   554,    -1,    -1,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,   288,    83,    84,    -1,    -1,   293,
     577,    -1,    -1,    -1,    -1,    87,   300,   301,    -1,    -1,
      -1,    -1,   538,    -1,    -1,   877,    -1,   879,    -1,    -1,
      -1,   883,   110,    -1,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,    -1,    -1,    -1,    -1,    -1,   333,
      -1,    -1,   130,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   577,    -1,    -1,    -1,   416,    -1,    -1,   694,    -1,
      -1,   195,    -1,    -1,   425,    -1,   928,    -1,   930,   646,
      -1,   933,    -1,   650,    -1,   652,   653,    -1,    -1,   715,
      -1,   658,   659,    -1,    -1,    -1,    -1,   949,    -1,   666,
     726,    -1,    -1,   227,    -1,    -1,    -1,    -1,    52,    -1,
      54,    55,    56,    57,   238,   239,    -1,    -1,    -1,   971,
     628,    -1,   630,   195,    -1,    -1,    -1,    -1,    -1,    -1,
     646,    -1,   416,    -1,   258,    -1,    -1,    -1,   705,    -1,
      -1,   425,    -1,   710,   711,    89,   713,   714,    -1,    -1,
     666,    95,    96,   720,    -1,   227,    -1,    -1,   442,    -1,
      -1,    -1,    -1,    -1,   288,    -1,   238,   239,    -1,   293,
      -1,    -1,   116,   799,    -1,   119,   300,   301,    -1,    -1,
      -1,   532,    -1,   534,   810,    -1,    -1,    -1,    -1,   815,
      -1,   758,    -1,    -1,    -1,   762,    -1,    -1,   482,    -1,
      -1,    -1,   769,    -1,   720,   556,    -1,    -1,    -1,   333,
      -1,    -1,    -1,    -1,    -1,    -1,   288,    -1,    -1,    -1,
      -1,   293,   789,    -1,    -1,    -1,    -1,    -1,   300,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     807,    -1,   758,   751,    -1,   753,   754,    -1,   532,    -1,
     534,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   333,    -1,    -1,    -1,    -1,     0,    -1,    -1,    -1,
      -1,    -1,   556,   789,     8,     9,    10,    -1,    -1,    13,
      14,    15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   416,    27,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   425,    -1,    37,    38,    -1,    40,    41,    42,    43,
      44,   819,   820,    -1,   822,    -1,   824,   825,   442,   603,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   688,    -1,    -1,
      -1,    -1,    -1,    -1,   416,   696,    -1,    -1,    -1,    -1,
      -1,    85,    -1,   425,    -1,    -1,    -1,    -1,   482,    -1,
      -1,    -1,    -1,    -1,    -1,   649,    -1,    -1,    -1,    -1,
      -1,   938,    -1,    -1,    -1,    -1,    -1,    -1,   729,   730,
      -1,    -1,    -1,   891,   892,   893,    -1,   895,   896,    -1,
     741,    -1,    -1,   127,    -1,   129,    -1,    -1,   132,   133,
      -1,   135,    -1,   137,   688,    -1,   757,    -1,   532,    -1,
     534,    -1,   696,   921,   922,   923,   924,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   779,    -1,
      -1,    -1,   556,    -1,    -1,   786,   787,    -1,    -1,   790,
      -1,    -1,    -1,    -1,    -1,   729,   730,   955,   956,   957,
     958,    -1,   803,   804,    -1,    -1,    -1,   741,    -1,    -1,
     532,   969,   534,    -1,    -1,    -1,    -1,    -1,    -1,   977,
      -1,    -1,    -1,   757,    -1,    -1,    -1,    -1,    -1,   603,
      -1,    -1,    -1,    -1,   556,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   779,    -1,    -1,   849,    -1,
      -1,    -1,   786,   787,    -1,    -1,   790,    -1,    -1,   860,
     634,    -1,    -1,    -1,    -1,    -1,    -1,   868,    -1,   803,
     804,    -1,    -1,    -1,    -1,   649,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      47,    -1,    -1,   827,    51,    52,    53,    -1,    -1,    -1,
      -1,    -1,   903,    -1,   905,    -1,    63,    -1,    -1,    -1,
     911,   845,   913,    -1,   688,   849,    -1,    -1,    75,    -1,
      -1,    -1,   696,    -1,    -1,    -1,   860,    -1,    -1,    -1,
      87,    88,    89,    -1,   868,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,   729,   730,    83,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   688,   741,    -1,   903,
      -1,   905,    -1,    -1,   696,    -1,    -1,   911,    -1,   913,
      -1,    -1,    -1,   757,   110,    -1,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   779,    -1,   729,   730,    -1,
      -1,   137,   786,   787,    -1,    -1,   790,    -1,    -1,   741,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   803,
     804,    -1,    -1,    -1,    -1,   757,   193,    -1,    -1,   196,
     197,   198,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   827,    -1,    -1,    -1,   779,    -1,    -1,
      -1,    -1,    -1,    -1,   786,   787,    -1,    -1,   790,    -1,
      -1,   845,    -1,    -1,    -1,   849,    -1,    -1,    -1,    -1,
      -1,   803,   804,    -1,    -1,    -1,   860,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   868,    -1,    -1,    -1,    -1,    -1,
      -1,   258,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   845,    -1,    -1,    -1,   849,    -1,   903,
      -1,   905,    -1,    -1,    -1,    -1,    -1,   911,   860,   913,
      -1,    -1,    -1,    -1,   301,    -1,   868,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,    -1,    -1,    -1,    44,    -1,    -1,    -1,
      -1,   903,    -1,   905,    -1,    -1,    -1,    -1,    -1,   911,
      -1,   913,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   388,   389,   390,    -1,    -1,    -1,    -1,    -1,   396,
     397,   398,   110,    -1,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,    -1,    -1,    -1,    -1,   415,    -1,
      -1,    -1,    -1,   420,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   429,    -1,    -1,   432,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   442,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   464,   465,    -1,
       0,     1,    -1,     3,     4,     5,     6,     7,    -1,   476,
      -1,    11,    12,    -1,    -1,   482,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    59,
      60,    -1,    62,    63,    64,    -1,    66,    67,    -1,    -1,
      -1,   538,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    -1,    97,    98,    99,
     100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,
     577,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   122,   123,   124,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   135,   603,   137,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     8,     9,    10,    -1,    -1,
      13,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    28,    29,    -1,    -1,   646,
      -1,    -1,   649,   650,    37,    38,    -1,    40,    41,    42,
      43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   666,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    85,    -1,    87,    88,    -1,    -1,   705,    -1,
      -1,    94,    -1,   710,   711,    -1,   713,   714,    -1,    -1,
      -1,    -1,    -1,   720,   107,    -1,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,    -1,
      -1,    -1,    -1,    -1,   127,   128,   129,   130,    -1,    -1,
     133,   134,   135,    -1,   137,    -1,    -1,    -1,    -1,    -1,
      -1,   758,    -1,    -1,    -1,   762,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     0,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     8,
       9,    10,   789,    -1,    13,    14,    15,    -1,    17,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,    27,    28,
     807,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,    38,
      -1,    40,    41,    42,    43,    44,    -1,    -1,    -1,    -1,
     827,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   845,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    -1,    -1,    83,    84,    85,    -1,    -1,    88,
      -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   110,    -1,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,    -1,    -1,    -1,    -1,   126,   127,   128,
     129,   130,     0,   132,   133,   134,   135,    -1,   137,    -1,
       8,     9,    10,    -1,    -1,    13,    14,    15,    -1,    17,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,
      28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,
      38,    -1,    40,    41,    42,    43,    44,    -1,    -1,    -1,
      -1,    -1,    -1,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    -1,    -1,    83,    84,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    85,    -1,    -1,
      88,    -1,    -1,    -1,    -1,   110,    94,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,    -1,    -1,   107,
      -1,    -1,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,    -1,    -1,    -1,    -1,    -1,   127,
     128,   129,   130,     0,    -1,   133,   134,   135,    -1,   137,
      -1,     8,     9,    10,    -1,    -1,    13,    14,    15,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,
      27,    28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      37,    38,    -1,    40,    41,    42,    43,    44,    -1,    -1,
      -1,    -1,    -1,    -1,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    -1,    -1,    83,
      84,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    -1,    -1,    83,    84,    85,    -1,
      -1,    88,    -1,    -1,    -1,    -1,    -1,    94,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    -1,    -1,
      -1,    -1,    -1,   110,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,   126,
     127,   128,   129,   130,     0,   132,   133,   134,   135,    -1,
     137,    -1,     8,     9,    10,    -1,    -1,    13,    14,    15,
      -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    37,    38,    -1,    40,    41,    42,    43,    44,    -1,
      -1,    -1,    -1,    -1,    -1,    68,    69,    70,    71,    72,
      73,    74,    75,    -1,    77,    78,    -1,    -1,    -1,    -1,
      83,    84,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    -1,    -1,    83,    84,    85,
      -1,    -1,    88,    -1,    -1,    -1,    -1,    -1,    94,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,    -1,
      -1,    -1,    -1,    -1,   110,    -1,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,
      -1,   127,   128,   129,   130,     0,   132,   133,   134,   135,
      -1,   137,    -1,     8,     9,    10,    -1,    -1,    -1,    14,
      15,    -1,    17,    -1,    -1,    -1,    -1,    44,    -1,    -1,
      -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    37,    38,    -1,    40,    41,    42,    43,    44,
      -1,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    -1,    -1,    83,    84,    -1,    -1,
      -1,    -1,    -1,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    -1,    -1,    83,    84,
      85,    -1,    87,   110,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   109,   110,    -1,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,    -1,    -1,    -1,
      -1,   126,   127,   128,   129,   130,     0,    -1,   133,    -1,
     135,    -1,   137,    -1,     8,     9,    10,    -1,    -1,    -1,
      14,    15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    37,    38,    -1,    40,    41,    42,    43,
      44,    68,    69,    70,    71,    72,    73,    74,    -1,    -1,
      77,    78,    -1,    -1,    -1,    -1,    83,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    -1,    -1,    83,
      84,    85,    -1,    87,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   109,   110,    -1,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    -1,    -1,
      -1,    -1,    -1,   127,   128,   129,   130,    -1,    -1,   133,
      -1,   135,     1,   137,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    -1,    -1,    15,    16,    -1,    18,
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
      -1,    -1,    -1,    -1,    -1,    -1,   135,     1,   137,     3,
       4,     5,     6,     7,    -1,    -1,    10,    11,    12,    -1,
      14,    15,    16,    -1,    18,    19,    20,    21,    22,    23,
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
     124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   135,     1,   137,     3,     4,     5,     6,     7,    -1,
      -1,    10,    11,    12,    -1,    -1,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    45,    46,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,    63,    64,    -1,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    -1,    97,    98,
      99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,
      -1,     1,    -1,     3,     4,     5,     6,     7,    -1,    -1,
      -1,    11,    12,   122,   123,   124,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,   135,    -1,   137,    -1,
      30,    31,    32,    33,    34,    35,    36,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    59,
      60,    -1,    62,    63,    64,    -1,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    -1,    97,    98,    99,
     100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   122,   123,   124,    -1,    -1,   127,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   135,     1,   137,     3,     4,
       5,     6,     7,    -1,    -1,    10,    11,    12,    -1,    -1,
      15,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      45,    46,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,    -1,    62,    63,    64,
      -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      95,    -1,    97,    98,    99,   100,   101,   102,   103,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,
       6,     7,    -1,     9,    10,    11,    12,   122,   123,   124,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
     135,    -1,   137,    -1,    30,    31,    32,    33,    34,    35,
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
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,    63,    64,    -1,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,
      97,    98,    99,   100,   101,   102,   103,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   122,   123,   124,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   133,    -1,   135,     1,
     137,     3,     4,     5,     6,     7,    -1,    -1,    -1,    11,
      12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    45,    46,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    -1,
      62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    -1,    97,    98,    99,   100,   101,
     102,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     122,   123,   124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   133,    -1,   135,     1,   137,     3,     4,     5,     6,
       7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,    46,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,    63,    64,    -1,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,
      97,    98,    99,   100,   101,   102,   103,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   122,   123,   124,    -1,    -1,
     127,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   135,     1,
     137,     3,     4,     5,     6,     7,    -1,    -1,    10,    11,
      12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    45,    46,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    -1,
      62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    -1,    97,    98,    99,   100,   101,
     102,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     122,   123,   124,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,   135,    -1,   137,    -1,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    -1,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    -1,    62,
      63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    96,    97,    98,    99,   100,   101,   102,
     103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   122,
     123,   124,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,   137,    -1,    30,    31,    32,    33,
      34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    63,
      64,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    -1,    -1,    97,    98,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   122,   123,
     124,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,   135,    -1,   137,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    -1,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,    -1,    62,    63,    64,
      -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      -1,    -1,    97,    98,    99,   100,   101,   102,   103,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,    -1,    -1,    -1,    11,    12,   122,   123,   124,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,   137,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    64,    -1,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      -1,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,    -1,    -1,    -1,    11,    12,   122,   123,   124,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,   135,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,    46,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,    63,    64,    -1,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,
      97,    98,    99,   100,   101,   102,   103,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   122,   123,   124,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   135,     3,
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
     124,     3,     4,     5,    -1,     7,    -1,   131,   132,    11,
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
      18,    19,    20,    21,    22,    23,    24,    -1,    26,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    -1,
      -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,
      -1,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    62,    63,    64,    -1,    66,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    -1,    92,    93,    -1,    95,    96,    97,
      98,    99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   122,   123,   124,    -1,   126,     3,
       4,     5,    -1,     7,   132,    -1,    -1,    11,    12,    -1,
      -1,    -1,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    -1,    62,    63,
      64,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    87,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    96,    97,    98,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,    -1,   109,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,   123,
     124,     3,     4,     5,    -1,     7,    -1,    -1,   132,    11,
      12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
      62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    96,    97,    98,    99,   100,   101,
     102,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     122,   123,   124,     3,     4,     5,    -1,     7,    -1,    -1,
     132,    11,    12,    -1,    -1,    -1,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    -1,    62,    63,    64,    -1,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    96,    97,    98,    99,
     100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   122,   123,   124,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   132,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    -1,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    -1,    -1,    77,    78,    -1,
      -1,    81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,    -1,   123,   124,    -1,    -1,    -1,    -1,    -1,
      -1,   131,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    -1,
      -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    -1,    56,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,
      81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,   124,    -1,    -1,    -1,    -1,    -1,    -1,
     131,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    -1,    -1,
      -1,    -1,    -1,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    -1,    -1,    56,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,    81,
      82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    95,    -1,    -1,    98,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
      -1,   123,   124,    -1,    -1,    -1,    -1,    -1,    -1,   131,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    -1,    -1,    -1,
      -1,    -1,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    -1,    -1,    56,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    -1,    -1,    77,    78,    -1,    -1,    81,    82,
      83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,    -1,
     123,   124,     3,     4,     5,    -1,     7,    -1,   131,    -1,
      11,    12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,
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
      16,   122,    18,    19,    20,    21,    22,    23,    24,   130,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    -1,    62,    63,    64,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    -1,
      -1,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,   122,    18,    19,    20,
      21,    22,    23,    24,   130,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      -1,    62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    95,    -1,    97,    98,    99,   100,
     101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,    -1,    -1,    -1,    11,
      12,   122,   123,   124,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    45,    46,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    -1,
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
      93,    -1,    95,    96,    97,    98,    99,   100,   101,   102,
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
      -1,    95,    96,    97,    98,    99,   100,   101,   102,   103,
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
      95,    -1,    97,    98,    99,   100,   101,   102,   103,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   122,   123,   124,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    -1,    62,    63,    64,    -1,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    -1,
      96,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   122,   123,   124,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    -1,    62,    63,    64,    -1,    66,
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
      58,    59,    60,    -1,    62,    63,    64,    -1,    66,    67,
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
      -1,    92,    93,    -1,    95,    -1,    97,    98,    99,   100,
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
      92,    93,    -1,    95,    -1,    97,    98,    99,   100,   101,
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
      -1,    -1,    -1,    97,    98,    99,   100,   101,   102,   103,
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
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    -1,
      -1,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   122,   123,   124,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,    63,    64,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,
      97,    98,    99,   100,   101,   102,   103,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,    -1,    -1,    -1,    16,   122,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    -1,
      62,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    -1,    97,    98,    99,   100,   101,
     102,   103,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,
     122,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,    63,    64,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    83,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    -1,    -1,
      97,    98,    99,   100,   101,   102,   103,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,    -1,    -1,    -1,    16,   122,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    -1,
      62,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    -1,    -1,    97,    98,    99,   100,   101,
     102,   103,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,
     122,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,    63,    64,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    -1,    -1,
      97,    98,    99,   100,   101,   102,   103,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,    -1,    -1,    -1,    16,   122,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    -1,
      62,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    -1,    -1,    97,    98,    99,   100,   101,
     102,   103,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,
     122,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,    63,    64,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    -1,    -1,
      97,    98,    99,   100,   101,   102,   103,    -1,    -1,    -1,
      -1,    -1,    52,    53,    -1,    -1,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   122,    66,    67,    68,    69,
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
      81,    82,    83,    84,    -1,    -1,    -1,    68,    69,    70,
      71,    72,    73,    74,    95,    -1,    77,    78,    -1,    -1,
      -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,   124,    -1,    -1,    -1,    -1,    -1,    -1,
     131,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121
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
     262,   272,   274,   275,   276,   277,   278,   279,   302,   313,
     143,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    56,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    77,    78,    81,    82,    83,    84,
      95,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   123,   124,   131,   168,   169,   170,   171,   173,   174,
     272,   274,    39,    58,    86,    89,    95,    96,   123,   157,
     165,   175,   177,   182,   185,   187,   206,   276,   278,   279,
     300,   301,   182,   182,   132,   183,   184,   132,   179,   183,
     132,   137,   307,    54,   170,   307,   144,   126,    21,    22,
      30,    31,    32,   175,   206,   302,   175,    56,     1,    89,
     146,   147,   148,   159,   160,   313,   150,   178,   187,   300,
     313,   177,   299,   300,   313,    46,    86,   122,   130,   164,
     189,   206,   276,   279,   232,   233,    54,    55,    57,   168,
     265,   273,   264,   265,   266,   136,   260,   136,   263,    59,
      60,   152,   175,   175,   135,   137,   306,   311,   312,    40,
      41,    42,    43,    44,    37,    38,    28,   237,   109,   130,
      89,    95,   161,   109,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    83,    84,   110,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
      85,   128,   129,   190,   150,   151,   151,   193,   195,   151,
     306,   312,    86,   158,   165,   206,   222,   276,   279,    52,
      56,    83,    86,   166,   167,   206,   276,   279,   167,    33,
      34,    35,    36,    49,    50,    51,    52,    56,   132,   168,
     277,   297,    85,   129,    26,   126,   242,   256,    87,    87,
     179,   183,   242,   130,   177,    56,   177,   177,   109,    88,
     130,   186,   313,    85,   128,   129,    87,    87,   130,   186,
     182,   307,   308,   182,   181,   182,   313,   150,   308,   150,
      54,    63,    64,   149,   132,   176,   126,   146,    85,   129,
      87,   148,   159,   133,   306,   312,   308,   191,   134,   130,
     137,   310,   130,   310,   127,   310,   307,    56,    59,    60,
     161,   163,   130,    85,   128,   129,   234,    61,   104,   105,
     106,   267,   106,   267,   106,    65,   267,   106,   106,   261,
     267,   106,    61,   106,    68,    68,   135,   143,   151,   151,
     151,   151,   148,   150,   150,   238,    95,   152,   177,   187,
     188,   159,   130,   164,   130,   152,   175,   177,   188,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,    52,    53,    56,   173,   179,
     303,   304,   181,    52,    53,    56,   173,   179,   303,   145,
     146,    13,   218,   311,   218,   151,   151,   306,    17,   247,
      56,    85,   128,   129,    25,   150,    52,    56,   166,     1,
     113,   280,   311,    85,   128,   129,   202,   298,   203,    85,
     129,   305,    52,    56,   303,   303,   245,   243,   152,   175,
     152,   175,    94,   154,   172,   175,   177,    95,   177,   185,
     300,    52,    56,   181,    52,    56,   301,   308,   133,   308,
     308,   170,   192,   175,   141,   127,   303,   303,   175,   308,
     148,   308,   300,   130,   163,    52,    56,   181,    52,    56,
      52,    54,    55,    56,    57,    89,    95,    96,   116,   119,
     132,   235,   283,   284,   285,   286,   287,   288,   291,   292,
     293,   294,   295,   269,   268,   136,   267,   136,   175,   175,
      76,   114,   227,   228,   313,   177,   130,   308,   163,   130,
      44,   307,    87,    87,   179,   183,   307,   309,    87,    87,
     179,   180,   183,   313,    10,   217,     8,   249,   313,   146,
      13,   146,    27,   219,   311,   219,   247,   187,   217,    52,
      56,   181,    52,    56,   197,   200,   311,   281,   199,    52,
      56,   166,   181,   145,   150,   132,   282,   283,   204,   180,
     183,   180,   183,   227,   227,    44,   155,   170,   177,   186,
      87,    87,   309,    87,    87,   150,   127,   310,   161,   309,
     109,    52,    89,    95,   223,   224,   225,   285,   283,    29,
     107,   236,   130,   296,   313,   130,   296,    52,   130,   296,
      52,   270,    54,    55,    57,   271,   279,    52,   135,   226,
     229,   287,   289,   290,   293,   295,   313,   239,    95,   177,
     163,   175,   111,   152,   175,   152,   175,   154,   134,    87,
     152,   175,   152,   175,   154,   177,   188,   250,   313,    15,
     221,   313,    14,   220,   221,   221,   194,   196,   217,   130,
     218,   309,   151,   311,   151,   145,   309,   217,   308,   283,
     145,   311,   168,   246,   244,   175,   227,    87,   308,   177,
     225,   130,   285,   130,   308,   229,   146,   146,   286,   291,
     293,   295,   287,   288,   293,   287,   146,   109,    52,   230,
     231,   284,   229,   114,   130,   296,   130,   296,   130,   296,
     146,   177,   175,   152,   175,    88,   251,   313,   146,     9,
     252,   313,   151,   217,   217,   146,   146,   177,   146,   219,
     201,   311,   217,   308,   217,   205,   146,   146,   156,   308,
     224,   130,    95,   223,   308,    10,   127,   130,   296,   130,
     296,   130,   296,   130,   296,   296,   127,    86,   206,   130,
     114,   290,   293,   287,   289,   293,   287,    10,    86,   165,
     206,   276,   279,   218,   146,   218,   217,   217,   221,   247,
     248,   198,   145,   282,    10,   127,   146,   130,   224,   130,
     285,   287,   293,   287,   287,    56,    85,   231,   130,   296,
     130,   296,   296,   130,   296,   296,    56,    85,   128,   129,
     146,   146,   146,   217,   145,   127,   224,   130,   296,   130,
     296,   296,   296,    52,    56,   287,   293,   287,   287,    52,
      56,   181,    52,    56,   249,   220,   217,   217,   224,   287,
     296,   130,   296,   296,   296,   309,   296,   287,   296
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
#line 580 "grammar19.y"
    {
                    lex_state = EXPR_BEG;
                    variables = new LocalState(0);
                    class_nest = 0;
                  ;}
    break;

  case 3:
#line 586 "grammar19.y"
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
#line 605 "grammar19.y"
    {
                    void_stmts((yyvsp[(1) - (2)].node));
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                  ;}
    break;

  case 5:
#line 612 "grammar19.y"
    {
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 6:
#line 616 "grammar19.y"
    {
                    (yyval.node) = newline_node((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 7:
#line 620 "grammar19.y"
    {
                    (yyval.node) = block_append((yyvsp[(1) - (3)].node), newline_node((yyvsp[(3) - (3)].node)));
                  ;}
    break;

  case 8:
#line 624 "grammar19.y"
    {
                    (yyval.node) = remove_begin((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 10:
#line 631 "grammar19.y"
    {
                    if(in_def || in_single) {
                      yy_error("BEGIN in method");
                    }
                  ;}
    break;

  case 11:
#line 637 "grammar19.y"
    {
                    /* TODO
                    block_append( , $4);
                    */
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 12:
#line 649 "grammar19.y"
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
#line 669 "grammar19.y"
    {
                    void_stmts((yyvsp[(1) - (2)].node));
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                  ;}
    break;

  case 14:
#line 676 "grammar19.y"
    {
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 15:
#line 680 "grammar19.y"
    {
                    (yyval.node) = newline_node((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 16:
#line 684 "grammar19.y"
    {
                    (yyval.node) = block_append((yyvsp[(1) - (3)].node), newline_node((yyvsp[(3) - (3)].node)));
                  ;}
    break;

  case 17:
#line 688 "grammar19.y"
    {
                    (yyval.node) = remove_begin((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 18:
#line 693 "grammar19.y"
    {lex_state = EXPR_FNAME;;}
    break;

  case 19:
#line 694 "grammar19.y"
    {
                    (yyval.node) = NEW_ALIAS((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 20:
#line 698 "grammar19.y"
    {
                    (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 21:
#line 702 "grammar19.y"
    {
                    char buf[3];

                    snprintf(buf, sizeof(buf), "$%c", (char)(yyvsp[(3) - (3)].node)->nd_nth);
                    (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), rb_parser_sym(buf));
                  ;}
    break;

  case 22:
#line 709 "grammar19.y"
    {
                    yy_error("can't make alias for the number variables");
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 23:
#line 714 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 24:
#line 718 "grammar19.y"
    {
                    (yyval.node) = NEW_IF(cond((yyvsp[(3) - (3)].node)), remove_begin((yyvsp[(1) - (3)].node)), 0);
                    fixpos((yyval.node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 25:
#line 723 "grammar19.y"
    {
                    (yyval.node) = NEW_UNLESS(cond((yyvsp[(3) - (3)].node)), remove_begin((yyvsp[(1) - (3)].node)), 0);
                    fixpos((yyval.node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 26:
#line 728 "grammar19.y"
    {
                    if((yyvsp[(1) - (3)].node) && nd_type((yyvsp[(1) - (3)].node)) == NODE_BEGIN) {
                      (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (3)].node)), (yyvsp[(1) - (3)].node)->nd_body, 0);
                    } else {
                      (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (3)].node)), (yyvsp[(1) - (3)].node), 1);
                    }
                  ;}
    break;

  case 27:
#line 736 "grammar19.y"
    {
                    if((yyvsp[(1) - (3)].node) && nd_type((yyvsp[(1) - (3)].node)) == NODE_BEGIN) {
                      (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (3)].node)), (yyvsp[(1) - (3)].node)->nd_body, 0);
                    } else {
                      (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (3)].node)), (yyvsp[(1) - (3)].node), 1);
                    }
                  ;}
    break;

  case 28:
#line 744 "grammar19.y"
    {
                    NODE *resq = NEW_RESBODY(0, remove_begin((yyvsp[(3) - (3)].node)), 0);
                    (yyval.node) = NEW_RESCUE(remove_begin((yyvsp[(1) - (3)].node)), resq, 0);
                  ;}
    break;

  case 29:
#line 749 "grammar19.y"
    {
                    if(in_def || in_single) {
                      rb_warn("END in method; use at_exit");
                    }

                    (yyval.node) = NEW_POSTEXE(NEW_NODE(NODE_SCOPE,
                            0 /* tbl */, (yyvsp[(3) - (4)].node) /* body */, 0 /* args */));
                  ;}
    break;

  case 30:
#line 758 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 31:
#line 763 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyvsp[(1) - (3)].node)->nd_value = ((yyvsp[(1) - (3)].node)->nd_head) ? NEW_TO_ARY((yyvsp[(3) - (3)].node)) : NEW_ARRAY((yyvsp[(3) - (3)].node));
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 32:
#line 769 "grammar19.y"
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
#line 791 "grammar19.y"
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
#line 806 "grammar19.y"
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
#line 817 "grammar19.y"
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
#line 828 "grammar19.y"
    {
                    yy_error("constant re-assignment");
                    (yyval.node) = 0;
                  ;}
    break;

  case 37:
#line 833 "grammar19.y"
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
#line 844 "grammar19.y"
    {
                    rb_backref_error((yyvsp[(1) - (3)].node));
                    (yyval.node) = 0;
                  ;}
    break;

  case 39:
#line 849 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 40:
#line 854 "grammar19.y"
    {
                    (yyvsp[(1) - (3)].node)->nd_value = ((yyvsp[(1) - (3)].node)->nd_head) ? NEW_TO_ARY((yyvsp[(3) - (3)].node)) : NEW_ARRAY((yyvsp[(3) - (3)].node));
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 41:
#line 859 "grammar19.y"
    {
                    (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 44:
#line 868 "grammar19.y"
    {
                    (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 45:
#line 872 "grammar19.y"
    {
                    (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 46:
#line 876 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[(3) - (3)].node)), '!');
                  ;}
    break;

  case 47:
#line 880 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[(2) - (2)].node)), '!');
                  ;}
    break;

  case 49:
#line 887 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  ;}
    break;

  case 53:
#line 900 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 54:
#line 904 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 55:
#line 910 "grammar19.y"
    {
                    /* TODO */
                    (yyvsp[(1) - (1)].num) = ruby_sourceline;
                    reset_block();
                  ;}
    break;

  case 56:
#line 915 "grammar19.y"
    { (yyval.vars) = variables->block_vars; ;}
    break;

  case 57:
#line 918 "grammar19.y"
    {
                    /* TODO $$ = NEW_ITER($3, 0, extract_block_vars($5, * $<vars>4)); */
                    (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), extract_block_vars((yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                    nd_set_line((yyval.node), (yyvsp[(2) - (6)].num));
                  ;}
    break;

  case 58:
#line 926 "grammar19.y"
    {
                    (yyval.node) = NEW_FCALL((yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                 ;}
    break;

  case 59:
#line 931 "grammar19.y"
    {
                    block_dup_check((yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].node));
                    (yyvsp[(3) - (3)].node)->nd_iter = NEW_FCALL((yyvsp[(1) - (3)].id), (yyvsp[(2) - (3)].node));
                    (yyval.node) = (yyvsp[(3) - (3)].node);
                    fixpos((yyval.node), (yyvsp[(2) - (3)].node));
                 ;}
    break;

  case 60:
#line 938 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 61:
#line 943 "grammar19.y"
    {
                    block_dup_check((yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                    (yyvsp[(5) - (5)].node)->nd_iter = NEW_CALL((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].node));
                    (yyval.node) = (yyvsp[(5) - (5)].node);
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                 ;}
    break;

  case 62:
#line 950 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 63:
#line 955 "grammar19.y"
    {
                    block_dup_check((yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                    (yyvsp[(5) - (5)].node)->nd_iter = NEW_CALL((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].node));
                    (yyval.node) = (yyvsp[(5) - (5)].node);
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 64:
#line 962 "grammar19.y"
    {
                    (yyval.node) = NEW_SUPER((yyvsp[(2) - (2)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 65:
#line 967 "grammar19.y"
    {
                    (yyval.node) = new_yield((yyvsp[(2) - (2)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 66:
#line 972 "grammar19.y"
    {
                    (yyval.node) = NEW_RETURN(ret_args((yyvsp[(2) - (2)].node)));
                  ;}
    break;

  case 67:
#line 976 "grammar19.y"
    {
                    (yyval.node) = NEW_BREAK(ret_args((yyvsp[(2) - (2)].node)));
                  ;}
    break;

  case 68:
#line 980 "grammar19.y"
    {
                    (yyval.node) = NEW_NEXT(ret_args((yyvsp[(2) - (2)].node)));
                  ;}
    break;

  case 70:
#line 987 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 72:
#line 994 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(NEW_LIST((yyvsp[(2) - (3)].node)), 0);
                  ;}
    break;

  case 73:
#line 1000 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (1)].node), 0);
                  ;}
    break;

  case 74:
#line 1004 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(list_append((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node)), 0);
                  ;}
    break;

  case 75:
#line 1008 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 76:
#line 1012 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (5)].node), NEW_POSTARG((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)));
                  ;}
    break;

  case 77:
#line 1016 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (2)].node), -1);
                  ;}
    break;

  case 78:
#line 1020 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (4)].node), NEW_POSTARG(-1, (yyvsp[(4) - (4)].node)));
                  ;}
    break;

  case 79:
#line 1024 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 80:
#line 1028 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].node)));
                  ;}
    break;

  case 81:
#line 1032 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, -1);
                  ;}
    break;

  case 82:
#line 1036 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG(-1, (yyvsp[(3) - (3)].node)));
                  ;}
    break;

  case 84:
#line 1043 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 85:
#line 1049 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (2)].node));
                  ;}
    break;

  case 86:
#line 1053 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                  ;}
    break;

  case 87:
#line 1059 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 88:
#line 1063 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 89:
#line 1069 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                  ;}
    break;

  case 90:
#line 1073 "grammar19.y"
    {
                    (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
                  ;}
    break;

  case 91:
#line 1077 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 92:
#line 1081 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 93:
#line 1085 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 94:
#line 1089 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                  ;}
    break;

  case 95:
#line 1095 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                  ;}
    break;

  case 96:
#line 1101 "grammar19.y"
    {
                    rb_backref_error((yyvsp[(1) - (1)].node));
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 97:
#line 1108 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                    if(!(yyval.node)) (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 98:
#line 1113 "grammar19.y"
    {
                    (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
                  ;}
    break;

  case 99:
#line 1117 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 100:
#line 1121 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 101:
#line 1125 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 102:
#line 1129 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                  ;}
    break;

  case 103:
#line 1135 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                  ;}
    break;

  case 104:
#line 1141 "grammar19.y"
    {
                    rb_backref_error((yyvsp[(1) - (1)].node));
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 105:
#line 1148 "grammar19.y"
    {
                    yy_error("class/module name must be CONSTANT");
                  ;}
    break;

  case 107:
#line 1155 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 108:
#line 1159 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON2(0, (yyval.node));
                  ;}
    break;

  case 109:
#line 1163 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 113:
#line 1172 "grammar19.y"
    {
                    lex_state = EXPR_ENDFN;
                    (yyval.id) = convert_op((yyvsp[(1) - (1)].id));
                  ;}
    break;

  case 114:
#line 1177 "grammar19.y"
    {
                    lex_state = EXPR_ENDFN;
                    (yyval.id) = (yyvsp[(1) - (1)].id);
                  ;}
    break;

  case 117:
#line 1188 "grammar19.y"
    {
                    (yyval.node) = NEW_LIT(QUID2SYM((yyvsp[(1) - (1)].id)));
                  ;}
    break;

  case 119:
#line 1195 "grammar19.y"
    {
                    (yyval.node) = NEW_UNDEF((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 120:
#line 1198 "grammar19.y"
    {lex_state = EXPR_FNAME;;}
    break;

  case 121:
#line 1199 "grammar19.y"
    {
                    (yyval.node) = block_append((yyvsp[(1) - (4)].node), NEW_UNDEF((yyvsp[(4) - (4)].node)));
                  ;}
    break;

  case 122:
#line 1204 "grammar19.y"
    { (yyval.id) = '|'; ;}
    break;

  case 123:
#line 1205 "grammar19.y"
    { (yyval.id) = '^'; ;}
    break;

  case 124:
#line 1206 "grammar19.y"
    { (yyval.id) = '&'; ;}
    break;

  case 125:
#line 1207 "grammar19.y"
    { (yyval.id) = tCMP; ;}
    break;

  case 126:
#line 1208 "grammar19.y"
    { (yyval.id) = tEQ; ;}
    break;

  case 127:
#line 1209 "grammar19.y"
    { (yyval.id) = tEQQ; ;}
    break;

  case 128:
#line 1210 "grammar19.y"
    { (yyval.id) = tMATCH; ;}
    break;

  case 129:
#line 1211 "grammar19.y"
    { (yyval.id) = tNMATCH; ;}
    break;

  case 130:
#line 1212 "grammar19.y"
    { (yyval.id) = '>'; ;}
    break;

  case 131:
#line 1213 "grammar19.y"
    { (yyval.id) = tGEQ; ;}
    break;

  case 132:
#line 1214 "grammar19.y"
    { (yyval.id) = '<'; ;}
    break;

  case 133:
#line 1215 "grammar19.y"
    { (yyval.id) = tLEQ; ;}
    break;

  case 134:
#line 1216 "grammar19.y"
    { (yyval.id) = tNEQ; ;}
    break;

  case 135:
#line 1217 "grammar19.y"
    { (yyval.id) = tLSHFT; ;}
    break;

  case 136:
#line 1218 "grammar19.y"
    { (yyval.id) = tRSHFT; ;}
    break;

  case 137:
#line 1219 "grammar19.y"
    { (yyval.id) = '+'; ;}
    break;

  case 138:
#line 1220 "grammar19.y"
    { (yyval.id) = '-'; ;}
    break;

  case 139:
#line 1221 "grammar19.y"
    { (yyval.id) = '*'; ;}
    break;

  case 140:
#line 1222 "grammar19.y"
    { (yyval.id) = '*'; ;}
    break;

  case 141:
#line 1223 "grammar19.y"
    { (yyval.id) = '/'; ;}
    break;

  case 142:
#line 1224 "grammar19.y"
    { (yyval.id) = '%'; ;}
    break;

  case 143:
#line 1225 "grammar19.y"
    { (yyval.id) = tPOW; ;}
    break;

  case 144:
#line 1226 "grammar19.y"
    { (yyval.id) = '!'; ;}
    break;

  case 145:
#line 1227 "grammar19.y"
    { (yyval.id) = '~'; ;}
    break;

  case 146:
#line 1228 "grammar19.y"
    { (yyval.id) = tUPLUS; ;}
    break;

  case 147:
#line 1229 "grammar19.y"
    { (yyval.id) = tUMINUS; ;}
    break;

  case 148:
#line 1230 "grammar19.y"
    { (yyval.id) = tAREF; ;}
    break;

  case 149:
#line 1231 "grammar19.y"
    { (yyval.id) = tASET; ;}
    break;

  case 150:
#line 1232 "grammar19.y"
    { (yyval.id) = '`'; ;}
    break;

  case 192:
#line 1250 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 193:
#line 1255 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (5)].node));
                    (yyvsp[(3) - (5)].node) = NEW_RESCUE((yyvsp[(3) - (5)].node), NEW_RESBODY(0, (yyvsp[(5) - (5)].node), 0), 0);
                    (yyval.node) = node_assign((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node));
                  ;}
    break;

  case 194:
#line 1261 "grammar19.y"
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
#line 1283 "grammar19.y"
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
#line 1306 "grammar19.y"
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
#line 1325 "grammar19.y"
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
#line 1336 "grammar19.y"
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
#line 1347 "grammar19.y"
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
#line 1358 "grammar19.y"
    {
                    yy_error("constant re-assignment");
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 201:
#line 1363 "grammar19.y"
    {
                    yy_error("constant re-assignment");
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 202:
#line 1368 "grammar19.y"
    {
                    rb_backref_error((yyvsp[(1) - (3)].node));
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 203:
#line 1373 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (3)].node));
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = NEW_DOT2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 204:
#line 1379 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (3)].node));
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = NEW_DOT3((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 205:
#line 1385 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '+', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 206:
#line 1389 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '-', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 207:
#line 1393 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '*', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 208:
#line 1397 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '/', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 209:
#line 1401 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '%', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 210:
#line 1405 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tPOW, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 211:
#line 1409 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL(call_bin_op((yyvsp[(2) - (4)].node), tPOW, (yyvsp[(4) - (4)].node)), tUMINUS, 0);
                  ;}
    break;

  case 212:
#line 1413 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL(call_bin_op((yyvsp[(2) - (4)].node), tPOW, (yyvsp[(4) - (4)].node)), tUMINUS, 0);
                  ;}
    break;

  case 213:
#line 1417 "grammar19.y"
    {
                    (yyval.node) = call_uni_op((yyvsp[(2) - (2)].node), tUPLUS);
                  ;}
    break;

  case 214:
#line 1421 "grammar19.y"
    {
                    (yyval.node) = call_uni_op((yyvsp[(2) - (2)].node), tUMINUS);
                  ;}
    break;

  case 215:
#line 1425 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '|', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 216:
#line 1429 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '^', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 217:
#line 1433 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '&', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 218:
#line 1437 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tCMP, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 219:
#line 1441 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '>', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 220:
#line 1445 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tGEQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 221:
#line 1449 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '<', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 222:
#line 1453 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tLEQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 223:
#line 1457 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tEQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 224:
#line 1461 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tEQQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 225:
#line 1465 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tNEQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 226:
#line 1469 "grammar19.y"
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
#line 1479 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tNMATCH, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 228:
#line 1483 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[(2) - (2)].node)), '!');
                  ;}
    break;

  case 229:
#line 1487 "grammar19.y"
    {
                    (yyval.node) = call_uni_op((yyvsp[(2) - (2)].node), '~');
                  ;}
    break;

  case 230:
#line 1491 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tLSHFT, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 231:
#line 1495 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tRSHFT, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 232:
#line 1499 "grammar19.y"
    {
                    (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 233:
#line 1503 "grammar19.y"
    {
                    (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 234:
#line 1506 "grammar19.y"
    {in_defined = 1;;}
    break;

  case 235:
#line 1507 "grammar19.y"
    {
                    in_defined = 0;
                    (yyval.node) = NEW_DEFINED((yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 236:
#line 1512 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (6)].node));
                    (yyval.node) = NEW_IF(cond((yyvsp[(1) - (6)].node)), (yyvsp[(3) - (6)].node), (yyvsp[(6) - (6)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (6)].node));
                  ;}
    break;

  case 237:
#line 1518 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                  ;}
    break;

  case 238:
#line 1524 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  ;}
    break;

  case 240:
#line 1533 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                  ;}
    break;

  case 241:
#line 1537 "grammar19.y"
    {
                    (yyval.node) = arg_append((yyvsp[(1) - (4)].node), NEW_HASH((yyvsp[(3) - (4)].node)));
                  ;}
    break;

  case 242:
#line 1541 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                  ;}
    break;

  case 243:
#line 1547 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 248:
#line 1561 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 249:
#line 1566 "grammar19.y"
    {
                    (yyval.node) = arg_blk_pass((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 250:
#line 1570 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                    (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 251:
#line 1575 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (4)].node), NEW_HASH((yyvsp[(3) - (4)].node)));
                    (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 253:
#line 1582 "grammar19.y"
    {
                    (yyval.val) = cmdarg_stack;
                    CMDARG_PUSH(1);
                  ;}
    break;

  case 254:
#line 1587 "grammar19.y"
    {
                    /* CMDARG_POP() */
                    cmdarg_stack = (yyvsp[(1) - (2)].val);
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 255:
#line 1595 "grammar19.y"
    {
                    (yyval.node) = NEW_BLOCK_PASS((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 256:
#line 1601 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 257:
#line 1605 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 258:
#line 1609 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 259:
#line 1615 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 260:
#line 1619 "grammar19.y"
    {
                    (yyval.node) = NEW_SPLAT((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 261:
#line 1623 "grammar19.y"
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
#line 1632 "grammar19.y"
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
#line 1643 "grammar19.y"
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
#line 1652 "grammar19.y"
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
#line 1661 "grammar19.y"
    {
                    (yyval.node) = NEW_SPLAT((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 274:
#line 1675 "grammar19.y"
    {
                    (yyval.node) = NEW_FCALL((yyvsp[(1) - (1)].id), 0);
                  ;}
    break;

  case 275:
#line 1679 "grammar19.y"
    {
                    (yyval.num) = ruby_sourceline;
                  ;}
    break;

  case 276:
#line 1684 "grammar19.y"
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
#line 1695 "grammar19.y"
    {lex_state = EXPR_ENDARG;;}
    break;

  case 278:
#line 1696 "grammar19.y"
    {
                    rb_warning("(...) interpreted as grouped expression");
                    (yyval.node) = (yyvsp[(2) - (4)].node);
                  ;}
    break;

  case 279:
#line 1701 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 280:
#line 1705 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 281:
#line 1709 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 282:
#line 1713 "grammar19.y"
    {
                    if((yyvsp[(2) - (3)].node) == 0) {
                      (yyval.node) = NEW_ZARRAY(); /* zero length array*/
                    } else {
                      (yyval.node) = (yyvsp[(2) - (3)].node);
                    }
                  ;}
    break;

  case 283:
#line 1721 "grammar19.y"
    {
                    (yyval.node) = NEW_HASH((yyvsp[(2) - (3)].node));
                  ;}
    break;

  case 284:
#line 1725 "grammar19.y"
    {
                    (yyval.node) = NEW_RETURN(0);
                  ;}
    break;

  case 285:
#line 1729 "grammar19.y"
    {
                    (yyval.node) = new_yield((yyvsp[(3) - (4)].node));
                  ;}
    break;

  case 286:
#line 1733 "grammar19.y"
    {
                    (yyval.node) = NEW_YIELD(0, Qfalse);
                  ;}
    break;

  case 287:
#line 1737 "grammar19.y"
    {
                    (yyval.node) = NEW_YIELD(0, Qfalse);
                  ;}
    break;

  case 288:
#line 1740 "grammar19.y"
    {in_defined = 1;;}
    break;

  case 289:
#line 1741 "grammar19.y"
    {
                    in_defined = 0;
                    (yyval.node) = NEW_DEFINED((yyvsp[(5) - (6)].node));
                  ;}
    break;

  case 290:
#line 1746 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[(3) - (4)].node)), '!');
                  ;}
    break;

  case 291:
#line 1750 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond(NEW_NIL()), '!');
                  ;}
    break;

  case 292:
#line 1754 "grammar19.y"
    {
                    (yyvsp[(2) - (2)].node)->nd_iter = NEW_FCALL((yyvsp[(1) - (2)].id), 0);
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                    fixpos((yyvsp[(2) - (2)].node)->nd_iter, (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 294:
#line 1761 "grammar19.y"
    {
                    block_dup_check((yyvsp[(1) - (2)].node)->nd_args, (yyvsp[(2) - (2)].node));
                    (yyvsp[(2) - (2)].node)->nd_iter = (yyvsp[(1) - (2)].node);
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                    fixpos((yyval.node), (yyvsp[(1) - (2)].node));
                  ;}
    break;

  case 295:
#line 1768 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 296:
#line 1775 "grammar19.y"
    {
                    (yyval.node) = NEW_IF(cond((yyvsp[(2) - (6)].node)), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (6)].node));
                  ;}
    break;

  case 297:
#line 1783 "grammar19.y"
    {
                    (yyval.node) = NEW_UNLESS(cond((yyvsp[(2) - (6)].node)), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (6)].node));
                  ;}
    break;

  case 298:
#line 1787 "grammar19.y"
    {COND_PUSH(1);;}
    break;

  case 299:
#line 1787 "grammar19.y"
    {COND_POP();;}
    break;

  case 300:
#line 1790 "grammar19.y"
    {
                    (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (7)].node)), (yyvsp[(6) - (7)].node), 1);
                    fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                  ;}
    break;

  case 301:
#line 1794 "grammar19.y"
    {COND_PUSH(1);;}
    break;

  case 302:
#line 1794 "grammar19.y"
    {COND_POP();;}
    break;

  case 303:
#line 1797 "grammar19.y"
    {
                    (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (7)].node)), (yyvsp[(6) - (7)].node), 1);
                    fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                  ;}
    break;

  case 304:
#line 1804 "grammar19.y"
    {
                    (yyval.node) = NEW_CASE((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (5)].node));
                  ;}
    break;

  case 305:
#line 1809 "grammar19.y"
    {
                    (yyval.node) = NEW_CASE(0, (yyvsp[(3) - (4)].node));
                  ;}
    break;

  case 306:
#line 1813 "grammar19.y"
    {COND_PUSH(1);;}
    break;

  case 307:
#line 1815 "grammar19.y"
    {COND_POP();;}
    break;

  case 308:
#line 1818 "grammar19.y"
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
                    (yyval.node) = NEW_FOR((yyvsp[(2) - (9)].node), (yyvsp[(5) - (9)].node), (yyvsp[(8) - (9)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (9)].node));
                  ;}
    break;

  case 309:
#line 1832 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("class definition in method body");
                    class_nest++;
                    local_push(0);
                    (yyval.num) = ruby_sourceline;
                  ;}
    break;

  case 310:
#line 1841 "grammar19.y"
    {
                    (yyval.node) = NEW_CLASS((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(3) - (6)].node));
                    nd_set_line((yyval.node), (yyvsp[(4) - (6)].num));
                    local_pop();
                    class_nest--;
                  ;}
    break;

  case 311:
#line 1848 "grammar19.y"
    {
                    (yyval.num) = in_def;
                    in_def = 0;
                  ;}
    break;

  case 312:
#line 1853 "grammar19.y"
    {
                    (yyval.num) = in_single;
                    in_single = 0;
                    class_nest++;
                    local_push(0);
                  ;}
    break;

  case 313:
#line 1861 "grammar19.y"
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
#line 1870 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("module definition in method body");
                    class_nest++;
                    local_push(0);
                    (yyval.num) = ruby_sourceline;
                  ;}
    break;

  case 315:
#line 1879 "grammar19.y"
    {
                    (yyval.node) = NEW_MODULE((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
                    nd_set_line((yyval.node), (yyvsp[(3) - (5)].num));
                    local_pop();
                    class_nest--;
                  ;}
    break;

  case 316:
#line 1886 "grammar19.y"
    {
                    (yyval.id) = cur_mid;
                    cur_mid = (yyvsp[(2) - (2)].id);
                    in_def++;
                    local_push(0);
                  ;}
    break;

  case 317:
#line 1895 "grammar19.y"
    {
                    NODE* body = remove_begin((yyvsp[(5) - (6)].node));
                    (yyval.node) = NEW_DEFN((yyvsp[(2) - (6)].id), (yyvsp[(4) - (6)].node), body, NOEX_PRIVATE);
                    nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    local_pop();
                    in_def--;
                    cur_mid = (yyvsp[(3) - (6)].id);
                  ;}
    break;

  case 318:
#line 1903 "grammar19.y"
    {lex_state = EXPR_FNAME;;}
    break;

  case 319:
#line 1904 "grammar19.y"
    {
                    in_single++;
                    lex_state = EXPR_ENDFN; /* force for args */
                    local_push(0);
                  ;}
    break;

  case 320:
#line 1912 "grammar19.y"
    {
                    NODE* body = remove_begin((yyvsp[(8) - (9)].node));
                    (yyval.node) = NEW_DEFS((yyvsp[(2) - (9)].node), (yyvsp[(5) - (9)].id), (yyvsp[(7) - (9)].node), body);
                    nd_set_line((yyval.node), (yyvsp[(1) - (9)].num));
                    local_pop();
                    in_single--;
                  ;}
    break;

  case 321:
#line 1920 "grammar19.y"
    {
                    (yyval.node) = NEW_BREAK(0);
                  ;}
    break;

  case 322:
#line 1924 "grammar19.y"
    {
                    (yyval.node) = NEW_NEXT(0);
                  ;}
    break;

  case 323:
#line 1928 "grammar19.y"
    {
                    (yyval.node) = NEW_REDO();
                  ;}
    break;

  case 324:
#line 1932 "grammar19.y"
    {
                    (yyval.node) = NEW_RETRY();
                  ;}
    break;

  case 325:
#line 1938 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  ;}
    break;

  case 326:
#line 1946 "grammar19.y"
    {
                    token_info_push("begin");
                  ;}
    break;

  case 327:
#line 1952 "grammar19.y"
    {
                    token_info_push("if");
                  ;}
    break;

  case 328:
#line 1958 "grammar19.y"
    {
                    token_info_push("unless");
                  ;}
    break;

  case 329:
#line 1964 "grammar19.y"
    {
                    token_info_push("while");
                  ;}
    break;

  case 330:
#line 1970 "grammar19.y"
    {
                    token_info_push("until");
                  ;}
    break;

  case 331:
#line 1976 "grammar19.y"
    {
                    token_info_push("case");
                  ;}
    break;

  case 332:
#line 1982 "grammar19.y"
    {
                    token_info_push("for");
                  ;}
    break;

  case 333:
#line 1988 "grammar19.y"
    {
                    token_info_push("class");
                  ;}
    break;

  case 334:
#line 1994 "grammar19.y"
    {
                    token_info_push("module");
                  ;}
    break;

  case 335:
#line 2000 "grammar19.y"
    {
                    token_info_push("def");
                    (yyval.num) = ruby_sourceline;
                  ;}
    break;

  case 336:
#line 2007 "grammar19.y"
    {
                    token_info_pop("end");
                  ;}
    break;

  case 343:
#line 2025 "grammar19.y"
    {
                    (yyval.node) = NEW_IF(cond((yyvsp[(2) - (5)].node)), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (5)].node));
                  ;}
    break;

  case 345:
#line 2033 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 348:
#line 2043 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                  ;}
    break;

  case 349:
#line 2047 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 350:
#line 2053 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 351:
#line 2057 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 352:
#line 2063 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (1)].node), 0);
                  ;}
    break;

  case 353:
#line 2067 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(4) - (4)].id), 0);
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (4)].node), (yyval.node));
                  ;}
    break;

  case 354:
#line 2072 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(4) - (6)].id), 0);
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (6)].node), NEW_POSTARG((yyval.node), (yyvsp[(6) - (6)].node)));
                  ;}
    break;

  case 355:
#line 2077 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (3)].node), -1);
                  ;}
    break;

  case 356:
#line 2081 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (5)].node), NEW_POSTARG(-1, (yyvsp[(5) - (5)].node)));
                  ;}
    break;

  case 357:
#line 2085 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(2) - (2)].id), 0);
                    (yyval.node) = NEW_MASGN(0, (yyval.node));
                  ;}
    break;

  case 358:
#line 2090 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(2) - (4)].id), 0);
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG((yyval.node), (yyvsp[(4) - (4)].node)));
                  ;}
    break;

  case 359:
#line 2095 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, -1);
                  ;}
    break;

  case 360:
#line 2099 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG(-1, (yyvsp[(3) - (3)].node)));
                  ;}
    break;

  case 361:
#line 2105 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].id), 0, (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 362:
#line 2109 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (8)].node), (yyvsp[(3) - (8)].node), (yyvsp[(5) - (8)].id), (yyvsp[(7) - (8)].node), (yyvsp[(8) - (8)].id));
                  ;}
    break;

  case 363:
#line 2113 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), 0, 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 364:
#line 2117 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].node), 0, (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 365:
#line 2121 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (4)].node), 0, (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 366:
#line 2125 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (2)].node), 0, 1, 0, 0);
                  ;}
    break;

  case 367:
#line 2129 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), 0, (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 368:
#line 2133 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (2)].node), 0, 0, 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 369:
#line 2137 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 370:
#line 2141 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 371:
#line 2145 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (2)].node), 0, 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 372:
#line 2149 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (4)].node), 0, (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 373:
#line 2153 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[(1) - (2)].id), 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 374:
#line 2157 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[(1) - (4)].id), (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 375:
#line 2161 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, 0, 0, (yyvsp[(1) - (1)].id));
                  ;}
    break;

  case 377:
#line 2168 "grammar19.y"
    {
                    command_start = TRUE;
                  ;}
    break;

  case 378:
#line 2174 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 379:
#line 2178 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 380:
#line 2182 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (4)].node);
                  ;}
    break;

  case 382:
#line 2189 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 385:
#line 2199 "grammar19.y"
    {
                    new_bv(get_id((yyvsp[(1) - (1)].id)));
                  ;}
    break;

  case 386:
#line 2203 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 387:
#line 2208 "grammar19.y"
    {
                    // no dyn variables
                  ;}
    break;

  case 388:
#line 2211 "grammar19.y"
    {
                    (yyval.num) = lpar_beg;
                    lpar_beg = ++paren_nest;
                  ;}
    break;

  case 389:
#line 2217 "grammar19.y"
    {
                    lpar_beg = (yyvsp[(2) - (4)].num);
                    (yyval.node) = (yyvsp[(3) - (4)].node);
                    (yyval.node)->nd_body = NEW_SCOPE((yyvsp[(3) - (4)].node)->nd_head, (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 390:
#line 2225 "grammar19.y"
    {
                    (yyval.node) = NEW_LAMBDA((yyvsp[(2) - (4)].node));
                  ;}
    break;

  case 391:
#line 2229 "grammar19.y"
    {
                    (yyval.node) = NEW_LAMBDA((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 392:
#line 2235 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 393:
#line 2239 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 394:
#line 2245 "grammar19.y"
    {
                    (yyval.num) = ruby_sourceline;
                    reset_block();
                  ;}
    break;

  case 395:
#line 2250 "grammar19.y"
    {
                    (yyval.vars) = variables->block_vars;
                  ;}
    break;

  case 396:
#line 2255 "grammar19.y"
    {
                    /* TODO $$ = NEW_ITER($3, 0, extract_block_vars($5, * $<vars>4)); */
                    (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), extract_block_vars((yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                    nd_set_line((yyval.node), (yyvsp[(2) - (6)].num));
                  ;}
    break;

  case 397:
#line 2263 "grammar19.y"
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
#line 2274 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 399:
#line 2278 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 400:
#line 2284 "grammar19.y"
    {
                    (yyval.node) = NEW_FCALL((yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 401:
#line 2289 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 402:
#line 2294 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 403:
#line 2299 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), 0);
                  ;}
    break;

  case 404:
#line 2303 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (3)].node), rb_parser_sym("call"), (yyvsp[(3) - (3)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (3)].node));
                  ;}
    break;

  case 405:
#line 2308 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (3)].node), rb_parser_sym("call"), (yyvsp[(3) - (3)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (3)].node));
                  ;}
    break;

  case 406:
#line 2313 "grammar19.y"
    {
                    (yyval.node) = NEW_SUPER((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 407:
#line 2317 "grammar19.y"
    {
                    (yyval.node) = NEW_ZSUPER();
                  ;}
    break;

  case 408:
#line 2321 "grammar19.y"
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
#line 2332 "grammar19.y"
    {
                    (yyval.num) = ruby_sourceline;
                    reset_block();
                  ;}
    break;

  case 410:
#line 2336 "grammar19.y"
    { (yyval.vars) = variables->block_vars; ;}
    break;

  case 411:
#line 2338 "grammar19.y"
    {
                    /* TODO $$ = NEW_ITER($3, 0, extract_block_vars($5, * $<vars>4)); */
                    (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), extract_block_vars((yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                    nd_set_line((yyval.node), (yyvsp[(2) - (6)].num));
                  ;}
    break;

  case 412:
#line 2344 "grammar19.y"
    {
                    (yyval.num) = ruby_sourceline;
                    reset_block();
                  ;}
    break;

  case 413:
#line 2348 "grammar19.y"
    { (yyval.vars) = variables->block_vars; ;}
    break;

  case 414:
#line 2350 "grammar19.y"
    {
                    /* TODO $$ = NEW_ITER($3, 0, extract_block_vars($5, * $<vars>4)); */
                    (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), extract_block_vars((yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                    nd_set_line((yyval.node), (yyvsp[(2) - (6)].num));
                  ;}
    break;

  case 415:
#line 2360 "grammar19.y"
    {
                    (yyval.node) = NEW_WHEN((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                  ;}
    break;

  case 418:
#line 2372 "grammar19.y"
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
#line 2385 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 421:
#line 2389 "grammar19.y"
    {
                    if(!((yyval.node) = splat_array((yyvsp[(1) - (1)].node)))) (yyval.node) = (yyvsp[(1) - (1)].node);
                  ;}
    break;

  case 423:
#line 2396 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 425:
#line 2403 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 428:
#line 2411 "grammar19.y"
    {
                    (yyval.node) = NEW_LIT(QUID2SYM((yyvsp[(1) - (1)].id)));
                  ;}
    break;

  case 430:
#line 2418 "grammar19.y"
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
#line 2432 "grammar19.y"
    {
                    (yyval.node) = literal_concat((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 434:
#line 2438 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 435:
#line 2444 "grammar19.y"
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
#line 2466 "grammar19.y"
    {
                    // TODO
                    intptr_t options = (yyvsp[(3) - (3)].num);
                    NODE *node = (yyvsp[(2) - (3)].node);
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
#line 2497 "grammar19.y"
    {
                    (yyval.node) = NEW_ZARRAY();
                  ;}
    break;

  case 438:
#line 2501 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 439:
#line 2507 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 440:
#line 2511 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), evstr2dstr((yyvsp[(2) - (3)].node)));
                  ;}
    break;

  case 442:
#line 2518 "grammar19.y"
    {
                    (yyval.node) = literal_concat((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 443:
#line 2524 "grammar19.y"
    {
                    (yyval.node) = NEW_ZARRAY();
                  ;}
    break;

  case 444:
#line 2528 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 445:
#line 2534 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 446:
#line 2538 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                  ;}
    break;

  case 447:
#line 2544 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 448:
#line 2548 "grammar19.y"
    {
                    (yyval.node) = literal_concat((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 449:
#line 2554 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 450:
#line 2558 "grammar19.y"
    {
                    (yyval.node) = literal_concat((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 451:
#line 2564 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 452:
#line 2568 "grammar19.y"
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
                        head = list_append(NEW_DSTR(STR_NEW0()), head);
                        break;
                      }
                      (yyval.node) = list_append(head, tail);
                    }
                  ;}
    break;

  case 454:
#line 2592 "grammar19.y"
    {
                    (yyval.node) = lex_strterm;
                    lex_strterm = 0;
                    lex_state = EXPR_BEG;
                  ;}
    break;

  case 455:
#line 2598 "grammar19.y"
    {
                    lex_strterm = (yyvsp[(2) - (3)].node);
                    (yyval.node) = NEW_EVSTR((yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 456:
#line 2603 "grammar19.y"
    {
                    (yyvsp[(1) - (1)].val) = cond_stack;
                    (yyval.val) = cmdarg_stack;
                    cond_stack = 0;
                    cmdarg_stack = 0;
                  ;}
    break;

  case 457:
#line 2609 "grammar19.y"
    {
                    (yyval.node) = lex_strterm;
                    lex_strterm = 0;
                    lex_state = EXPR_BEG;
                  ;}
    break;

  case 458:
#line 2615 "grammar19.y"
    {
                    cond_stack = (yyvsp[(1) - (5)].val);
                    cmdarg_stack = (yyvsp[(2) - (5)].val);
                    lex_strterm = (yyvsp[(3) - (5)].node);

                    if((yyvsp[(4) - (5)].node)) (yyvsp[(4) - (5)].node)->flags &= ~NODE_FL_NEWLINE;
                    (yyval.node) = new_evstr((yyvsp[(4) - (5)].node));
                  ;}
    break;

  case 459:
#line 2625 "grammar19.y"
    {(yyval.node) = NEW_GVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 460:
#line 2626 "grammar19.y"
    {(yyval.node) = NEW_IVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 461:
#line 2627 "grammar19.y"
    {(yyval.node) = NEW_CVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 463:
#line 2632 "grammar19.y"
    {
                    lex_state = EXPR_END;
                    (yyval.id) = (yyvsp[(2) - (2)].id);
                  ;}
    break;

  case 468:
#line 2645 "grammar19.y"
    {
                    lex_state = EXPR_END;
                    if(!((yyval.node) = (yyvsp[(2) - (3)].node))) {
                      (yyval.node) = NEW_LIT(QUID2SYM(rb_parser_sym("")));
                    } else {
                      VALUE lit;

                      switch(nd_type((yyval.node))) {
                      case NODE_DSTR:
                        nd_set_type((yyval.node), NODE_DSYM);
                        break;
                      case NODE_STR:
                        lit = (yyval.node)->nd_lit;
                        // TODO: intern function that takes a String
                        // so the embedded \x00 or captured.
                        (yyval.node)->nd_lit = QUID2SYM(rb_parser_sym(RSTRING_PTR(lit)));
                        nd_set_type((yyval.node), NODE_LIT);
                        break;
                      default:
                        (yyval.node) = NEW_NODE(NODE_DSYM, STR_NEW0(), 1, NEW_LIST((yyval.node)));
                        break;
                      }
                    }
                  ;}
    break;

  case 471:
#line 2674 "grammar19.y"
    {
                    (yyval.node) = negate_lit((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 472:
#line 2678 "grammar19.y"
    {
                    (yyval.node) = negate_lit((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 478:
#line 2688 "grammar19.y"
    {(yyval.id) = keyword_nil;;}
    break;

  case 479:
#line 2689 "grammar19.y"
    {(yyval.id) = keyword_self;;}
    break;

  case 480:
#line 2690 "grammar19.y"
    {(yyval.id) = keyword_true;;}
    break;

  case 481:
#line 2691 "grammar19.y"
    {(yyval.id) = keyword_false;;}
    break;

  case 482:
#line 2692 "grammar19.y"
    {(yyval.id) = keyword__FILE__;;}
    break;

  case 483:
#line 2693 "grammar19.y"
    {(yyval.id) = keyword__LINE__;;}
    break;

  case 484:
#line 2694 "grammar19.y"
    {(yyval.id) = keyword__ENCODING__;;}
    break;

  case 485:
#line 2698 "grammar19.y"
    {
                    if(!((yyval.node) = gettable((yyvsp[(1) - (1)].id)))) {
                      (yyval.node) = NEW_BEGIN(0);
                    }
                  ;}
    break;

  case 486:
#line 2706 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                  ;}
    break;

  case 489:
#line 2716 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 490:
#line 2720 "grammar19.y"
    {
                    lex_state = EXPR_BEG;
                  ;}
    break;

  case 491:
#line 2724 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(3) - (4)].node);
                  ;}
    break;

  case 492:
#line 2728 "grammar19.y"
    {
                    yyerrok;
                    (yyval.node) = 0;
                  ;}
    break;

  case 493:
#line 2735 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                    lex_state = EXPR_BEG;
                    command_start = TRUE;
                  ;}
    break;

  case 494:
#line 2741 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                  ;}
    break;

  case 495:
#line 2747 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].id), 0, (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 496:
#line 2751 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (8)].node), (yyvsp[(3) - (8)].node), (yyvsp[(5) - (8)].id), (yyvsp[(7) - (8)].node), (yyvsp[(8) - (8)].id));
                  ;}
    break;

  case 497:
#line 2755 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), 0, 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 498:
#line 2759 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].node), 0, (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 499:
#line 2763 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (4)].node), 0, (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 500:
#line 2767 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), 0, (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 501:
#line 2771 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (2)].node), 0, 0, 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 502:
#line 2775 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 503:
#line 2779 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 504:
#line 2783 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (2)].node), 0, 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 505:
#line 2787 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (4)].node), 0, (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 506:
#line 2791 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[(1) - (2)].id), 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 507:
#line 2795 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[(1) - (4)].id), (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 508:
#line 2799 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, 0, 0, (yyvsp[(1) - (1)].id));
                  ;}
    break;

  case 509:
#line 2803 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, 0, 0, 0);
                  ;}
    break;

  case 510:
#line 2809 "grammar19.y"
    {
                    yy_error("formal argument cannot be a constant");
                    (yyval.id) = 0;
                  ;}
    break;

  case 511:
#line 2814 "grammar19.y"
    {
                    yy_error("formal argument cannot be an instance variable");
                    (yyval.id) = 0;
                  ;}
    break;

  case 512:
#line 2819 "grammar19.y"
    {
                    yy_error("formal argument cannot be a global variable");
                    (yyval.id) = 0;
                  ;}
    break;

  case 513:
#line 2824 "grammar19.y"
    {
                    yy_error("formal argument cannot be a class variable");
                    (yyval.id) = 0;
                  ;}
    break;

  case 515:
#line 2832 "grammar19.y"
    {
                    formal_argument(get_id((yyvsp[(1) - (1)].id)));
                    (yyval.id) = (yyvsp[(1) - (1)].id);
                  ;}
    break;

  case 516:
#line 2839 "grammar19.y"
    {
                    arg_var(get_id((yyvsp[(1) - (1)].id)));
                    (yyval.node) = NEW_ARGS_AUX((yyvsp[(1) - (1)].id), 1);
                  ;}
    break;

  case 517:
#line 2844 "grammar19.y"
    {
                    /* TODO */
                    QUID tid = internal_id();
                    arg_var(tid);
                    (yyvsp[(2) - (3)].node)->nd_value = NEW_LVAR(tid);
                    (yyval.node) = NEW_ARGS_AUX(tid, 1);
                    (yyval.node)->nd_next = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 519:
#line 2856 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                    (yyval.node)->nd_plen++;
                    (yyval.node)->nd_next = block_append((yyval.node)->nd_next, (yyvsp[(3) - (3)].node)->nd_next);
                  ;}
    break;

  case 520:
#line 2864 "grammar19.y"
    {
                    arg_var(formal_argument(get_id((yyvsp[(1) - (3)].id))));
                    (yyval.node) = assignable((yyvsp[(1) - (3)].id), (yyvsp[(3) - (3)].node));
                    (yyval.node) = NEW_OPT_ARG(0, (yyval.node));
                  ;}
    break;

  case 521:
#line 2872 "grammar19.y"
    {
                    arg_var(formal_argument(get_id((yyvsp[(1) - (3)].id))));
                    (yyval.node) = assignable((yyvsp[(1) - (3)].id), (yyvsp[(3) - (3)].node));
                    (yyval.node) = NEW_OPT_ARG(0, (yyval.node));
                  ;}
    break;

  case 522:
#line 2880 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                  ;}
    break;

  case 523:
#line 2884 "grammar19.y"
    {
                    NODE *opts = (yyvsp[(1) - (3)].node);
                    while(opts->nd_next) {
                      opts = opts->nd_next;
                    }
                    opts->nd_next = (yyvsp[(3) - (3)].node);
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 524:
#line 2895 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                  ;}
    break;

  case 525:
#line 2899 "grammar19.y"
    {
                    NODE *opts = (yyvsp[(1) - (3)].node);
                    while(opts->nd_next) {
                      opts = opts->nd_next;
                    }
                    opts->nd_next = (yyvsp[(3) - (3)].node);
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 528:
#line 2914 "grammar19.y"
    {
                    if(!is_local_id((yyvsp[(2) - (2)].id))) {
                      yy_error("rest argument must be local variable");
                    }
                    arg_var(shadowing_lvar(get_id((yyvsp[(2) - (2)].id))));
                    (yyval.id) = (yyvsp[(2) - (2)].id);
                  ;}
    break;

  case 529:
#line 2922 "grammar19.y"
    {
                    (yyval.id) = internal_id();
                    arg_var((yyval.id));
                  ;}
    break;

  case 532:
#line 2933 "grammar19.y"
    {
                    if(!is_local_id((yyvsp[(2) - (2)].id)))
                      yy_error("block argument must be local variable");
                    else if(local_id((yyvsp[(2) - (2)].id)))
                      yy_error("duplicate block argument name");
                    arg_var(shadowing_lvar(get_id((yyvsp[(2) - (2)].id))));
                    (yyval.id) = (yyvsp[(2) - (2)].id);
                  ;}
    break;

  case 533:
#line 2944 "grammar19.y"
    {
                    (yyval.id) = (yyvsp[(2) - (2)].id);
                  ;}
    break;

  case 534:
#line 2948 "grammar19.y"
    {
                    (yyval.id) = 0;
                  ;}
    break;

  case 535:
#line 2954 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  ;}
    break;

  case 536:
#line 2959 "grammar19.y"
    {lex_state = EXPR_BEG;;}
    break;

  case 537:
#line 2960 "grammar19.y"
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

  case 539:
#line 2985 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                  ;}
    break;

  case 541:
#line 2992 "grammar19.y"
    {
                    (yyval.node) = list_concat((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 542:
#line 2998 "grammar19.y"
    {
                    (yyval.node) = list_append(NEW_LIST((yyvsp[(1) - (3)].node)), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 543:
#line 3002 "grammar19.y"
    {
                    (yyval.node) = list_append(NEW_LIST(NEW_LIT(QUID2SYM((yyvsp[(1) - (2)].id)))), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 565:
#line 3046 "grammar19.y"
    {yyerrok;;}
    break;

  case 568:
#line 3051 "grammar19.y"
    {yyerrok;;}
    break;

  case 569:
#line 3054 "grammar19.y"
    {(yyval.node) = 0;;}
    break;


/* Line 1267 of yacc.c.  */
#line 8161 "grammar19.cpp"
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


#line 3056 "grammar19.y"


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
  ruby__end__seen = 0;
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

  return n;
}

static rb_encoding*
must_be_ascii_compatible(VALUE s)
{
  rb_encoding *enc = rb_enc_get(s);
  if(!rb_enc_asciicompat(enc)) {
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
  return rb_enc_str_new(beg, end - beg, enc);
}

static VALUE
lex_getline(rb_parser_state* parser_state)
{
  VALUE line = (*lex_gets)(parser_state, lex_input);
  if(NIL_P(line)) return line;
  must_be_ascii_compatible(line);

  return line;
}

VALUE process_parse_tree(rb_parser_state*, VALUE, NODE*, QUID*);

VALUE
string_to_ast(VALUE ptp, VALUE name, VALUE source, VALUE line)
{
  int n;
  int l = FIX2INT(line);
  VALUE ret;
  rb_parser_state* parser_state = parser_alloc_state();

  lex_input = source;
  lex_gets = lex_get_str;
  lex_gets_ptr = 0;
  processor = ptp;
  ruby_sourceline = l - 1;
  compile_for_eval = 1;

  n = yycompile(parser_state, RSTRING_PTR(name), l);

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

#define LEX_IO_BUFLEN  1024

static VALUE parse_io_gets(rb_parser_state* parser_state, VALUE s) {
  VALUE str = Qnil;

  while(true) {
    if(lex_io_total == 0) {
      lex_io_total = read(lex_io, lex_io_buf, LEX_IO_BUFLEN);

      if(lex_io_total < 1) {
        lex_io_total = 0;
        return str;
      } else {
        lex_io_index = 0;
      }
    }

    // TODO: encoding aware.
    for(int i = lex_io_index; i < lex_io_total; i++) {
      if(lex_io_buf[i] == '\n') {
        int len = i - lex_io_index + 1;

        if(str == Qnil) {
          str = rb_str_new(lex_io_buf + lex_io_index, len);
        } else {
          rb_str_cat(str, lex_io_buf + lex_io_index, len);
        }

        lex_io_count += len;
        if(i == lex_io_total - 1) {
          lex_io_total = 0;
        } else {
          lex_io_index = i + 1;
        }

        return str;
      }
    }

    str = rb_str_new(lex_io_buf + lex_io_index, lex_io_total - lex_io_index);
    lex_io_total = 0;
  }

  return Qnil;
}

VALUE
file_to_ast(VALUE ptp, const char *f, int fd, int start)
{
  int n;
  VALUE ret;
  rb_parser_state* parser_state = parser_alloc_state();

  lex_input = Qnil;
  lex_io = fd;
  lex_io_buf = (char*)malloc(LEX_IO_BUFLEN);
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
parser_str_new(const char *p, long n, rb_encoding *enc, int func, rb_encoding *enc0)
{
  VALUE str;

  str = rb_enc_str_new(p, n, enc);
  if(!(func & STR_FUNC_REGEXP) && rb_enc_asciicompat(enc)) {
    if(rb_enc_str_coderange(str) == ENC_CODERANGE_7BIT) {
      // Do nothing.
    } else if(enc0 == rb_usascii_encoding() && enc != rb_utf8_encoding()) {
      rb_enc_associate(str, rb_ascii8bit_encoding());
    }
  }

  return str;
}

#define lex_goto_eol(parser_state)  (lex_p = lex_pend)
#define peek(c) (lex_p < lex_pend && (c) == *lex_p)

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
      ruby_sourceline = heredoc_end;
      heredoc_end = 0;
    }

    ruby_sourceline++;
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
    ruby_sourceline = nd_line(quote);
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
                             STR_NEW(tok(), toklen()),  /* nd_lit */
                             (VALUE)len,                /* nd_nth */
                             (VALUE)lex_lastline);      /* nd_orig */
  nd_set_line(lex_strterm, ruby_sourceline);
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
  heredoc_end = ruby_sourceline;
  ruby_sourceline = nd_line(here);
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
        str = STR_NEW(p, pend - p);
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
    str = STR_NEW3(tok(), toklen(), enc, func);
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
  switch(c) {
  case '#':
    if(peek('!')) parser_state->has_shebang = 1;
    break;
  case 0xef:		/* UTF-8 BOM marker */
    if(lex_pend - lex_p >= 2 &&
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
    // TODO: encoding magic comments
    if(char* str = parse_comment(parser_state)) {
        int len = lex_pend - str - 1; // - 1 for the \n
        //cur_line = blk2bstr(str, len);
        //magic_comments->push_back(cur_line);
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
        for (;;) {
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
    }
    else if(c == '>') {
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
      arg_ambiguous();
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
      set_yylval_name(rb_parser_sym(tok()));
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
      set_yylval_name(rb_parser_sym(tok()));
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
            set_yylval_name(rb_parser_sym(kw->name));
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
      QUID ident = TOK_INTERN(!ENC_SINGLE(mb));

      set_yylval_name(ident);
      if(last_state != EXPR_DOT && is_local_id(ident) && lvar_defined(ident)) {
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

  nd = end->nd_head;
  switch(nd_type(nd)) {
  case NODE_RETURN:
  case NODE_BREAK:
  case NODE_NEXT:
  case NODE_REDO:
  case NODE_RETRY:
    if(verbose) {
      parser_warning(parser_state, nd, "statement not reached");
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
literal_concat0(rb_parser_state* parser_state, VALUE head, VALUE tail)
{
  if(NIL_P(tail)) return 1;
  if(!rb_enc_compatible(head, tail)) {
    rb_compile_error(parser_state, "string literal encodings differ (%s / %s)",
    rb_enc_name(rb_enc_get(head)),
    rb_enc_name(rb_enc_get(tail)));
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
      if(!literal_concat0(parser_state, head->nd_lit, tail->nd_lit)) {
      error:
        return 0;
      }
    } else {
      list_append(head, tail);
    }
    break;

  case NODE_DSTR:
    if(htype == NODE_STR) {
      if(!literal_concat0(parser_state, head->nd_lit, tail->nd_lit))
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
parser_gettable(rb_parser_state* parser_state, QUID id)
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
    return NEW_NUMBER(INT2FIX(ruby_sourceline));
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

static NODE *cond0(rb_parser_state* parser_state, NODE *node);

static NODE*
range_op(rb_parser_state* parser_state, NODE *node)
{
  if(node == 0) return 0;

  value_expr(node);
  if(nd_type(node) == NODE_LIT && FIXNUM_P(node->nd_lit)) {
    warn_unless_e_option(parser_state, node, "integer literal in conditional range");
    return NEW_CALL(node, tEQ, NEW_LIST(NEW_GVAR(rb_parser_sym("$."))));
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
    break;

  case NODE_DREGX:
  case NODE_DREGX_ONCE:
    local_cnt('_');
    local_cnt('~');
    return NEW_MATCH2(node, NEW_GVAR(rb_parser_sym("$_")));

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

static NODE*
negate_lit(NODE *node)
{
  switch(TYPE(node->nd_lit)) {
  case T_FIXNUM:
    node->nd_lit = LONG2FIX(-FIX2LONG(node->nd_lit));
    break;
  case T_BIGNUM:
    node->nd_lit = rb_funcall(node->nd_lit, tUMINUS, 0, 0);
    break;
  case T_FLOAT:
    node->nd_lit = rb_float_new(-NUM2DBL(node->nd_lit));
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
parser_local_id(rb_parser_state* parser_state, QUID id)
{
  if(variables->block_vars) {
    if(var_table_find_chained(variables->block_vars, id) >= 0) return 1;
  }

  if(var_table_find(variables->local_vars, id) >= 0) return 1;
  return 0;
}

static int
parser_arg_var(rb_parser_state* parser_state, QUID id)
{
  return var_table_add(variables->local_vars, id);
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


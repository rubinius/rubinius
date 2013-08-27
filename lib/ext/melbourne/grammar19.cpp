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
#line 399 "grammar19.y"
{
    VALUE val;
    NODE *node;
    ID id;
    int num;
    const struct vtable* vars;
}
/* Line 193 of yacc.c.  */
#line 706 "grammar19.cpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 719 "grammar19.cpp"

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
#define YYLAST   10432

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  138
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  174
/* YYNRULES -- Number of rules.  */
#define YYNRULES  571
/* YYNRULES -- Number of states.  */
#define YYNSTATES  985

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
      95,    97,   101,   105,   112,   118,   124,   130,   136,   140,
     144,   148,   152,   154,   158,   162,   164,   168,   172,   176,
     179,   181,   183,   185,   187,   189,   194,   199,   200,   206,
     209,   213,   218,   224,   229,   235,   238,   241,   244,   247,
     250,   252,   256,   258,   262,   264,   267,   271,   277,   280,
     285,   288,   293,   295,   299,   301,   305,   308,   312,   314,
     318,   320,   322,   327,   331,   335,   339,   343,   346,   348,
     350,   352,   357,   361,   365,   369,   373,   376,   378,   380,
     382,   385,   387,   391,   393,   395,   397,   399,   401,   403,
     405,   407,   409,   411,   412,   417,   419,   421,   423,   425,
     427,   429,   431,   433,   435,   437,   439,   441,   443,   445,
     447,   449,   451,   453,   455,   457,   459,   461,   463,   465,
     467,   469,   471,   473,   475,   477,   479,   481,   483,   485,
     487,   489,   491,   493,   495,   497,   499,   501,   503,   505,
     507,   509,   511,   513,   515,   517,   519,   521,   523,   525,
     527,   529,   531,   533,   535,   537,   539,   541,   543,   545,
     547,   549,   551,   553,   555,   557,   561,   567,   571,   577,
     584,   590,   596,   602,   608,   613,   617,   621,   625,   629,
     633,   637,   641,   645,   649,   654,   659,   662,   665,   669,
     673,   677,   681,   685,   689,   693,   697,   701,   705,   709,
     713,   717,   720,   723,   727,   731,   735,   739,   740,   745,
     752,   754,   756,   758,   761,   766,   769,   773,   775,   777,
     779,   781,   783,   786,   789,   794,   796,   797,   800,   803,
     806,   808,   810,   812,   815,   819,   824,   828,   833,   836,
     838,   840,   842,   844,   846,   848,   850,   852,   854,   855,
     860,   861,   866,   870,   874,   877,   881,   885,   887,   892,
     896,   898,   899,   906,   911,   915,   918,   920,   923,   926,
     933,   940,   941,   942,   950,   951,   952,   960,   966,   971,
     972,   973,   983,   984,   991,   992,   993,  1002,  1003,  1009,
    1010,  1017,  1018,  1019,  1029,  1031,  1033,  1035,  1037,  1039,
    1041,  1043,  1045,  1047,  1049,  1051,  1053,  1055,  1057,  1059,
    1061,  1063,  1065,  1068,  1070,  1072,  1074,  1080,  1082,  1085,
    1087,  1089,  1091,  1095,  1097,  1101,  1103,  1108,  1115,  1119,
    1125,  1128,  1133,  1135,  1139,  1146,  1155,  1160,  1167,  1172,
    1175,  1182,  1185,  1190,  1197,  1200,  1205,  1208,  1213,  1215,
    1217,  1219,  1223,  1225,  1230,  1232,  1235,  1237,  1241,  1243,
    1245,  1246,  1247,  1252,  1257,  1259,  1263,  1267,  1268,  1274,
    1277,  1282,  1287,  1290,  1295,  1300,  1304,  1308,  1312,  1315,
    1317,  1322,  1323,  1329,  1330,  1336,  1342,  1344,  1346,  1353,
    1355,  1357,  1359,  1361,  1364,  1366,  1369,  1371,  1373,  1375,
    1377,  1379,  1381,  1383,  1386,  1390,  1394,  1398,  1402,  1406,
    1407,  1411,  1413,  1416,  1420,  1424,  1425,  1429,  1430,  1433,
    1434,  1437,  1438,  1441,  1443,  1444,  1448,  1449,  1450,  1456,
    1458,  1460,  1462,  1464,  1467,  1469,  1471,  1473,  1475,  1479,
    1481,  1483,  1486,  1489,  1491,  1493,  1495,  1497,  1499,  1501,
    1503,  1505,  1507,  1509,  1511,  1513,  1515,  1517,  1519,  1521,
    1523,  1525,  1527,  1528,  1533,  1536,  1540,  1543,  1550,  1559,
    1564,  1571,  1576,  1583,  1586,  1591,  1598,  1601,  1606,  1609,
    1614,  1616,  1617,  1619,  1621,  1623,  1625,  1627,  1629,  1631,
    1635,  1637,  1641,  1645,  1649,  1651,  1655,  1657,  1661,  1663,
    1665,  1668,  1670,  1672,  1674,  1677,  1680,  1682,  1684,  1685,
    1690,  1692,  1695,  1697,  1701,  1705,  1708,  1710,  1712,  1714,
    1716,  1718,  1720,  1722,  1724,  1726,  1728,  1730,  1732,  1733,
    1735,  1736,  1738,  1741,  1744,  1745,  1747,  1749,  1751,  1753,
    1755,  1758
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     139,     0,    -1,    -1,   140,   141,    -1,   142,   304,    -1,
     311,    -1,   143,    -1,   142,   310,   143,    -1,     1,   143,
      -1,   148,    -1,    -1,    47,   144,   126,   141,   127,    -1,
     146,   246,   221,   249,    -1,   147,   304,    -1,   311,    -1,
     148,    -1,   147,   310,   148,    -1,     1,   148,    -1,    -1,
      45,   170,   149,   170,    -1,    45,    54,    54,    -1,    45,
      54,    64,    -1,    45,    54,    63,    -1,     6,   171,    -1,
     148,    40,   152,    -1,   148,    41,   152,    -1,   148,    42,
     152,    -1,   148,    43,   152,    -1,   148,    44,   148,    -1,
      48,   126,   146,   127,    -1,   150,    -1,   158,   109,   153,
      -1,   276,    87,   153,    -1,   206,   128,   181,   307,    87,
     153,    -1,   206,   129,    52,    87,   153,    -1,   206,   129,
      56,    87,   153,    -1,   206,    85,    56,    87,   153,    -1,
     206,    85,    52,    87,   153,    -1,   277,    87,   153,    -1,
     165,   109,   188,    -1,   158,   109,   177,    -1,   158,   109,
     188,    -1,   151,    -1,   165,   109,   153,    -1,   165,   109,
     150,    -1,   153,    -1,   151,    37,   151,    -1,   151,    38,
     151,    -1,    39,   305,   151,    -1,   123,   153,    -1,   175,
      -1,   151,    -1,   157,    -1,   154,    -1,   239,    -1,   239,
     129,   301,   183,    -1,   239,    85,   301,   183,    -1,    -1,
      94,   156,   227,   146,   127,    -1,   300,   183,    -1,   300,
     183,   155,    -1,   206,   129,   301,   183,    -1,   206,   129,
     301,   183,   155,    -1,   206,    85,   301,   183,    -1,   206,
      85,   301,   183,   155,    -1,    32,   183,    -1,    31,   183,
      -1,    30,   182,    -1,    21,   182,    -1,    22,   182,    -1,
     160,    -1,    89,   159,   306,    -1,   160,    -1,    89,   159,
     306,    -1,   162,    -1,   162,   161,    -1,   162,    95,   164,
      -1,   162,    95,   164,   130,   163,    -1,   162,    95,    -1,
     162,    95,   130,   163,    -1,    95,   164,    -1,    95,   164,
     130,   163,    -1,    95,    -1,    95,   130,   163,    -1,   164,
      -1,    89,   159,   306,    -1,   161,   130,    -1,   162,   161,
     130,    -1,   161,    -1,   163,   130,   161,    -1,   273,    -1,
     274,    -1,   206,   128,   181,   307,    -1,   206,   129,    52,
      -1,   206,    85,    52,    -1,   206,   129,    56,    -1,   206,
      85,    56,    -1,    86,    56,    -1,   277,    -1,   273,    -1,
     274,    -1,   206,   128,   181,   307,    -1,   206,   129,    52,
      -1,   206,    85,    52,    -1,   206,   129,    56,    -1,   206,
      85,    56,    -1,    86,    56,    -1,   277,    -1,    52,    -1,
      56,    -1,    86,   166,    -1,   166,    -1,   206,    85,   166,
      -1,    52,    -1,    56,    -1,    53,    -1,   173,    -1,   174,
      -1,   168,    -1,   269,    -1,   169,    -1,   271,    -1,   170,
      -1,    -1,   171,   130,   172,   170,    -1,   114,    -1,   115,
      -1,   116,    -1,    69,    -1,    70,    -1,    71,    -1,    77,
      -1,    78,    -1,   112,    -1,    73,    -1,   113,    -1,    74,
      -1,    72,    -1,    83,    -1,    84,    -1,   117,    -1,   118,
      -1,   119,    -1,    95,    -1,   120,    -1,   121,    -1,    68,
      -1,   123,    -1,   124,    -1,    66,    -1,    67,    -1,    81,
      -1,    82,    -1,   131,    -1,    49,    -1,    50,    -1,    51,
      -1,    47,    -1,    48,    -1,    45,    -1,    37,    -1,     7,
      -1,    21,    -1,    16,    -1,     3,    -1,     5,    -1,    46,
      -1,    26,    -1,    15,    -1,    14,    -1,    10,    -1,     9,
      -1,    36,    -1,    20,    -1,    25,    -1,     4,    -1,    22,
      -1,    34,    -1,    39,    -1,    38,    -1,    23,    -1,     8,
      -1,    24,    -1,    30,    -1,    33,    -1,    32,    -1,    13,
      -1,    35,    -1,     6,    -1,    17,    -1,    31,    -1,    11,
      -1,    12,    -1,    18,    -1,    19,    -1,   165,   109,   175,
      -1,   165,   109,   175,    44,   175,    -1,   276,    87,   175,
      -1,   276,    87,   175,    44,   175,    -1,   206,   128,   181,
     307,    87,   175,    -1,   206,   129,    52,    87,   175,    -1,
     206,   129,    56,    87,   175,    -1,   206,    85,    52,    87,
     175,    -1,   206,    85,    56,    87,   175,    -1,    86,    56,
      87,   175,    -1,   277,    87,   175,    -1,   175,    79,   175,
      -1,   175,    80,   175,    -1,   175,   117,   175,    -1,   175,
     118,   175,    -1,   175,   119,   175,    -1,   175,   120,   175,
      -1,   175,   121,   175,    -1,   175,    68,   175,    -1,   122,
      59,    68,   175,    -1,   122,    60,    68,   175,    -1,    66,
     175,    -1,    67,   175,    -1,   175,   114,   175,    -1,   175,
     115,   175,    -1,   175,   116,   175,    -1,   175,    69,   175,
      -1,   175,   112,   175,    -1,   175,    73,   175,    -1,   175,
     113,   175,    -1,   175,    74,   175,    -1,   175,    70,   175,
      -1,   175,    71,   175,    -1,   175,    72,   175,    -1,   175,
      77,   175,    -1,   175,    78,   175,    -1,   123,   175,    -1,
     124,   175,    -1,   175,    83,   175,    -1,   175,    84,   175,
      -1,   175,    75,   175,    -1,   175,    76,   175,    -1,    -1,
      46,   305,   176,   175,    -1,   175,   110,   175,   305,   111,
     175,    -1,   189,    -1,   175,    -1,   311,    -1,   187,   308,
      -1,   187,   130,   298,   308,    -1,   298,   308,    -1,   132,
     181,   306,    -1,   311,    -1,   179,    -1,   311,    -1,   182,
      -1,   157,    -1,   187,   186,    -1,   298,   186,    -1,   187,
     130,   298,   186,    -1,   185,    -1,    -1,   184,   182,    -1,
      96,   177,    -1,   130,   185,    -1,   130,    -1,   311,    -1,
     177,    -1,    95,   177,    -1,   187,   130,   177,    -1,   187,
     130,    95,   177,    -1,   187,   130,   177,    -1,   187,   130,
      95,   177,    -1,    95,   177,    -1,   250,    -1,   251,    -1,
     254,    -1,   255,    -1,   256,    -1,   259,    -1,   275,    -1,
     277,    -1,    53,    -1,    -1,   207,   190,   145,   217,    -1,
      -1,    90,   151,   191,   306,    -1,    89,   146,   133,    -1,
     206,    85,    56,    -1,    86,    56,    -1,    92,   178,   134,
      -1,    93,   297,   127,    -1,    30,    -1,    31,   132,   182,
     306,    -1,    31,   132,   306,    -1,    31,    -1,    -1,    46,
     305,   132,   192,   151,   306,    -1,    39,   132,   151,   306,
      -1,    39,   132,   306,    -1,   300,   241,    -1,   240,    -1,
     240,   241,    -1,    97,   232,    -1,   208,   152,   218,   146,
     220,   217,    -1,   209,   152,   218,   146,   221,   217,    -1,
      -1,    -1,   210,   193,   152,   219,   194,   146,   217,    -1,
      -1,    -1,   211,   195,   152,   219,   196,   146,   217,    -1,
     212,   152,   304,   244,   217,    -1,   212,   304,   244,   217,
      -1,    -1,    -1,   213,   222,    25,   197,   152,   219,   198,
     146,   217,    -1,    -1,   214,   167,   278,   199,   145,   217,
      -1,    -1,    -1,   214,    83,   151,   200,   309,   201,   145,
     217,    -1,    -1,   215,   167,   202,   145,   217,    -1,    -1,
     216,   168,   203,   280,   145,   217,    -1,    -1,    -1,   216,
     295,   303,   204,   168,   205,   280,   145,   217,    -1,    21,
      -1,    22,    -1,    23,    -1,    24,    -1,   189,    -1,     7,
      -1,    11,    -1,    12,    -1,    18,    -1,    19,    -1,    16,
      -1,    20,    -1,     3,    -1,     4,    -1,     5,    -1,    10,
      -1,   309,    -1,    13,    -1,   309,    13,    -1,   309,    -1,
      27,    -1,   221,    -1,    14,   152,   218,   146,   220,    -1,
     311,    -1,    15,   146,    -1,   165,    -1,   158,    -1,   283,
      -1,    89,   225,   306,    -1,   223,    -1,   224,   130,   223,
      -1,   224,    -1,   224,   130,    95,   283,    -1,   224,   130,
      95,   283,   130,   224,    -1,   224,   130,    95,    -1,   224,
     130,    95,   130,   224,    -1,    95,   283,    -1,    95,   283,
     130,   224,    -1,    95,    -1,    95,   130,   224,    -1,   285,
     130,   288,   130,   291,   294,    -1,   285,   130,   288,   130,
     291,   130,   285,   294,    -1,   285,   130,   288,   294,    -1,
     285,   130,   288,   130,   285,   294,    -1,   285,   130,   291,
     294,    -1,   285,   130,    -1,   285,   130,   291,   130,   285,
     294,    -1,   285,   294,    -1,   288,   130,   291,   294,    -1,
     288,   130,   291,   130,   285,   294,    -1,   288,   294,    -1,
     288,   130,   285,   294,    -1,   291,   294,    -1,   291,   130,
     285,   294,    -1,   293,    -1,   311,    -1,   228,    -1,   114,
     229,   114,    -1,    76,    -1,   114,   226,   229,   114,    -1,
     311,    -1,   135,   230,    -1,   231,    -1,   230,   130,   231,
      -1,    52,    -1,   282,    -1,    -1,    -1,   233,   234,   235,
     236,    -1,   132,   281,   229,   306,    -1,   281,    -1,   107,
     146,   127,    -1,    29,   146,    10,    -1,    -1,    28,   238,
     227,   146,    10,    -1,   157,   237,    -1,   239,   129,   301,
     180,    -1,   239,    85,   301,   180,    -1,   300,   179,    -1,
     206,   129,   301,   180,    -1,   206,    85,   301,   179,    -1,
     206,    85,   302,    -1,   206,   129,   179,    -1,   206,    85,
     179,    -1,    32,   179,    -1,    32,    -1,   206,   128,   181,
     307,    -1,    -1,   126,   242,   227,   146,   127,    -1,    -1,
      26,   243,   227,   146,    10,    -1,    17,   187,   218,   146,
     245,    -1,   221,    -1,   244,    -1,     8,   247,   248,   218,
     146,   246,    -1,   311,    -1,   177,    -1,   188,    -1,   311,
      -1,    88,   165,    -1,   311,    -1,     9,   146,    -1,   311,
      -1,   272,    -1,   269,    -1,   271,    -1,   252,    -1,    62,
      -1,   253,    -1,   252,   253,    -1,    99,   261,   106,    -1,
     100,   262,   106,    -1,   101,   263,    65,    -1,   102,   136,
     106,    -1,   102,   257,   106,    -1,    -1,   257,   258,   136,
      -1,   264,    -1,   258,   264,    -1,   103,   136,   106,    -1,
     103,   260,   106,    -1,    -1,   260,    61,   136,    -1,    -1,
     261,   264,    -1,    -1,   262,   264,    -1,    -1,   263,   264,
      -1,    61,    -1,    -1,   105,   265,   268,    -1,    -1,    -1,
     104,   266,   267,   146,   127,    -1,    54,    -1,    55,    -1,
      57,    -1,   277,    -1,    98,   270,    -1,   168,    -1,    55,
      -1,    54,    -1,    57,    -1,    98,   262,   106,    -1,    59,
      -1,    60,    -1,   122,    59,    -1,   122,    60,    -1,    52,
      -1,    55,    -1,    54,    -1,    56,    -1,    57,    -1,    34,
      -1,    33,    -1,    35,    -1,    36,    -1,    50,    -1,    49,
      -1,    51,    -1,   273,    -1,   274,    -1,   273,    -1,   274,
      -1,    63,    -1,    64,    -1,   309,    -1,    -1,   113,   279,
     152,   309,    -1,     1,   309,    -1,   132,   281,   306,    -1,
     281,   309,    -1,   285,   130,   289,   130,   291,   294,    -1,
     285,   130,   289,   130,   291,   130,   285,   294,    -1,   285,
     130,   289,   294,    -1,   285,   130,   289,   130,   285,   294,
      -1,   285,   130,   291,   294,    -1,   285,   130,   291,   130,
     285,   294,    -1,   285,   294,    -1,   289,   130,   291,   294,
      -1,   289,   130,   291,   130,   285,   294,    -1,   289,   294,
      -1,   289,   130,   285,   294,    -1,   291,   294,    -1,   291,
     130,   285,   294,    -1,   293,    -1,    -1,    56,    -1,    55,
      -1,    54,    -1,    57,    -1,   282,    -1,    52,    -1,   283,
      -1,    89,   225,   306,    -1,   284,    -1,   285,   130,   284,
      -1,    52,   109,   177,    -1,    52,   109,   206,    -1,   287,
      -1,   288,   130,   287,    -1,   286,    -1,   289,   130,   286,
      -1,   119,    -1,    95,    -1,   290,    52,    -1,   290,    -1,
     116,    -1,    96,    -1,   292,    52,    -1,   130,   293,    -1,
     311,    -1,   275,    -1,    -1,   132,   296,   151,   306,    -1,
     311,    -1,   298,   308,    -1,   299,    -1,   298,   130,   299,
      -1,   177,    88,   177,    -1,    58,   177,    -1,    52,    -1,
      56,    -1,    53,    -1,    52,    -1,    56,    -1,    53,    -1,
     173,    -1,    52,    -1,    53,    -1,   173,    -1,   129,    -1,
      85,    -1,    -1,   310,    -1,    -1,   137,    -1,   305,   133,
      -1,   305,   134,    -1,    -1,   137,    -1,   130,    -1,   135,
      -1,   137,    -1,   309,    -1,   310,   135,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   547,   547,   547,   572,   579,   583,   587,   591,   597,
     599,   598,   610,   633,   640,   644,   648,   652,   658,   658,
     662,   666,   673,   678,   682,   687,   692,   700,   708,   713,
     722,   723,   729,   751,   768,   781,   794,   799,   812,   817,
     822,   827,   832,   834,   839,   845,   846,   850,   854,   858,
     862,   865,   873,   874,   877,   878,   882,   889,   888,   903,
     908,   915,   920,   927,   932,   939,   944,   949,   953,   957,
     963,   964,   970,   971,   977,   981,   985,   989,   993,   997,
    1001,  1005,  1009,  1013,  1019,  1020,  1026,  1030,  1036,  1040,
    1046,  1050,  1054,  1058,  1062,  1066,  1070,  1076,  1082,  1089,
    1094,  1099,  1103,  1107,  1111,  1115,  1121,  1127,  1134,  1138,
    1141,  1145,  1149,  1155,  1156,  1157,  1158,  1163,  1170,  1171,
    1174,  1178,  1181,  1185,  1185,  1191,  1192,  1193,  1194,  1195,
    1196,  1197,  1198,  1199,  1200,  1201,  1202,  1203,  1204,  1205,
    1206,  1207,  1208,  1209,  1210,  1211,  1212,  1213,  1214,  1215,
    1216,  1217,  1218,  1219,  1222,  1222,  1222,  1223,  1223,  1224,
    1224,  1224,  1225,  1225,  1225,  1225,  1226,  1226,  1226,  1226,
    1227,  1227,  1227,  1228,  1228,  1228,  1228,  1229,  1229,  1229,
    1229,  1230,  1230,  1230,  1230,  1231,  1231,  1231,  1231,  1232,
    1232,  1232,  1232,  1233,  1233,  1236,  1241,  1247,  1269,  1292,
    1313,  1326,  1339,  1352,  1357,  1362,  1367,  1373,  1379,  1383,
    1387,  1391,  1395,  1399,  1403,  1407,  1411,  1415,  1419,  1423,
    1427,  1431,  1435,  1439,  1443,  1447,  1451,  1455,  1459,  1463,
    1473,  1477,  1481,  1485,  1489,  1493,  1497,  1501,  1501,  1506,
    1512,  1518,  1526,  1527,  1531,  1535,  1541,  1547,  1548,  1551,
    1552,  1555,  1560,  1564,  1569,  1574,  1577,  1577,  1589,  1595,
    1599,  1603,  1609,  1613,  1617,  1626,  1637,  1646,  1655,  1661,
    1662,  1663,  1664,  1665,  1666,  1667,  1668,  1669,  1674,  1673,
    1690,  1690,  1695,  1699,  1703,  1707,  1715,  1719,  1723,  1727,
    1731,  1735,  1735,  1740,  1744,  1748,  1754,  1755,  1762,  1766,
    1774,  1782,  1782,  1782,  1789,  1789,  1789,  1796,  1803,  1808,
    1810,  1807,  1827,  1826,  1843,  1848,  1842,  1865,  1864,  1881,
    1880,  1898,  1899,  1898,  1914,  1918,  1922,  1926,  1932,  1940,
    1946,  1952,  1958,  1964,  1970,  1976,  1982,  1988,  1994,  2001,
    2007,  2008,  2009,  2012,  2013,  2016,  2017,  2026,  2027,  2033,
    2034,  2037,  2041,  2047,  2051,  2057,  2061,  2066,  2071,  2075,
    2079,  2084,  2089,  2093,  2099,  2103,  2107,  2111,  2115,  2119,
    2123,  2127,  2131,  2135,  2139,  2143,  2147,  2151,  2155,  2161,
    2162,  2168,  2172,  2176,  2186,  2190,  2196,  2200,  2206,  2211,
    2217,  2220,  2217,  2234,  2238,  2244,  2248,  2255,  2254,  2269,
    2280,  2284,  2290,  2295,  2300,  2305,  2309,  2314,  2319,  2323,
    2327,  2339,  2338,  2351,  2350,  2364,  2372,  2373,  2376,  2388,
    2391,  2395,  2399,  2402,  2406,  2409,  2413,  2416,  2417,  2421,
    2424,  2436,  2437,  2438,  2444,  2450,  2472,  2532,  2536,  2543,
    2546,  2552,  2553,  2559,  2563,  2570,  2573,  2580,  2583,  2590,
    2593,  2600,  2603,  2626,  2628,  2627,  2639,  2645,  2638,  2661,
    2662,  2663,  2664,  2667,  2674,  2675,  2676,  2677,  2680,  2705,
    2706,  2707,  2711,  2717,  2718,  2719,  2720,  2721,  2724,  2725,
    2726,  2727,  2728,  2729,  2730,  2733,  2739,  2747,  2751,  2757,
    2758,  2761,  2766,  2765,  2773,  2780,  2786,  2794,  2798,  2802,
    2806,  2810,  2814,  2818,  2822,  2826,  2830,  2834,  2838,  2842,
    2846,  2851,  2856,  2861,  2866,  2871,  2878,  2879,  2886,  2891,
    2901,  2902,  2910,  2918,  2926,  2930,  2941,  2945,  2956,  2957,
    2960,  2968,  2975,  2976,  2979,  2990,  2994,  3000,  3006,  3006,
    3030,  3031,  3037,  3038,  3044,  3048,  3054,  3055,  3056,  3059,
    3060,  3061,  3062,  3065,  3066,  3067,  3070,  3071,  3074,  3075,
    3078,  3079,  3082,  3085,  3088,  3089,  3090,  3093,  3094,  3097,
    3098,  3101
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
  "top_stmt", "@2", "bodystmt", "compstmt", "stmts", "stmt", "@3",
  "command_asgn", "expr", "expr_value", "command_call", "block_command",
  "cmd_brace_block", "@4", "command", "mlhs", "mlhs_inner", "mlhs_basic",
  "mlhs_item", "mlhs_head", "mlhs_post", "mlhs_node", "lhs", "cname",
  "cpath", "fname", "fsym", "fitem", "undef_list", "@5", "op", "reswords",
  "arg", "@6", "arg_value", "aref_args", "paren_args", "opt_paren_args",
  "opt_call_args", "call_args", "command_args", "@7", "block_arg",
  "opt_block_arg", "args", "mrhs", "primary", "@8", "@9", "@10", "@11",
  "@12", "@13", "@14", "@15", "@16", "@17", "@18", "@19", "@20", "@21",
  "@22", "@23", "primary_value", "k_begin", "k_if", "k_unless", "k_while",
  "k_until", "k_case", "k_for", "k_class", "k_module", "k_def", "k_end",
  "then", "do", "if_tail", "opt_else", "for_var", "f_marg", "f_marg_list",
  "f_margs", "block_param", "opt_block_param", "block_param_def",
  "opt_bv_decl", "bv_decls", "bvar", "lambda", "@24", "@25", "f_larglist",
  "lambda_body", "do_block", "@26", "block_call", "method_call",
  "brace_block", "@27", "@28", "case_body", "cases", "opt_rescue",
  "exc_list", "exc_var", "opt_ensure", "literal", "strings", "string",
  "string1", "xstring", "regexp", "words", "word_list", "word", "qwords",
  "qword_list", "string_contents", "xstring_contents", "regexp_contents",
  "string_content", "@29", "@30", "@31", "string_dvar", "symbol", "sym",
  "dsym", "numeric", "user_variable", "keyword_variable", "var_ref",
  "var_lhs", "backref", "superclass", "@32", "f_arglist", "f_args",
  "f_bad_arg", "f_norm_arg", "f_arg_item", "f_arg", "f_opt", "f_block_opt",
  "f_block_optarg", "f_optarg", "restarg_mark", "f_rest_arg",
  "blkarg_mark", "f_block_arg", "opt_f_block_arg", "singleton", "@33",
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
     148,   148,   148,   150,   150,   151,   151,   151,   151,   151,
     151,   152,   153,   153,   154,   154,   154,   156,   155,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     158,   158,   159,   159,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   161,   161,   162,   162,   163,   163,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   166,   166,
     167,   167,   167,   168,   168,   168,   168,   168,   169,   169,
     170,   170,   171,   172,   171,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   176,   175,   175,
     175,   177,   178,   178,   178,   178,   179,   180,   180,   181,
     181,   182,   182,   182,   182,   182,   184,   183,   185,   186,
     186,   186,   187,   187,   187,   187,   188,   188,   188,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   190,   189,
     191,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   192,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   193,   194,   189,   195,   196,   189,   189,   189,   197,
     198,   189,   199,   189,   200,   201,   189,   202,   189,   203,
     189,   204,   205,   189,   189,   189,   189,   189,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   218,   218,   219,   219,   220,   220,   221,   221,   222,
     222,   223,   223,   224,   224,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   226,   226,   226,   226,   226,   226,
     226,   226,   226,   226,   226,   226,   226,   226,   226,   227,
     227,   228,   228,   228,   229,   229,   230,   230,   231,   231,
     233,   234,   232,   235,   235,   236,   236,   238,   237,   239,
     239,   239,   240,   240,   240,   240,   240,   240,   240,   240,
     240,   242,   241,   243,   241,   244,   245,   245,   246,   246,
     247,   247,   247,   248,   248,   249,   249,   250,   250,   250,
     251,   252,   252,   252,   253,   254,   255,   256,   256,   257,
     257,   258,   258,   259,   259,   260,   260,   261,   261,   262,
     262,   263,   263,   264,   265,   264,   266,   267,   264,   268,
     268,   268,   268,   269,   270,   270,   270,   270,   271,   272,
     272,   272,   272,   273,   273,   273,   273,   273,   274,   274,
     274,   274,   274,   274,   274,   275,   275,   276,   276,   277,
     277,   278,   279,   278,   278,   280,   280,   281,   281,   281,
     281,   281,   281,   281,   281,   281,   281,   281,   281,   281,
     281,   281,   282,   282,   282,   282,   283,   283,   284,   284,
     285,   285,   286,   287,   288,   288,   289,   289,   290,   290,
     291,   291,   292,   292,   293,   294,   294,   295,   296,   295,
     297,   297,   298,   298,   299,   299,   300,   300,   300,   301,
     301,   301,   301,   302,   302,   302,   303,   303,   304,   304,
     305,   305,   306,   307,   308,   308,   308,   309,   309,   310,
     310,   311
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     1,     1,     3,     2,     1,
       0,     5,     4,     2,     1,     1,     3,     2,     0,     4,
       3,     3,     3,     2,     3,     3,     3,     3,     3,     4,
       1,     3,     3,     6,     5,     5,     5,     5,     3,     3,
       3,     3,     1,     3,     3,     1,     3,     3,     3,     2,
       1,     1,     1,     1,     1,     4,     4,     0,     5,     2,
       3,     4,     5,     4,     5,     2,     2,     2,     2,     2,
       1,     3,     1,     3,     1,     2,     3,     5,     2,     4,
       2,     4,     1,     3,     1,     3,     2,     3,     1,     3,
       1,     1,     4,     3,     3,     3,     3,     2,     1,     1,
       1,     4,     3,     3,     3,     3,     2,     1,     1,     1,
       2,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     5,     3,     5,     6,
       5,     5,     5,     5,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     4,     4,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     3,     3,     3,     3,     0,     4,     6,
       1,     1,     1,     2,     4,     2,     3,     1,     1,     1,
       1,     1,     2,     2,     4,     1,     0,     2,     2,     2,
       1,     1,     1,     2,     3,     4,     3,     4,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     4,
       0,     4,     3,     3,     2,     3,     3,     1,     4,     3,
       1,     0,     6,     4,     3,     2,     1,     2,     2,     6,
       6,     0,     0,     7,     0,     0,     7,     5,     4,     0,
       0,     9,     0,     6,     0,     0,     8,     0,     5,     0,
       6,     0,     0,     9,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     5,     1,     2,     1,
       1,     1,     3,     1,     3,     1,     4,     6,     3,     5,
       2,     4,     1,     3,     6,     8,     4,     6,     4,     2,
       6,     2,     4,     6,     2,     4,     2,     4,     1,     1,
       1,     3,     1,     4,     1,     2,     1,     3,     1,     1,
       0,     0,     4,     4,     1,     3,     3,     0,     5,     2,
       4,     4,     2,     4,     4,     3,     3,     3,     2,     1,
       4,     0,     5,     0,     5,     5,     1,     1,     6,     1,
       1,     1,     1,     2,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     3,     3,     3,     0,
       3,     1,     2,     3,     3,     0,     3,     0,     2,     0,
       2,     0,     2,     1,     0,     3,     0,     0,     5,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     3,     1,
       1,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     4,     2,     3,     2,     6,     8,     4,
       6,     4,     6,     2,     4,     6,     2,     4,     2,     4,
       1,     0,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     3,     3,     3,     1,     3,     1,     3,     1,     1,
       2,     1,     1,     1,     2,     2,     1,     1,     0,     4,
       1,     2,     1,     3,     3,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     1,
       0,     1,     2,     2,     0,     1,     1,     1,     1,     1,
       2,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     0,     1,     0,   336,   337,   338,     0,   329,
     330,   331,   334,   332,   333,   335,   324,   325,   326,   327,
     287,   256,   256,   479,   478,   480,   481,   560,     0,   560,
      10,     0,   483,   482,   484,   473,   548,   475,   474,   476,
     477,   469,   470,   431,   489,   490,     0,     0,     0,     0,
       0,   571,   571,    82,   390,   449,   447,   449,   451,   439,
     445,     0,     0,     0,     3,   558,     6,     9,    30,    42,
      45,    53,    52,     0,    70,     0,    74,    84,     0,    50,
     240,     0,   278,     0,     0,   301,   304,   558,     0,     0,
       0,     0,    54,   296,   269,   270,   430,   432,   271,   272,
     273,   274,   428,   429,   427,   485,   486,   275,     0,   276,
     256,     5,     8,   164,   175,   165,   188,   161,   181,   171,
     170,   191,   192,   186,   169,   168,   163,   189,   193,   194,
     173,   162,   176,   180,   182,   174,   167,   183,   190,   185,
     184,   177,   187,   172,   160,   179,   178,   159,   166,   157,
     158,   154,   155,   156,   113,   115,   114,   149,   150,   146,
     128,   129,   130,   137,   134,   136,   131,   132,   151,   152,
     138,   139,   143,   133,   135,   125,   126,   127,   140,   141,
     142,   144,   145,   147,   148,   153,   118,   120,   122,    23,
     116,   117,   119,   121,     0,     0,     0,     0,     0,     0,
       0,   251,     0,   241,   262,    68,   255,   571,     0,   485,
     486,     0,   276,   571,   542,    69,    67,   560,    66,     0,
     571,   408,    65,   560,   561,     0,     0,    18,   237,     0,
       0,   324,   325,   287,   290,   409,   216,     0,     0,   217,
     284,     0,     0,     0,   558,    15,   560,    72,    14,   280,
       0,   564,   564,   242,     0,     0,   564,   540,   560,     0,
       0,     0,    80,   328,     0,    90,    91,    98,   298,   391,
     466,   465,   467,   464,     0,   463,     0,     0,     0,     0,
       0,     0,     0,   471,   472,    49,   231,   232,   567,   568,
       4,   569,   559,     0,     0,     0,     0,     0,     0,     0,
     397,   399,     0,    86,     0,    78,    75,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   571,     0,     0,    51,     0,
       0,     0,     0,   558,     0,   559,     0,   350,   349,     0,
       0,   485,   486,   276,   108,   109,     0,     0,   111,     0,
       0,   485,   486,   276,   317,   184,   177,   187,   172,   154,
     155,   156,   113,   114,   538,   319,   537,     0,     0,     0,
     413,   411,   297,   433,     0,     0,   402,    59,   295,   123,
     545,   284,   263,   258,     0,     0,   260,   252,   261,     0,
     571,     0,     0,     0,   260,   253,   560,     0,   289,   257,
     560,   250,   249,   560,   294,    48,    20,    22,    21,     0,
     291,     0,     0,     0,     0,     0,     0,    17,   560,   282,
      13,   559,    71,   560,   285,   566,   565,   243,   566,   245,
     286,   541,     0,    97,   471,   472,    88,    83,     0,     0,
     571,     0,   511,   453,   456,   454,   468,   450,   434,   448,
     435,   436,   452,   437,   438,     0,   441,   443,     0,   444,
       0,     0,   570,     7,    24,    25,    26,    27,    28,    46,
      47,   571,     0,    31,    40,     0,    41,   560,     0,    76,
      87,    44,    43,     0,   195,   262,    39,   213,   221,   226,
     227,   228,   223,   225,   235,   236,   229,   230,   206,   207,
     233,   234,   560,   222,   224,   218,   219,   220,   208,   209,
     210,   211,   212,   549,   554,   550,   555,   407,   256,   405,
     560,   549,   551,   550,   552,   406,   256,     0,   571,   341,
       0,   340,     0,     0,     0,     0,     0,     0,   284,     0,
     571,     0,   309,   314,   108,   109,   110,     0,   492,   312,
     491,     0,   571,     0,     0,     0,   511,   557,   556,   321,
     549,   550,   256,   256,   571,   571,    32,   197,    38,   205,
      57,    60,     0,   195,   544,     0,   264,   259,   571,   553,
     550,   560,   549,   550,   543,   288,   562,   246,   293,    19,
       0,   238,     0,    29,     0,   571,   204,    73,    16,   281,
     564,     0,    81,    94,    96,   560,   549,   550,   517,   514,
     513,   512,   515,     0,   529,   533,   532,   528,   511,     0,
     394,   516,   518,   520,   571,   526,   571,   531,   571,     0,
     510,   457,     0,   440,   442,   446,   214,   215,   382,   571,
       0,   380,   379,   268,     0,    85,    79,     0,     0,     0,
       0,     0,     0,   404,    63,     0,   410,     0,     0,   248,
     403,    61,   247,   339,   279,   571,   571,   419,   571,   342,
     571,   344,   302,   343,   305,     0,     0,   308,   553,   283,
     560,   549,   550,     0,     0,   494,     0,     0,   108,   109,
     112,   560,     0,   560,   511,     0,     0,     0,   401,    56,
     400,    55,     0,     0,     0,   571,   124,   265,   254,     0,
       0,   410,     0,     0,   560,    11,   244,    89,    92,     0,
     517,     0,   362,   353,   355,   560,   351,   571,     0,     0,
     392,     0,   503,   536,     0,   506,   530,     0,   508,   534,
       0,   459,   460,   461,   455,   462,   517,     0,   571,     0,
     571,   524,   571,   571,   378,   384,     0,     0,   266,    77,
     196,     0,    37,   202,    36,   203,    64,   563,     0,    34,
     200,    35,   201,    62,   420,   421,   571,   422,     0,   571,
     347,     0,     0,   345,     0,     0,     0,   307,     0,     0,
     410,     0,   315,     0,     0,   410,   318,   539,   560,     0,
     496,   322,     0,     0,   198,     0,     0,   292,   522,   560,
       0,   360,     0,   519,   560,     0,     0,   521,   571,   571,
     535,   571,   527,   571,   571,     0,     0,   388,   385,   386,
     389,     0,   381,   369,   371,     0,   374,     0,   376,   398,
     267,   239,    33,   199,     0,     0,   424,   348,     0,    12,
     426,     0,   299,   300,     0,     0,   264,   571,   310,     0,
     493,   313,   495,   320,   511,   414,   412,     0,   352,   363,
       0,   358,   354,   393,   396,   395,     0,   499,     0,   501,
       0,   507,     0,   504,   509,   458,     0,   523,     0,   383,
     571,   571,   571,   525,   571,   571,     0,   423,     0,    99,
     100,   107,     0,   425,     0,   303,   306,   416,   417,   415,
       0,     0,     0,    58,     0,   361,     0,   356,   571,   571,
     571,   571,   284,     0,   387,     0,   366,     0,   368,   375,
       0,   372,   377,   106,     0,   571,     0,   571,   571,     0,
     316,     0,   359,     0,   500,     0,   497,   502,   505,   553,
     283,   571,   571,   571,   571,   553,   105,   560,   549,   550,
     418,   346,   311,   323,   357,   571,   367,     0,   364,   370,
     373,   410,   498,   571,   365
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    64,    65,    66,   229,   537,   538,   244,
     245,   419,    68,    69,   339,    70,    71,   581,   715,    72,
      73,   246,    74,    75,    76,   447,    77,   202,   358,   359,
     186,   187,   188,   189,   582,   534,   191,    79,   421,   204,
     250,   527,   670,   410,   411,   218,   219,   206,   397,   207,
     486,    80,   337,   433,   600,   341,   795,   342,   796,   693,
     920,   697,   694,   869,   564,   566,   707,   874,   237,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,   674,
     540,   682,   792,   793,   350,   733,   734,   735,   758,   650,
     651,   759,   838,   839,   268,   269,   452,   629,   740,   301,
     481,    92,    93,   388,   575,   574,   547,   919,   676,   786,
     855,   859,    94,    95,    96,    97,    98,    99,   100,   280,
     465,   101,   282,   276,   274,   278,   457,   642,   641,   750,
     754,   102,   275,   103,   104,   209,   210,   107,   211,   212,
     559,   696,   705,   706,   631,   632,   633,   634,   635,   761,
     762,   636,   637,   638,   639,   830,   742,   377,   565,   255,
     213,   214,   110,   604,   529,   569,   290,   407,   408,   666,
     437,   541,   345,   248
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -766
static const yytype_int16 yypact[] =
{
    -766,    88,  2503,  -766,  6885,  -766,  -766,  -766,  6424,  -766,
    -766,  -766,  -766,  -766,  -766,  -766,  7103,  7103,  -766,  -766,
    7103,  3210,  2817,  -766,  -766,  -766,  -766,   237,  6295,   -17,
    -766,    29,  -766,  -766,  -766,  5544,  2948,  -766,  -766,  5668,
    -766,  -766,  -766,  -766,  -766,  -766,  8302,  8302,    93,  4339,
    8411,  7321,  7648,  6675,  -766,  6166,  -766,  -766,  -766,    43,
     126,    78,  8520,  8302,  -766,   143,  -766,   846,  -766,   371,
    -766,  -766,   137,   106,  -766,   212,  8629,  -766,   258,  2931,
     198,   283,  -766,  8411,  8411,  -766,  -766,  4962,  8734,  8839,
    8944,  5422,    14,    45,  -766,  -766,   277,  -766,  -766,  -766,
    -766,  -766,  -766,  -766,  -766,   253,   411,  -766,   307,   552,
      57,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,
    -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,
    -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,
    -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,
    -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,
    -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,
    -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,
    -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,   270,
    -766,  -766,  -766,  -766,   306,  8302,   388,  4474,  8302,  8302,
    8302,  -766,   337,  2931,   389,  -766,  -766,   355,   377,    27,
     168,   404,   173,   403,  -766,  -766,  -766,  4853,  -766,  7103,
    7103,  -766,  -766,  5071,  -766,  8411,   410,  -766,   405,   409,
    4609,  -766,  -766,  -766,   408,   413,  -766,   429,    57,   470,
     658,  6994,  4339,   416,   143,   846,   -17,   437,  -766,   371,
     420,   138,   201,  -766,   389,   423,   201,  -766,   -17,   509,
     369,  9049,   445,  -766,   440,   471,   513,   516,  -766,  -766,
    -766,  -766,  -766,  -766,   103,  -766,   364,   406,   384,   473,
     457,   474,    21,   523,   528,  -766,  -766,  -766,  -766,  -766,
    -766,  -766,  5180,  8411,  8411,  8411,  8411,  6994,  8411,  8411,
    -766,  -766,  7757,  -766,  4339,  6780,   447,  7757,  8302,  8302,
    8302,  8302,  8302,  8302,  8302,  8302,  8302,  8302,  8302,  8302,
    8302,  8302,  8302,  8302,  8302,  8302,  8302,  8302,  8302,  8302,
    8302,  8302,  8302,  8302,  2323,  7103,  9316,  3607,   371,    63,
      63,  8411,  8411,   143,   589,   472,   554,  -766,  -766,   580,
     590,    72,    76,    86,   375,   387,  8411,   426,  -766,    53,
     583,  -766,  -766,  -766,  -766,    33,    49,    62,   145,   160,
     179,   194,   276,   288,  -766,  -766,  -766,   303, 10192, 10192,
    -766,  -766,  -766,  -766,  8520,  8520,  -766,   514,  -766,  -766,
    -766,   272,  -766,  -766,  8302,  8302,  7212,  -766,  -766,  9389,
    7103,  9462,  8302,  8302,  7430,  -766,   -17,   484,  -766,  -766,
     -17,  -766,  -766,   120,  -766,  -766,  -766,  -766,  -766,  6424,
    -766,  8302,  3986,   495,  9389,  9462,  8302,   846,   -17,  -766,
    -766,  5289,   496,   -17,  -766,  7539,  -766,  -766,  7648,  -766,
    -766,  -766,   405,   592,  -766,  -766,  -766,   501,  9049,  9535,
    7103,  9608,   670,  -766,  -766,  -766,  -766,  -766,  -766,  -766,
    -766,  -766,  -766,  -766,  -766,   299,  -766,  -766,   497,  -766,
    8302,  8302,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,
    -766,   191,  8302,  -766,   518,   542,  -766,   -17,  9049,   545,
    -766,  -766,  -766,   534,  2163,  -766,  -766,   470,  3193,  3193,
    3193,  3193,  1174,  1174, 10311,  3062,  3193,  3193,  2253,  2253,
     613,   613, 10256,  1174,  1174,  1081,  1081,   537,   321,   321,
     470,   470,   470,  3341,  5914,  3472,  6036,  -766,   413,  -766,
     -17,   686,  -766,   709,  -766,  -766,  3079,   636,   671,  -766,
    3742,   672,  4121,    38,    38,   589,  7866,   636,    90,  9681,
    7103,  9754,  -766,   371,  -766,   592,  -766,   143,  -766,  -766,
    -766,  9827,  7103,  9900,  3607,  8411,   788,  -766,  -766,  -766,
    -766,  -766,  1895,  1895,   191,   191,  -766,  3319,  -766,  2931,
    -766,  -766,  6424,  3450,  -766,  8302,   389,  -766,   403,  5792,
    2653,   -17,   290,   305,  -766,  -766,  -766,  -766,  -766,  -766,
    8411,  2931,   560,  -766,   413,   413,  2931,    16,   846,  -766,
     201,  9049,   501,   395,    31,   -17,   286,   328,   579,  -766,
    -766,  -766,  -766,  1116,  -766,  -766,  -766,  -766,   939,    79,
    -766,  -766,  -766,  -766,   561,  -766,   564,   644,   570,   649,
    -766,  -766,   818,  -766,  -766,  -766,   470,   470,  -766,   796,
    4744,  -766,  -766,   572,  7975,  -766,   501,  9049,  8520,  8302,
     594,  8520,  8520,  -766,   514,   576,   724,  8520,  8520,  -766,
    -766,   514,  -766,  -766,  -766,  8084,   691,  -766,   469,  -766,
     691,  -766,  -766,  -766,  -766,   636,    96,  -766,    50,    71,
     -17,    94,   110,  8411,   143,  -766,  8411,  3607,   395,    31,
    -766,   -17,   636,   120,   939,  3607,   143,  6553,  -766,  -766,
    -766,  -766,  4744,  4609,  8302,   191,  -766,  -766,  -766,  8302,
    8302,   367,  8302,  8302,   120,  -766,  -766,  -766,   466,  8302,
    -766,  1116,   300,  -766,   587,   -17,  -766,   601,  4744,  4609,
    -766,   939,  -766,  -766,   939,  -766,  -766,   948,  -766,  -766,
    4609,  -766,  -766,  -766,  -766,  -766,   609,   975,   601,   629,
     617,  -766,   624,   625,  -766,  -766,   747,  8302,   632,   501,
    2931,  8302,  -766,  2931,  -766,  2931,  -766,  -766,  8520,  -766,
    2931,  -766,  2931,  -766,   518,  -766,   676,  -766,  4230,   761,
    -766,  8411,   636,  -766,   636,  4744,  4744,  -766,  8193,  3877,
     129,    38,  -766,   143,   636,  -766,  -766,  -766,   -17,   636,
    -766,  -766,   762,   650,  2931,  4609,  8302,  -766,  -766,   -17,
    1246,   646,  1217,  -766,   -17,   769,   656,  -766,   655,   661,
    -766,   664,  -766,   669,   664,   681,  9154,  -766,   673,  -766,
    -766,   705,  -766,  1050,  -766,  1050,  -766,   948,  -766,  -766,
     692,  2931,  -766,  2931,  9259,    63,  -766,  -766,  4744,  -766,
    -766,    63,  -766,  -766,   636,   636,  -766,   442,  -766,  3607,
    -766,  -766,  -766,  -766,   788,  -766,  -766,   696,  -766,   695,
    1246,   573,  -766,  -766,  -766,  -766,   939,  -766,   948,  -766,
     948,  -766,   948,  -766,  -766,  -766,   772,   653,   975,  -766,
     700,   704,   664,  -766,   706,   664,   779,  -766,   678,   471,
     513,   516,  3607,  -766,  3742,  -766,  -766,  -766,  -766,  -766,
    4744,   636,  3607,  -766,  1246,   695,  1246,   707,   664,   717,
     664,   664,  -766,  9973,  -766,  1050,  -766,   948,  -766,  -766,
     948,  -766,  -766,   592, 10046,  7103, 10119,   671,   469,   636,
    -766,   636,   695,  1246,  -766,   948,  -766,  -766,  -766,   723,
     725,   664,   719,   664,   664,   115,    31,   -17,    99,   100,
    -766,  -766,  -766,  -766,   695,   664,  -766,   948,  -766,  -766,
    -766,   116,  -766,   664,  -766
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -766,  -766,  -766,   436,  -766,    28,  -766,  -544,   -46,  -766,
      44,  -766,  -286,    23,    55,    22,  -766,  -479,  -766,    -7,
     774,  -137,    30,   -33,  -766,  -419,   -12,  1273,  -312,   773,
     -54,  -766,     8,  -766,  -766,    32,  -766,   758,  -766,   333,
    -766,    46,   -43,  -333,   128,    11,  -766,  -264,  -208,   -37,
    -300,     5,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,
    -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,     2,  -766,
    -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,  -766,   176,
    -302,  -510,   -84,  -606,  -766,  -750,  -758,   136,  -766,  -518,
    -766,  -445,  -766,   -29,  -766,  -766,  -766,  -766,  -766,  -766,
    -766,  -766,  -766,   777,  -766,  -766,  -521,  -766,   -71,  -766,
    -766,  -766,  -766,  -766,  -766,   784,  -766,  -766,  -766,  -766,
    -766,  -766,  -766,  -766,   836,  -766,  -178,  -766,  -766,  -766,
    -766,     3,  -766,     7,  -766,   933,   962,   803,  1387,  1352,
    -766,  -766,    35,  -389,  -704,  -490,  -713,   394,  -705,  -765,
      52,   155,  -766,  -572,  -766,  -435,   285,  -766,  -766,  -766,
     -25,  -148,  2030,  -275,  -766,  -766,   -40,    -4,    89,  -549,
    -240,   -57,   -21,    -2
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -572
static const yytype_int16 yytable[] =
{
     111,   273,   530,   243,    81,   405,    81,   496,   291,   201,
     201,   192,   439,   201,   251,   193,   441,   640,   208,   208,
     702,   491,   208,   225,   685,   228,   252,   256,   827,   612,
     291,   192,   112,   222,   684,   193,   227,   375,   542,   832,
     190,   262,   721,   306,   292,   556,    67,   344,    67,   253,
     257,    81,   208,   840,   557,   264,   712,   713,   263,   528,
     190,   536,   879,   630,   208,   681,   728,   591,   221,   656,
     789,   380,   882,   249,   794,  -103,   539,   763,   264,   247,
     903,   263,   468,   380,   285,   208,   208,   190,     3,   208,
     349,   360,   360,   263,   263,   263,  -105,   -99,   459,   378,
     462,  -100,   466,   572,   573,   428,   338,   338,   738,   539,
     338,  -107,  -102,  -104,  -487,  -106,  -283,   615,  -479,  -102,
     224,   387,   925,   190,   528,   -71,   536,   469,  -103,  -101,
     827,   640,   587,   736,  -478,  -104,   -99,   283,   284,   340,
     587,   800,   343,   379,   205,   215,   -85,  -480,   216,   240,
     605,   243,   805,   804,  -101,   230,   386,   298,   299,  -283,
    -283,   809,  -479,  -550,   453,   300,   558,   487,   952,   829,
     903,   381,   833,   288,   882,   289,   605,   827,  -478,   279,
     -94,   832,  -549,   381,   423,   776,   739,   291,   288,   220,
     289,  -480,   783,   640,   840,   974,   243,   815,   288,    81,
     289,   -96,   -90,  -550,   430,   398,   -91,   454,   455,   456,
     201,   398,   201,   201,   764,   302,   -98,   690,   412,   208,
     -97,   208,   208,   431,   -93,   208,   798,   208,   446,   701,
    -481,   288,    81,   289,  -102,  -104,  -102,  -104,   769,   737,
     -95,   736,   821,    81,    81,  -483,   413,  -549,   415,   700,
    -103,  -101,  -103,  -101,   442,  -488,   594,   224,   243,   -92,
     403,   917,   281,   264,  -482,   485,   263,   648,   435,   640,
     485,   901,   247,   904,  -481,   436,   605,  -100,   288,  -484,
     289,   221,  -107,  -328,   386,   427,   291,   644,   605,  -483,
     594,   868,   824,   489,    81,   208,   208,   208,   208,    81,
     208,   208,   560,   545,   208,   649,    81,   264,  -482,   208,
     263,   -93,   414,   841,   929,   808,   338,   338,   338,   338,
     473,   479,   480,  -484,   483,   921,  -328,  -328,   201,   492,
     736,   438,   736,   412,   247,   432,    67,   208,   436,    81,
    -487,   478,   303,   208,   208,   406,   918,   409,   474,   475,
     476,   477,   730,   -95,   619,   620,   621,   622,   208,   426,
     453,  -473,   -99,   962,   338,   338,   526,   307,   334,   223,
     726,   588,   491,  -476,   224,   785,    56,   722,   951,   553,
     718,  -106,   535,   -90,   799,   254,   208,   208,   567,   308,
     736,   927,   723,   201,   384,   -93,   543,   544,   412,  -102,
     389,  -546,   208,   454,   455,  -473,   576,   578,   298,   299,
     610,   335,   336,  -547,  -104,   446,   -93,  -476,   981,   -93,
     111,  -553,   192,   -93,    81,   453,   193,   599,   444,   445,
     820,   526,   568,    81,   736,   643,   736,   -95,   223,   640,
     331,   332,   333,   201,   391,   453,   394,   535,   412,   461,
     264,   190,   208,   263,   816,   446,   526,   788,   -95,   546,
    -473,   -95,   399,   736,   416,   -95,    67,   453,   454,   455,
     458,   535,  -476,   417,   418,   608,  -101,   395,   554,   652,
    -553,   526,   555,   791,   788,   396,   683,   683,   454,   455,
     264,   402,  -410,   263,   678,   595,   680,   535,  -488,   597,
     695,  -546,   598,  -473,  -473,   400,   401,  -546,   660,   686,
     454,   455,   460,  -547,   424,  -476,  -476,   607,   453,  -547,
    -100,  -553,   609,  -553,  -553,   449,   665,  -549,   390,   708,
     710,   392,   393,   404,   672,   422,   677,   420,   308,   664,
     217,   -91,    81,   201,    81,   220,   -70,   671,   412,   429,
     440,  -410,   208,   912,   434,   201,  -485,   400,   425,   914,
     412,   454,   455,   464,   208,   443,    81,   208,   450,   451,
     672,   672,   652,   652,   663,   448,   655,   490,   727,   463,
     467,   526,   669,   709,   711,   192,   398,   665,   703,   193,
     716,   470,  -410,   526,  -410,  -410,   471,   535,  -486,  -485,
    -485,  -276,   208,   672,   766,   308,   546,   472,   580,   535,
     548,   665,   967,   264,   190,   552,   263,   596,   669,   669,
     321,   322,   603,   724,   446,   730,   -85,   619,   620,   621,
     622,   611,   743,   645,   743,   484,   743,   802,   485,   385,
     495,  -486,  -486,   658,  -276,  -276,   673,   765,  -262,   810,
     663,   669,    81,   811,   329,   330,   331,   332,   333,   264,
     208,  -107,   263,   208,   208,   549,   812,   813,   561,   208,
     208,   605,   654,   787,   790,   657,   790,  -284,   790,   675,
     492,   308,   -98,   772,   774,   679,   665,   725,   729,   779,
     781,   741,   825,   826,   744,   208,   746,   665,   208,    81,
     747,   749,  -263,   926,   835,   771,   788,    81,   550,   551,
     777,   562,   563,   652,    81,    81,   338,   822,   836,   338,
    -284,  -284,   618,   687,   619,   620,   621,   622,   584,   586,
     329,   330,   331,   332,   333,   765,   757,   254,   933,   190,
      81,    81,   857,   842,   683,   426,   870,   843,   801,   864,
     865,   803,    81,   867,   845,   847,   765,   849,   743,   623,
     743,   743,  -264,   944,   854,   624,   625,  -106,   586,   877,
     858,   254,   875,   667,   203,   203,   880,   876,   203,   884,
     208,   562,   563,   885,   856,   886,   626,   860,   -97,   627,
      81,   888,   807,   208,   890,  -102,   668,    81,    81,   892,
     852,    81,   628,   898,   236,   239,   945,   946,   895,   203,
     203,   778,   913,   817,   338,   653,   -93,    81,  -104,   899,
     286,   287,  -265,   923,   823,   924,   743,   743,   932,   743,
     935,   743,   743,  -101,   937,   943,   940,   953,   897,   -95,
     618,   263,   619,   620,   621,   622,   861,   955,   756,   977,
     619,   620,   621,   622,   -92,  -549,   908,  -550,   602,   263,
      81,   797,   347,   364,   971,   790,   947,   819,   948,   934,
     382,    81,   751,   752,   949,   753,   970,   623,   806,   495,
     383,    44,    45,   624,   625,   623,   293,   294,   295,   296,
     297,   624,   625,   277,   376,   900,   828,   872,   743,   743,
     743,     0,   743,   743,   626,     0,     0,   627,   878,   922,
       0,     0,   626,   883,    81,   627,    81,     0,   717,     0,
     704,   745,    81,   748,    81,     0,   743,   743,   743,   743,
       0,   757,     0,     0,     0,   105,     0,   105,   201,     0,
       0,     0,     0,   412,     0,   677,   790,   208,     0,     0,
       0,     0,     0,   203,     0,     0,   203,   203,   286,   743,
     743,   743,   743,   665,   106,   526,   106,     0,   862,     0,
     863,     0,     0,   743,     0,   203,   526,   203,   203,     0,
     871,   743,   105,     0,     0,   873,   265,   768,     0,     0,
       0,   618,   535,   619,   620,   621,   622,     0,     0,     0,
     730,     0,   619,   620,   621,   622,     0,     0,   784,   265,
       0,   106,     0,     0,     0,   266,     0,     0,     0,     0,
       0,   351,   361,   361,   361,     0,     0,   837,   623,   619,
     620,   621,   622,     0,   624,   625,     0,   623,   266,     0,
     915,   916,     0,   760,   625,   844,     0,   846,   848,     0,
     352,   362,   362,   362,     0,   626,     0,     0,   627,     0,
     203,     0,   818,     0,   626,   494,   497,   498,   499,   500,
     501,   502,   503,   504,   505,   506,   507,   508,   509,   510,
     511,   512,   513,   514,   515,   516,   517,   518,   519,   520,
     521,   522,     0,   203,     0,     0,     0,   950,     0,     0,
     850,     0,   756,     0,   619,   620,   621,   622,     0,     0,
       0,     0,     0,   887,   889,     0,   891,     0,   893,   894,
       0,     0,     0,     0,     0,   972,     0,   973,     0,     0,
     105,   866,     0,     0,     0,     0,     0,     0,   831,   623,
       0,   834,   577,   579,     0,   624,   625,     0,     0,   308,
       0,     0,   583,   203,   203,     0,     0,     0,   203,   106,
     577,   579,   203,   105,   321,   322,   626,     0,   730,   627,
     619,   620,   621,   622,   105,   105,     0,     0,     0,   601,
       0,     0,     0,     0,   606,   936,   938,   939,     0,   941,
     942,     0,   106,   203,   265,     0,   203,   328,   329,   330,
     331,   332,   333,   106,   106,   731,     0,     0,   203,     0,
       0,   732,     0,   954,   956,   957,   958,     0,     0,     0,
       0,     0,     0,   266,     0,   105,     0,     0,   646,   647,
     105,     0,     0,     0,     0,     0,     0,   105,   265,   902,
     203,   905,   308,     0,     0,     0,   976,   978,   979,   980,
       0,     0,     0,     0,   106,     0,     0,   321,   322,   106,
     982,     0,     0,     0,     0,     0,   106,   266,   984,   730,
     105,   619,   620,   621,   622,    78,     0,    78,     0,     0,
     928,     0,   930,     0,     0,     0,   931,     0,   326,   327,
     328,   329,   330,   331,   332,   333,     0,     0,   730,   106,
     619,   620,   621,   622,   203,     0,   731,     0,   203,     0,
       0,     0,   881,     0,     0,     0,     0,     0,     0,     0,
     203,     0,    78,     0,     0,     0,     0,     0,     0,   961,
       0,   963,     0,     0,   964,   731,     0,     0,     0,     0,
       0,     0,     0,   203,     0,     0,     0,     0,     0,   975,
       0,     0,     0,     0,   109,   105,   109,     0,     0,     0,
       0,   348,     0,     0,   105,     0,     0,     0,     0,     0,
       0,   983,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   265,     0,     0,   106,     0,     0,     0,     0,   108,
       0,   108,     0,   106,     0,     0,     0,     0,     0,     0,
       0,   109,     0,     0,     0,   267,     0,     0,     0,     0,
     266,     0,   203,     0,     0,     0,   583,   770,     0,   773,
     775,   265,     0,     0,     0,   780,   782,     0,   267,     0,
       0,     0,     0,   203,     0,     0,   108,     0,     0,     0,
     353,   363,   363,     0,     0,     0,     0,     0,     0,     0,
     266,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      78,     0,   814,   105,     0,   105,     0,   773,   775,     0,
     780,   782,     0,     0,     0,     0,     0,   203,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   105,     0,     0,
       0,     0,   106,    78,   106,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    78,    78,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   203,   106,     0,     0,   851,
       0,     0,     0,     0,     0,     0,   853,     0,     0,     0,
       0,     0,     0,     0,   265,     0,     0,     0,     0,   109,
       0,     0,     0,     0,     0,     0,   203,     0,     0,     0,
       0,     0,     0,     0,     0,    78,     0,     0,     0,     0,
      78,     0,     0,   266,   853,     0,     0,    78,     0,     0,
     493,     0,   109,   105,   108,     0,     0,     0,     0,     0,
     265,     0,     0,   109,   109,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      78,     0,   106,   267,     0,     0,     0,   108,     0,   266,
       0,     0,     0,     0,     0,     0,     0,     0,   108,   108,
     105,     0,     0,     0,     0,     0,     0,     0,   105,     0,
       0,     0,     0,     0,   109,   105,   105,     0,     0,   109,
       0,     0,     0,     0,     0,     0,   109,   267,     0,   106,
       0,     0,     0,     0,     0,     0,     0,   106,     0,     0,
       0,   105,   105,     0,   106,   106,     0,     0,     0,   108,
       0,     0,     0,   105,   108,     0,     0,     0,     0,   109,
       0,   108,     0,     0,     0,    78,     0,     0,     0,     0,
     106,   106,     0,   203,    78,     0,     0,     0,     0,     0,
       0,     0,   106,     0,     0,     0,     0,     0,     0,     0,
       0,   105,     0,     0,   108,     0,     0,     0,   105,   105,
       0,     0,   105,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   105,     0,
     106,     0,     0,     0,     0,     0,     0,   106,   106,     0,
       0,   106,     0,     0,     0,     0,     0,     0,     0,   361,
       0,     0,     0,     0,   109,     0,     0,   106,     0,     0,
       0,     0,     0,   109,     0,     0,     0,   909,     0,     0,
       0,   105,     0,     0,     0,     0,     0,     0,   362,     0,
     267,     0,   105,     0,     0,     0,     0,     0,     0,   108,
       0,     0,     0,    78,     0,    78,   910,     0,   108,     0,
     106,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   106,     0,     0,     0,     0,     0,    78,     0,     0,
     267,     0,     0,     0,     0,   105,     0,   105,     0,     0,
       0,     0,     0,   105,     0,   105,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   106,     0,   106,     0,     0,     0,
       0,     0,   106,     0,   106,     0,     0,     0,     0,     0,
       0,     0,   109,     0,   109,  -571,     0,     0,     0,     0,
       0,     0,     0,  -571,  -571,  -571,     0,     0,  -571,  -571,
    -571,     0,  -571,     0,     0,     0,   109,     0,     0,     0,
       0,     0,  -571,    78,     0,     0,     0,   108,     0,   108,
       0,   493,  -571,  -571,     0,  -571,  -571,  -571,  -571,  -571,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   108,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   267,     0,     0,     0,     0,     0,     0,
      78,     0,     0,     0,     0,     0,     0,     0,    78,     0,
    -571,     0,     0,     0,     0,    78,    78,     0,     0,     0,
       0,     0,     0,     0,   755,     0,     0,     0,     0,     0,
       0,     0,   109,     0,     0,     0,     0,     0,     0,   267,
       0,    78,    78,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -571,    78,  -571,     0,     0,   220,  -571,     0,
    -571,     0,  -571,     0,     0,     0,     0,   108,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   109,
       0,     0,     0,     0,     0,     0,     0,   109,     0,     0,
       0,    78,     0,     0,   109,   109,     0,     0,    78,    78,
       0,     0,    78,     0,     0,     0,   238,   238,     0,     0,
       0,   238,   238,   238,   108,     0,     0,     0,    78,     0,
     109,   109,   108,   238,     0,     0,     0,     0,     0,   108,
     108,     0,   109,     0,     0,     0,   238,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   238,   238,
     238,     0,     0,     0,     0,   108,   108,   907,     0,     0,
       0,    78,     0,     0,     0,     0,     0,   108,     0,     0,
     109,     0,    78,     0,     0,     0,     0,   109,   109,     0,
       0,   109,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   109,     0,     0,
       0,     0,     0,     0,     0,   108,     0,     0,     0,     0,
       0,     0,   108,   108,     0,    78,   108,    78,   363,     0,
       0,     0,     0,    78,     0,    78,     0,     0,     0,     0,
       0,     0,   108,     0,     0,     0,   911,   659,     0,     0,
     109,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   109,     0,     0,     0,   238,     0,     0,   238,   238,
     238,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   318,   319,   320,     0,   108,   321,   322,     0,     0,
       0,     0,     0,     0,     0,     0,   108,     0,     0,     0,
       0,     0,     0,     0,   109,     0,   109,     0,     0,     0,
       0,     0,   109,   323,   109,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,     0,     0,     0,     0,     0,
       0,   238,     0,  -241,     0,     0,     0,     0,     0,   108,
       0,   108,     0,     0,     0,     0,     0,   108,     0,   108,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   318,  -572,  -572,     0,   238,   321,   322,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,     0,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   523,   524,     0,     0,   525,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   157,
     158,   159,   160,   161,   162,   163,   164,   165,     0,     0,
     166,   167,     0,     0,   168,   169,   170,   171,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   172,     0,
       0,     0,     0,     0,   238,   238,   238,     0,     0,     0,
       0,     0,   238,   238,   238,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,     0,   183,   184,     0,     0,
       0,   238,     0,     0,   185,   220,   238,     0,     0,     0,
       0,     0,     0,     0,     0,   238,     0,     0,   238,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   238,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     238,   238,     0,  -571,     4,     0,     5,     6,     7,     8,
       9,     0,   238,     0,    10,    11,     0,     0,   238,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,     0,    41,    42,     0,    43,    44,    45,     0,    46,
      47,     0,     0,     0,     0,     0,   238,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,    49,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     0,     0,     0,     0,   238,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    61,    62,    63,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -571,     0,
    -571,   238,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -283,     0,     0,     0,     0,     0,     0,
       0,  -283,  -283,  -283,     0,     0,  -283,  -283,  -283,     0,
    -283,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -283,  -283,  -283,     0,   238,     0,     0,   238,     0,   238,
    -283,  -283,     0,  -283,  -283,  -283,  -283,  -283,     0,     0,
       0,     0,     0,     0,     0,   238,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,  -283,  -283,     0,     0,  -283,  -283,  -283,     0,
     720,  -283,     0,     0,   238,     0,     0,  -283,     0,   238,
     238,     0,   238,   238,     0,     0,     0,     0,     0,   238,
    -283,     0,  -105,  -283,  -283,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,  -283,  -283,  -283,     0,     0,     0,     0,     0,
    -283,  -283,  -283,  -283,     0,     0,  -283,  -283,  -283,     0,
    -283,     0,     0,     0,     0,     0,     0,   238,     0,     0,
       0,   238,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -409,     0,     0,
       0,     0,     0,     0,     0,  -409,  -409,  -409,   238,     0,
    -409,  -409,  -409,     0,  -409,     0,     0,     0,     0,     0,
       0,     0,     0,  -409,  -409,  -409,   238,     0,     0,     0,
       0,     0,     0,     0,  -409,  -409,     0,  -409,  -409,  -409,
    -409,  -409,     0,     0,     0,     0,   238,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   238,  -409,  -409,  -409,  -409,  -409,
    -409,  -409,  -409,  -409,  -409,  -409,  -409,  -409,     0,     0,
    -409,  -409,  -409,     0,     0,  -409,     0,     0,     0,     0,
       0,  -409,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -409,     0,  -409,
    -409,  -409,  -409,  -409,  -409,  -409,  -409,  -409,  -409,     0,
       0,     0,     0,  -409,  -409,  -409,  -409,  -409,  -277,   220,
    -409,  -409,  -409,     0,  -409,     0,  -277,  -277,  -277,     0,
       0,  -277,  -277,  -277,     0,  -277,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -277,  -277,  -277,     0,     0,
       0,     0,     0,     0,     0,  -277,  -277,     0,  -277,  -277,
    -277,  -277,  -277,     0,     0,     0,     0,     0,     0,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,     0,     0,   321,   322,  -277,  -277,  -277,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,     0,
       0,  -277,  -277,  -277,     0,     0,  -277,     0,     0,     0,
       0,   323,  -277,   324,   325,   326,   327,   328,   329,   330,
     331,   332,   333,     0,     0,  -277,     0,     0,  -277,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,
       0,     0,     0,     0,     0,  -277,  -277,  -277,  -277,  -571,
       0,  -277,  -277,  -277,     0,  -277,     0,  -571,  -571,  -571,
       0,     0,  -571,  -571,  -571,     0,  -571,     0,     0,     0,
       0,     0,     0,     0,     0,  -571,  -571,  -571,     0,     0,
       0,     0,     0,     0,     0,     0,  -571,  -571,     0,  -571,
    -571,  -571,  -571,  -571,     0,     0,     0,     0,     0,     0,
     308,   309,   310,   311,   312,   313,   314,   315,     0,   317,
     318,     0,     0,     0,     0,   321,   322,  -571,  -571,  -571,
    -571,  -571,  -571,  -571,  -571,  -571,  -571,  -571,  -571,  -571,
       0,     0,  -571,  -571,  -571,     0,     0,  -571,     0,     0,
       0,     0,     0,  -571,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,     0,     0,     0,     0,     0,  -571,
       0,  -571,  -571,  -571,  -571,  -571,  -571,  -571,  -571,  -571,
    -571,     0,     0,     0,     0,  -571,  -571,  -571,  -571,  -571,
    -290,   220,  -571,  -571,  -571,     0,  -571,     0,  -290,  -290,
    -290,     0,     0,  -290,  -290,  -290,     0,  -290,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -290,  -290,     0,
       0,     0,     0,     0,     0,     0,     0,  -290,  -290,     0,
    -290,  -290,  -290,  -290,  -290,     0,     0,     0,     0,     0,
       0,   308,  -572,  -572,  -572,  -572,   313,   314,     0,     0,
    -572,  -572,     0,     0,     0,     0,   321,   322,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,     0,     0,  -290,  -290,  -290,     0,     0,  -290,     0,
       0,     0,     0,     0,  -290,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,     0,     0,     0,     0,     0,
    -290,     0,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,     0,     0,     0,     0,     0,  -290,  -290,  -290,
    -290,  -553,   217,  -290,  -290,  -290,     0,  -290,     0,  -553,
    -553,  -553,     0,     0,     0,  -553,  -553,     0,  -553,     0,
       0,     0,     0,   714,     0,     0,     0,  -553,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -553,  -553,
       0,  -553,  -553,  -553,  -553,  -553,     0,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
       0,     0,   321,   322,     0,     0,     0,     0,     0,  -553,
    -553,  -553,  -553,  -553,  -553,  -553,  -553,  -553,  -553,  -553,
    -553,  -553,     0,     0,  -553,  -553,  -553,     0,   661,   323,
       0,   324,   325,   326,   327,   328,   329,   330,   331,   332,
     333,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -103,  -553,     0,  -553,  -553,  -553,  -553,  -553,  -553,  -553,
    -553,  -553,  -553,     0,     0,     0,     0,  -553,  -553,  -553,
    -553,   -94,  -283,     0,  -553,     0,  -553,     0,  -553,     0,
    -283,  -283,  -283,     0,     0,     0,  -283,  -283,     0,  -283,
       0,     0,     0,     0,   659,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -283,
    -283,     0,  -283,  -283,  -283,  -283,  -283,     0,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,     0,     0,   321,   322,     0,     0,     0,     0,     0,
    -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,  -283,     0,     0,  -283,  -283,  -283,     0,   662,
     323,     0,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -105,  -283,     0,  -283,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,  -283,  -283,     0,     0,     0,     0,     0,  -283,
    -283,  -283,   -96,     0,     0,  -283,     0,  -283,   241,  -283,
       5,     6,     7,     8,     9,  -571,  -571,  -571,    10,    11,
       0,     0,  -571,    12,     0,    13,    14,    15,    16,    17,
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
       0,     0,  -571,   241,  -571,     5,     6,     7,     8,     9,
       0,     0,  -571,    10,    11,     0,  -571,  -571,    12,     0,
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
       0,     0,     0,     0,     0,     0,     0,  -571,   241,  -571,
       5,     6,     7,     8,     9,     0,     0,  -571,    10,    11,
       0,     0,  -571,    12,  -571,    13,    14,    15,    16,    17,
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
      19,     0,  -571,     0,  -571,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,     0,    41,    42,     0,    43,    44,
      45,     0,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,     0,     0,    49,    50,     0,    51,    52,
       0,    53,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    61,    62,
      63,     0,     0,  -571,     0,     0,     0,     0,     0,     0,
       0,  -571,   241,  -571,     5,     6,     7,     8,     9,     0,
       0,  -571,    10,    11,     0,     0,  -571,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,    29,     0,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
      41,    42,     0,    43,    44,    45,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    48,     0,     0,
      49,    50,     0,    51,    52,     0,    53,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     0,
       0,   241,     0,     5,     6,     7,     8,     9,     0,  -571,
    -571,    10,    11,    61,    62,    63,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,  -571,     0,  -571,     0,
      20,    21,    22,    23,    24,    25,    26,     0,     0,    27,
       0,     0,     0,     0,     0,    28,    29,     0,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,     0,    41,
      42,     0,    43,    44,    45,     0,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,     0,     0,    49,
      50,     0,    51,    52,     0,    53,     0,    54,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     0,     0,
     241,     0,     5,     6,     7,     8,     9,     0,     0,     0,
      10,    11,    61,    62,    63,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,  -571,     0,  -571,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,    28,    29,     0,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
       0,    43,    44,    45,     0,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    48,     0,     0,   242,    50,
       0,    51,    52,     0,    53,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    61,    62,    63,     0,     0,     0,     0,     0,     0,
       0,     0,  -571,     0,  -571,   241,  -571,     5,     6,     7,
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
       0,     0,     0,     0,     0,     0,     0,  -571,     0,  -571,
     241,  -571,     5,     6,     7,     8,     9,     0,     0,     0,
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
       0,    61,    62,    63,     0,     0,  -571,     0,     0,     0,
       0,     0,     0,     0,  -571,   241,  -571,     5,     6,     7,
       8,     9,     0,     0,  -571,    10,    11,     0,     0,     0,
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
       0,    13,    14,    15,    16,    17,    18,    19,     0,  -571,
       0,  -571,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,   194,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,   195,    41,    42,     0,    43,    44,    45,     0,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   196,
       0,     0,   197,    50,     0,    51,    52,     0,   198,   199,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    61,   200,    63,    12,     0,
      13,    14,    15,    16,    17,    18,    19,     0,     0,     0,
     224,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
       0,    41,    42,     0,    43,    44,    45,     0,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   196,     0,
       0,   197,    50,     0,    51,    52,     0,     0,     0,    54,
      55,    56,    57,    58,    59,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    61,    62,    63,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,   288,     0,   289,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
      41,    42,     0,    43,    44,    45,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   196,     0,     0,
     197,    50,     0,    51,    52,     0,     0,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     8,     9,     0,     0,
       0,    10,    11,    61,    62,    63,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,   224,     0,
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
      16,    17,    18,    19,     0,   472,     0,     0,     0,    20,
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
       0,     0,     0,     0,   472,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,     0,
       0,     0,   137,   138,   139,   365,   366,   367,   368,   144,
     145,   146,     0,     0,     0,     0,     0,   147,   148,   149,
     150,   369,   370,   371,   372,   155,    37,    38,   373,    40,
       0,     0,     0,     0,     0,     0,     0,     0,   157,   158,
     159,   160,   161,   162,   163,   164,   165,     0,     0,   166,
     167,     0,     0,   168,   169,   170,   171,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   172,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,     0,   183,   184,  -546,  -546,  -546,
       0,  -546,     0,   185,   374,  -546,  -546,     0,     0,     0,
    -546,     0,  -546,  -546,  -546,  -546,  -546,  -546,  -546,     0,
    -546,     0,     0,     0,  -546,  -546,  -546,  -546,  -546,  -546,
    -546,     0,     0,  -546,     0,     0,     0,     0,     0,     0,
    -546,     0,     0,  -546,  -546,  -546,  -546,  -546,  -546,  -546,
    -546,  -546,  -546,  -546,  -546,     0,  -546,  -546,  -546,     0,
    -546,  -546,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -546,     0,     0,  -546,  -546,     0,  -546,  -546,     0,  -546,
    -546,  -546,  -546,  -546,  -546,  -546,  -546,  -546,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -546,  -546,  -546,     0,
    -546,  -547,  -547,  -547,     0,  -547,  -546,     0,     0,  -547,
    -547,     0,     0,     0,  -547,     0,  -547,  -547,  -547,  -547,
    -547,  -547,  -547,     0,  -547,     0,     0,     0,  -547,  -547,
    -547,  -547,  -547,  -547,  -547,     0,     0,  -547,     0,     0,
       0,     0,     0,     0,  -547,     0,     0,  -547,  -547,  -547,
    -547,  -547,  -547,  -547,  -547,  -547,  -547,  -547,  -547,     0,
    -547,  -547,  -547,     0,  -547,  -547,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -547,     0,     0,  -547,  -547,     0,
    -547,  -547,     0,  -547,  -547,  -547,  -547,  -547,  -547,  -547,
    -547,  -547,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -547,  -547,  -547,     0,  -547,  -549,  -549,  -549,     0,  -549,
    -547,     0,     0,  -549,  -549,     0,     0,     0,  -549,     0,
    -549,  -549,  -549,  -549,  -549,  -549,  -549,     0,     0,     0,
       0,     0,  -549,  -549,  -549,  -549,  -549,  -549,  -549,     0,
       0,  -549,     0,     0,     0,     0,     0,     0,  -549,     0,
       0,  -549,  -549,  -549,  -549,  -549,  -549,  -549,  -549,  -549,
    -549,  -549,  -549,     0,  -549,  -549,  -549,     0,  -549,  -549,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -549,   719,
       0,  -549,  -549,     0,  -549,  -549,     0,  -549,  -549,  -549,
    -549,  -549,  -549,  -549,  -549,  -549,     0,     0,     0,     0,
       0,  -103,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -549,  -549,  -549,  -551,  -551,  -551,
       0,  -551,     0,     0,  -549,  -551,  -551,     0,     0,     0,
    -551,     0,  -551,  -551,  -551,  -551,  -551,  -551,  -551,     0,
       0,     0,     0,     0,  -551,  -551,  -551,  -551,  -551,  -551,
    -551,     0,     0,  -551,     0,     0,     0,     0,     0,     0,
    -551,     0,     0,  -551,  -551,  -551,  -551,  -551,  -551,  -551,
    -551,  -551,  -551,  -551,  -551,     0,  -551,  -551,  -551,     0,
    -551,  -551,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -551,     0,     0,  -551,  -551,     0,  -551,  -551,     0,  -551,
    -551,  -551,  -551,  -551,  -551,  -551,  -551,  -551,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -551,  -551,  -551,  -552,
    -552,  -552,     0,  -552,     0,     0,  -551,  -552,  -552,     0,
       0,     0,  -552,     0,  -552,  -552,  -552,  -552,  -552,  -552,
    -552,     0,     0,     0,     0,     0,  -552,  -552,  -552,  -552,
    -552,  -552,  -552,     0,     0,  -552,     0,     0,     0,     0,
       0,     0,  -552,     0,     0,  -552,  -552,  -552,  -552,  -552,
    -552,  -552,  -552,  -552,  -552,  -552,  -552,     0,  -552,  -552,
    -552,     0,  -552,  -552,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -552,     0,     0,  -552,  -552,     0,  -552,  -552,
       0,  -552,  -552,  -552,  -552,  -552,  -552,  -552,  -552,  -552,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -552,  -552,
    -552,     0,     0,     0,     0,     0,     0,     0,  -552,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,     0,     0,     0,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,     0,     0,     0,     0,
       0,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     270,   271,   156,   272,     0,     0,     0,     0,     0,     0,
       0,     0,   157,   158,   159,   160,   161,   162,   163,   164,
     165,     0,     0,   166,   167,     0,     0,   168,   169,   170,
     171,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   172,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,     0,   183,
     184,     0,     0,     0,     0,     0,     0,   185,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,     0,     0,     0,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,     0,     0,     0,     0,     0,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   226,
       0,   156,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   157,   158,   159,   160,   161,   162,   163,   164,   165,
       0,     0,   166,   167,     0,     0,   168,   169,   170,   171,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     172,     0,     0,    55,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,     0,   183,   184,
       0,     0,     0,     0,     0,     0,   185,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,     0,     0,     0,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,     0,     0,     0,     0,     0,   147,
     148,   149,   150,   151,   152,   153,   154,   155,     0,     0,
     156,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     157,   158,   159,   160,   161,   162,   163,   164,   165,     0,
       0,   166,   167,     0,     0,   168,   169,   170,   171,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   172,
       0,     0,    55,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,     0,   183,   184,     0,
       0,     0,     0,     0,     0,   185,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
       0,     0,     0,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,     0,     0,     0,     0,     0,   147,   148,
     149,   150,   151,   152,   153,   154,   155,     0,     0,   156,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   157,
     158,   159,   160,   161,   162,   163,   164,   165,     0,     0,
     166,   167,     0,     0,   168,   169,   170,   171,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   172,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,     0,   183,   184,     5,     6,
       7,     0,     9,     0,   185,     0,    10,    11,     0,     0,
       0,    12,     0,    13,    14,    15,   231,   232,    18,    19,
       0,     0,     0,     0,     0,   233,   234,   235,    23,    24,
      25,    26,     0,     0,   194,     0,     0,     0,     0,     0,
       0,   258,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,     0,    41,    42,     0,    43,    44,    45,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   259,     0,     0,   197,    50,     0,    51,    52,     0,
       0,     0,    54,    55,    56,    57,    58,    59,    60,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,     0,     0,     0,    12,   260,    13,    14,
      15,   231,   232,    18,    19,   261,     0,     0,     0,     0,
     233,   234,   235,    23,    24,    25,    26,     0,     0,   194,
       0,     0,     0,     0,     0,     0,   258,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,     0,    41,
      42,     0,    43,    44,    45,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   259,     0,     0,   197,
      50,     0,    51,    52,     0,     0,     0,    54,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     5,     6,
       7,     8,     9,     0,     0,     0,    10,    11,     0,     0,
       0,    12,   260,    13,    14,    15,    16,    17,    18,    19,
     488,     0,     0,     0,     0,    20,    21,    22,    23,    24,
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
       0,     0,   194,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,   195,    41,    42,     0,    43,    44,    45,     0,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   196,
       0,     0,   197,    50,     0,    51,    52,     0,   198,   199,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    61,   200,    63,    12,     0,
      13,    14,    15,   231,   232,    18,    19,     0,     0,     0,
       0,     0,   233,   234,   235,    23,    24,    25,    26,     0,
       0,   194,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
     195,    41,    42,     0,    43,    44,    45,     0,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   196,     0,
       0,   197,    50,     0,    51,    52,     0,   585,   199,    54,
      55,    56,    57,    58,    59,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    61,   200,    63,    12,     0,    13,
      14,    15,   231,   232,    18,    19,     0,     0,     0,     0,
       0,   233,   234,   235,    23,    24,    25,    26,     0,     0,
     194,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,   195,
      41,    42,     0,    43,    44,    45,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   196,     0,     0,
     197,    50,     0,    51,    52,     0,   198,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    61,   200,    63,    12,     0,    13,    14,
      15,   231,   232,    18,    19,     0,     0,     0,     0,     0,
     233,   234,   235,    23,    24,    25,    26,     0,     0,   194,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,   195,    41,
      42,     0,    43,    44,    45,     0,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   196,     0,     0,   197,
      50,     0,    51,    52,     0,     0,   199,    54,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    61,   200,    63,    12,     0,    13,    14,    15,
     231,   232,    18,    19,     0,     0,     0,     0,     0,   233,
     234,   235,    23,    24,    25,    26,     0,     0,   194,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,   195,    41,    42,
       0,    43,    44,    45,     0,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   196,     0,     0,   197,    50,
       0,    51,    52,     0,   585,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    61,   200,    63,    12,     0,    13,    14,    15,   231,
     232,    18,    19,     0,     0,     0,     0,     0,   233,   234,
     235,    23,    24,    25,    26,     0,     0,   194,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,   195,    41,    42,     0,
      43,    44,    45,     0,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   196,     0,     0,   197,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      61,   200,    63,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,   194,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,     0,    41,    42,     0,    43,
      44,    45,     0,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   196,     0,     0,   197,    50,     0,    51,
      52,     0,   482,     0,    54,    55,    56,    57,    58,    59,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    61,
     200,    63,    12,     0,    13,    14,    15,   231,   232,    18,
      19,     0,     0,     0,     0,     0,   233,   234,   235,    23,
      24,    25,    26,     0,     0,   194,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,     0,    41,    42,     0,    43,    44,
      45,     0,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   196,     0,     0,   197,    50,     0,    51,    52,
       0,   198,     0,    54,    55,    56,    57,    58,    59,    60,
       0,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    61,   200,
      63,    12,     0,    13,    14,    15,   231,   232,    18,    19,
       0,     0,     0,     0,     0,   233,   234,   235,    23,    24,
      25,    26,     0,     0,   194,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,     0,    41,    42,     0,    43,    44,    45,
       0,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   196,     0,     0,   197,    50,     0,    51,    52,     0,
     767,     0,    54,    55,    56,    57,    58,    59,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    61,   200,    63,
      12,     0,    13,    14,    15,   231,   232,    18,    19,     0,
       0,     0,     0,     0,   233,   234,   235,    23,    24,    25,
      26,     0,     0,   194,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     196,     0,     0,   197,    50,     0,    51,    52,     0,   482,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    61,   200,    63,    12,
       0,    13,    14,    15,   231,   232,    18,    19,     0,     0,
       0,     0,     0,   233,   234,   235,    23,    24,    25,    26,
       0,     0,   194,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,     0,    41,    42,     0,    43,    44,    45,     0,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   196,
       0,     0,   197,    50,     0,    51,    52,     0,   585,     0,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    61,   200,    63,    12,     0,
      13,    14,    15,   231,   232,    18,    19,     0,     0,     0,
       0,     0,   233,   234,   235,    23,    24,    25,    26,     0,
       0,   194,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
       0,    41,    42,     0,    43,    44,    45,     0,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   196,     0,
       0,   197,    50,     0,    51,    52,     0,     0,     0,    54,
      55,    56,    57,    58,    59,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    61,   200,    63,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
      41,    42,     0,    43,    44,    45,     0,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   196,     0,     0,
     197,    50,     0,    51,    52,     0,     0,     0,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    61,    62,    63,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,     0,
      20,    21,    22,    23,    24,    25,    26,     0,     0,   194,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,     0,    41,
      42,     0,    43,    44,    45,     0,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   196,     0,     0,   197,
      50,     0,    51,    52,     0,     0,     0,    54,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    61,   200,    63,    12,     0,    13,    14,    15,
     231,   232,    18,    19,     0,     0,     0,     0,     0,   233,
     234,   235,    23,    24,    25,    26,     0,     0,   194,     0,
       0,     0,     0,     0,     0,   258,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
       0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   259,     0,     0,   304,    50,
       0,    51,    52,     0,   305,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,     0,     0,     0,
      12,   260,    13,    14,    15,   231,   232,    18,    19,     0,
       0,     0,     0,     0,   233,   234,   235,    23,    24,    25,
      26,     0,     0,   194,     0,     0,     0,     0,     0,     0,
     258,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     346,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   260,    13,    14,    15,
     231,   232,    18,    19,     0,     0,     0,     0,     0,   233,
     234,   235,    23,    24,    25,    26,     0,     0,   194,     0,
       0,     0,     0,     0,     0,   258,     0,     0,    32,    33,
      34,   354,    36,    37,    38,   355,    40,     0,    41,    42,
       0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   356,     0,     0,   357,     0,     0,   197,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,     0,     0,     0,
      12,   260,    13,    14,    15,   231,   232,    18,    19,     0,
       0,     0,     0,     0,   233,   234,   235,    23,    24,    25,
      26,     0,     0,   194,     0,     0,     0,     0,     0,     0,
     258,     0,     0,    32,    33,    34,   354,    36,    37,    38,
     355,    40,     0,    41,    42,     0,    43,    44,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     357,     0,     0,   197,    50,     0,    51,    52,     0,     0,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   260,    13,    14,    15,
     231,   232,    18,    19,     0,     0,     0,     0,     0,   233,
     234,   235,    23,    24,    25,    26,     0,     0,   194,     0,
       0,     0,     0,     0,     0,   258,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
       0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   259,     0,     0,   304,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,     0,     0,     0,
      12,   260,    13,    14,    15,   231,   232,    18,    19,     0,
       0,     0,     0,     0,   233,   234,   235,    23,    24,    25,
      26,     0,     0,   194,     0,     0,     0,     0,     0,     0,
     258,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,     0,    41,    42,     0,    43,    44,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     896,     0,     0,   197,    50,     0,    51,    52,     0,     0,
       0,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   260,    13,    14,    15,
     231,   232,    18,    19,     0,     0,     0,     0,     0,   233,
     234,   235,    23,    24,    25,    26,     0,     0,   194,     0,
       0,     0,     0,     0,     0,   258,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
       0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   906,     0,     0,   197,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,   531,   532,
       0,     0,   533,     0,     0,     0,     0,     0,     0,     0,
       0,   260,   157,   158,   159,   160,   161,   162,   163,   164,
     165,     0,     0,   166,   167,     0,     0,   168,   169,   170,
     171,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   172,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,     0,   183,
     184,   589,   524,     0,     0,   590,     0,   185,   220,     0,
       0,     0,     0,     0,     0,   157,   158,   159,   160,   161,
     162,   163,   164,   165,     0,     0,   166,   167,     0,     0,
     168,   169,   170,   171,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   172,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,     0,   183,   184,   592,   532,     0,     0,   593,     0,
     185,   220,     0,     0,     0,     0,     0,     0,   157,   158,
     159,   160,   161,   162,   163,   164,   165,     0,     0,   166,
     167,     0,     0,   168,   169,   170,   171,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   172,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,     0,   183,   184,   613,   524,     0,
       0,   614,     0,   185,   220,     0,     0,     0,     0,     0,
       0,   157,   158,   159,   160,   161,   162,   163,   164,   165,
       0,     0,   166,   167,     0,     0,   168,   169,   170,   171,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     172,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,     0,   183,   184,
     616,   532,     0,     0,   617,     0,   185,   220,     0,     0,
       0,     0,     0,     0,   157,   158,   159,   160,   161,   162,
     163,   164,   165,     0,     0,   166,   167,     0,     0,   168,
     169,   170,   171,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   172,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
       0,   183,   184,   688,   524,     0,     0,   689,     0,   185,
     220,     0,     0,     0,     0,     0,     0,   157,   158,   159,
     160,   161,   162,   163,   164,   165,     0,     0,   166,   167,
       0,     0,   168,   169,   170,   171,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   172,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,     0,   183,   184,   691,   532,     0,     0,
     692,     0,   185,   220,     0,     0,     0,     0,     0,     0,
     157,   158,   159,   160,   161,   162,   163,   164,   165,     0,
       0,   166,   167,     0,     0,   168,   169,   170,   171,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   172,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,     0,   183,   184,   698,
     524,     0,     0,   699,     0,   185,   220,     0,     0,     0,
       0,     0,     0,   157,   158,   159,   160,   161,   162,   163,
     164,   165,     0,     0,   166,   167,     0,     0,   168,   169,
     170,   171,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   172,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,     0,
     183,   184,   570,   532,     0,     0,   571,     0,   185,   220,
       0,     0,     0,     0,     0,     0,   157,   158,   159,   160,
     161,   162,   163,   164,   165,     0,     0,   166,   167,     0,
       0,   168,   169,   170,   171,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   172,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,     0,   183,   184,   959,   524,     0,     0,   960,
       0,   185,   220,     0,     0,     0,     0,     0,     0,   157,
     158,   159,   160,   161,   162,   163,   164,   165,     0,     0,
     166,   167,     0,     0,   168,   169,   170,   171,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   172,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,     0,   183,   184,   965,   524,
       0,     0,   966,     0,   185,   220,     0,     0,     0,     0,
       0,     0,   157,   158,   159,   160,   161,   162,   163,   164,
     165,     0,     0,   166,   167,     0,     0,   168,   169,   170,
     171,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   172,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,     0,   183,
     184,   968,   532,     0,     0,   969,     0,   185,   220,     0,
       0,     0,     0,     0,     0,   157,   158,   159,   160,   161,
     162,   163,   164,   165,     0,     0,   166,   167,     0,     0,
     168,   169,   170,   171,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   172,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,     0,   183,   184,   570,   532,     0,     0,   571,     0,
     185,   220,     0,     0,     0,     0,     0,     0,   157,   158,
     159,   160,   161,   162,   163,   164,   165,     0,     0,   166,
     167,     0,     0,   168,   169,   170,   171,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   172,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,     0,   183,   184,     0,     0,     0,
       0,     0,     0,   185,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   318,   319,   320,     0,     0,   321,
     322,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   323,     0,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,     0,   308,
     309,   310,   311,   312,   313,   314,     0,     0,   317,   318,
       0,     0,     0,   224,   321,   322,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   324,   325,   326,   327,   328,   329,   330,
     331,   332,   333
};

static const yytype_int16 yycheck[] =
{
       2,    55,   335,    49,     2,   213,     4,   307,    65,    16,
      17,     8,   252,    20,    51,     8,   256,   452,    16,    17,
     564,   307,    20,    27,   545,    29,    51,    52,   741,   448,
      87,    28,     4,    22,   544,    28,    28,    91,   340,   744,
       8,    53,   591,    76,    65,   357,     2,    87,     4,    51,
      52,    49,    50,   757,     1,    53,   574,   575,    53,   334,
      28,   336,   820,   452,    62,    27,   615,   400,    22,   488,
     676,    26,   822,    50,   680,    25,    13,   649,    76,    49,
     845,    76,    61,    26,    62,    83,    84,    55,     0,    87,
      88,    89,    90,    88,    89,    90,    25,    25,   276,    85,
     278,    25,   280,   378,   379,   242,    83,    84,    29,    13,
      87,    25,    13,    13,    87,    25,    85,   450,    85,    25,
     137,   110,   880,    91,   399,   109,   401,   106,    13,    13,
     843,   566,   396,   623,    85,    25,   109,    59,    60,    84,
     404,   690,    87,   129,    16,    17,   130,    85,    20,    56,
     425,   197,   701,   697,    25,   126,   110,    37,    38,   128,
     129,   705,   129,   132,    61,    28,   113,   304,   926,   741,
     935,   126,   744,   135,   924,   137,   451,   890,   129,   136,
     130,   886,   132,   126,   230,   664,   107,   244,   135,   132,
     137,   129,   671,   628,   898,   953,   242,   715,   135,   197,
     137,   130,   130,   132,   244,   207,   130,   104,   105,   106,
     217,   213,   219,   220,   649,   109,   130,   550,   220,   217,
     130,   219,   220,   244,   130,   223,   130,   225,   261,   562,
      85,   135,   230,   137,   135,   135,   137,   137,   657,   628,
     130,   731,   732,   241,   242,    85,   223,   132,   225,   561,
     135,   135,   137,   137,   258,    87,   404,   137,   304,   130,
      87,   867,   136,   261,    85,   302,   261,    76,   130,   704,
     307,   843,   242,   845,   129,   137,   551,   109,   135,    85,
     137,   235,   109,    85,   238,   241,   343,   465,   563,   129,
     438,   801,   737,   305,   292,   293,   294,   295,   296,   297,
     298,   299,   359,   343,   302,   114,   304,   305,   129,   307,
     305,    25,   223,   758,   886,   704,   293,   294,   295,   296,
     292,   298,   299,   129,   302,   869,   128,   129,   335,   307,
     820,   130,   822,   335,   304,   246,   292,   335,   137,   337,
      87,   297,   130,   341,   342,   217,   867,   219,   293,   294,
     295,   296,    52,    25,    54,    55,    56,    57,   356,    87,
      61,    85,   109,   935,   341,   342,   334,   109,    85,   132,
     610,   396,   658,    85,   137,   675,    99,    87,   922,   356,
     588,   109,   336,   130,   686,    52,   384,   385,    85,    68,
     880,   881,    87,   400,    87,   109,   341,   342,   400,   109,
     130,    26,   400,   104,   105,   129,   384,   385,    37,    38,
     435,   128,   129,    26,   109,   448,   130,   129,   967,   133,
     422,    26,   419,   137,   422,    61,   419,   419,    59,    60,
     130,   399,   129,   431,   924,   136,   926,   109,   132,   874,
     119,   120,   121,   450,    56,    61,   109,   401,   450,    65,
     448,   419,   450,   448,    87,   488,   424,    15,   130,    17,
      85,   133,    85,   953,    54,   137,   422,    61,   104,   105,
     106,   425,    85,    63,    64,   431,   109,    88,    52,   481,
      85,   449,    56,    14,    15,   130,   543,   544,   104,   105,
     488,    87,    26,   488,   540,   406,   542,   451,    87,   410,
     557,   126,   413,   128,   129,   128,   129,   132,   512,   546,
     104,   105,   106,   126,    85,   128,   129,   428,    61,   132,
     109,   126,   433,   128,   129,    85,   530,   132,   195,   572,
     573,   198,   199,   130,   536,   126,   538,   132,    68,   528,
     132,   130,   540,   550,   542,   132,   109,   536,   550,   133,
     127,    85,   550,   855,   134,   562,    85,   128,   129,   861,
     562,   104,   105,   106,   562,    56,   564,   565,   128,   129,
     572,   573,   574,   575,   528,   130,   487,   130,   611,   106,
     106,   549,   536,   572,   573,   582,   588,   591,   565,   582,
     582,    68,   126,   561,   128,   129,    68,   551,    85,   128,
     129,    85,   600,   605,   650,    68,    17,   135,    94,   563,
      56,   615,   945,   611,   582,    25,   611,   133,   572,   573,
      83,    84,   127,   600,   657,    52,   130,    54,    55,    56,
      57,   130,   634,   136,   636,   302,   638,   694,   675,    87,
     307,   128,   129,   109,   128,   129,    10,   649,   130,   706,
     604,   605,   650,   707,   117,   118,   119,   120,   121,   657,
     658,   109,   657,   661,   662,    85,   712,   713,    85,   667,
     668,   946,   130,   675,   676,   130,   678,    85,   680,     8,
     658,    68,   130,   661,   662,    13,   690,   127,   109,   667,
     668,   130,   738,   739,   130,   693,    52,   701,   696,   697,
     130,    52,   130,   130,   750,   111,    15,   705,   128,   129,
     134,   128,   129,   715,   712,   713,   693,   130,   109,   696,
     128,   129,    52,   547,    54,    55,    56,    57,   395,   396,
     117,   118,   119,   120,   121,   737,   135,   404,    85,   707,
     738,   739,   788,   114,   801,    87,   803,   130,   693,   795,
     796,   696,   750,   799,   130,   130,   758,    10,   760,    89,
     762,   763,   130,    85,    88,    95,    96,   109,   435,   815,
       9,   438,    10,    87,    16,    17,   130,   127,    20,    10,
     778,   128,   129,   127,   786,   130,   116,   789,   130,   119,
     788,   130,   703,   791,   130,   109,    87,   795,   796,   130,
     778,   799,   132,   130,    46,    47,   128,   129,   127,    51,
      52,    87,   858,   724,   791,   482,   130,   815,   109,   114,
      62,    63,   130,   127,   735,   130,   828,   829,    56,   831,
     130,   833,   834,   109,   130,    56,   130,   130,   836,   130,
      52,   836,    54,    55,    56,    57,   791,   130,    52,   130,
      54,    55,    56,    57,   130,   132,   854,   132,   422,   854,
     858,   685,    88,    90,   948,   867,   912,   731,   914,   898,
      93,   869,    54,    55,   920,    57,   947,    89,   702,   546,
      96,    63,    64,    95,    96,    89,    40,    41,    42,    43,
      44,    95,    96,    57,    91,   843,   741,   808,   900,   901,
     902,    -1,   904,   905,   116,    -1,    -1,   119,   819,   874,
      -1,    -1,   116,   824,   912,   119,   914,    -1,   585,    -1,
     132,   636,   920,   638,   922,    -1,   928,   929,   930,   931,
      -1,   135,    -1,    -1,    -1,     2,    -1,     4,   945,    -1,
      -1,    -1,    -1,   945,    -1,   947,   948,   945,    -1,    -1,
      -1,    -1,    -1,   195,    -1,    -1,   198,   199,   200,   961,
     962,   963,   964,   967,     2,   933,     4,    -1,   792,    -1,
     794,    -1,    -1,   975,    -1,   217,   944,   219,   220,    -1,
     804,   983,    49,    -1,    -1,   809,    53,   654,    -1,    -1,
      -1,    52,   946,    54,    55,    56,    57,    -1,    -1,    -1,
      52,    -1,    54,    55,    56,    57,    -1,    -1,   675,    76,
      -1,    49,    -1,    -1,    -1,    53,    -1,    -1,    -1,    -1,
      -1,    88,    89,    90,    91,    -1,    -1,    52,    89,    54,
      55,    56,    57,    -1,    95,    96,    -1,    89,    76,    -1,
     864,   865,    -1,   649,    96,   760,    -1,   762,   763,    -1,
      88,    89,    90,    91,    -1,   116,    -1,    -1,   119,    -1,
     302,    -1,   729,    -1,   116,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,    -1,   335,    -1,    -1,    -1,   921,    -1,    -1,
     767,    -1,    52,    -1,    54,    55,    56,    57,    -1,    -1,
      -1,    -1,    -1,   828,   829,    -1,   831,    -1,   833,   834,
      -1,    -1,    -1,    -1,    -1,   949,    -1,   951,    -1,    -1,
     197,   798,    -1,    -1,    -1,    -1,    -1,    -1,   744,    89,
      -1,   747,   384,   385,    -1,    95,    96,    -1,    -1,    68,
      -1,    -1,   394,   395,   396,    -1,    -1,    -1,   400,   197,
     402,   403,   404,   230,    83,    84,   116,    -1,    52,   119,
      54,    55,    56,    57,   241,   242,    -1,    -1,    -1,   421,
      -1,    -1,    -1,    -1,   426,   900,   901,   902,    -1,   904,
     905,    -1,   230,   435,   261,    -1,   438,   116,   117,   118,
     119,   120,   121,   241,   242,    89,    -1,    -1,   450,    -1,
      -1,    95,    -1,   928,   929,   930,   931,    -1,    -1,    -1,
      -1,    -1,    -1,   261,    -1,   292,    -1,    -1,   470,   471,
     297,    -1,    -1,    -1,    -1,    -1,    -1,   304,   305,   845,
     482,   847,    68,    -1,    -1,    -1,   961,   962,   963,   964,
      -1,    -1,    -1,    -1,   292,    -1,    -1,    83,    84,   297,
     975,    -1,    -1,    -1,    -1,    -1,   304,   305,   983,    52,
     337,    54,    55,    56,    57,     2,    -1,     4,    -1,    -1,
     886,    -1,   888,    -1,    -1,    -1,   892,    -1,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,    -1,    52,   337,
      54,    55,    56,    57,   546,    -1,    89,    -1,   550,    -1,
      -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     562,    -1,    49,    -1,    -1,    -1,    -1,    -1,    -1,   935,
      -1,   937,    -1,    -1,   940,    89,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   585,    -1,    -1,    -1,    -1,    -1,   955,
      -1,    -1,    -1,    -1,     2,   422,     4,    -1,    -1,    -1,
      -1,    88,    -1,    -1,   431,    -1,    -1,    -1,    -1,    -1,
      -1,   977,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   448,    -1,    -1,   422,    -1,    -1,    -1,    -1,     2,
      -1,     4,    -1,   431,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    -1,    -1,    -1,    53,    -1,    -1,    -1,    -1,
     448,    -1,   654,    -1,    -1,    -1,   658,   659,    -1,   661,
     662,   488,    -1,    -1,    -1,   667,   668,    -1,    76,    -1,
      -1,    -1,    -1,   675,    -1,    -1,    49,    -1,    -1,    -1,
      88,    89,    90,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     488,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     197,    -1,   714,   540,    -1,   542,    -1,   719,   720,    -1,
     722,   723,    -1,    -1,    -1,    -1,    -1,   729,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   564,    -1,    -1,
      -1,    -1,   540,   230,   542,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   241,   242,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   767,   564,    -1,    -1,   771,
      -1,    -1,    -1,    -1,    -1,    -1,   778,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   611,    -1,    -1,    -1,    -1,   197,
      -1,    -1,    -1,    -1,    -1,    -1,   798,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   292,    -1,    -1,    -1,    -1,
     297,    -1,    -1,   611,   816,    -1,    -1,   304,    -1,    -1,
     307,    -1,   230,   650,   197,    -1,    -1,    -1,    -1,    -1,
     657,    -1,    -1,   241,   242,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     337,    -1,   650,   261,    -1,    -1,    -1,   230,    -1,   657,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   241,   242,
     697,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   705,    -1,
      -1,    -1,    -1,    -1,   292,   712,   713,    -1,    -1,   297,
      -1,    -1,    -1,    -1,    -1,    -1,   304,   305,    -1,   697,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   705,    -1,    -1,
      -1,   738,   739,    -1,   712,   713,    -1,    -1,    -1,   292,
      -1,    -1,    -1,   750,   297,    -1,    -1,    -1,    -1,   337,
      -1,   304,    -1,    -1,    -1,   422,    -1,    -1,    -1,    -1,
     738,   739,    -1,   945,   431,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   750,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   788,    -1,    -1,   337,    -1,    -1,    -1,   795,   796,
      -1,    -1,   799,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   815,    -1,
     788,    -1,    -1,    -1,    -1,    -1,    -1,   795,   796,    -1,
      -1,   799,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   836,
      -1,    -1,    -1,    -1,   422,    -1,    -1,   815,    -1,    -1,
      -1,    -1,    -1,   431,    -1,    -1,    -1,   854,    -1,    -1,
      -1,   858,    -1,    -1,    -1,    -1,    -1,    -1,   836,    -1,
     448,    -1,   869,    -1,    -1,    -1,    -1,    -1,    -1,   422,
      -1,    -1,    -1,   540,    -1,   542,   854,    -1,   431,    -1,
     858,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   869,    -1,    -1,    -1,    -1,    -1,   564,    -1,    -1,
     488,    -1,    -1,    -1,    -1,   912,    -1,   914,    -1,    -1,
      -1,    -1,    -1,   920,    -1,   922,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   912,    -1,   914,    -1,    -1,    -1,
      -1,    -1,   920,    -1,   922,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   540,    -1,   542,     0,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     8,     9,    10,    -1,    -1,    13,    14,
      15,    -1,    17,    -1,    -1,    -1,   564,    -1,    -1,    -1,
      -1,    -1,    27,   650,    -1,    -1,    -1,   540,    -1,   542,
      -1,   658,    37,    38,    -1,    40,    41,    42,    43,    44,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   564,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   611,    -1,    -1,    -1,    -1,    -1,    -1,
     697,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   705,    -1,
      85,    -1,    -1,    -1,    -1,   712,   713,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   642,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   650,    -1,    -1,    -1,    -1,    -1,    -1,   657,
      -1,   738,   739,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   127,   750,   129,    -1,    -1,   132,   133,    -1,
     135,    -1,   137,    -1,    -1,    -1,    -1,   650,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   697,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   705,    -1,    -1,
      -1,   788,    -1,    -1,   712,   713,    -1,    -1,   795,   796,
      -1,    -1,   799,    -1,    -1,    -1,    46,    47,    -1,    -1,
      -1,    51,    52,    53,   697,    -1,    -1,    -1,   815,    -1,
     738,   739,   705,    63,    -1,    -1,    -1,    -1,    -1,   712,
     713,    -1,   750,    -1,    -1,    -1,    76,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,
      90,    -1,    -1,    -1,    -1,   738,   739,   854,    -1,    -1,
      -1,   858,    -1,    -1,    -1,    -1,    -1,   750,    -1,    -1,
     788,    -1,   869,    -1,    -1,    -1,    -1,   795,   796,    -1,
      -1,   799,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   815,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   788,    -1,    -1,    -1,    -1,
      -1,    -1,   795,   796,    -1,   912,   799,   914,   836,    -1,
      -1,    -1,    -1,   920,    -1,   922,    -1,    -1,    -1,    -1,
      -1,    -1,   815,    -1,    -1,    -1,   854,    44,    -1,    -1,
     858,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   869,    -1,    -1,    -1,   195,    -1,    -1,   198,   199,
     200,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    -1,   858,    83,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   869,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   912,    -1,   914,    -1,    -1,    -1,
      -1,    -1,   920,   110,   922,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,    -1,
      -1,   261,    -1,   130,    -1,    -1,    -1,    -1,    -1,   912,
      -1,   914,    -1,    -1,    -1,    -1,    -1,   920,    -1,   922,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    -1,   305,    83,    84,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    52,    53,    -1,    -1,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    -1,    -1,
      77,    78,    -1,    -1,    81,    82,    83,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,
      -1,    -1,    -1,    -1,   394,   395,   396,    -1,    -1,    -1,
      -1,    -1,   402,   403,   404,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,   123,   124,    -1,    -1,
      -1,   421,    -1,    -1,   131,   132,   426,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   435,    -1,    -1,   438,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   448,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     470,   471,    -1,     0,     1,    -1,     3,     4,     5,     6,
       7,    -1,   482,    -1,    11,    12,    -1,    -1,   488,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,    63,    64,    -1,    66,
      67,    -1,    -1,    -1,    -1,    -1,   546,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,
      97,    98,    99,   100,   101,   102,   103,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   585,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   122,   123,   124,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   135,    -1,
     137,   611,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     0,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     8,     9,    10,    -1,    -1,    13,    14,    15,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    28,    29,    -1,   654,    -1,    -1,   657,    -1,   659,
      37,    38,    -1,    40,    41,    42,    43,    44,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   675,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    -1,    -1,    83,    84,    85,    -1,
      87,    88,    -1,    -1,   714,    -1,    -1,    94,    -1,   719,
     720,    -1,   722,   723,    -1,    -1,    -1,    -1,    -1,   729,
     107,    -1,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,    -1,    -1,    -1,    -1,
     127,   128,   129,   130,    -1,    -1,   133,   134,   135,    -1,
     137,    -1,    -1,    -1,    -1,    -1,    -1,   767,    -1,    -1,
      -1,   771,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     8,     9,    10,   798,    -1,
      13,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    26,    27,    28,   816,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    37,    38,    -1,    40,    41,    42,
      43,    44,    -1,    -1,    -1,    -1,   836,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   854,    68,    69,    70,    71,    72,
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
      68,    69,    70,    71,    72,    73,    74,    75,    -1,    77,
      78,    -1,    -1,    -1,    -1,    83,    84,    68,    69,    70,
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
     121,    -1,   123,   124,    52,    53,    -1,    -1,    56,    -1,
     131,   132,    -1,    -1,    -1,    -1,    -1,    -1,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    -1,    -1,    77,
      78,    -1,    -1,    81,    82,    83,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,    -1,   123,   124,    -1,    -1,    -1,
      -1,    -1,    -1,   131,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    -1,    -1,    83,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   110,    -1,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    -1,    68,
      69,    70,    71,    72,    73,    74,    -1,    -1,    77,    78,
      -1,    -1,    -1,   137,    83,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121
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
     103,   122,   123,   124,   141,   142,   143,   148,   150,   151,
     153,   154,   157,   158,   160,   161,   162,   164,   165,   175,
     189,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   239,   240,   250,   251,   252,   253,   254,   255,
     256,   259,   269,   271,   272,   273,   274,   275,   276,   277,
     300,   311,   143,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    56,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    77,    78,    81,    82,
      83,    84,    95,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   123,   124,   131,   168,   169,   170,   171,
     173,   174,   269,   271,    39,    58,    86,    89,    95,    96,
     123,   157,   165,   175,   177,   182,   185,   187,   206,   273,
     274,   276,   277,   298,   299,   182,   182,   132,   183,   184,
     132,   179,   183,   132,   137,   305,    54,   170,   305,   144,
     126,    21,    22,    30,    31,    32,   175,   206,   300,   175,
      56,     1,    89,   146,   147,   148,   159,   160,   311,   151,
     178,   187,   298,   311,   177,   297,   298,   311,    46,    86,
     122,   130,   164,   189,   206,   273,   274,   277,   232,   233,
      54,    55,    57,   168,   262,   270,   261,   262,   263,   136,
     257,   136,   260,    59,    60,   153,   175,   175,   135,   137,
     304,   309,   310,    40,    41,    42,    43,    44,    37,    38,
      28,   237,   109,   130,    89,    95,   161,   109,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    83,    84,   110,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,    85,   128,   129,   190,   151,   152,
     152,   193,   195,   152,   304,   310,    86,   158,   165,   206,
     222,   273,   274,   277,    52,    56,    83,    86,   166,   167,
     206,   273,   274,   277,   167,    33,    34,    35,    36,    49,
      50,    51,    52,    56,   132,   168,   275,   295,    85,   129,
      26,   126,   241,   253,    87,    87,   179,   183,   241,   130,
     177,    56,   177,   177,   109,    88,   130,   186,   311,    85,
     128,   129,    87,    87,   130,   186,   182,   305,   306,   182,
     181,   182,   311,   151,   306,   151,    54,    63,    64,   149,
     132,   176,   126,   146,    85,   129,    87,   148,   159,   133,
     304,   310,   306,   191,   134,   130,   137,   308,   130,   308,
     127,   308,   305,    56,    59,    60,   161,   163,   130,    85,
     128,   129,   234,    61,   104,   105,   106,   264,   106,   264,
     106,    65,   264,   106,   106,   258,   264,   106,    61,   106,
      68,    68,   135,   143,   152,   152,   152,   152,   148,   151,
     151,   238,    95,   153,   177,   187,   188,   159,   130,   164,
     130,   150,   153,   165,   175,   177,   188,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,    52,    53,    56,   173,   179,   301,   302,
     181,    52,    53,    56,   173,   179,   301,   145,   146,    13,
     218,   309,   218,   152,   152,   304,    17,   244,    56,    85,
     128,   129,    25,   151,    52,    56,   166,     1,   113,   278,
     309,    85,   128,   129,   202,   296,   203,    85,   129,   303,
      52,    56,   301,   301,   243,   242,   153,   175,   153,   175,
      94,   155,   172,   175,   177,    95,   177,   185,   298,    52,
      56,   181,    52,    56,   299,   306,   133,   306,   306,   170,
     192,   175,   141,   127,   301,   301,   175,   306,   148,   306,
     298,   130,   163,    52,    56,   181,    52,    56,    52,    54,
      55,    56,    57,    89,    95,    96,   116,   119,   132,   235,
     281,   282,   283,   284,   285,   286,   289,   290,   291,   292,
     293,   266,   265,   136,   264,   136,   175,   175,    76,   114,
     227,   228,   311,   177,   130,   306,   163,   130,   109,    44,
     305,    87,    87,   179,   183,   305,   307,    87,    87,   179,
     180,   183,   311,    10,   217,     8,   246,   311,   146,    13,
     146,    27,   219,   309,   219,   244,   187,   217,    52,    56,
     181,    52,    56,   197,   200,   309,   279,   199,    52,    56,
     166,   181,   145,   151,   132,   280,   281,   204,   180,   183,
     180,   183,   227,   227,    44,   156,   170,   177,   186,    87,
      87,   307,    87,    87,   151,   127,   308,   161,   307,   109,
      52,    89,    95,   223,   224,   225,   283,   281,    29,   107,
     236,   130,   294,   311,   130,   294,    52,   130,   294,    52,
     267,    54,    55,    57,   268,   277,    52,   135,   226,   229,
     285,   287,   288,   291,   293,   311,   146,    95,   177,   163,
     175,   111,   153,   175,   153,   175,   155,   134,    87,   153,
     175,   153,   175,   155,   177,   188,   247,   311,    15,   221,
     311,    14,   220,   221,   221,   194,   196,   217,   130,   218,
     307,   152,   309,   152,   145,   307,   217,   306,   281,   145,
     309,   168,   146,   146,   175,   227,    87,   306,   177,   225,
     130,   283,   130,   306,   229,   146,   146,   284,   289,   291,
     293,   285,   286,   291,   285,   146,   109,    52,   230,   231,
     282,   229,   114,   130,   294,   130,   294,   130,   294,    10,
     177,   175,   153,   175,    88,   248,   311,   146,     9,   249,
     311,   152,   217,   217,   146,   146,   177,   146,   219,   201,
     309,   217,   306,   217,   205,    10,   127,   146,   306,   224,
     130,    95,   223,   306,    10,   127,   130,   294,   130,   294,
     130,   294,   130,   294,   294,   127,    86,   206,   130,   114,
     288,   291,   285,   287,   291,   285,    86,   165,   206,   273,
     274,   277,   218,   146,   218,   217,   217,   221,   244,   245,
     198,   145,   280,   127,   130,   224,   130,   283,   285,   291,
     285,   285,    56,    85,   231,   130,   294,   130,   294,   294,
     130,   294,   294,    56,    85,   128,   129,   146,   146,   146,
     217,   145,   224,   130,   294,   130,   294,   294,   294,    52,
      56,   285,   291,   285,   285,    52,    56,   181,    52,    56,
     246,   220,   217,   217,   224,   285,   294,   130,   294,   294,
     294,   307,   294,   285,   294
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
# define YYLEX yylex (&yylval, parser_state)
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
#line 547 "grammar19.y"
    {
                    lex_state = EXPR_BEG;
                    local_push(0);
                    class_nest = 0;
                  ;}
    break;

  case 3:
#line 553 "grammar19.y"
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
                    local_pop();
                  ;}
    break;

  case 4:
#line 573 "grammar19.y"
    {
                    void_stmts((yyvsp[(1) - (2)].node));
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                  ;}
    break;

  case 5:
#line 580 "grammar19.y"
    {
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 6:
#line 584 "grammar19.y"
    {
                    (yyval.node) = newline_node((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 7:
#line 588 "grammar19.y"
    {
                    (yyval.node) = block_append((yyvsp[(1) - (3)].node), newline_node((yyvsp[(3) - (3)].node)));
                  ;}
    break;

  case 8:
#line 592 "grammar19.y"
    {
                    (yyval.node) = remove_begin((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 10:
#line 599 "grammar19.y"
    {
                    if(in_def || in_single) {
                      yy_error("BEGIN in method");
                    }
                  ;}
    break;

  case 11:
#line 605 "grammar19.y"
    {
                    (yyval.node) = NEW_PREEXE((yyvsp[(4) - (5)].node));
                  ;}
    break;

  case 12:
#line 614 "grammar19.y"
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
#line 634 "grammar19.y"
    {
                    void_stmts((yyvsp[(1) - (2)].node));
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                  ;}
    break;

  case 14:
#line 641 "grammar19.y"
    {
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 15:
#line 645 "grammar19.y"
    {
                    (yyval.node) = newline_node((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 16:
#line 649 "grammar19.y"
    {
                    (yyval.node) = block_append((yyvsp[(1) - (3)].node), newline_node((yyvsp[(3) - (3)].node)));
                  ;}
    break;

  case 17:
#line 653 "grammar19.y"
    {
                    (yyval.node) = remove_begin((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 18:
#line 658 "grammar19.y"
    {lex_state = EXPR_FNAME;;}
    break;

  case 19:
#line 659 "grammar19.y"
    {
                    (yyval.node) = NEW_ALIAS((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 20:
#line 663 "grammar19.y"
    {
                    (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 21:
#line 667 "grammar19.y"
    {
                    char buf[2];
                    buf[0] = '$';
                    buf[1] = (char)(yyvsp[(3) - (3)].node)->nd_nth;
                    (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), parser_intern2(buf, 2));
                  ;}
    break;

  case 22:
#line 674 "grammar19.y"
    {
                    yy_error("can't make alias for the number variables");
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 23:
#line 679 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 24:
#line 683 "grammar19.y"
    {
                    (yyval.node) = NEW_IF(cond((yyvsp[(3) - (3)].node)), remove_begin((yyvsp[(1) - (3)].node)), 0);
                    fixpos((yyval.node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 25:
#line 688 "grammar19.y"
    {
                    (yyval.node) = NEW_UNLESS(cond((yyvsp[(3) - (3)].node)), remove_begin((yyvsp[(1) - (3)].node)), 0);
                    fixpos((yyval.node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 26:
#line 693 "grammar19.y"
    {
                    if((yyvsp[(1) - (3)].node) && nd_type((yyvsp[(1) - (3)].node)) == NODE_BEGIN) {
                      (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (3)].node)), (yyvsp[(1) - (3)].node)->nd_body, 0);
                    } else {
                      (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (3)].node)), (yyvsp[(1) - (3)].node), 1);
                    }
                  ;}
    break;

  case 27:
#line 701 "grammar19.y"
    {
                    if((yyvsp[(1) - (3)].node) && nd_type((yyvsp[(1) - (3)].node)) == NODE_BEGIN) {
                      (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (3)].node)), (yyvsp[(1) - (3)].node)->nd_body, 0);
                    } else {
                      (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (3)].node)), (yyvsp[(1) - (3)].node), 1);
                    }
                  ;}
    break;

  case 28:
#line 709 "grammar19.y"
    {
                    NODE *resq = NEW_RESBODY(0, remove_begin((yyvsp[(3) - (3)].node)), 0);
                    (yyval.node) = NEW_RESCUE(remove_begin((yyvsp[(1) - (3)].node)), resq, 0);
                  ;}
    break;

  case 29:
#line 714 "grammar19.y"
    {
                    if(in_def || in_single) {
                      rb_warn("END in method; use at_exit");
                    }

                    (yyval.node) = NEW_POSTEXE(NEW_NODE(NODE_SCOPE,
                            0 /* tbl */, (yyvsp[(3) - (4)].node) /* body */, 0 /* args */));
                  ;}
    break;

  case 31:
#line 724 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyvsp[(1) - (3)].node)->nd_value = ((yyvsp[(1) - (3)].node)->nd_head) ? NEW_TO_ARY((yyvsp[(3) - (3)].node)) : NEW_ARRAY((yyvsp[(3) - (3)].node));
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 32:
#line 730 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    if((yyvsp[(1) - (3)].node)) {
                      ID vid = (yyvsp[(1) - (3)].node)->nd_vid;
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
#line 752 "grammar19.y"
    {
                    NODE *args;

                    value_expr((yyvsp[(6) - (6)].node));
                    if(!(yyvsp[(3) - (6)].node)) (yyvsp[(3) - (6)].node) = NEW_ZARRAY();
                    args = arg_concat((yyvsp[(3) - (6)].node), (yyvsp[(6) - (6)].node));
                    if((yyvsp[(5) - (6)].id) == tOROP) {
                      (yyvsp[(5) - (6)].id) = 0;
                    } else if((yyvsp[(5) - (6)].id) == tANDOP) {
                      (yyvsp[(5) - (6)].id) = 1;
                    } else {
                      (yyvsp[(5) - (6)].id) = convert_op((yyvsp[(5) - (6)].id));
                    }
                    (yyval.node) = NEW_OP_ASGN1((yyvsp[(1) - (6)].node), (yyvsp[(5) - (6)].id), args);
                    fixpos((yyval.node), (yyvsp[(1) - (6)].node));
                  ;}
    break;

  case 34:
#line 769 "grammar19.y"
    {
                    value_expr((yyvsp[(5) - (5)].node));
                    if((yyvsp[(4) - (5)].id) == tOROP) {
                      (yyvsp[(4) - (5)].id) = 0;
                    } else if((yyvsp[(4) - (5)].id) == tANDOP) {
                      (yyvsp[(4) - (5)].id) = 1;
                    } else {
                      (yyvsp[(4) - (5)].id) = convert_op((yyvsp[(4) - (5)].id));
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 35:
#line 782 "grammar19.y"
    {
                    value_expr((yyvsp[(5) - (5)].node));
                    if((yyvsp[(4) - (5)].id) == tOROP) {
                      (yyvsp[(4) - (5)].id) = 0;
                    } else if((yyvsp[(4) - (5)].id) == tANDOP) {
                      (yyvsp[(4) - (5)].id) = 1;
                    } else {
                      (yyvsp[(4) - (5)].id) = convert_op((yyvsp[(4) - (5)].id));
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 36:
#line 795 "grammar19.y"
    {
                    yy_error("constant re-assignment");
                    (yyval.node) = 0;
                  ;}
    break;

  case 37:
#line 800 "grammar19.y"
    {
                    value_expr((yyvsp[(5) - (5)].node));
                    if((yyvsp[(4) - (5)].id) == tOROP) {
                      (yyvsp[(4) - (5)].id) = 0;
                    } else if((yyvsp[(4) - (5)].id) == tANDOP) {
                      (yyvsp[(4) - (5)].id) = 1;
                    } else {
                      (yyvsp[(4) - (5)].id) = convert_op((yyvsp[(4) - (5)].id));
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 38:
#line 813 "grammar19.y"
    {
                    rb_backref_error((yyvsp[(1) - (3)].node));
                    (yyval.node) = 0;
                  ;}
    break;

  case 39:
#line 818 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 40:
#line 823 "grammar19.y"
    {
                    (yyvsp[(1) - (3)].node)->nd_value = ((yyvsp[(1) - (3)].node)->nd_head) ? NEW_TO_ARY((yyvsp[(3) - (3)].node)) : NEW_ARRAY((yyvsp[(3) - (3)].node));
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 41:
#line 828 "grammar19.y"
    {
                    (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 43:
#line 835 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 44:
#line 840 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 46:
#line 847 "grammar19.y"
    {
                    (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 47:
#line 851 "grammar19.y"
    {
                    (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 48:
#line 855 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[(3) - (3)].node)), '!');
                  ;}
    break;

  case 49:
#line 859 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[(2) - (2)].node)), '!');
                  ;}
    break;

  case 51:
#line 866 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  ;}
    break;

  case 55:
#line 879 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 56:
#line 883 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 57:
#line 889 "grammar19.y"
    {
                    (yyvsp[(1) - (1)].vars) = bv_push();
                    (yyval.num) = sourceline;
                  ;}
    break;

  case 58:
#line 896 "grammar19.y"
    {
                    (yyval.node) = NEW_ITER((yyvsp[(3) - (5)].node), (yyvsp[(4) - (5)].node));
                    nd_set_line((yyval.node), (yyvsp[(2) - (5)].num));
                    bv_pop((yyvsp[(1) - (5)].vars));
                  ;}
    break;

  case 59:
#line 904 "grammar19.y"
    {
                    (yyval.node) = NEW_FCALL((yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                 ;}
    break;

  case 60:
#line 909 "grammar19.y"
    {
                    block_dup_check((yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].node));
                    (yyvsp[(3) - (3)].node)->nd_iter = NEW_FCALL((yyvsp[(1) - (3)].id), (yyvsp[(2) - (3)].node));
                    (yyval.node) = (yyvsp[(3) - (3)].node);
                    fixpos((yyval.node), (yyvsp[(2) - (3)].node));
                 ;}
    break;

  case 61:
#line 916 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 62:
#line 921 "grammar19.y"
    {
                    block_dup_check((yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                    (yyvsp[(5) - (5)].node)->nd_iter = NEW_CALL((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].node));
                    (yyval.node) = (yyvsp[(5) - (5)].node);
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                 ;}
    break;

  case 63:
#line 928 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 64:
#line 933 "grammar19.y"
    {
                    block_dup_check((yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                    (yyvsp[(5) - (5)].node)->nd_iter = NEW_CALL((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].node));
                    (yyval.node) = (yyvsp[(5) - (5)].node);
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 65:
#line 940 "grammar19.y"
    {
                    (yyval.node) = NEW_SUPER((yyvsp[(2) - (2)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 66:
#line 945 "grammar19.y"
    {
                    (yyval.node) = new_yield((yyvsp[(2) - (2)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 67:
#line 950 "grammar19.y"
    {
                    (yyval.node) = NEW_RETURN(ret_args((yyvsp[(2) - (2)].node)));
                  ;}
    break;

  case 68:
#line 954 "grammar19.y"
    {
                    (yyval.node) = NEW_BREAK(ret_args((yyvsp[(2) - (2)].node)));
                  ;}
    break;

  case 69:
#line 958 "grammar19.y"
    {
                    (yyval.node) = NEW_NEXT(ret_args((yyvsp[(2) - (2)].node)));
                  ;}
    break;

  case 71:
#line 965 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 73:
#line 972 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(NEW_LIST((yyvsp[(2) - (3)].node)), 0);
                  ;}
    break;

  case 74:
#line 978 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (1)].node), 0);
                  ;}
    break;

  case 75:
#line 982 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(list_append((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node)), 0);
                  ;}
    break;

  case 76:
#line 986 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 77:
#line 990 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (5)].node), NEW_POSTARG((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)));
                  ;}
    break;

  case 78:
#line 994 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (2)].node), -1);
                  ;}
    break;

  case 79:
#line 998 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (4)].node), NEW_POSTARG(-1, (yyvsp[(4) - (4)].node)));
                  ;}
    break;

  case 80:
#line 1002 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 81:
#line 1006 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].node)));
                  ;}
    break;

  case 82:
#line 1010 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, -1);
                  ;}
    break;

  case 83:
#line 1014 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG(-1, (yyvsp[(3) - (3)].node)));
                  ;}
    break;

  case 85:
#line 1021 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 86:
#line 1027 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (2)].node));
                  ;}
    break;

  case 87:
#line 1031 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                  ;}
    break;

  case 88:
#line 1037 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 89:
#line 1041 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 90:
#line 1047 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                  ;}
    break;

  case 91:
#line 1051 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                  ;}
    break;

  case 92:
#line 1055 "grammar19.y"
    {
                    (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
                  ;}
    break;

  case 93:
#line 1059 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 94:
#line 1063 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 95:
#line 1067 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 96:
#line 1071 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                  ;}
    break;

  case 97:
#line 1077 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                  ;}
    break;

  case 98:
#line 1083 "grammar19.y"
    {
                    rb_backref_error((yyvsp[(1) - (1)].node));
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 99:
#line 1090 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                    if(!(yyval.node)) (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 100:
#line 1095 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                    if(!(yyval.node)) (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 101:
#line 1100 "grammar19.y"
    {
                    (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
                  ;}
    break;

  case 102:
#line 1104 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 103:
#line 1108 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 104:
#line 1112 "grammar19.y"
    {
                    (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 105:
#line 1116 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                  ;}
    break;

  case 106:
#line 1122 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("dynamic constant assignment");
                    (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                  ;}
    break;

  case 107:
#line 1128 "grammar19.y"
    {
                    rb_backref_error((yyvsp[(1) - (1)].node));
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 108:
#line 1135 "grammar19.y"
    {
                    yy_error("class/module name must be CONSTANT");
                  ;}
    break;

  case 110:
#line 1142 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 111:
#line 1146 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON2(0, (yyval.node));
                  ;}
    break;

  case 112:
#line 1150 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 116:
#line 1159 "grammar19.y"
    {
                    lex_state = EXPR_ENDFN;
                    (yyval.id) = convert_op((yyvsp[(1) - (1)].id));
                  ;}
    break;

  case 117:
#line 1164 "grammar19.y"
    {
                    lex_state = EXPR_ENDFN;
                    (yyval.id) = (yyvsp[(1) - (1)].id);
                  ;}
    break;

  case 120:
#line 1175 "grammar19.y"
    {
                    (yyval.node) = NEW_LIT(ID2SYM((yyvsp[(1) - (1)].id)));
                  ;}
    break;

  case 122:
#line 1182 "grammar19.y"
    {
                    (yyval.node) = NEW_UNDEF((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 123:
#line 1185 "grammar19.y"
    {lex_state = EXPR_FNAME;;}
    break;

  case 124:
#line 1186 "grammar19.y"
    {
                    (yyval.node) = block_append((yyvsp[(1) - (4)].node), NEW_UNDEF((yyvsp[(4) - (4)].node)));
                  ;}
    break;

  case 125:
#line 1191 "grammar19.y"
    { (yyval.id) = '|'; ;}
    break;

  case 126:
#line 1192 "grammar19.y"
    { (yyval.id) = '^'; ;}
    break;

  case 127:
#line 1193 "grammar19.y"
    { (yyval.id) = '&'; ;}
    break;

  case 128:
#line 1194 "grammar19.y"
    { (yyval.id) = tCMP; ;}
    break;

  case 129:
#line 1195 "grammar19.y"
    { (yyval.id) = tEQ; ;}
    break;

  case 130:
#line 1196 "grammar19.y"
    { (yyval.id) = tEQQ; ;}
    break;

  case 131:
#line 1197 "grammar19.y"
    { (yyval.id) = tMATCH; ;}
    break;

  case 132:
#line 1198 "grammar19.y"
    { (yyval.id) = tNMATCH; ;}
    break;

  case 133:
#line 1199 "grammar19.y"
    { (yyval.id) = '>'; ;}
    break;

  case 134:
#line 1200 "grammar19.y"
    { (yyval.id) = tGEQ; ;}
    break;

  case 135:
#line 1201 "grammar19.y"
    { (yyval.id) = '<'; ;}
    break;

  case 136:
#line 1202 "grammar19.y"
    { (yyval.id) = tLEQ; ;}
    break;

  case 137:
#line 1203 "grammar19.y"
    { (yyval.id) = tNEQ; ;}
    break;

  case 138:
#line 1204 "grammar19.y"
    { (yyval.id) = tLSHFT; ;}
    break;

  case 139:
#line 1205 "grammar19.y"
    { (yyval.id) = tRSHFT; ;}
    break;

  case 140:
#line 1206 "grammar19.y"
    { (yyval.id) = '+'; ;}
    break;

  case 141:
#line 1207 "grammar19.y"
    { (yyval.id) = '-'; ;}
    break;

  case 142:
#line 1208 "grammar19.y"
    { (yyval.id) = '*'; ;}
    break;

  case 143:
#line 1209 "grammar19.y"
    { (yyval.id) = '*'; ;}
    break;

  case 144:
#line 1210 "grammar19.y"
    { (yyval.id) = '/'; ;}
    break;

  case 145:
#line 1211 "grammar19.y"
    { (yyval.id) = '%'; ;}
    break;

  case 146:
#line 1212 "grammar19.y"
    { (yyval.id) = tPOW; ;}
    break;

  case 147:
#line 1213 "grammar19.y"
    { (yyval.id) = '!'; ;}
    break;

  case 148:
#line 1214 "grammar19.y"
    { (yyval.id) = '~'; ;}
    break;

  case 149:
#line 1215 "grammar19.y"
    { (yyval.id) = tUPLUS; ;}
    break;

  case 150:
#line 1216 "grammar19.y"
    { (yyval.id) = tUMINUS; ;}
    break;

  case 151:
#line 1217 "grammar19.y"
    { (yyval.id) = tAREF; ;}
    break;

  case 152:
#line 1218 "grammar19.y"
    { (yyval.id) = tASET; ;}
    break;

  case 153:
#line 1219 "grammar19.y"
    { (yyval.id) = '`'; ;}
    break;

  case 195:
#line 1237 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 196:
#line 1242 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (5)].node));
                    (yyvsp[(3) - (5)].node) = NEW_RESCUE((yyvsp[(3) - (5)].node), NEW_RESBODY(0, (yyvsp[(5) - (5)].node), 0), 0);
                    (yyval.node) = node_assign((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node));
                  ;}
    break;

  case 197:
#line 1248 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (3)].node));
                    if((yyvsp[(1) - (3)].node)) {
                      ID vid = (yyvsp[(1) - (3)].node)->nd_vid;
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

  case 198:
#line 1270 "grammar19.y"
    {
                    value_expr((yyvsp[(3) - (5)].node));
                    (yyvsp[(3) - (5)].node) = NEW_RESCUE((yyvsp[(3) - (5)].node), NEW_RESBODY(0, (yyvsp[(5) - (5)].node), 0), 0);
                    if((yyvsp[(1) - (5)].node)) {
                      ID vid = (yyvsp[(1) - (5)].node)->nd_vid;
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

  case 199:
#line 1293 "grammar19.y"
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
                    } else {
                      (yyvsp[(5) - (6)].id) = convert_op((yyvsp[(5) - (6)].id));
                    }
                    (yyval.node) = NEW_OP_ASGN1((yyvsp[(1) - (6)].node), (yyvsp[(5) - (6)].id), args);
                    fixpos((yyval.node), (yyvsp[(1) - (6)].node));
                  ;}
    break;

  case 200:
#line 1314 "grammar19.y"
    {
                    value_expr((yyvsp[(5) - (5)].node));
                    if((yyvsp[(4) - (5)].id) == tOROP) {
                      (yyvsp[(4) - (5)].id) = 0;
                    } else if((yyvsp[(4) - (5)].id) == tANDOP) {
                      (yyvsp[(4) - (5)].id) = 1;
                    } else {
                      (yyvsp[(4) - (5)].id) = convert_op((yyvsp[(4) - (5)].id));
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 201:
#line 1327 "grammar19.y"
    {
                    value_expr((yyvsp[(5) - (5)].node));
                    if((yyvsp[(4) - (5)].id) == tOROP) {
                      (yyvsp[(4) - (5)].id) = 0;
                    } else if((yyvsp[(4) - (5)].id) == tANDOP) {
                      (yyvsp[(4) - (5)].id) = 1;
                    } else {
                      (yyvsp[(4) - (5)].id) = convert_op((yyvsp[(4) - (5)].id));
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 202:
#line 1340 "grammar19.y"
    {
                    value_expr((yyvsp[(5) - (5)].node));
                    if((yyvsp[(4) - (5)].id) == tOROP) {
                      (yyvsp[(4) - (5)].id) = 0;
                    } else if((yyvsp[(4) - (5)].id) == tANDOP) {
                      (yyvsp[(4) - (5)].id) = 1;
                    } else {
                      (yyvsp[(4) - (5)].id) = convert_op((yyvsp[(4) - (5)].id));
                    }
                    (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                  ;}
    break;

  case 203:
#line 1353 "grammar19.y"
    {
                    yy_error("constant re-assignment");
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 204:
#line 1358 "grammar19.y"
    {
                    yy_error("constant re-assignment");
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 205:
#line 1363 "grammar19.y"
    {
                    rb_backref_error((yyvsp[(1) - (3)].node));
                    (yyval.node) = NEW_BEGIN(0);
                  ;}
    break;

  case 206:
#line 1368 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (3)].node));
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = NEW_DOT2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 207:
#line 1374 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (3)].node));
                    value_expr((yyvsp[(3) - (3)].node));
                    (yyval.node) = NEW_DOT3((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 208:
#line 1380 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '+', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 209:
#line 1384 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '-', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 210:
#line 1388 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '*', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 211:
#line 1392 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '/', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 212:
#line 1396 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '%', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 213:
#line 1400 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tPOW, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 214:
#line 1404 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL(call_bin_op((yyvsp[(2) - (4)].node), tPOW, (yyvsp[(4) - (4)].node)), tUMINUS, 0);
                  ;}
    break;

  case 215:
#line 1408 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL(call_bin_op((yyvsp[(2) - (4)].node), tPOW, (yyvsp[(4) - (4)].node)), tUMINUS, 0);
                  ;}
    break;

  case 216:
#line 1412 "grammar19.y"
    {
                    (yyval.node) = call_uni_op((yyvsp[(2) - (2)].node), tUPLUS);
                  ;}
    break;

  case 217:
#line 1416 "grammar19.y"
    {
                    (yyval.node) = call_uni_op((yyvsp[(2) - (2)].node), tUMINUS);
                  ;}
    break;

  case 218:
#line 1420 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '|', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 219:
#line 1424 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '^', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 220:
#line 1428 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '&', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 221:
#line 1432 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tCMP, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 222:
#line 1436 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '>', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 223:
#line 1440 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tGEQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 224:
#line 1444 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), '<', (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 225:
#line 1448 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tLEQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 226:
#line 1452 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tEQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 227:
#line 1456 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tEQQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 228:
#line 1460 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tNEQ, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 229:
#line 1464 "grammar19.y"
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

  case 230:
#line 1474 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tNMATCH, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 231:
#line 1478 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[(2) - (2)].node)), '!');
                  ;}
    break;

  case 232:
#line 1482 "grammar19.y"
    {
                    (yyval.node) = call_uni_op((yyvsp[(2) - (2)].node), '~');
                  ;}
    break;

  case 233:
#line 1486 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tLSHFT, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 234:
#line 1490 "grammar19.y"
    {
                    (yyval.node) = call_bin_op((yyvsp[(1) - (3)].node), tRSHFT, (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 235:
#line 1494 "grammar19.y"
    {
                    (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 236:
#line 1498 "grammar19.y"
    {
                    (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 237:
#line 1501 "grammar19.y"
    {in_defined = 1;;}
    break;

  case 238:
#line 1502 "grammar19.y"
    {
                    in_defined = 0;
                    (yyval.node) = NEW_DEFINED((yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 239:
#line 1507 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (6)].node));
                    (yyval.node) = NEW_IF(cond((yyvsp[(1) - (6)].node)), (yyvsp[(3) - (6)].node), (yyvsp[(6) - (6)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (6)].node));
                  ;}
    break;

  case 240:
#line 1513 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                  ;}
    break;

  case 241:
#line 1519 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  ;}
    break;

  case 243:
#line 1528 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                  ;}
    break;

  case 244:
#line 1532 "grammar19.y"
    {
                    (yyval.node) = arg_append((yyvsp[(1) - (4)].node), NEW_HASH((yyvsp[(3) - (4)].node)));
                  ;}
    break;

  case 245:
#line 1536 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                  ;}
    break;

  case 246:
#line 1542 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 251:
#line 1556 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 252:
#line 1561 "grammar19.y"
    {
                    (yyval.node) = arg_blk_pass((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 253:
#line 1565 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                    (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 254:
#line 1570 "grammar19.y"
    {
                    (yyval.node) = arg_append((yyvsp[(1) - (4)].node), NEW_HASH((yyvsp[(3) - (4)].node)));
                    (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 256:
#line 1577 "grammar19.y"
    {
                    (yyval.val) = cmdarg_stack;
                    CMDARG_PUSH(1);
                  ;}
    break;

  case 257:
#line 1582 "grammar19.y"
    {
                    /* CMDARG_POP() */
                    cmdarg_stack = (yyvsp[(1) - (2)].val);
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 258:
#line 1590 "grammar19.y"
    {
                    (yyval.node) = NEW_BLOCK_PASS((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 259:
#line 1596 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 260:
#line 1600 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 261:
#line 1604 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 262:
#line 1610 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 263:
#line 1614 "grammar19.y"
    {
                    (yyval.node) = NEW_SPLAT((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 264:
#line 1618 "grammar19.y"
    {
                    NODE *n1;
                    if((n1 = splat_array((yyvsp[(1) - (3)].node))) != 0) {
                      (yyval.node) = list_append(n1, (yyvsp[(3) - (3)].node));
                    } else {
                      (yyval.node) = arg_append((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    }
                  ;}
    break;

  case 265:
#line 1627 "grammar19.y"
    {
                    NODE *n1;
                    if((nd_type((yyvsp[(4) - (4)].node)) == NODE_ARRAY) && (n1 = splat_array((yyvsp[(1) - (4)].node))) != 0) {
                      (yyval.node) = list_concat(n1, (yyvsp[(4) - (4)].node));
                    } else {
                      (yyval.node) = arg_concat((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node));
                    }
                  ;}
    break;

  case 266:
#line 1638 "grammar19.y"
    {
                    NODE *n1;
                    if((n1 = splat_array((yyvsp[(1) - (3)].node))) != 0) {
                      (yyval.node) = list_append(n1, (yyvsp[(3) - (3)].node));
                    } else {
                      (yyval.node) = arg_append((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    }
                  ;}
    break;

  case 267:
#line 1647 "grammar19.y"
    {
                    NODE *n1;
                    if(nd_type((yyvsp[(4) - (4)].node)) == NODE_ARRAY && (n1 = splat_array((yyvsp[(1) - (4)].node))) != 0) {
                      (yyval.node) = list_concat(n1, (yyvsp[(4) - (4)].node));
                    } else {
                      (yyval.node) = arg_concat((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node));
                    }
                  ;}
    break;

  case 268:
#line 1656 "grammar19.y"
    {
                    (yyval.node) = NEW_SPLAT((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 277:
#line 1670 "grammar19.y"
    {
                    (yyval.node) = NEW_FCALL((yyvsp[(1) - (1)].id), 0);
                  ;}
    break;

  case 278:
#line 1674 "grammar19.y"
    {
                    (yyval.num) = sourceline;
                  ;}
    break;

  case 279:
#line 1679 "grammar19.y"
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

  case 280:
#line 1690 "grammar19.y"
    {lex_state = EXPR_ENDARG;;}
    break;

  case 281:
#line 1691 "grammar19.y"
    {
                    rb_warning("(...) interpreted as grouped expression");
                    (yyval.node) = (yyvsp[(2) - (4)].node);
                  ;}
    break;

  case 282:
#line 1696 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 283:
#line 1700 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                  ;}
    break;

  case 284:
#line 1704 "grammar19.y"
    {
                    (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 285:
#line 1708 "grammar19.y"
    {
                    if((yyvsp[(2) - (3)].node) == 0) {
                      (yyval.node) = NEW_ZARRAY(); /* zero length array*/
                    } else {
                      (yyval.node) = (yyvsp[(2) - (3)].node);
                    }
                  ;}
    break;

  case 286:
#line 1716 "grammar19.y"
    {
                    (yyval.node) = NEW_HASH((yyvsp[(2) - (3)].node));
                  ;}
    break;

  case 287:
#line 1720 "grammar19.y"
    {
                    (yyval.node) = NEW_RETURN(0);
                  ;}
    break;

  case 288:
#line 1724 "grammar19.y"
    {
                    (yyval.node) = new_yield((yyvsp[(3) - (4)].node));
                  ;}
    break;

  case 289:
#line 1728 "grammar19.y"
    {
                    (yyval.node) = NEW_YIELD(0, Qfalse);
                  ;}
    break;

  case 290:
#line 1732 "grammar19.y"
    {
                    (yyval.node) = NEW_YIELD(0, Qfalse);
                  ;}
    break;

  case 291:
#line 1735 "grammar19.y"
    {in_defined = 1;;}
    break;

  case 292:
#line 1736 "grammar19.y"
    {
                    in_defined = 0;
                    (yyval.node) = NEW_DEFINED((yyvsp[(5) - (6)].node));
                  ;}
    break;

  case 293:
#line 1741 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond((yyvsp[(3) - (4)].node)), '!');
                  ;}
    break;

  case 294:
#line 1745 "grammar19.y"
    {
                    (yyval.node) = call_uni_op(cond(NEW_NIL()), '!');
                  ;}
    break;

  case 295:
#line 1749 "grammar19.y"
    {
                    (yyvsp[(2) - (2)].node)->nd_iter = NEW_FCALL((yyvsp[(1) - (2)].id), 0);
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                    fixpos((yyvsp[(2) - (2)].node)->nd_iter, (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 297:
#line 1756 "grammar19.y"
    {
                    block_dup_check((yyvsp[(1) - (2)].node)->nd_args, (yyvsp[(2) - (2)].node));
                    (yyvsp[(2) - (2)].node)->nd_iter = (yyvsp[(1) - (2)].node);
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                    fixpos((yyval.node), (yyvsp[(1) - (2)].node));
                  ;}
    break;

  case 298:
#line 1763 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 299:
#line 1770 "grammar19.y"
    {
                    (yyval.node) = NEW_IF(cond((yyvsp[(2) - (6)].node)), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (6)].node));
                  ;}
    break;

  case 300:
#line 1778 "grammar19.y"
    {
                    (yyval.node) = NEW_UNLESS(cond((yyvsp[(2) - (6)].node)), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (6)].node));
                  ;}
    break;

  case 301:
#line 1782 "grammar19.y"
    {COND_PUSH(1);;}
    break;

  case 302:
#line 1782 "grammar19.y"
    {COND_POP();;}
    break;

  case 303:
#line 1785 "grammar19.y"
    {
                    (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (7)].node)), (yyvsp[(6) - (7)].node), 1);
                    fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                  ;}
    break;

  case 304:
#line 1789 "grammar19.y"
    {COND_PUSH(1);;}
    break;

  case 305:
#line 1789 "grammar19.y"
    {COND_POP();;}
    break;

  case 306:
#line 1792 "grammar19.y"
    {
                    (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (7)].node)), (yyvsp[(6) - (7)].node), 1);
                    fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                  ;}
    break;

  case 307:
#line 1799 "grammar19.y"
    {
                    (yyval.node) = NEW_CASE((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (5)].node));
                  ;}
    break;

  case 308:
#line 1804 "grammar19.y"
    {
                    (yyval.node) = NEW_CASE(0, (yyvsp[(3) - (4)].node));
                  ;}
    break;

  case 309:
#line 1808 "grammar19.y"
    {COND_PUSH(1);;}
    break;

  case 310:
#line 1810 "grammar19.y"
    {COND_POP();;}
    break;

  case 311:
#line 1813 "grammar19.y"
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

  case 312:
#line 1827 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("class definition in method body");
                    class_nest++;
                    local_push(0);
                    (yyval.num) = sourceline;
                  ;}
    break;

  case 313:
#line 1836 "grammar19.y"
    {
                    (yyval.node) = NEW_CLASS((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(3) - (6)].node));
                    nd_set_line((yyval.node), (yyvsp[(4) - (6)].num));
                    local_pop();
                    class_nest--;
                  ;}
    break;

  case 314:
#line 1843 "grammar19.y"
    {
                    (yyval.num) = in_def;
                    in_def = 0;
                  ;}
    break;

  case 315:
#line 1848 "grammar19.y"
    {
                    (yyval.num) = in_single;
                    in_single = 0;
                    class_nest++;
                    local_push(0);
                  ;}
    break;

  case 316:
#line 1856 "grammar19.y"
    {
                    (yyval.node) = NEW_SCLASS((yyvsp[(3) - (8)].node), (yyvsp[(7) - (8)].node));
                    fixpos((yyval.node), (yyvsp[(3) - (8)].node));
                    local_pop();
                    class_nest--;
                    in_def = (yyvsp[(4) - (8)].num);
                    in_single = (yyvsp[(6) - (8)].num);
                  ;}
    break;

  case 317:
#line 1865 "grammar19.y"
    {
                    if(in_def || in_single)
                      yy_error("module definition in method body");
                    class_nest++;
                    local_push(0);
                    (yyval.num) = sourceline;
                  ;}
    break;

  case 318:
#line 1874 "grammar19.y"
    {
                    (yyval.node) = NEW_MODULE((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
                    nd_set_line((yyval.node), (yyvsp[(3) - (5)].num));
                    local_pop();
                    class_nest--;
                  ;}
    break;

  case 319:
#line 1881 "grammar19.y"
    {
                    (yyval.id) = cur_mid;
                    cur_mid = (yyvsp[(2) - (2)].id);
                    in_def++;
                    local_push(0);
                  ;}
    break;

  case 320:
#line 1890 "grammar19.y"
    {
                    NODE* body = remove_begin((yyvsp[(5) - (6)].node));
                    (yyval.node) = NEW_DEFN((yyvsp[(2) - (6)].id), (yyvsp[(4) - (6)].node), body, NOEX_PRIVATE);
                    nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    local_pop();
                    in_def--;
                    cur_mid = (yyvsp[(3) - (6)].id);
                  ;}
    break;

  case 321:
#line 1898 "grammar19.y"
    {lex_state = EXPR_FNAME;;}
    break;

  case 322:
#line 1899 "grammar19.y"
    {
                    in_single++;
                    lex_state = EXPR_ENDFN; /* force for args */
                    local_push(0);
                  ;}
    break;

  case 323:
#line 1907 "grammar19.y"
    {
                    NODE* body = remove_begin((yyvsp[(8) - (9)].node));
                    (yyval.node) = NEW_DEFS((yyvsp[(2) - (9)].node), (yyvsp[(5) - (9)].id), (yyvsp[(7) - (9)].node), body);
                    nd_set_line((yyval.node), (yyvsp[(1) - (9)].num));
                    local_pop();
                    in_single--;
                  ;}
    break;

  case 324:
#line 1915 "grammar19.y"
    {
                    (yyval.node) = NEW_BREAK(0);
                  ;}
    break;

  case 325:
#line 1919 "grammar19.y"
    {
                    (yyval.node) = NEW_NEXT(0);
                  ;}
    break;

  case 326:
#line 1923 "grammar19.y"
    {
                    (yyval.node) = NEW_REDO();
                  ;}
    break;

  case 327:
#line 1927 "grammar19.y"
    {
                    (yyval.node) = NEW_RETRY();
                  ;}
    break;

  case 328:
#line 1933 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  ;}
    break;

  case 329:
#line 1941 "grammar19.y"
    {
                    token_info_push("begin");
                  ;}
    break;

  case 330:
#line 1947 "grammar19.y"
    {
                    token_info_push("if");
                  ;}
    break;

  case 331:
#line 1953 "grammar19.y"
    {
                    token_info_push("unless");
                  ;}
    break;

  case 332:
#line 1959 "grammar19.y"
    {
                    token_info_push("while");
                  ;}
    break;

  case 333:
#line 1965 "grammar19.y"
    {
                    token_info_push("until");
                  ;}
    break;

  case 334:
#line 1971 "grammar19.y"
    {
                    token_info_push("case");
                  ;}
    break;

  case 335:
#line 1977 "grammar19.y"
    {
                    token_info_push("for");
                  ;}
    break;

  case 336:
#line 1983 "grammar19.y"
    {
                    token_info_push("class");
                  ;}
    break;

  case 337:
#line 1989 "grammar19.y"
    {
                    token_info_push("module");
                  ;}
    break;

  case 338:
#line 1995 "grammar19.y"
    {
                    token_info_push("def");
                    (yyval.num) = sourceline;
                  ;}
    break;

  case 339:
#line 2002 "grammar19.y"
    {
                    token_info_pop("end");
                  ;}
    break;

  case 346:
#line 2020 "grammar19.y"
    {
                    (yyval.node) = NEW_IF(cond((yyvsp[(2) - (5)].node)), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (5)].node));
                  ;}
    break;

  case 348:
#line 2028 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 351:
#line 2038 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                  ;}
    break;

  case 352:
#line 2042 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 353:
#line 2048 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 354:
#line 2052 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 355:
#line 2058 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (1)].node), 0);
                  ;}
    break;

  case 356:
#line 2062 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(4) - (4)].id), 0);
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (4)].node), (yyval.node));
                  ;}
    break;

  case 357:
#line 2067 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(4) - (6)].id), 0);
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (6)].node), NEW_POSTARG((yyval.node), (yyvsp[(6) - (6)].node)));
                  ;}
    break;

  case 358:
#line 2072 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (3)].node), -1);
                  ;}
    break;

  case 359:
#line 2076 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN((yyvsp[(1) - (5)].node), NEW_POSTARG(-1, (yyvsp[(5) - (5)].node)));
                  ;}
    break;

  case 360:
#line 2080 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(2) - (2)].id), 0);
                    (yyval.node) = NEW_MASGN(0, (yyval.node));
                  ;}
    break;

  case 361:
#line 2085 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(2) - (4)].id), 0);
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG((yyval.node), (yyvsp[(4) - (4)].node)));
                  ;}
    break;

  case 362:
#line 2090 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, -1);
                  ;}
    break;

  case 363:
#line 2094 "grammar19.y"
    {
                    (yyval.node) = NEW_MASGN(0, NEW_POSTARG(-1, (yyvsp[(3) - (3)].node)));
                  ;}
    break;

  case 364:
#line 2100 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].id), 0, (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 365:
#line 2104 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (8)].node), (yyvsp[(3) - (8)].node), (yyvsp[(5) - (8)].id), (yyvsp[(7) - (8)].node), (yyvsp[(8) - (8)].id));
                  ;}
    break;

  case 366:
#line 2108 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), 0, 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 367:
#line 2112 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].node), 0, (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 368:
#line 2116 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (4)].node), 0, (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 369:
#line 2120 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (2)].node), 0, 1, 0, 0);
                  ;}
    break;

  case 370:
#line 2124 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), 0, (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 371:
#line 2128 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (2)].node), 0, 0, 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 372:
#line 2132 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 373:
#line 2136 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 374:
#line 2140 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (2)].node), 0, 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 375:
#line 2144 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (4)].node), 0, (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 376:
#line 2148 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[(1) - (2)].id), 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 377:
#line 2152 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[(1) - (4)].id), (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 378:
#line 2156 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, 0, 0, (yyvsp[(1) - (1)].id));
                  ;}
    break;

  case 380:
#line 2163 "grammar19.y"
    {
                    command_start = TRUE;
                  ;}
    break;

  case 381:
#line 2169 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node) ? NEW_ARGS_AUX(0,(yyvsp[(2) - (3)].node)) : 0;
                  ;}
    break;

  case 382:
#line 2173 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 383:
#line 2177 "grammar19.y"
    {
                    if((yyvsp[(3) - (4)].node)) {
                      (yyval.node) = NEW_ARGS_AUX((yyvsp[(2) - (4)].node), (yyvsp[(3) - (4)].node));
                    } else {
                      (yyval.node) = (yyvsp[(2) - (4)].node);
                    }
                  ;}
    break;

  case 384:
#line 2187 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 385:
#line 2191 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 386:
#line 2197 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 387:
#line 2201 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 388:
#line 2207 "grammar19.y"
    {
                    new_bv(get_id((yyvsp[(1) - (1)].id)));
                    (yyval.node) = NEW_LIT(ID2SYM(get_id((yyvsp[(1) - (1)].id))));
                  ;}
    break;

  case 389:
#line 2212 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 390:
#line 2217 "grammar19.y"
    {
                    (yyval.vars) = bv_push();
                  ;}
    break;

  case 391:
#line 2220 "grammar19.y"
    {
                    (yyval.num) = lpar_beg;
                    lpar_beg = ++paren_nest;
                  ;}
    break;

  case 392:
#line 2226 "grammar19.y"
    {
                    lpar_beg = (yyvsp[(2) - (4)].num);
                    (yyval.node) = (yyvsp[(3) - (4)].node);
                    (yyval.node)->nd_body = NEW_SCOPE((yyvsp[(3) - (4)].node)->nd_head, (yyvsp[(4) - (4)].node));
                    bv_pop((yyvsp[(1) - (4)].vars));
                  ;}
    break;

  case 393:
#line 2235 "grammar19.y"
    {
                    (yyval.node) = NEW_LAMBDA((yyvsp[(2) - (4)].node));
                  ;}
    break;

  case 394:
#line 2239 "grammar19.y"
    {
                    (yyval.node) = NEW_LAMBDA((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 395:
#line 2245 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 396:
#line 2249 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 397:
#line 2255 "grammar19.y"
    {
                    (yyvsp[(1) - (1)].vars) = bv_push();
                    (yyval.num) = sourceline;
                  ;}
    break;

  case 398:
#line 2262 "grammar19.y"
    {
                    (yyval.node) = NEW_ITER((yyvsp[(3) - (5)].node), (yyvsp[(4) - (5)].node));
                    nd_set_line((yyval.node), (yyvsp[(2) - (5)].num));
                    bv_pop((yyvsp[(1) - (5)].vars));
                  ;}
    break;

  case 399:
#line 2270 "grammar19.y"
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

  case 400:
#line 2281 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 401:
#line 2285 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                  ;}
    break;

  case 402:
#line 2291 "grammar19.y"
    {
                    (yyval.node) = NEW_FCALL((yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 403:
#line 2296 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 404:
#line 2301 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 405:
#line 2306 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), 0);
                  ;}
    break;

  case 406:
#line 2310 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (3)].node), parser_intern("call"), (yyvsp[(3) - (3)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (3)].node));
                  ;}
    break;

  case 407:
#line 2315 "grammar19.y"
    {
                    (yyval.node) = NEW_CALL((yyvsp[(1) - (3)].node), parser_intern("call"), (yyvsp[(3) - (3)].node));
                    fixpos((yyval.node), (yyvsp[(1) - (3)].node));
                  ;}
    break;

  case 408:
#line 2320 "grammar19.y"
    {
                    (yyval.node) = NEW_SUPER((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 409:
#line 2324 "grammar19.y"
    {
                    (yyval.node) = NEW_ZSUPER();
                  ;}
    break;

  case 410:
#line 2328 "grammar19.y"
    {
                    if((yyvsp[(1) - (4)].node) && nd_type((yyvsp[(1) - (4)].node)) == NODE_SELF) {
                      (yyval.node) = NEW_FCALL(tAREF, (yyvsp[(3) - (4)].node));
                    } else {
                      (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), tAREF, (yyvsp[(3) - (4)].node));
                    }
                    fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                  ;}
    break;

  case 411:
#line 2339 "grammar19.y"
    {
                    (yyvsp[(1) - (1)].vars) = bv_push();
                    (yyval.num) = sourceline;
                  ;}
    break;

  case 412:
#line 2345 "grammar19.y"
    {
                    (yyval.node) = NEW_ITER((yyvsp[(3) - (5)].node), (yyvsp[(4) - (5)].node));
                    nd_set_line((yyval.node), (yyvsp[(2) - (5)].num));
                    bv_pop((yyvsp[(1) - (5)].vars));
                  ;}
    break;

  case 413:
#line 2351 "grammar19.y"
    {
                    (yyvsp[(1) - (1)].vars) = bv_push();
                    (yyval.num) = sourceline;
                  ;}
    break;

  case 414:
#line 2357 "grammar19.y"
    {
                    (yyval.node) = NEW_ITER((yyvsp[(3) - (5)].node), (yyvsp[(4) - (5)].node));
                    nd_set_line((yyval.node), (yyvsp[(2) - (5)].num));
                    bv_pop((yyvsp[(1) - (5)].vars));
                  ;}
    break;

  case 415:
#line 2367 "grammar19.y"
    {
                    (yyval.node) = NEW_WHEN((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                  ;}
    break;

  case 418:
#line 2379 "grammar19.y"
    {
                    if((yyvsp[(3) - (6)].node)) {
                      /* TODO NEW_ERRINFO() */
                      (yyvsp[(3) - (6)].node) = node_assign((yyvsp[(3) - (6)].node), NEW_GVAR(parser_intern("$!")));
                      (yyvsp[(5) - (6)].node) = block_append((yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node));
                    }
                    (yyval.node) = NEW_RESBODY((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].node));
                    fixpos((yyval.node), (yyvsp[(2) - (6)].node) ? (yyvsp[(2) - (6)].node) : (yyvsp[(5) - (6)].node));
                  ;}
    break;

  case 420:
#line 2392 "grammar19.y"
    {
                    (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                  ;}
    break;

  case 421:
#line 2396 "grammar19.y"
    {
                    if(!((yyval.node) = splat_array((yyvsp[(1) - (1)].node)))) (yyval.node) = (yyvsp[(1) - (1)].node);
                  ;}
    break;

  case 423:
#line 2403 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 425:
#line 2410 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (2)].node);
                  ;}
    break;

  case 428:
#line 2418 "grammar19.y"
    {
                    (yyval.node) = NEW_LIT(ID2SYM((yyvsp[(1) - (1)].id)));
                  ;}
    break;

  case 430:
#line 2425 "grammar19.y"
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
#line 2439 "grammar19.y"
    {
                    (yyval.node) = literal_concat((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 434:
#line 2445 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 435:
#line 2451 "grammar19.y"
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
                        node = NEW_NODE(NODE_DXSTR, REF(STR_NEW0()), 1, NEW_LIST(node));
                        break;
                      }
                    }
                    (yyval.node) = node;
                  ;}
    break;

  case 436:
#line 2473 "grammar19.y"
    {
                    intptr_t options = (yyvsp[(3) - (3)].num);
                    NODE *node = (yyvsp[(2) - (3)].node);
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
                  ;}
    break;

  case 437:
#line 2533 "grammar19.y"
    {
                    (yyval.node) = NEW_ZARRAY();
                  ;}
    break;

  case 438:
#line 2537 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 439:
#line 2543 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 440:
#line 2547 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), evstr2dstr((yyvsp[(2) - (3)].node)));
                  ;}
    break;

  case 442:
#line 2554 "grammar19.y"
    {
                    (yyval.node) = literal_concat((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 443:
#line 2560 "grammar19.y"
    {
                    (yyval.node) = NEW_ZARRAY();
                  ;}
    break;

  case 444:
#line 2564 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 445:
#line 2570 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 446:
#line 2574 "grammar19.y"
    {
                    (yyval.node) = list_append((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                  ;}
    break;

  case 447:
#line 2580 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 448:
#line 2584 "grammar19.y"
    {
                    (yyval.node) = literal_concat((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 449:
#line 2590 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 450:
#line 2594 "grammar19.y"
    {
                    (yyval.node) = literal_concat((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 451:
#line 2600 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 452:
#line 2604 "grammar19.y"
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
#line 2628 "grammar19.y"
    {
                    (yyval.node) = lex_strterm;
                    lex_strterm = 0;
                    lex_state = EXPR_BEG;
                  ;}
    break;

  case 455:
#line 2634 "grammar19.y"
    {
                    lex_strterm = (yyvsp[(2) - (3)].node);
                    (yyval.node) = NEW_EVSTR((yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 456:
#line 2639 "grammar19.y"
    {
                    (yyvsp[(1) - (1)].val) = cond_stack;
                    (yyval.val) = cmdarg_stack;
                    cond_stack = 0;
                    cmdarg_stack = 0;
                  ;}
    break;

  case 457:
#line 2645 "grammar19.y"
    {
                    (yyval.node) = lex_strterm;
                    lex_strterm = 0;
                    lex_state = EXPR_BEG;
                  ;}
    break;

  case 458:
#line 2651 "grammar19.y"
    {
                    cond_stack = (yyvsp[(1) - (5)].val);
                    cmdarg_stack = (yyvsp[(2) - (5)].val);
                    lex_strterm = (yyvsp[(3) - (5)].node);

                    if((yyvsp[(4) - (5)].node)) (yyvsp[(4) - (5)].node)->flags &= ~NODE_FL_NEWLINE;
                    (yyval.node) = new_evstr((yyvsp[(4) - (5)].node));
                  ;}
    break;

  case 459:
#line 2661 "grammar19.y"
    {(yyval.node) = NEW_GVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 460:
#line 2662 "grammar19.y"
    {(yyval.node) = NEW_IVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 461:
#line 2663 "grammar19.y"
    {(yyval.node) = NEW_CVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 463:
#line 2668 "grammar19.y"
    {
                    lex_state = EXPR_END;
                    (yyval.id) = (yyvsp[(2) - (2)].id);
                  ;}
    break;

  case 468:
#line 2681 "grammar19.y"
    {
                    lex_state = EXPR_END;
                    if(!((yyval.node) = (yyvsp[(2) - (3)].node))) {
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
                  ;}
    break;

  case 471:
#line 2708 "grammar19.y"
    {
                    (yyval.node) = negate_lit((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 472:
#line 2712 "grammar19.y"
    {
                    (yyval.node) = negate_lit((yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 478:
#line 2724 "grammar19.y"
    {(yyval.id) = keyword_nil;;}
    break;

  case 479:
#line 2725 "grammar19.y"
    {(yyval.id) = keyword_self;;}
    break;

  case 480:
#line 2726 "grammar19.y"
    {(yyval.id) = keyword_true;;}
    break;

  case 481:
#line 2727 "grammar19.y"
    {(yyval.id) = keyword_false;;}
    break;

  case 482:
#line 2728 "grammar19.y"
    {(yyval.id) = keyword__FILE__;;}
    break;

  case 483:
#line 2729 "grammar19.y"
    {(yyval.id) = keyword__LINE__;;}
    break;

  case 484:
#line 2730 "grammar19.y"
    {(yyval.id) = keyword__ENCODING__;;}
    break;

  case 485:
#line 2734 "grammar19.y"
    {
                    if(!((yyval.node) = gettable((yyvsp[(1) - (1)].id)))) {
                      (yyval.node) = NEW_BEGIN(0);
                    }
                  ;}
    break;

  case 486:
#line 2740 "grammar19.y"
    {
                    if(!((yyval.node) = gettable((yyvsp[(1) - (1)].id)))) {
                      (yyval.node) = NEW_BEGIN(0);
                    }
                  ;}
    break;

  case 487:
#line 2748 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                  ;}
    break;

  case 488:
#line 2752 "grammar19.y"
    {
                    (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0);
                  ;}
    break;

  case 491:
#line 2762 "grammar19.y"
    {
                    (yyval.node) = 0;
                  ;}
    break;

  case 492:
#line 2766 "grammar19.y"
    {
                    lex_state = EXPR_BEG;
                  ;}
    break;

  case 493:
#line 2770 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(3) - (4)].node);
                  ;}
    break;

  case 494:
#line 2774 "grammar19.y"
    {
                    yyerrok;
                    (yyval.node) = 0;
                  ;}
    break;

  case 495:
#line 2781 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(2) - (3)].node);
                    lex_state = EXPR_BEG;
                    command_start = TRUE;
                  ;}
    break;

  case 496:
#line 2787 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                    lex_state = EXPR_BEG;
                    command_start = TRUE;
                  ;}
    break;

  case 497:
#line 2795 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].id), 0, (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 498:
#line 2799 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (8)].node), (yyvsp[(3) - (8)].node), (yyvsp[(5) - (8)].id), (yyvsp[(7) - (8)].node), (yyvsp[(8) - (8)].id));
                  ;}
    break;

  case 499:
#line 2803 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), 0, 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 500:
#line 2807 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].node), 0, (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 501:
#line 2811 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (4)].node), 0, (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 502:
#line 2815 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (6)].node), 0, (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 503:
#line 2819 "grammar19.y"
    {
                    (yyval.node) = new_args((yyvsp[(1) - (2)].node), 0, 0, 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 504:
#line 2823 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 505:
#line 2827 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].id));
                  ;}
    break;

  case 506:
#line 2831 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (2)].node), 0, 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 507:
#line 2835 "grammar19.y"
    {
                    (yyval.node) = new_args(0, (yyvsp[(1) - (4)].node), 0, (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 508:
#line 2839 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[(1) - (2)].id), 0, (yyvsp[(2) - (2)].id));
                  ;}
    break;

  case 509:
#line 2843 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, (yyvsp[(1) - (4)].id), (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].id));
                  ;}
    break;

  case 510:
#line 2847 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, 0, 0, (yyvsp[(1) - (1)].id));
                  ;}
    break;

  case 511:
#line 2851 "grammar19.y"
    {
                    (yyval.node) = new_args(0, 0, 0, 0, 0);
                  ;}
    break;

  case 512:
#line 2857 "grammar19.y"
    {
                    yy_error("formal argument cannot be a constant");
                    (yyval.id) = 0;
                  ;}
    break;

  case 513:
#line 2862 "grammar19.y"
    {
                    yy_error("formal argument cannot be an instance variable");
                    (yyval.id) = 0;
                  ;}
    break;

  case 514:
#line 2867 "grammar19.y"
    {
                    yy_error("formal argument cannot be a global variable");
                    (yyval.id) = 0;
                  ;}
    break;

  case 515:
#line 2872 "grammar19.y"
    {
                    yy_error("formal argument cannot be a class variable");
                    (yyval.id) = 0;
                  ;}
    break;

  case 517:
#line 2880 "grammar19.y"
    {
                    formal_argument(get_id((yyvsp[(1) - (1)].id)));
                    (yyval.id) = (yyvsp[(1) - (1)].id);
                  ;}
    break;

  case 518:
#line 2887 "grammar19.y"
    {
                    arg_var(get_id((yyvsp[(1) - (1)].id)));
                    (yyval.node) = NEW_ARGS_AUX((yyvsp[(1) - (1)].id), 1);
                  ;}
    break;

  case 519:
#line 2892 "grammar19.y"
    {
                    ID tid = internal_id();
                    arg_var(tid);
                    (yyvsp[(2) - (3)].node)->nd_value = NEW_LVAR(tid);
                    (yyval.node) = NEW_ARGS_AUX(tid, 1);
                    (yyval.node)->nd_next = (yyvsp[(2) - (3)].node);
                  ;}
    break;

  case 521:
#line 2903 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                    (yyval.node)->nd_plen++;
                    (yyval.node)->nd_next = block_append((yyval.node)->nd_next, (yyvsp[(3) - (3)].node)->nd_next);
                  ;}
    break;

  case 522:
#line 2911 "grammar19.y"
    {
                    arg_var(formal_argument(get_id((yyvsp[(1) - (3)].id))));
                    (yyval.node) = assignable((yyvsp[(1) - (3)].id), (yyvsp[(3) - (3)].node));
                    (yyval.node) = NEW_OPT_ARG(0, (yyval.node));
                  ;}
    break;

  case 523:
#line 2919 "grammar19.y"
    {
                    arg_var(formal_argument(get_id((yyvsp[(1) - (3)].id))));
                    (yyval.node) = assignable((yyvsp[(1) - (3)].id), (yyvsp[(3) - (3)].node));
                    (yyval.node) = NEW_OPT_ARG(0, (yyval.node));
                  ;}
    break;

  case 524:
#line 2927 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                  ;}
    break;

  case 525:
#line 2931 "grammar19.y"
    {
                    NODE *opts = (yyvsp[(1) - (3)].node);
                    while(opts->nd_next) {
                      opts = opts->nd_next;
                    }
                    opts->nd_next = (yyvsp[(3) - (3)].node);
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 526:
#line 2942 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                  ;}
    break;

  case 527:
#line 2946 "grammar19.y"
    {
                    NODE *opts = (yyvsp[(1) - (3)].node);
                    while(opts->nd_next) {
                      opts = opts->nd_next;
                    }
                    opts->nd_next = (yyvsp[(3) - (3)].node);
                    (yyval.node) = (yyvsp[(1) - (3)].node);
                  ;}
    break;

  case 530:
#line 2961 "grammar19.y"
    {
                    if(!is_local_id((yyvsp[(2) - (2)].id))) {
                      yy_error("rest argument must be local variable");
                    }
                    arg_var(shadowing_lvar(get_id((yyvsp[(2) - (2)].id))));
                    (yyval.id) = (yyvsp[(2) - (2)].id);
                  ;}
    break;

  case 531:
#line 2969 "grammar19.y"
    {
                    (yyval.id) = internal_id();
                    arg_var((yyval.id));
                  ;}
    break;

  case 534:
#line 2980 "grammar19.y"
    {
                    if(!is_local_id((yyvsp[(2) - (2)].id)))
                      yy_error("block argument must be local variable");
                    else if(!in_block() && local_id((yyvsp[(2) - (2)].id)))
                      yy_error("duplicate block argument name");
                    arg_var(shadowing_lvar(get_id((yyvsp[(2) - (2)].id))));
                    (yyval.id) = (yyvsp[(2) - (2)].id);
                  ;}
    break;

  case 535:
#line 2991 "grammar19.y"
    {
                    (yyval.id) = (yyvsp[(2) - (2)].id);
                  ;}
    break;

  case 536:
#line 2995 "grammar19.y"
    {
                    (yyval.id) = 0;
                  ;}
    break;

  case 537:
#line 3001 "grammar19.y"
    {
                    value_expr((yyvsp[(1) - (1)].node));
                    (yyval.node) = (yyvsp[(1) - (1)].node);
                    if(!(yyval.node)) (yyval.node) = NEW_NIL();
                  ;}
    break;

  case 538:
#line 3006 "grammar19.y"
    {lex_state = EXPR_BEG;;}
    break;

  case 539:
#line 3007 "grammar19.y"
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

  case 541:
#line 3032 "grammar19.y"
    {
                    (yyval.node) = (yyvsp[(1) - (2)].node);
                  ;}
    break;

  case 543:
#line 3039 "grammar19.y"
    {
                    (yyval.node) = list_concat((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 544:
#line 3045 "grammar19.y"
    {
                    (yyval.node) = list_append(NEW_LIST((yyvsp[(1) - (3)].node)), (yyvsp[(3) - (3)].node));
                  ;}
    break;

  case 545:
#line 3049 "grammar19.y"
    {
                    (yyval.node) = list_append(NEW_LIST(NEW_LIT(ID2SYM((yyvsp[(1) - (2)].id)))), (yyvsp[(2) - (2)].node));
                  ;}
    break;

  case 567:
#line 3093 "grammar19.y"
    {yyerrok;;}
    break;

  case 570:
#line 3098 "grammar19.y"
    {yyerrok;;}
    break;

  case 571:
#line 3101 "grammar19.y"
    {(yyval.node) = 0;;}
    break;


/* Line 1267 of yacc.c.  */
#line 8283 "grammar19.cpp"
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


#line 3103 "grammar19.y"


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
  VALUE str = parser_enc_str_new(name, len, enc);
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
  return parser_intern3(RSTRING_PTR(str), RSTRING_LEN(str), parser_enc_get(str));
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


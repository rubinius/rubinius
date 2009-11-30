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
     kCLASS = 258,
     kMODULE = 259,
     kDEF = 260,
     kUNDEF = 261,
     kBEGIN = 262,
     kRESCUE = 263,
     kENSURE = 264,
     kEND = 265,
     kIF = 266,
     kUNLESS = 267,
     kTHEN = 268,
     kELSIF = 269,
     kELSE = 270,
     kCASE = 271,
     kWHEN = 272,
     kWHILE = 273,
     kUNTIL = 274,
     kFOR = 275,
     kBREAK = 276,
     kNEXT = 277,
     kREDO = 278,
     kRETRY = 279,
     kIN = 280,
     kDO = 281,
     kDO_COND = 282,
     kDO_BLOCK = 283,
     kRETURN = 284,
     kYIELD = 285,
     kSUPER = 286,
     kSELF = 287,
     kNIL = 288,
     kTRUE = 289,
     kFALSE = 290,
     kAND = 291,
     kOR = 292,
     kNOT = 293,
     kIF_MOD = 294,
     kUNLESS_MOD = 295,
     kWHILE_MOD = 296,
     kUNTIL_MOD = 297,
     kRESCUE_MOD = 298,
     kALIAS = 299,
     kDEFINED = 300,
     klBEGIN = 301,
     klEND = 302,
     k__LINE__ = 303,
     k__FILE__ = 304,
     k__END__ = 305,
     tIDENTIFIER = 306,
     tFID = 307,
     tGVAR = 308,
     tIVAR = 309,
     tCONSTANT = 310,
     tCVAR = 311,
     tXSTRING_BEG = 312,
     tINTEGER = 313,
     tFLOAT = 314,
     tSTRING_CONTENT = 315,
     tEND_DATA = 316,
     tNTH_REF = 317,
     tBACK_REF = 318,
     tREGEXP_END = 319,
     tUPLUS = 320,
     tUMINUS = 321,
     tUBS = 322,
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
     tSYMBEG = 352,
     tSTRING_BEG = 353,
     tREGEXP_BEG = 354,
     tWORDS_BEG = 355,
     tQWORDS_BEG = 356,
     tSTRING_DBEG = 357,
     tSTRING_DVAR = 358,
     tSTRING_END = 359,
     tLOWEST = 360,
     tUMINUS_NUM = 361,
     tLAST_TOKEN = 362
   };
#endif
/* Tokens.  */
#define kCLASS 258
#define kMODULE 259
#define kDEF 260
#define kUNDEF 261
#define kBEGIN 262
#define kRESCUE 263
#define kENSURE 264
#define kEND 265
#define kIF 266
#define kUNLESS 267
#define kTHEN 268
#define kELSIF 269
#define kELSE 270
#define kCASE 271
#define kWHEN 272
#define kWHILE 273
#define kUNTIL 274
#define kFOR 275
#define kBREAK 276
#define kNEXT 277
#define kREDO 278
#define kRETRY 279
#define kIN 280
#define kDO 281
#define kDO_COND 282
#define kDO_BLOCK 283
#define kRETURN 284
#define kYIELD 285
#define kSUPER 286
#define kSELF 287
#define kNIL 288
#define kTRUE 289
#define kFALSE 290
#define kAND 291
#define kOR 292
#define kNOT 293
#define kIF_MOD 294
#define kUNLESS_MOD 295
#define kWHILE_MOD 296
#define kUNTIL_MOD 297
#define kRESCUE_MOD 298
#define kALIAS 299
#define kDEFINED 300
#define klBEGIN 301
#define klEND 302
#define k__LINE__ 303
#define k__FILE__ 304
#define k__END__ 305
#define tIDENTIFIER 306
#define tFID 307
#define tGVAR 308
#define tIVAR 309
#define tCONSTANT 310
#define tCVAR 311
#define tXSTRING_BEG 312
#define tINTEGER 313
#define tFLOAT 314
#define tSTRING_CONTENT 315
#define tEND_DATA 316
#define tNTH_REF 317
#define tBACK_REF 318
#define tREGEXP_END 319
#define tUPLUS 320
#define tUMINUS 321
#define tUBS 322
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
#define tSYMBEG 352
#define tSTRING_BEG 353
#define tREGEXP_BEG 354
#define tWORDS_BEG 355
#define tQWORDS_BEG 356
#define tSTRING_DBEG 357
#define tSTRING_DVAR 358
#define tSTRING_END 359
#define tLOWEST 360
#define tUMINUS_NUM 361
#define tLAST_TOKEN 362




/* Copy the first part of user declarations.  */
#line 13 "grammar.y"


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

#define RE_OPTION_ONCE 0x80
#define RE_OPTION_IGNORECASE (1L)
#define RE_OPTION_EXTENDED   (RE_OPTION_IGNORECASE<<1)
#define RE_OPTION_MULTILINE  (RE_OPTION_EXTENDED<<1)
#define RE_OPTION_SINGLELINE (RE_OPTION_MULTILINE<<1)
#define RE_OPTION_LONGEST    (RE_OPTION_SINGLELINE<<1)
#define RE_MAY_IGNORECASE    (RE_OPTION_LONGEST<<1)
#define RE_OPTIMIZE_ANCHOR   (RE_MAY_IGNORECASE<<1)
#define RE_OPTIMIZE_EXACTN   (RE_OPTIMIZE_ANCHOR<<1)
#define RE_OPTIMIZE_NO_BM    (RE_OPTIMIZE_EXACTN<<1)
#define RE_OPTIMIZE_BMATCH   (RE_OPTIMIZE_NO_BM<<1)

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

/* Older versions of Yacc set YYMAXDEPTH to a very low value by default (150,
   for instance).  This is too low for Ruby to parse some files, such as
   date/format.rb, therefore bump the value up to at least Bison's default. */
#ifdef OLD_YACC
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif
#endif

#define vps ((rb_parse_state*)parse_state)



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
#line 295 "grammar.y"
{
    NODE *node;
    QUID id;
    int num;
    var_table vars;
}
/* Line 193 of yacc.c.  */
#line 597 "grammar.cpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 610 "grammar.cpp"

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
#define YYLAST   9761

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  136
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  152
/* YYNRULES -- Number of rules.  */
#define YYNRULES  514
/* YYNRULES -- Number of states.  */
#define YYNSTATES  912

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   362

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     134,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,   133,   120,     2,     2,     2,   118,   113,     2,
     131,   128,   116,   114,   129,   115,   127,   117,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   108,   135,
     110,   106,   109,   107,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   125,   132,   126,   112,     2,   130,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   123,   111,   124,   121,     2,     2,     2,
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
     105,   119,   122
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     8,    13,    16,    18,    21,    23,
      25,    29,    32,    33,    38,    42,    46,    50,    53,    57,
      61,    65,    69,    73,    74,    80,    85,    89,    93,    97,
     104,   110,   116,   122,   126,   130,   134,   138,   140,   142,
     146,   150,   153,   156,   158,   160,   162,   164,   167,   170,
     173,   175,   180,   185,   186,   187,   194,   197,   201,   206,
     212,   217,   223,   226,   229,   231,   235,   237,   241,   243,
     246,   250,   253,   256,   258,   260,   264,   267,   271,   273,
     278,   282,   286,   290,   294,   297,   299,   301,   306,   310,
     314,   318,   322,   325,   327,   329,   331,   334,   336,   340,
     342,   344,   346,   348,   350,   352,   354,   356,   358,   360,
     361,   366,   368,   370,   372,   374,   376,   378,   380,   382,
     384,   386,   388,   390,   392,   394,   396,   398,   400,   402,
     404,   406,   408,   410,   412,   414,   416,   418,   420,   422,
     424,   426,   428,   430,   432,   434,   436,   438,   440,   442,
     444,   446,   448,   450,   452,   454,   456,   458,   460,   462,
     464,   466,   468,   470,   472,   474,   476,   478,   480,   482,
     484,   486,   488,   490,   492,   494,   496,   498,   500,   502,
     506,   512,   516,   523,   529,   535,   541,   547,   552,   556,
     560,   564,   568,   572,   576,   580,   584,   588,   593,   598,
     601,   604,   608,   612,   616,   620,   624,   628,   632,   636,
     640,   644,   648,   652,   656,   659,   662,   666,   670,   674,
     678,   679,   684,   685,   692,   694,   696,   698,   701,   704,
     710,   713,   717,   721,   726,   731,   738,   740,   742,   744,
     747,   753,   756,   762,   767,   775,   779,   781,   786,   790,
     796,   804,   807,   813,   818,   825,   833,   843,   847,   849,
     850,   853,   855,   856,   860,   861,   866,   869,   872,   874,
     876,   880,   884,   889,   892,   894,   896,   898,   900,   902,
     904,   906,   908,   910,   911,   916,   917,   923,   927,   931,
     934,   939,   943,   947,   949,   954,   958,   960,   961,   968,
     971,   973,   976,   977,   985,   986,   994,   995,   996,  1004,
    1005,  1006,  1014,  1015,  1022,  1023,  1029,  1030,  1037,  1038,
    1039,  1040,  1051,  1052,  1059,  1060,  1061,  1070,  1071,  1077,
    1078,  1085,  1086,  1087,  1097,  1099,  1101,  1103,  1105,  1107,
    1109,  1111,  1113,  1116,  1118,  1120,  1122,  1124,  1130,  1132,
    1135,  1137,  1139,  1141,  1144,  1146,  1150,  1151,  1152,  1159,
    1162,  1167,  1172,  1175,  1180,  1185,  1189,  1193,  1196,  1199,
    1201,  1202,  1203,  1210,  1211,  1212,  1219,  1225,  1227,  1232,
    1235,  1237,  1239,  1246,  1248,  1250,  1252,  1254,  1257,  1259,
    1262,  1264,  1266,  1268,  1270,  1272,  1274,  1277,  1281,  1285,
    1289,  1293,  1297,  1298,  1302,  1304,  1307,  1311,  1315,  1316,
    1320,  1321,  1324,  1325,  1328,  1330,  1331,  1335,  1336,  1341,
    1343,  1345,  1347,  1349,  1352,  1354,  1356,  1358,  1360,  1364,
    1366,  1368,  1371,  1374,  1376,  1378,  1380,  1382,  1384,  1386,
    1388,  1390,  1392,  1394,  1396,  1398,  1400,  1402,  1404,  1406,
    1407,  1412,  1415,  1420,  1423,  1430,  1435,  1440,  1443,  1448,
    1451,  1454,  1456,  1457,  1459,  1461,  1463,  1465,  1467,  1469,
    1473,  1477,  1479,  1483,  1485,  1487,  1490,  1492,  1494,  1496,
    1499,  1502,  1504,  1506,  1507,  1513,  1515,  1518,  1521,  1523,
    1527,  1531,  1533,  1535,  1537,  1539,  1541,  1543,  1545,  1547,
    1549,  1551,  1553,  1555,  1556,  1558,  1559,  1561,  1562,  1564,
    1566,  1568,  1570,  1572,  1575
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     137,     0,    -1,    -1,   138,   140,   141,    -1,   140,   231,
     215,   234,    -1,   142,   282,    -1,   287,    -1,    50,    61,
      -1,   287,    -1,   143,    -1,   142,   286,   143,    -1,     1,
     143,    -1,    -1,    44,   165,   144,   165,    -1,    44,    53,
      53,    -1,    44,    53,    63,    -1,    44,    53,    62,    -1,
       6,   166,    -1,   143,    39,   147,    -1,   143,    40,   147,
      -1,   143,    41,   147,    -1,   143,    42,   147,    -1,   143,
      43,   143,    -1,    -1,    46,   145,   123,   140,   124,    -1,
      47,   123,   140,   124,    -1,   160,   106,   148,    -1,   154,
     106,   148,    -1,   258,    87,   148,    -1,   211,   125,   174,
     126,    87,   148,    -1,   211,   127,    51,    87,   148,    -1,
     211,   127,    55,    87,   148,    -1,   211,    85,    51,    87,
     148,    -1,   259,    87,   148,    -1,   160,   106,   187,    -1,
     154,   106,   173,    -1,   154,   106,   187,    -1,   146,    -1,
     148,    -1,   146,    36,   146,    -1,   146,    37,   146,    -1,
      38,   146,    -1,   120,   148,    -1,   170,    -1,   146,    -1,
     153,    -1,   149,    -1,    29,   177,    -1,    21,   177,    -1,
      22,   177,    -1,   221,    -1,   221,   127,   279,   179,    -1,
     221,    85,   279,   179,    -1,    -1,    -1,    94,   151,   217,
     152,   140,   124,    -1,   278,   179,    -1,   278,   179,   150,
      -1,   211,   127,   279,   179,    -1,   211,   127,   279,   179,
     150,    -1,   211,    85,   279,   179,    -1,   211,    85,   279,
     179,   150,    -1,    31,   179,    -1,    30,   179,    -1,   156,
      -1,    89,   155,   128,    -1,   156,    -1,    89,   155,   128,
      -1,   158,    -1,   158,   157,    -1,   158,    95,   159,    -1,
     158,    95,    -1,    95,   159,    -1,    95,    -1,   159,    -1,
      89,   155,   128,    -1,   157,   129,    -1,   158,   157,   129,
      -1,   256,    -1,   211,   125,   174,   126,    -1,   211,   127,
      51,    -1,   211,    85,    51,    -1,   211,   127,    55,    -1,
     211,    85,    55,    -1,    86,    55,    -1,   259,    -1,   256,
      -1,   211,   125,   174,   126,    -1,   211,   127,    51,    -1,
     211,    85,    51,    -1,   211,   127,    55,    -1,   211,    85,
      55,    -1,    86,    55,    -1,   259,    -1,    51,    -1,    55,
      -1,    86,   161,    -1,   161,    -1,   211,    85,   161,    -1,
      51,    -1,    55,    -1,    52,    -1,   168,    -1,   169,    -1,
     163,    -1,   252,    -1,   164,    -1,   254,    -1,   165,    -1,
      -1,   166,   129,   167,   165,    -1,   111,    -1,   112,    -1,
     113,    -1,    69,    -1,    70,    -1,    71,    -1,    77,    -1,
     109,    -1,    73,    -1,   110,    -1,    74,    -1,    83,    -1,
      84,    -1,   114,    -1,   115,    -1,   116,    -1,    95,    -1,
     117,    -1,   118,    -1,    68,    -1,   121,    -1,    65,    -1,
      66,    -1,    81,    -1,    82,    -1,   130,    -1,    48,    -1,
      49,    -1,    46,    -1,    47,    -1,    50,    -1,    44,    -1,
      36,    -1,     7,    -1,    21,    -1,    16,    -1,     3,    -1,
       5,    -1,    45,    -1,    26,    -1,    15,    -1,    14,    -1,
      10,    -1,     9,    -1,    35,    -1,    20,    -1,    25,    -1,
       4,    -1,    22,    -1,    33,    -1,    38,    -1,    37,    -1,
      23,    -1,     8,    -1,    24,    -1,    29,    -1,    32,    -1,
      31,    -1,    13,    -1,    34,    -1,     6,    -1,    17,    -1,
      30,    -1,    39,    -1,    40,    -1,    41,    -1,    42,    -1,
      43,    -1,   160,   106,   170,    -1,   160,   106,   170,    43,
     170,    -1,   258,    87,   170,    -1,   211,   125,   174,   126,
      87,   170,    -1,   211,   127,    51,    87,   170,    -1,   211,
     127,    55,    87,   170,    -1,   211,    85,    51,    87,   170,
      -1,   211,    85,    55,    87,   170,    -1,    86,    55,    87,
     170,    -1,   259,    87,   170,    -1,   170,    79,   170,    -1,
     170,    80,   170,    -1,   170,   114,   170,    -1,   170,   115,
     170,    -1,   170,   116,   170,    -1,   170,   117,   170,    -1,
     170,   118,   170,    -1,   170,    68,   170,    -1,   119,    58,
      68,   170,    -1,   119,    59,    68,   170,    -1,    65,   170,
      -1,    66,   170,    -1,   170,   111,   170,    -1,   170,   112,
     170,    -1,   170,   113,   170,    -1,   170,    69,   170,    -1,
     170,   109,   170,    -1,   170,    73,   170,    -1,   170,   110,
     170,    -1,   170,    74,   170,    -1,   170,    70,   170,    -1,
     170,    71,   170,    -1,   170,    72,   170,    -1,   170,    77,
     170,    -1,   170,    78,   170,    -1,   120,   170,    -1,   121,
     170,    -1,   170,    83,   170,    -1,   170,    84,   170,    -1,
     170,    75,   170,    -1,   170,    76,   170,    -1,    -1,    45,
     283,   171,   170,    -1,    -1,   170,   107,   172,   170,   108,
     170,    -1,   188,    -1,   170,    -1,   287,    -1,   153,   283,
      -1,   186,   284,    -1,   186,   129,    95,   170,   283,    -1,
     276,   284,    -1,    95,   170,   283,    -1,   131,   287,   128,
      -1,   131,   177,   283,   128,    -1,   131,   221,   283,   128,
      -1,   131,   186,   129,   221,   283,   128,    -1,   287,    -1,
     175,    -1,   153,    -1,   186,   185,    -1,   186,   129,    95,
     173,   185,    -1,   276,   185,    -1,   276,   129,    95,   173,
     185,    -1,   186,   129,   276,   185,    -1,   186,   129,   276,
     129,    95,   170,   185,    -1,    95,   173,   185,    -1,   184,
      -1,   173,   129,   186,   185,    -1,   173,   129,   184,    -1,
     173,   129,    95,   173,   185,    -1,   173,   129,   186,   129,
      95,   173,   185,    -1,   276,   185,    -1,   276,   129,    95,
     173,   185,    -1,   173,   129,   276,   185,    -1,   173,   129,
     186,   129,   276,   185,    -1,   173,   129,   276,   129,    95,
     173,   185,    -1,   173,   129,   186,   129,   276,   129,    95,
     173,   185,    -1,    95,   173,   185,    -1,   184,    -1,    -1,
     180,   181,    -1,   177,    -1,    -1,    90,   182,   128,    -1,
      -1,    90,   178,   183,   128,    -1,    96,   173,    -1,   129,
     184,    -1,   287,    -1,   173,    -1,   186,   129,   173,    -1,
     186,   129,   173,    -1,   186,   129,    95,   173,    -1,    95,
     173,    -1,   235,    -1,   236,    -1,   239,    -1,   240,    -1,
     241,    -1,   244,    -1,   257,    -1,   259,    -1,    52,    -1,
      -1,     7,   189,   139,    10,    -1,    -1,    90,   146,   190,
     283,   128,    -1,    89,   140,   128,    -1,   211,    85,    55,
      -1,    86,    55,    -1,   211,   125,   174,   126,    -1,    92,
     174,   126,    -1,    93,   275,   124,    -1,    29,    -1,    30,
     131,   177,   128,    -1,    30,   131,   128,    -1,    30,    -1,
      -1,    45,   283,   131,   191,   146,   128,    -1,   278,   223,
      -1,   222,    -1,   222,   223,    -1,    -1,    11,   192,   147,
     212,   140,   214,    10,    -1,    -1,    12,   193,   147,   212,
     140,   215,    10,    -1,    -1,    -1,    18,   194,   147,   213,
     195,   140,    10,    -1,    -1,    -1,    19,   196,   147,   213,
     197,   140,    10,    -1,    -1,    16,   198,   147,   282,   228,
      10,    -1,    -1,    16,   282,   199,   228,    10,    -1,    -1,
      16,   282,   200,    15,   140,    10,    -1,    -1,    -1,    -1,
      20,   201,   216,    25,   202,   147,   213,   203,   140,    10,
      -1,    -1,     3,   162,   260,   204,   139,    10,    -1,    -1,
      -1,     3,    83,   146,   205,   285,   206,   139,    10,    -1,
      -1,     4,   162,   207,   139,    10,    -1,    -1,     5,   163,
     208,   262,   139,    10,    -1,    -1,    -1,     5,   273,   281,
     209,   163,   210,   262,   139,    10,    -1,    21,    -1,    22,
      -1,    23,    -1,    24,    -1,   188,    -1,   285,    -1,   108,
      -1,    13,    -1,   285,    13,    -1,   285,    -1,   108,    -1,
      27,    -1,   215,    -1,    14,   147,   212,   140,   214,    -1,
     287,    -1,    15,   140,    -1,   160,    -1,   154,    -1,   287,
      -1,   111,   111,    -1,    76,    -1,   111,   216,   111,    -1,
      -1,    -1,    28,   219,   217,   220,   140,    10,    -1,   153,
     218,    -1,   221,   127,   279,   176,    -1,   221,    85,   279,
     176,    -1,   278,   175,    -1,   211,   127,   279,   176,    -1,
     211,    85,   279,   175,    -1,   211,    85,   280,    -1,   211,
     132,   279,    -1,    67,   279,    -1,    31,   175,    -1,    31,
      -1,    -1,    -1,   123,   224,   217,   225,   140,   124,    -1,
      -1,    -1,    26,   226,   217,   227,   140,    10,    -1,    17,
     229,   212,   140,   230,    -1,   186,    -1,   186,   129,    95,
     173,    -1,    95,   173,    -1,   215,    -1,   228,    -1,     8,
     232,   233,   212,   140,   231,    -1,   287,    -1,   173,    -1,
     187,    -1,   287,    -1,    88,   160,    -1,   287,    -1,     9,
     140,    -1,   287,    -1,   255,    -1,   252,    -1,   254,    -1,
     237,    -1,   238,    -1,   237,   238,    -1,    98,   246,   104,
      -1,    57,   247,   104,    -1,    99,   247,    64,    -1,   100,
     133,   104,    -1,   100,   242,   104,    -1,    -1,   242,   243,
     133,    -1,   248,    -1,   243,   248,    -1,   101,   133,   104,
      -1,   101,   245,   104,    -1,    -1,   245,    60,   133,    -1,
      -1,   246,   248,    -1,    -1,   247,   248,    -1,    60,    -1,
      -1,   103,   249,   251,    -1,    -1,   102,   250,   140,   124,
      -1,    53,    -1,    54,    -1,    56,    -1,   259,    -1,    97,
     253,    -1,   163,    -1,    54,    -1,    53,    -1,    56,    -1,
      97,   247,   104,    -1,    58,    -1,    59,    -1,   119,    58,
      -1,   119,    59,    -1,    51,    -1,    54,    -1,    53,    -1,
      55,    -1,    56,    -1,    33,    -1,    32,    -1,    34,    -1,
      35,    -1,    49,    -1,    48,    -1,   256,    -1,   256,    -1,
      62,    -1,    63,    -1,   285,    -1,    -1,   110,   261,   147,
     285,    -1,     1,   285,    -1,   131,   263,   283,   128,    -1,
     263,   285,    -1,   265,   129,   267,   129,   269,   272,    -1,
     265,   129,   267,   272,    -1,   265,   129,   269,   272,    -1,
     265,   272,    -1,   267,   129,   269,   272,    -1,   267,   272,
      -1,   269,   272,    -1,   271,    -1,    -1,    55,    -1,    54,
      -1,    53,    -1,    56,    -1,    51,    -1,   264,    -1,   265,
     129,   264,    -1,    51,   106,   173,    -1,   266,    -1,   267,
     129,   266,    -1,   116,    -1,    95,    -1,   268,    51,    -1,
     268,    -1,   113,    -1,    96,    -1,   270,    51,    -1,   129,
     271,    -1,   287,    -1,   257,    -1,    -1,   131,   274,   146,
     283,   128,    -1,   287,    -1,   276,   284,    -1,   186,   284,
      -1,   277,    -1,   276,   129,   277,    -1,   173,    88,   173,
      -1,    51,    -1,    55,    -1,    52,    -1,    51,    -1,    55,
      -1,    52,    -1,   168,    -1,    51,    -1,    52,    -1,   168,
      -1,   127,    -1,    85,    -1,    -1,   286,    -1,    -1,   134,
      -1,    -1,   134,    -1,   129,    -1,   135,    -1,   134,    -1,
     285,    -1,   286,   135,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   433,   433,   433,   458,   478,   485,   486,   491,   492,
     496,   500,   506,   506,   510,   514,   521,   526,   530,   539,
     548,   560,   572,   578,   577,   593,   601,   605,   611,   636,
     652,   664,   676,   688,   693,   697,   702,   707,   710,   711,
     715,   719,   723,   727,   730,   737,   738,   739,   743,   747,
     753,   754,   758,   765,   769,   764,   778,   783,   795,   800,
     812,   817,   829,   834,   841,   842,   848,   849,   855,   859,
     863,   867,   871,   875,   881,   882,   888,   892,   898,   902,
     906,   910,   914,   918,   924,   930,   937,   941,   945,   949,
     953,   957,   963,   969,   976,   980,   983,   987,   991,   997,
     998,   999,  1000,  1005,  1012,  1013,  1016,  1020,  1023,  1027,
    1027,  1033,  1034,  1035,  1036,  1037,  1038,  1039,  1040,  1041,
    1042,  1043,  1044,  1045,  1046,  1047,  1048,  1049,  1050,  1051,
    1052,  1053,  1054,  1055,  1056,  1057,  1058,  1061,  1061,  1061,
    1061,  1061,  1062,  1062,  1062,  1062,  1062,  1062,  1062,  1063,
    1063,  1063,  1063,  1063,  1063,  1063,  1064,  1064,  1064,  1064,
    1064,  1064,  1065,  1065,  1065,  1065,  1065,  1065,  1065,  1066,
    1066,  1066,  1066,  1066,  1067,  1067,  1067,  1067,  1067,  1070,
    1074,  1078,  1103,  1119,  1131,  1143,  1155,  1160,  1165,  1170,
    1176,  1182,  1186,  1190,  1194,  1198,  1202,  1206,  1210,  1214,
    1223,  1227,  1231,  1235,  1239,  1243,  1247,  1251,  1255,  1259,
    1263,  1267,  1271,  1275,  1279,  1283,  1287,  1291,  1295,  1299,
    1303,  1303,  1308,  1308,  1314,  1320,  1327,  1328,  1333,  1337,
    1342,  1346,  1353,  1357,  1361,  1366,  1373,  1374,  1377,  1382,
    1386,  1391,  1396,  1401,  1406,  1412,  1416,  1419,  1423,  1427,
    1432,  1437,  1442,  1447,  1452,  1457,  1462,  1468,  1472,  1475,
    1475,  1487,  1488,  1488,  1493,  1493,  1500,  1506,  1510,  1513,
    1517,  1523,  1527,  1531,  1537,  1538,  1539,  1540,  1541,  1542,
    1543,  1544,  1545,  1550,  1549,  1564,  1564,  1569,  1574,  1578,
    1582,  1591,  1600,  1604,  1608,  1612,  1616,  1620,  1620,  1625,
    1631,  1632,  1641,  1641,  1657,  1657,  1673,  1676,  1673,  1687,
    1690,  1687,  1701,  1701,  1711,  1711,  1718,  1718,  1725,  1727,
    1727,  1725,  1736,  1735,  1754,  1760,  1753,  1778,  1777,  1796,
    1795,  1815,  1816,  1815,  1832,  1836,  1840,  1844,  1850,  1857,
    1858,  1859,  1860,  1863,  1864,  1865,  1868,  1869,  1878,  1879,
    1885,  1886,  1889,  1890,  1894,  1898,  1905,  1911,  1904,  1923,
    1932,  1936,  1942,  1947,  1952,  1957,  1961,  1966,  1971,  1975,
    1982,  1986,  1981,  1993,  1998,  1992,  2007,  2014,  2015,  2019,
    2025,  2026,  2029,  2040,  2043,  2047,  2048,  2051,  2055,  2058,
    2066,  2069,  2070,  2074,  2077,  2090,  2091,  2097,  2103,  2126,
    2156,  2160,  2167,  2170,  2176,  2177,  2183,  2187,  2194,  2197,
    2204,  2207,  2214,  2217,  2223,  2225,  2224,  2236,  2235,  2255,
    2256,  2257,  2258,  2261,  2268,  2269,  2270,  2271,  2274,  2305,
    2306,  2307,  2311,  2317,  2318,  2319,  2320,  2321,  2322,  2323,
    2324,  2325,  2326,  2327,  2330,  2336,  2342,  2343,  2346,  2351,
    2350,  2358,  2361,  2367,  2373,  2377,  2381,  2385,  2389,  2393,
    2397,  2401,  2406,  2411,  2415,  2419,  2423,  2427,  2438,  2439,
    2445,  2455,  2460,  2466,  2467,  2470,  2478,  2484,  2485,  2488,
    2498,  2502,  2505,  2510,  2510,  2535,  2536,  2540,  2549,  2550,
    2556,  2562,  2563,  2564,  2567,  2568,  2569,  2570,  2573,  2574,
    2575,  2578,  2579,  2582,  2583,  2586,  2587,  2590,  2591,  2592,
    2595,  2596,  2599,  2600,  2603
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "kCLASS", "kMODULE", "kDEF", "kUNDEF",
  "kBEGIN", "kRESCUE", "kENSURE", "kEND", "kIF", "kUNLESS", "kTHEN",
  "kELSIF", "kELSE", "kCASE", "kWHEN", "kWHILE", "kUNTIL", "kFOR",
  "kBREAK", "kNEXT", "kREDO", "kRETRY", "kIN", "kDO", "kDO_COND",
  "kDO_BLOCK", "kRETURN", "kYIELD", "kSUPER", "kSELF", "kNIL", "kTRUE",
  "kFALSE", "kAND", "kOR", "kNOT", "kIF_MOD", "kUNLESS_MOD", "kWHILE_MOD",
  "kUNTIL_MOD", "kRESCUE_MOD", "kALIAS", "kDEFINED", "klBEGIN", "klEND",
  "k__LINE__", "k__FILE__", "k__END__", "tIDENTIFIER", "tFID", "tGVAR",
  "tIVAR", "tCONSTANT", "tCVAR", "tXSTRING_BEG", "tINTEGER", "tFLOAT",
  "tSTRING_CONTENT", "tEND_DATA", "tNTH_REF", "tBACK_REF", "tREGEXP_END",
  "tUPLUS", "tUMINUS", "tUBS", "tPOW", "tCMP", "tEQ", "tEQQ", "tNEQ",
  "tGEQ", "tLEQ", "tANDOP", "tOROP", "tMATCH", "tNMATCH", "tDOT2", "tDOT3",
  "tAREF", "tASET", "tLSHFT", "tRSHFT", "tCOLON2", "tCOLON3", "tOP_ASGN",
  "tASSOC", "tLPAREN", "tLPAREN_ARG", "tRPAREN", "tLBRACK", "tLBRACE",
  "tLBRACE_ARG", "tSTAR", "tAMPER", "tSYMBEG", "tSTRING_BEG",
  "tREGEXP_BEG", "tWORDS_BEG", "tQWORDS_BEG", "tSTRING_DBEG",
  "tSTRING_DVAR", "tSTRING_END", "tLOWEST", "'='", "'?'", "':'", "'>'",
  "'<'", "'|'", "'^'", "'&'", "'+'", "'-'", "'*'", "'/'", "'%'",
  "tUMINUS_NUM", "'!'", "'~'", "tLAST_TOKEN", "'{'", "'}'", "'['", "']'",
  "'.'", "')'", "','", "'`'", "'('", "'\\\\'", "' '", "'\\n'", "';'",
  "$accept", "program", "@1", "bodystmt", "compstmt", "opt_end_data",
  "stmts", "stmt", "@2", "@3", "expr", "expr_value", "command_call",
  "block_command", "cmd_brace_block", "@4", "@5", "command", "mlhs",
  "mlhs_entry", "mlhs_basic", "mlhs_item", "mlhs_head", "mlhs_node", "lhs",
  "cname", "cpath", "fname", "fsym", "fitem", "undef_list", "@6", "op",
  "reswords", "arg", "@7", "@8", "arg_value", "aref_args", "paren_args",
  "opt_paren_args", "call_args", "call_args2", "command_args", "@9",
  "open_args", "@10", "@11", "block_arg", "opt_block_arg", "args", "mrhs",
  "primary", "@12", "@13", "@14", "@15", "@16", "@17", "@18", "@19", "@20",
  "@21", "@22", "@23", "@24", "@25", "@26", "@27", "@28", "@29", "@30",
  "@31", "@32", "@33", "primary_value", "then", "do", "if_tail",
  "opt_else", "block_var", "opt_block_var", "do_block", "@34", "@35",
  "block_call", "method_call", "brace_block", "@36", "@37", "@38", "@39",
  "case_body", "when_args", "cases", "opt_rescue", "exc_list", "exc_var",
  "opt_ensure", "literal", "strings", "string", "string1", "xstring",
  "regexp", "words", "word_list", "word", "qwords", "qword_list",
  "string_contents", "xstring_contents", "string_content", "@40", "@41",
  "string_dvar", "symbol", "sym", "dsym", "numeric", "variable", "var_ref",
  "var_lhs", "backref", "superclass", "@42", "f_arglist", "f_args",
  "f_norm_arg", "f_arg", "f_opt", "f_optarg", "restarg_mark", "f_rest_arg",
  "blkarg_mark", "f_block_arg", "opt_f_block_arg", "singleton", "@43",
  "assoc_list", "assocs", "assoc", "operation", "operation2", "operation3",
  "dot_or_colon", "opt_terms", "opt_nl", "trailer", "term", "terms",
  "none", 0
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
     355,   356,   357,   358,   359,   360,    61,    63,    58,    62,
      60,   124,    94,    38,    43,    45,    42,    47,    37,   361,
      33,   126,   362,   123,   125,    91,    93,    46,    41,    44,
      96,    40,    92,    32,    10,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   136,   138,   137,   139,   140,   141,   141,   142,   142,
     142,   142,   144,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   145,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   146,   146,
     146,   146,   146,   146,   147,   148,   148,   148,   148,   148,
     149,   149,   149,   151,   152,   150,   153,   153,   153,   153,
     153,   153,   153,   153,   154,   154,   155,   155,   156,   156,
     156,   156,   156,   156,   157,   157,   158,   158,   159,   159,
     159,   159,   159,   159,   159,   159,   160,   160,   160,   160,
     160,   160,   160,   160,   161,   161,   162,   162,   162,   163,
     163,   163,   163,   163,   164,   164,   165,   165,   166,   167,
     166,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     171,   170,   172,   170,   170,   173,   174,   174,   174,   174,
     174,   174,   175,   175,   175,   175,   176,   176,   177,   177,
     177,   177,   177,   177,   177,   177,   177,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   180,
     179,   181,   182,   181,   183,   181,   184,   185,   185,   186,
     186,   187,   187,   187,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   189,   188,   190,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   191,   188,   188,
     188,   188,   192,   188,   193,   188,   194,   195,   188,   196,
     197,   188,   198,   188,   199,   188,   200,   188,   201,   202,
     203,   188,   204,   188,   205,   206,   188,   207,   188,   208,
     188,   209,   210,   188,   188,   188,   188,   188,   211,   212,
     212,   212,   212,   213,   213,   213,   214,   214,   215,   215,
     216,   216,   217,   217,   217,   217,   219,   220,   218,   221,
     221,   221,   222,   222,   222,   222,   222,   222,   222,   222,
     224,   225,   223,   226,   227,   223,   228,   229,   229,   229,
     230,   230,   231,   231,   232,   232,   232,   233,   233,   234,
     234,   235,   235,   235,   236,   237,   237,   238,   239,   240,
     241,   241,   242,   242,   243,   243,   244,   244,   245,   245,
     246,   246,   247,   247,   248,   249,   248,   250,   248,   251,
     251,   251,   251,   252,   253,   253,   253,   253,   254,   255,
     255,   255,   255,   256,   256,   256,   256,   256,   256,   256,
     256,   256,   256,   256,   257,   258,   259,   259,   260,   261,
     260,   260,   262,   262,   263,   263,   263,   263,   263,   263,
     263,   263,   263,   264,   264,   264,   264,   264,   265,   265,
     266,   267,   267,   268,   268,   269,   269,   270,   270,   271,
     272,   272,   273,   274,   273,   275,   275,   275,   276,   276,
     277,   278,   278,   278,   279,   279,   279,   279,   280,   280,
     280,   281,   281,   282,   282,   283,   283,   284,   284,   284,
     285,   285,   286,   286,   287
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     3,     4,     2,     1,     2,     1,     1,
       3,     2,     0,     4,     3,     3,     3,     2,     3,     3,
       3,     3,     3,     0,     5,     4,     3,     3,     3,     6,
       5,     5,     5,     3,     3,     3,     3,     1,     1,     3,
       3,     2,     2,     1,     1,     1,     1,     2,     2,     2,
       1,     4,     4,     0,     0,     6,     2,     3,     4,     5,
       4,     5,     2,     2,     1,     3,     1,     3,     1,     2,
       3,     2,     2,     1,     1,     3,     2,     3,     1,     4,
       3,     3,     3,     3,     2,     1,     1,     4,     3,     3,
       3,     3,     2,     1,     1,     1,     2,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       4,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       5,     3,     6,     5,     5,     5,     5,     4,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     4,     4,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     3,     3,     3,     3,
       0,     4,     0,     6,     1,     1,     1,     2,     2,     5,
       2,     3,     3,     4,     4,     6,     1,     1,     1,     2,
       5,     2,     5,     4,     7,     3,     1,     4,     3,     5,
       7,     2,     5,     4,     6,     7,     9,     3,     1,     0,
       2,     1,     0,     3,     0,     4,     2,     2,     1,     1,
       3,     3,     4,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     4,     0,     5,     3,     3,     2,
       4,     3,     3,     1,     4,     3,     1,     0,     6,     2,
       1,     2,     0,     7,     0,     7,     0,     0,     7,     0,
       0,     7,     0,     6,     0,     5,     0,     6,     0,     0,
       0,    10,     0,     6,     0,     0,     8,     0,     5,     0,
       6,     0,     0,     9,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     5,     1,     2,
       1,     1,     1,     2,     1,     3,     0,     0,     6,     2,
       4,     4,     2,     4,     4,     3,     3,     2,     2,     1,
       0,     0,     6,     0,     0,     6,     5,     1,     4,     2,
       1,     1,     6,     1,     1,     1,     1,     2,     1,     2,
       1,     1,     1,     1,     1,     1,     2,     3,     3,     3,
       3,     3,     0,     3,     1,     2,     3,     3,     0,     3,
       0,     2,     0,     2,     1,     0,     3,     0,     4,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     3,     1,
       1,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       4,     2,     4,     2,     6,     4,     4,     2,     4,     2,
       2,     1,     0,     1,     1,     1,     1,     1,     1,     3,
       3,     1,     3,     1,     1,     2,     1,     1,     1,     2,
       2,     1,     1,     0,     5,     1,     2,     2,     1,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     1,     0,     1,     0,     1,     1,
       1,     1,     1,     2,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     0,     1,     0,     0,     0,     0,     0,   283,
     302,   304,   312,   306,   309,   318,   334,   335,   336,   337,
     293,   296,   369,   439,   438,   440,   441,     0,     0,   505,
      23,     0,   443,   442,   433,   282,   435,   434,   436,   437,
     412,   429,   430,   446,   447,     0,     0,     0,     0,     0,
       0,   514,   514,    73,   412,   410,   412,   402,   408,     0,
       0,     0,   514,   503,     9,    37,    38,    46,    45,     0,
      64,     0,    68,    74,     0,    43,   224,     0,    50,   300,
     274,   275,   394,   395,   276,   277,   278,   279,   392,   393,
     391,   444,   280,     0,   281,   259,     8,    11,   334,   335,
     293,   296,   369,   505,    94,    95,     0,     0,     0,     0,
      97,     0,   338,     0,   444,   281,     0,   327,   147,   158,
     148,   171,   144,   164,   154,   153,   169,   152,   151,   146,
     172,   156,   145,   159,   163,   165,   157,   150,   166,   173,
     168,   167,   160,   170,   155,   143,   162,   161,   174,   175,
     176,   177,   178,   142,   149,   139,   140,   137,   138,   141,
      99,   101,   100,   132,   133,   130,   114,   115,   116,   119,
     121,   117,   134,   135,   122,   123,   127,   118,   120,   111,
     112,   113,   124,   125,   126,   128,   129,   131,   136,   483,
     329,   102,   103,   482,     0,   167,   160,   170,   155,   137,
     138,    99,   100,   104,   106,   108,    17,   105,   107,     0,
       0,     0,   511,   510,     0,   314,   512,   504,     0,     0,
       0,     0,     0,     0,     0,   238,     0,   225,   269,    48,
     246,   514,     0,   444,     0,   281,   514,   488,    49,    47,
       0,    63,     0,   514,   368,    62,    41,     0,    12,   506,
     220,     0,     0,     0,   199,     0,   200,   494,   496,   495,
     497,   367,   289,     0,     0,     0,    64,   285,     0,   505,
       0,   507,   507,   226,   507,     0,   507,   485,     0,    72,
       0,    78,    85,   426,   425,   427,   424,     0,   423,     0,
       0,     0,     0,     0,     0,   431,   432,    42,   214,   215,
       0,     3,     6,     5,   504,     0,     0,     0,     0,     0,
       0,     0,   356,   359,     0,    76,     0,    71,    69,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   222,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   514,     0,     0,
       0,     0,   373,   370,   301,   396,     0,     0,   362,    56,
     299,     0,   324,    94,    95,    96,   431,   432,     0,   449,
     322,   448,     0,   514,     0,     0,     0,   462,   502,   501,
     331,   109,     0,   514,    44,     0,     0,   503,     0,     0,
     513,     0,     0,     0,   351,   350,     0,     0,   444,   281,
     289,   514,   266,     0,     0,     0,   239,   268,     0,   514,
       0,     0,     0,     0,   241,   295,     0,   262,   261,   260,
     238,   505,   514,   505,     0,    14,    16,    15,     0,   297,
       0,     0,     0,   414,   417,   415,   398,   413,     0,     0,
       0,     0,   287,    65,   505,   505,   227,   291,   509,   508,
     228,   509,   230,   509,   487,   292,   486,    84,     0,   514,
       0,   428,   397,   411,   399,   400,   401,     0,   404,   406,
       0,   407,     0,     0,     7,    10,    18,    19,    20,    21,
      22,    39,    40,   514,     0,    27,    35,     0,    36,     0,
      70,    77,    26,   179,   269,    34,   196,   204,   209,   210,
     211,   206,   208,   218,   219,   212,   213,   189,   190,   216,
     217,     0,   205,   207,   201,   202,   203,   191,   192,   193,
     194,   195,   494,   499,   495,   500,   259,   365,     0,   494,
     495,   514,   366,   259,   259,   514,   514,    28,   181,    33,
     188,    53,    57,     0,   451,     0,     0,    94,    95,    98,
       0,     0,   514,     0,   505,   467,   465,   464,   463,   466,
     474,   478,   477,   473,   462,     0,     0,   468,   514,   471,
     514,   476,   514,     0,   461,     0,     0,   284,   514,   514,
     383,   341,   340,     0,   339,     0,     0,     0,     0,     0,
     345,   344,   307,   343,   310,   289,     0,   514,     0,   319,
       0,   245,   179,   490,     0,   270,   267,   514,   498,   288,
       0,   494,   495,     0,     0,   489,   294,     0,    43,     0,
     264,     0,   258,   514,     0,     0,     0,     0,     0,   232,
      13,     0,   221,     0,    25,     0,     0,   187,    65,     0,
     231,     0,   270,   498,    83,     0,   494,   495,   403,   405,
     409,   197,   198,   354,     0,   357,   352,   273,     0,    75,
       0,     0,     0,     0,   364,    60,   290,     0,     0,   237,
     363,    58,   236,   361,    52,   360,    51,   374,   371,   514,
     325,     0,     0,   290,   328,     0,     0,   505,     0,   453,
       0,   457,   481,     0,   459,   475,     0,   460,   479,   332,
     110,   384,   385,   514,   386,     0,   514,   348,   514,   342,
     514,     0,     0,   377,     0,   315,     0,     0,     0,   498,
     288,     0,   494,   495,     0,   514,     0,   243,     0,   290,
       0,     0,   514,   514,     0,     0,   263,     0,   251,   233,
       0,   505,   514,   514,   234,     0,    24,     0,   419,   420,
     421,   416,   422,   286,   505,    79,   353,     0,     0,     0,
     271,   180,     0,    32,   185,   186,    61,     0,    30,   183,
      31,   184,    59,     0,     0,    54,     0,   450,   323,   484,
     470,     0,   330,   469,   514,   514,   480,     0,   472,   514,
     462,     0,     0,   388,   349,     0,     4,   390,     0,     0,
     346,     0,   313,   379,     0,     0,   317,     0,     0,   290,
       0,   240,     0,     0,   242,   257,     0,   248,   514,   514,
     265,     0,     0,   298,   418,   229,   355,     0,   272,   223,
      29,   182,     0,     0,     0,     0,   452,     0,   455,   456,
     458,     0,     0,   387,     0,    86,    93,     0,   389,     0,
     303,   305,     0,   514,   308,   311,   320,   514,   514,     0,
     247,     0,   253,   514,   235,   358,   375,   372,     0,   326,
     514,     0,    92,     0,   514,     0,   514,     0,   378,   380,
     381,   376,     0,   244,   249,     0,   514,     0,   252,    55,
     454,   333,   498,    91,     0,   494,   495,   382,   514,     0,
     514,     0,   254,   514,    87,   347,   321,   250,     0,   255,
     514,   256
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,   382,   383,   301,    63,    64,   428,   251,
      65,   385,    66,    67,   542,   679,   834,    68,    69,   265,
      70,    71,    72,    73,   226,   110,   111,   203,   204,   205,
     206,   576,   260,   192,    75,   430,   511,   228,   270,   669,
     670,   229,   620,   241,   242,   419,   621,   735,   606,   406,
     271,   488,    76,   209,   444,   631,   210,   211,   218,   717,
     219,   718,   214,   388,   389,   220,   724,   882,   546,   543,
     776,   375,   377,   575,   790,   255,   583,   592,   799,   800,
     397,   655,   313,   483,   758,    78,    79,   360,   536,   774,
     535,   773,   588,   714,   881,   579,   703,   792,   796,    80,
      81,    82,    83,    84,    85,    86,   292,   467,    87,   294,
     289,   253,   437,   636,   635,   751,    88,   288,    89,    90,
     233,    92,   234,   235,   370,   545,   565,   566,   567,   568,
     569,   570,   571,   572,   573,   786,   691,   194,   376,   275,
     272,   237,   116,   550,   527,   380,   215,   250,   450,   584,
     217,    96
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -644
static const yytype_int16 yypact[] =
{
    -644,   101,  2214,  -644,  5647,  8187,  8493,  3971,  5297,  -644,
    -644,  -644,    78,  -644,  -644,  -644,  5859,  5859,  -644,  -644,
    5859,  4622,  4728,  -644,  -644,  -644,  -644,  7025,  5178,    -9,
    -644,    32,  -644,  -644,  4092,  4198,  -644,  -644,  4304,  -644,
    -644,  -644,  -644,  -644,  -644,  7873,  7873,  2588,    73,  3350,
    7025,  7131,  7873,  8595,  5059,  -644,  -644,    43,    97,   290,
    7979,  7873,    98,   330,   859,   519,  -644,  -644,   130,    59,
    -644,    53,  8289,  -644,   110,  2458,   259,   355,    42,    86,
    -644,  -644,   146,  -644,  -644,  -644,  -644,  -644,  -644,  -644,
    -644,     7,  -644,   160,    20,    58,  -644,   859,  -644,  -644,
    -644,   147,   151,    -9,   148,   396,  7025,    66,  3476,   430,
    -644,    57,  -644,   407,  -644,  -644,    58,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,
    -644,    46,    94,   248,   279,  -644,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,  -644,  -644,  -644,   300,   345,  -644,
     348,  -644,   415,  -644,  -644,  -644,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,   419,  -644,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,  -644,  -644,   156,  -644,  -644,  2740,
    7025,  7025,  -644,  -644,  7025,   281,  -644,   218,  7025,  7025,
    8391,   301,  7873,  7873,  7873,  -644,   255,  2458,   278,  -644,
    -644,   247,   462,    28,   358,    48,   283,  -644,  -644,  -644,
    5536,  -644,  5965,  5859,  -644,  -644,  -644,   452,  -644,  -644,
     286,   360,  3602,   395,  -644,   483,   417,  -644,  -644,  -644,
    -644,  -644,   132,  3350,   368,   381,   392,   519,  7873,    -9,
     375,   -18,    88,  -644,   126,   424,    88,  -644,   497,  -644,
     505,   576,   579,  -644,  -644,  -644,  -644,   409,  -644,   514,
     307,   460,   524,   465,     5,   493,   509,  -644,  -644,  -644,
     517,  -644,  -644,  -644,  3848,  7025,  7025,  7025,  7025,  5647,
    7025,  7025,  -644,  -644,  7237,  -644,  3350,  8595,   451,  7237,
    7873,  7873,  7873,  7873,  7873,  7873,  7873,  7873,  7873,  7873,
    7873,  7873,  7873,  7873,  7873,  -644,  7873,  7873,  7873,  7873,
    7873,  7873,  7873,  7873,  7873,  7873,  8749,  7131,  8817,  2588,
    2588,  2588,  -644,  -644,  -644,  -644,  7979,  7979,  -644,   499,
    -644,   286,   519,  -644,   613,  -644,  -644,  -644,   330,  -644,
    -644,  -644,  8885,  7131,  2588,  2740,  7025,   628,  -644,  -644,
    -644,  -644,   585,   604,   519,    64,    64,   330,   596,   608,
    -644,    62,    62,   569,  -644,  -644,   645,   609,   143,   165,
     162,   507,  -644,  7873,  7873,  6071,  -644,  -644,  8953,  7131,
    9021,  7873,  7873,  6177,  -644,  -644,   510,  5753,  -644,  -644,
     130,    -9,   511,    25,   513,  -644,  -644,  -644,  5297,  -644,
    7873,  3602,   521,  -644,  -644,  -644,  -644,  -644,  8953,  9021,
    7873,   522,  -644,   525,    -9,  2048,  -644,  -644,  7343,  -644,
    -644,  7873,  -644,  7873,  -644,  -644,  -644,   613,  9089,  7131,
    9157,  -644,  -644,  -644,  -644,  -644,  -644,    38,  -644,  -644,
     523,  -644,  7873,  7873,  -644,   859,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,    27,  7873,  -644,   526,   530,  -644,   532,
    -644,  -644,  -644,  1376,  -644,  -644,   417,  9592,  9592,  9592,
    9592,   711,   711,  9643,  1472,  9592,  9592,  9575,  9575,    29,
      29,  7873,   711,   711,   489,   489,   175,    69,    69,   417,
     417,   417,  2345,  4834,  2475,  4940,   151,  -644,   537,   320,
     373,  4728,  -644,  2605,  2605,    27,    27,  -644,  2458,  -644,
    2458,  -644,  -644,   330,  -644,  7025,  2740,   418,   288,  -644,
     151,   546,   151,   663,    54,   568,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,   639,  2740,   330,  -644,   557,  -644,
     559,   638,   573,   654,  -644,  5416,  5297,  -644,  7449,   692,
    -644,  -644,  -644,  2866,   700,  3108,   596,  7555,   706,  3728,
    -644,  -644,  -644,  -644,  -644,   190,  9225,  7131,  9293,  -644,
     621,  -644,  1030,  -644,  7873,   278,  -644,   590,  4410,  4516,
     599,   272,   315,  7873,   278,  -644,  -644,  7873,  9492,    35,
    -644,   594,  -644,   597,   601,  6283,  2588,  2588,   622,  -644,
    -644,  7025,  2458,   603,  -644,  3602,   529,  2458,   431,   625,
    -644,  7873,  -644,   117,   288,   606,   365,   425,  -644,  -644,
    -644,   417,   417,  -644,  8085,  -644,  -644,  -644,  7661,  -644,
    7873,  2328,  7979,  7873,  -644,   499,   533,  7979,  7979,  -644,
    -644,   499,  -644,  -644,  -644,  -644,  -644,  -644,  -644,    27,
    -644,   330,   739,  -644,  -644,   626,  7873,    -9,   746,  -644,
     639,  -644,  -644,    37,  -644,  -644,     9,  -644,  -644,  -644,
    -644,   526,  -644,   669,  -644,  3229,   749,  -644,   552,  -644,
     692,   750,  7873,   646,    64,  -644,   770,  3728,  3728,    89,
     152,   655,   241,   263,  7025,   507,  6389,  -644,  7873,   347,
    7873,  7873,   507,   507,  6495,   658,  -644,  6601,  -644,  -644,
     130,    25,   151,   151,  -644,    83,  -644,   659,  -644,  -644,
    -644,  -644,  -644,  -644,  2048,   657,  -644,   674,  3728,  7873,
     661,  2458,  7873,  -644,  2458,  2458,  -644,  7979,  -644,  2458,
    -644,  2458,  -644,  3728,  3602,  -644,  2740,  -644,  -644,  -644,
    -644,   664,  -644,  -644,   662,   573,  -644,   568,  -644,   573,
     628,  8697,    64,  -644,  -644,  3728,  -644,  -644,  7025,   777,
    -644,   783,  -644,  -644,  7767,  2987,  -644,   788,   789,   325,
      62,  -644,  7873,  7873,  -644,  -644,  7873,  -644,   672,   673,
    -644,  7873,   675,  -644,  -644,  -644,  -644,   798,  -644,  2458,
    -644,  2458,   800,   688,  3602,   805,  -644,    37,  -644,  -644,
    -644,  2740,   762,  -644,   684,   576,   579,  2740,  -644,    64,
    -644,  -644,  7873,    85,  -644,  -644,  -644,  9513,   507,  6707,
    -644,  6813,  -644,   507,  -644,  -644,  -644,  -644,   694,  -644,
     573,   811,   613,  9361,  7131,  9429,   604,  2866,  -644,  -644,
    -644,  -644,  3728,  -644,  -644,  7873,   702,  7873,  -644,  -644,
    -644,  -644,    49,   288,   708,    67,    70,  -644,   552,   825,
     507,  6919,  -644,   507,   657,  -644,  -644,  -644,  7873,  -644,
     507,  -644
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -644,  -644,  -644,  -351,   102,  -644,  -644,    30,  -644,  -644,
     853,  -145,   -19,  -644,  -541,  -644,  -644,   -11,  -187,  -177,
      14,   765,  -644,    18,  1307,   -70,   832,    -5,  -644,   -25,
    -644,  -644,     0,  -644,   -16,  -644,  -644,  1248,  -330,   -10,
    -486,    22,  -644,     4,  -644,  -644,  -644,  -644,    -1,   124,
      44,  -281,    15,  -644,  -644,  -644,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,  -644,   457,  -363,  -379,   -59,  -554,
     187,  -454,  -644,  -644,  -644,  -221,  -644,   763,  -644,  -644,
    -644,  -644,  -510,  -644,  -644,   -32,  -644,  -644,  -644,  -644,
    -644,  -644,   764,  -644,  -644,  -644,  -644,  -644,  -644,  -644,
    -644,   197,  -217,  -644,  -644,  -644,    23,  -644,    24,  -644,
     986,   840,   631,  1164,  -644,  -644,    60,   284,   161,  -644,
    -643,   164,  -644,  -634,  -644,  -324,  -515,  -644,  -644,  -644,
      -6,  -343,   969,    31,  -644,  -644,   -45,   -76,   250,    55,
     792,  1466
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -515
static const yytype_int16 yytable[] =
{
     227,   227,   190,   248,   227,   225,   225,   191,   191,   225,
     236,   236,   244,   594,   236,   230,   230,   528,   303,   230,
     112,   112,   423,   585,   553,   706,   245,   361,   191,   254,
     256,   207,   208,   394,    97,   227,   227,   365,   495,   238,
     269,   297,   239,   551,   298,   299,   276,   673,   675,   286,
     788,   207,   208,   574,   191,   694,   785,   697,   368,   789,
     231,   231,   -89,   266,   231,   470,   386,   216,   112,   387,
     615,   279,   463,   391,   392,   468,   711,   581,   261,   610,
     -88,   677,   678,   -90,   352,   358,   441,   112,   787,   590,
     310,   311,   244,  -503,  -445,  -503,   274,   320,   433,   359,
     705,     3,   587,   653,    62,   561,   358,   357,   615,   471,
     626,   448,   352,   -86,   -89,  -445,   449,   363,   216,   310,
     311,   364,   562,   404,   766,   249,   -93,   350,   262,   645,
     772,  -439,   560,   561,   -86,   412,   -78,   320,   654,   489,
     434,   435,   -81,   341,   342,   343,   344,   345,   300,   -85,
     562,   264,   627,   563,   -93,   252,   801,   -89,   312,   249,
     476,   477,   478,   479,   734,   314,   371,   369,   -86,   351,
     591,   648,   582,  -439,  -491,   -88,   291,   -91,   -90,  -438,
    -494,   353,   315,   -89,   -89,   343,   344,   345,   249,   243,
     -93,   212,   213,   446,   788,   682,   212,   213,   212,   213,
     -89,   -88,   -88,   870,   -90,   -90,   227,   227,   298,   353,
     264,   823,   212,   213,   688,   -92,   319,   451,   -81,   440,
    -494,  -438,   449,   -81,   227,   775,   227,   227,   -81,   225,
     293,   225,   420,  -433,   236,   112,   236,   236,   -92,   230,
     574,   230,   230,   320,    55,   -81,   -81,   356,  -494,   440,
     649,   287,   445,   290,   -86,   453,   673,   675,   333,   334,
     449,   -84,   416,   -91,   418,   421,   -88,   721,   -92,   838,
     839,  -491,   -78,  -433,   840,  -433,   -93,   266,   240,  -491,
    -433,   -83,   243,  -495,   231,   381,   231,   422,   -90,   341,
     342,   343,   344,   345,   -85,   485,  -316,   702,   227,   879,
     492,   -92,   549,   493,   496,   497,   498,   499,   500,   501,
     502,   503,   504,   505,   506,   507,   508,   509,   510,   -84,
     512,   513,   514,   515,   516,   517,   518,   519,   520,   521,
     266,   227,   112,  -440,   475,   490,   269,   537,   539,   480,
     538,   540,   586,   880,  -338,   624,   525,   628,   295,   296,
     -87,   805,   -88,   390,   432,   890,   400,   227,   487,   730,
     414,   403,   269,   487,  -441,   264,   404,   433,   639,   640,
     -80,   464,   525,  -288,   -90,  -440,   405,   526,   -88,   531,
     532,   533,   534,   615,  -338,  -443,  -338,   602,   227,   227,
     -80,  -338,   -82,   227,   615,   538,   540,   227,   269,   607,
     681,   618,   731,   630,   741,   552,  -441,   667,   525,   434,
     435,   623,   413,  -288,   632,  -288,   622,   429,   264,  -495,
    -288,   -90,  -492,   544,   637,   835,   -88,  -443,   191,   847,
    -442,   856,   227,  -433,   813,   227,   -87,   227,   525,   526,
     346,   531,   216,   227,  -498,   411,   593,   593,   269,   -80,
     -82,   207,   208,   -87,   -79,   433,   651,   652,   525,    77,
     668,    77,   113,   113,   212,   213,   574,   394,   227,   433,
     552,   -80,  -442,   232,   232,  -433,   -80,   232,   685,   -90,
     347,  -436,   348,   431,   232,   320,   877,   349,   366,   367,
     871,   552,   372,   -80,   -80,   661,   442,   434,   435,   436,
    -436,   447,   -82,  -498,   378,   425,    77,   232,   232,   443,
     280,   434,   435,   461,   426,   427,   664,   232,   615,  -492,
     -66,  -436,   452,  -436,   454,   601,   456,  -492,  -436,   280,
     665,   -82,   373,   633,   374,   671,   -82,   674,   676,   349,
     664,  -498,  -436,  -498,   894,  -498,   379,   408,   455,  -494,
    -498,   700,   457,   -82,   -82,   310,   311,   320,   615,   -67,
     -75,   472,   227,   232,   465,    77,   798,   705,   438,   469,
     699,   227,   333,   334,   433,   191,   191,   473,   474,   810,
     491,   227,   748,   749,   433,   750,   269,   409,   227,   410,
     458,    43,    44,   541,   349,   577,   525,   227,   680,   207,
     208,   227,   340,   341,   342,   343,   344,   345,   409,   227,
     439,   781,   578,   587,   740,   349,   434,   435,   462,   607,
     767,   689,   487,   589,   595,   754,   434,   435,   466,   552,
     459,   713,   460,    93,   599,    93,   600,   349,   616,   -87,
     625,   629,   227,   763,   761,   634,   764,   765,   768,   770,
     638,   769,   771,   849,   -75,  -269,   650,   742,   743,   658,
     659,  -444,   -79,   666,  -281,   822,    77,   232,   232,   112,
     227,   232,   683,   684,   686,   232,   232,   396,   825,   555,
      93,   556,   557,   558,   559,   708,   690,   710,   693,   695,
     555,   716,   556,   557,   558,   559,   227,   232,  -289,   232,
     232,  -444,   696,  -444,  -281,   698,  -281,   705,  -444,    77,
     227,  -281,   764,   709,   769,   771,   715,   223,   227,   726,
      77,   227,   736,   560,   561,   729,   737,   746,   819,   739,
     596,   727,   755,   817,   560,   561,   777,   747,  -289,    93,
    -289,   562,  -290,   227,   563,  -289,   829,   738,   830,   778,
     744,   831,   562,   753,   779,   563,   782,   791,   795,   564,
     802,    77,   232,   232,   232,   232,    77,   232,   232,   873,
     597,   232,   598,    77,   280,   804,   232,   349,   818,   320,
     806,   809,  -290,   824,  -290,   826,   820,   850,   227,  -290,
    -270,   837,   836,   851,   333,   334,   857,   831,   854,   855,
     227,   859,   861,   864,   232,   227,   112,   794,   865,   874,
     866,   875,   867,   232,   232,   869,   349,   872,   889,   807,
     808,   891,   338,   339,   340,   341,   342,   343,   344,   345,
     232,   901,    77,   232,   904,   906,   227,   318,   117,   905,
      93,   757,   354,   227,   897,   227,   355,   193,   687,   811,
     841,   783,     0,   886,   784,   304,   814,   815,   227,     0,
     827,     0,     0,   269,     0,   593,   232,     0,     0,   227,
       0,   227,     0,   525,   232,   832,   833,     0,     0,     0,
     246,     0,     0,    93,     0,   227,     0,     0,    77,     0,
       0,     0,   227,     0,    93,     0,     0,   848,   305,   306,
     307,   308,   309,   267,     0,     0,   552,   853,     0,     0,
       0,     0,     0,     0,     0,     0,   232,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    93,   868,     0,     0,     0,
      93,     0,   860,   862,     0,     0,     0,    93,     0,   876,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   362,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    95,     0,    95,     0,     0,     0,     0,     0,   898,
       0,   883,   884,     0,   899,    95,    95,   888,    91,    95,
      91,   114,   114,   114,     0,     0,    95,     0,     0,     0,
       0,     0,   232,    77,     0,     0,    93,     0,     0,     0,
     902,     0,     0,     0,     0,     0,     0,     0,    95,    95,
      95,     0,    77,     0,   907,     0,     0,   909,     0,    95,
       0,     0,     0,     0,   911,    91,     0,     0,     0,   281,
      77,     0,    77,     0,     0,     0,    77,     0,     0,     0,
       0,     0,     0,     0,   232,     0,     0,     0,   281,     0,
       0,     0,    93,   384,   384,     0,     0,   384,     0,     0,
       0,   384,   384,   660,     0,    95,     0,    95,     0,     0,
       0,     0,   232,     0,     0,     0,     0,     0,   232,     0,
       0,     0,    77,     0,    91,     0,     0,     0,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   396,     0,   333,   334,     0,     0,     0,     0,   232,
       0,     0,     0,     0,   232,   232,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   335,     0,   336,
     337,   338,   339,   340,   341,   342,   343,   344,   345,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   384,   384,
     384,   384,    77,   481,   482,     0,    94,     0,    94,   115,
     115,     0,     0,     0,    77,    77,     0,    93,    95,    95,
      95,   232,     0,    95,     0,     0,     0,    95,    95,     0,
       0,     0,     0,     0,     0,    91,    93,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   398,     0,     0,    95,
       0,    95,    95,    94,    93,    77,    93,   282,     0,     0,
      93,    95,     0,     0,   232,     0,     0,     0,     0,   554,
      77,    77,    95,    77,     0,     0,   282,     0,    91,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   844,    91,
       0,     0,    77,     0,     0,   232,     0,     0,     0,     0,
       0,     0,    77,     0,     0,     0,    93,     0,     0,     0,
     267,     0,    94,    95,    95,    95,    95,    95,    95,    95,
      95,     0,     0,    95,     0,    95,     0,     0,    95,     0,
      91,    77,     0,     0,     0,    91,     0,     0,    77,     0,
       0,     0,    91,   281,    77,     0,     0,     0,     0,    74,
       0,    74,     0,     0,     0,     0,    95,     0,     0,     0,
       0,     0,     0,     0,     0,    95,    95,     0,     0,     0,
       0,   232,     0,     0,    77,     0,    93,     0,     0,    77,
       0,     0,    95,     0,    95,    95,     0,     0,    93,    93,
       0,     0,     0,     0,     0,     0,    74,     0,     0,     0,
       0,    91,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    94,     0,     0,     0,     0,    95,     0,
       0,     0,     0,     0,   399,     0,    95,     0,     0,    93,
       0,     0,     0,     0,     0,     0,     0,     0,   384,     0,
      95,     0,     0,     0,    93,    93,     0,    93,     0,     0,
       0,     0,     0,     0,     0,    74,    94,    91,     0,   660,
       0,     0,     0,     0,     0,     0,    93,    94,    95,     0,
       0,     0,     0,     0,     0,     0,    93,     0,     0,     0,
       0,     0,     0,     0,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,     0,     0,   333,
     334,     0,     0,     0,     0,    93,     0,     0,    94,     0,
     401,   402,    93,    94,     0,     0,     0,     0,    93,     0,
      94,   282,     0,   335,   745,   336,   337,   338,   339,   340,
     341,   342,   343,   344,   345,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -225,     0,     0,    93,     0,
       0,     0,     0,    93,    95,    95,    74,   273,   277,     0,
       0,     0,     0,     0,     0,     0,     0,   395,   302,     0,
       0,     0,    91,     0,    95,     0,     0,     0,     0,    94,
     320,   321,   322,   323,   324,   325,   326,   327,     0,   329,
     330,    91,    95,     0,    95,   333,   334,     0,    95,    74,
       0,     0,   486,     0,     0,     0,    95,   494,     0,    91,
      74,    91,     0,     0,     0,    91,     0,   384,     0,     0,
       0,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,     0,     0,     0,    95,    94,     0,     0,     0,     0,
      95,     0,     0,     0,    95,     0,     0,     0,     0,     0,
       0,    74,     0,     0,     0,     0,    74,     0,     0,     0,
       0,    91,     0,    74,     0,     0,     0,     0,     0,     0,
       0,    95,     0,     0,     0,     0,    95,    95,     0,     0,
     398,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   384,   603,   605,     0,     0,     0,     0,     0,     0,
       0,   614,     0,     0,     0,   619,     0,     0,     0,     0,
       0,     0,     0,     0,    95,     0,     0,     0,     0,     0,
       0,     0,    74,     0,     0,     0,    95,    95,     0,     0,
       0,    91,     0,    95,     0,     0,   642,   407,     0,   614,
       0,   642,   407,    91,    91,     0,     0,     0,     0,   424,
      94,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    95,     0,    94,
       0,     0,   657,     0,     0,     0,    95,     0,    74,     0,
       0,     0,    95,    95,    91,    95,     0,    94,     0,    94,
       0,     0,     0,    94,     0,     0,     0,     0,     0,    91,
      91,     0,    91,     0,    95,     0,     0,    95,     0,     0,
       0,     0,     0,     0,    95,     0,     0,   845,     0,     0,
       0,    91,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    91,     0,     0,     0,     0,     0,     0,     0,    94,
     752,     0,     0,    95,     0,     0,     0,     0,     0,     0,
      95,     0,     0,   273,     0,     0,    95,     0,   399,     0,
      91,     0,     0,     0,     0,     0,   701,    91,     0,     0,
       0,     0,     0,    91,     0,   494,     0,     0,     0,   273,
       0,     0,     0,    95,     0,     0,    95,     0,     0,   580,
       0,    95,   725,    74,     0,     0,     0,     0,     0,     0,
       0,   732,     0,    91,     0,   733,     0,   407,    91,    94,
       0,     0,    74,   605,     0,   273,     0,     0,     0,     0,
       0,    94,    94,     0,     0,     0,     0,     0,   407,     0,
      74,     0,    74,     0,     0,     0,    74,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   760,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    94,     0,     0,   273,     0,     0,     0,     0,
       0,     0,     0,     0,   780,     0,     0,    94,    94,     0,
      94,     0,    74,     0,     0,     0,     0,     0,     0,   656,
       0,     0,     0,     0,     0,   846,     0,     0,     0,    94,
     803,   395,     0,     0,     0,     0,     0,     0,     0,    94,
       0,     0,     0,     0,   614,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   614,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   672,    94,   672,
     672,   656,   656,     0,     0,    94,     0,   828,     0,     0,
       0,    94,    74,     0,     0,     0,     0,     0,   672,     0,
       0,     0,     0,     0,    74,    74,     0,     0,     0,     0,
       0,     0,     0,     0,   692,     0,   692,     0,   692,     0,
       0,    94,     0,     0,   704,   707,    94,     0,     0,     0,
       0,     0,   642,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   273,   858,    74,     0,     0,     0,   863,
       0,     0,     0,   407,     0,     0,     0,     0,     0,     0,
      74,    74,     0,    74,     0,     0,     0,     0,     0,   407,
       0,     0,     0,     0,     0,     0,     0,     0,   843,     0,
     878,     0,    74,     0,     0,     0,     0,   605,     0,   614,
       0,     0,    74,     0,     0,     0,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,     0,
       0,   333,   334,   900,     0,   903,     0,     0,     0,     0,
       0,    74,     0,     0,     0,   656,     0,     0,    74,   614,
       0,     0,     0,     0,    74,   335,   910,   336,   337,   338,
     339,   340,   341,   342,   343,   344,   345,     0,     0,   793,
       0,     0,   797,     0,   707,     0,   707,     0,     0,     0,
       0,     0,   249,     0,    74,     0,     0,     0,     0,    74,
       0,   407,     0,     0,     0,     0,     0,     0,   407,   407,
       0,     0,     0,     0,     0,     0,     0,     0,   672,   672,
       0,     0,     0,     0,  -514,     4,     0,     5,     6,     7,
       8,     9,     0,     0,     0,    10,    11,     0,     0,     0,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
     692,   692,    27,     0,     0,   692,     0,     0,    28,    29,
      30,    31,    32,    33,  -514,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,     0,    43,    44,     0,    45,
      46,    47,     0,     0,   407,   407,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,     0,     0,     0,   707,
       0,     0,     0,   407,   407,     0,     0,     0,     0,   407,
       0,     0,     0,    59,    60,    61,   692,     0,     0,     0,
     273,     0,   580,     0,     0,  -498,     0,     0,  -514,  -514,
       0,     0,   407,  -498,  -498,  -498,     0,     0,     0,  -498,
    -498,     0,  -498,     0,   707,     0,   407,     0,     0,   407,
       0,  -498,     0,     0,     0,     0,   407,     0,     0,     0,
       0,  -498,  -498,     0,  -498,  -498,  -498,  -498,  -498,     0,
       0,     0,     0,     0,     0,  -498,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,     0,
       0,   333,   334,  -498,  -498,  -498,  -498,  -498,  -498,  -498,
    -498,  -498,  -498,  -498,  -498,  -498,     0,     0,  -498,  -498,
    -498,     0,   662,     0,     0,   335,   762,   336,   337,   338,
     339,   340,   341,   342,   343,   344,   345,     0,     0,     0,
       0,   -89,  -498,     0,  -498,  -498,  -498,  -498,  -498,  -498,
    -498,  -498,  -498,  -498,     0,     0,     0,     0,  -498,  -498,
    -498,     0,  -498,  -498,   -81,  -288,     0,  -498,     0,  -498,
    -498,     0,     0,  -288,  -288,  -288,     0,     0,     0,  -288,
    -288,     0,  -288,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -288,  -288,     0,  -288,  -288,  -288,  -288,  -288,     0,
       0,     0,     0,     0,     0,  -288,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,     0,
       0,   333,   334,  -288,  -288,  -288,  -288,  -288,  -288,  -288,
    -288,  -288,  -288,  -288,  -288,  -288,     0,     0,  -288,  -288,
    -288,     0,   663,     0,     0,   335,     0,   336,   337,   338,
     339,   340,   341,   342,   343,   344,   345,     0,     0,     0,
       0,   -91,  -288,     0,  -288,  -288,  -288,  -288,  -288,  -288,
    -288,  -288,  -288,  -288,     0,     0,     0,     0,     0,  -288,
    -288,     0,  -288,  -288,   -83,  -514,     0,  -288,     0,  -288,
    -288,     0,     0,  -514,  -514,  -514,     0,     0,  -514,  -514,
    -514,     0,  -514,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -514,     0,     0,     0,     0,     0,     0,   257,
     258,  -514,  -514,   259,  -514,  -514,  -514,  -514,  -514,     0,
       0,     0,     0,   163,   164,  -514,   165,   166,   167,   168,
       0,   169,   170,     0,     0,   171,     0,     0,     0,   172,
     173,   174,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,     0,     0,     0,     0,     0,     0,
    -514,     0,     0,     0,     0,     0,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,     0,     0,   187,
       0,     0,     0,  -514,     0,     0,     0,     0,   188,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -514,
       0,     0,  -514,  -514,     0,     0,   243,     0,     0,  -514,
    -514,     4,     0,     5,     6,     7,     8,     9,  -514,  -514,
    -514,    10,    11,     0,     0,  -514,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,    28,    29,    30,    31,    32,    33,
       0,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,     0,    43,    44,     0,    45,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,     0,     0,    49,
      50,     0,    51,    52,     0,    53,     0,    54,    55,    56,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    59,
      60,    61,     0,     0,     0,     0,     0,     4,     0,     5,
       6,     7,     8,     9,  -514,  -514,  -514,    10,    11,     0,
    -514,  -514,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
      28,    29,    30,    31,    32,    33,     0,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,     0,     0,    49,    50,     0,    51,    52,
       0,    53,     0,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    59,    60,    61,     4,     0,
       5,     6,     7,     8,     9,     0,     0,  -514,    10,    11,
    -514,  -514,  -514,    12,  -514,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,    33,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,     0,     0,    49,    50,     0,    51,
      52,     0,    53,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    59,    60,    61,     4,
       0,     5,     6,     7,     8,     9,     0,     0,  -514,    10,
      11,  -514,  -514,  -514,    12,     0,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,    27,     0,     0,     0,
       0,     0,    28,    29,    30,    31,    32,    33,     0,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,     0,
      43,    44,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,     0,    49,    50,     0,
      51,    52,     0,    53,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    59,    60,    61,
       4,     0,     5,     6,     7,     8,     9,     0,  -514,  -514,
      10,    11,  -514,  -514,     0,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,    33,     0,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
       0,    43,    44,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    48,     0,     0,    49,    50,
       0,    51,    52,     0,    53,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    59,    60,
      61,     4,     0,     5,     6,     7,     8,     9,     0,     0,
       0,    10,    11,  -514,  -514,     0,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,    28,    29,    30,    31,    32,    33,
       0,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,     0,    43,    44,     0,    45,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,     0,     0,   263,
      50,     0,    51,    52,     0,    53,     0,    54,    55,    56,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    59,
      60,    61,     0,     0,     0,     0,     0,     4,  -514,     5,
       6,     7,     8,     9,  -514,  -514,     0,    10,    11,     0,
       0,     0,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
      28,    29,    30,    31,    32,    33,     0,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,     0,     0,    49,    50,     0,    51,    52,
       0,    53,     0,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    59,    60,    61,     0,     0,
       0,     0,     0,     4,  -514,     5,     6,     7,     8,     9,
    -514,  -514,     0,    10,    11,     0,     0,     0,    12,     0,
      13,    14,    15,    16,    17,    18,    19,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,    29,    30,    31,
      32,    33,     0,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,     0,    43,    44,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,     0,
       0,    49,    50,     0,    51,    52,     0,    53,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    59,    60,    61,     0,     0,  -514,     0,     0,     4,
       0,     5,     6,     7,     8,     9,  -514,  -514,  -514,    10,
      11,     0,     0,     0,    12,     0,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,    27,     0,     0,     0,
       0,     0,    28,    29,    30,    31,    32,    33,     0,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,     0,
      43,    44,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,     0,    49,    50,     0,
      51,    52,     0,    53,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    59,    60,    61,
       0,     5,     6,     7,     8,     9,     0,     0,     0,    10,
      11,     0,  -514,  -514,    12,     0,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,    27,     0,     0,     0,
       0,     0,    28,    29,    30,    31,    32,    33,     0,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,     0,
      43,    44,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,     0,    49,    50,     0,
      51,    52,     0,    53,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    59,    60,    61,
       0,     0,     0,     0,   118,   119,   120,   121,   122,   123,
     124,   125,     0,   390,   126,   127,   128,   129,   130,     0,
       0,   131,   132,   133,   134,   135,   136,   137,     0,     0,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,    36,    37,   162,    39,     0,     0,
       0,     0,     0,     0,     0,     0,   163,   164,     0,   165,
     166,   167,   168,     0,   169,   170,     0,     0,   171,     0,
       0,     0,   172,   173,   174,   175,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
       0,     0,   187,     0,     0,  -491,  -491,  -491,     0,  -491,
       0,   188,   189,  -491,  -491,     0,     0,     0,  -491,     0,
    -491,  -491,  -491,  -491,  -491,  -491,  -491,     0,  -491,     0,
       0,  -491,  -491,  -491,  -491,  -491,  -491,  -491,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -491,     0,     0,
    -491,  -491,     0,  -491,  -491,  -491,  -491,  -491,  -491,  -491,
    -491,  -491,     0,     0,  -491,  -491,     0,  -491,  -491,  -491,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -491,     0,
       0,  -491,  -491,     0,  -491,  -491,     0,  -491,  -491,  -491,
    -491,  -491,  -491,  -491,     0,     0,     0,     0,     0,     0,
       0,  -493,  -493,  -493,     0,  -493,     0,     0,     0,  -493,
    -493,  -491,  -491,  -491,  -493,  -491,  -493,  -493,  -493,  -493,
    -493,  -493,  -493,  -491,  -493,     0,     0,  -493,  -493,  -493,
    -493,  -493,  -493,  -493,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -493,     0,     0,  -493,  -493,     0,  -493,
    -493,  -493,  -493,  -493,  -493,  -493,  -493,  -493,     0,     0,
    -493,  -493,     0,  -493,  -493,  -493,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -493,     0,     0,  -493,  -493,     0,
    -493,  -493,     0,  -493,  -493,  -493,  -493,  -493,  -493,  -493,
       0,     0,     0,     0,     0,     0,     0,  -492,  -492,  -492,
       0,  -492,     0,     0,     0,  -492,  -492,  -493,  -493,  -493,
    -492,  -493,  -492,  -492,  -492,  -492,  -492,  -492,  -492,  -493,
    -492,     0,     0,  -492,  -492,  -492,  -492,  -492,  -492,  -492,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -492,
       0,     0,  -492,  -492,     0,  -492,  -492,  -492,  -492,  -492,
    -492,  -492,  -492,  -492,     0,     0,  -492,  -492,     0,  -492,
    -492,  -492,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -492,     0,     0,  -492,  -492,     0,  -492,  -492,     0,  -492,
    -492,  -492,  -492,  -492,  -492,  -492,     0,     0,     0,     0,
       0,     0,     0,  -494,  -494,  -494,     0,  -494,     0,     0,
       0,  -494,  -494,  -492,  -492,  -492,  -494,  -492,  -494,  -494,
    -494,  -494,  -494,  -494,  -494,  -492,     0,     0,     0,  -494,
    -494,  -494,  -494,  -494,  -494,  -494,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -494,     0,     0,  -494,  -494,
       0,  -494,  -494,  -494,  -494,  -494,  -494,  -494,  -494,  -494,
       0,     0,  -494,  -494,     0,  -494,  -494,  -494,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -494,   728,     0,  -494,
    -494,     0,  -494,  -494,     0,  -494,  -494,  -494,  -494,  -494,
    -494,  -494,     0,     0,     0,     0,   -89,     0,     0,  -495,
    -495,  -495,     0,  -495,     0,     0,     0,  -495,  -495,  -494,
    -494,  -494,  -495,     0,  -495,  -495,  -495,  -495,  -495,  -495,
    -495,  -494,     0,     0,     0,  -495,  -495,  -495,  -495,  -495,
    -495,  -495,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -495,     0,     0,  -495,  -495,     0,  -495,  -495,  -495,
    -495,  -495,  -495,  -495,  -495,  -495,     0,     0,  -495,  -495,
       0,  -495,  -495,  -495,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -495,   663,     0,  -495,  -495,     0,  -495,  -495,
       0,  -495,  -495,  -495,  -495,  -495,  -495,  -495,     0,     0,
       0,     0,   -91,     0,     0,  -259,  -259,  -259,     0,  -259,
       0,     0,     0,  -259,  -259,  -495,  -495,  -495,  -259,     0,
    -259,  -259,  -259,  -259,  -259,  -259,  -259,  -495,     0,     0,
       0,  -259,  -259,  -259,  -259,  -259,  -259,  -259,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -259,     0,     0,
    -259,  -259,     0,  -259,  -259,  -259,  -259,  -259,  -259,  -259,
    -259,  -259,     0,     0,  -259,  -259,     0,  -259,  -259,  -259,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -259,     0,
       0,  -259,  -259,     0,  -259,  -259,     0,  -259,  -259,  -259,
    -259,  -259,  -259,  -259,     0,     0,     0,     0,     0,     0,
       0,  -259,  -259,  -259,     0,  -259,     0,     0,     0,  -259,
    -259,  -259,  -259,  -259,  -259,     0,  -259,  -259,  -259,  -259,
    -259,  -259,  -259,   240,     0,     0,     0,  -259,  -259,  -259,
    -259,  -259,  -259,  -259,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -259,     0,     0,  -259,  -259,     0,  -259,
    -259,  -259,  -259,  -259,  -259,  -259,  -259,  -259,     0,     0,
    -259,  -259,     0,  -259,  -259,  -259,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -259,     0,     0,  -259,  -259,     0,
    -259,  -259,     0,  -259,  -259,  -259,  -259,  -259,  -259,  -259,
       0,     0,     0,     0,     0,     0,     0,  -496,  -496,  -496,
       0,  -496,     0,     0,     0,  -496,  -496,  -259,  -259,  -259,
    -496,     0,  -496,  -496,  -496,  -496,  -496,  -496,  -496,   243,
       0,     0,     0,  -496,  -496,  -496,  -496,  -496,  -496,  -496,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -496,
       0,     0,  -496,  -496,     0,  -496,  -496,  -496,  -496,  -496,
    -496,  -496,  -496,  -496,     0,     0,  -496,  -496,     0,  -496,
    -496,  -496,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -496,     0,     0,  -496,  -496,     0,  -496,  -496,     0,  -496,
    -496,  -496,  -496,  -496,  -496,  -496,     0,     0,     0,     0,
       0,     0,     0,  -497,  -497,  -497,     0,  -497,     0,     0,
       0,  -497,  -497,  -496,  -496,  -496,  -497,     0,  -497,  -497,
    -497,  -497,  -497,  -497,  -497,  -496,     0,     0,     0,  -497,
    -497,  -497,  -497,  -497,  -497,  -497,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -497,     0,     0,  -497,  -497,
       0,  -497,  -497,  -497,  -497,  -497,  -497,  -497,  -497,  -497,
       0,     0,  -497,  -497,     0,  -497,  -497,  -497,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -497,     0,     0,  -497,
    -497,     0,  -497,  -497,     0,  -497,  -497,  -497,  -497,  -497,
    -497,  -497,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -497,
    -497,  -497,   118,   119,   120,   121,   122,   123,   124,   125,
       0,  -497,   126,   127,   128,   129,   130,     0,     0,   131,
     132,   133,   134,   135,   136,   137,     0,     0,   138,   139,
     140,   195,   196,   197,   198,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   199,   200,   159,
     201,   161,   283,   284,   202,   285,     0,     0,     0,     0,
       0,     0,     0,     0,   163,   164,     0,   165,   166,   167,
     168,     0,   169,   170,     0,     0,   171,     0,     0,     0,
     172,   173,   174,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,     0,     0,
     187,   118,   119,   120,   121,   122,   123,   124,   125,   188,
       0,   126,   127,   128,   129,   130,     0,     0,   131,   132,
     133,   134,   135,   136,   137,     0,     0,   138,   139,   140,
     195,   196,   197,   198,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   199,   200,   159,   201,
     161,   247,     0,   202,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   163,   164,     0,   165,   166,   167,   168,
       0,   169,   170,     0,     0,   171,     0,     0,     0,   172,
     173,   174,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,     0,    54,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,     0,     0,   187,
     118,   119,   120,   121,   122,   123,   124,   125,   188,     0,
     126,   127,   128,   129,   130,     0,     0,   131,   132,   133,
     134,   135,   136,   137,     0,     0,   138,   139,   140,   195,
     196,   197,   198,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   199,   200,   159,   201,   161,
       0,     0,   202,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   163,   164,     0,   165,   166,   167,   168,     0,
     169,   170,     0,     0,   171,     0,     0,     0,   172,   173,
     174,   175,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   176,     0,    54,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,     0,     0,   187,   118,
     119,   120,   121,   122,   123,   124,   125,   188,     0,   126,
     127,   128,   129,   130,     0,     0,   131,   132,   133,   134,
     135,   136,   137,     0,     0,   138,   139,   140,   195,   196,
     197,   198,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   199,   200,   159,   201,   161,     0,
       0,   202,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   163,   164,     0,   165,   166,   167,   168,     0,   169,
     170,     0,     0,   171,     0,     0,     0,   172,   173,   174,
     175,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   176,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,     0,     0,   187,     0,     5,
       6,     7,     0,     9,     0,     0,   188,    10,    11,     0,
       0,     0,    12,     0,    13,    14,    15,    98,    99,    18,
      19,     0,     0,     0,     0,   100,    21,    22,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,     0,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   221,     0,     0,   108,    50,     0,    51,    52,
       0,   222,   223,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     8,     9,    59,   224,    61,    10,    11,
       0,     0,     0,    12,   415,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,    33,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,     0,     0,    49,    50,     0,    51,
      52,     0,    53,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    59,    60,    61,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,     0,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,     0,    43,    44,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   221,
       0,     0,   108,    50,     0,    51,    52,     0,   617,   223,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,    60,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,    21,
      22,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,     0,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
       0,    43,    44,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   221,     0,     0,   108,    50,
       0,    51,    52,     0,   222,   223,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   224,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,    21,    22,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,     0,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,     0,    43,    44,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   221,     0,     0,   108,   417,     0,    51,    52,     0,
     222,   223,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    59,   224,    61,    12,     0,    13,
      14,    15,    98,    99,    18,    19,     0,     0,     0,     0,
     100,   101,   102,    23,    24,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,     0,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     0,     0,    43,    44,     0,    45,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   221,     0,     0,
     108,    50,     0,    51,    52,     0,   604,   223,    54,    55,
      56,    57,    58,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      59,   224,    61,    12,     0,    13,    14,    15,    98,    99,
      18,    19,     0,     0,     0,     0,   100,   101,   102,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   221,     0,     0,   108,    50,     0,    51,
      52,     0,   613,   223,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    59,   224,    61,    12,
       0,    13,    14,    15,    98,    99,    18,    19,     0,     0,
       0,     0,   100,    21,    22,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,     0,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,     0,    43,    44,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   221,
       0,     0,   108,    50,     0,    51,    52,     0,   604,   223,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   224,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,     0,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
       0,    43,    44,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   221,     0,     0,   108,    50,
       0,    51,    52,     0,   812,   223,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   224,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,     0,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,     0,    43,    44,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   221,     0,     0,   108,    50,     0,    51,    52,     0,
     816,   223,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    59,   224,    61,    12,     0,    13,
      14,    15,    98,    99,    18,    19,     0,     0,     0,     0,
     100,   101,   102,    23,    24,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,     0,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     0,     0,    43,    44,     0,    45,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   221,     0,     0,
     108,    50,     0,    51,    52,     0,   821,   223,    54,    55,
      56,    57,    58,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      59,   224,    61,    12,     0,    13,    14,    15,    98,    99,
      18,    19,     0,     0,     0,     0,   100,   101,   102,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   221,     0,     0,   108,    50,     0,    51,
      52,     0,   885,   223,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    59,   224,    61,    12,
       0,    13,    14,    15,    98,    99,    18,    19,     0,     0,
       0,     0,   100,   101,   102,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,     0,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,     0,    43,    44,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   221,
       0,     0,   108,    50,     0,    51,    52,     0,   887,   223,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   224,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,     0,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
       0,    43,    44,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   221,     0,     0,   108,    50,
       0,    51,    52,     0,   908,   223,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   224,
      61,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,     0,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,     0,    43,    44,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   221,     0,     0,   108,    50,     0,    51,    52,     0,
       0,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    59,    60,    61,    12,     0,    13,
      14,    15,    98,    99,    18,    19,     0,     0,     0,     0,
     100,    21,    22,    23,    24,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,     0,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     0,     0,    43,    44,     0,    45,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   221,     0,     0,
     108,    50,     0,    51,    52,     0,   268,     0,    54,    55,
      56,    57,    58,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      59,   224,    61,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   221,     0,     0,   108,    50,     0,    51,
      52,     0,   484,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    59,   224,    61,    12,
       0,    13,    14,    15,    98,    99,    18,    19,     0,     0,
       0,     0,   100,   101,   102,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,     0,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,     0,    43,    44,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   221,
       0,     0,   108,    50,     0,    51,    52,     0,   641,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   224,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,     0,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
       0,    43,    44,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   221,     0,     0,   108,    50,
       0,    51,    52,     0,   484,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   224,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,     0,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,     0,    43,    44,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   221,     0,     0,   108,    50,     0,    51,    52,     0,
     712,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    59,   224,    61,    12,     0,    13,
      14,    15,    98,    99,    18,    19,     0,     0,     0,     0,
     100,   101,   102,    23,    24,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,     0,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     0,     0,    43,    44,     0,    45,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   221,     0,     0,
     108,    50,     0,    51,    52,     0,   759,     0,    54,    55,
      56,    57,    58,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      59,   224,    61,    12,     0,    13,    14,    15,    98,    99,
      18,    19,     0,     0,     0,     0,   100,   101,   102,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   221,     0,     0,   108,    50,     0,    51,
      52,     0,   852,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    59,   224,    61,    12,
       0,    13,    14,    15,    98,    99,    18,    19,     0,     0,
       0,     0,   100,   101,   102,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,     0,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,     0,    43,    44,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   221,
       0,     0,   108,    50,     0,    51,    52,     0,     0,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   224,    61,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,     0,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
       0,    43,    44,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   221,     0,     0,   108,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   224,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     103,     0,     0,    32,    33,     0,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,     0,    43,    44,     0,
       0,     0,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   393,     0,     0,    49,    50,     0,    51,    52,     0,
      53,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       5,     6,     7,     0,     9,     0,   756,     0,    10,    11,
       0,     0,     0,    12,   109,    13,    14,    15,    98,    99,
      18,    19,     0,     0,     0,     0,   100,   101,   102,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   103,     0,     0,    32,    33,     0,   104,    35,
      36,    37,   105,    39,    40,    41,    42,     0,     0,    43,
      44,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     106,     0,     0,   107,     0,     0,   108,    50,     0,    51,
      52,     0,     0,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   109,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,     0,     0,    32,    33,     0,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
       0,    43,    44,     0,     0,     0,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   278,     0,     0,   316,    50,
       0,    51,    52,     0,   317,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,     0,     0,     0,    12,   109,    13,
      14,    15,    98,    99,    18,    19,     0,     0,     0,     0,
     100,   101,   102,    23,    24,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   103,     0,     0,    32,
      33,     0,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     0,     0,    43,    44,     0,     0,     0,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   393,     0,     0,
      49,    50,     0,    51,    52,     0,    53,     0,    54,    55,
      56,    57,    58,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,     0,     0,     0,    12,
     109,    13,    14,    15,    98,    99,    18,    19,     0,     0,
       0,     0,   100,   101,   102,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   103,     0,
       0,    32,    33,     0,   104,    35,    36,    37,   105,    39,
      40,    41,    42,     0,     0,    43,    44,     0,     0,     0,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   107,
       0,     0,   108,    50,     0,    51,    52,     0,     0,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,     0,     0,
       0,    12,   109,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     103,     0,     0,    32,    33,     0,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,     0,    43,    44,     0,
       0,     0,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   278,     0,     0,   108,    50,     0,    51,    52,     0,
       0,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
       0,     0,     0,    12,   109,    13,    14,    15,    98,    99,
      18,    19,     0,     0,     0,     0,   100,   101,   102,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   103,     0,     0,    32,    33,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,     0,    43,
      44,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   842,     0,     0,   108,    50,     0,    51,
      52,     0,     0,     0,    54,    55,    56,    57,    58,     0,
     522,   523,     0,     0,   524,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   163,   164,   109,   165,   166,   167,
     168,     0,   169,   170,     0,     0,   171,     0,     0,     0,
     172,   173,   174,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   529,   258,
     187,     0,   530,     0,     0,     0,     0,     0,     0,   188,
       0,     0,   163,   164,     0,   165,   166,   167,   168,     0,
     169,   170,     0,     0,   171,     0,     0,     0,   172,   173,
     174,   175,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   176,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   547,   523,   187,     0,
     548,     0,     0,     0,     0,     0,     0,   188,     0,     0,
     163,   164,     0,   165,   166,   167,   168,     0,   169,   170,
       0,     0,   171,     0,     0,     0,   172,   173,   174,   175,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     176,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   608,   523,   187,     0,   609,     0,
       0,     0,     0,     0,     0,   188,     0,     0,   163,   164,
       0,   165,   166,   167,   168,     0,   169,   170,     0,     0,
     171,     0,     0,     0,   172,   173,   174,   175,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   176,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   611,   258,   187,     0,   612,     0,     0,     0,
       0,     0,     0,   188,     0,     0,   163,   164,     0,   165,
     166,   167,   168,     0,   169,   170,     0,     0,   171,     0,
       0,     0,   172,   173,   174,   175,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     643,   523,   187,     0,   644,     0,     0,     0,     0,     0,
       0,   188,     0,     0,   163,   164,     0,   165,   166,   167,
     168,     0,   169,   170,     0,     0,   171,     0,     0,     0,
     172,   173,   174,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   646,   258,
     187,     0,   647,     0,     0,     0,     0,     0,     0,   188,
       0,     0,   163,   164,     0,   165,   166,   167,   168,     0,
     169,   170,     0,     0,   171,     0,     0,     0,   172,   173,
     174,   175,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   176,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   719,   523,   187,     0,
     720,     0,     0,     0,     0,     0,     0,   188,     0,     0,
     163,   164,     0,   165,   166,   167,   168,     0,   169,   170,
       0,     0,   171,     0,     0,     0,   172,   173,   174,   175,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     176,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   722,   258,   187,     0,   723,     0,
       0,     0,     0,     0,     0,   188,     0,     0,   163,   164,
       0,   165,   166,   167,   168,     0,   169,   170,     0,     0,
     171,     0,     0,     0,   172,   173,   174,   175,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   176,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   892,   523,   187,     0,   893,     0,     0,     0,
       0,     0,     0,   188,     0,     0,   163,   164,     0,   165,
     166,   167,   168,     0,   169,   170,     0,     0,   171,     0,
       0,     0,   172,   173,   174,   175,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     895,   258,   187,     0,   896,     0,     0,     0,     0,     0,
       0,   188,     0,     0,   163,   164,     0,   165,   166,   167,
     168,     0,   169,   170,     0,     0,   171,     0,     0,     0,
     172,   173,   174,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,     0,     0,
     187,     0,     0,     0,     0,     0,     0,     0,     0,   188,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,     0,     0,   333,   334,     0,     0,     0,
    -225,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,     0,     0,   333,   334,     0,   335,
       0,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     335,  -225,   336,   337,   338,   339,   340,   341,   342,   343,
     344,   345,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   600,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,   330,  -515,  -515,     0,     0,   333,   334,
     320,  -515,  -515,  -515,  -515,   325,   326,     0,     0,  -515,
    -515,     0,     0,     0,     0,   333,   334,     0,     0,     0,
       0,     0,     0,     0,   336,   337,   338,   339,   340,   341,
     342,   343,   344,   345,     0,     0,     0,     0,     0,     0,
       0,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   320,   321,   322,   323,   324,   325,   326,     0,     0,
     329,   330,     0,     0,     0,     0,   333,   334,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   336,   337,   338,   339,   340,   341,   342,   343,
     344,   345
};

static const yytype_int16 yycheck[] =
{
      16,    17,     7,    28,    20,    16,    17,     7,     8,    20,
      16,    17,    22,   392,    20,    16,    17,   347,    63,    20,
       5,     6,   243,   386,   375,   579,    22,   103,    28,    45,
      46,     8,     8,   220,     4,    51,    52,   107,   319,    17,
      51,    60,    20,   373,    60,    61,    52,   533,   534,    54,
     693,    28,    28,   377,    54,   570,   690,   572,     1,   693,
      16,    17,    13,    49,    20,    60,   211,    12,    53,   214,
     413,    53,   289,   218,   219,   292,   586,    13,    47,   409,
      13,   535,   536,    13,    26,    95,   263,    72,    51,    27,
      36,    37,   102,    15,    87,    17,    52,    68,    60,    95,
      15,     0,    17,    76,     2,    96,   116,    87,   451,   104,
      85,   129,    26,   106,    25,    87,   134,    51,    63,    36,
      37,    55,   113,    88,   665,   134,   106,    85,    55,   459,
     671,    85,    95,    96,   106,    87,   129,    68,   111,   316,
     102,   103,    25,   114,   115,   116,   117,   118,    50,   129,
     113,    49,   127,   116,   106,   123,   710,   108,    28,   134,
     305,   306,   307,   308,   129,   106,   111,   110,    25,   127,
     108,   133,   108,   127,    26,   108,   133,    25,   108,    85,
     131,   123,   129,   134,   135,   116,   117,   118,   134,   131,
      25,   134,   135,   269,   837,   546,   134,   135,   134,   135,
     111,   134,   135,   837,   134,   135,   222,   223,   224,   123,
     108,   128,   134,   135,   565,    25,   106,   129,   129,    87,
     131,   127,   134,   106,   240,   679,   242,   243,   111,   240,
     133,   242,   243,    85,   240,   220,   242,   243,   106,   240,
     564,   242,   243,    68,    98,   128,   129,    87,   131,    87,
     467,    54,   268,    56,   111,   129,   742,   743,    83,    84,
     134,   129,   240,   111,   242,   243,    25,   597,   106,   784,
     785,   123,   129,   125,   789,   127,   111,   263,   131,   131,
     132,   129,   131,   131,   240,   129,   242,   243,    25,   114,
     115,   116,   117,   118,   129,   314,    15,   578,   314,   853,
     319,   111,   372,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   129,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     316,   347,   317,    85,   304,   317,   347,   356,   357,   309,
     356,   357,   387,   853,    85,   421,   346,   423,    58,    59,
      25,   714,   111,   135,   252,   870,    55,   373,   314,    87,
     236,   106,   373,   319,    85,   263,    88,    60,   444,   445,
     129,    64,   372,    85,   111,   127,   129,   346,   106,   348,
     349,   350,   351,   726,   125,    85,   127,   403,   404,   405,
      25,   132,   129,   409,   737,   411,   412,   413,   409,   405,
     545,   417,    87,   428,   625,   374,   127,    87,   408,   102,
     103,   417,   129,   125,   430,   127,   417,   131,   316,   131,
     132,   106,    26,   368,   440,   776,   106,   127,   428,   792,
      85,   810,   448,    85,    87,   451,   111,   453,   438,   408,
      85,   410,   387,   459,    26,    87,   391,   392,   459,   129,
      25,   428,   428,   106,   129,    60,   472,   473,   458,     2,
      87,     4,     5,     6,   134,   135,   790,   654,   484,    60,
     439,   106,   127,    16,    17,   127,   111,    20,   554,   106,
     125,    85,   127,   123,    27,    68,   849,   132,    58,    59,
     841,   460,    85,   128,   129,   511,   128,   102,   103,   104,
      85,   126,   129,    85,    85,    53,    49,    50,    51,   128,
      53,   102,   103,   104,    62,    63,   526,    60,   861,   123,
     128,   125,   272,   127,   274,   401,   276,   131,   132,    72,
     526,   106,   125,   431,   127,   531,   111,   533,   534,   132,
     550,   123,   127,   125,   874,   127,   127,    85,   124,   131,
     132,   576,    55,   128,   129,    36,    37,    68,   901,   128,
     129,    68,   578,   106,   104,   108,    14,    15,    85,   104,
     575,   587,    83,    84,    60,   575,   576,    68,    61,   724,
     129,   597,    53,    54,    60,    56,   597,   125,   604,   127,
      85,    62,    63,    94,   132,    10,   596,   613,   543,   576,
     576,   617,   113,   114,   115,   116,   117,   118,   125,   625,
     127,   687,     8,    17,   625,   132,   102,   103,   104,   625,
      87,   566,   578,    15,    55,   641,   102,   103,   104,   598,
     125,   587,   127,     2,    25,     4,   129,   132,   128,   106,
     129,   128,   658,   662,   660,   124,   662,   663,   667,   668,
     128,   667,   668,   798,   129,   129,   133,   626,   627,   129,
     128,    85,   129,   126,    85,   741,   209,   210,   211,   654,
     686,   214,   126,    10,   106,   218,   219,   220,   754,    51,
      49,    53,    54,    55,    56,   583,   129,   585,   129,    51,
      51,   589,    53,    54,    55,    56,   712,   240,    85,   242,
     243,   125,   129,   127,   125,    51,   127,    15,   132,   252,
     726,   132,   728,    13,   730,   731,    10,    96,   734,   129,
     263,   737,   128,    95,    96,   126,   129,   124,   734,   128,
      85,   607,   126,   734,    95,    96,   681,   635,   125,   108,
     127,   113,    85,   759,   116,   132,   762,   623,   767,    10,
     128,   767,   113,   128,   128,   116,    10,    88,     9,   131,
      10,   304,   305,   306,   307,   308,   309,   310,   311,    85,
     125,   314,   127,   316,   317,   129,   319,   132,   734,    68,
      10,   126,   125,   124,   127,   111,   128,    10,   804,   132,
     129,   129,   128,    10,    83,    84,   812,   813,    10,    10,
     816,   129,   129,   128,   347,   821,   791,   705,    10,   125,
      10,   127,   124,   356,   357,    10,   132,    55,   124,   717,
     718,    10,   111,   112,   113,   114,   115,   116,   117,   118,
     373,   129,   375,   376,   126,    10,   852,    72,     6,   898,
     209,   654,    79,   859,   876,   861,    82,     7,   564,   725,
     790,   690,    -1,   859,   690,    63,   732,   733,   874,    -1,
     758,    -1,    -1,   874,    -1,   810,   409,    -1,    -1,   885,
      -1,   887,    -1,   873,   417,   773,   774,    -1,    -1,    -1,
      27,    -1,    -1,   252,    -1,   901,    -1,    -1,   431,    -1,
      -1,    -1,   908,    -1,   263,    -1,    -1,   795,    39,    40,
      41,    42,    43,    50,    -1,    -1,   875,   805,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   459,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   304,   834,    -1,    -1,    -1,
     309,    -1,   818,   819,    -1,    -1,    -1,   316,    -1,   847,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   106,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     2,    -1,     4,    -1,    -1,    -1,    -1,    -1,   877,
      -1,   857,   858,    -1,   882,    16,    17,   863,     2,    20,
       4,     5,     6,     7,    -1,    -1,    27,    -1,    -1,    -1,
      -1,    -1,   545,   546,    -1,    -1,   375,    -1,    -1,    -1,
     886,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,
      51,    -1,   565,    -1,   900,    -1,    -1,   903,    -1,    60,
      -1,    -1,    -1,    -1,   910,    49,    -1,    -1,    -1,    53,
     583,    -1,   585,    -1,    -1,    -1,   589,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   597,    -1,    -1,    -1,    72,    -1,
      -1,    -1,   431,   210,   211,    -1,    -1,   214,    -1,    -1,
      -1,   218,   219,    43,    -1,   106,    -1,   108,    -1,    -1,
      -1,    -1,   625,    -1,    -1,    -1,    -1,    -1,   631,    -1,
      -1,    -1,   635,    -1,   108,    -1,    -1,    -1,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,   654,    -1,    83,    84,    -1,    -1,    -1,    -1,   662,
      -1,    -1,    -1,    -1,   667,   668,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,    -1,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   305,   306,
     307,   308,   705,   310,   311,    -1,     2,    -1,     4,     5,
       6,    -1,    -1,    -1,   717,   718,    -1,   546,   209,   210,
     211,   724,    -1,   214,    -1,    -1,    -1,   218,   219,    -1,
      -1,    -1,    -1,    -1,    -1,   209,   565,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   220,    -1,    -1,   240,
      -1,   242,   243,    49,   583,   758,   585,    53,    -1,    -1,
     589,   252,    -1,    -1,   767,    -1,    -1,    -1,    -1,   376,
     773,   774,   263,   776,    -1,    -1,    72,    -1,   252,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   791,   263,
      -1,    -1,   795,    -1,    -1,   798,    -1,    -1,    -1,    -1,
      -1,    -1,   805,    -1,    -1,    -1,   635,    -1,    -1,    -1,
     417,    -1,   108,   304,   305,   306,   307,   308,   309,   310,
     311,    -1,    -1,   314,    -1,   316,    -1,    -1,   319,    -1,
     304,   834,    -1,    -1,    -1,   309,    -1,    -1,   841,    -1,
      -1,    -1,   316,   317,   847,    -1,    -1,    -1,    -1,     2,
      -1,     4,    -1,    -1,    -1,    -1,   347,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   356,   357,    -1,    -1,    -1,
      -1,   874,    -1,    -1,   877,    -1,   705,    -1,    -1,   882,
      -1,    -1,   373,    -1,   375,   376,    -1,    -1,   717,   718,
      -1,    -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,
      -1,   375,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   209,    -1,    -1,    -1,    -1,   409,    -1,
      -1,    -1,    -1,    -1,   220,    -1,   417,    -1,    -1,   758,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   545,    -1,
     431,    -1,    -1,    -1,   773,   774,    -1,   776,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   108,   252,   431,    -1,    43,
      -1,    -1,    -1,    -1,    -1,    -1,   795,   263,   459,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   805,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    -1,    -1,    83,
      84,    -1,    -1,    -1,    -1,   834,    -1,    -1,   304,    -1,
     222,   223,   841,   309,    -1,    -1,    -1,    -1,   847,    -1,
     316,   317,    -1,   107,   631,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   129,    -1,    -1,   877,    -1,
      -1,    -1,    -1,   882,   545,   546,   209,    51,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   220,    62,    -1,
      -1,    -1,   546,    -1,   565,    -1,    -1,    -1,    -1,   375,
      68,    69,    70,    71,    72,    73,    74,    75,    -1,    77,
      78,   565,   583,    -1,   585,    83,    84,    -1,   589,   252,
      -1,    -1,   314,    -1,    -1,    -1,   597,   319,    -1,   583,
     263,   585,    -1,    -1,    -1,   589,    -1,   724,    -1,    -1,
      -1,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,    -1,    -1,    -1,   625,   431,    -1,    -1,    -1,    -1,
     631,    -1,    -1,    -1,   635,    -1,    -1,    -1,    -1,    -1,
      -1,   304,    -1,    -1,    -1,    -1,   309,    -1,    -1,    -1,
      -1,   635,    -1,   316,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   662,    -1,    -1,    -1,    -1,   667,   668,    -1,    -1,
     654,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   798,   404,   405,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   413,    -1,    -1,    -1,   417,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   705,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   375,    -1,    -1,    -1,   717,   718,    -1,    -1,
      -1,   705,    -1,   724,    -1,    -1,   448,   231,    -1,   451,
      -1,   453,   236,   717,   718,    -1,    -1,    -1,    -1,   243,
     546,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   758,    -1,   565,
      -1,    -1,   484,    -1,    -1,    -1,   767,    -1,   431,    -1,
      -1,    -1,   773,   774,   758,   776,    -1,   583,    -1,   585,
      -1,    -1,    -1,   589,    -1,    -1,    -1,    -1,    -1,   773,
     774,    -1,   776,    -1,   795,    -1,    -1,   798,    -1,    -1,
      -1,    -1,    -1,    -1,   805,    -1,    -1,   791,    -1,    -1,
      -1,   795,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   805,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   635,
     636,    -1,    -1,   834,    -1,    -1,    -1,    -1,    -1,    -1,
     841,    -1,    -1,   347,    -1,    -1,   847,    -1,   654,    -1,
     834,    -1,    -1,    -1,    -1,    -1,   578,   841,    -1,    -1,
      -1,    -1,    -1,   847,    -1,   587,    -1,    -1,    -1,   373,
      -1,    -1,    -1,   874,    -1,    -1,   877,    -1,    -1,   383,
      -1,   882,   604,   546,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   613,    -1,   877,    -1,   617,    -1,   401,   882,   705,
      -1,    -1,   565,   625,    -1,   409,    -1,    -1,    -1,    -1,
      -1,   717,   718,    -1,    -1,    -1,    -1,    -1,   422,    -1,
     583,    -1,   585,    -1,    -1,    -1,   589,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   658,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   758,    -1,    -1,   459,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   686,    -1,    -1,   773,   774,    -1,
     776,    -1,   635,    -1,    -1,    -1,    -1,    -1,    -1,   483,
      -1,    -1,    -1,    -1,    -1,   791,    -1,    -1,    -1,   795,
     712,   654,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   805,
      -1,    -1,    -1,    -1,   726,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   737,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   531,   834,   533,
     534,   535,   536,    -1,    -1,   841,    -1,   759,    -1,    -1,
      -1,   847,   705,    -1,    -1,    -1,    -1,    -1,   552,    -1,
      -1,    -1,    -1,    -1,   717,   718,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   568,    -1,   570,    -1,   572,    -1,
      -1,   877,    -1,    -1,   578,   579,   882,    -1,    -1,    -1,
      -1,    -1,   804,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   597,   816,   758,    -1,    -1,    -1,   821,
      -1,    -1,    -1,   607,    -1,    -1,    -1,    -1,    -1,    -1,
     773,   774,    -1,   776,    -1,    -1,    -1,    -1,    -1,   623,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   791,    -1,
     852,    -1,   795,    -1,    -1,    -1,    -1,   859,    -1,   861,
      -1,    -1,   805,    -1,    -1,    -1,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    -1,
      -1,    83,    84,   885,    -1,   887,    -1,    -1,    -1,    -1,
      -1,   834,    -1,    -1,    -1,   679,    -1,    -1,   841,   901,
      -1,    -1,    -1,    -1,   847,   107,   908,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,    -1,    -1,   703,
      -1,    -1,   706,    -1,   708,    -1,   710,    -1,    -1,    -1,
      -1,    -1,   134,    -1,   877,    -1,    -1,    -1,    -1,   882,
      -1,   725,    -1,    -1,    -1,    -1,    -1,    -1,   732,   733,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   742,   743,
      -1,    -1,    -1,    -1,     0,     1,    -1,     3,     4,     5,
       6,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
     784,   785,    38,    -1,    -1,   789,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,    63,    -1,    65,
      66,    67,    -1,    -1,   818,   819,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      -1,    97,    98,    99,   100,   101,    -1,    -1,    -1,   853,
      -1,    -1,    -1,   857,   858,    -1,    -1,    -1,    -1,   863,
      -1,    -1,    -1,   119,   120,   121,   870,    -1,    -1,    -1,
     874,    -1,   876,    -1,    -1,     0,    -1,    -1,   134,   135,
      -1,    -1,   886,     8,     9,    10,    -1,    -1,    -1,    14,
      15,    -1,    17,    -1,   898,    -1,   900,    -1,    -1,   903,
      -1,    26,    -1,    -1,    -1,    -1,   910,    -1,    -1,    -1,
      -1,    36,    37,    -1,    39,    40,    41,    42,    43,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    -1,
      -1,    83,    84,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    -1,    -1,    83,    84,
      85,    -1,    87,    -1,    -1,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,    -1,    -1,    -1,
      -1,   106,   107,    -1,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,    -1,    -1,    -1,    -1,   123,   124,
     125,    -1,   127,   128,   129,     0,    -1,   132,    -1,   134,
     135,    -1,    -1,     8,     9,    10,    -1,    -1,    -1,    14,
      15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    36,    37,    -1,    39,    40,    41,    42,    43,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    -1,
      -1,    83,    84,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    -1,    -1,    83,    84,
      85,    -1,    87,    -1,    -1,   107,    -1,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,    -1,    -1,    -1,
      -1,   106,   107,    -1,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,    -1,    -1,    -1,    -1,    -1,   124,
     125,    -1,   127,   128,   129,     0,    -1,   132,    -1,   134,
     135,    -1,    -1,     8,     9,    10,    -1,    -1,    13,    14,
      15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    51,
      52,    36,    37,    55,    39,    40,    41,    42,    43,    -1,
      -1,    -1,    -1,    65,    66,    50,    68,    69,    70,    71,
      -1,    73,    74,    -1,    -1,    77,    -1,    -1,    -1,    81,
      82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,    -1,    -1,   121,
      -1,    -1,    -1,   108,    -1,    -1,    -1,    -1,   130,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   124,
      -1,    -1,   127,   128,    -1,    -1,   131,    -1,    -1,   134,
     135,     1,    -1,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    -1,    -1,    15,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    -1,    97,    98,    99,
     100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,
     120,   121,    -1,    -1,    -1,    -1,    -1,     1,    -1,     3,
       4,     5,     6,     7,   134,   135,    10,    11,    12,    -1,
      14,    15,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,
      44,    45,    46,    47,    48,    49,    -1,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    62,    63,
      -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    -1,    97,    98,    99,   100,   101,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   119,   120,   121,     1,    -1,
       3,     4,     5,     6,     7,    -1,    -1,    10,    11,    12,
     134,   135,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      -1,    44,    45,    46,    47,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    -1,    97,    98,    99,   100,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   119,   120,   121,     1,
      -1,     3,     4,     5,     6,     7,    -1,    -1,    10,    11,
      12,   134,   135,    15,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    48,    49,    -1,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    -1,    97,    98,    99,   100,   101,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,   120,   121,
       1,    -1,     3,     4,     5,     6,     7,    -1,     9,    10,
      11,    12,   134,   135,    -1,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    44,    45,    46,    47,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    -1,
      -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    95,    -1,    97,    98,    99,   100,
     101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,   120,
     121,     1,    -1,     3,     4,     5,     6,     7,    -1,    -1,
      -1,    11,    12,   134,   135,    -1,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    -1,    97,    98,    99,
     100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,
     120,   121,    -1,    -1,    -1,    -1,    -1,     1,   128,     3,
       4,     5,     6,     7,   134,   135,    -1,    11,    12,    -1,
      -1,    -1,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,
      44,    45,    46,    47,    48,    49,    -1,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    62,    63,
      -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    -1,    97,    98,    99,   100,   101,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   119,   120,   121,    -1,    -1,
      -1,    -1,    -1,     1,   128,     3,     4,     5,     6,     7,
     134,   135,    -1,    11,    12,    -1,    -1,    -1,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,
      48,    49,    -1,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    62,    63,    -1,    65,    66,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,    97,
      98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   119,   120,   121,    -1,    -1,   124,    -1,    -1,     1,
      -1,     3,     4,     5,     6,     7,   134,   135,    10,    11,
      12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    48,    49,    -1,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    -1,    97,    98,    99,   100,   101,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,   120,   121,
      -1,     3,     4,     5,     6,     7,    -1,    -1,    -1,    11,
      12,    -1,   134,   135,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    48,    49,    -1,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    -1,    97,    98,    99,   100,   101,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,   120,   121,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    -1,   135,    13,    14,    15,    16,    17,    -1,
      -1,    20,    21,    22,    23,    24,    25,    26,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    -1,    68,
      69,    70,    71,    -1,    73,    74,    -1,    -1,    77,    -1,
      -1,    -1,    81,    82,    83,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
      -1,    -1,   121,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,   130,   131,    11,    12,    -1,    -1,    -1,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    26,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    -1,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    62,    63,    -1,    65,    66,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    -1,    92,    93,    -1,    95,    96,    97,
      98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   119,   120,   121,    16,   123,    18,    19,    20,    21,
      22,    23,    24,   131,    26,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    96,    97,    98,    99,   100,   101,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   119,   120,   121,
      16,   123,    18,    19,    20,    21,    22,    23,    24,   131,
      26,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    -1,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,    63,    -1,    65,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      96,    97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   119,   120,   121,    16,   123,    18,    19,
      20,    21,    22,    23,    24,   131,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    96,    97,    98,    99,
     100,   101,    -1,    -1,    -1,    -1,   106,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   119,
     120,   121,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,   131,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    62,    63,
      -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    87,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    96,    97,    98,    99,   100,   101,    -1,    -1,
      -1,    -1,   106,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   119,   120,   121,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,   131,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    -1,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    62,    63,    -1,    65,    66,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    -1,    92,    93,    -1,    95,    96,    97,
      98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   119,   120,   121,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,   131,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    96,    97,    98,    99,   100,   101,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   119,   120,   121,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,   131,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    -1,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,    63,    -1,    65,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      96,    97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   119,   120,   121,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,   131,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    96,    97,    98,    99,
     100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,
     120,   121,     3,     4,     5,     6,     7,     8,     9,    10,
      -1,   131,    13,    14,    15,    16,    17,    -1,    -1,    20,
      21,    22,    23,    24,    25,    26,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    65,    66,    -1,    68,    69,    70,
      71,    -1,    73,    74,    -1,    -1,    77,    -1,    -1,    -1,
      81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
     121,     3,     4,     5,     6,     7,     8,     9,    10,   130,
      -1,    13,    14,    15,    16,    17,    -1,    -1,    20,    21,
      22,    23,    24,    25,    26,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    65,    66,    -1,    68,    69,    70,    71,
      -1,    73,    74,    -1,    -1,    77,    -1,    -1,    -1,    81,
      82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    95,    -1,    97,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,    -1,    -1,   121,
       3,     4,     5,     6,     7,     8,     9,    10,   130,    -1,
      13,    14,    15,    16,    17,    -1,    -1,    20,    21,    22,
      23,    24,    25,    26,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    65,    66,    -1,    68,    69,    70,    71,    -1,
      73,    74,    -1,    -1,    77,    -1,    -1,    -1,    81,    82,
      83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    95,    -1,    97,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,    -1,    -1,   121,     3,
       4,     5,     6,     7,     8,     9,    10,   130,    -1,    13,
      14,    15,    16,    17,    -1,    -1,    20,    21,    22,    23,
      24,    25,    26,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    -1,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    65,    66,    -1,    68,    69,    70,    71,    -1,    73,
      74,    -1,    -1,    77,    -1,    -1,    -1,    81,    82,    83,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,    -1,    -1,   121,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,   130,    11,    12,    -1,
      -1,    -1,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    62,    63,
      -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    96,    97,    98,    99,   100,   101,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,   119,   120,   121,    11,    12,
      -1,    -1,    -1,    16,   128,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      -1,    44,    45,    46,    47,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    -1,    97,    98,    99,   100,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   119,   120,   121,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,    62,    63,    -1,    65,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    96,
      97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   119,   120,   121,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    -1,
      -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    95,    96,    97,    98,    99,   100,
     101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   119,   120,
     121,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    -1,
      65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      95,    96,    97,    98,    99,   100,   101,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   119,   120,   121,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    -1,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    -1,    -1,    62,    63,    -1,    65,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    96,    97,    98,
      99,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     119,   120,   121,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    96,    97,    98,    99,   100,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   119,   120,   121,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,    62,    63,    -1,    65,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    96,
      97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   119,   120,   121,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    -1,
      -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    95,    96,    97,    98,    99,   100,
     101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   119,   120,
     121,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    -1,
      65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      95,    96,    97,    98,    99,   100,   101,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   119,   120,   121,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    -1,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    -1,    -1,    62,    63,    -1,    65,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    96,    97,    98,
      99,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     119,   120,   121,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    96,    97,    98,    99,   100,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   119,   120,   121,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,    62,    63,    -1,    65,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    96,
      97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   119,   120,   121,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    -1,
      -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    95,    96,    97,    98,    99,   100,
     101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   119,   120,
     121,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    -1,
      65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      -1,    -1,    97,    98,    99,   100,   101,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   119,   120,   121,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    -1,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    -1,    -1,    62,    63,    -1,    65,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    -1,    97,    98,
      99,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     119,   120,   121,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    -1,    97,    98,    99,   100,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   119,   120,   121,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,    62,    63,    -1,    65,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,
      97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   119,   120,   121,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    -1,
      -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    95,    -1,    97,    98,    99,   100,
     101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   119,   120,
     121,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    -1,
      65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      95,    -1,    97,    98,    99,   100,   101,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   119,   120,   121,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    -1,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    -1,    -1,    62,    63,    -1,    65,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    -1,    97,    98,
      99,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     119,   120,   121,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    -1,    97,    98,    99,   100,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   119,   120,   121,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,    62,    63,    -1,    65,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    -1,    -1,
      97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   119,   120,   121,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    -1,
      -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    -1,    -1,    97,    98,    99,   100,
     101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   119,   120,
     121,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    -1,
      -1,    -1,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      95,    -1,    97,    98,    99,   100,   101,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,   111,    -1,    11,    12,
      -1,    -1,    -1,    16,   119,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      63,    -1,    -1,    -1,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    -1,    -1,    97,    98,    99,   100,   101,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,   119,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    -1,
      -1,    62,    63,    -1,    -1,    -1,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    95,    -1,    97,    98,    99,   100,
     101,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,    -1,    -1,    -1,    16,   119,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    -1,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    -1,    -1,    62,    63,    -1,    -1,    -1,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    -1,    97,    98,
      99,   100,   101,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,
     119,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,    62,    63,    -1,    -1,    -1,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    -1,    -1,
      97,    98,    99,   100,   101,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,
      -1,    16,   119,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    -1,
      -1,    -1,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      -1,    -1,    97,    98,    99,   100,   101,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
      -1,    -1,    -1,    16,   119,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      63,    -1,    -1,    -1,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    -1,    -1,    97,    98,    99,   100,   101,    -1,
      51,    52,    -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    65,    66,   119,    68,    69,    70,
      71,    -1,    73,    74,    -1,    -1,    77,    -1,    -1,    -1,
      81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    51,    52,
     121,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,   130,
      -1,    -1,    65,    66,    -1,    68,    69,    70,    71,    -1,
      73,    74,    -1,    -1,    77,    -1,    -1,    -1,    81,    82,
      83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,    51,    52,   121,    -1,
      55,    -1,    -1,    -1,    -1,    -1,    -1,   130,    -1,    -1,
      65,    66,    -1,    68,    69,    70,    71,    -1,    73,    74,
      -1,    -1,    77,    -1,    -1,    -1,    81,    82,    83,    84,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,    51,    52,   121,    -1,    55,    -1,
      -1,    -1,    -1,    -1,    -1,   130,    -1,    -1,    65,    66,
      -1,    68,    69,    70,    71,    -1,    73,    74,    -1,    -1,
      77,    -1,    -1,    -1,    81,    82,    83,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,    51,    52,   121,    -1,    55,    -1,    -1,    -1,
      -1,    -1,    -1,   130,    -1,    -1,    65,    66,    -1,    68,
      69,    70,    71,    -1,    73,    74,    -1,    -1,    77,    -1,
      -1,    -1,    81,    82,    83,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
      51,    52,   121,    -1,    55,    -1,    -1,    -1,    -1,    -1,
      -1,   130,    -1,    -1,    65,    66,    -1,    68,    69,    70,
      71,    -1,    73,    74,    -1,    -1,    77,    -1,    -1,    -1,
      81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    51,    52,
     121,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,   130,
      -1,    -1,    65,    66,    -1,    68,    69,    70,    71,    -1,
      73,    74,    -1,    -1,    77,    -1,    -1,    -1,    81,    82,
      83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,    51,    52,   121,    -1,
      55,    -1,    -1,    -1,    -1,    -1,    -1,   130,    -1,    -1,
      65,    66,    -1,    68,    69,    70,    71,    -1,    73,    74,
      -1,    -1,    77,    -1,    -1,    -1,    81,    82,    83,    84,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,    51,    52,   121,    -1,    55,    -1,
      -1,    -1,    -1,    -1,    -1,   130,    -1,    -1,    65,    66,
      -1,    68,    69,    70,    71,    -1,    73,    74,    -1,    -1,
      77,    -1,    -1,    -1,    81,    82,    83,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,    51,    52,   121,    -1,    55,    -1,    -1,    -1,
      -1,    -1,    -1,   130,    -1,    -1,    65,    66,    -1,    68,
      69,    70,    71,    -1,    73,    74,    -1,    -1,    77,    -1,
      -1,    -1,    81,    82,    83,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
      51,    52,   121,    -1,    55,    -1,    -1,    -1,    -1,    -1,
      -1,   130,    -1,    -1,    65,    66,    -1,    68,    69,    70,
      71,    -1,    73,    74,    -1,    -1,    77,    -1,    -1,    -1,
      81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
     121,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    -1,    -1,    -1,
      88,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    -1,    -1,    83,    84,    -1,   107,
      -1,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     107,   129,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   129,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    -1,    -1,    83,    84,
      68,    69,    70,    71,    72,    73,    74,    -1,    -1,    77,
      78,    -1,    -1,    -1,    -1,    83,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,    68,    69,    70,    71,    72,    73,    74,    -1,    -1,
      77,    78,    -1,    -1,    -1,    -1,    83,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   137,   138,     0,     1,     3,     4,     5,     6,     7,
      11,    12,    16,    18,    19,    20,    21,    22,    23,    24,
      29,    30,    31,    32,    33,    34,    35,    38,    44,    45,
      46,    47,    48,    49,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    62,    63,    65,    66,    67,    86,    89,
      90,    92,    93,    95,    97,    98,    99,   100,   101,   119,
     120,   121,   140,   142,   143,   146,   148,   149,   153,   154,
     156,   157,   158,   159,   160,   170,   188,   211,   221,   222,
     235,   236,   237,   238,   239,   240,   241,   244,   252,   254,
     255,   256,   257,   258,   259,   278,   287,   143,    21,    22,
      29,    30,    31,    45,    51,    55,    83,    86,    89,   119,
     161,   162,   188,   211,   256,   259,   278,   162,     3,     4,
       5,     6,     7,     8,     9,    10,    13,    14,    15,    16,
      17,    20,    21,    22,    23,    24,    25,    26,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    55,    65,    66,    68,    69,    70,    71,    73,
      74,    77,    81,    82,    83,    84,    95,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   121,   130,   131,
     163,   168,   169,   257,   273,    32,    33,    34,    35,    48,
      49,    51,    55,   163,   164,   165,   166,   252,   254,   189,
     192,   193,   134,   135,   198,   282,   285,   286,   194,   196,
     201,    86,    95,    96,   120,   153,   160,   170,   173,   177,
     184,   186,   211,   256,   258,   259,   276,   277,   177,   177,
     131,   179,   180,   131,   175,   179,   146,    53,   165,   134,
     283,   145,   123,   247,   170,   211,   170,    51,    52,    55,
     168,   279,    55,    89,   140,   155,   156,   146,    95,   153,
     174,   186,   276,   287,   186,   275,   276,   287,    86,   159,
     211,   256,   259,    53,    54,    56,   163,   247,   253,   246,
     247,   133,   242,   133,   245,    58,    59,   148,   170,   170,
      50,   141,   287,   282,   286,    39,    40,    41,    42,    43,
      36,    37,    28,   218,   106,   129,    89,    95,   157,   106,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    83,    84,   107,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,    85,   125,   127,   132,
      85,   127,    26,   123,   223,   238,    87,    87,   175,   179,
     223,   283,   146,    51,    55,   161,    58,    59,     1,   110,
     260,   285,    85,   125,   127,   207,   274,   208,    85,   127,
     281,   129,   139,   140,   146,   147,   147,   147,   199,   200,
     135,   147,   147,    86,   154,   160,   211,   216,   256,   259,
      55,   173,   173,   106,    88,   129,   185,   287,    85,   125,
     127,    87,    87,   129,   185,   128,   177,    90,   177,   181,
     153,   177,   186,   221,   287,    53,    62,    63,   144,   131,
     171,   123,   140,    60,   102,   103,   104,   248,    85,   127,
      87,   155,   128,   128,   190,   170,   283,   126,   129,   134,
     284,   129,   284,   129,   284,   124,   284,    55,    85,   125,
     127,   104,   104,   248,    64,   104,   104,   243,   248,   104,
      60,   104,    68,    68,    61,   143,   147,   147,   147,   147,
     143,   146,   146,   219,    95,   148,   173,   186,   187,   155,
     159,   129,   148,   170,   173,   187,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   172,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,    51,    52,    55,   168,   279,   280,   174,    51,
      55,   279,   279,   279,   279,   226,   224,   148,   170,   148,
     170,    94,   150,   205,   285,   261,   204,    51,    55,   161,
     279,   174,   279,   139,   146,    51,    53,    54,    55,    56,
      95,    96,   113,   116,   131,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   209,   167,    10,     8,   231,
     287,    13,   108,   212,   285,   212,   282,    17,   228,    15,
      27,   108,   213,   285,   213,    55,    85,   125,   127,    25,
     129,   185,   170,   173,    95,   173,   184,   276,    51,    55,
     174,    51,    55,    95,   173,   277,   128,    95,   170,   173,
     178,   182,   184,   276,   283,   129,    85,   127,   283,   128,
     165,   191,   170,   140,   124,   250,   249,   170,   128,   283,
     283,    95,   173,    51,    55,   174,    51,    55,   133,   248,
     133,   170,   170,    76,   111,   217,   287,   173,   129,   128,
      43,   170,    87,    87,   175,   179,   126,    87,    87,   175,
     176,   179,   287,   176,   179,   176,   179,   217,   217,   151,
     285,   147,   139,   126,    10,   283,   106,   263,   139,   285,
     129,   272,   287,   129,   272,    51,   129,   272,    51,   163,
     165,   173,   187,   232,   287,    15,   215,   287,   140,    13,
     140,   228,    95,   186,   229,    10,   140,   195,   197,    51,
      55,   174,    51,    55,   202,   173,   129,   185,    87,   126,
      87,    87,   173,   173,   129,   183,   128,   129,   185,   128,
     153,   221,   279,   279,   128,   146,   124,   140,    53,    54,
      56,   251,   259,   128,   170,   126,   111,   216,   220,    95,
     173,   170,   108,   148,   170,   170,   150,    87,   148,   170,
     148,   170,   150,   227,   225,   217,   206,   285,    10,   128,
     173,   283,    10,   264,   267,   269,   271,    51,   266,   269,
     210,    88,   233,   287,   140,     9,   234,   287,    14,   214,
     215,   215,    10,   173,   129,   212,    10,   140,   140,   126,
     147,   185,    95,    87,   185,   185,    95,   184,   186,   276,
     128,    95,   283,   128,   124,   283,   111,   140,   173,   170,
     148,   170,   140,   140,   152,   139,   128,   129,   272,   272,
     272,   262,    86,   160,   211,   256,   259,   212,   140,   147,
      10,    10,    95,   140,    10,    10,   213,   170,   173,   129,
     185,   129,   185,   173,   128,    10,    10,   124,   140,    10,
     269,   139,    55,    85,   125,   127,   140,   212,   173,   215,
     228,   230,   203,   185,   185,    95,   276,    95,   185,   124,
     272,    10,    51,    55,   174,    51,    55,   231,   140,   140,
     173,   129,   185,   173,   126,   214,    10,   185,    95,   185,
     173,   185
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
      yyerror (YY_("syntax error: cannot back up")); \
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
		  Type, Value); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
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
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

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
int yyparse (void);
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
yyparse (void)
#else
int
yyparse ()

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
#line 433 "grammar.y"
    {
                        vps->lex_state = EXPR_BEG;
                        vps->variables = new LocalState(0);
                        class_nest = 0;
                    ;}
    break;

  case 3:
#line 439 "grammar.y"
    {
                        if ((yyvsp[(2) - (3)].node) && !compile_for_eval) {
                            /* last expression should not be void */
                            if (nd_type((yyvsp[(2) - (3)].node)) != NODE_BLOCK) void_expr((yyvsp[(2) - (3)].node));
                            else {
                                NODE *node = (yyvsp[(2) - (3)].node);
                                while (node->nd_next) {
                                    node = node->nd_next;
                                }
                                void_expr(node->nd_head);
                            }
                        }
                        vps->top = block_append(vps, vps->top, (yyvsp[(2) - (3)].node));
                        if ((yyvsp[(3) - (3)].node))
                            vps->top = block_append(vps, (yyvsp[(3) - (3)].node), vps->top);
                        class_nest = 0;
                    ;}
    break;

  case 4:
#line 462 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (4)].node);
                        if ((yyvsp[(2) - (4)].node)) {
                            (yyval.node) = NEW_RESCUE((yyvsp[(1) - (4)].node), (yyvsp[(2) - (4)].node), (yyvsp[(3) - (4)].node));
                        }
                        else if ((yyvsp[(3) - (4)].node)) {
                            rb_warn("else without rescue is useless");
                            (yyval.node) = block_append(vps, (yyval.node), (yyvsp[(3) - (4)].node));
                        }
                        if ((yyvsp[(4) - (4)].node)) {
                            (yyval.node) = NEW_ENSURE((yyval.node), (yyvsp[(4) - (4)].node));
                        }
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 5:
#line 479 "grammar.y"
    {
                        void_stmts((yyvsp[(1) - (2)].node), vps);
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 7:
#line 487 "grammar.y"
    {
                      (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 9:
#line 493 "grammar.y"
    {
                        (yyval.node) = newline_node(vps, (yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 10:
#line 497 "grammar.y"
    {
                        (yyval.node) = block_append(vps, (yyvsp[(1) - (3)].node), newline_node(vps, (yyvsp[(3) - (3)].node)));
                    ;}
    break;

  case 11:
#line 501 "grammar.y"
    {
                        (yyval.node) = remove_begin((yyvsp[(2) - (2)].node), vps);
                    ;}
    break;

  case 12:
#line 506 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 13:
#line 507 "grammar.y"
    {
                        (yyval.node) = NEW_ALIAS((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 14:
#line 511 "grammar.y"
    {
                        (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 15:
#line 515 "grammar.y"
    {
                        char buf[3];

                        snprintf(buf, sizeof(buf), "$%c", (char)(yyvsp[(3) - (3)].node)->nd_nth);
                        (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), rb_parser_sym(buf));
                    ;}
    break;

  case 16:
#line 522 "grammar.y"
    {
                        yyerror("can't make alias for the number variables");
                        (yyval.node) = 0;
                    ;}
    break;

  case 17:
#line 527 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 18:
#line 531 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(3) - (3)].node), vps), remove_begin((yyvsp[(1) - (3)].node), vps), 0);
                        fixpos((yyval.node), (yyvsp[(3) - (3)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            (yyval.node)->nd_else = (yyval.node)->nd_body;
                            (yyval.node)->nd_body = 0;
                        }
                    ;}
    break;

  case 19:
#line 540 "grammar.y"
    {
                        (yyval.node) = NEW_UNLESS(cond((yyvsp[(3) - (3)].node), vps), remove_begin((yyvsp[(1) - (3)].node), vps), 0);
                        fixpos((yyval.node), (yyvsp[(3) - (3)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            (yyval.node)->nd_body = (yyval.node)->nd_else;
                            (yyval.node)->nd_else = 0;
                        }
                    ;}
    break;

  case 20:
#line 549 "grammar.y"
    {
                        if ((yyvsp[(1) - (3)].node) && nd_type((yyvsp[(1) - (3)].node)) == NODE_BEGIN) {
                            (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (3)].node), vps), (yyvsp[(1) - (3)].node)->nd_body, 0);
                        }
                        else {
                            (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (3)].node), vps), (yyvsp[(1) - (3)].node), 1);
                        }
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            nd_set_type((yyval.node), NODE_UNTIL);
                        }
                    ;}
    break;

  case 21:
#line 561 "grammar.y"
    {
                        if ((yyvsp[(1) - (3)].node) && nd_type((yyvsp[(1) - (3)].node)) == NODE_BEGIN) {
                            (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (3)].node), vps), (yyvsp[(1) - (3)].node)->nd_body, 0);
                        }
                        else {
                            (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (3)].node), vps), (yyvsp[(1) - (3)].node), 1);
                        }
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            nd_set_type((yyval.node), NODE_WHILE);
                        }
                    ;}
    break;

  case 22:
#line 573 "grammar.y"
    {
                        NODE *resq = NEW_RESBODY(0, remove_begin((yyvsp[(3) - (3)].node), vps), 0);
                        (yyval.node) = NEW_RESCUE(remove_begin((yyvsp[(1) - (3)].node), vps), resq, 0);
                    ;}
    break;

  case 23:
#line 578 "grammar.y"
    {
                        if (in_def || in_single) {
                            yyerror("BEGIN in method");
                        }
                        local_push(0);
                    ;}
    break;

  case 24:
#line 585 "grammar.y"
    {
                        /*
                        ruby_eval_tree_begin = block_append(ruby_eval_tree_begin,
                                                            NEW_PREEXE($4));
                        */
                        local_pop();
                        (yyval.node) = 0;
                    ;}
    break;

  case 25:
#line 594 "grammar.y"
    {
                        if (in_def || in_single) {
                            rb_warn("END in method; use at_exit");
                        }

                        (yyval.node) = NEW_ITER(0, NEW_POSTEXE(), (yyvsp[(3) - (4)].node));
                    ;}
    break;

  case 26:
#line 602 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 27:
#line 606 "grammar.y"
    {
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyvsp[(1) - (3)].node)->nd_value = ((yyvsp[(1) - (3)].node)->nd_head) ? NEW_TO_ARY((yyvsp[(3) - (3)].node)) : NEW_ARRAY((yyvsp[(3) - (3)].node));
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 28:
#line 612 "grammar.y"
    {
                        value_expr((yyvsp[(3) - (3)].node));
                        if ((yyvsp[(1) - (3)].node)) {
                            QUID vid = (yyvsp[(1) - (3)].node)->nd_vid;
                            if ((yyvsp[(2) - (3)].id) == tOROP) {
                                (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                                (yyval.node) = NEW_OP_ASGN_OR(gettable(vid), (yyvsp[(1) - (3)].node));
                                if (is_asgn_or_id(vid)) {
                                    (yyval.node)->nd_aid = vid;
                                }
                            }
                            else if ((yyvsp[(2) - (3)].id) == tANDOP) {
                                (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                                (yyval.node) = NEW_OP_ASGN_AND(gettable(vid), (yyvsp[(1) - (3)].node));
                            }
                            else {
                                (yyval.node) = (yyvsp[(1) - (3)].node);
                                (yyval.node)->nd_value = call_op(gettable(vid),(yyvsp[(2) - (3)].id),1,(yyvsp[(3) - (3)].node), vps);
                            }
                        }
                        else {
                            (yyval.node) = 0;
                        }
                    ;}
    break;

  case 29:
#line 637 "grammar.y"
    {
                        NODE *args;

                        value_expr((yyvsp[(6) - (6)].node));
                        if (!(yyvsp[(3) - (6)].node)) (yyvsp[(3) - (6)].node) = NEW_ZARRAY();
                        args = arg_concat(vps, (yyvsp[(6) - (6)].node), (yyvsp[(3) - (6)].node));
                        if ((yyvsp[(5) - (6)].id) == tOROP) {
                            (yyvsp[(5) - (6)].id) = 0;
                        }
                        else if ((yyvsp[(5) - (6)].id) == tANDOP) {
                            (yyvsp[(5) - (6)].id) = 1;
                        }
                        (yyval.node) = NEW_OP_ASGN1((yyvsp[(1) - (6)].node), (yyvsp[(5) - (6)].id), args);
                        fixpos((yyval.node), (yyvsp[(1) - (6)].node));
                    ;}
    break;

  case 30:
#line 653 "grammar.y"
    {
                        value_expr((yyvsp[(5) - (5)].node));
                        if ((yyvsp[(4) - (5)].id) == tOROP) {
                            (yyvsp[(4) - (5)].id) = 0;
                        }
                        else if ((yyvsp[(4) - (5)].id) == tANDOP) {
                            (yyvsp[(4) - (5)].id) = 1;
                        }
                        (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                    ;}
    break;

  case 31:
#line 665 "grammar.y"
    {
                        value_expr((yyvsp[(5) - (5)].node));
                        if ((yyvsp[(4) - (5)].id) == tOROP) {
                            (yyvsp[(4) - (5)].id) = 0;
                        }
                        else if ((yyvsp[(4) - (5)].id) == tANDOP) {
                            (yyvsp[(4) - (5)].id) = 1;
                        }
                        (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                    ;}
    break;

  case 32:
#line 677 "grammar.y"
    {
                        value_expr((yyvsp[(5) - (5)].node));
                        if ((yyvsp[(4) - (5)].id) == tOROP) {
                            (yyvsp[(4) - (5)].id) = 0;
                        }
                        else if ((yyvsp[(4) - (5)].id) == tANDOP) {
                            (yyvsp[(4) - (5)].id) = 1;
                        }
                        (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                    ;}
    break;

  case 33:
#line 689 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (3)].node), vps);
                        (yyval.node) = 0;
                    ;}
    break;

  case 34:
#line 694 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), NEW_SVALUE((yyvsp[(3) - (3)].node)), vps);
                    ;}
    break;

  case 35:
#line 698 "grammar.y"
    {
                        (yyvsp[(1) - (3)].node)->nd_value = ((yyvsp[(1) - (3)].node)->nd_head) ? NEW_TO_ARY((yyvsp[(3) - (3)].node)) : NEW_ARRAY((yyvsp[(3) - (3)].node));
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 36:
#line 703 "grammar.y"
    {
                        (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 39:
#line 712 "grammar.y"
    {
                        (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 40:
#line 716 "grammar.y"
    {
                        (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 41:
#line 720 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), vps));
                    ;}
    break;

  case 42:
#line 724 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), vps));
                    ;}
    break;

  case 44:
#line 731 "grammar.y"
    {
                        value_expr((yyval.node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 47:
#line 740 "grammar.y"
    {
                        (yyval.node) = NEW_RETURN(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 48:
#line 744 "grammar.y"
    {
                        (yyval.node) = NEW_BREAK(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 49:
#line 748 "grammar.y"
    {
                        (yyval.node) = NEW_NEXT(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 51:
#line 755 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 52:
#line 759 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 53:
#line 765 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 54:
#line 769 "grammar.y"
    { (yyval.vars) = vps->variables->block_vars; ;}
    break;

  case 55:
#line 772 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 56:
#line 779 "grammar.y"
    {
                        (yyval.node) = new_fcall(vps, (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                   ;}
    break;

  case 57:
#line 784 "grammar.y"
    {
                        (yyval.node) = new_fcall(vps, (yyvsp[(1) - (3)].id), (yyvsp[(2) - (3)].node));
                        if ((yyvsp[(3) - (3)].node)) {
                            if (nd_type((yyval.node)) == NODE_BLOCK_PASS) {
                                rb_compile_error(vps, "both block arg and actual block given");
                            }
                            (yyvsp[(3) - (3)].node)->nd_iter = (yyval.node);
                            (yyval.node) = (yyvsp[(3) - (3)].node);
                        }
                        fixpos((yyval.node), (yyvsp[(2) - (3)].node));
                   ;}
    break;

  case 58:
#line 796 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 59:
#line 801 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].node));
                        if ((yyvsp[(5) - (5)].node)) {
                            if (nd_type((yyval.node)) == NODE_BLOCK_PASS) {
                                rb_compile_error(vps, "both block arg and actual block given");
                            }
                            (yyvsp[(5) - (5)].node)->nd_iter = (yyval.node);
                            (yyval.node) = (yyvsp[(5) - (5)].node);
                        }
                        fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                   ;}
    break;

  case 60:
#line 813 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 61:
#line 818 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].node));
                        if ((yyvsp[(5) - (5)].node)) {
                            if (nd_type((yyval.node)) == NODE_BLOCK_PASS) {
                                rb_compile_error(vps, "both block arg and actual block given");
                            }
                            (yyvsp[(5) - (5)].node)->nd_iter = (yyval.node);
                            (yyval.node) = (yyvsp[(5) - (5)].node);
                        }
                        fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                   ;}
    break;

  case 62:
#line 830 "grammar.y"
    {
                        (yyval.node) = new_super(vps, (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 63:
#line 835 "grammar.y"
    {
                        (yyval.node) = new_yield(vps, (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 65:
#line 843 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 67:
#line 850 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(NEW_LIST((yyvsp[(2) - (3)].node)), 0);
                    ;}
    break;

  case 68:
#line 856 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (1)].node), 0);
                    ;}
    break;

  case 69:
#line 860 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(list_append(vps, (yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node)), 0);
                    ;}
    break;

  case 70:
#line 864 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 71:
#line 868 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (2)].node), -1);
                    ;}
    break;

  case 72:
#line 872 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(0, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 73:
#line 876 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(0, -1);
                    ;}
    break;

  case 75:
#line 883 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 76:
#line 889 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 77:
#line 893 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 78:
#line 899 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, vps);
                    ;}
    break;

  case 79:
#line 903 "grammar.y"
    {
                        (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), vps);
                    ;}
    break;

  case 80:
#line 907 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 81:
#line 911 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 82:
#line 915 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 83:
#line 919 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                    ;}
    break;

  case 84:
#line 925 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                    ;}
    break;

  case 85:
#line 931 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (1)].node), vps);
                        (yyval.node) = 0;
                    ;}
    break;

  case 86:
#line 938 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, vps);
                    ;}
    break;

  case 87:
#line 942 "grammar.y"
    {
                        (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), vps);
                    ;}
    break;

  case 88:
#line 946 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 89:
#line 950 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 90:
#line 954 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 91:
#line 958 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                    ;}
    break;

  case 92:
#line 964 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                    ;}
    break;

  case 93:
#line 970 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (1)].node), vps);
                        (yyval.node) = 0;
                    ;}
    break;

  case 94:
#line 977 "grammar.y"
    {
                        yyerror("class/module name must be CONSTANT");
                    ;}
    break;

  case 96:
#line 984 "grammar.y"
    {
                        (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 97:
#line 988 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2(0, (yyval.node));
                    ;}
    break;

  case 98:
#line 992 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 102:
#line 1001 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = convert_op((yyvsp[(1) - (1)].id));
                    ;}
    break;

  case 103:
#line 1006 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = (yyvsp[(1) - (1)].id);
                    ;}
    break;

  case 106:
#line 1017 "grammar.y"
    {
                        (yyval.node) = NEW_LIT(QUID2SYM((yyvsp[(1) - (1)].id)));
                    ;}
    break;

  case 108:
#line 1024 "grammar.y"
    {
                        (yyval.node) = NEW_UNDEF((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 109:
#line 1027 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 110:
#line 1028 "grammar.y"
    {
                        (yyval.node) = block_append(vps, (yyvsp[(1) - (4)].node), NEW_UNDEF((yyvsp[(4) - (4)].node)));
                    ;}
    break;

  case 111:
#line 1033 "grammar.y"
    { (yyval.id) = '|'; ;}
    break;

  case 112:
#line 1034 "grammar.y"
    { (yyval.id) = '^'; ;}
    break;

  case 113:
#line 1035 "grammar.y"
    { (yyval.id) = '&'; ;}
    break;

  case 114:
#line 1036 "grammar.y"
    { (yyval.id) = tCMP; ;}
    break;

  case 115:
#line 1037 "grammar.y"
    { (yyval.id) = tEQ; ;}
    break;

  case 116:
#line 1038 "grammar.y"
    { (yyval.id) = tEQQ; ;}
    break;

  case 117:
#line 1039 "grammar.y"
    { (yyval.id) = tMATCH; ;}
    break;

  case 118:
#line 1040 "grammar.y"
    { (yyval.id) = '>'; ;}
    break;

  case 119:
#line 1041 "grammar.y"
    { (yyval.id) = tGEQ; ;}
    break;

  case 120:
#line 1042 "grammar.y"
    { (yyval.id) = '<'; ;}
    break;

  case 121:
#line 1043 "grammar.y"
    { (yyval.id) = tLEQ; ;}
    break;

  case 122:
#line 1044 "grammar.y"
    { (yyval.id) = tLSHFT; ;}
    break;

  case 123:
#line 1045 "grammar.y"
    { (yyval.id) = tRSHFT; ;}
    break;

  case 124:
#line 1046 "grammar.y"
    { (yyval.id) = '+'; ;}
    break;

  case 125:
#line 1047 "grammar.y"
    { (yyval.id) = '-'; ;}
    break;

  case 126:
#line 1048 "grammar.y"
    { (yyval.id) = '*'; ;}
    break;

  case 127:
#line 1049 "grammar.y"
    { (yyval.id) = '*'; ;}
    break;

  case 128:
#line 1050 "grammar.y"
    { (yyval.id) = '/'; ;}
    break;

  case 129:
#line 1051 "grammar.y"
    { (yyval.id) = '%'; ;}
    break;

  case 130:
#line 1052 "grammar.y"
    { (yyval.id) = tPOW; ;}
    break;

  case 131:
#line 1053 "grammar.y"
    { (yyval.id) = '~'; ;}
    break;

  case 132:
#line 1054 "grammar.y"
    { (yyval.id) = tUPLUS; ;}
    break;

  case 133:
#line 1055 "grammar.y"
    { (yyval.id) = tUMINUS; ;}
    break;

  case 134:
#line 1056 "grammar.y"
    { (yyval.id) = tAREF; ;}
    break;

  case 135:
#line 1057 "grammar.y"
    { (yyval.id) = tASET; ;}
    break;

  case 136:
#line 1058 "grammar.y"
    { (yyval.id) = '`'; ;}
    break;

  case 179:
#line 1071 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 180:
#line 1075 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (5)].node), NEW_RESCUE((yyvsp[(3) - (5)].node), NEW_RESBODY(0,(yyvsp[(5) - (5)].node),0), 0), vps);
                    ;}
    break;

  case 181:
#line 1079 "grammar.y"
    {
                        value_expr((yyvsp[(3) - (3)].node));
                        if ((yyvsp[(1) - (3)].node)) {
                            QUID vid = (yyvsp[(1) - (3)].node)->nd_vid;
                            if ((yyvsp[(2) - (3)].id) == tOROP) {
                                (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                                (yyval.node) = NEW_OP_ASGN_OR(gettable(vid), (yyvsp[(1) - (3)].node));
                                if (is_asgn_or_id(vid)) {
                                    (yyval.node)->nd_aid = vid;
                                }
                            }
                            else if ((yyvsp[(2) - (3)].id) == tANDOP) {
                                (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                                (yyval.node) = NEW_OP_ASGN_AND(gettable(vid), (yyvsp[(1) - (3)].node));
                            }
                            else {
                                (yyval.node) = (yyvsp[(1) - (3)].node);
                                (yyval.node)->nd_value = call_op(gettable(vid),(yyvsp[(2) - (3)].id),1,(yyvsp[(3) - (3)].node), vps);
                            }
                        }
                        else {
                            (yyval.node) = 0;
                        }
                    ;}
    break;

  case 182:
#line 1104 "grammar.y"
    {
                        NODE *args;

                        value_expr((yyvsp[(6) - (6)].node));
                        if (!(yyvsp[(3) - (6)].node)) (yyvsp[(3) - (6)].node) = NEW_ZARRAY();
                        args = arg_concat(vps, (yyvsp[(6) - (6)].node), (yyvsp[(3) - (6)].node));
                        if ((yyvsp[(5) - (6)].id) == tOROP) {
                            (yyvsp[(5) - (6)].id) = 0;
                        }
                        else if ((yyvsp[(5) - (6)].id) == tANDOP) {
                            (yyvsp[(5) - (6)].id) = 1;
                        }
                        (yyval.node) = NEW_OP_ASGN1((yyvsp[(1) - (6)].node), (yyvsp[(5) - (6)].id), args);
                        fixpos((yyval.node), (yyvsp[(1) - (6)].node));
                    ;}
    break;

  case 183:
#line 1120 "grammar.y"
    {
                        value_expr((yyvsp[(5) - (5)].node));
                        if ((yyvsp[(4) - (5)].id) == tOROP) {
                            (yyvsp[(4) - (5)].id) = 0;
                        }
                        else if ((yyvsp[(4) - (5)].id) == tANDOP) {
                            (yyvsp[(4) - (5)].id) = 1;
                        }
                        (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                    ;}
    break;

  case 184:
#line 1132 "grammar.y"
    {
                        value_expr((yyvsp[(5) - (5)].node));
                        if ((yyvsp[(4) - (5)].id) == tOROP) {
                            (yyvsp[(4) - (5)].id) = 0;
                        }
                        else if ((yyvsp[(4) - (5)].id) == tANDOP) {
                            (yyvsp[(4) - (5)].id) = 1;
                        }
                        (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                    ;}
    break;

  case 185:
#line 1144 "grammar.y"
    {
                        value_expr((yyvsp[(5) - (5)].node));
                        if ((yyvsp[(4) - (5)].id) == tOROP) {
                            (yyvsp[(4) - (5)].id) = 0;
                        }
                        else if ((yyvsp[(4) - (5)].id) == tANDOP) {
                            (yyvsp[(4) - (5)].id) = 1;
                        }
                        (yyval.node) = NEW_OP_ASGN2((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                    ;}
    break;

  case 186:
#line 1156 "grammar.y"
    {
                        yyerror("constant re-assignment");
                        (yyval.node) = 0;
                    ;}
    break;

  case 187:
#line 1161 "grammar.y"
    {
                        yyerror("constant re-assignment");
                        (yyval.node) = 0;
                    ;}
    break;

  case 188:
#line 1166 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (3)].node), vps);
                        (yyval.node) = 0;
                    ;}
    break;

  case 189:
#line 1171 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (3)].node));
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyval.node) = NEW_DOT2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 190:
#line 1177 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (3)].node));
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyval.node) = NEW_DOT3((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 191:
#line 1183 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '+', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 192:
#line 1187 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '-', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 193:
#line 1191 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '*', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 194:
#line 1195 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '/', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 195:
#line 1199 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '%', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 196:
#line 1203 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tPOW, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 197:
#line 1207 "grammar.y"
    {
                        (yyval.node) = call_op(call_op((yyvsp[(2) - (4)].node), tPOW, 1, (yyvsp[(4) - (4)].node), vps), tUMINUS, 0, 0, vps);
                    ;}
    break;

  case 198:
#line 1211 "grammar.y"
    {
                        (yyval.node) = call_op(call_op((yyvsp[(2) - (4)].node), tPOW, 1, (yyvsp[(4) - (4)].node), vps), tUMINUS, 0, 0, vps);
                    ;}
    break;

  case 199:
#line 1215 "grammar.y"
    {
                        if ((yyvsp[(2) - (2)].node) && nd_type((yyvsp[(2) - (2)].node)) == NODE_LIT) {
                            (yyval.node) = (yyvsp[(2) - (2)].node);
                        }
                        else {
                            (yyval.node) = call_op((yyvsp[(2) - (2)].node), tUPLUS, 0, 0, vps);
                        }
                    ;}
    break;

  case 200:
#line 1224 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(2) - (2)].node), tUMINUS, 0, 0, vps);
                    ;}
    break;

  case 201:
#line 1228 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '|', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 202:
#line 1232 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '^', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 203:
#line 1236 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '&', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 204:
#line 1240 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tCMP, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 205:
#line 1244 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '>', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 206:
#line 1248 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tGEQ, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 207:
#line 1252 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '<', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 208:
#line 1256 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tLEQ, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 209:
#line 1260 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tEQ, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 210:
#line 1264 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tEQQ, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 211:
#line 1268 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(call_op((yyvsp[(1) - (3)].node), tEQ, 1, (yyvsp[(3) - (3)].node), vps));
                    ;}
    break;

  case 212:
#line 1272 "grammar.y"
    {
                        (yyval.node) = match_gen((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 213:
#line 1276 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(match_gen((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps));
                    ;}
    break;

  case 214:
#line 1280 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), vps));
                    ;}
    break;

  case 215:
#line 1284 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(2) - (2)].node), '~', 0, 0, vps);
                    ;}
    break;

  case 216:
#line 1288 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tLSHFT, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 217:
#line 1292 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tRSHFT, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 218:
#line 1296 "grammar.y"
    {
                        (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 219:
#line 1300 "grammar.y"
    {
                        (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 220:
#line 1303 "grammar.y"
    {vps->in_defined = 1;;}
    break;

  case 221:
#line 1304 "grammar.y"
    {
                        vps->in_defined = 0;
                        (yyval.node) = NEW_DEFINED((yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 222:
#line 1308 "grammar.y"
    {vps->ternary_colon++;;}
    break;

  case 223:
#line 1309 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(1) - (6)].node), vps), (yyvsp[(4) - (6)].node), (yyvsp[(6) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (6)].node));
                        vps->ternary_colon--;
                    ;}
    break;

  case 224:
#line 1315 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 225:
#line 1321 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (1)].node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 227:
#line 1329 "grammar.y"
    {
                        rb_warn("parenthesize argument(s) for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 228:
#line 1334 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 229:
#line 1338 "grammar.y"
    {
                        value_expr((yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_concat(vps, (yyvsp[(1) - (5)].node), (yyvsp[(4) - (5)].node));
                    ;}
    break;

  case 230:
#line 1343 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                    ;}
    break;

  case 231:
#line 1347 "grammar.y"
    {
                        value_expr((yyvsp[(2) - (3)].node));
                        (yyval.node) = NEW_NEWLINE(NEW_SPLAT((yyvsp[(2) - (3)].node)));
                    ;}
    break;

  case 232:
#line 1354 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 233:
#line 1358 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                    ;}
    break;

  case 234:
#line 1362 "grammar.y"
    {
                        rb_warn("parenthesize argument for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(2) - (4)].node));
                    ;}
    break;

  case 235:
#line 1367 "grammar.y"
    {
                        rb_warn("parenthesize argument for future version");
                        (yyval.node) = list_append(vps, (yyvsp[(2) - (6)].node), (yyvsp[(4) - (6)].node));
                    ;}
    break;

  case 238:
#line 1378 "grammar.y"
    {
                        rb_warn("parenthesize argument(s) for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 239:
#line 1383 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 240:
#line 1387 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, (yyvsp[(1) - (5)].node), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 241:
#line 1392 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 242:
#line 1397 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, NEW_LIST(NEW_HASH((yyvsp[(1) - (5)].node))), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 243:
#line 1402 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (4)].node), NEW_HASH((yyvsp[(3) - (4)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 244:
#line 1407 "grammar.y"
    {
                        value_expr((yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_concat(vps, list_append(vps, (yyvsp[(1) - (7)].node), NEW_HASH((yyvsp[(3) - (7)].node))), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 245:
#line 1413 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(NEW_SPLAT((yyvsp[(2) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 247:
#line 1420 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(list_concat(NEW_LIST((yyvsp[(1) - (4)].node)),(yyvsp[(3) - (4)].node)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 248:
#line 1424 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 249:
#line 1428 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, NEW_LIST((yyvsp[(1) - (5)].node)), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 250:
#line 1433 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, list_concat(NEW_LIST((yyvsp[(1) - (7)].node)),(yyvsp[(3) - (7)].node)), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 251:
#line 1438 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 252:
#line 1443 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, NEW_LIST(NEW_HASH((yyvsp[(1) - (5)].node))), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 253:
#line 1448 "grammar.y"
    {
                        (yyval.node) = list_append(vps, NEW_LIST((yyvsp[(1) - (4)].node)), NEW_HASH((yyvsp[(3) - (4)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 254:
#line 1453 "grammar.y"
    {
                        (yyval.node) = list_append(vps, list_concat(NEW_LIST((yyvsp[(1) - (6)].node)),(yyvsp[(3) - (6)].node)), NEW_HASH((yyvsp[(5) - (6)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(6) - (6)].node));
                    ;}
    break;

  case 255:
#line 1458 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, list_append(vps, NEW_LIST((yyvsp[(1) - (7)].node)), NEW_HASH((yyvsp[(3) - (7)].node))), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 256:
#line 1463 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, list_append(vps,
                                        list_concat(NEW_LIST((yyvsp[(1) - (9)].node)), (yyvsp[(3) - (9)].node)), NEW_HASH((yyvsp[(5) - (9)].node))), (yyvsp[(8) - (9)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(9) - (9)].node));
                    ;}
    break;

  case 257:
#line 1469 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(NEW_SPLAT((yyvsp[(2) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 259:
#line 1475 "grammar.y"
    {
                        (yyval.num) = vps->cmdarg_stack;
                        CMDARG_PUSH(1);
                    ;}
    break;

  case 260:
#line 1480 "grammar.y"
    {
                        /* CMDARG_POP() */
                        vps->cmdarg_stack = (yyvsp[(1) - (2)].num);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 262:
#line 1488 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 263:
#line 1489 "grammar.y"
    {
                        rb_warn("don't put space before argument parentheses");
                        (yyval.node) = 0;
                    ;}
    break;

  case 264:
#line 1493 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 265:
#line 1494 "grammar.y"
    {
                        rb_warn("don't put space before argument parentheses");
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                    ;}
    break;

  case 266:
#line 1501 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK_PASS((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 267:
#line 1507 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 269:
#line 1514 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 270:
#line 1518 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 271:
#line 1524 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 272:
#line 1528 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, (yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 273:
#line 1532 "grammar.y"
    {
                        (yyval.node) = NEW_SPLAT((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 282:
#line 1546 "grammar.y"
    {
                        (yyval.node) = NEW_FCALL((yyvsp[(1) - (1)].id), 0);
                    ;}
    break;

  case 283:
#line 1550 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        PUSH_LINE("begin");
                    ;}
    break;

  case 284:
#line 1556 "grammar.y"
    {
                        POP_LINE();
                        if ((yyvsp[(3) - (4)].node) == NULL)
                            (yyval.node) = NEW_NIL();
                        else
                            (yyval.node) = NEW_BEGIN((yyvsp[(3) - (4)].node));
                        nd_set_line((yyval.node), (yyvsp[(1) - (4)].num));
                    ;}
    break;

  case 285:
#line 1564 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 286:
#line 1565 "grammar.y"
    {
                        rb_warning("(...) interpreted as grouped expression");
                        (yyval.node) = (yyvsp[(2) - (5)].node);
                    ;}
    break;

  case 287:
#line 1570 "grammar.y"
    {
                        if (!(yyvsp[(2) - (3)].node)) (yyval.node) = NEW_NIL();
                        else (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 288:
#line 1575 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 289:
#line 1579 "grammar.y"
    {
                        (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 290:
#line 1583 "grammar.y"
    {
                        if ((yyvsp[(1) - (4)].node) && nd_type((yyvsp[(1) - (4)].node)) == NODE_SELF) {
                            (yyval.node) = NEW_FCALL(convert_op(tAREF), (yyvsp[(3) - (4)].node));
                        } else {
                            (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), convert_op(tAREF), (yyvsp[(3) - (4)].node));
                        }
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 291:
#line 1592 "grammar.y"
    {
                        if ((yyvsp[(2) - (3)].node) == 0) {
                            (yyval.node) = NEW_ZARRAY(); /* zero length array*/
                        }
                        else {
                            (yyval.node) = (yyvsp[(2) - (3)].node);
                        }
                    ;}
    break;

  case 292:
#line 1601 "grammar.y"
    {
                        (yyval.node) = NEW_HASH((yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 293:
#line 1605 "grammar.y"
    {
                        (yyval.node) = NEW_RETURN(0);
                    ;}
    break;

  case 294:
#line 1609 "grammar.y"
    {
                        (yyval.node) = new_yield(vps, (yyvsp[(3) - (4)].node));
                    ;}
    break;

  case 295:
#line 1613 "grammar.y"
    {
                        (yyval.node) = NEW_YIELD(0, Qfalse);
                    ;}
    break;

  case 296:
#line 1617 "grammar.y"
    {
                        (yyval.node) = NEW_YIELD(0, Qfalse);
                    ;}
    break;

  case 297:
#line 1620 "grammar.y"
    {vps->in_defined = 1;;}
    break;

  case 298:
#line 1621 "grammar.y"
    {
                        vps->in_defined = 0;
                        (yyval.node) = NEW_DEFINED((yyvsp[(5) - (6)].node));
                    ;}
    break;

  case 299:
#line 1626 "grammar.y"
    {
                        (yyvsp[(2) - (2)].node)->nd_iter = NEW_FCALL((yyvsp[(1) - (2)].id), 0);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                        fixpos((yyvsp[(2) - (2)].node)->nd_iter, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 301:
#line 1633 "grammar.y"
    {
                        if ((yyvsp[(1) - (2)].node) && nd_type((yyvsp[(1) - (2)].node)) == NODE_BLOCK_PASS) {
                            rb_compile_error(vps, "both block arg and actual block given");
                        }
                        (yyvsp[(2) - (2)].node)->nd_iter = (yyvsp[(1) - (2)].node);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                        fixpos((yyval.node), (yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 302:
#line 1641 "grammar.y"
    {
                    PUSH_LINE("if");
                  ;}
    break;

  case 303:
#line 1647 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_IF(cond((yyvsp[(3) - (7)].node), vps), (yyvsp[(5) - (7)].node), (yyvsp[(6) - (7)].node));
                        fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            NODE *tmp = (yyval.node)->nd_body;
                            (yyval.node)->nd_body = (yyval.node)->nd_else;
                            (yyval.node)->nd_else = tmp;
                        }
                    ;}
    break;

  case 304:
#line 1657 "grammar.y"
    {
                    PUSH_LINE("unless");
                  ;}
    break;

  case 305:
#line 1663 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_UNLESS(cond((yyvsp[(3) - (7)].node), vps), (yyvsp[(5) - (7)].node), (yyvsp[(6) - (7)].node));
                        fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            NODE *tmp = (yyval.node)->nd_body;
                            (yyval.node)->nd_body = (yyval.node)->nd_else;
                            (yyval.node)->nd_else = tmp;
                        }
                    ;}
    break;

  case 306:
#line 1673 "grammar.y"
    {
                    PUSH_LINE("while");
                    COND_PUSH(1);
                  ;}
    break;

  case 307:
#line 1676 "grammar.y"
    {COND_POP();;}
    break;

  case 308:
#line 1679 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (7)].node), vps), (yyvsp[(6) - (7)].node), 1);
                        fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            nd_set_type((yyval.node), NODE_UNTIL);
                        }
                    ;}
    break;

  case 309:
#line 1687 "grammar.y"
    {
                    PUSH_LINE("until");
                    COND_PUSH(1);
                  ;}
    break;

  case 310:
#line 1690 "grammar.y"
    {COND_POP();;}
    break;

  case 311:
#line 1693 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (7)].node), vps), (yyvsp[(6) - (7)].node), 1);
                        fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            nd_set_type((yyval.node), NODE_WHILE);
                        }
                    ;}
    break;

  case 312:
#line 1701 "grammar.y"
    {
                    PUSH_LINE("case");
                  ;}
    break;

  case 313:
#line 1706 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_CASE((yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(3) - (6)].node));
                    ;}
    break;

  case 314:
#line 1711 "grammar.y"
    { 
                    push_start_line((rb_parse_state*)parse_state, ruby_sourceline - 1, "case");
                  ;}
    break;

  case 315:
#line 1714 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = (yyvsp[(4) - (5)].node);
                    ;}
    break;

  case 316:
#line 1718 "grammar.y"
    { 
                    push_start_line((rb_parse_state*)parse_state, ruby_sourceline - 1, "case");
                  ;}
    break;

  case 317:
#line 1721 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = (yyvsp[(5) - (6)].node);
                    ;}
    break;

  case 318:
#line 1725 "grammar.y"
    {
                    PUSH_LINE("for");
                  ;}
    break;

  case 319:
#line 1727 "grammar.y"
    {COND_PUSH(1);;}
    break;

  case 320:
#line 1727 "grammar.y"
    {COND_POP();;}
    break;

  case 321:
#line 1730 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_FOR((yyvsp[(3) - (10)].node), (yyvsp[(6) - (10)].node), (yyvsp[(9) - (10)].node));
                        fixpos((yyval.node), (yyvsp[(3) - (10)].node));
                    ;}
    break;

  case 322:
#line 1736 "grammar.y"
    {
                        PUSH_LINE("class");
                        if (in_def || in_single)
                            yyerror("class definition in method body");
                        class_nest++;
                        local_push(0);
                        (yyval.num) = ruby_sourceline;
                    ;}
    break;

  case 323:
#line 1746 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_CLASS((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(3) - (6)].node));
                        nd_set_line((yyval.node), (yyvsp[(4) - (6)].num));
                        local_pop();
                        class_nest--;
                    ;}
    break;

  case 324:
#line 1754 "grammar.y"
    {
                        PUSH_LINE("class");
                        (yyval.num) = in_def;
                        in_def = 0;
                    ;}
    break;

  case 325:
#line 1760 "grammar.y"
    {
                        (yyval.num) = in_single;
                        in_single = 0;
                        class_nest++;
                        local_push(0);
                    ;}
    break;

  case 326:
#line 1768 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_SCLASS((yyvsp[(3) - (8)].node), (yyvsp[(7) - (8)].node));
                        fixpos((yyval.node), (yyvsp[(3) - (8)].node));
                        local_pop();
                        class_nest--;
                        in_def = (yyvsp[(4) - (8)].num);
                        in_single = (yyvsp[(6) - (8)].num);
                    ;}
    break;

  case 327:
#line 1778 "grammar.y"
    {
                        PUSH_LINE("module");
                        if (in_def || in_single)
                            yyerror("module definition in method body");
                        class_nest++;
                        local_push(0);
                        (yyval.num) = ruby_sourceline;
                    ;}
    break;

  case 328:
#line 1788 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_MODULE((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
                        nd_set_line((yyval.node), (yyvsp[(3) - (5)].num));
                        local_pop();
                        class_nest--;
                    ;}
    break;

  case 329:
#line 1796 "grammar.y"
    {
                        PUSH_LINE("def");
                        (yyval.id) = cur_mid;
                        cur_mid = (yyvsp[(2) - (2)].id);
                        in_def++;
                        local_push(0);
                    ;}
    break;

  case 330:
#line 1806 "grammar.y"
    {
                        POP_LINE();
                        if (!(yyvsp[(5) - (6)].node)) (yyvsp[(5) - (6)].node) = NEW_NIL();
                        (yyval.node) = NEW_DEFN((yyvsp[(2) - (6)].id), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node), NOEX_PRIVATE);
                        fixpos((yyval.node), (yyvsp[(4) - (6)].node));
                        local_pop();
                        in_def--;
                        cur_mid = (yyvsp[(3) - (6)].id);
                    ;}
    break;

  case 331:
#line 1815 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 332:
#line 1816 "grammar.y"
    {
                        PUSH_LINE("def");
                        in_single++;
                        local_push(0);
                        vps->lex_state = EXPR_END; /* force for args */
                    ;}
    break;

  case 333:
#line 1825 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_DEFS((yyvsp[(2) - (9)].node), (yyvsp[(5) - (9)].id), (yyvsp[(7) - (9)].node), (yyvsp[(8) - (9)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (9)].node));
                        local_pop();
                        in_single--;
                    ;}
    break;

  case 334:
#line 1833 "grammar.y"
    {
                        (yyval.node) = NEW_BREAK(0);
                    ;}
    break;

  case 335:
#line 1837 "grammar.y"
    {
                        (yyval.node) = NEW_NEXT(0);
                    ;}
    break;

  case 336:
#line 1841 "grammar.y"
    {
                        (yyval.node) = NEW_REDO();
                    ;}
    break;

  case 337:
#line 1845 "grammar.y"
    {
                        (yyval.node) = NEW_RETRY();
                    ;}
    break;

  case 338:
#line 1851 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (1)].node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 347:
#line 1872 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(2) - (5)].node), vps), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (5)].node));
                    ;}
    break;

  case 349:
#line 1880 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 353:
#line 1891 "grammar.y"
    {
                        (yyval.node) = (NODE*)1;
                    ;}
    break;

  case 354:
#line 1895 "grammar.y"
    {
                        (yyval.node) = (NODE*)1;
                    ;}
    break;

  case 355:
#line 1899 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 356:
#line 1905 "grammar.y"
    {
                        PUSH_LINE("do");
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 357:
#line 1911 "grammar.y"
    {
                      (yyval.vars) = vps->variables->block_vars;
                    ;}
    break;

  case 358:
#line 1916 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 359:
#line 1924 "grammar.y"
    {
                        if ((yyvsp[(1) - (2)].node) && nd_type((yyvsp[(1) - (2)].node)) == NODE_BLOCK_PASS) {
                            rb_compile_error(vps, "both block arg and actual block given");
                        }
                        (yyvsp[(2) - (2)].node)->nd_iter = (yyvsp[(1) - (2)].node);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                        fixpos((yyval.node), (yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 360:
#line 1933 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 361:
#line 1937 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 362:
#line 1943 "grammar.y"
    {
                        (yyval.node) = new_fcall(vps, (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 363:
#line 1948 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 364:
#line 1953 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 365:
#line 1958 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), 0);
                    ;}
    break;

  case 366:
#line 1962 "grammar.y"
    {
                        (yyval.node) = NEW_CALL((yyvsp[(1) - (3)].node), rb_parser_sym("get_reference"),
                        NEW_LIST(NEW_LIT(QUID2SYM((yyvsp[(3) - (3)].id)))));
                    ;}
    break;

  case 367:
#line 1967 "grammar.y"
    {
                        (yyval.node) = NEW_FCALL(rb_parser_sym("get_reference"),
                        NEW_LIST(NEW_LIT(QUID2SYM((yyvsp[(2) - (2)].id)))));
                    ;}
    break;

  case 368:
#line 1972 "grammar.y"
    {
                        (yyval.node) = new_super(vps, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 369:
#line 1976 "grammar.y"
    {
                        (yyval.node) = NEW_ZSUPER();
                    ;}
    break;

  case 370:
#line 1982 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 371:
#line 1986 "grammar.y"
    { (yyval.vars) = vps->variables->block_vars; ;}
    break;

  case 372:
#line 1988 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 373:
#line 1993 "grammar.y"
    {
                        PUSH_LINE("do");
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 374:
#line 1998 "grammar.y"
    { (yyval.vars) = vps->variables->block_vars; ;}
    break;

  case 375:
#line 2000 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 376:
#line 2010 "grammar.y"
    {
                        (yyval.node) = NEW_WHEN((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 378:
#line 2016 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (4)].node), NEW_WHEN((yyvsp[(4) - (4)].node), 0, 0));
                    ;}
    break;

  case 379:
#line 2020 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_WHEN((yyvsp[(2) - (2)].node), 0, 0));
                    ;}
    break;

  case 382:
#line 2032 "grammar.y"
    {
                        if ((yyvsp[(3) - (6)].node)) {
                            (yyvsp[(3) - (6)].node) = node_assign((yyvsp[(3) - (6)].node), NEW_GVAR(rb_parser_sym("$!")), vps);
                            (yyvsp[(5) - (6)].node) = block_append(vps, (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node));
                        }
                        (yyval.node) = NEW_RESBODY((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (6)].node)?(yyvsp[(2) - (6)].node):(yyvsp[(5) - (6)].node));
                    ;}
    break;

  case 384:
#line 2044 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 387:
#line 2052 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 389:
#line 2059 "grammar.y"
    {
                        if ((yyvsp[(2) - (2)].node))
                            (yyval.node) = (yyvsp[(2) - (2)].node);
                        else
                            /* place holder */
                            (yyval.node) = NEW_NIL();
                    ;}
    break;

  case 392:
#line 2071 "grammar.y"
    {
                        (yyval.node) = NEW_LIT(QUID2SYM((yyvsp[(1) - (1)].id)));
                    ;}
    break;

  case 394:
#line 2078 "grammar.y"
    {
                        NODE *node = (yyvsp[(1) - (1)].node);
                        if (!node) {
                            node = NEW_STR(string_new(0, 0));
                        }
                        else {
                            node = evstr2dstr(vps, node);
                        }
                        (yyval.node) = node;
                    ;}
    break;

  case 396:
#line 2092 "grammar.y"
    {
                        (yyval.node) = literal_concat(vps, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 397:
#line 2098 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 398:
#line 2104 "grammar.y"
    {
                        NODE *node = (yyvsp[(2) - (3)].node);
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
                        (yyval.node) = node;
                    ;}
    break;

  case 399:
#line 2127 "grammar.y"
    {
                        intptr_t options = (yyvsp[(3) - (3)].num);
                        NODE *node = (yyvsp[(2) - (3)].node);
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
                        (yyval.node) = node;
                    ;}
    break;

  case 400:
#line 2157 "grammar.y"
    {
                        (yyval.node) = NEW_ZARRAY();
                    ;}
    break;

  case 401:
#line 2161 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 402:
#line 2167 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 403:
#line 2171 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), evstr2dstr(vps, (yyvsp[(2) - (3)].node)));
                    ;}
    break;

  case 405:
#line 2178 "grammar.y"
    {
                        (yyval.node) = literal_concat(vps, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 406:
#line 2184 "grammar.y"
    {
                        (yyval.node) = NEW_ZARRAY();
                    ;}
    break;

  case 407:
#line 2188 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 408:
#line 2194 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 409:
#line 2198 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 410:
#line 2204 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 411:
#line 2208 "grammar.y"
    {
                        (yyval.node) = literal_concat(vps, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 412:
#line 2214 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 413:
#line 2218 "grammar.y"
    {
                        (yyval.node) = literal_concat(vps, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 415:
#line 2225 "grammar.y"
    {
                        (yyval.node) = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                    ;}
    break;

  case 416:
#line 2231 "grammar.y"
    {
                        lex_strterm = (yyvsp[(2) - (3)].node);
                        (yyval.node) = NEW_EVSTR((yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 417:
#line 2236 "grammar.y"
    {
                        (yyval.node) = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                        COND_PUSH(0);
                        CMDARG_PUSH(0);
                    ;}
    break;

  case 418:
#line 2244 "grammar.y"
    {
                        lex_strterm = (yyvsp[(2) - (4)].node);
                        COND_LEXPOP();
                        CMDARG_LEXPOP();
                        if (((yyval.node) = (yyvsp[(3) - (4)].node)) && nd_type((yyval.node)) == NODE_NEWLINE) {
                            (yyval.node) = (yyval.node)->nd_next;
                        }
                        (yyval.node) = new_evstr(vps, (yyval.node));
                    ;}
    break;

  case 419:
#line 2255 "grammar.y"
    {(yyval.node) = NEW_GVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 420:
#line 2256 "grammar.y"
    {(yyval.node) = NEW_IVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 421:
#line 2257 "grammar.y"
    {(yyval.node) = NEW_CVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 423:
#line 2262 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = (yyvsp[(2) - (2)].id);
                    ;}
    break;

  case 428:
#line 2275 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        if (!((yyval.node) = (yyvsp[(2) - (3)].node))) {
                            yyerror("empty symbol literal");
                        }
                        else {
                            switch (nd_type((yyval.node))) {
                              case NODE_DSTR:
                                nd_set_type((yyval.node), NODE_DSYM);
                                break;
                              case NODE_STR:
                                /* TODO: this line should never fail unless nd_str is binary */
                                if (strlen(bdatae((yyval.node)->nd_str,"")) == (size_t)blength((yyval.node)->nd_str)) {
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
                                (yyval.node) = NEW_NODE(NODE_DSYM, string_new(0, 0), 1, NEW_LIST((yyval.node)));
                                break;
                            }
                        }
                    ;}
    break;

  case 431:
#line 2308 "grammar.y"
    {
                        (yyval.node) = NEW_NEGATE((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 432:
#line 2312 "grammar.y"
    {
                        (yyval.node) = NEW_NEGATE((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 438:
#line 2322 "grammar.y"
    {(yyval.id) = kNIL;;}
    break;

  case 439:
#line 2323 "grammar.y"
    {(yyval.id) = kSELF;;}
    break;

  case 440:
#line 2324 "grammar.y"
    {(yyval.id) = kTRUE;;}
    break;

  case 441:
#line 2325 "grammar.y"
    {(yyval.id) = kFALSE;;}
    break;

  case 442:
#line 2326 "grammar.y"
    {(yyval.id) = k__FILE__;;}
    break;

  case 443:
#line 2327 "grammar.y"
    {(yyval.id) = k__LINE__;;}
    break;

  case 444:
#line 2331 "grammar.y"
    {
                        (yyval.node) = gettable((yyvsp[(1) - (1)].id));
                    ;}
    break;

  case 445:
#line 2337 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, vps);
                    ;}
    break;

  case 448:
#line 2347 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 449:
#line 2351 "grammar.y"
    {
                        vps->lex_state = EXPR_BEG;
                    ;}
    break;

  case 450:
#line 2355 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(3) - (4)].node);
                    ;}
    break;

  case 451:
#line 2358 "grammar.y"
    {yyerrok; (yyval.node) = 0;;}
    break;

  case 452:
#line 2362 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                        vps->lex_state = EXPR_BEG;
                        command_start = TRUE;
                    ;}
    break;

  case 453:
#line 2368 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 454:
#line 2374 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS((intptr_t)(yyvsp[(1) - (6)].num), (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].id)), (yyvsp[(6) - (6)].node));
                    ;}
    break;

  case 455:
#line 2378 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS((intptr_t)(yyvsp[(1) - (4)].num), (yyvsp[(3) - (4)].node), 0), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 456:
#line 2382 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS((intptr_t)(yyvsp[(1) - (4)].num), 0, (yyvsp[(3) - (4)].id)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 457:
#line 2386 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS((intptr_t)(yyvsp[(1) - (2)].num), 0, 0), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 458:
#line 2390 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS(0, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 459:
#line 2394 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS(0, (yyvsp[(1) - (2)].node), 0), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 460:
#line 2398 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS(0, 0, (yyvsp[(1) - (2)].id)), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 461:
#line 2402 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS(0, 0, 0), (yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 462:
#line 2406 "grammar.y"
    {
                        (yyval.node) = NEW_ARGS(0, 0, 0);
                    ;}
    break;

  case 463:
#line 2412 "grammar.y"
    {
                        yyerror("formal argument cannot be a constant");
                    ;}
    break;

  case 464:
#line 2416 "grammar.y"
    {
                        yyerror("formal argument cannot be an instance variable");
                    ;}
    break;

  case 465:
#line 2420 "grammar.y"
    {
                        yyerror("formal argument cannot be a global variable");
                    ;}
    break;

  case 466:
#line 2424 "grammar.y"
    {
                        yyerror("formal argument cannot be a class variable");
                    ;}
    break;

  case 467:
#line 2428 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(1) - (1)].id)))
                            yyerror("formal argument must be local variable");
                        else if (local_id((yyvsp[(1) - (1)].id)))
                            yyerror("duplicate argument name");
                        local_cnt((yyvsp[(1) - (1)].id));
                        (yyval.num) = 1;
                    ;}
    break;

  case 469:
#line 2440 "grammar.y"
    {
                        (yyval.num) += 1;
                    ;}
    break;

  case 470:
#line 2446 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(1) - (3)].id)))
                            yyerror("formal argument must be local variable");
                        else if (local_id((yyvsp[(1) - (3)].id)))
                            yyerror("duplicate optional argument name");
                        (yyval.node) = assignable((yyvsp[(1) - (3)].id), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 471:
#line 2456 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK((yyvsp[(1) - (1)].node));
                        (yyval.node)->nd_end = (yyval.node);
                    ;}
    break;

  case 472:
#line 2461 "grammar.y"
    {
                        (yyval.node) = block_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 475:
#line 2471 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(2) - (2)].id)))
                            yyerror("rest argument must be local variable");
                        else if (local_id((yyvsp[(2) - (2)].id)))
                            yyerror("duplicate rest argument name");
                        (yyval.id) = local_cnt((yyvsp[(2) - (2)].id)) + 1;
                    ;}
    break;

  case 476:
#line 2479 "grammar.y"
    {
                        (yyval.id) = -2;
                    ;}
    break;

  case 479:
#line 2489 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(2) - (2)].id)))
                            yyerror("block argument must be local variable");
                        else if (local_id((yyvsp[(2) - (2)].id)))
                            yyerror("duplicate block argument name");
                        (yyval.node) = NEW_BLOCK_ARG((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 480:
#line 2499 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 482:
#line 2506 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                        value_expr((yyval.node));
                    ;}
    break;

  case 483:
#line 2510 "grammar.y"
    {vps->lex_state = EXPR_BEG;;}
    break;

  case 484:
#line 2511 "grammar.y"
    {
                        if ((yyvsp[(3) - (5)].node) == 0) {
                            yyerror("can't define singleton method for ().");
                        }
                        else {
                            switch (nd_type((yyvsp[(3) - (5)].node))) {
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
                                value_expr((yyvsp[(3) - (5)].node));
                                break;
                            }
                        }
                        (yyval.node) = (yyvsp[(3) - (5)].node);
                    ;}
    break;

  case 486:
#line 2537 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 487:
#line 2541 "grammar.y"
    {
                        if ((yyvsp[(1) - (2)].node)->nd_alen%2 != 0) {
                            yyerror("odd number list for Hash");
                        }
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 489:
#line 2551 "grammar.y"
    {
                        (yyval.node) = list_concat((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 490:
#line 2557 "grammar.y"
    {
                        (yyval.node) = list_append(vps, NEW_LIST((yyvsp[(1) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 510:
#line 2595 "grammar.y"
    {yyerrok;;}
    break;

  case 513:
#line 2600 "grammar.y"
    {yyerrok;;}
    break;

  case 514:
#line 2603 "grammar.y"
    {(yyval.node) = 0;;}
    break;


/* Line 1267 of yacc.c.  */
#line 7483 "grammar.cpp"
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
      yyerror (YY_("syntax error"));
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
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
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
		      yytoken, &yylval);
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
		  yystos[yystate], yyvsp);
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
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
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


#line 2605 "grammar.y"


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

    if (parse_state->end_seen) {  /* After __END__ */
      newtok(parse_state);
      while ((c = nextc()) != -1) {
        tokadd(c, parse_state);
      }
      tokfix();
      pslval->node = NEW_END_DATA(string_new(tok(), toklen()));
      parse_state->end_seen = 0;
      return tEND_DATA;
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
            parse_state->lex_lastline = 0;
            parse_state->end_seen = 1;
            parse_state->lex_p = parse_state->lex_pend;
            return k__END__;
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


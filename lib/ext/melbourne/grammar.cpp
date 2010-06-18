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
     tIDENTIFIER = 305,
     tFID = 306,
     tGVAR = 307,
     tIVAR = 308,
     tCONSTANT = 309,
     tCVAR = 310,
     tXSTRING_BEG = 311,
     tINTEGER = 312,
     tFLOAT = 313,
     tSTRING_CONTENT = 314,
     tNTH_REF = 315,
     tBACK_REF = 316,
     tREGEXP_END = 317,
     tUPLUS = 318,
     tUMINUS = 319,
     tUBS = 320,
     tPOW = 321,
     tCMP = 322,
     tEQ = 323,
     tEQQ = 324,
     tNEQ = 325,
     tGEQ = 326,
     tLEQ = 327,
     tANDOP = 328,
     tOROP = 329,
     tMATCH = 330,
     tNMATCH = 331,
     tDOT2 = 332,
     tDOT3 = 333,
     tAREF = 334,
     tASET = 335,
     tLSHFT = 336,
     tRSHFT = 337,
     tCOLON2 = 338,
     tCOLON3 = 339,
     tOP_ASGN = 340,
     tASSOC = 341,
     tLPAREN = 342,
     tLPAREN_ARG = 343,
     tRPAREN = 344,
     tLBRACK = 345,
     tLBRACE = 346,
     tLBRACE_ARG = 347,
     tSTAR = 348,
     tAMPER = 349,
     tSYMBEG = 350,
     tSTRING_BEG = 351,
     tREGEXP_BEG = 352,
     tWORDS_BEG = 353,
     tQWORDS_BEG = 354,
     tSTRING_DBEG = 355,
     tSTRING_DVAR = 356,
     tSTRING_END = 357,
     tLOWEST = 358,
     tUMINUS_NUM = 359,
     tLAST_TOKEN = 360
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
#define tIDENTIFIER 305
#define tFID 306
#define tGVAR 307
#define tIVAR 308
#define tCONSTANT 309
#define tCVAR 310
#define tXSTRING_BEG 311
#define tINTEGER 312
#define tFLOAT 313
#define tSTRING_CONTENT 314
#define tNTH_REF 315
#define tBACK_REF 316
#define tREGEXP_END 317
#define tUPLUS 318
#define tUMINUS 319
#define tUBS 320
#define tPOW 321
#define tCMP 322
#define tEQ 323
#define tEQQ 324
#define tNEQ 325
#define tGEQ 326
#define tLEQ 327
#define tANDOP 328
#define tOROP 329
#define tMATCH 330
#define tNMATCH 331
#define tDOT2 332
#define tDOT3 333
#define tAREF 334
#define tASET 335
#define tLSHFT 336
#define tRSHFT 337
#define tCOLON2 338
#define tCOLON3 339
#define tOP_ASGN 340
#define tASSOC 341
#define tLPAREN 342
#define tLPAREN_ARG 343
#define tRPAREN 344
#define tLBRACK 345
#define tLBRACE 346
#define tLBRACE_ARG 347
#define tSTAR 348
#define tAMPER 349
#define tSYMBEG 350
#define tSTRING_BEG 351
#define tREGEXP_BEG 352
#define tWORDS_BEG 353
#define tQWORDS_BEG 354
#define tSTRING_DBEG 355
#define tSTRING_DVAR 356
#define tSTRING_END 357
#define tLOWEST 358
#define tUMINUS_NUM 359
#define tLAST_TOKEN 360




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
#line 293 "grammar.y"
{
    NODE *node;
    QUID id;
    int num;
    var_table vars;
}
/* Line 193 of yacc.c.  */
#line 591 "grammar.cpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 604 "grammar.cpp"

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
#define YYLAST   10022

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  134
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  153
/* YYNRULES -- Number of rules.  */
#define YYNRULES  525
/* YYNRULES -- Number of states.  */
#define YYNSTATES  931

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   360

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     132,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,   131,   118,     2,     2,     2,   116,   111,     2,
     129,   126,   114,   112,   127,   113,   125,   115,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   106,   133,
     108,   104,   107,   105,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   123,   130,   124,   110,     2,   128,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   121,   109,   122,   119,     2,     2,     2,
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
      95,    96,    97,    98,    99,   100,   101,   102,   103,   117,
     120
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    12,    15,    17,    19,    23,
      26,    27,    32,    36,    40,    44,    47,    51,    55,    59,
      63,    67,    68,    74,    79,    83,    87,    91,    98,   104,
     110,   116,   120,   124,   128,   132,   134,   136,   140,   144,
     147,   150,   152,   154,   156,   158,   161,   164,   167,   169,
     174,   179,   180,   181,   188,   191,   195,   200,   206,   211,
     217,   220,   223,   225,   229,   231,   235,   237,   240,   244,
     247,   250,   252,   254,   258,   261,   265,   267,   272,   276,
     280,   284,   288,   291,   293,   295,   300,   304,   308,   312,
     316,   319,   321,   323,   325,   328,   330,   334,   336,   338,
     340,   342,   344,   346,   348,   350,   352,   354,   355,   360,
     362,   364,   366,   368,   370,   372,   374,   376,   378,   380,
     382,   384,   386,   388,   390,   392,   394,   396,   398,   400,
     402,   404,   406,   408,   410,   412,   414,   416,   418,   420,
     422,   424,   426,   428,   430,   432,   434,   436,   438,   440,
     442,   444,   446,   448,   450,   452,   454,   456,   458,   460,
     462,   464,   466,   468,   470,   472,   474,   476,   478,   480,
     482,   484,   486,   488,   490,   492,   494,   498,   504,   508,
     515,   521,   527,   533,   539,   544,   548,   552,   556,   560,
     564,   568,   572,   576,   580,   585,   590,   593,   596,   600,
     604,   608,   612,   616,   620,   624,   628,   632,   636,   640,
     644,   648,   651,   654,   658,   662,   666,   670,   671,   676,
     677,   684,   686,   688,   690,   693,   696,   702,   705,   709,
     713,   718,   723,   730,   732,   734,   736,   739,   745,   748,
     754,   759,   767,   771,   773,   778,   782,   788,   796,   799,
     805,   810,   817,   825,   835,   839,   841,   842,   845,   847,
     848,   852,   853,   858,   861,   864,   866,   868,   872,   876,
     881,   884,   886,   888,   890,   892,   894,   896,   898,   900,
     902,   903,   908,   909,   915,   919,   923,   926,   931,   935,
     939,   941,   946,   950,   952,   953,   960,   963,   965,   968,
     969,   977,   978,   986,   987,   988,   996,   997,   998,  1006,
    1007,  1014,  1015,  1021,  1022,  1029,  1030,  1031,  1032,  1043,
    1044,  1051,  1052,  1053,  1062,  1063,  1069,  1070,  1077,  1078,
    1079,  1089,  1091,  1093,  1095,  1097,  1099,  1101,  1103,  1105,
    1108,  1110,  1112,  1114,  1116,  1122,  1124,  1127,  1129,  1131,
    1133,  1137,  1139,  1142,  1147,  1155,  1162,  1167,  1171,  1177,
    1182,  1185,  1187,  1190,  1192,  1195,  1197,  1201,  1202,  1203,
    1210,  1213,  1218,  1223,  1226,  1231,  1236,  1240,  1244,  1247,
    1250,  1252,  1253,  1254,  1261,  1262,  1263,  1270,  1276,  1278,
    1283,  1286,  1288,  1290,  1297,  1299,  1301,  1303,  1305,  1308,
    1310,  1313,  1315,  1317,  1319,  1321,  1323,  1325,  1328,  1332,
    1336,  1340,  1344,  1348,  1349,  1353,  1355,  1358,  1362,  1366,
    1367,  1371,  1372,  1375,  1376,  1379,  1381,  1382,  1386,  1387,
    1392,  1394,  1396,  1398,  1400,  1403,  1405,  1407,  1409,  1411,
    1415,  1417,  1419,  1422,  1425,  1427,  1429,  1431,  1433,  1435,
    1437,  1439,  1441,  1443,  1445,  1447,  1449,  1451,  1453,  1455,
    1457,  1458,  1463,  1466,  1471,  1474,  1481,  1486,  1491,  1494,
    1499,  1502,  1505,  1507,  1508,  1510,  1512,  1514,  1516,  1518,
    1520,  1524,  1528,  1530,  1534,  1536,  1538,  1541,  1543,  1545,
    1547,  1550,  1553,  1555,  1557,  1558,  1564,  1566,  1569,  1572,
    1574,  1578,  1582,  1584,  1586,  1588,  1590,  1592,  1594,  1596,
    1598,  1600,  1602,  1604,  1606,  1607,  1609,  1610,  1612,  1613,
    1615,  1617,  1619,  1621,  1623,  1626
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     135,     0,    -1,    -1,   136,   138,    -1,   138,   230,   212,
     233,    -1,   139,   281,    -1,   286,    -1,   140,    -1,   139,
     285,   140,    -1,     1,   140,    -1,    -1,    44,   162,   141,
     162,    -1,    44,    52,    52,    -1,    44,    52,    61,    -1,
      44,    52,    60,    -1,     6,   163,    -1,   140,    39,   144,
      -1,   140,    40,   144,    -1,   140,    41,   144,    -1,   140,
      42,   144,    -1,   140,    43,   140,    -1,    -1,    46,   142,
     121,   138,   122,    -1,    47,   121,   138,   122,    -1,   157,
     104,   145,    -1,   151,   104,   145,    -1,   257,    85,   145,
      -1,   208,   123,   171,   124,    85,   145,    -1,   208,   125,
      50,    85,   145,    -1,   208,   125,    54,    85,   145,    -1,
     208,    83,    50,    85,   145,    -1,   258,    85,   145,    -1,
     157,   104,   184,    -1,   151,   104,   170,    -1,   151,   104,
     184,    -1,   143,    -1,   145,    -1,   143,    36,   143,    -1,
     143,    37,   143,    -1,    38,   143,    -1,   118,   145,    -1,
     167,    -1,   143,    -1,   150,    -1,   146,    -1,    29,   174,
      -1,    21,   174,    -1,    22,   174,    -1,   220,    -1,   220,
     125,   278,   176,    -1,   220,    83,   278,   176,    -1,    -1,
      -1,    92,   148,   216,   149,   138,   122,    -1,   277,   176,
      -1,   277,   176,   147,    -1,   208,   125,   278,   176,    -1,
     208,   125,   278,   176,   147,    -1,   208,    83,   278,   176,
      -1,   208,    83,   278,   176,   147,    -1,    31,   176,    -1,
      30,   176,    -1,   153,    -1,    87,   152,   126,    -1,   153,
      -1,    87,   152,   126,    -1,   155,    -1,   155,   154,    -1,
     155,    93,   156,    -1,   155,    93,    -1,    93,   156,    -1,
      93,    -1,   156,    -1,    87,   152,   126,    -1,   154,   127,
      -1,   155,   154,   127,    -1,   255,    -1,   208,   123,   171,
     124,    -1,   208,   125,    50,    -1,   208,    83,    50,    -1,
     208,   125,    54,    -1,   208,    83,    54,    -1,    84,    54,
      -1,   258,    -1,   255,    -1,   208,   123,   171,   124,    -1,
     208,   125,    50,    -1,   208,    83,    50,    -1,   208,   125,
      54,    -1,   208,    83,    54,    -1,    84,    54,    -1,   258,
      -1,    50,    -1,    54,    -1,    84,   158,    -1,   158,    -1,
     208,    83,   158,    -1,    50,    -1,    54,    -1,    51,    -1,
     165,    -1,   166,    -1,   160,    -1,   251,    -1,   161,    -1,
     253,    -1,   162,    -1,    -1,   163,   127,   164,   162,    -1,
     109,    -1,   110,    -1,   111,    -1,    67,    -1,    68,    -1,
      69,    -1,    75,    -1,   107,    -1,    71,    -1,   108,    -1,
      72,    -1,    81,    -1,    82,    -1,   112,    -1,   113,    -1,
     114,    -1,    93,    -1,   115,    -1,   116,    -1,    66,    -1,
     119,    -1,    63,    -1,    64,    -1,    79,    -1,    80,    -1,
     128,    -1,    48,    -1,    49,    -1,    46,    -1,    47,    -1,
      44,    -1,    36,    -1,     7,    -1,    21,    -1,    16,    -1,
       3,    -1,     5,    -1,    45,    -1,    26,    -1,    15,    -1,
      14,    -1,    10,    -1,     9,    -1,    35,    -1,    20,    -1,
      25,    -1,     4,    -1,    22,    -1,    33,    -1,    38,    -1,
      37,    -1,    23,    -1,     8,    -1,    24,    -1,    29,    -1,
      32,    -1,    31,    -1,    13,    -1,    34,    -1,     6,    -1,
      17,    -1,    30,    -1,    39,    -1,    40,    -1,    41,    -1,
      42,    -1,    43,    -1,   157,   104,   167,    -1,   157,   104,
     167,    43,   167,    -1,   257,    85,   167,    -1,   208,   123,
     171,   124,    85,   167,    -1,   208,   125,    50,    85,   167,
      -1,   208,   125,    54,    85,   167,    -1,   208,    83,    50,
      85,   167,    -1,   208,    83,    54,    85,   167,    -1,    84,
      54,    85,   167,    -1,   258,    85,   167,    -1,   167,    77,
     167,    -1,   167,    78,   167,    -1,   167,   112,   167,    -1,
     167,   113,   167,    -1,   167,   114,   167,    -1,   167,   115,
     167,    -1,   167,   116,   167,    -1,   167,    66,   167,    -1,
     117,    57,    66,   167,    -1,   117,    58,    66,   167,    -1,
      63,   167,    -1,    64,   167,    -1,   167,   109,   167,    -1,
     167,   110,   167,    -1,   167,   111,   167,    -1,   167,    67,
     167,    -1,   167,   107,   167,    -1,   167,    71,   167,    -1,
     167,   108,   167,    -1,   167,    72,   167,    -1,   167,    68,
     167,    -1,   167,    69,   167,    -1,   167,    70,   167,    -1,
     167,    75,   167,    -1,   167,    76,   167,    -1,   118,   167,
      -1,   119,   167,    -1,   167,    81,   167,    -1,   167,    82,
     167,    -1,   167,    73,   167,    -1,   167,    74,   167,    -1,
      -1,    45,   282,   168,   167,    -1,    -1,   167,   105,   169,
     167,   106,   167,    -1,   185,    -1,   167,    -1,   286,    -1,
     150,   282,    -1,   183,   283,    -1,   183,   127,    93,   167,
     282,    -1,   275,   283,    -1,    93,   167,   282,    -1,   129,
     286,   126,    -1,   129,   174,   282,   126,    -1,   129,   220,
     282,   126,    -1,   129,   183,   127,   220,   282,   126,    -1,
     286,    -1,   172,    -1,   150,    -1,   183,   182,    -1,   183,
     127,    93,   170,   182,    -1,   275,   182,    -1,   275,   127,
      93,   170,   182,    -1,   183,   127,   275,   182,    -1,   183,
     127,   275,   127,    93,   167,   182,    -1,    93,   170,   182,
      -1,   181,    -1,   170,   127,   183,   182,    -1,   170,   127,
     181,    -1,   170,   127,    93,   170,   182,    -1,   170,   127,
     183,   127,    93,   170,   182,    -1,   275,   182,    -1,   275,
     127,    93,   170,   182,    -1,   170,   127,   275,   182,    -1,
     170,   127,   183,   127,   275,   182,    -1,   170,   127,   275,
     127,    93,   170,   182,    -1,   170,   127,   183,   127,   275,
     127,    93,   170,   182,    -1,    93,   170,   182,    -1,   181,
      -1,    -1,   177,   178,    -1,   174,    -1,    -1,    88,   179,
     126,    -1,    -1,    88,   175,   180,   126,    -1,    94,   170,
      -1,   127,   181,    -1,   286,    -1,   170,    -1,   183,   127,
     170,    -1,   183,   127,   170,    -1,   183,   127,    93,   170,
      -1,    93,   170,    -1,   234,    -1,   235,    -1,   238,    -1,
     239,    -1,   240,    -1,   243,    -1,   256,    -1,   258,    -1,
      51,    -1,    -1,     7,   186,   137,    10,    -1,    -1,    88,
     143,   187,   282,   126,    -1,    87,   138,   126,    -1,   208,
      83,    54,    -1,    84,    54,    -1,   208,   123,   171,   124,
      -1,    90,   171,   124,    -1,    91,   274,   122,    -1,    29,
      -1,    30,   129,   174,   126,    -1,    30,   129,   126,    -1,
      30,    -1,    -1,    45,   282,   129,   188,   143,   126,    -1,
     277,   222,    -1,   221,    -1,   221,   222,    -1,    -1,    11,
     189,   144,   209,   138,   211,    10,    -1,    -1,    12,   190,
     144,   209,   138,   212,    10,    -1,    -1,    -1,    18,   191,
     144,   210,   192,   138,    10,    -1,    -1,    -1,    19,   193,
     144,   210,   194,   138,    10,    -1,    -1,    16,   195,   144,
     281,   227,    10,    -1,    -1,    16,   281,   196,   227,    10,
      -1,    -1,    16,   281,   197,    15,   138,    10,    -1,    -1,
      -1,    -1,    20,   198,   213,    25,   199,   144,   210,   200,
     138,    10,    -1,    -1,     3,   159,   259,   201,   137,    10,
      -1,    -1,    -1,     3,    81,   143,   202,   284,   203,   137,
      10,    -1,    -1,     4,   159,   204,   137,    10,    -1,    -1,
       5,   160,   205,   261,   137,    10,    -1,    -1,    -1,     5,
     272,   280,   206,   160,   207,   261,   137,    10,    -1,    21,
      -1,    22,    -1,    23,    -1,    24,    -1,   185,    -1,   284,
      -1,   106,    -1,    13,    -1,   284,    13,    -1,   284,    -1,
     106,    -1,    27,    -1,   212,    -1,    14,   144,   209,   138,
     211,    -1,   286,    -1,    15,   138,    -1,   157,    -1,   151,
      -1,   154,    -1,   214,   127,   154,    -1,   214,    -1,   214,
     127,    -1,   214,   127,    94,   157,    -1,   214,   127,    93,
     157,   127,    94,   157,    -1,   214,   127,    93,   127,    94,
     157,    -1,   214,   127,    93,   157,    -1,   214,   127,    93,
      -1,    93,   157,   127,    94,   157,    -1,    93,   127,    94,
     157,    -1,    93,   157,    -1,    93,    -1,    94,   157,    -1,
     286,    -1,   109,   109,    -1,    74,    -1,   109,   215,   109,
      -1,    -1,    -1,    28,   218,   216,   219,   138,    10,    -1,
     150,   217,    -1,   220,   125,   278,   173,    -1,   220,    83,
     278,   173,    -1,   277,   172,    -1,   208,   125,   278,   173,
      -1,   208,    83,   278,   172,    -1,   208,    83,   279,    -1,
     208,   130,   278,    -1,    65,   278,    -1,    31,   172,    -1,
      31,    -1,    -1,    -1,   121,   223,   216,   224,   138,   122,
      -1,    -1,    -1,    26,   225,   216,   226,   138,    10,    -1,
      17,   228,   209,   138,   229,    -1,   183,    -1,   183,   127,
      93,   170,    -1,    93,   170,    -1,   212,    -1,   227,    -1,
       8,   231,   232,   209,   138,   230,    -1,   286,    -1,   170,
      -1,   184,    -1,   286,    -1,    86,   157,    -1,   286,    -1,
       9,   138,    -1,   286,    -1,   254,    -1,   251,    -1,   253,
      -1,   236,    -1,   237,    -1,   236,   237,    -1,    96,   245,
     102,    -1,    56,   246,   102,    -1,    97,   246,    62,    -1,
      98,   131,   102,    -1,    98,   241,   102,    -1,    -1,   241,
     242,   131,    -1,   247,    -1,   242,   247,    -1,    99,   131,
     102,    -1,    99,   244,   102,    -1,    -1,   244,    59,   131,
      -1,    -1,   245,   247,    -1,    -1,   246,   247,    -1,    59,
      -1,    -1,   101,   248,   250,    -1,    -1,   100,   249,   138,
     122,    -1,    52,    -1,    53,    -1,    55,    -1,   258,    -1,
      95,   252,    -1,   160,    -1,    53,    -1,    52,    -1,    55,
      -1,    95,   246,   102,    -1,    57,    -1,    58,    -1,   117,
      57,    -1,   117,    58,    -1,    50,    -1,    53,    -1,    52,
      -1,    54,    -1,    55,    -1,    33,    -1,    32,    -1,    34,
      -1,    35,    -1,    49,    -1,    48,    -1,   255,    -1,   255,
      -1,    60,    -1,    61,    -1,   284,    -1,    -1,   108,   260,
     144,   284,    -1,     1,   284,    -1,   129,   262,   282,   126,
      -1,   262,   284,    -1,   264,   127,   266,   127,   268,   271,
      -1,   264,   127,   266,   271,    -1,   264,   127,   268,   271,
      -1,   264,   271,    -1,   266,   127,   268,   271,    -1,   266,
     271,    -1,   268,   271,    -1,   270,    -1,    -1,    54,    -1,
      53,    -1,    52,    -1,    55,    -1,    50,    -1,   263,    -1,
     264,   127,   263,    -1,    50,   104,   170,    -1,   265,    -1,
     266,   127,   265,    -1,   114,    -1,    93,    -1,   267,    50,
      -1,   267,    -1,   111,    -1,    94,    -1,   269,    50,    -1,
     127,   270,    -1,   286,    -1,   256,    -1,    -1,   129,   273,
     143,   282,   126,    -1,   286,    -1,   275,   283,    -1,   183,
     283,    -1,   276,    -1,   275,   127,   276,    -1,   170,    86,
     170,    -1,    50,    -1,    54,    -1,    51,    -1,    50,    -1,
      54,    -1,    51,    -1,   165,    -1,    50,    -1,    51,    -1,
     165,    -1,   125,    -1,    83,    -1,    -1,   285,    -1,    -1,
     132,    -1,    -1,   132,    -1,   127,    -1,   133,    -1,   132,
      -1,   284,    -1,   285,   133,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   431,   431,   431,   454,   474,   481,   482,   486,   490,
     496,   496,   500,   504,   511,   516,   520,   529,   538,   550,
     562,   568,   567,   583,   591,   595,   601,   626,   642,   654,
     666,   678,   683,   687,   692,   697,   700,   701,   705,   709,
     713,   717,   720,   727,   728,   729,   733,   737,   743,   744,
     748,   755,   759,   754,   768,   773,   785,   790,   802,   807,
     819,   824,   831,   832,   838,   839,   845,   849,   853,   857,
     861,   865,   871,   872,   878,   882,   888,   892,   896,   900,
     904,   908,   914,   920,   927,   931,   935,   939,   943,   947,
     953,   959,   966,   970,   973,   977,   981,   987,   988,   989,
     990,   995,  1002,  1003,  1006,  1010,  1013,  1017,  1017,  1023,
    1024,  1025,  1026,  1027,  1028,  1029,  1030,  1031,  1032,  1033,
    1034,  1035,  1036,  1037,  1038,  1039,  1040,  1041,  1042,  1043,
    1044,  1045,  1046,  1047,  1048,  1051,  1051,  1051,  1051,  1052,
    1052,  1052,  1052,  1052,  1052,  1052,  1053,  1053,  1053,  1053,
    1053,  1053,  1053,  1054,  1054,  1054,  1054,  1054,  1054,  1055,
    1055,  1055,  1055,  1055,  1055,  1055,  1056,  1056,  1056,  1056,
    1056,  1057,  1057,  1057,  1057,  1057,  1060,  1064,  1068,  1093,
    1109,  1121,  1133,  1145,  1150,  1155,  1160,  1166,  1172,  1176,
    1180,  1184,  1188,  1192,  1196,  1200,  1204,  1213,  1217,  1221,
    1225,  1229,  1233,  1237,  1241,  1245,  1249,  1253,  1257,  1261,
    1265,  1269,  1273,  1277,  1281,  1285,  1289,  1293,  1293,  1298,
    1298,  1304,  1310,  1317,  1318,  1323,  1327,  1332,  1336,  1343,
    1347,  1351,  1356,  1363,  1364,  1367,  1372,  1376,  1381,  1386,
    1391,  1396,  1402,  1406,  1409,  1413,  1417,  1422,  1427,  1432,
    1437,  1442,  1447,  1452,  1458,  1462,  1465,  1465,  1477,  1478,
    1478,  1483,  1483,  1490,  1496,  1500,  1503,  1507,  1513,  1517,
    1521,  1527,  1528,  1529,  1530,  1531,  1532,  1533,  1534,  1535,
    1540,  1539,  1554,  1554,  1559,  1564,  1568,  1572,  1581,  1590,
    1594,  1598,  1602,  1606,  1610,  1610,  1615,  1621,  1622,  1631,
    1631,  1647,  1647,  1663,  1666,  1663,  1677,  1680,  1677,  1691,
    1691,  1701,  1701,  1708,  1708,  1715,  1717,  1717,  1715,  1726,
    1725,  1744,  1750,  1743,  1768,  1767,  1786,  1785,  1805,  1806,
    1805,  1822,  1826,  1830,  1834,  1840,  1847,  1848,  1849,  1850,
    1853,  1854,  1855,  1858,  1859,  1868,  1869,  1875,  1876,  1879,
    1883,  1889,  1898,  1902,  1906,  1910,  1914,  1918,  1922,  1926,
    1930,  1934,  1938,  1944,  1945,  1949,  1953,  1960,  1966,  1959,
    1978,  1987,  1991,  1997,  2002,  2007,  2012,  2016,  2021,  2026,
    2030,  2037,  2041,  2036,  2048,  2053,  2047,  2062,  2069,  2070,
    2074,  2080,  2081,  2084,  2095,  2098,  2102,  2103,  2106,  2110,
    2113,  2121,  2124,  2125,  2129,  2132,  2145,  2146,  2152,  2158,
    2181,  2211,  2215,  2222,  2225,  2231,  2232,  2238,  2242,  2249,
    2252,  2259,  2262,  2269,  2272,  2278,  2280,  2279,  2291,  2290,
    2310,  2311,  2312,  2313,  2316,  2323,  2324,  2325,  2326,  2329,
    2360,  2361,  2362,  2366,  2372,  2373,  2374,  2375,  2376,  2377,
    2378,  2379,  2380,  2381,  2382,  2385,  2391,  2397,  2398,  2401,
    2406,  2405,  2413,  2416,  2422,  2428,  2432,  2436,  2440,  2444,
    2448,  2452,  2456,  2461,  2466,  2470,  2474,  2478,  2482,  2493,
    2494,  2500,  2510,  2515,  2521,  2522,  2525,  2533,  2539,  2540,
    2543,  2553,  2557,  2560,  2565,  2565,  2590,  2591,  2595,  2604,
    2605,  2611,  2617,  2618,  2619,  2622,  2623,  2624,  2625,  2628,
    2629,  2630,  2633,  2634,  2637,  2638,  2641,  2642,  2645,  2646,
    2647,  2650,  2651,  2654,  2655,  2658
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
  "k__LINE__", "k__FILE__", "tIDENTIFIER", "tFID", "tGVAR", "tIVAR",
  "tCONSTANT", "tCVAR", "tXSTRING_BEG", "tINTEGER", "tFLOAT",
  "tSTRING_CONTENT", "tNTH_REF", "tBACK_REF", "tREGEXP_END", "tUPLUS",
  "tUMINUS", "tUBS", "tPOW", "tCMP", "tEQ", "tEQQ", "tNEQ", "tGEQ", "tLEQ",
  "tANDOP", "tOROP", "tMATCH", "tNMATCH", "tDOT2", "tDOT3", "tAREF",
  "tASET", "tLSHFT", "tRSHFT", "tCOLON2", "tCOLON3", "tOP_ASGN", "tASSOC",
  "tLPAREN", "tLPAREN_ARG", "tRPAREN", "tLBRACK", "tLBRACE", "tLBRACE_ARG",
  "tSTAR", "tAMPER", "tSYMBEG", "tSTRING_BEG", "tREGEXP_BEG", "tWORDS_BEG",
  "tQWORDS_BEG", "tSTRING_DBEG", "tSTRING_DVAR", "tSTRING_END", "tLOWEST",
  "'='", "'?'", "':'", "'>'", "'<'", "'|'", "'^'", "'&'", "'+'", "'-'",
  "'*'", "'/'", "'%'", "tUMINUS_NUM", "'!'", "'~'", "tLAST_TOKEN", "'{'",
  "'}'", "'['", "']'", "'.'", "')'", "','", "'`'", "'('", "'\\\\'", "' '",
  "'\\n'", "';'", "$accept", "program", "@1", "bodystmt", "compstmt",
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
  "opt_else", "for_var", "block_par", "block_var", "opt_block_var",
  "do_block", "@34", "@35", "block_call", "method_call", "brace_block",
  "@36", "@37", "@38", "@39", "case_body", "when_args", "cases",
  "opt_rescue", "exc_list", "exc_var", "opt_ensure", "literal", "strings",
  "string", "string1", "xstring", "regexp", "words", "word_list", "word",
  "qwords", "qword_list", "string_contents", "xstring_contents",
  "string_content", "@40", "@41", "string_dvar", "symbol", "sym", "dsym",
  "numeric", "variable", "var_ref", "var_lhs", "backref", "superclass",
  "@42", "f_arglist", "f_args", "f_norm_arg", "f_arg", "f_opt", "f_optarg",
  "restarg_mark", "f_rest_arg", "blkarg_mark", "f_block_arg",
  "opt_f_block_arg", "singleton", "@43", "assoc_list", "assocs", "assoc",
  "operation", "operation2", "operation3", "dot_or_colon", "opt_terms",
  "opt_nl", "trailer", "term", "terms", "none", 0
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
     355,   356,   357,   358,    61,    63,    58,    62,    60,   124,
      94,    38,    43,    45,    42,    47,    37,   359,    33,   126,
     360,   123,   125,    91,    93,    46,    41,    44,    96,    40,
      92,    32,    10,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   134,   136,   135,   137,   138,   139,   139,   139,   139,
     141,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   142,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   143,   143,   143,   143,
     143,   143,   144,   145,   145,   145,   145,   145,   146,   146,
     146,   148,   149,   147,   150,   150,   150,   150,   150,   150,
     150,   150,   151,   151,   152,   152,   153,   153,   153,   153,
     153,   153,   154,   154,   155,   155,   156,   156,   156,   156,
     156,   156,   156,   156,   157,   157,   157,   157,   157,   157,
     157,   157,   158,   158,   159,   159,   159,   160,   160,   160,
     160,   160,   161,   161,   162,   162,   163,   164,   163,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   168,   167,   169,
     167,   167,   170,   171,   171,   171,   171,   171,   171,   172,
     172,   172,   172,   173,   173,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   177,   176,   178,   179,
     178,   180,   178,   181,   182,   182,   183,   183,   184,   184,
     184,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     186,   185,   187,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   188,   185,   185,   185,   185,   189,
     185,   190,   185,   191,   192,   185,   193,   194,   185,   195,
     185,   196,   185,   197,   185,   198,   199,   200,   185,   201,
     185,   202,   203,   185,   204,   185,   205,   185,   206,   207,
     185,   185,   185,   185,   185,   208,   209,   209,   209,   209,
     210,   210,   210,   211,   211,   212,   212,   213,   213,   214,
     214,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     215,   215,   215,   216,   216,   216,   216,   218,   219,   217,
     220,   220,   220,   221,   221,   221,   221,   221,   221,   221,
     221,   223,   224,   222,   225,   226,   222,   227,   228,   228,
     228,   229,   229,   230,   230,   231,   231,   231,   232,   232,
     233,   233,   234,   234,   234,   235,   236,   236,   237,   238,
     239,   240,   240,   241,   241,   242,   242,   243,   243,   244,
     244,   245,   245,   246,   246,   247,   248,   247,   249,   247,
     250,   250,   250,   250,   251,   252,   252,   252,   252,   253,
     254,   254,   254,   254,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   256,   257,   258,   258,   259,
     260,   259,   259,   261,   261,   262,   262,   262,   262,   262,
     262,   262,   262,   262,   263,   263,   263,   263,   263,   264,
     264,   265,   266,   266,   267,   267,   268,   268,   269,   269,
     270,   271,   271,   272,   273,   272,   274,   274,   274,   275,
     275,   276,   277,   277,   277,   278,   278,   278,   278,   279,
     279,   279,   280,   280,   281,   281,   282,   282,   283,   283,
     283,   284,   284,   285,   285,   286
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     4,     2,     1,     1,     3,     2,
       0,     4,     3,     3,     3,     2,     3,     3,     3,     3,
       3,     0,     5,     4,     3,     3,     3,     6,     5,     5,
       5,     3,     3,     3,     3,     1,     1,     3,     3,     2,
       2,     1,     1,     1,     1,     2,     2,     2,     1,     4,
       4,     0,     0,     6,     2,     3,     4,     5,     4,     5,
       2,     2,     1,     3,     1,     3,     1,     2,     3,     2,
       2,     1,     1,     3,     2,     3,     1,     4,     3,     3,
       3,     3,     2,     1,     1,     4,     3,     3,     3,     3,
       2,     1,     1,     1,     2,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     5,     3,     6,
       5,     5,     5,     5,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     4,     4,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     3,     3,     3,     3,     0,     4,     0,
       6,     1,     1,     1,     2,     2,     5,     2,     3,     3,
       4,     4,     6,     1,     1,     1,     2,     5,     2,     5,
       4,     7,     3,     1,     4,     3,     5,     7,     2,     5,
       4,     6,     7,     9,     3,     1,     0,     2,     1,     0,
       3,     0,     4,     2,     2,     1,     1,     3,     3,     4,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     4,     0,     5,     3,     3,     2,     4,     3,     3,
       1,     4,     3,     1,     0,     6,     2,     1,     2,     0,
       7,     0,     7,     0,     0,     7,     0,     0,     7,     0,
       6,     0,     5,     0,     6,     0,     0,     0,    10,     0,
       6,     0,     0,     8,     0,     5,     0,     6,     0,     0,
       9,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     5,     1,     2,     1,     1,     1,
       3,     1,     2,     4,     7,     6,     4,     3,     5,     4,
       2,     1,     2,     1,     2,     1,     3,     0,     0,     6,
       2,     4,     4,     2,     4,     4,     3,     3,     2,     2,
       1,     0,     0,     6,     0,     0,     6,     5,     1,     4,
       2,     1,     1,     6,     1,     1,     1,     1,     2,     1,
       2,     1,     1,     1,     1,     1,     1,     2,     3,     3,
       3,     3,     3,     0,     3,     1,     2,     3,     3,     0,
       3,     0,     2,     0,     2,     1,     0,     3,     0,     4,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     3,
       1,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     4,     2,     4,     2,     6,     4,     4,     2,     4,
       2,     2,     1,     0,     1,     1,     1,     1,     1,     1,
       3,     3,     1,     3,     1,     1,     2,     1,     1,     1,
       2,     2,     1,     1,     0,     5,     1,     2,     2,     1,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     1,     0,     1,     0,     1,
       1,     1,     1,     1,     2,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     0,     1,     0,     0,     0,     0,     0,   280,
     299,   301,   309,   303,   306,   315,   331,   332,   333,   334,
     290,   293,   380,   450,   449,   451,   452,     0,     0,   516,
      21,     0,   454,   453,   444,   279,   446,   445,   447,   448,
     423,   440,   441,   457,   458,     0,     0,     0,     0,     0,
       0,   525,   525,    71,   423,   421,   423,   413,   419,     0,
       0,     0,     3,   514,     7,    35,    36,    44,    43,     0,
      62,     0,    66,    72,     0,    41,   221,     0,    48,   297,
     271,   272,   405,   406,   273,   274,   275,   276,   403,   404,
     402,   455,   277,     0,   278,   256,     6,     9,   331,   332,
     290,   293,   380,   516,    92,    93,     0,     0,     0,     0,
      95,     0,   335,     0,   455,   278,     0,   324,   144,   155,
     145,   168,   141,   161,   151,   150,   166,   149,   148,   143,
     169,   153,   142,   156,   160,   162,   154,   147,   163,   170,
     165,   164,   157,   167,   152,   140,   159,   158,   171,   172,
     173,   174,   175,   139,   146,   137,   138,   135,   136,    97,
      99,    98,   130,   131,   128,   112,   113,   114,   117,   119,
     115,   132,   133,   120,   121,   125,   116,   118,   109,   110,
     111,   122,   123,   124,   126,   127,   129,   134,   494,   326,
     100,   101,   493,     0,   164,   157,   167,   152,   135,   136,
      97,    98,   102,   104,   106,    15,   103,   105,     0,     0,
       0,   522,   521,     0,   311,   523,   515,     0,     0,     0,
       0,     0,     0,     0,   235,     0,   222,   266,    46,   243,
     525,     0,   455,     0,   278,   525,   499,    47,    45,     0,
      61,     0,   525,   379,    60,    39,     0,    10,   517,   217,
       0,     0,     0,   196,     0,   197,   505,   507,   506,   508,
     378,   286,     0,     0,     0,    62,   282,     0,   516,     0,
     518,   518,   223,   518,     0,   518,   496,     0,    70,     0,
      76,    83,   437,   436,   438,   435,     0,   434,     0,     0,
       0,     0,     0,     0,   442,   443,    40,   211,   212,     5,
     515,     0,     0,     0,     0,     0,     0,     0,   367,   370,
       0,    74,     0,    69,    67,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   219,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   525,     0,     0,     0,     0,   384,   381,
     298,   407,     0,     0,   373,    54,   296,     0,   321,    92,
      93,    94,   442,   443,     0,   460,   319,   459,     0,   525,
       0,     0,     0,   473,   513,   512,   328,   107,     0,   525,
      42,     0,     0,   514,     0,     0,   524,     0,     0,     0,
     348,   347,     0,     0,   455,   278,   286,   525,   263,     0,
       0,     0,   236,   265,     0,   525,     0,     0,     0,     0,
     238,   292,     0,   259,   258,   257,   235,   516,   525,   516,
       0,    12,    14,    13,     0,   294,     0,     0,     0,   425,
     428,   426,   409,   424,     0,     0,     0,     0,   284,    63,
     516,   516,   224,   288,   520,   519,   225,   520,   227,   520,
     498,   289,   497,    82,     0,   525,     0,   439,   408,   422,
     410,   411,   412,     0,   415,   417,     0,   418,     0,     0,
       8,    16,    17,    18,    19,    20,    37,    38,   525,     0,
      25,    33,     0,    34,     0,    68,    75,    24,   176,   266,
      32,   193,   201,   206,   207,   208,   203,   205,   215,   216,
     209,   210,   186,   187,   213,   214,     0,   202,   204,   198,
     199,   200,   188,   189,   190,   191,   192,   505,   510,   506,
     511,   256,   376,     0,   505,   506,   525,   377,   256,   256,
     525,   525,    26,   178,    31,   185,    51,    55,     0,   462,
       0,     0,    92,    93,    96,     0,     0,   525,     0,   516,
     478,   476,   475,   474,   477,   485,   489,   488,   484,   473,
       0,     0,   479,   525,   482,   525,   487,   525,     0,   472,
       0,     0,   281,   525,   525,   394,   338,   337,     0,   336,
       0,     0,     0,     0,     0,   342,   341,   304,   340,   307,
     286,     0,   525,     0,   316,     0,   242,   176,   501,     0,
     267,   264,   525,   509,   285,     0,   505,   506,     0,     0,
     500,   291,     0,    41,     0,   261,     0,   255,   525,     0,
       0,     0,     0,     0,   229,    11,     0,   218,     0,    23,
       0,     0,   184,    63,     0,   228,     0,   267,   509,    81,
       0,   505,   506,   414,   416,   420,   194,   195,   365,     0,
     368,   363,   270,     0,    73,     0,     0,     0,     0,   375,
      58,   287,     0,     0,   234,   374,    56,   233,   372,    50,
     371,    49,   385,   382,   525,   322,     0,     0,   287,   325,
       0,     0,   516,     0,   464,     0,   468,   492,     0,   470,
     486,     0,   471,   490,   329,   108,   395,   396,   525,   397,
       0,   525,   345,   525,   339,   525,     0,     0,   388,     0,
     312,     0,     0,     0,   509,   285,     0,   505,   506,     0,
     525,     0,   240,     0,   287,     0,     0,   525,   525,     0,
       0,   260,     0,   248,   230,     0,   516,   525,   525,   231,
       0,    22,     0,   430,   431,   432,   427,   433,   283,   516,
      77,   361,     0,   364,   349,   351,     0,     0,     0,   268,
     177,     0,    30,   182,   183,    59,     0,    28,   180,    29,
     181,    57,     0,     0,    52,     0,   461,   320,   495,   481,
       0,   327,   480,   525,   525,   491,     0,   483,   525,   473,
       0,     0,   399,   346,     0,     4,   401,     0,     0,   343,
       0,   310,   390,     0,     0,   314,     0,     0,   287,     0,
     237,     0,     0,   239,   254,     0,   245,   525,   525,   262,
       0,     0,   295,   429,   226,     0,     0,   360,     0,    84,
      91,   362,   352,   366,     0,   269,   220,    27,   179,     0,
       0,     0,     0,   463,     0,   466,   467,   469,     0,   398,
       0,   400,     0,   300,   302,     0,   525,   305,   308,   317,
     525,   525,     0,   244,     0,   250,   525,   232,    90,     0,
       0,     0,   525,     0,   357,     0,   350,   369,   386,   383,
       0,   323,   525,     0,   525,     0,   389,   391,   392,   387,
       0,   241,   246,     0,   525,     0,   249,   359,     0,    87,
      89,     0,   505,   506,     0,   356,   353,    53,   465,   330,
     393,   525,     0,   525,     0,   251,   525,   358,    85,     0,
       0,   344,   318,   247,     0,   252,   355,     0,   525,   354,
     253
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,   378,   379,    63,    64,   424,   250,    65,
     381,    66,    67,   537,   674,   841,    68,    69,   264,    70,
      71,    72,    73,   225,   110,   111,   202,   203,   204,   205,
     571,   259,   191,    75,   426,   506,   227,   269,   664,   665,
     228,   615,   240,   241,   415,   616,   730,   601,   402,   270,
     483,    76,   208,   440,   626,   209,   210,   217,   712,   218,
     713,   213,   384,   385,   219,   719,   890,   541,   538,   775,
     371,   373,   570,   789,   254,   578,   587,   798,   799,   393,
     755,   756,   650,   309,   478,   757,    78,    79,   356,   531,
     773,   530,   772,   583,   709,   889,   574,   698,   791,   795,
      80,    81,    82,    83,    84,    85,    86,   291,   463,    87,
     293,   288,   252,   433,   631,   630,   746,    88,   287,    89,
      90,   232,    92,   233,   234,   366,   540,   560,   561,   562,
     563,   564,   565,   566,   567,   568,   785,   686,   193,   372,
     274,   271,   236,   116,   545,   522,   376,   214,   249,   446,
     579,   216,    96
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -645
static const yytype_int16 yypact[] =
{
    -645,   100,  2630,  -645,  5898,  8490,  8790,  4052,  5354,  -645,
    -645,  -645,   102,  -645,  -645,  -645,  6106,  6106,  -645,  -645,
    6106,  4691,  4795,  -645,  -645,  -645,  -645,  7250,  5237,   -10,
    -645,    19,  -645,  -645,  4171,  4275,  -645,  -645,  4379,  -645,
    -645,  -645,  -645,  -645,  -645,  8082,  8082,  1139,    82,  3559,
    7250,  7354,  8082,  8890,  5120,  -645,  -645,    23,    35,    94,
    8186,  8082,  -645,   133,   772,   420,  -645,  -645,   176,    41,
    -645,   111,  8590,  -645,   177,  9787,   212,   276,    76,    57,
    -645,  -645,   194,  -645,  -645,  -645,  -645,  -645,  -645,  -645,
    -645,    12,  -645,   234,   169,    20,  -645,   772,  -645,  -645,
    -645,   193,   196,   -10,   397,   424,  7250,   384,  3683,   436,
    -645,    39,  -645,   372,  -645,  -645,    20,  -645,  -645,  -645,
    -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,
    -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,
    -645,    77,    80,   158,   184,  -645,  -645,  -645,  -645,  -645,
    -645,  -645,  -645,  -645,  -645,  -645,  -645,   187,   214,   288,
    -645,   353,  -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,
    -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,
    -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,
    -645,  -645,  -645,   371,  -645,  -645,  -645,  -645,  -645,  -645,
    -645,  -645,  -645,  -645,  -645,   219,  -645,  -645,  2959,  7250,
    7250,  -645,  -645,  7250,   338,  -645,   247,  7250,  7250,  8690,
     339,  8082,  8082,  8082,  -645,   300,  9787,   321,  -645,  -645,
     302,   468,   105,   354,   167,   332,  -645,  -645,  -645,  5789,
    -645,  6210,  6106,  -645,  -645,  -645,   409,  -645,  -645,   346,
     347,  3807,   466,  -645,   497,   411,  -645,  -645,  -645,  -645,
    -645,   222,  3559,   383,   386,   388,   420,  8082,   -10,   399,
     -31,   216,  -645,   273,   406,   216,  -645,   467,  -645,   514,
     528,   534,  -645,  -645,  -645,  -645,   501,  -645,   612,    67,
     441,   660,   450,    22,   470,   480,  -645,  -645,  -645,  -645,
    2474,  7250,  7250,  7250,  7250,  5898,  7250,  7250,  -645,  -645,
    7458,  -645,  3559,  8890,   429,  7458,  8082,  8082,  8082,  8082,
    8082,  8082,  8082,  8082,  8082,  8082,  8082,  8082,  8082,  8082,
    8082,  -645,  8082,  8082,  8082,  8082,  8082,  8082,  8082,  8082,
    8082,  8082,  1988,  7354,  9042,  1139,  1139,  1139,  -645,  -645,
    -645,  -645,  8186,  8186,  -645,   485,  -645,   346,   420,  -645,
     563,  -645,  -645,  -645,   133,  -645,  -645,  -645,  9109,  7354,
    1139,  2959,  7250,   615,  -645,  -645,  -645,  -645,   552,   564,
     420,    44,    44,   133,   569,   574,  -645,    29,    29,   546,
    -645,  -645,   566,   584,    47,    55,   189,   483,  -645,  8082,
    8082,  6314,  -645,  -645,  9176,  7354,  9243,  8082,  8082,  6418,
    -645,  -645,   486,  6002,  -645,  -645,   176,   -10,   489,    54,
     495,  -645,  -645,  -645,  5354,  -645,  8082,  3807,   503,  -645,
    -645,  -645,  -645,  -645,  9176,  9243,  8082,   500,  -645,   496,
     -10,  2508,  -645,  -645,  7562,  -645,  -645,  8082,  -645,  8082,
    -645,  -645,  -645,   563,  9310,  7354,  9377,  -645,  -645,  -645,
    -645,  -645,  -645,   161,  -645,  -645,   498,  -645,  8082,  8082,
     772,  -645,  -645,  -645,  -645,  -645,  -645,  -645,    79,  8082,
    -645,   506,   507,  -645,   517,  -645,  -645,  -645,  1902,  -645,
    -645,   411,  9855,  9855,  9855,  9855,   926,   926,  9906,  9838,
    9855,  9855,  1645,  1645,   350,   350,  8082,   926,   926,   685,
     685,   419,   330,   330,   411,   411,   411,  2698,  4899,  2826,
    5003,   196,  -645,   521,   444,   488,  4795,  -645,  1418,  1418,
      79,    79,  -645,  9787,  -645,  9787,  -645,  -645,   133,  -645,
    7250,  2959,   434,   253,  -645,   196,   524,   196,   644,    78,
     558,  -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,   622,
    2959,   133,  -645,   539,  -645,   551,   629,   554,   637,  -645,
    5471,  5354,  -645,  7666,   675,  -645,  -645,  -645,  3083,   681,
    3321,   569,  7770,   687,  3931,  -645,  -645,  -645,  -645,  -645,
      85,  9444,  7354,  9511,  -645,   613,  -645,  2117,  -645,  8082,
     321,  -645,   579,  4483,  4587,   594,   260,   269,  8082,   321,
    -645,  -645,  8082,  2805,   -34,  -645,   602,  -645,   604,   606,
    6522,  1139,  1139,   611,  -645,  -645,  7250,  9787,   616,  -645,
    3807,   765,  9787,   379,   619,  -645,  8082,  -645,   117,   253,
     624,   377,   415,  -645,  -645,  -645,   411,   411,  -645,  8290,
    -645,  -645,  -645,  7874,  -645,  8082,  9770,  8186,  8082,  -645,
     485,   597,  8186,  8186,  -645,  -645,   485,  -645,  -645,  -645,
    -645,  -645,  -645,  -645,    79,  -645,   133,   731,  -645,  -645,
     627,  8082,   -10,   744,  -645,   622,  -645,  -645,   164,  -645,
    -645,   148,  -645,  -645,  -645,  -645,   506,  -645,   670,  -645,
    3440,   749,  -645,   580,  -645,   675,   755,  8082,   642,    44,
    -645,   764,  3931,  3931,    60,    64,   651,    95,    98,  7250,
     483,  6626,  -645,  8082,   304,  8082,  8082,   483,   483,  6730,
     650,  -645,  6834,  -645,  -645,   176,    54,   196,   196,  -645,
      97,  -645,   655,  -645,  -645,  -645,  -645,  -645,  -645,  2508,
     600,  5589,  8990,  -645,  -645,   652,   671,  3931,  8082,   654,
    9787,  8082,  -645,  9787,  9787,  -645,  8186,  -645,  9787,  -645,
    9787,  -645,  3931,  3807,  -645,  2959,  -645,  -645,  -645,  -645,
     661,  -645,  -645,   659,   554,  -645,   558,  -645,   554,   615,
    8990,    44,  -645,  -645,  3931,  -645,  -645,  7250,   779,  -645,
     780,  -645,  -645,  7978,  3202,  -645,   781,   784,   106,    29,
    -645,  8082,  8082,  -645,  -645,  8082,  -645,   676,   678,  -645,
    8082,   690,  -645,  -645,  -645,   753,   728,   700,   617,   528,
     534,  -645,  8390,  -645,   821,  -645,  9787,  -645,  9787,   822,
     711,  3807,   824,  -645,   164,  -645,  -645,  -645,  2959,  -645,
    2959,  -645,    44,  -645,  -645,  8082,   113,  -645,  -645,  -645,
    9708,   483,  6938,  -645,  7042,  -645,   483,  -645,   563,  8990,
     741,  9578,  7354,  9645,  5689,  8990,  -645,  -645,  -645,  -645,
     715,  -645,   554,   828,   564,  3083,  -645,  -645,  -645,  -645,
    3931,  -645,  -645,  8082,   713,  8082,  -645,  -645,  8990,   434,
     253,   718,    37,    86,   756,   716,  -645,  -645,  -645,  -645,
    -645,   580,   841,   483,  7146,  -645,   483,  -645,   600,  8990,
     758,  -645,  -645,  -645,  8082,  -645,  -645,  8990,   483,  -645,
    -645
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -645,  -645,  -645,  -366,   319,  -645,    15,  -645,  -645,    59,
    -105,    42,  -645,  -591,  -645,  -645,    43,   634,  -164,   -33,
     -64,  -645,   -24,   422,   -80,   848,    -1,  -645,   -27,  -645,
    -645,    13,  -645,  1043,  -645,  -645,  1733,  -298,    16,  -474,
     210,  -645,   -13,  -645,  -645,  -645,  -645,     6,   239,    14,
    -287,    31,  -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,
    -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,  -645,
    -645,  -645,  -645,  -645,    -2,  -369,  -377,   -53,  -550,  -645,
    -645,  -645,  -443,  -645,  -645,  -645,  -232,  -645,   776,  -645,
    -645,  -645,  -645,  -538,  -645,  -645,   -23,  -645,  -645,  -645,
    -645,  -645,  -645,   778,  -645,  -645,  -645,  -645,  -645,  -645,
    -645,  -645,   364,  -227,  -645,  -645,  -645,     4,  -645,    34,
    -645,    72,   857,   976,   179,  -645,  -645,    88,   306,   183,
    -645,  -644,   186,  -645,  -620,  -645,  -356,  -409,  -645,  -645,
    -645,    75,  -374,  1577,   -14,  -645,  -645,   -56,    70,   367,
      27,   806,  1547
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -526
static const yytype_int16 yytable[] =
{
      77,   247,    77,   113,   113,   548,   189,   299,   314,   244,
     419,   589,   206,   580,   231,   231,   265,   569,   231,    97,
     190,   190,   229,   229,   701,   231,   229,   361,   490,   278,
     230,   230,   206,   260,   230,   610,   112,   112,   243,   215,
     364,   190,   207,   706,   787,   523,   348,    77,   231,   231,
     -86,   279,   400,   285,   668,   670,   585,   576,   231,   224,
     224,   459,   207,   224,   464,   784,   273,   190,   788,   765,
     279,   546,   -84,   610,    91,   771,    91,   114,   114,   114,
     -91,   466,   355,   348,   112,   -87,   245,   672,   673,   -89,
     215,   235,   235,   729,   268,   235,   444,  -456,   437,   -88,
       3,   445,   296,   112,   231,   382,    77,   605,   383,   266,
     -90,   354,   387,   388,   306,   307,   -84,  -514,   243,  -514,
     -86,    91,   248,   -88,   467,   280,   429,   275,   700,   460,
     582,   -85,   354,   306,   307,   586,   261,   621,   367,   -76,
     251,   349,   -79,   -86,   280,   310,   -86,   365,   484,   242,
     577,   294,   295,   648,   290,   800,   689,   640,   692,   346,
    -450,   211,   212,  -449,   -86,   358,   292,   430,   431,   -86,
     -86,   211,   212,   357,   -76,   677,   211,   212,   349,   622,
      91,    94,   -83,    94,   115,   115,   248,   -79,   649,  -505,
    -456,   -81,   -88,  -506,   683,   -88,   471,   472,   473,   474,
     787,   347,  -450,   569,   308,  -449,    77,   231,   231,   -84,
     248,   231,   -82,   -88,   786,   231,   231,   392,   -88,   -88,
     429,   -79,   -78,   822,   882,   -80,   -79,   237,    94,   265,
     238,   774,   281,   -77,   211,   212,   644,   231,   311,   231,
     231,  -451,   556,   -79,   -79,   229,  -505,   229,   229,    77,
     112,   281,   408,   230,   353,   230,   418,   555,   556,   557,
      77,   430,   431,   668,   670,   211,   212,  -452,   380,   380,
    -454,   -91,   380,   -91,   436,   557,   380,   380,   558,   265,
      91,   315,   224,  -451,   224,   416,   697,    94,   544,   485,
      55,   394,   643,   -90,   716,  -335,   -83,  -453,    77,   231,
     231,   231,   231,    77,   231,   231,   887,   436,   231,  -452,
      77,   279,  -454,   231,   235,   470,   235,   235,   888,   352,
     475,    62,   239,    91,   482,   242,   -90,   581,   521,   482,
     526,   527,   528,   529,    91,  -335,  -285,  -335,   442,  -453,
     804,   231,  -335,   447,   112,   725,   377,   610,   445,   -82,
     231,   231,   480,  -313,   726,   520,   547,   487,   610,   342,
     380,   380,   380,   380,   -86,   476,   477,   231,   263,    77,
     231,  -444,    91,   -88,   845,   846,  -285,    91,  -285,   847,
     386,   520,  -506,  -285,    91,   280,   268,    94,   736,   812,
     521,   539,   526,   396,   532,   534,   316,   625,   395,   343,
     449,   344,   -78,   231,   399,   445,   345,   400,   -85,   842,
     215,   231,   268,  -444,   588,   588,   316,   520,   286,   617,
     289,   547,   850,  -502,    74,    77,    74,   263,   206,   401,
      94,   549,   859,   569,   359,   676,  -447,   190,   360,   407,
     -80,    94,   547,    91,   339,   340,   341,   520,   268,   412,
    -503,   414,   417,   231,   374,   368,   306,   307,   207,   409,
    -509,   421,   337,   338,   339,   340,   341,   520,   427,   422,
     423,    74,   266,   908,   410,   425,   602,   316,  -447,    94,
    -444,   -78,   883,   885,    94,   316,   -78,   619,   618,   623,
     610,    94,   281,   362,   363,   369,   375,   370,   268,    91,
     329,   330,   345,   -78,   -78,   -65,   -73,  -447,   660,   438,
     634,   635,   439,   666,   -64,   669,   671,  -509,  -502,   -80,
    -444,   453,  -444,   443,   -80,   429,  -502,  -444,   451,   662,
      74,   337,   338,   339,   340,   341,   468,   659,   231,    77,
     610,   -80,   -80,   461,   695,  -503,   469,  -447,   -86,  -447,
      94,   404,   465,  -503,  -447,  -509,   486,  -509,    77,  -509,
     429,   659,   572,  -505,  -509,   675,   430,   431,   432,   694,
     428,   -78,   573,   663,   901,   206,    77,   536,    77,   547,
     434,   263,    77,   190,   190,   754,   582,   482,   684,   584,
     231,   405,   -88,   406,   797,   700,   708,   454,   345,   380,
     590,   430,   431,   457,   520,   207,    94,   737,   738,   594,
     595,  -455,   611,    91,   809,   -80,   620,  -278,   231,   680,
     405,   624,   435,   -73,   231,   629,   633,   345,    77,   645,
      74,   263,    91,  -266,   653,   268,   596,   455,   448,   456,
     450,   391,   452,   654,   345,   661,  -286,   279,   678,   591,
      91,  -455,    91,  -455,   679,   231,    91,  -278,  -455,  -278,
     231,   231,   681,   735,  -278,   550,   685,   551,   552,   553,
     554,   429,   550,    74,   551,   552,   553,   554,   688,   690,
     112,   691,   766,  -287,    74,   740,  -286,   693,  -286,   592,
     700,   593,   852,  -286,   704,   602,   345,   710,    77,   762,
     871,   -85,    91,   776,   767,   769,   721,   222,   555,   556,
      77,    77,   430,   431,   458,   555,   556,   231,   724,   429,
      94,   280,    74,  -287,   -77,  -287,   557,    74,   731,   558,
    -287,   732,   734,   557,    74,   816,   558,   739,   741,    94,
     872,   777,   873,   817,   559,   748,   628,   345,   750,   828,
     828,   316,   780,   778,   781,    77,   790,    94,   794,    94,
     430,   431,   462,    94,   231,   801,   329,   330,   876,   803,
      77,    77,    91,    77,   805,   808,   819,   823,   380,   832,
     833,  -267,   112,   112,    91,    91,   844,   843,   828,   853,
     854,   857,    77,    74,   858,   231,   336,   337,   338,   339,
     340,   341,    77,   862,   818,   864,   821,   868,   837,    94,
     747,   301,   302,   303,   304,   305,   867,   743,   744,   824,
     745,   112,   869,   829,   829,    43,    44,   870,   281,    91,
     279,   877,   878,   879,   881,   898,   588,   907,   909,    77,
     914,   722,   918,   920,    91,    91,    77,    91,    77,    74,
     919,   922,   927,   390,   117,   350,   380,   733,   921,   547,
     351,   910,   829,   112,   192,   682,    91,   828,   782,   300,
     231,   783,   828,   828,     0,     0,    91,   848,     0,    94,
       0,     0,     0,    77,   520,     0,     0,     0,    77,     0,
       0,    94,    94,     0,     0,     0,   828,   703,     0,   705,
     112,     0,     0,   711,   280,   112,   112,     0,     0,     0,
       0,     0,     0,    91,     0,   268,     0,   828,     0,     0,
      91,     0,    91,     0,     0,   828,     0,     0,     0,   112,
     830,   830,     0,     0,     0,     0,    94,   894,     0,     0,
       0,   829,     0,     0,     0,     0,   829,   829,     0,   742,
     112,    94,    94,     0,    94,     0,     0,    91,   112,   810,
       0,     0,    91,    74,     0,     0,   813,   814,     0,   830,
     829,     0,     0,    94,     0,     0,     0,     0,    93,     0,
      93,     0,    74,    94,     0,     0,     0,     0,     0,     0,
       0,   829,   316,     0,     0,     0,     0,     0,     0,   829,
      74,     0,    74,     0,     0,     0,    74,   329,   330,     0,
       0,   281,     0,     0,     0,     0,     0,     0,     0,   793,
      94,     0,     0,     0,     0,    93,     0,    94,     0,    94,
       0,   806,   807,     0,     0,   334,   335,   336,   337,   338,
     339,   340,   341,     0,     0,     0,     0,     0,   830,     0,
       0,     0,    74,   830,   830,     0,   863,   865,     0,   226,
     226,     0,     0,   226,    94,     0,     0,     0,     0,    94,
       0,     0,     0,     0,     0,     0,   834,   830,     0,     0,
       0,     0,     0,     0,    93,     0,     0,     0,   253,   255,
       0,   839,   840,     0,   226,   226,     0,     0,   830,   891,
     892,     0,     0,   297,   298,   896,   830,     0,     0,     0,
       0,     0,     0,   851,     0,     0,     0,     0,     0,     0,
       0,     0,    74,   856,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   915,    74,    74,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   923,     0,     0,   925,     0,     0,     0,     0,
     880,     0,     0,     0,     0,     0,     0,   930,     0,   884,
       0,     0,     0,   827,   831,     0,     0,     0,     0,    74,
       0,     0,     0,     0,    93,     0,     0,     0,     0,   256,
     257,     0,     0,   258,    74,    74,     0,    74,     0,     0,
       0,     0,   162,   163,   911,   164,   165,   166,   167,   912,
     168,   169,   849,     0,   170,     0,    74,     0,   171,   172,
     173,   174,     0,     0,     0,     0,    74,    93,     0,     0,
       0,     0,   175,     0,     0,     0,     0,     0,    93,     0,
       0,     0,     0,     0,     0,     0,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,     0,     0,   186,     0,
       0,     0,     0,    74,   226,   226,   297,   187,     0,     0,
      74,     0,    74,     0,     0,     0,    93,     0,     0,     0,
       0,    93,   226,     0,   226,   226,     0,     0,    93,     0,
       0,   897,     0,     0,     0,     0,   905,   906,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    74,     0,     0,
     441,     0,    74,     0,     0,     0,     0,     0,     0,     0,
     917,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   926,     0,     0,     0,     0,     0,    93,     0,   929,
       0,     0,     0,   226,     0,     0,     0,     0,   488,   491,
     492,   493,   494,   495,   496,   497,   498,   499,   500,   501,
     502,   503,   504,   505,     0,   507,   508,   509,   510,   511,
     512,   513,   514,   515,   516,     0,   226,     0,     0,     0,
       0,     0,     0,     0,     0,   533,   535,     0,     0,     0,
       0,     0,     0,    93,     0,     0,     0,     0,     0,     0,
       0,     0,   226,     0,     0,     0,     0,     0,  -525,     0,
       0,     0,     0,     0,     0,     0,  -525,  -525,  -525,     0,
       0,  -525,  -525,  -525,     0,  -525,     0,     0,     0,     0,
       0,     0,   597,   226,   226,  -525,     0,     0,   226,     0,
     533,   535,   226,     0,  -525,  -525,   613,  -525,  -525,  -525,
    -525,  -525,     0,     0,     0,     0,     0,     0,     0,   627,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   632,
       0,     0,     0,     0,     0,     0,     0,   226,     0,     0,
     226,     0,   226,     0,     0,     0,     0,     0,   226,     0,
       0,  -525,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   646,   647,     0,     0,     0,     0,    93,     0,     0,
       0,     0,   226,     0,  -525,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    93,     0,     0,     0,
    -525,     0,     0,  -525,  -525,     0,     0,   242,     0,   656,
    -525,  -525,     0,     0,    93,     0,    93,     0,     0,     0,
      93,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
       0,    95,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    95,    95,     0,     0,    95,   272,   276,
       0,     0,     0,     0,    95,     0,    93,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   226,     0,     0,     0,
       0,     0,     0,     0,     0,   226,    95,    95,    95,     0,
       0,     0,     0,     0,     0,   226,     0,    95,     0,     0,
       0,     0,   226,     0,     0,     0,     0,     0,     0,     0,
       0,   226,     0,     0,     0,   226,     0,     0,     0,     0,
       0,     0,     0,   226,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    93,     0,     0,   749,
       0,     0,     0,    95,     0,    95,     0,     0,    93,    93,
       0,     0,     0,     0,     0,     0,   226,     0,   760,     0,
     763,   764,     0,     0,     0,   768,   770,     0,     0,     0,
       0,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,  -526,  -526,   226,     0,   329,   330,     0,     0,
       0,     0,     0,    93,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    93,    93,
     226,    93,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,     0,     0,   226,     0,   763,     0,   768,   770,
      93,     0,   226,     0,     0,   226,     0,   403,     0,     0,
      93,     0,   403,     0,     0,    95,    95,    95,     0,   420,
      95,     0,     0,     0,    95,    95,     0,     0,     0,     0,
       0,   226,     0,     0,   836,     0,     0,     0,     0,   838,
       0,     0,     0,     0,     0,     0,    95,    93,    95,    95,
       0,     0,     0,     0,    93,     0,    93,     0,    95,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
       0,     0,     0,     0,     0,     0,   226,     0,     0,     0,
       0,     0,     0,     0,   860,   838,     0,     0,   226,     0,
       0,    93,     0,   226,     0,     0,    93,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    95,    95,    95,
      95,    95,    95,    95,    95,     0,     0,    95,     0,    95,
     272,     0,    95,     0,     0,     0,     0,     0,   226,     0,
       0,     0,     0,     0,     0,   226,     0,   226,     0,     0,
       0,     0,     0,     0,     0,   226,   272,     0,     0,     0,
      95,     0,     0,     0,     0,     0,   575,     0,     0,    95,
      95,     0,     0,     0,     0,     0,   226,     0,   226,     0,
       0,     0,     0,     0,   403,   655,    95,     0,    95,    95,
       0,     0,   272,     0,   397,   398,     0,   226,     0,     0,
       0,     0,     0,     0,     0,   403,     0,   226,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,     0,    95,   329,   330,     0,     0,     0,     0,     0,
      95,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   272,     0,    95,     0,     0,   331,     0,   332,
     333,   334,   335,   336,   337,   338,   339,   340,   341,     0,
       0,     0,     0,     0,     0,   651,     0,     0,     0,  -222,
       0,     0,    95,     0,     0,     0,     0,     0,   517,   518,
       0,     0,   519,   481,     0,     0,     0,     0,   489,     0,
       0,   162,   163,     0,   164,   165,   166,   167,     0,   168,
     169,     0,     0,   170,     0,     0,     0,   171,   172,   173,
     174,     0,     0,   667,     0,   667,   667,   651,   651,     0,
       0,   175,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   667,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,     0,     0,   186,     0,     0,
     687,     0,   687,     0,   687,     0,   187,    95,    95,     0,
     699,   702,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   598,   600,     0,     0,    95,     0,   272,
       0,     0,   609,     0,     0,     0,   614,     0,     0,   403,
       0,     0,     0,     0,     0,    95,     0,    95,     0,     0,
     655,    95,     0,     0,     0,   403,     0,     0,     0,    95,
       0,     0,     0,     0,     0,     0,     0,   637,     0,     0,
     609,     0,   637,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,     0,    95,   329,   330,
       0,     0,     0,    95,     0,     0,     0,    95,     0,     0,
       0,     0,   652,     0,     0,     0,     0,     0,     0,     0,
       0,   651,   331,     0,   332,   333,   334,   335,   336,   337,
     338,   339,   340,   341,    95,     0,     0,     0,     0,    95,
      95,     0,     0,     0,     0,   792,     0,     0,   796,     0,
     702,     0,   702,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   403,     0,     0,
       0,     0,     0,     0,   403,   403,     0,    95,     0,     0,
       0,     0,     0,     0,   667,   667,     0,     0,     0,    95,
      95,     0,     0,     0,     0,     0,    95,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   696,     0,     0,     0,
       0,     0,     0,     0,     0,   489,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     687,   687,   720,     0,    95,   687,     0,     0,     0,     0,
       0,   727,     0,    95,     0,   728,     0,     0,     0,    95,
      95,     0,    95,   600,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   403,   403,     0,     0,     0,     0,
       0,    95,     0,     0,    95,     0,     0,     0,     0,     0,
       0,    95,     0,     0,     0,     0,   759,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   702,     0,     0,     0,   403,   403,     0,
       0,     0,     0,   403,   779,     0,     0,     0,    95,   272,
       0,     0,     0,     0,     0,    95,     0,    95,     0,   687,
       0,   575,     0,     0,     0,     0,     0,     0,     0,     0,
     802,   403,     0,     0,     0,     0,     0,     0,     0,    95,
       0,     0,     0,     0,   609,     0,     0,     0,   702,     0,
     403,     0,    95,   403,     0,   609,     0,    95,     0,     0,
       0,     0,     0,     0,     0,   403,     0,     5,     6,     7,
       8,     9,     0,     0,     0,    10,    11,     0,     0,     0,
      12,   835,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,   637,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,   861,     0,
       0,     0,     0,   866,     0,     0,     0,     0,    48,     0,
       0,    49,    50,     0,    51,    52,     0,    53,     0,    54,
      55,    56,    57,    58,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,     0,   886,   329,
     330,    59,    60,    61,     0,   600,     0,   609,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   386,     0,     0,
       0,     0,     0,   331,     0,   332,   333,   334,   335,   336,
     337,   338,   339,   340,   341,     0,   913,     0,   916,     0,
    -525,     4,     0,     5,     6,     7,     8,     9,     0,     0,
     248,    10,    11,     0,     0,     0,    12,   609,    13,    14,
      15,    16,    17,    18,    19,     0,     0,   928,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,    45,    46,    47,     0,     0,  -509,     0,
       0,     0,     0,     0,     0,     0,  -509,  -509,  -509,     0,
       0,     0,  -509,  -509,    48,  -509,     0,    49,    50,     0,
      51,    52,     0,    53,  -509,    54,    55,    56,    57,    58,
       0,     0,     0,     0,  -509,  -509,     0,  -509,  -509,  -509,
    -509,  -509,     0,     0,     0,     0,     0,    59,    60,    61,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -525,  -525,  -509,  -509,  -509,  -509,  -509,  -509,
    -509,  -509,  -509,  -509,  -509,  -509,  -509,     0,     0,  -509,
    -509,  -509,     0,   657,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   -87,  -509,     0,  -509,  -509,  -509,  -509,  -509,
    -509,  -509,  -509,  -509,  -509,     0,     0,     0,     0,  -509,
    -509,  -509,     0,  -509,  -509,   -79,  -285,     0,  -509,     0,
    -509,  -509,     0,     0,  -285,  -285,  -285,     0,     0,     0,
    -285,  -285,     0,  -285,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -285,  -285,     0,  -285,  -285,  -285,  -285,  -285,
       0,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,     0,     0,   329,   330,     0,     0,
       0,  -222,  -285,  -285,  -285,  -285,  -285,  -285,  -285,  -285,
    -285,  -285,  -285,  -285,  -285,     0,     0,  -285,  -285,  -285,
     331,   658,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,     0,     0,     0,     0,     0,     0,     0,     0,
     -89,  -285,  -222,  -285,  -285,  -285,  -285,  -285,  -285,  -285,
    -285,  -285,  -285,     0,     0,     0,     0,     0,  -285,  -285,
       0,  -285,  -285,   -81,     0,     0,  -285,     0,  -285,  -285,
       4,     0,     5,     6,     7,     8,     9,  -525,  -525,  -525,
      10,    11,     0,     0,  -525,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,     0,     0,    49,    50,     0,    51,
      52,     0,    53,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    59,    60,    61,     0,
       0,     0,     0,     0,     4,     0,     5,     6,     7,     8,
       9,  -525,  -525,  -525,    10,    11,     0,  -525,  -525,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,    43,    44,     0,    45,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    48,     0,     0,
      49,    50,     0,    51,    52,     0,    53,     0,    54,    55,
      56,    57,    58,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      59,    60,    61,     4,     0,     5,     6,     7,     8,     9,
       0,     0,  -525,    10,    11,  -525,  -525,  -525,    12,  -525,
      13,    14,    15,    16,    17,    18,    19,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     0,    43,    44,     0,    45,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,     0,     0,    49,
      50,     0,    51,    52,     0,    53,     0,    54,    55,    56,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    59,
      60,    61,     4,     0,     5,     6,     7,     8,     9,     0,
       0,  -525,    10,    11,  -525,  -525,  -525,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
      20,    21,    22,    23,    24,    25,    26,     0,     0,    27,
       0,     0,     0,     0,     0,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,    43,    44,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    48,     0,     0,    49,    50,
       0,    51,    52,     0,    53,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    59,    60,
      61,     4,     0,     5,     6,     7,     8,     9,     0,  -525,
    -525,    10,    11,  -525,  -525,     0,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,     0,    49,    50,     0,
      51,    52,     0,    53,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    59,    60,    61,
       4,     0,     5,     6,     7,     8,     9,     0,     0,     0,
      10,    11,  -525,  -525,     0,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,     0,     0,   262,    50,     0,    51,
      52,     0,    53,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    59,    60,    61,     0,
       0,     0,     0,     0,     4,  -525,     5,     6,     7,     8,
       9,  -525,  -525,     0,    10,    11,     0,     0,     0,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,    43,    44,     0,    45,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    48,     0,     0,
      49,    50,     0,    51,    52,     0,    53,     0,    54,    55,
      56,    57,    58,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      59,    60,    61,     0,     0,     0,     0,     0,     4,  -525,
       5,     6,     7,     8,     9,  -525,  -525,     0,    10,    11,
       0,     0,     0,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,    43,    44,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    48,     0,     0,    49,    50,     0,    51,    52,     0,
      53,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    59,    60,    61,     0,     0,  -525,
       0,     0,     4,     0,     5,     6,     7,     8,     9,  -525,
    -525,  -525,    10,    11,     0,     0,     0,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
      20,    21,    22,    23,    24,    25,    26,     0,     0,    27,
       0,     0,     0,     0,     0,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,    43,    44,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    48,     0,     0,    49,    50,
       0,    51,    52,     0,    53,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    59,    60,
      61,     0,     0,     0,     0,   118,   119,   120,   121,   122,
     123,   124,   125,  -525,  -525,   126,   127,   128,   129,   130,
       0,     0,   131,   132,   133,   134,   135,   136,   137,     0,
       0,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,    36,    37,   161,    39,     0,     0,
       0,     0,     0,     0,     0,   162,   163,     0,   164,   165,
     166,   167,     0,   168,   169,     0,     0,   170,     0,     0,
       0,   171,   172,   173,   174,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   175,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,     0,
       0,   186,     0,     0,  -502,  -502,  -502,     0,  -502,     0,
     187,   188,  -502,  -502,     0,     0,     0,  -502,     0,  -502,
    -502,  -502,  -502,  -502,  -502,  -502,     0,  -502,     0,     0,
    -502,  -502,  -502,  -502,  -502,  -502,  -502,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -502,     0,     0,  -502,
    -502,  -502,  -502,  -502,  -502,  -502,  -502,  -502,  -502,  -502,
       0,  -502,  -502,     0,  -502,  -502,  -502,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -502,     0,     0,  -502,  -502,
       0,  -502,  -502,     0,  -502,  -502,  -502,  -502,  -502,  -502,
    -502,     0,     0,     0,     0,     0,     0,     0,  -504,  -504,
    -504,     0,  -504,     0,     0,     0,  -504,  -504,  -502,  -502,
    -502,  -504,  -502,  -504,  -504,  -504,  -504,  -504,  -504,  -504,
    -502,  -504,     0,     0,  -504,  -504,  -504,  -504,  -504,  -504,
    -504,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -504,     0,     0,  -504,  -504,  -504,  -504,  -504,  -504,  -504,
    -504,  -504,  -504,  -504,     0,  -504,  -504,     0,  -504,  -504,
    -504,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -504,
       0,     0,  -504,  -504,     0,  -504,  -504,     0,  -504,  -504,
    -504,  -504,  -504,  -504,  -504,     0,     0,     0,     0,     0,
       0,     0,  -503,  -503,  -503,     0,  -503,     0,     0,     0,
    -503,  -503,  -504,  -504,  -504,  -503,  -504,  -503,  -503,  -503,
    -503,  -503,  -503,  -503,  -504,  -503,     0,     0,  -503,  -503,
    -503,  -503,  -503,  -503,  -503,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -503,     0,     0,  -503,  -503,  -503,
    -503,  -503,  -503,  -503,  -503,  -503,  -503,  -503,     0,  -503,
    -503,     0,  -503,  -503,  -503,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -503,     0,     0,  -503,  -503,     0,  -503,
    -503,     0,  -503,  -503,  -503,  -503,  -503,  -503,  -503,     0,
       0,     0,     0,     0,     0,     0,  -505,  -505,  -505,     0,
    -505,     0,     0,     0,  -505,  -505,  -503,  -503,  -503,  -505,
    -503,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -503,     0,
       0,     0,  -505,  -505,  -505,  -505,  -505,  -505,  -505,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -505,     0,
       0,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,     0,  -505,  -505,     0,  -505,  -505,  -505,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -505,   723,     0,
    -505,  -505,     0,  -505,  -505,     0,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,     0,     0,     0,     0,   -87,     0,     0,
    -506,  -506,  -506,     0,  -506,     0,     0,     0,  -506,  -506,
    -505,  -505,  -505,  -506,     0,  -506,  -506,  -506,  -506,  -506,
    -506,  -506,  -505,     0,     0,     0,  -506,  -506,  -506,  -506,
    -506,  -506,  -506,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -506,     0,     0,  -506,  -506,  -506,  -506,  -506,
    -506,  -506,  -506,  -506,  -506,  -506,     0,  -506,  -506,     0,
    -506,  -506,  -506,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -506,   658,     0,  -506,  -506,     0,  -506,  -506,     0,
    -506,  -506,  -506,  -506,  -506,  -506,  -506,     0,     0,     0,
       0,   -89,     0,     0,  -256,  -256,  -256,     0,  -256,     0,
       0,     0,  -256,  -256,  -506,  -506,  -506,  -256,     0,  -256,
    -256,  -256,  -256,  -256,  -256,  -256,  -506,     0,     0,     0,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -256,     0,     0,  -256,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
       0,  -256,  -256,     0,  -256,  -256,  -256,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -256,     0,     0,  -256,  -256,
       0,  -256,  -256,     0,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,     0,     0,     0,     0,     0,     0,     0,  -256,  -256,
    -256,     0,  -256,     0,     0,     0,  -256,  -256,  -256,  -256,
    -256,  -256,     0,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
     239,     0,     0,     0,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -256,     0,     0,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,     0,  -256,  -256,     0,  -256,  -256,
    -256,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -256,
       0,     0,  -256,  -256,     0,  -256,  -256,     0,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,     0,     0,     0,     0,     0,
       0,     0,  -507,  -507,  -507,     0,  -507,     0,     0,     0,
    -507,  -507,  -256,  -256,  -256,  -507,     0,  -507,  -507,  -507,
    -507,  -507,  -507,  -507,   242,     0,     0,     0,  -507,  -507,
    -507,  -507,  -507,  -507,  -507,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -507,     0,     0,  -507,  -507,  -507,
    -507,  -507,  -507,  -507,  -507,  -507,  -507,  -507,     0,  -507,
    -507,     0,  -507,  -507,  -507,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -507,     0,     0,  -507,  -507,     0,  -507,
    -507,     0,  -507,  -507,  -507,  -507,  -507,  -507,  -507,     0,
       0,     0,     0,     0,     0,     0,  -508,  -508,  -508,     0,
    -508,     0,     0,     0,  -508,  -508,  -507,  -507,  -507,  -508,
       0,  -508,  -508,  -508,  -508,  -508,  -508,  -508,  -507,     0,
       0,     0,  -508,  -508,  -508,  -508,  -508,  -508,  -508,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -508,     0,
       0,  -508,  -508,  -508,  -508,  -508,  -508,  -508,  -508,  -508,
    -508,  -508,     0,  -508,  -508,     0,  -508,  -508,  -508,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -508,     0,     0,
    -508,  -508,     0,  -508,  -508,     0,  -508,  -508,  -508,  -508,
    -508,  -508,  -508,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -508,  -508,  -508,   118,   119,   120,   121,   122,   123,   124,
     125,     0,  -508,   126,   127,   128,   129,   130,     0,     0,
     131,   132,   133,   134,   135,   136,   137,     0,     0,   138,
     139,   140,   194,   195,   196,   197,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   198,   199,
     200,   160,   282,   283,   201,   284,     0,     0,     0,     0,
       0,     0,     0,   162,   163,     0,   164,   165,   166,   167,
       0,   168,   169,     0,     0,   170,     0,     0,     0,   171,
     172,   173,   174,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,     0,     0,   186,
     118,   119,   120,   121,   122,   123,   124,   125,   187,     0,
     126,   127,   128,   129,   130,     0,     0,   131,   132,   133,
     134,   135,   136,   137,     0,     0,   138,   139,   140,   194,
     195,   196,   197,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   198,   199,   200,   160,   246,
       0,   201,     0,     0,     0,     0,     0,     0,     0,     0,
     162,   163,     0,   164,   165,   166,   167,     0,   168,   169,
       0,     0,   170,     0,     0,     0,   171,   172,   173,   174,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     175,     0,    54,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,     0,     0,   186,   118,   119,   120,
     121,   122,   123,   124,   125,   187,     0,   126,   127,   128,
     129,   130,     0,     0,   131,   132,   133,   134,   135,   136,
     137,     0,     0,   138,   139,   140,   194,   195,   196,   197,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   198,   199,   200,   160,     0,     0,   201,     0,
       0,     0,     0,     0,     0,     0,     0,   162,   163,     0,
     164,   165,   166,   167,     0,   168,   169,     0,     0,   170,
       0,     0,     0,   171,   172,   173,   174,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   175,     0,    54,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,     0,     0,   186,   118,   119,   120,   121,   122,   123,
     124,   125,   187,     0,   126,   127,   128,   129,   130,     0,
       0,   131,   132,   133,   134,   135,   136,   137,     0,     0,
     138,   139,   140,   194,   195,   196,   197,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   198,
     199,   200,   160,     0,     0,   201,     0,     0,     0,     0,
       0,     0,     0,     0,   162,   163,     0,   164,   165,   166,
     167,     0,   168,   169,     0,     0,   170,     0,     0,     0,
     171,   172,   173,   174,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   175,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,     0,     0,
     186,     0,     5,     6,     7,     0,     9,     0,     0,   187,
      10,    11,     0,     0,     0,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   825,     0,     0,   108,    50,     0,    51,
      52,     0,     0,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   109,    13,    14,    15,
      98,    99,    18,    19,     0,     0,   826,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   825,     0,     0,   108,    50,     0,    51,
      52,     0,     0,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   109,    13,    14,    15,
      98,    99,    18,    19,     0,     0,   904,     0,   100,    21,
      22,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   220,     0,     0,   108,    50,     0,    51,
      52,     0,   221,   222,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     8,     9,    59,   223,    61,    10,
      11,     0,     0,     0,    12,   411,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,    27,     0,     0,     0,
       0,     0,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,     0,     0,    49,    50,     0,    51,    52,
       0,    53,     0,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    59,    60,    61,    12,     0,
      13,    14,    15,    16,    17,    18,    19,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     0,    43,    44,     0,    45,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   220,     0,     0,   108,
      50,     0,    51,    52,     0,   612,   222,    54,    55,    56,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    59,
      60,    61,    12,     0,    13,    14,    15,    98,    99,    18,
      19,     0,     0,     0,     0,   100,    21,    22,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,    43,    44,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     220,     0,     0,   108,    50,     0,    51,    52,     0,   221,
     222,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   223,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
      21,    22,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   220,     0,     0,   108,   413,     0,
      51,    52,     0,   221,   222,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   223,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   220,     0,
       0,   108,    50,     0,    51,    52,     0,   599,   222,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    59,   223,    61,    12,     0,    13,    14,    15,    98,
      99,    18,    19,     0,     0,     0,     0,   100,   101,   102,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   220,     0,     0,   108,    50,     0,    51,    52,
       0,   608,   222,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    59,   223,    61,    12,     0,
      13,    14,    15,    98,    99,    18,    19,     0,     0,     0,
       0,   100,    21,    22,    23,    24,    25,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     0,    43,    44,     0,    45,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   220,     0,     0,   108,
      50,     0,    51,    52,     0,   599,   222,    54,    55,    56,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    59,
     223,    61,    12,     0,    13,    14,    15,    98,    99,    18,
      19,     0,     0,     0,     0,   100,   101,   102,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,    43,    44,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     220,     0,     0,   108,    50,     0,    51,    52,     0,   811,
     222,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   223,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   220,     0,     0,   108,    50,     0,
      51,    52,     0,   815,   222,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   223,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   220,     0,
       0,   108,    50,     0,    51,    52,     0,   820,   222,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    59,   223,    61,    12,     0,    13,    14,    15,    98,
      99,    18,    19,     0,     0,     0,     0,   100,   101,   102,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   220,     0,     0,   108,    50,     0,    51,    52,
       0,   893,   222,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    59,   223,    61,    12,     0,
      13,    14,    15,    98,    99,    18,    19,     0,     0,     0,
       0,   100,   101,   102,    23,    24,    25,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     0,    43,    44,     0,    45,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   220,     0,     0,   108,
      50,     0,    51,    52,     0,   895,   222,    54,    55,    56,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    59,
     223,    61,    12,     0,    13,    14,    15,    98,    99,    18,
      19,     0,     0,     0,     0,   100,   101,   102,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,    43,    44,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     220,     0,     0,   108,    50,     0,    51,    52,     0,   924,
     222,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   223,    61,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   220,     0,     0,   108,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,    60,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,    21,    22,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   220,     0,
       0,   108,    50,     0,    51,    52,     0,   267,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    59,   223,    61,    12,     0,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   220,     0,     0,   108,    50,     0,    51,    52,
       0,   479,     0,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    59,   223,    61,    12,     0,
      13,    14,    15,    98,    99,    18,    19,     0,     0,     0,
       0,   100,   101,   102,    23,    24,    25,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     0,    43,    44,     0,    45,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   220,     0,     0,   108,
      50,     0,    51,    52,     0,   636,     0,    54,    55,    56,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    59,
     223,    61,    12,     0,    13,    14,    15,    98,    99,    18,
      19,     0,     0,     0,     0,   100,   101,   102,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,    43,    44,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     220,     0,     0,   108,    50,     0,    51,    52,     0,   479,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   223,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   220,     0,     0,   108,    50,     0,
      51,    52,     0,   707,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   223,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   220,     0,
       0,   108,    50,     0,    51,    52,     0,   758,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    59,   223,    61,    12,     0,    13,    14,    15,    98,
      99,    18,    19,     0,     0,     0,     0,   100,   101,   102,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   220,     0,     0,   108,    50,     0,    51,    52,
       0,   855,     0,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    59,   223,    61,    12,     0,
      13,    14,    15,    98,    99,    18,    19,     0,     0,     0,
       0,   100,   101,   102,    23,    24,    25,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     0,    43,    44,     0,    45,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   220,     0,     0,   108,
      50,     0,    51,    52,     0,     0,     0,    54,    55,    56,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    59,
     223,    61,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,    43,    44,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     220,     0,     0,   108,    50,     0,    51,    52,     0,     0,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   223,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   103,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   277,     0,     0,   312,    50,     0,
      51,    52,     0,   751,   752,    54,    55,    56,    57,    58,
       0,     0,     0,     5,     6,     7,     0,     9,     0,   753,
       0,    10,    11,     0,     0,     0,    12,   109,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   103,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   277,     0,     0,   312,    50,     0,
      51,    52,     0,   874,   875,    54,    55,    56,    57,    58,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,     0,     0,     0,    12,   109,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   103,     0,     0,    32,    33,
     104,    35,    36,    37,   105,    39,    40,    41,    42,     0,
      43,    44,     0,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   106,     0,     0,   107,     0,     0,   108,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,     0,     0,     0,    12,   109,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   103,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   277,     0,     0,   312,    50,     0,
      51,    52,     0,   313,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,     0,     0,     0,    12,   109,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   103,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   389,     0,     0,    49,    50,     0,
      51,    52,     0,    53,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,     0,     0,     0,    12,   109,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   103,     0,     0,    32,    33,
     104,    35,    36,    37,   105,    39,    40,    41,    42,     0,
      43,    44,     0,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   107,     0,     0,   108,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,     0,     0,     0,    12,   109,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   103,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   277,     0,     0,   108,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,     0,     0,     0,    12,   109,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   103,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   825,     0,     0,   108,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
       0,     0,   524,   257,     0,     0,   525,     0,     0,     0,
       0,     0,     0,     0,     0,   162,   163,   109,   164,   165,
     166,   167,     0,   168,   169,     0,     0,   170,     0,     0,
       0,   171,   172,   173,   174,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   175,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   542,
     518,   186,     0,   543,     0,     0,     0,     0,     0,     0,
     187,     0,   162,   163,     0,   164,   165,   166,   167,     0,
     168,   169,     0,     0,   170,     0,     0,     0,   171,   172,
     173,   174,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   603,   518,   186,     0,
     604,     0,     0,     0,     0,     0,     0,   187,     0,   162,
     163,     0,   164,   165,   166,   167,     0,   168,   169,     0,
       0,   170,     0,     0,     0,   171,   172,   173,   174,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   175,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   606,   257,   186,     0,   607,     0,     0,
       0,     0,     0,     0,   187,     0,   162,   163,     0,   164,
     165,   166,   167,     0,   168,   169,     0,     0,   170,     0,
       0,     0,   171,   172,   173,   174,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   175,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     638,   518,   186,     0,   639,     0,     0,     0,     0,     0,
       0,   187,     0,   162,   163,     0,   164,   165,   166,   167,
       0,   168,   169,     0,     0,   170,     0,     0,     0,   171,
     172,   173,   174,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   641,   257,   186,
       0,   642,     0,     0,     0,     0,     0,     0,   187,     0,
     162,   163,     0,   164,   165,   166,   167,     0,   168,   169,
       0,     0,   170,     0,     0,     0,   171,   172,   173,   174,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     175,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   714,   518,   186,     0,   715,     0,
       0,     0,     0,     0,     0,   187,     0,   162,   163,     0,
     164,   165,   166,   167,     0,   168,   169,     0,     0,   170,
       0,     0,     0,   171,   172,   173,   174,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   175,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   717,   257,   186,     0,   718,     0,     0,     0,     0,
       0,     0,   187,     0,   162,   163,     0,   164,   165,   166,
     167,     0,   168,   169,     0,     0,   170,     0,     0,     0,
     171,   172,   173,   174,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   175,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   899,   518,
     186,     0,   900,     0,     0,     0,     0,     0,     0,   187,
       0,   162,   163,     0,   164,   165,   166,   167,     0,   168,
     169,     0,     0,   170,     0,     0,     0,   171,   172,   173,
     174,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   175,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   902,   257,   186,     0,   903,
       0,     0,     0,     0,     0,     0,   187,     0,   162,   163,
       0,   164,   165,   166,   167,     0,   168,   169,     0,     0,
     170,     0,     0,     0,   171,   172,   173,   174,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   175,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,     0,     0,   186,     0,     0,     0,     0,     0,
       0,     0,     0,   187,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,     0,     0,   329,
     330,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   331,     0,   332,   333,   334,   335,   336,
     337,   338,   339,   340,   341,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   595,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,     0,
       0,   329,   330,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,     0,     0,   329,   330,
       0,     0,     0,     0,     0,   331,   761,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,     0,     0,     0,
       0,     0,   331,     0,   332,   333,   334,   335,   336,   337,
     338,   339,   340,   341,   316,   317,   318,   319,   320,   321,
     322,   323,     0,   325,   326,     0,     0,     0,     0,   329,
     330,   316,  -526,  -526,  -526,  -526,   321,   322,     0,     0,
    -526,  -526,     0,     0,     0,     0,   329,   330,     0,     0,
       0,     0,     0,     0,     0,   332,   333,   334,   335,   336,
     337,   338,   339,   340,   341,     0,     0,     0,     0,     0,
       0,     0,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   316,   317,   318,   319,   320,   321,   322,     0,
       0,   325,   326,     0,     0,     0,     0,   329,   330,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341
};

static const yytype_int16 yycheck[] =
{
       2,    28,     4,     5,     6,   371,     7,    63,    72,    22,
     242,   388,     8,   382,    16,    17,    49,   373,    20,     4,
       7,     8,    16,    17,   574,    27,    20,   107,   315,    53,
      16,    17,    28,    47,    20,   409,     5,     6,    22,    12,
       1,    28,     8,   581,   688,   343,    26,    49,    50,    51,
      13,    53,    86,    54,   528,   529,    27,    13,    60,    16,
      17,   288,    28,    20,   291,   685,    52,    54,   688,   660,
      72,   369,    25,   447,     2,   666,     4,     5,     6,     7,
      25,    59,    95,    26,    53,    25,    27,   530,   531,    25,
      63,    16,    17,   127,    51,    20,   127,    85,   262,    13,
       0,   132,    60,    72,   106,   210,   108,   405,   213,    50,
      25,    95,   217,   218,    36,    37,   104,    15,   102,    17,
      25,    49,   132,    25,   102,    53,    59,    52,    15,    62,
      17,    25,   116,    36,    37,   106,    54,    83,   111,   127,
     121,   121,    25,   106,    72,   104,   109,   108,   312,   129,
     106,    57,    58,    74,   131,   705,   565,   455,   567,    83,
      83,   132,   133,    83,   127,   106,   131,   100,   101,   132,
     133,   132,   133,   103,   127,   541,   132,   133,   121,   125,
     108,     2,   127,     4,     5,     6,   132,   127,   109,   129,
      85,   127,   106,   129,   560,   109,   301,   302,   303,   304,
     844,   125,   125,   559,    28,   125,   208,   209,   210,   104,
     132,   213,   127,   127,    50,   217,   218,   219,   132,   133,
      59,   104,   127,   126,   844,   127,   109,    17,    49,   262,
      20,   674,    53,   127,   132,   133,   463,   239,   127,   241,
     242,    83,    94,   126,   127,   239,   129,   241,   242,   251,
     219,    72,    85,   239,    85,   241,   242,    93,    94,   111,
     262,   100,   101,   737,   738,   132,   133,    83,   209,   210,
      83,   104,   213,   104,    85,   111,   217,   218,   114,   312,
     208,   104,   239,   125,   241,   242,   573,   108,   368,   313,
      96,   219,   131,   104,   592,    83,   127,    83,   300,   301,
     302,   303,   304,   305,   306,   307,   856,    85,   310,   125,
     312,   313,   125,   315,   239,   300,   241,   242,   856,    85,
     305,     2,   129,   251,   310,   129,   104,   383,   342,   315,
     344,   345,   346,   347,   262,   123,    83,   125,   268,   125,
     709,   343,   130,   127,   313,    85,   127,   721,   132,   127,
     352,   353,   310,    15,    85,   342,   370,   315,   732,    83,
     301,   302,   303,   304,   104,   306,   307,   369,    49,   371,
     372,    83,   300,   104,   783,   784,   123,   305,   125,   788,
     133,   368,   129,   130,   312,   313,   343,   208,   620,    85,
     404,   364,   406,    54,   352,   353,    66,   424,   219,   123,
     127,   125,    25,   405,   104,   132,   130,    86,   104,   775,
     383,   413,   369,   125,   387,   388,    66,   404,    54,   413,
      56,   435,   791,    26,     2,   427,     4,   108,   424,   127,
     251,   372,   809,   789,    50,   540,    83,   424,    54,    85,
      25,   262,   456,   371,   114,   115,   116,   434,   405,   239,
      26,   241,   242,   455,    83,    83,    36,    37,   424,   127,
      26,    52,   112,   113,   114,   115,   116,   454,   121,    60,
      61,    49,   413,   882,   235,   129,   401,    66,   125,   300,
      83,   104,   848,   852,   305,    66,   109,   417,   413,   419,
     864,   312,   313,    57,    58,   123,   125,   125,   455,   427,
      81,    82,   130,   126,   127,   126,   127,    83,   521,   126,
     440,   441,   126,   526,   126,   528,   529,    83,   121,   104,
     123,    54,   125,   124,   109,    59,   129,   130,   122,    85,
     108,   112,   113,   114,   115,   116,    66,   521,   540,   541,
     914,   126,   127,   102,   571,   121,    66,   123,   104,   125,
     371,    83,   102,   129,   130,   121,   127,   123,   560,   125,
      59,   545,    10,   129,   130,   538,   100,   101,   102,   570,
     251,   127,     8,    85,   872,   571,   578,    92,   580,   593,
      83,   262,   584,   570,   571,   649,    17,   573,   561,    15,
     592,   123,   104,   125,    14,    15,   582,    83,   130,   540,
      54,   100,   101,   102,   591,   571,   427,   621,   622,    25,
     127,    83,   126,   541,   719,   127,   127,    83,   620,   549,
     123,   126,   125,   127,   626,   122,   126,   130,   630,   131,
     208,   312,   560,   127,   127,   592,   397,   123,   271,   125,
     273,   219,   275,   126,   130,   124,    83,   649,   124,    83,
     578,   123,   580,   125,    10,   657,   584,   123,   130,   125,
     662,   663,   104,   620,   130,    50,   127,    52,    53,    54,
      55,    59,    50,   251,    52,    53,    54,    55,   127,    50,
     649,   127,    85,    83,   262,   626,   123,    50,   125,   123,
      15,   125,   797,   130,    13,   620,   130,    10,   700,   657,
      83,   104,   630,   676,   662,   663,   127,    94,    93,    94,
     712,   713,   100,   101,   102,    93,    94,   719,   124,    59,
     541,   649,   300,   123,   127,   125,   111,   305,   126,   114,
     130,   127,   126,   111,   312,   729,   114,   126,   122,   560,
     123,    10,   125,   729,   129,   126,   427,   130,   124,   751,
     752,    66,   682,   126,    10,   757,    86,   578,     9,   580,
     100,   101,   102,   584,   766,    10,    81,    82,   832,   127,
     772,   773,   700,   775,    10,   124,   126,   122,   719,   127,
     109,   127,   751,   752,   712,   713,   127,   126,   790,    10,
      10,    10,   794,   371,    10,   797,   111,   112,   113,   114,
     115,   116,   804,   127,   729,   127,   736,    54,   766,   630,
     631,    39,    40,    41,    42,    43,   126,    52,    53,   749,
      55,   790,    94,   751,   752,    60,    61,   127,   649,   757,
     832,    10,    10,   122,    10,    94,   809,   122,    10,   841,
     127,   602,   124,   127,   772,   773,   848,   775,   850,   427,
      94,    10,    94,   219,     6,    79,   797,   618,   911,   873,
      82,   884,   790,   832,     7,   559,   794,   869,   685,    63,
     872,   685,   874,   875,    -1,    -1,   804,   789,    -1,   700,
      -1,    -1,    -1,   885,   871,    -1,    -1,    -1,   890,    -1,
      -1,   712,   713,    -1,    -1,    -1,   898,   578,    -1,   580,
     869,    -1,    -1,   584,   832,   874,   875,    -1,    -1,    -1,
      -1,    -1,    -1,   841,    -1,   872,    -1,   919,    -1,    -1,
     848,    -1,   850,    -1,    -1,   927,    -1,    -1,    -1,   898,
     751,   752,    -1,    -1,    -1,    -1,   757,   862,    -1,    -1,
      -1,   869,    -1,    -1,    -1,    -1,   874,   875,    -1,   630,
     919,   772,   773,    -1,   775,    -1,    -1,   885,   927,   720,
      -1,    -1,   890,   541,    -1,    -1,   727,   728,    -1,   790,
     898,    -1,    -1,   794,    -1,    -1,    -1,    -1,     2,    -1,
       4,    -1,   560,   804,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   919,    66,    -1,    -1,    -1,    -1,    -1,    -1,   927,
     578,    -1,   580,    -1,    -1,    -1,   584,    81,    82,    -1,
      -1,   832,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   700,
     841,    -1,    -1,    -1,    -1,    49,    -1,   848,    -1,   850,
      -1,   712,   713,    -1,    -1,   109,   110,   111,   112,   113,
     114,   115,   116,    -1,    -1,    -1,    -1,    -1,   869,    -1,
      -1,    -1,   630,   874,   875,    -1,   817,   818,    -1,    16,
      17,    -1,    -1,    20,   885,    -1,    -1,    -1,    -1,   890,
      -1,    -1,    -1,    -1,    -1,    -1,   757,   898,    -1,    -1,
      -1,    -1,    -1,    -1,   108,    -1,    -1,    -1,    45,    46,
      -1,   772,   773,    -1,    51,    52,    -1,    -1,   919,   860,
     861,    -1,    -1,    60,    61,   866,   927,    -1,    -1,    -1,
      -1,    -1,    -1,   794,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   700,   804,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   894,   712,   713,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   913,    -1,    -1,   916,    -1,    -1,    -1,    -1,
     841,    -1,    -1,    -1,    -1,    -1,    -1,   928,    -1,   850,
      -1,    -1,    -1,   751,   752,    -1,    -1,    -1,    -1,   757,
      -1,    -1,    -1,    -1,   208,    -1,    -1,    -1,    -1,    50,
      51,    -1,    -1,    54,   772,   773,    -1,   775,    -1,    -1,
      -1,    -1,    63,    64,   885,    66,    67,    68,    69,   890,
      71,    72,   790,    -1,    75,    -1,   794,    -1,    79,    80,
      81,    82,    -1,    -1,    -1,    -1,   804,   251,    -1,    -1,
      -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,   262,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,    -1,    -1,   119,    -1,
      -1,    -1,    -1,   841,   221,   222,   223,   128,    -1,    -1,
     848,    -1,   850,    -1,    -1,    -1,   300,    -1,    -1,    -1,
      -1,   305,   239,    -1,   241,   242,    -1,    -1,   312,    -1,
      -1,   869,    -1,    -1,    -1,    -1,   874,   875,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   885,    -1,    -1,
     267,    -1,   890,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     898,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   919,    -1,    -1,    -1,    -1,    -1,   371,    -1,   927,
      -1,    -1,    -1,   310,    -1,    -1,    -1,    -1,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,   330,    -1,   332,   333,   334,   335,   336,
     337,   338,   339,   340,   341,    -1,   343,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   352,   353,    -1,    -1,    -1,
      -1,    -1,    -1,   427,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   369,    -1,    -1,    -1,    -1,    -1,     0,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     8,     9,    10,    -1,
      -1,    13,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,
      -1,    -1,   399,   400,   401,    27,    -1,    -1,   405,    -1,
     407,   408,   409,    -1,    36,    37,   413,    39,    40,    41,
      42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   426,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   436,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   444,    -1,    -1,
     447,    -1,   449,    -1,    -1,    -1,    -1,    -1,   455,    -1,
      -1,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   468,   469,    -1,    -1,    -1,    -1,   541,    -1,    -1,
      -1,    -1,   479,    -1,   106,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   560,    -1,    -1,    -1,
     122,    -1,    -1,   125,   126,    -1,    -1,   129,    -1,   506,
     132,   133,    -1,    -1,   578,    -1,   580,    -1,    -1,    -1,
     584,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     2,
      -1,     4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    16,    17,    -1,    -1,    20,    51,    52,
      -1,    -1,    -1,    -1,    27,    -1,   630,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   573,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   582,    49,    50,    51,    -1,
      -1,    -1,    -1,    -1,    -1,   592,    -1,    60,    -1,    -1,
      -1,    -1,   599,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   608,    -1,    -1,    -1,   612,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   620,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   700,    -1,    -1,   636,
      -1,    -1,    -1,   106,    -1,   108,    -1,    -1,   712,   713,
      -1,    -1,    -1,    -1,    -1,    -1,   653,    -1,   655,    -1,
     657,   658,    -1,    -1,    -1,   662,   663,    -1,    -1,    -1,
      -1,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,   681,    -1,    81,    82,    -1,    -1,
      -1,    -1,    -1,   757,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   772,   773,
     707,   775,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,    -1,    -1,   721,    -1,   723,    -1,   725,   726,
     794,    -1,   729,    -1,    -1,   732,    -1,   230,    -1,    -1,
     804,    -1,   235,    -1,    -1,   208,   209,   210,    -1,   242,
     213,    -1,    -1,    -1,   217,   218,    -1,    -1,    -1,    -1,
      -1,   758,    -1,    -1,   761,    -1,    -1,    -1,    -1,   766,
      -1,    -1,    -1,    -1,    -1,    -1,   239,   841,   241,   242,
      -1,    -1,    -1,    -1,   848,    -1,   850,    -1,   251,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   262,
      -1,    -1,    -1,    -1,    -1,    -1,   803,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   811,   812,    -1,    -1,   815,    -1,
      -1,   885,    -1,   820,    -1,    -1,   890,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   300,   301,   302,
     303,   304,   305,   306,   307,    -1,    -1,   310,    -1,   312,
     343,    -1,   315,    -1,    -1,    -1,    -1,    -1,   855,    -1,
      -1,    -1,    -1,    -1,    -1,   862,    -1,   864,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   872,   369,    -1,    -1,    -1,
     343,    -1,    -1,    -1,    -1,    -1,   379,    -1,    -1,   352,
     353,    -1,    -1,    -1,    -1,    -1,   893,    -1,   895,    -1,
      -1,    -1,    -1,    -1,   397,    43,   369,    -1,   371,   372,
      -1,    -1,   405,    -1,   221,   222,    -1,   914,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   418,    -1,   924,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    -1,   405,    81,    82,    -1,    -1,    -1,    -1,    -1,
     413,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   455,    -1,   427,    -1,    -1,   105,    -1,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,    -1,
      -1,    -1,    -1,    -1,    -1,   478,    -1,    -1,    -1,   127,
      -1,    -1,   455,    -1,    -1,    -1,    -1,    -1,    50,    51,
      -1,    -1,    54,   310,    -1,    -1,    -1,    -1,   315,    -1,
      -1,    63,    64,    -1,    66,    67,    68,    69,    -1,    71,
      72,    -1,    -1,    75,    -1,    -1,    -1,    79,    80,    81,
      82,    -1,    -1,   526,    -1,   528,   529,   530,   531,    -1,
      -1,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   547,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,    -1,    -1,   119,    -1,    -1,
     563,    -1,   565,    -1,   567,    -1,   128,   540,   541,    -1,
     573,   574,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   400,   401,    -1,    -1,   560,    -1,   592,
      -1,    -1,   409,    -1,    -1,    -1,   413,    -1,    -1,   602,
      -1,    -1,    -1,    -1,    -1,   578,    -1,   580,    -1,    -1,
      43,   584,    -1,    -1,    -1,   618,    -1,    -1,    -1,   592,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   444,    -1,    -1,
     447,    -1,   449,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    -1,   620,    81,    82,
      -1,    -1,    -1,   626,    -1,    -1,    -1,   630,    -1,    -1,
      -1,    -1,   479,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   674,   105,    -1,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   657,    -1,    -1,    -1,    -1,   662,
     663,    -1,    -1,    -1,    -1,   698,    -1,    -1,   701,    -1,
     703,    -1,   705,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   720,    -1,    -1,
      -1,    -1,    -1,    -1,   727,   728,    -1,   700,    -1,    -1,
      -1,    -1,    -1,    -1,   737,   738,    -1,    -1,    -1,   712,
     713,    -1,    -1,    -1,    -1,    -1,   719,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   573,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   582,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     783,   784,   599,    -1,   757,   788,    -1,    -1,    -1,    -1,
      -1,   608,    -1,   766,    -1,   612,    -1,    -1,    -1,   772,
     773,    -1,   775,   620,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   817,   818,    -1,    -1,    -1,    -1,
      -1,   794,    -1,    -1,   797,    -1,    -1,    -1,    -1,    -1,
      -1,   804,    -1,    -1,    -1,    -1,   653,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   856,    -1,    -1,    -1,   860,   861,    -1,
      -1,    -1,    -1,   866,   681,    -1,    -1,    -1,   841,   872,
      -1,    -1,    -1,    -1,    -1,   848,    -1,   850,    -1,   882,
      -1,   884,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     707,   894,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   872,
      -1,    -1,    -1,    -1,   721,    -1,    -1,    -1,   911,    -1,
     913,    -1,   885,   916,    -1,   732,    -1,   890,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   928,    -1,     3,     4,     5,
       6,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,
      16,   758,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,   803,    63,    64,    65,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   815,    -1,
      -1,    -1,    -1,   820,    -1,    -1,    -1,    -1,    84,    -1,
      -1,    87,    88,    -1,    90,    91,    -1,    93,    -1,    95,
      96,    97,    98,    99,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    -1,   855,    81,
      82,   117,   118,   119,    -1,   862,    -1,   864,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   133,    -1,    -1,
      -1,    -1,    -1,   105,    -1,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,    -1,   893,    -1,   895,    -1,
       0,     1,    -1,     3,     4,     5,     6,     7,    -1,    -1,
     132,    11,    12,    -1,    -1,    -1,    16,   914,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,   924,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    -1,    -1,     0,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     8,     9,    10,    -1,
      -1,    -1,    14,    15,    84,    17,    -1,    87,    88,    -1,
      90,    91,    -1,    93,    26,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    36,    37,    -1,    39,    40,    41,
      42,    43,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   132,   133,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    -1,    -1,    81,
      82,    83,    -1,    85,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   104,   105,    -1,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,    -1,    -1,    -1,    -1,   121,
     122,   123,    -1,   125,   126,   127,     0,    -1,   130,    -1,
     132,   133,    -1,    -1,     8,     9,    10,    -1,    -1,    -1,
      14,    15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    36,    37,    -1,    39,    40,    41,    42,    43,
      -1,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    -1,    -1,    81,    82,    -1,    -1,
      -1,    86,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    -1,    -1,    81,    82,    83,
     105,    85,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     104,   105,   127,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,    -1,    -1,    -1,    -1,    -1,   122,   123,
      -1,   125,   126,   127,    -1,    -1,   130,    -1,   132,   133,
       1,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    -1,    -1,    15,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    93,    -1,    95,    96,    97,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,    -1,
      -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,
       7,   132,   133,    10,    11,    12,    -1,    14,    15,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    63,    64,    65,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,
      87,    88,    -1,    90,    91,    -1,    93,    -1,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     117,   118,   119,     1,    -1,     3,     4,     5,     6,     7,
      -1,    -1,    10,    11,    12,   132,   133,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,
      88,    -1,    90,    91,    -1,    93,    -1,    95,    96,    97,
      98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,
     118,   119,     1,    -1,     3,     4,     5,     6,     7,    -1,
      -1,    10,    11,    12,   132,   133,    15,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    38,
      -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,
      -1,    90,    91,    -1,    93,    -1,    95,    96,    97,    98,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,
     119,     1,    -1,     3,     4,     5,     6,     7,    -1,     9,
      10,    11,    12,   132,   133,    -1,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    93,    -1,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,
       1,    -1,     3,     4,     5,     6,     7,    -1,    -1,    -1,
      11,    12,   132,   133,    -1,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    93,    -1,    95,    96,    97,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,    -1,
      -1,    -1,    -1,    -1,     1,   126,     3,     4,     5,     6,
       7,   132,   133,    -1,    11,    12,    -1,    -1,    -1,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    63,    64,    65,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,
      87,    88,    -1,    90,    91,    -1,    93,    -1,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     117,   118,   119,    -1,    -1,    -1,    -1,    -1,     1,   126,
       3,     4,     5,     6,     7,   132,   133,    -1,    11,    12,
      -1,    -1,    -1,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      -1,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
      63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    -1,    -1,    87,    88,    -1,    90,    91,    -1,
      93,    -1,    95,    96,    97,    98,    99,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   117,   118,   119,    -1,    -1,   122,
      -1,    -1,     1,    -1,     3,     4,     5,     6,     7,   132,
     133,    10,    11,    12,    -1,    -1,    -1,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    38,
      -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,
      -1,    90,    91,    -1,    93,    -1,    95,    96,    97,    98,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,
     119,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,   132,   133,    13,    14,    15,    16,    17,
      -1,    -1,    20,    21,    22,    23,    24,    25,    26,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    63,    64,    -1,    66,    67,
      68,    69,    -1,    71,    72,    -1,    -1,    75,    -1,    -1,
      -1,    79,    80,    81,    82,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,    -1,
      -1,   119,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
     128,   129,    11,    12,    -1,    -1,    -1,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    26,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,
      -1,    90,    91,    -1,    93,    94,    95,    96,    97,    98,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   117,   118,
     119,    16,   121,    18,    19,    20,    21,    22,    23,    24,
     129,    26,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    87,    88,    -1,    90,    91,    -1,    93,    94,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   117,   118,   119,    16,   121,    18,    19,    20,
      21,    22,    23,    24,   129,    26,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    93,    94,    95,    96,    97,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   117,   118,   119,    16,
     121,    18,    19,    20,    21,    22,    23,    24,   129,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    63,    64,    65,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    -1,
      87,    88,    -1,    90,    91,    -1,    93,    94,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,   104,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     117,   118,   119,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,   129,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
      63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    85,    -1,    87,    88,    -1,    90,    91,    -1,
      93,    94,    95,    96,    97,    98,    99,    -1,    -1,    -1,
      -1,   104,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   117,   118,   119,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,   129,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,
      -1,    90,    91,    -1,    93,    94,    95,    96,    97,    98,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   117,   118,
     119,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
     129,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    87,    88,    -1,    90,    91,    -1,    93,    94,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   117,   118,   119,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,   129,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    93,    94,    95,    96,    97,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   117,   118,   119,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,   129,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    63,    64,    65,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,
      87,    88,    -1,    90,    91,    -1,    93,    94,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     117,   118,   119,     3,     4,     5,     6,     7,     8,     9,
      10,    -1,   129,    13,    14,    15,    16,    17,    -1,    -1,
      20,    21,    22,    23,    24,    25,    26,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    63,    64,    -1,    66,    67,    68,    69,
      -1,    71,    72,    -1,    -1,    75,    -1,    -1,    -1,    79,
      80,    81,    82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,    -1,    -1,   119,
       3,     4,     5,     6,     7,     8,     9,    10,   128,    -1,
      13,    14,    15,    16,    17,    -1,    -1,    20,    21,    22,
      23,    24,    25,    26,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      63,    64,    -1,    66,    67,    68,    69,    -1,    71,    72,
      -1,    -1,    75,    -1,    -1,    -1,    79,    80,    81,    82,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      93,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,    -1,    -1,   119,     3,     4,     5,
       6,     7,     8,     9,    10,   128,    -1,    13,    14,    15,
      16,    17,    -1,    -1,    20,    21,    22,    23,    24,    25,
      26,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    -1,    -1,    54,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    63,    64,    -1,
      66,    67,    68,    69,    -1,    71,    72,    -1,    -1,    75,
      -1,    -1,    -1,    79,    80,    81,    82,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    93,    -1,    95,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,    -1,    -1,   119,     3,     4,     5,     6,     7,     8,
       9,    10,   128,    -1,    13,    14,    15,    16,    17,    -1,
      -1,    20,    21,    22,    23,    24,    25,    26,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    -1,    -1,    54,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    63,    64,    -1,    66,    67,    68,
      69,    -1,    71,    72,    -1,    -1,    75,    -1,    -1,    -1,
      79,    80,    81,    82,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,    -1,    -1,
     119,    -1,     3,     4,     5,    -1,     7,    -1,    -1,   128,
      11,    12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    -1,    -1,    95,    96,    97,    98,    99,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,   117,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,   127,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    -1,    -1,    95,    96,    97,    98,    99,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,   117,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,   127,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    93,    94,    95,    96,    97,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,   117,   118,   119,    11,
      12,    -1,    -1,    -1,    16,   126,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,    91,
      -1,    93,    -1,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   117,   118,   119,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,
      88,    -1,    90,    91,    -1,    93,    94,    95,    96,    97,
      98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   117,
     118,   119,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    60,    61,    -1,    63,
      64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    -1,    -1,    87,    88,    -1,    90,    91,    -1,    93,
      94,    95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   117,   118,   119,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    93,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   117,   118,   119,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,
      -1,    87,    88,    -1,    90,    91,    -1,    93,    94,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   117,   118,   119,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,    91,
      -1,    93,    94,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   117,   118,   119,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,
      88,    -1,    90,    91,    -1,    93,    94,    95,    96,    97,
      98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   117,
     118,   119,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    60,    61,    -1,    63,
      64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    -1,    -1,    87,    88,    -1,    90,    91,    -1,    93,
      94,    95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   117,   118,   119,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    93,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   117,   118,   119,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,
      -1,    87,    88,    -1,    90,    91,    -1,    93,    94,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   117,   118,   119,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,    91,
      -1,    93,    94,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   117,   118,   119,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,
      88,    -1,    90,    91,    -1,    93,    94,    95,    96,    97,
      98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   117,
     118,   119,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    60,    61,    -1,    63,
      64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    -1,    -1,    87,    88,    -1,    90,    91,    -1,    93,
      94,    95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   117,   118,   119,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    -1,    -1,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   117,   118,   119,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,
      -1,    87,    88,    -1,    90,    91,    -1,    93,    -1,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   117,   118,   119,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,    91,
      -1,    93,    -1,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   117,   118,   119,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,
      88,    -1,    90,    91,    -1,    93,    -1,    95,    96,    97,
      98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   117,
     118,   119,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    60,    61,    -1,    63,
      64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    -1,    -1,    87,    88,    -1,    90,    91,    -1,    93,
      -1,    95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   117,   118,   119,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    93,    -1,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   117,   118,   119,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,
      -1,    87,    88,    -1,    90,    91,    -1,    93,    -1,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   117,   118,   119,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,    91,
      -1,    93,    -1,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   117,   118,   119,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,
      88,    -1,    90,    91,    -1,    -1,    -1,    95,    96,    97,
      98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   117,
     118,   119,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    60,    61,    -1,    63,
      64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    -1,    -1,    87,    88,    -1,    90,    91,    -1,    -1,
      -1,    95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   117,   118,   119,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    93,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,   109,
      -1,    11,    12,    -1,    -1,    -1,    16,   117,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    93,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,    -1,    -1,    -1,    16,   117,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    81,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    -1,    -1,    95,    96,    97,    98,    99,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,    -1,    -1,    -1,    16,   117,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    93,    -1,    95,    96,    97,    98,    99,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,    -1,    -1,    -1,    16,   117,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    93,    -1,    95,    96,    97,    98,    99,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,    -1,    -1,    -1,    16,   117,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    -1,    -1,    95,    96,    97,    98,    99,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,    -1,    -1,    -1,    16,   117,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    -1,    -1,    95,    96,    97,    98,    99,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,    -1,    -1,    -1,    16,   117,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    -1,    -1,    95,    96,    97,    98,    99,
      -1,    -1,    50,    51,    -1,    -1,    54,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    63,    64,   117,    66,    67,
      68,    69,    -1,    71,    72,    -1,    -1,    75,    -1,    -1,
      -1,    79,    80,    81,    82,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,    50,
      51,   119,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,
     128,    -1,    63,    64,    -1,    66,    67,    68,    69,    -1,
      71,    72,    -1,    -1,    75,    -1,    -1,    -1,    79,    80,
      81,    82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,    50,    51,   119,    -1,
      54,    -1,    -1,    -1,    -1,    -1,    -1,   128,    -1,    63,
      64,    -1,    66,    67,    68,    69,    -1,    71,    72,    -1,
      -1,    75,    -1,    -1,    -1,    79,    80,    81,    82,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    93,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,    50,    51,   119,    -1,    54,    -1,    -1,
      -1,    -1,    -1,    -1,   128,    -1,    63,    64,    -1,    66,
      67,    68,    69,    -1,    71,    72,    -1,    -1,    75,    -1,
      -1,    -1,    79,    80,    81,    82,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
      50,    51,   119,    -1,    54,    -1,    -1,    -1,    -1,    -1,
      -1,   128,    -1,    63,    64,    -1,    66,    67,    68,    69,
      -1,    71,    72,    -1,    -1,    75,    -1,    -1,    -1,    79,
      80,    81,    82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,    50,    51,   119,
      -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,   128,    -1,
      63,    64,    -1,    66,    67,    68,    69,    -1,    71,    72,
      -1,    -1,    75,    -1,    -1,    -1,    79,    80,    81,    82,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,    50,    51,   119,    -1,    54,    -1,
      -1,    -1,    -1,    -1,    -1,   128,    -1,    63,    64,    -1,
      66,    67,    68,    69,    -1,    71,    72,    -1,    -1,    75,
      -1,    -1,    -1,    79,    80,    81,    82,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    93,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,    50,    51,   119,    -1,    54,    -1,    -1,    -1,    -1,
      -1,    -1,   128,    -1,    63,    64,    -1,    66,    67,    68,
      69,    -1,    71,    72,    -1,    -1,    75,    -1,    -1,    -1,
      79,    80,    81,    82,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,    50,    51,
     119,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,   128,
      -1,    63,    64,    -1,    66,    67,    68,    69,    -1,    71,
      72,    -1,    -1,    75,    -1,    -1,    -1,    79,    80,    81,
      82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,    50,    51,   119,    -1,    54,
      -1,    -1,    -1,    -1,    -1,    -1,   128,    -1,    63,    64,
      -1,    66,    67,    68,    69,    -1,    71,    72,    -1,    -1,
      75,    -1,    -1,    -1,    79,    80,    81,    82,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    93,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,    -1,    -1,   119,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   128,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    -1,    -1,    81,
      82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   105,    -1,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   127,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    -1,
      -1,    81,    82,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    -1,    -1,    81,    82,
      -1,    -1,    -1,    -1,    -1,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,    -1,    -1,    -1,
      -1,    -1,   105,    -1,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,    66,    67,    68,    69,    70,    71,
      72,    73,    -1,    75,    76,    -1,    -1,    -1,    -1,    81,
      82,    66,    67,    68,    69,    70,    71,    72,    -1,    -1,
      75,    76,    -1,    -1,    -1,    -1,    81,    82,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,    66,    67,    68,    69,    70,    71,    72,    -1,
      -1,    75,    76,    -1,    -1,    -1,    -1,    81,    82,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   135,   136,     0,     1,     3,     4,     5,     6,     7,
      11,    12,    16,    18,    19,    20,    21,    22,    23,    24,
      29,    30,    31,    32,    33,    34,    35,    38,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    60,    61,    63,    64,    65,    84,    87,
      88,    90,    91,    93,    95,    96,    97,    98,    99,   117,
     118,   119,   138,   139,   140,   143,   145,   146,   150,   151,
     153,   154,   155,   156,   157,   167,   185,   208,   220,   221,
     234,   235,   236,   237,   238,   239,   240,   243,   251,   253,
     254,   255,   256,   257,   258,   277,   286,   140,    21,    22,
      29,    30,    31,    45,    50,    54,    81,    84,    87,   117,
     158,   159,   185,   208,   255,   258,   277,   159,     3,     4,
       5,     6,     7,     8,     9,    10,    13,    14,    15,    16,
      17,    20,    21,    22,    23,    24,    25,    26,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    54,    63,    64,    66,    67,    68,    69,    71,    72,
      75,    79,    80,    81,    82,    93,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   119,   128,   129,   160,
     165,   166,   256,   272,    32,    33,    34,    35,    48,    49,
      50,    54,   160,   161,   162,   163,   251,   253,   186,   189,
     190,   132,   133,   195,   281,   284,   285,   191,   193,   198,
      84,    93,    94,   118,   150,   157,   167,   170,   174,   181,
     183,   208,   255,   257,   258,   275,   276,   174,   174,   129,
     176,   177,   129,   172,   176,   143,    52,   162,   132,   282,
     142,   121,   246,   167,   208,   167,    50,    51,    54,   165,
     278,    54,    87,   138,   152,   153,   143,    93,   150,   171,
     183,   275,   286,   183,   274,   275,   286,    84,   156,   208,
     255,   258,    52,    53,    55,   160,   246,   252,   245,   246,
     131,   241,   131,   244,    57,    58,   145,   167,   167,   281,
     285,    39,    40,    41,    42,    43,    36,    37,    28,   217,
     104,   127,    87,    93,   154,   104,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    81,
      82,   105,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,    83,   123,   125,   130,    83,   125,    26,   121,
     222,   237,    85,    85,   172,   176,   222,   282,   143,    50,
      54,   158,    57,    58,     1,   108,   259,   284,    83,   123,
     125,   204,   273,   205,    83,   125,   280,   127,   137,   138,
     143,   144,   144,   144,   196,   197,   133,   144,   144,    84,
     151,   157,   208,   213,   255,   258,    54,   170,   170,   104,
      86,   127,   182,   286,    83,   123,   125,    85,    85,   127,
     182,   126,   174,    88,   174,   178,   150,   174,   183,   220,
     286,    52,    60,    61,   141,   129,   168,   121,   138,    59,
     100,   101,   102,   247,    83,   125,    85,   152,   126,   126,
     187,   167,   282,   124,   127,   132,   283,   127,   283,   127,
     283,   122,   283,    54,    83,   123,   125,   102,   102,   247,
      62,   102,   102,   242,   247,   102,    59,   102,    66,    66,
     140,   144,   144,   144,   144,   140,   143,   143,   218,    93,
     145,   170,   183,   184,   152,   156,   127,   145,   167,   170,
     184,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   169,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,    50,    51,    54,
     165,   278,   279,   171,    50,    54,   278,   278,   278,   278,
     225,   223,   145,   167,   145,   167,    92,   147,   202,   284,
     260,   201,    50,    54,   158,   278,   171,   278,   137,   143,
      50,    52,    53,    54,    55,    93,    94,   111,   114,   129,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     206,   164,    10,     8,   230,   286,    13,   106,   209,   284,
     209,   281,    17,   227,    15,    27,   106,   210,   284,   210,
      54,    83,   123,   125,    25,   127,   182,   167,   170,    93,
     170,   181,   275,    50,    54,   171,    50,    54,    93,   170,
     276,   126,    93,   167,   170,   175,   179,   181,   275,   282,
     127,    83,   125,   282,   126,   162,   188,   167,   138,   122,
     249,   248,   167,   126,   282,   282,    93,   170,    50,    54,
     171,    50,    54,   131,   247,   131,   167,   167,    74,   109,
     216,   286,   170,   127,   126,    43,   167,    85,    85,   172,
     176,   124,    85,    85,   172,   173,   176,   286,   173,   176,
     173,   176,   216,   216,   148,   284,   144,   137,   124,    10,
     282,   104,   262,   137,   284,   127,   271,   286,   127,   271,
      50,   127,   271,    50,   160,   162,   170,   184,   231,   286,
      15,   212,   286,   138,    13,   138,   227,    93,   183,   228,
      10,   138,   192,   194,    50,    54,   171,    50,    54,   199,
     170,   127,   182,    85,   124,    85,    85,   170,   170,   127,
     180,   126,   127,   182,   126,   150,   220,   278,   278,   126,
     143,   122,   138,    52,    53,    55,   250,   258,   126,   167,
     124,    93,    94,   109,   154,   214,   215,   219,    93,   170,
     167,   106,   145,   167,   167,   147,    85,   145,   167,   145,
     167,   147,   226,   224,   216,   203,   284,    10,   126,   170,
     282,    10,   263,   266,   268,   270,    50,   265,   268,   207,
      86,   232,   286,   138,     9,   233,   286,    14,   211,   212,
     212,    10,   170,   127,   209,    10,   138,   138,   124,   144,
     182,    93,    85,   182,   182,    93,   181,   183,   275,   126,
      93,   282,   126,   122,   282,    84,   127,   157,   208,   255,
     258,   157,   127,   109,   138,   170,   167,   145,   167,   138,
     138,   149,   137,   126,   127,   271,   271,   271,   261,   157,
     209,   138,   144,    10,    10,    93,   138,    10,    10,   210,
     167,   170,   127,   182,   127,   182,   170,   126,    54,    94,
     127,    83,   123,   125,    93,    94,   154,    10,    10,   122,
     138,    10,   268,   137,   138,   209,   170,   212,   227,   229,
     200,   182,   182,    93,   275,    93,   182,   157,    94,    50,
      54,   171,    50,    54,   127,   157,   157,   122,   271,    10,
     230,   138,   138,   170,   127,   182,   170,   157,   124,    94,
     127,   211,    10,   182,    93,   182,   157,    94,   170,   157,
     182
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
#line 431 "grammar.y"
    {
                        vps->lex_state = EXPR_BEG;
                        vps->variables = new LocalState(0);
                        class_nest = 0;
                    ;}
    break;

  case 3:
#line 437 "grammar.y"
    {
                        if ((yyvsp[(2) - (2)].node) && !compile_for_eval) {
                            /* last expression should not be void */
                            if (nd_type((yyvsp[(2) - (2)].node)) != NODE_BLOCK) void_expr((yyvsp[(2) - (2)].node));
                            else {
                                NODE *node = (yyvsp[(2) - (2)].node);
                                while (node->nd_next) {
                                    node = node->nd_next;
                                }
                                void_expr(node->nd_head);
                            }
                        }
                        vps->top = block_append(vps, vps->top, (yyvsp[(2) - (2)].node));
                        class_nest = 0;
                    ;}
    break;

  case 4:
#line 458 "grammar.y"
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
#line 475 "grammar.y"
    {
                        void_stmts((yyvsp[(1) - (2)].node), vps);
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 7:
#line 483 "grammar.y"
    {
                        (yyval.node) = newline_node(vps, (yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 8:
#line 487 "grammar.y"
    {
                        (yyval.node) = block_append(vps, (yyvsp[(1) - (3)].node), newline_node(vps, (yyvsp[(3) - (3)].node)));
                    ;}
    break;

  case 9:
#line 491 "grammar.y"
    {
                        (yyval.node) = remove_begin((yyvsp[(2) - (2)].node), vps);
                    ;}
    break;

  case 10:
#line 496 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 11:
#line 497 "grammar.y"
    {
                        (yyval.node) = NEW_ALIAS((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 12:
#line 501 "grammar.y"
    {
                        (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 13:
#line 505 "grammar.y"
    {
                        char buf[3];

                        snprintf(buf, sizeof(buf), "$%c", (char)(yyvsp[(3) - (3)].node)->nd_nth);
                        (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), rb_parser_sym(buf));
                    ;}
    break;

  case 14:
#line 512 "grammar.y"
    {
                        yyerror("can't make alias for the number variables");
                        (yyval.node) = 0;
                    ;}
    break;

  case 15:
#line 517 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 16:
#line 521 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(3) - (3)].node), vps), remove_begin((yyvsp[(1) - (3)].node), vps), 0);
                        fixpos((yyval.node), (yyvsp[(3) - (3)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            (yyval.node)->nd_else = (yyval.node)->nd_body;
                            (yyval.node)->nd_body = 0;
                        }
                    ;}
    break;

  case 17:
#line 530 "grammar.y"
    {
                        (yyval.node) = NEW_UNLESS(cond((yyvsp[(3) - (3)].node), vps), remove_begin((yyvsp[(1) - (3)].node), vps), 0);
                        fixpos((yyval.node), (yyvsp[(3) - (3)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            (yyval.node)->nd_body = (yyval.node)->nd_else;
                            (yyval.node)->nd_else = 0;
                        }
                    ;}
    break;

  case 18:
#line 539 "grammar.y"
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

  case 19:
#line 551 "grammar.y"
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

  case 20:
#line 563 "grammar.y"
    {
                        NODE *resq = NEW_RESBODY(0, remove_begin((yyvsp[(3) - (3)].node), vps), 0);
                        (yyval.node) = NEW_RESCUE(remove_begin((yyvsp[(1) - (3)].node), vps), resq, 0);
                    ;}
    break;

  case 21:
#line 568 "grammar.y"
    {
                        if (in_def || in_single) {
                            yyerror("BEGIN in method");
                        }
                        local_push(0);
                    ;}
    break;

  case 22:
#line 575 "grammar.y"
    {
                        /*
                        ruby_eval_tree_begin = block_append(ruby_eval_tree_begin,
                                                            NEW_PREEXE($4));
                        */
                        local_pop();
                        (yyval.node) = 0;
                    ;}
    break;

  case 23:
#line 584 "grammar.y"
    {
                        if (in_def || in_single) {
                            rb_warn("END in method; use at_exit");
                        }

                        (yyval.node) = NEW_ITER(0, NEW_POSTEXE(), (yyvsp[(3) - (4)].node));
                    ;}
    break;

  case 24:
#line 592 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 25:
#line 596 "grammar.y"
    {
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyvsp[(1) - (3)].node)->nd_value = ((yyvsp[(1) - (3)].node)->nd_head) ? NEW_TO_ARY((yyvsp[(3) - (3)].node)) : NEW_ARRAY((yyvsp[(3) - (3)].node));
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 26:
#line 602 "grammar.y"
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

  case 27:
#line 627 "grammar.y"
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

  case 28:
#line 643 "grammar.y"
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

  case 29:
#line 655 "grammar.y"
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

  case 30:
#line 667 "grammar.y"
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
#line 679 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (3)].node), vps);
                        (yyval.node) = 0;
                    ;}
    break;

  case 32:
#line 684 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), NEW_SVALUE((yyvsp[(3) - (3)].node)), vps);
                    ;}
    break;

  case 33:
#line 688 "grammar.y"
    {
                        (yyvsp[(1) - (3)].node)->nd_value = ((yyvsp[(1) - (3)].node)->nd_head) ? NEW_TO_ARY((yyvsp[(3) - (3)].node)) : NEW_ARRAY((yyvsp[(3) - (3)].node));
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 34:
#line 693 "grammar.y"
    {
                        (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 37:
#line 702 "grammar.y"
    {
                        (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 38:
#line 706 "grammar.y"
    {
                        (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 39:
#line 710 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), vps));
                    ;}
    break;

  case 40:
#line 714 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), vps));
                    ;}
    break;

  case 42:
#line 721 "grammar.y"
    {
                        value_expr((yyval.node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 45:
#line 730 "grammar.y"
    {
                        (yyval.node) = NEW_RETURN(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 46:
#line 734 "grammar.y"
    {
                        (yyval.node) = NEW_BREAK(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 47:
#line 738 "grammar.y"
    {
                        (yyval.node) = NEW_NEXT(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 49:
#line 745 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 50:
#line 749 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 51:
#line 755 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 52:
#line 759 "grammar.y"
    { (yyval.vars) = vps->variables->block_vars; ;}
    break;

  case 53:
#line 762 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 54:
#line 769 "grammar.y"
    {
                        (yyval.node) = new_fcall(vps, (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                   ;}
    break;

  case 55:
#line 774 "grammar.y"
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

  case 56:
#line 786 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 57:
#line 791 "grammar.y"
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

  case 58:
#line 803 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 59:
#line 808 "grammar.y"
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
#line 820 "grammar.y"
    {
                        (yyval.node) = new_super(vps, (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 61:
#line 825 "grammar.y"
    {
                        (yyval.node) = new_yield(vps, (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 63:
#line 833 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 65:
#line 840 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(NEW_LIST((yyvsp[(2) - (3)].node)), 0);
                    ;}
    break;

  case 66:
#line 846 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (1)].node), 0);
                    ;}
    break;

  case 67:
#line 850 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(list_append(vps, (yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node)), 0);
                    ;}
    break;

  case 68:
#line 854 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 69:
#line 858 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (2)].node), -1);
                    ;}
    break;

  case 70:
#line 862 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(0, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 71:
#line 866 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(0, -1);
                    ;}
    break;

  case 73:
#line 873 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 74:
#line 879 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 75:
#line 883 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 76:
#line 889 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, vps);
                    ;}
    break;

  case 77:
#line 893 "grammar.y"
    {
                        (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), vps);
                    ;}
    break;

  case 78:
#line 897 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 79:
#line 901 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 80:
#line 905 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 81:
#line 909 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                    ;}
    break;

  case 82:
#line 915 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                    ;}
    break;

  case 83:
#line 921 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (1)].node), vps);
                        (yyval.node) = 0;
                    ;}
    break;

  case 84:
#line 928 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, vps);
                    ;}
    break;

  case 85:
#line 932 "grammar.y"
    {
                        (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), vps);
                    ;}
    break;

  case 86:
#line 936 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 87:
#line 940 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 88:
#line 944 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 89:
#line 948 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                    ;}
    break;

  case 90:
#line 954 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                    ;}
    break;

  case 91:
#line 960 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (1)].node), vps);
                        (yyval.node) = 0;
                    ;}
    break;

  case 92:
#line 967 "grammar.y"
    {
                        yyerror("class/module name must be CONSTANT");
                    ;}
    break;

  case 94:
#line 974 "grammar.y"
    {
                        (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 95:
#line 978 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2(0, (yyval.node));
                    ;}
    break;

  case 96:
#line 982 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 100:
#line 991 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = convert_op((yyvsp[(1) - (1)].id));
                    ;}
    break;

  case 101:
#line 996 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = (yyvsp[(1) - (1)].id);
                    ;}
    break;

  case 104:
#line 1007 "grammar.y"
    {
                        (yyval.node) = NEW_LIT(QUID2SYM((yyvsp[(1) - (1)].id)));
                    ;}
    break;

  case 106:
#line 1014 "grammar.y"
    {
                        (yyval.node) = NEW_UNDEF((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 107:
#line 1017 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 108:
#line 1018 "grammar.y"
    {
                        (yyval.node) = block_append(vps, (yyvsp[(1) - (4)].node), NEW_UNDEF((yyvsp[(4) - (4)].node)));
                    ;}
    break;

  case 109:
#line 1023 "grammar.y"
    { (yyval.id) = '|'; ;}
    break;

  case 110:
#line 1024 "grammar.y"
    { (yyval.id) = '^'; ;}
    break;

  case 111:
#line 1025 "grammar.y"
    { (yyval.id) = '&'; ;}
    break;

  case 112:
#line 1026 "grammar.y"
    { (yyval.id) = tCMP; ;}
    break;

  case 113:
#line 1027 "grammar.y"
    { (yyval.id) = tEQ; ;}
    break;

  case 114:
#line 1028 "grammar.y"
    { (yyval.id) = tEQQ; ;}
    break;

  case 115:
#line 1029 "grammar.y"
    { (yyval.id) = tMATCH; ;}
    break;

  case 116:
#line 1030 "grammar.y"
    { (yyval.id) = '>'; ;}
    break;

  case 117:
#line 1031 "grammar.y"
    { (yyval.id) = tGEQ; ;}
    break;

  case 118:
#line 1032 "grammar.y"
    { (yyval.id) = '<'; ;}
    break;

  case 119:
#line 1033 "grammar.y"
    { (yyval.id) = tLEQ; ;}
    break;

  case 120:
#line 1034 "grammar.y"
    { (yyval.id) = tLSHFT; ;}
    break;

  case 121:
#line 1035 "grammar.y"
    { (yyval.id) = tRSHFT; ;}
    break;

  case 122:
#line 1036 "grammar.y"
    { (yyval.id) = '+'; ;}
    break;

  case 123:
#line 1037 "grammar.y"
    { (yyval.id) = '-'; ;}
    break;

  case 124:
#line 1038 "grammar.y"
    { (yyval.id) = '*'; ;}
    break;

  case 125:
#line 1039 "grammar.y"
    { (yyval.id) = '*'; ;}
    break;

  case 126:
#line 1040 "grammar.y"
    { (yyval.id) = '/'; ;}
    break;

  case 127:
#line 1041 "grammar.y"
    { (yyval.id) = '%'; ;}
    break;

  case 128:
#line 1042 "grammar.y"
    { (yyval.id) = tPOW; ;}
    break;

  case 129:
#line 1043 "grammar.y"
    { (yyval.id) = '~'; ;}
    break;

  case 130:
#line 1044 "grammar.y"
    { (yyval.id) = tUPLUS; ;}
    break;

  case 131:
#line 1045 "grammar.y"
    { (yyval.id) = tUMINUS; ;}
    break;

  case 132:
#line 1046 "grammar.y"
    { (yyval.id) = tAREF; ;}
    break;

  case 133:
#line 1047 "grammar.y"
    { (yyval.id) = tASET; ;}
    break;

  case 134:
#line 1048 "grammar.y"
    { (yyval.id) = '`'; ;}
    break;

  case 176:
#line 1061 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 177:
#line 1065 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (5)].node), NEW_RESCUE((yyvsp[(3) - (5)].node), NEW_RESBODY(0,(yyvsp[(5) - (5)].node),0), 0), vps);
                    ;}
    break;

  case 178:
#line 1069 "grammar.y"
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

  case 179:
#line 1094 "grammar.y"
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

  case 180:
#line 1110 "grammar.y"
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

  case 181:
#line 1122 "grammar.y"
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

  case 182:
#line 1134 "grammar.y"
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

  case 183:
#line 1146 "grammar.y"
    {
                        yyerror("constant re-assignment");
                        (yyval.node) = 0;
                    ;}
    break;

  case 184:
#line 1151 "grammar.y"
    {
                        yyerror("constant re-assignment");
                        (yyval.node) = 0;
                    ;}
    break;

  case 185:
#line 1156 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (3)].node), vps);
                        (yyval.node) = 0;
                    ;}
    break;

  case 186:
#line 1161 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (3)].node));
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyval.node) = NEW_DOT2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 187:
#line 1167 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (3)].node));
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyval.node) = NEW_DOT3((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 188:
#line 1173 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '+', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 189:
#line 1177 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '-', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 190:
#line 1181 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '*', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 191:
#line 1185 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '/', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 192:
#line 1189 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '%', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 193:
#line 1193 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tPOW, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 194:
#line 1197 "grammar.y"
    {
                        (yyval.node) = call_op(call_op((yyvsp[(2) - (4)].node), tPOW, 1, (yyvsp[(4) - (4)].node), vps), tUMINUS, 0, 0, vps);
                    ;}
    break;

  case 195:
#line 1201 "grammar.y"
    {
                        (yyval.node) = call_op(call_op((yyvsp[(2) - (4)].node), tPOW, 1, (yyvsp[(4) - (4)].node), vps), tUMINUS, 0, 0, vps);
                    ;}
    break;

  case 196:
#line 1205 "grammar.y"
    {
                        if ((yyvsp[(2) - (2)].node) && nd_type((yyvsp[(2) - (2)].node)) == NODE_LIT) {
                            (yyval.node) = (yyvsp[(2) - (2)].node);
                        }
                        else {
                            (yyval.node) = call_op((yyvsp[(2) - (2)].node), tUPLUS, 0, 0, vps);
                        }
                    ;}
    break;

  case 197:
#line 1214 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(2) - (2)].node), tUMINUS, 0, 0, vps);
                    ;}
    break;

  case 198:
#line 1218 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '|', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 199:
#line 1222 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '^', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 200:
#line 1226 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '&', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 201:
#line 1230 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tCMP, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 202:
#line 1234 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '>', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 203:
#line 1238 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tGEQ, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 204:
#line 1242 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '<', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 205:
#line 1246 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tLEQ, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 206:
#line 1250 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tEQ, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 207:
#line 1254 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tEQQ, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 208:
#line 1258 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(call_op((yyvsp[(1) - (3)].node), tEQ, 1, (yyvsp[(3) - (3)].node), vps));
                    ;}
    break;

  case 209:
#line 1262 "grammar.y"
    {
                        (yyval.node) = match_gen((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 210:
#line 1266 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(match_gen((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps));
                    ;}
    break;

  case 211:
#line 1270 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), vps));
                    ;}
    break;

  case 212:
#line 1274 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(2) - (2)].node), '~', 0, 0, vps);
                    ;}
    break;

  case 213:
#line 1278 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tLSHFT, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 214:
#line 1282 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tRSHFT, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 215:
#line 1286 "grammar.y"
    {
                        (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 216:
#line 1290 "grammar.y"
    {
                        (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 217:
#line 1293 "grammar.y"
    {vps->in_defined = 1;;}
    break;

  case 218:
#line 1294 "grammar.y"
    {
                        vps->in_defined = 0;
                        (yyval.node) = NEW_DEFINED((yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 219:
#line 1298 "grammar.y"
    {vps->ternary_colon++;;}
    break;

  case 220:
#line 1299 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(1) - (6)].node), vps), (yyvsp[(4) - (6)].node), (yyvsp[(6) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (6)].node));
                        vps->ternary_colon--;
                    ;}
    break;

  case 221:
#line 1305 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 222:
#line 1311 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (1)].node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 224:
#line 1319 "grammar.y"
    {
                        rb_warn("parenthesize argument(s) for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 225:
#line 1324 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 226:
#line 1328 "grammar.y"
    {
                        value_expr((yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_concat(vps, (yyvsp[(1) - (5)].node), (yyvsp[(4) - (5)].node));
                    ;}
    break;

  case 227:
#line 1333 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                    ;}
    break;

  case 228:
#line 1337 "grammar.y"
    {
                        value_expr((yyvsp[(2) - (3)].node));
                        (yyval.node) = NEW_NEWLINE(NEW_SPLAT((yyvsp[(2) - (3)].node)));
                    ;}
    break;

  case 229:
#line 1344 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 230:
#line 1348 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                    ;}
    break;

  case 231:
#line 1352 "grammar.y"
    {
                        rb_warn("parenthesize argument for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(2) - (4)].node));
                    ;}
    break;

  case 232:
#line 1357 "grammar.y"
    {
                        rb_warn("parenthesize argument for future version");
                        (yyval.node) = list_append(vps, (yyvsp[(2) - (6)].node), (yyvsp[(4) - (6)].node));
                    ;}
    break;

  case 235:
#line 1368 "grammar.y"
    {
                        rb_warn("parenthesize argument(s) for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 236:
#line 1373 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 237:
#line 1377 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, (yyvsp[(1) - (5)].node), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 238:
#line 1382 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 239:
#line 1387 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, NEW_LIST(NEW_HASH((yyvsp[(1) - (5)].node))), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 240:
#line 1392 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (4)].node), NEW_HASH((yyvsp[(3) - (4)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 241:
#line 1397 "grammar.y"
    {
                        value_expr((yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_concat(vps, list_append(vps, (yyvsp[(1) - (7)].node), NEW_HASH((yyvsp[(3) - (7)].node))), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 242:
#line 1403 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(NEW_SPLAT((yyvsp[(2) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 244:
#line 1410 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(list_concat(NEW_LIST((yyvsp[(1) - (4)].node)),(yyvsp[(3) - (4)].node)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 245:
#line 1414 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 246:
#line 1418 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, NEW_LIST((yyvsp[(1) - (5)].node)), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 247:
#line 1423 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, list_concat(NEW_LIST((yyvsp[(1) - (7)].node)),(yyvsp[(3) - (7)].node)), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 248:
#line 1428 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 249:
#line 1433 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, NEW_LIST(NEW_HASH((yyvsp[(1) - (5)].node))), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 250:
#line 1438 "grammar.y"
    {
                        (yyval.node) = list_append(vps, NEW_LIST((yyvsp[(1) - (4)].node)), NEW_HASH((yyvsp[(3) - (4)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 251:
#line 1443 "grammar.y"
    {
                        (yyval.node) = list_append(vps, list_concat(NEW_LIST((yyvsp[(1) - (6)].node)),(yyvsp[(3) - (6)].node)), NEW_HASH((yyvsp[(5) - (6)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(6) - (6)].node));
                    ;}
    break;

  case 252:
#line 1448 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, list_append(vps, NEW_LIST((yyvsp[(1) - (7)].node)), NEW_HASH((yyvsp[(3) - (7)].node))), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 253:
#line 1453 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, list_append(vps,
                                        list_concat(NEW_LIST((yyvsp[(1) - (9)].node)), (yyvsp[(3) - (9)].node)), NEW_HASH((yyvsp[(5) - (9)].node))), (yyvsp[(8) - (9)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(9) - (9)].node));
                    ;}
    break;

  case 254:
#line 1459 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(NEW_SPLAT((yyvsp[(2) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 256:
#line 1465 "grammar.y"
    {
                        (yyval.num) = vps->cmdarg_stack;
                        CMDARG_PUSH(1);
                    ;}
    break;

  case 257:
#line 1470 "grammar.y"
    {
                        /* CMDARG_POP() */
                        vps->cmdarg_stack = (yyvsp[(1) - (2)].num);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 259:
#line 1478 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 260:
#line 1479 "grammar.y"
    {
                        rb_warn("don't put space before argument parentheses");
                        (yyval.node) = 0;
                    ;}
    break;

  case 261:
#line 1483 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 262:
#line 1484 "grammar.y"
    {
                        rb_warn("don't put space before argument parentheses");
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                    ;}
    break;

  case 263:
#line 1491 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK_PASS((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 264:
#line 1497 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 266:
#line 1504 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 267:
#line 1508 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 268:
#line 1514 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 269:
#line 1518 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, (yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 270:
#line 1522 "grammar.y"
    {
                        (yyval.node) = NEW_SPLAT((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 279:
#line 1536 "grammar.y"
    {
                        (yyval.node) = NEW_FCALL((yyvsp[(1) - (1)].id), 0);
                    ;}
    break;

  case 280:
#line 1540 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        PUSH_LINE("begin");
                    ;}
    break;

  case 281:
#line 1546 "grammar.y"
    {
                        POP_LINE();
                        if ((yyvsp[(3) - (4)].node) == NULL)
                            (yyval.node) = NEW_NIL();
                        else
                            (yyval.node) = NEW_BEGIN((yyvsp[(3) - (4)].node));
                        nd_set_line((yyval.node), (yyvsp[(1) - (4)].num));
                    ;}
    break;

  case 282:
#line 1554 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 283:
#line 1555 "grammar.y"
    {
                        rb_warning("(...) interpreted as grouped expression");
                        (yyval.node) = (yyvsp[(2) - (5)].node);
                    ;}
    break;

  case 284:
#line 1560 "grammar.y"
    {
                        if (!(yyvsp[(2) - (3)].node)) (yyval.node) = NEW_NIL();
                        else (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 285:
#line 1565 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 286:
#line 1569 "grammar.y"
    {
                        (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 287:
#line 1573 "grammar.y"
    {
                        if ((yyvsp[(1) - (4)].node) && nd_type((yyvsp[(1) - (4)].node)) == NODE_SELF) {
                            (yyval.node) = NEW_FCALL(convert_op(tAREF), (yyvsp[(3) - (4)].node));
                        } else {
                            (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), convert_op(tAREF), (yyvsp[(3) - (4)].node));
                        }
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 288:
#line 1582 "grammar.y"
    {
                        if ((yyvsp[(2) - (3)].node) == 0) {
                            (yyval.node) = NEW_ZARRAY(); /* zero length array*/
                        }
                        else {
                            (yyval.node) = (yyvsp[(2) - (3)].node);
                        }
                    ;}
    break;

  case 289:
#line 1591 "grammar.y"
    {
                        (yyval.node) = NEW_HASH((yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 290:
#line 1595 "grammar.y"
    {
                        (yyval.node) = NEW_RETURN(0);
                    ;}
    break;

  case 291:
#line 1599 "grammar.y"
    {
                        (yyval.node) = new_yield(vps, (yyvsp[(3) - (4)].node));
                    ;}
    break;

  case 292:
#line 1603 "grammar.y"
    {
                        (yyval.node) = NEW_YIELD(0, Qfalse);
                    ;}
    break;

  case 293:
#line 1607 "grammar.y"
    {
                        (yyval.node) = NEW_YIELD(0, Qfalse);
                    ;}
    break;

  case 294:
#line 1610 "grammar.y"
    {vps->in_defined = 1;;}
    break;

  case 295:
#line 1611 "grammar.y"
    {
                        vps->in_defined = 0;
                        (yyval.node) = NEW_DEFINED((yyvsp[(5) - (6)].node));
                    ;}
    break;

  case 296:
#line 1616 "grammar.y"
    {
                        (yyvsp[(2) - (2)].node)->nd_iter = NEW_FCALL((yyvsp[(1) - (2)].id), 0);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                        fixpos((yyvsp[(2) - (2)].node)->nd_iter, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 298:
#line 1623 "grammar.y"
    {
                        if ((yyvsp[(1) - (2)].node) && nd_type((yyvsp[(1) - (2)].node)) == NODE_BLOCK_PASS) {
                            rb_compile_error(vps, "both block arg and actual block given");
                        }
                        (yyvsp[(2) - (2)].node)->nd_iter = (yyvsp[(1) - (2)].node);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                        fixpos((yyval.node), (yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 299:
#line 1631 "grammar.y"
    {
                    PUSH_LINE("if");
                  ;}
    break;

  case 300:
#line 1637 "grammar.y"
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

  case 301:
#line 1647 "grammar.y"
    {
                    PUSH_LINE("unless");
                  ;}
    break;

  case 302:
#line 1653 "grammar.y"
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

  case 303:
#line 1663 "grammar.y"
    {
                    PUSH_LINE("while");
                    COND_PUSH(1);
                  ;}
    break;

  case 304:
#line 1666 "grammar.y"
    {COND_POP();;}
    break;

  case 305:
#line 1669 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (7)].node), vps), (yyvsp[(6) - (7)].node), 1);
                        fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            nd_set_type((yyval.node), NODE_UNTIL);
                        }
                    ;}
    break;

  case 306:
#line 1677 "grammar.y"
    {
                    PUSH_LINE("until");
                    COND_PUSH(1);
                  ;}
    break;

  case 307:
#line 1680 "grammar.y"
    {COND_POP();;}
    break;

  case 308:
#line 1683 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (7)].node), vps), (yyvsp[(6) - (7)].node), 1);
                        fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            nd_set_type((yyval.node), NODE_WHILE);
                        }
                    ;}
    break;

  case 309:
#line 1691 "grammar.y"
    {
                    PUSH_LINE("case");
                  ;}
    break;

  case 310:
#line 1696 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_CASE((yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(3) - (6)].node));
                    ;}
    break;

  case 311:
#line 1701 "grammar.y"
    { 
                    push_start_line((rb_parse_state*)parse_state, ruby_sourceline - 1, "case");
                  ;}
    break;

  case 312:
#line 1704 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = (yyvsp[(4) - (5)].node);
                    ;}
    break;

  case 313:
#line 1708 "grammar.y"
    {
                    push_start_line((rb_parse_state*)parse_state, ruby_sourceline - 1, "case");
                  ;}
    break;

  case 314:
#line 1711 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = (yyvsp[(5) - (6)].node);
                    ;}
    break;

  case 315:
#line 1715 "grammar.y"
    {
                    PUSH_LINE("for");
                  ;}
    break;

  case 316:
#line 1717 "grammar.y"
    {COND_PUSH(1);;}
    break;

  case 317:
#line 1717 "grammar.y"
    {COND_POP();;}
    break;

  case 318:
#line 1720 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_FOR((yyvsp[(3) - (10)].node), (yyvsp[(6) - (10)].node), (yyvsp[(9) - (10)].node));
                        fixpos((yyval.node), (yyvsp[(3) - (10)].node));
                    ;}
    break;

  case 319:
#line 1726 "grammar.y"
    {
                        PUSH_LINE("class");
                        if (in_def || in_single)
                            yyerror("class definition in method body");
                        class_nest++;
                        local_push(0);
                        (yyval.num) = ruby_sourceline;
                    ;}
    break;

  case 320:
#line 1736 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_CLASS((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(3) - (6)].node));
                        nd_set_line((yyval.node), (yyvsp[(4) - (6)].num));
                        local_pop();
                        class_nest--;
                    ;}
    break;

  case 321:
#line 1744 "grammar.y"
    {
                        PUSH_LINE("class");
                        (yyval.num) = in_def;
                        in_def = 0;
                    ;}
    break;

  case 322:
#line 1750 "grammar.y"
    {
                        (yyval.num) = in_single;
                        in_single = 0;
                        class_nest++;
                        local_push(0);
                    ;}
    break;

  case 323:
#line 1758 "grammar.y"
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

  case 324:
#line 1768 "grammar.y"
    {
                        PUSH_LINE("module");
                        if (in_def || in_single)
                            yyerror("module definition in method body");
                        class_nest++;
                        local_push(0);
                        (yyval.num) = ruby_sourceline;
                    ;}
    break;

  case 325:
#line 1778 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_MODULE((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
                        nd_set_line((yyval.node), (yyvsp[(3) - (5)].num));
                        local_pop();
                        class_nest--;
                    ;}
    break;

  case 326:
#line 1786 "grammar.y"
    {
                        PUSH_LINE("def");
                        (yyval.id) = cur_mid;
                        cur_mid = (yyvsp[(2) - (2)].id);
                        in_def++;
                        local_push(0);
                    ;}
    break;

  case 327:
#line 1796 "grammar.y"
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

  case 328:
#line 1805 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 329:
#line 1806 "grammar.y"
    {
                        PUSH_LINE("def");
                        in_single++;
                        local_push(0);
                        vps->lex_state = EXPR_END; /* force for args */
                    ;}
    break;

  case 330:
#line 1815 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_DEFS((yyvsp[(2) - (9)].node), (yyvsp[(5) - (9)].id), (yyvsp[(7) - (9)].node), (yyvsp[(8) - (9)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (9)].node));
                        local_pop();
                        in_single--;
                    ;}
    break;

  case 331:
#line 1823 "grammar.y"
    {
                        (yyval.node) = NEW_BREAK(0);
                    ;}
    break;

  case 332:
#line 1827 "grammar.y"
    {
                        (yyval.node) = NEW_NEXT(0);
                    ;}
    break;

  case 333:
#line 1831 "grammar.y"
    {
                        (yyval.node) = NEW_REDO();
                    ;}
    break;

  case 334:
#line 1835 "grammar.y"
    {
                        (yyval.node) = NEW_RETRY();
                    ;}
    break;

  case 335:
#line 1841 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (1)].node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 344:
#line 1862 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(2) - (5)].node), vps), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (5)].node));
                    ;}
    break;

  case 346:
#line 1870 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 349:
#line 1880 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 350:
#line 1884 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 351:
#line 1890 "grammar.y"
    {
                        if ((yyvsp[(1) - (1)].node)->nd_alen == 1) {
                            (yyval.node) = (yyvsp[(1) - (1)].node)->nd_head;
                        }
                        else {
                            (yyval.node) = NEW_MASGN((yyvsp[(1) - (1)].node), 0);
                        }
                    ;}
    break;

  case 352:
#line 1899 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (2)].node), 0);
                    ;}
    break;

  case 353:
#line 1903 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK_VAR((yyvsp[(4) - (4)].node), NEW_MASGN((yyvsp[(1) - (4)].node), 0));
                    ;}
    break;

  case 354:
#line 1907 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK_VAR((yyvsp[(7) - (7)].node), NEW_MASGN((yyvsp[(1) - (7)].node), (yyvsp[(4) - (7)].node)));
                    ;}
    break;

  case 355:
#line 1911 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK_VAR((yyvsp[(6) - (6)].node), NEW_MASGN((yyvsp[(1) - (6)].node), -1));
                    ;}
    break;

  case 356:
#line 1915 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 357:
#line 1919 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (3)].node), -1);
                    ;}
    break;

  case 358:
#line 1923 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK_VAR((yyvsp[(5) - (5)].node), NEW_MASGN(0, (yyvsp[(2) - (5)].node)));
                    ;}
    break;

  case 359:
#line 1927 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK_VAR((yyvsp[(4) - (4)].node), NEW_MASGN(0, -1));
                    ;}
    break;

  case 360:
#line 1931 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(0, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 361:
#line 1935 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(0, -1);
                    ;}
    break;

  case 362:
#line 1939 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK_VAR((yyvsp[(2) - (2)].node), (NODE*)1);
                    ;}
    break;

  case 364:
#line 1946 "grammar.y"
    {
                        (yyval.node) = (NODE*)1;
                    ;}
    break;

  case 365:
#line 1950 "grammar.y"
    {
                        (yyval.node) = (NODE*)1;
                    ;}
    break;

  case 366:
#line 1954 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 367:
#line 1960 "grammar.y"
    {
                        PUSH_LINE("do");
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 368:
#line 1966 "grammar.y"
    {
                      (yyval.vars) = vps->variables->block_vars;
                    ;}
    break;

  case 369:
#line 1971 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 370:
#line 1979 "grammar.y"
    {
                        if ((yyvsp[(1) - (2)].node) && nd_type((yyvsp[(1) - (2)].node)) == NODE_BLOCK_PASS) {
                            rb_compile_error(vps, "both block arg and actual block given");
                        }
                        (yyvsp[(2) - (2)].node)->nd_iter = (yyvsp[(1) - (2)].node);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                        fixpos((yyval.node), (yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 371:
#line 1988 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 372:
#line 1992 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 373:
#line 1998 "grammar.y"
    {
                        (yyval.node) = new_fcall(vps, (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 374:
#line 2003 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 375:
#line 2008 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 376:
#line 2013 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), 0);
                    ;}
    break;

  case 377:
#line 2017 "grammar.y"
    {
                        (yyval.node) = NEW_CALL((yyvsp[(1) - (3)].node), rb_parser_sym("get_reference"),
                        NEW_LIST(NEW_LIT(QUID2SYM((yyvsp[(3) - (3)].id)))));
                    ;}
    break;

  case 378:
#line 2022 "grammar.y"
    {
                        (yyval.node) = NEW_FCALL(rb_parser_sym("get_reference"),
                        NEW_LIST(NEW_LIT(QUID2SYM((yyvsp[(2) - (2)].id)))));
                    ;}
    break;

  case 379:
#line 2027 "grammar.y"
    {
                        (yyval.node) = new_super(vps, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 380:
#line 2031 "grammar.y"
    {
                        (yyval.node) = NEW_ZSUPER();
                    ;}
    break;

  case 381:
#line 2037 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 382:
#line 2041 "grammar.y"
    { (yyval.vars) = vps->variables->block_vars; ;}
    break;

  case 383:
#line 2043 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 384:
#line 2048 "grammar.y"
    {
                        PUSH_LINE("do");
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 385:
#line 2053 "grammar.y"
    { (yyval.vars) = vps->variables->block_vars; ;}
    break;

  case 386:
#line 2055 "grammar.y"
    {
                        POP_LINE();
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 387:
#line 2065 "grammar.y"
    {
                        (yyval.node) = NEW_WHEN((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 389:
#line 2071 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (4)].node), NEW_WHEN((yyvsp[(4) - (4)].node), 0, 0));
                    ;}
    break;

  case 390:
#line 2075 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_WHEN((yyvsp[(2) - (2)].node), 0, 0));
                    ;}
    break;

  case 393:
#line 2087 "grammar.y"
    {
                        if ((yyvsp[(3) - (6)].node)) {
                            (yyvsp[(3) - (6)].node) = node_assign((yyvsp[(3) - (6)].node), NEW_GVAR(rb_parser_sym("$!")), vps);
                            (yyvsp[(5) - (6)].node) = block_append(vps, (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node));
                        }
                        (yyval.node) = NEW_RESBODY((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (6)].node)?(yyvsp[(2) - (6)].node):(yyvsp[(5) - (6)].node));
                    ;}
    break;

  case 395:
#line 2099 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 398:
#line 2107 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 400:
#line 2114 "grammar.y"
    {
                        if ((yyvsp[(2) - (2)].node))
                            (yyval.node) = (yyvsp[(2) - (2)].node);
                        else
                            /* place holder */
                            (yyval.node) = NEW_NIL();
                    ;}
    break;

  case 403:
#line 2126 "grammar.y"
    {
                        (yyval.node) = NEW_LIT(QUID2SYM((yyvsp[(1) - (1)].id)));
                    ;}
    break;

  case 405:
#line 2133 "grammar.y"
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

  case 407:
#line 2147 "grammar.y"
    {
                        (yyval.node) = literal_concat(vps, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 408:
#line 2153 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 409:
#line 2159 "grammar.y"
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

  case 410:
#line 2182 "grammar.y"
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

  case 411:
#line 2212 "grammar.y"
    {
                        (yyval.node) = NEW_ZARRAY();
                    ;}
    break;

  case 412:
#line 2216 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 413:
#line 2222 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 414:
#line 2226 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), evstr2dstr(vps, (yyvsp[(2) - (3)].node)));
                    ;}
    break;

  case 416:
#line 2233 "grammar.y"
    {
                        (yyval.node) = literal_concat(vps, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 417:
#line 2239 "grammar.y"
    {
                        (yyval.node) = NEW_ZARRAY();
                    ;}
    break;

  case 418:
#line 2243 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 419:
#line 2249 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 420:
#line 2253 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 421:
#line 2259 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 422:
#line 2263 "grammar.y"
    {
                        (yyval.node) = literal_concat(vps, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 423:
#line 2269 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 424:
#line 2273 "grammar.y"
    {
                        (yyval.node) = literal_concat(vps, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 426:
#line 2280 "grammar.y"
    {
                        (yyval.node) = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                    ;}
    break;

  case 427:
#line 2286 "grammar.y"
    {
                        lex_strterm = (yyvsp[(2) - (3)].node);
                        (yyval.node) = NEW_EVSTR((yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 428:
#line 2291 "grammar.y"
    {
                        (yyval.node) = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                        COND_PUSH(0);
                        CMDARG_PUSH(0);
                    ;}
    break;

  case 429:
#line 2299 "grammar.y"
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

  case 430:
#line 2310 "grammar.y"
    {(yyval.node) = NEW_GVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 431:
#line 2311 "grammar.y"
    {(yyval.node) = NEW_IVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 432:
#line 2312 "grammar.y"
    {(yyval.node) = NEW_CVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 434:
#line 2317 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = (yyvsp[(2) - (2)].id);
                    ;}
    break;

  case 439:
#line 2330 "grammar.y"
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

  case 442:
#line 2363 "grammar.y"
    {
                        (yyval.node) = NEW_NEGATE((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 443:
#line 2367 "grammar.y"
    {
                        (yyval.node) = NEW_NEGATE((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 449:
#line 2377 "grammar.y"
    {(yyval.id) = kNIL;;}
    break;

  case 450:
#line 2378 "grammar.y"
    {(yyval.id) = kSELF;;}
    break;

  case 451:
#line 2379 "grammar.y"
    {(yyval.id) = kTRUE;;}
    break;

  case 452:
#line 2380 "grammar.y"
    {(yyval.id) = kFALSE;;}
    break;

  case 453:
#line 2381 "grammar.y"
    {(yyval.id) = k__FILE__;;}
    break;

  case 454:
#line 2382 "grammar.y"
    {(yyval.id) = k__LINE__;;}
    break;

  case 455:
#line 2386 "grammar.y"
    {
                        (yyval.node) = gettable((yyvsp[(1) - (1)].id));
                    ;}
    break;

  case 456:
#line 2392 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, vps);
                    ;}
    break;

  case 459:
#line 2402 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 460:
#line 2406 "grammar.y"
    {
                        vps->lex_state = EXPR_BEG;
                    ;}
    break;

  case 461:
#line 2410 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(3) - (4)].node);
                    ;}
    break;

  case 462:
#line 2413 "grammar.y"
    {yyerrok; (yyval.node) = 0;;}
    break;

  case 463:
#line 2417 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                        vps->lex_state = EXPR_BEG;
                        command_start = TRUE;
                    ;}
    break;

  case 464:
#line 2423 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 465:
#line 2429 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS((intptr_t)(yyvsp[(1) - (6)].num), (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].id)), (yyvsp[(6) - (6)].node));
                    ;}
    break;

  case 466:
#line 2433 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS((intptr_t)(yyvsp[(1) - (4)].num), (yyvsp[(3) - (4)].node), 0), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 467:
#line 2437 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS((intptr_t)(yyvsp[(1) - (4)].num), 0, (yyvsp[(3) - (4)].id)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 468:
#line 2441 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS((intptr_t)(yyvsp[(1) - (2)].num), 0, 0), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 469:
#line 2445 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS(0, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 470:
#line 2449 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS(0, (yyvsp[(1) - (2)].node), 0), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 471:
#line 2453 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS(0, 0, (yyvsp[(1) - (2)].id)), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 472:
#line 2457 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS(0, 0, 0), (yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 473:
#line 2461 "grammar.y"
    {
                        (yyval.node) = NEW_ARGS(0, 0, 0);
                    ;}
    break;

  case 474:
#line 2467 "grammar.y"
    {
                        yyerror("formal argument cannot be a constant");
                    ;}
    break;

  case 475:
#line 2471 "grammar.y"
    {
                        yyerror("formal argument cannot be an instance variable");
                    ;}
    break;

  case 476:
#line 2475 "grammar.y"
    {
                        yyerror("formal argument cannot be a global variable");
                    ;}
    break;

  case 477:
#line 2479 "grammar.y"
    {
                        yyerror("formal argument cannot be a class variable");
                    ;}
    break;

  case 478:
#line 2483 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(1) - (1)].id)))
                            yyerror("formal argument must be local variable");
                        else if (local_id((yyvsp[(1) - (1)].id)))
                            yyerror("duplicate argument name");
                        local_cnt((yyvsp[(1) - (1)].id));
                        (yyval.num) = 1;
                    ;}
    break;

  case 480:
#line 2495 "grammar.y"
    {
                        (yyval.num) += 1;
                    ;}
    break;

  case 481:
#line 2501 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(1) - (3)].id)))
                            yyerror("formal argument must be local variable");
                        else if (local_id((yyvsp[(1) - (3)].id)))
                            yyerror("duplicate optional argument name");
                        (yyval.node) = assignable((yyvsp[(1) - (3)].id), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 482:
#line 2511 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK((yyvsp[(1) - (1)].node));
                        (yyval.node)->nd_end = (yyval.node);
                    ;}
    break;

  case 483:
#line 2516 "grammar.y"
    {
                        (yyval.node) = block_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 486:
#line 2526 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(2) - (2)].id)))
                            yyerror("rest argument must be local variable");
                        else if (local_id((yyvsp[(2) - (2)].id)))
                            yyerror("duplicate rest argument name");
                        (yyval.id) = local_cnt((yyvsp[(2) - (2)].id)) + 1;
                    ;}
    break;

  case 487:
#line 2534 "grammar.y"
    {
                        (yyval.id) = -2;
                    ;}
    break;

  case 490:
#line 2544 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(2) - (2)].id)))
                            yyerror("block argument must be local variable");
                        else if (local_id((yyvsp[(2) - (2)].id)))
                            yyerror("duplicate block argument name");
                        (yyval.node) = NEW_BLOCK_ARG((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 491:
#line 2554 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 493:
#line 2561 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                        value_expr((yyval.node));
                    ;}
    break;

  case 494:
#line 2565 "grammar.y"
    {vps->lex_state = EXPR_BEG;;}
    break;

  case 495:
#line 2566 "grammar.y"
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

  case 497:
#line 2592 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 498:
#line 2596 "grammar.y"
    {
                        if ((yyvsp[(1) - (2)].node)->nd_alen%2 != 0) {
                            yyerror("odd number list for Hash");
                        }
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 500:
#line 2606 "grammar.y"
    {
                        (yyval.node) = list_concat((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 501:
#line 2612 "grammar.y"
    {
                        (yyval.node) = list_append(vps, NEW_LIST((yyvsp[(1) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 521:
#line 2650 "grammar.y"
    {yyerrok;;}
    break;

  case 524:
#line 2655 "grammar.y"
    {yyerrok;;}
    break;

  case 525:
#line 2658 "grammar.y"
    {(yyval.node) = 0;;}
    break;


/* Line 1267 of yacc.c.  */
#line 7637 "grammar.cpp"
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


#line 2660 "grammar.y"


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
    int cmd_state;
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
                        // Hack. Ignore the different variants of do
                        // if we're just trying to match a FNAME
                        if(kw->id[0] == kDO) return kDO;
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


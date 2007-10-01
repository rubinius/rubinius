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

#include "grammar_internal.h"
#include "grammar_runtime.h"
#include "array.h"

static NODE *syd_node_newnode(rb_parse_state*, enum node_type, OBJECT, OBJECT, OBJECT);

#undef VALUE

#define ALLOC_N(what, many) ((what*)malloc(sizeof(what) * (many)))
#define REALLOC_N(ptr, what, many) ptr = realloc(ptr, sizeof(what) * (many))

#ifndef isnumber
#define isnumber isdigit
#endif

#define ISALPHA isalpha
#define ISSPACE isspace
#define ISALNUM(x) (isalpha(x) || isnumber(x))
#define ISDIGIT isdigit
#define ISXDIGIT isxdigit
#define ISUPPER isupper

#define ismbchar(c) (0)
#define mbclen(c) (1)

#define ID2SYM(i) i

#define string_new(ptr, len) blk2bstr(ptr, len)
#define string_new2(ptr) cstr2bstr(ptr)

static int syd_sourceline;
static char *syd_sourcefile;

#define ruby_sourceline syd_sourceline
#define ruby_sourcefile syd_sourcefile

static int
syd_yyerror(const char *, rb_parse_state*);
#define yyparse syd_yyparse
#define yylex syd_yylex
#define yyerror(str) syd_yyerror(str, parse_state)
#define yylval syd_yylval
#define yychar syd_yychar
#define yydebug syd_yydebug

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
static int yylex();


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
static void void_expr0(NODE *);
static void void_stmts(NODE*,rb_parse_state*);
static NODE *remove_begin(NODE*);
#define  value_expr(node)  value_expr0((node) = remove_begin(node), parse_state)
#define void_expr(node) void_expr0((node) = remove_begin(node))

static NODE *block_append(rb_parse_state*,NODE*,NODE*);
static NODE *list_append(rb_parse_state*,NODE*,NODE*);
static NODE *list_concat(NODE*,NODE*);
static NODE *arg_concat(rb_parse_state*,NODE*,NODE*);
static NODE *arg_prepend(rb_parse_state*,NODE*,NODE*);
static NODE *literal_concat(rb_parse_state*,NODE*,NODE*);
static NODE *new_evstr(rb_parse_state*,NODE*);
static NODE *evstr2dstr(rb_parse_state*,NODE*);
static NODE *call_op(NODE*,ID,int,NODE*,rb_parse_state*);

/* static NODE *negate_lit(NODE*); */
static NODE *ret_args(rb_parse_state*,NODE*);
static NODE *arg_blk_pass(NODE*,NODE*);
static NODE *new_call(rb_parse_state*,NODE*,ID,NODE*);
static NODE *new_fcall(rb_parse_state*,ID,NODE*);
static NODE *new_super(rb_parse_state*,NODE*);
static NODE *new_yield(rb_parse_state*,NODE*);

static NODE *syd_gettable(rb_parse_state*,ID);
#define gettable(i) syd_gettable(parse_state, i)
static NODE *assignable(ID,NODE*,rb_parse_state*);
static NODE *aryset(NODE*,NODE*,rb_parse_state*);
static NODE *attrset(NODE*,ID,rb_parse_state*);
static void rb_backref_error(NODE*);
static NODE *node_assign(NODE*,NODE*,rb_parse_state*);

static NODE *match_gen(NODE*,NODE*,rb_parse_state*);
static void syd_local_push(rb_parse_state*, int cnt);
#define local_push(cnt) syd_local_push(vps, cnt)
static void syd_local_pop(rb_parse_state*);
#define local_pop() syd_local_pop(vps)
static int  syd_local_append(rb_parse_state*,ID);
#define local_append(i) syd_local_append(vps, i)
static int  syd_local_cnt(rb_parse_state*,ID);
#define local_cnt(i) syd_local_cnt(vps, i)
static int  syd_local_id(rb_parse_state*,ID);
#define local_id(i) syd_local_id(vps, i)
static ID  *syd_local_tbl();
static ID   internal_id();
static ID   convert_op();

static void tokadd(char c, rb_parse_state *parse_state);
static int tokadd_string(int, int, int, int *, rb_parse_state*);
 
#define SHOW_PARSER_WARNS 0
 
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
 
//static void syd_rb_warn(char *fmt, ...);
#define rb_warn _debug_print
#define rb_warning _debug_print

// #define rb_warn printf
// #define rb_warning printf

//static void syd_compile_error(char *fmt, ...);
// #define rb_compile_error _debug_print

// #define rb_compile_error printf
#define rb_compile_error _debug_print

static ID rb_intern(const char *name);
static ID rb_id_attrset(ID);

rb_parse_state *alloc_parse_state();
static unsigned long scan_oct(const char *start, int len, int *retlen);
static unsigned long scan_hex(const char *start, int len, int *retlen);

static void reset_block(rb_parse_state *parse_state);
static NODE *extract_block_vars(rb_parse_state *parse_state, NODE* node, var_table *vars);

#define ruby_verbose 0

/*
static VALUE syd_dyna_push();
#define dyna_push() syd_dyna_push(vps)
static void syd_dyna_pop();
#define dyna_pop(i) syd_dyna_pop(vps, i)
static int syd_dyna_in_block();
#define dyna_in_block(vps)
static NODE *syd_dyna_init();
#define dyna_init(a, b) syd_dyna_init(vps, a, b)

#define ruby_dyna_vars syd_dyna_vars(vps)
static VALUE syd_dyna_vars();

#define rb_dvar_defined(id) syd_dvar_defined(vps, id)
static VALUE syd_dvar_defined();

#define rb_dvar_curr(id) syd_dvar_curr(vps, id)
static VALUE syd_dvar_curr();

#define rb_dvar_push(id, val) syd_dvar_push(vps, id, val)
static void syd_dvar_push();
*/
// static void top_local_init();
// static void top_local_setup();

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
#line 292 "grammar.y"
{
    NODE *node;
    ID id;
    int num;
    var_table *vars;
}
/* Line 193 of yacc.c.  */
#line 590 "grammar.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 603 "grammar.c"

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
#define YYLAST   9806

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  134
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  144
/* YYNRULES -- Number of rules.  */
#define YYNRULES  504
/* YYNRULES -- Number of states.  */
#define YYNSTATES  901

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
     340,   342,   344,   346,   348,   350,   351,   356,   358,   360,
     362,   364,   366,   368,   370,   372,   374,   376,   378,   380,
     382,   384,   386,   388,   390,   392,   394,   396,   398,   400,
     402,   404,   406,   408,   410,   412,   414,   416,   418,   420,
     422,   424,   426,   428,   430,   432,   434,   436,   438,   440,
     442,   444,   446,   448,   450,   452,   454,   456,   458,   460,
     462,   464,   466,   468,   470,   472,   474,   476,   478,   480,
     482,   484,   486,   488,   490,   492,   496,   502,   506,   513,
     519,   525,   531,   537,   542,   546,   550,   554,   558,   562,
     566,   570,   574,   578,   583,   588,   591,   594,   598,   602,
     606,   610,   614,   618,   622,   626,   630,   634,   638,   642,
     646,   649,   652,   656,   660,   664,   668,   669,   674,   675,
     682,   684,   686,   688,   691,   694,   700,   703,   707,   711,
     716,   721,   728,   730,   732,   734,   737,   743,   746,   752,
     757,   765,   769,   771,   776,   780,   786,   794,   797,   803,
     808,   815,   823,   833,   837,   839,   840,   843,   845,   846,
     850,   851,   856,   859,   862,   864,   866,   870,   874,   879,
     882,   884,   886,   888,   890,   892,   894,   896,   898,   900,
     901,   906,   907,   913,   917,   921,   924,   929,   933,   937,
     939,   944,   948,   950,   951,   958,   961,   963,   966,   973,
     980,   981,   982,   990,   991,   992,  1000,  1006,  1011,  1017,
    1018,  1019,  1029,  1030,  1037,  1038,  1039,  1048,  1049,  1055,
    1056,  1063,  1064,  1065,  1075,  1077,  1079,  1081,  1083,  1085,
    1087,  1089,  1091,  1094,  1096,  1098,  1100,  1102,  1108,  1110,
    1113,  1115,  1117,  1119,  1122,  1124,  1128,  1129,  1130,  1137,
    1140,  1145,  1150,  1153,  1158,  1163,  1167,  1171,  1174,  1177,
    1179,  1180,  1181,  1188,  1189,  1190,  1197,  1203,  1205,  1210,
    1213,  1215,  1217,  1224,  1226,  1228,  1230,  1232,  1235,  1237,
    1240,  1242,  1244,  1246,  1248,  1250,  1252,  1255,  1259,  1263,
    1267,  1271,  1275,  1276,  1280,  1282,  1285,  1289,  1293,  1294,
    1298,  1299,  1302,  1303,  1306,  1308,  1309,  1313,  1314,  1319,
    1321,  1323,  1325,  1327,  1330,  1332,  1334,  1336,  1338,  1342,
    1344,  1346,  1349,  1352,  1354,  1356,  1358,  1360,  1362,  1364,
    1366,  1368,  1370,  1372,  1374,  1376,  1378,  1380,  1382,  1384,
    1385,  1390,  1393,  1398,  1401,  1408,  1413,  1418,  1421,  1426,
    1429,  1432,  1434,  1435,  1437,  1439,  1441,  1443,  1445,  1447,
    1451,  1455,  1457,  1461,  1463,  1465,  1468,  1470,  1472,  1474,
    1477,  1480,  1482,  1484,  1485,  1491,  1493,  1496,  1499,  1501,
    1505,  1509,  1511,  1513,  1515,  1517,  1519,  1521,  1523,  1525,
    1527,  1529,  1531,  1533,  1534,  1536,  1537,  1539,  1540,  1542,
    1544,  1546,  1548,  1550,  1553
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     135,     0,    -1,    -1,   136,   138,    -1,   138,   221,   205,
     224,    -1,   139,   272,    -1,   277,    -1,   140,    -1,   139,
     276,   140,    -1,     1,   140,    -1,    -1,    44,   161,   141,
     161,    -1,    44,    52,    52,    -1,    44,    52,    61,    -1,
      44,    52,    60,    -1,     6,   162,    -1,   140,    39,   144,
      -1,   140,    40,   144,    -1,   140,    41,   144,    -1,   140,
      42,   144,    -1,   140,    43,   140,    -1,    -1,    46,   142,
     121,   138,   122,    -1,    47,   121,   138,   122,    -1,   157,
     104,   145,    -1,   151,   104,   145,    -1,   248,    85,   145,
      -1,   201,   123,   170,   124,    85,   145,    -1,   201,   125,
      50,    85,   145,    -1,   201,   125,    54,    85,   145,    -1,
     201,    83,    50,    85,   145,    -1,   249,    85,   145,    -1,
     157,   104,   183,    -1,   151,   104,   169,    -1,   151,   104,
     183,    -1,   143,    -1,   145,    -1,   143,    36,   143,    -1,
     143,    37,   143,    -1,    38,   143,    -1,   118,   145,    -1,
     166,    -1,   143,    -1,   150,    -1,   146,    -1,    29,   173,
      -1,    21,   173,    -1,    22,   173,    -1,   211,    -1,   211,
     125,   269,   175,    -1,   211,    83,   269,   175,    -1,    -1,
      -1,    92,   148,   207,   149,   138,   122,    -1,   268,   175,
      -1,   268,   175,   147,    -1,   201,   125,   269,   175,    -1,
     201,   125,   269,   175,   147,    -1,   201,    83,   269,   175,
      -1,   201,    83,   269,   175,   147,    -1,    31,   175,    -1,
      30,   175,    -1,   153,    -1,    87,   152,   126,    -1,   153,
      -1,    87,   152,   126,    -1,   155,    -1,   155,   154,    -1,
     155,    93,   156,    -1,   155,    93,    -1,    93,   156,    -1,
      93,    -1,   156,    -1,    87,   152,   126,    -1,   154,   127,
      -1,   155,   154,   127,    -1,   246,    -1,   201,   123,   170,
     124,    -1,   201,   125,    50,    -1,   201,    83,    50,    -1,
     201,   125,    54,    -1,   201,    83,    54,    -1,    84,    54,
      -1,   249,    -1,   246,    -1,   201,   123,   170,   124,    -1,
     201,   125,    50,    -1,   201,    83,    50,    -1,   201,   125,
      54,    -1,   201,    83,    54,    -1,    84,    54,    -1,   249,
      -1,    50,    -1,    54,    -1,    84,   158,    -1,   158,    -1,
     201,    83,   158,    -1,    50,    -1,    54,    -1,    51,    -1,
     164,    -1,   165,    -1,   160,    -1,   242,    -1,   161,    -1,
      -1,   162,   127,   163,   161,    -1,   109,    -1,   110,    -1,
     111,    -1,    67,    -1,    68,    -1,    70,    -1,    69,    -1,
      75,    -1,   107,    -1,    71,    -1,   108,    -1,    72,    -1,
      81,    -1,    82,    -1,   112,    -1,   113,    -1,   114,    -1,
      93,    -1,   115,    -1,   116,    -1,    66,    -1,   119,    -1,
      63,    -1,    64,    -1,    79,    -1,    80,    -1,   128,    -1,
      48,    -1,    49,    -1,    46,    -1,    47,    -1,    44,    -1,
      36,    -1,     7,    -1,    21,    -1,    16,    -1,     3,    -1,
       5,    -1,    45,    -1,    26,    -1,    15,    -1,    14,    -1,
      10,    -1,     9,    -1,    35,    -1,    20,    -1,    25,    -1,
       4,    -1,    22,    -1,    33,    -1,    38,    -1,    37,    -1,
      23,    -1,     8,    -1,    24,    -1,    29,    -1,    32,    -1,
      31,    -1,    13,    -1,    34,    -1,     6,    -1,    17,    -1,
      30,    -1,    39,    -1,    40,    -1,    41,    -1,    42,    -1,
      43,    -1,   157,   104,   166,    -1,   157,   104,   166,    43,
     166,    -1,   248,    85,   166,    -1,   201,   123,   170,   124,
      85,   166,    -1,   201,   125,    50,    85,   166,    -1,   201,
     125,    54,    85,   166,    -1,   201,    83,    50,    85,   166,
      -1,   201,    83,    54,    85,   166,    -1,    84,    54,    85,
     166,    -1,   249,    85,   166,    -1,   166,    77,   166,    -1,
     166,    78,   166,    -1,   166,   112,   166,    -1,   166,   113,
     166,    -1,   166,   114,   166,    -1,   166,   115,   166,    -1,
     166,   116,   166,    -1,   166,    66,   166,    -1,   117,    57,
      66,   166,    -1,   117,    58,    66,   166,    -1,    63,   166,
      -1,    64,   166,    -1,   166,   109,   166,    -1,   166,   110,
     166,    -1,   166,   111,   166,    -1,   166,    67,   166,    -1,
     166,   107,   166,    -1,   166,    71,   166,    -1,   166,   108,
     166,    -1,   166,    72,   166,    -1,   166,    68,   166,    -1,
     166,    69,   166,    -1,   166,    70,   166,    -1,   166,    75,
     166,    -1,   166,    76,   166,    -1,   118,   166,    -1,   119,
     166,    -1,   166,    81,   166,    -1,   166,    82,   166,    -1,
     166,    73,   166,    -1,   166,    74,   166,    -1,    -1,    45,
     273,   167,   166,    -1,    -1,   166,   105,   168,   166,   106,
     166,    -1,   184,    -1,   166,    -1,   277,    -1,   150,   273,
      -1,   182,   274,    -1,   182,   127,    93,   166,   273,    -1,
     266,   274,    -1,    93,   166,   273,    -1,   129,   277,   126,
      -1,   129,   173,   273,   126,    -1,   129,   211,   273,   126,
      -1,   129,   182,   127,   211,   273,   126,    -1,   277,    -1,
     171,    -1,   150,    -1,   182,   181,    -1,   182,   127,    93,
     169,   181,    -1,   266,   181,    -1,   266,   127,    93,   169,
     181,    -1,   182,   127,   266,   181,    -1,   182,   127,   266,
     127,    93,   166,   181,    -1,    93,   169,   181,    -1,   180,
      -1,   169,   127,   182,   181,    -1,   169,   127,   180,    -1,
     169,   127,    93,   169,   181,    -1,   169,   127,   182,   127,
      93,   169,   181,    -1,   266,   181,    -1,   266,   127,    93,
     169,   181,    -1,   169,   127,   266,   181,    -1,   169,   127,
     182,   127,   266,   181,    -1,   169,   127,   266,   127,    93,
     169,   181,    -1,   169,   127,   182,   127,   266,   127,    93,
     169,   181,    -1,    93,   169,   181,    -1,   180,    -1,    -1,
     176,   177,    -1,   173,    -1,    -1,    88,   178,   126,    -1,
      -1,    88,   174,   179,   126,    -1,    94,   169,    -1,   127,
     180,    -1,   277,    -1,   169,    -1,   182,   127,   169,    -1,
     182,   127,   169,    -1,   182,   127,    93,   169,    -1,    93,
     169,    -1,   225,    -1,   226,    -1,   229,    -1,   230,    -1,
     231,    -1,   234,    -1,   247,    -1,   249,    -1,    51,    -1,
      -1,     7,   185,   137,    10,    -1,    -1,    88,   143,   186,
     273,   126,    -1,    87,   138,   126,    -1,   201,    83,    54,
      -1,    84,    54,    -1,   201,   123,   170,   124,    -1,    90,
     170,   124,    -1,    91,   265,   122,    -1,    29,    -1,    30,
     129,   173,   126,    -1,    30,   129,   126,    -1,    30,    -1,
      -1,    45,   273,   129,   187,   143,   126,    -1,   268,   213,
      -1,   212,    -1,   212,   213,    -1,    11,   144,   202,   138,
     204,    10,    -1,    12,   144,   202,   138,   205,    10,    -1,
      -1,    -1,    18,   188,   144,   203,   189,   138,    10,    -1,
      -1,    -1,    19,   190,   144,   203,   191,   138,    10,    -1,
      16,   144,   272,   218,    10,    -1,    16,   272,   218,    10,
      -1,    16,   272,    15,   138,    10,    -1,    -1,    -1,    20,
     206,    25,   192,   144,   203,   193,   138,    10,    -1,    -1,
       3,   159,   250,   194,   137,    10,    -1,    -1,    -1,     3,
      81,   143,   195,   275,   196,   137,    10,    -1,    -1,     4,
     159,   197,   137,    10,    -1,    -1,     5,   160,   198,   252,
     137,    10,    -1,    -1,    -1,     5,   263,   271,   199,   160,
     200,   252,   137,    10,    -1,    21,    -1,    22,    -1,    23,
      -1,    24,    -1,   184,    -1,   275,    -1,   106,    -1,    13,
      -1,   275,    13,    -1,   275,    -1,   106,    -1,    27,    -1,
     205,    -1,    14,   144,   202,   138,   204,    -1,   277,    -1,
      15,   138,    -1,   157,    -1,   151,    -1,   277,    -1,   109,
     109,    -1,    74,    -1,   109,   206,   109,    -1,    -1,    -1,
      28,   209,   207,   210,   138,    10,    -1,   150,   208,    -1,
     211,   125,   269,   172,    -1,   211,    83,   269,   172,    -1,
     268,   171,    -1,   201,   125,   269,   172,    -1,   201,    83,
     269,   171,    -1,   201,    83,   270,    -1,   201,   130,   269,
      -1,    65,   269,    -1,    31,   171,    -1,    31,    -1,    -1,
      -1,   121,   214,   207,   215,   138,   122,    -1,    -1,    -1,
      26,   216,   207,   217,   138,    10,    -1,    17,   219,   202,
     138,   220,    -1,   182,    -1,   182,   127,    93,   169,    -1,
      93,   169,    -1,   205,    -1,   218,    -1,     8,   222,   223,
     202,   138,   221,    -1,   277,    -1,   169,    -1,   183,    -1,
     277,    -1,    86,   157,    -1,   277,    -1,     9,   138,    -1,
     277,    -1,   245,    -1,   242,    -1,   244,    -1,   227,    -1,
     228,    -1,   227,   228,    -1,    96,   236,   102,    -1,    56,
     237,   102,    -1,    97,   237,    62,    -1,    98,   131,   102,
      -1,    98,   232,   102,    -1,    -1,   232,   233,   131,    -1,
     238,    -1,   233,   238,    -1,    99,   131,   102,    -1,    99,
     235,   102,    -1,    -1,   235,    59,   131,    -1,    -1,   236,
     238,    -1,    -1,   237,   238,    -1,    59,    -1,    -1,   101,
     239,   241,    -1,    -1,   100,   240,   138,   122,    -1,    52,
      -1,    53,    -1,    55,    -1,   249,    -1,    95,   243,    -1,
     160,    -1,    53,    -1,    52,    -1,    55,    -1,    95,   237,
     102,    -1,    57,    -1,    58,    -1,   117,    57,    -1,   117,
      58,    -1,    50,    -1,    53,    -1,    52,    -1,    54,    -1,
      55,    -1,    33,    -1,    32,    -1,    34,    -1,    35,    -1,
      49,    -1,    48,    -1,   246,    -1,   246,    -1,    60,    -1,
      61,    -1,   275,    -1,    -1,   108,   251,   144,   275,    -1,
       1,   275,    -1,   129,   253,   273,   126,    -1,   253,   275,
      -1,   255,   127,   257,   127,   259,   262,    -1,   255,   127,
     257,   262,    -1,   255,   127,   259,   262,    -1,   255,   262,
      -1,   257,   127,   259,   262,    -1,   257,   262,    -1,   259,
     262,    -1,   261,    -1,    -1,    54,    -1,    53,    -1,    52,
      -1,    55,    -1,    50,    -1,   254,    -1,   255,   127,   254,
      -1,    50,   104,   169,    -1,   256,    -1,   257,   127,   256,
      -1,   114,    -1,    93,    -1,   258,    50,    -1,   258,    -1,
     111,    -1,    94,    -1,   260,    50,    -1,   127,   261,    -1,
     277,    -1,   247,    -1,    -1,   129,   264,   143,   273,   126,
      -1,   277,    -1,   266,   274,    -1,   182,   274,    -1,   267,
      -1,   266,   127,   267,    -1,   169,    86,   169,    -1,    50,
      -1,    54,    -1,    51,    -1,    50,    -1,    54,    -1,    51,
      -1,   164,    -1,    50,    -1,    51,    -1,   164,    -1,   125,
      -1,    83,    -1,    -1,   276,    -1,    -1,   132,    -1,    -1,
     132,    -1,   127,    -1,   133,    -1,   132,    -1,   275,    -1,
     276,   133,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   429,   429,   429,   460,   480,   487,   488,   492,   496,
     502,   502,   506,   510,   517,   522,   526,   535,   544,   556,
     568,   573,   572,   588,   596,   600,   606,   631,   650,   662,
     674,   686,   691,   695,   700,   705,   708,   709,   713,   717,
     721,   725,   728,   735,   736,   737,   741,   745,   751,   752,
     756,   763,   767,   762,   776,   781,   793,   798,   810,   815,
     827,   832,   839,   840,   846,   847,   853,   857,   861,   865,
     869,   873,   879,   880,   886,   890,   896,   900,   904,   908,
     912,   916,   922,   928,   935,   939,   943,   947,   951,   955,
     961,   967,   974,   978,   981,   985,   989,   995,   996,   997,
     998,  1003,  1010,  1011,  1014,  1018,  1018,  1024,  1025,  1026,
    1027,  1028,  1029,  1030,  1031,  1032,  1033,  1034,  1035,  1036,
    1037,  1038,  1039,  1040,  1041,  1042,  1043,  1044,  1045,  1046,
    1047,  1048,  1049,  1050,  1053,  1053,  1053,  1053,  1054,  1054,
    1054,  1054,  1054,  1054,  1054,  1055,  1055,  1055,  1055,  1055,
    1055,  1055,  1056,  1056,  1056,  1056,  1056,  1056,  1057,  1057,
    1057,  1057,  1057,  1057,  1057,  1058,  1058,  1058,  1058,  1058,
    1059,  1059,  1059,  1059,  1059,  1062,  1066,  1070,  1095,  1114,
    1126,  1138,  1150,  1155,  1160,  1165,  1171,  1177,  1181,  1185,
    1189,  1193,  1197,  1201,  1205,  1209,  1213,  1217,  1221,  1225,
    1229,  1233,  1237,  1241,  1245,  1249,  1253,  1257,  1261,  1265,
    1269,  1273,  1277,  1281,  1285,  1289,  1293,  1293,  1298,  1298,
    1304,  1310,  1317,  1318,  1323,  1327,  1332,  1336,  1343,  1347,
    1351,  1356,  1363,  1364,  1367,  1372,  1376,  1381,  1386,  1391,
    1396,  1402,  1406,  1409,  1413,  1417,  1422,  1427,  1432,  1437,
    1442,  1447,  1452,  1457,  1461,  1464,  1464,  1476,  1477,  1477,
    1482,  1482,  1489,  1495,  1499,  1502,  1506,  1512,  1516,  1520,
    1526,  1527,  1528,  1529,  1530,  1531,  1532,  1533,  1534,  1539,
    1538,  1551,  1551,  1556,  1560,  1564,  1568,  1577,  1586,  1590,
    1594,  1598,  1602,  1606,  1606,  1611,  1617,  1618,  1627,  1640,
    1653,  1653,  1653,  1663,  1663,  1663,  1673,  1680,  1684,  1688,
    1688,  1688,  1696,  1695,  1712,  1717,  1711,  1734,  1733,  1750,
    1749,  1767,  1768,  1767,  1782,  1786,  1790,  1794,  1800,  1807,
    1808,  1809,  1810,  1813,  1814,  1815,  1818,  1819,  1828,  1829,
    1835,  1836,  1839,  1840,  1844,  1848,  1855,  1860,  1854,  1871,
    1880,  1884,  1890,  1895,  1900,  1905,  1909,  1913,  1917,  1921,
    1928,  1932,  1927,  1939,  1943,  1938,  1951,  1958,  1959,  1963,
    1969,  1970,  1973,  1984,  1987,  1991,  1992,  1995,  1999,  2002,
    2010,  2013,  2014,  2018,  2021,  2034,  2035,  2041,  2047,  2076,
    2111,  2115,  2122,  2125,  2131,  2132,  2138,  2142,  2149,  2152,
    2159,  2162,  2169,  2172,  2178,  2180,  2179,  2191,  2190,  2211,
    2212,  2213,  2214,  2217,  2224,  2225,  2226,  2227,  2230,  2261,
    2262,  2263,  2267,  2273,  2274,  2275,  2276,  2277,  2278,  2279,
    2280,  2281,  2282,  2283,  2286,  2292,  2298,  2299,  2302,  2307,
    2306,  2314,  2317,  2322,  2328,  2333,  2337,  2342,  2346,  2351,
    2355,  2360,  2365,  2370,  2374,  2378,  2382,  2386,  2397,  2398,
    2404,  2414,  2419,  2425,  2426,  2429,  2437,  2444,  2445,  2448,
    2458,  2462,  2465,  2475,  2475,  2500,  2501,  2505,  2514,  2515,
    2521,  2527,  2528,  2529,  2532,  2533,  2534,  2535,  2538,  2539,
    2540,  2543,  2544,  2547,  2548,  2551,  2552,  2555,  2556,  2557,
    2560,  2561,  2564,  2565,  2568
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
  "cname", "cpath", "fname", "fitem", "undef_list", "@6", "op", "reswords",
  "arg", "@7", "@8", "arg_value", "aref_args", "paren_args",
  "opt_paren_args", "call_args", "call_args2", "command_args", "@9",
  "open_args", "@10", "@11", "block_arg", "opt_block_arg", "args", "mrhs",
  "primary", "@12", "@13", "@14", "@15", "@16", "@17", "@18", "@19", "@20",
  "@21", "@22", "@23", "@24", "@25", "@26", "@27", "primary_value", "then",
  "do", "if_tail", "opt_else", "block_var", "opt_block_var", "do_block",
  "@28", "@29", "block_call", "method_call", "brace_block", "@30", "@31",
  "@32", "@33", "case_body", "when_args", "cases", "opt_rescue",
  "exc_list", "exc_var", "opt_ensure", "literal", "strings", "string",
  "string1", "xstring", "regexp", "words", "word_list", "word", "qwords",
  "qword_list", "string_contents", "xstring_contents", "string_content",
  "@34", "@35", "string_dvar", "symbol", "sym", "dsym", "numeric",
  "variable", "var_ref", "var_lhs", "backref", "superclass", "@36",
  "f_arglist", "f_args", "f_norm_arg", "f_arg", "f_opt", "f_optarg",
  "restarg_mark", "f_rest_arg", "blkarg_mark", "f_block_arg",
  "opt_f_block_arg", "singleton", "@37", "assoc_list", "assocs", "assoc",
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
     160,   160,   161,   161,   162,   163,   162,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   167,   166,   168,   166,
     166,   169,   170,   170,   170,   170,   170,   170,   171,   171,
     171,   171,   172,   172,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   176,   175,   177,   178,   177,
     179,   177,   180,   181,   181,   182,   182,   183,   183,   183,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   185,
     184,   186,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   187,   184,   184,   184,   184,   184,   184,
     188,   189,   184,   190,   191,   184,   184,   184,   184,   192,
     193,   184,   194,   184,   195,   196,   184,   197,   184,   198,
     184,   199,   200,   184,   184,   184,   184,   184,   201,   202,
     202,   202,   202,   203,   203,   203,   204,   204,   205,   205,
     206,   206,   207,   207,   207,   207,   209,   210,   208,   211,
     211,   211,   212,   212,   212,   212,   212,   212,   212,   212,
     214,   215,   213,   216,   217,   213,   218,   219,   219,   219,
     220,   220,   221,   221,   222,   222,   222,   223,   223,   224,
     224,   225,   225,   225,   226,   227,   227,   228,   229,   230,
     231,   231,   232,   232,   233,   233,   234,   234,   235,   235,
     236,   236,   237,   237,   238,   239,   238,   240,   238,   241,
     241,   241,   241,   242,   243,   243,   243,   243,   244,   245,
     245,   245,   245,   246,   246,   246,   246,   246,   246,   246,
     246,   246,   246,   246,   247,   248,   249,   249,   250,   251,
     250,   250,   252,   252,   253,   253,   253,   253,   253,   253,
     253,   253,   253,   254,   254,   254,   254,   254,   255,   255,
     256,   257,   257,   258,   258,   259,   259,   260,   260,   261,
     262,   262,   263,   264,   263,   265,   265,   265,   266,   266,
     267,   268,   268,   268,   269,   269,   269,   269,   270,   270,
     270,   271,   271,   272,   272,   273,   273,   274,   274,   274,
     275,   275,   276,   276,   277
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
       1,     1,     1,     1,     1,     0,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     5,     3,     6,     5,
       5,     5,     5,     4,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     4,     4,     2,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     2,     3,     3,     3,     3,     0,     4,     0,     6,
       1,     1,     1,     2,     2,     5,     2,     3,     3,     4,
       4,     6,     1,     1,     1,     2,     5,     2,     5,     4,
       7,     3,     1,     4,     3,     5,     7,     2,     5,     4,
       6,     7,     9,     3,     1,     0,     2,     1,     0,     3,
       0,     4,     2,     2,     1,     1,     3,     3,     4,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       4,     0,     5,     3,     3,     2,     4,     3,     3,     1,
       4,     3,     1,     0,     6,     2,     1,     2,     6,     6,
       0,     0,     7,     0,     0,     7,     5,     4,     5,     0,
       0,     9,     0,     6,     0,     0,     8,     0,     5,     0,
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
       2,     0,     0,     1,     0,     0,     0,     0,     0,   279,
       0,     0,   493,   300,   303,     0,   324,   325,   326,   327,
     289,   292,   359,   429,   428,   430,   431,     0,     0,   495,
      21,     0,   433,   432,   423,   278,   425,   424,   426,   427,
     402,   419,   420,   436,   437,     0,     0,     0,     0,     0,
       0,   504,   504,    71,   402,   400,   402,   392,   398,     0,
       0,     0,     3,   493,     7,    35,    36,    44,    43,     0,
      62,     0,    66,    72,     0,    41,   220,     0,    48,   296,
     270,   271,   384,   385,   272,   273,   274,   275,   382,   383,
     381,   434,   276,     0,   277,   255,     6,     9,   324,   325,
     289,   292,   359,   495,    92,    93,     0,     0,     0,     0,
      95,     0,   328,     0,   434,   277,     0,   317,   143,   154,
     144,   167,   140,   160,   150,   149,   165,   148,   147,   142,
     168,   152,   141,   155,   159,   161,   153,   146,   162,   169,
     164,   163,   156,   166,   151,   139,   158,   157,   170,   171,
     172,   173,   174,   138,   145,   136,   137,   134,   135,    97,
      99,    98,   129,   130,   127,   110,   111,   113,   112,   116,
     118,   114,   131,   132,   119,   120,   124,   115,   117,   107,
     108,   109,   121,   122,   123,   125,   126,   128,   133,   473,
     319,   100,   101,   472,     0,   163,   156,   166,   151,   134,
     135,    97,    98,     0,   102,   104,    15,   103,     0,     0,
      42,     0,     0,     0,   434,     0,   277,     0,   501,   500,
     493,     0,   502,   494,     0,     0,     0,   341,   340,     0,
       0,   434,   277,     0,     0,     0,   234,   221,   265,    46,
     242,   504,   504,   478,    47,    45,     0,    61,     0,   504,
     358,    60,    39,     0,    10,   496,   216,     0,     0,     0,
     195,     0,   196,   484,   486,   485,   487,   357,   285,     0,
       0,     0,    62,   281,     0,   495,     0,   497,   497,   222,
     497,     0,   497,   475,     0,    70,     0,    76,    83,   416,
     415,   417,   414,     0,   413,     0,     0,     0,     0,     0,
       0,   421,   422,    40,   210,   211,     5,   494,     0,     0,
       0,     0,     0,     0,     0,   346,   349,     0,    74,     0,
      69,    67,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   218,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     504,     0,     0,     0,     0,   363,   360,   297,   386,     0,
       0,   352,    54,   295,     0,   314,    92,    93,    94,   421,
     422,     0,   439,   312,   438,     0,   504,     0,     0,     0,
     452,   492,   491,   321,   105,     0,   504,   285,   331,   330,
       0,   329,     0,     0,   504,     0,     0,     0,     0,     0,
       0,     0,     0,   503,     0,     0,   285,     0,   504,     0,
     309,   504,   262,     0,     0,   235,   264,     0,   237,   291,
       0,   258,   257,   256,   234,   495,   504,   495,     0,    12,
      14,    13,     0,   293,     0,     0,     0,   404,   407,   405,
     388,   403,     0,     0,     0,     0,   283,    63,   495,   495,
     223,   287,   499,   498,   224,   499,   226,   499,   477,   288,
     476,    82,     0,   504,     0,   418,   387,   401,   389,   390,
     391,     0,   394,   396,     0,   397,     0,     0,     8,    16,
      17,    18,    19,    20,    37,    38,   504,     0,    25,    33,
       0,    34,     0,    68,    75,    24,   175,   265,    32,   192,
     200,   205,   206,   207,   202,   204,   214,   215,   208,   209,
     185,   186,   212,   213,     0,   201,   203,   197,   198,   199,
     187,   188,   189,   190,   191,   484,   489,   485,   490,   255,
     355,     0,   484,   485,   504,   356,   255,   255,   504,   504,
      26,   177,    31,   184,    51,    55,     0,   441,     0,     0,
      92,    93,    96,     0,     0,   504,     0,   495,   457,   455,
     454,   453,   456,   464,   468,   467,   463,   452,     0,     0,
     458,   504,   461,   504,   466,   504,     0,   451,     0,     0,
     280,   504,   504,   373,   504,   332,   175,   488,   284,     0,
     484,   485,   504,     0,     0,     0,   367,     0,   307,   335,
     334,   301,   333,   304,   488,   284,     0,   484,   485,     0,
       0,   241,   480,     0,   266,   263,   504,     0,     0,   479,
     290,     0,    41,     0,   260,     0,   254,   504,     0,     0,
       0,     0,     0,   228,    11,     0,   217,     0,    23,     0,
       0,   183,    63,     0,   227,     0,   266,   488,    81,     0,
     484,   485,   393,   395,   399,   193,   194,   344,     0,   347,
     342,   269,     0,    73,     0,     0,     0,     0,   354,    58,
     286,     0,     0,   233,   353,    56,   232,   351,    50,   350,
      49,   364,   361,   504,   315,     0,     0,   286,   318,     0,
       0,   495,     0,   443,     0,   447,   471,     0,   449,   465,
       0,   450,   469,   322,   106,   374,   375,   504,   376,     0,
     504,   338,     0,     0,   336,     0,   286,     0,     0,     0,
     306,   308,   369,     0,     0,     0,     0,   286,     0,   504,
       0,   239,   504,   504,     0,     0,   259,     0,   247,   229,
       0,   495,   504,   504,   230,     0,    22,     0,   409,   410,
     411,   406,   412,   282,   495,    77,   343,     0,     0,     0,
     267,   176,     0,    30,   181,   182,    59,     0,    28,   179,
      29,   180,    57,     0,     0,    52,     0,   440,   313,   474,
     460,     0,   320,   459,   504,   504,   470,     0,   462,   504,
     452,     0,     0,   378,   339,     0,     4,   380,     0,   298,
       0,   299,     0,   504,     0,     0,   310,   236,     0,   238,
     253,     0,   244,   504,   504,   261,     0,     0,   294,   408,
     225,   345,     0,   268,   219,    27,   178,     0,     0,     0,
       0,   442,     0,   445,   446,   448,     0,     0,   377,     0,
      84,    91,     0,   379,     0,   368,   370,   371,   366,   302,
     305,     0,   504,   504,     0,   243,     0,   249,   504,   231,
     348,   365,   362,     0,   316,   504,     0,    90,     0,   504,
       0,   504,   504,     0,   240,   245,     0,   504,     0,   248,
      53,   444,   323,   488,    89,     0,   484,   485,   372,   337,
     311,   504,     0,   250,   504,    85,   246,     0,   251,   504,
     252
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,   385,   386,    63,    64,   432,   257,    65,
     211,    66,    67,   545,   683,   829,    68,    69,   271,    70,
      71,    72,    73,   212,   110,   111,   204,   205,   206,   579,
     266,   192,    75,   434,   514,   238,   276,   673,   674,   239,
     624,   247,   248,   423,   625,   735,   615,   415,   277,   491,
      76,   208,   448,   635,   224,   725,   225,   726,   609,   851,
     549,   546,   776,   378,   380,   578,   790,   261,   390,   601,
     713,   714,   230,   659,   316,   486,   758,    78,    79,   363,
     539,   774,   538,   773,   402,   597,   848,   582,   707,   792,
     796,    80,    81,    82,    83,    84,    85,    86,   298,   471,
      87,   300,   295,   259,   441,   640,   639,   751,    88,   294,
      89,    90,   214,    92,   215,   216,   373,   548,   568,   569,
     570,   571,   572,   573,   574,   575,   576,   786,   695,   194,
     379,   281,   278,   243,   116,   553,   530,   383,   221,   256,
     454,   391,   223,    96
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -671
static const yytype_int16 yypact[] =
{
    -671,   101,  2443,  -671,  5747,  8139,  8439,  4101,  5403,  -671,
    7099,  7099,  3980,  -671,  -671,  8239,  5955,  5955,  -671,  -671,
    5955,  4740,  4844,  -671,  -671,  -671,  -671,  7099,  5286,    -1,
    -671,     3,  -671,  -671,  4220,  4324,  -671,  -671,  4428,  -671,
    -671,  -671,  -671,  -671,  -671,  7931,  7931,  8691,    91,  3372,
    7099,  7203,  7931,  8539,  5169,  -671,  -671,    34,    68,   317,
    8035,  7931,  -671,   276,   720,   507,  -671,  -671,   186,   149,
    -671,   156,  8339,  -671,   183,  9622,    21,   448,    17,    66,
    -671,  -671,   208,  -671,  -671,  -671,  -671,  -671,  -671,  -671,
    -671,    29,  -671,   141,    45,    77,  -671,   720,  -671,  -671,
    -671,   193,   234,    -1,    99,   122,  7099,   136,  3496,   490,
    -671,    31,  -671,   485,  -671,  -671,    77,  -671,  -671,  -671,
    -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,
    -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,
    -671,    32,    43,   148,   210,  -671,  -671,  -671,  -671,  -671,
    -671,  -671,  -671,  -671,  -671,  -671,  -671,   269,   278,   328,
    -671,   366,  -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,
    -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,
    -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,
    -671,  -671,  -671,  -671,   369,  -671,  -671,  -671,  -671,  -671,
    -671,  -671,  -671,  5169,  -671,  -671,   257,  -671,  2772,   334,
     507,    48,   295,   486,    25,   337,    37,    48,  -671,  -671,
     276,   332,  -671,   307,  7099,  7099,   420,  -671,  -671,   503,
     464,    98,   128,  7931,  7931,  7931,  -671,  9622,   393,  -671,
    -671,   372,   380,  -671,  -671,  -671,  5638,  -671,  6059,  5955,
    -671,  -671,  -671,   425,  -671,  -671,   364,   405,  3620,    35,
    -671,   519,   470,  -671,  -671,  -671,  -671,  -671,   142,  3372,
     394,   404,   414,   507,  7931,    -1,   427,   224,   245,  -671,
     330,   435,   245,  -671,   509,  -671,   547,   555,   558,  -671,
    -671,  -671,  -671,   164,  -671,   400,   653,   472,   413,   481,
      50,   506,   532,  -671,  -671,  -671,  -671,  3862,  7099,  7099,
    7099,  7099,  5747,  7099,  7099,  -671,  -671,  7307,  -671,  3372,
    8539,   476,  7307,  7931,  7931,  7931,  7931,  7931,  7931,  7931,
    7931,  7931,  7931,  7931,  7931,  7931,  7931,  7931,  -671,  7931,
    7931,  7931,  7931,  7931,  7931,  7931,  7931,  7931,  7931,  8758,
    7203,  8825,  8691,  8691,  8691,  -671,  -671,  -671,  -671,  8035,
    8035,  -671,   483,  -671,   364,   507,  -671,   583,  -671,  -671,
    -671,   276,  -671,  -671,  -671,  8892,  7203,  8691,  2772,  7099,
     456,  -671,  -671,  -671,  -671,   589,   598,    84,  -671,  -671,
    2896,   594,  7931,  8959,  7203,  9026,  7931,  7931,  3134,   597,
    3744,  7411,   608,  -671,    28,    28,   145,  9093,  7203,  9160,
    -671,   495,  -671,  7931,  6163,  -671,  -671,  6267,  -671,  -671,
     498,  5851,  -671,  -671,   186,    -1,   500,    15,   499,  -671,
    -671,  -671,  5403,  -671,  7931,  3620,   510,  -671,  -671,  -671,
    -671,  -671,  8959,  9026,  7931,   505,  -671,   508,    -1,  9491,
    -671,  -671,  7515,  -671,  -671,  7931,  -671,  7931,  -671,  -671,
    -671,   583,  9227,  7203,  9294,  -671,  -671,  -671,  -671,  -671,
    -671,    12,  -671,  -671,   512,  -671,  7931,  7931,   720,  -671,
    -671,  -671,  -671,  -671,  -671,  -671,    85,  7931,  -671,   525,
     533,  -671,   541,  -671,  -671,  -671,  2313,  -671,  -671,   470,
     920,   920,   920,   920,   480,   480,  1889,  9690,   920,   920,
    9673,  9673,     4,     4,  7931,   480,   480,   715,   715,   751,
     184,   184,   470,   470,   470,  2511,  4948,  2639,  5052,   234,
    -671,   544,   319,   460,  4844,  -671,  1338,  1338,    85,    85,
    -671,  9622,  -671,  9622,  -671,  -671,   276,  -671,  7099,  2772,
     367,   399,  -671,   234,   562,   234,   624,    70,   575,  -671,
    -671,  -671,  -671,  -671,  -671,  -671,  -671,   802,  2772,   276,
    -671,   560,  -671,   563,   643,   569,   647,  -671,  5520,  5403,
    -671,  7619,   621,  -671,   540,  -671,  2618,  4532,  4636,   574,
     157,   163,   621,   693,   694,  7931,   578,    48,  -671,  -671,
    -671,  -671,  -671,  -671,    62,    86,   593,   150,   264,  7099,
     616,  -671,  -671,  7931,   393,  -671,   603,  7931,   393,  -671,
    -671,  7931,  1491,    -6,  -671,   605,  -671,   606,   610,  6371,
    8691,  8691,   615,  -671,  -671,  7099,  9622,   622,  -671,  3620,
     697,  9622,   454,   619,  -671,  7931,  -671,    74,   399,   596,
     130,   354,  -671,  -671,  -671,   470,   470,  -671,  2056,  -671,
    -671,  -671,  7723,  -671,  7931,  9605,  8035,  7931,  -671,   483,
     473,  8035,  8035,  -671,  -671,   483,  -671,  -671,  -671,  -671,
    -671,  -671,  -671,    85,  -671,   276,   738,  -671,  -671,   629,
    7931,    -1,   756,  -671,   802,  -671,  -671,   445,  -671,  -671,
     110,  -671,  -671,  -671,  -671,   525,  -671,   681,  -671,  3253,
     760,  -671,  7099,   761,  -671,  7931,   190,  7931,  7931,   762,
    -671,  -671,  -671,  7827,  3015,  3744,  3744,   280,    28,   495,
    6475,  -671,   495,   495,  6579,   650,  -671,  6683,  -671,  -671,
     186,    15,   234,   234,  -671,    53,  -671,   651,  -671,  -671,
    -671,  -671,  -671,  -671,  9491,   586,  -671,   670,  3744,  7931,
     655,  9622,  7931,  -671,  9622,  9622,  -671,  8035,  -671,  9622,
    -671,  9622,  -671,  3744,  3620,  -671,  2772,  -671,  -671,  -671,
    -671,   654,  -671,  -671,   658,   569,  -671,   575,  -671,   569,
     456,  8639,    48,  -671,  -671,  3744,  -671,  -671,    48,  -671,
    7931,  -671,  7931,   452,   777,   778,  -671,  -671,  7931,  -671,
    -671,  7931,  -671,   662,   671,  -671,  7931,   673,  -671,  -671,
    -671,  -671,   791,  -671,  9622,  -671,  9622,   793,   684,  3620,
     797,  -671,   445,  -671,  -671,  -671,  2772,   754,  -671,   617,
     555,   558,  2772,  -671,  2896,  -671,  -671,  -671,  -671,  -671,
    -671,  3744,  9543,   495,  6787,  -671,  6891,  -671,   495,  -671,
    -671,  -671,  -671,   688,  -671,   569,   801,   583,  9361,  7203,
    9428,   598,   540,   804,  -671,  -671,  7931,   685,  7931,  -671,
    -671,  -671,  -671,    44,   399,   696,    52,    60,  -671,  -671,
    -671,   495,  6995,  -671,   495,   586,  -671,  7931,  -671,   495,
    -671
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -671,  -671,  -671,  -352,   247,  -671,    47,  -671,  -671,   988,
      56,    23,  -671,  -537,  -671,  -671,    -8,    -5,  -191,   -33,
     746,  -671,   -32,  1099,  -101,   815,     6,   -26,  -671,  -671,
      41,  -671,   -16,  -671,  -671,  1561,  -336,   -11,  -440,    14,
    -671,    -2,  -671,  -671,  -671,  -671,    22,   -75,    36,  -289,
      13,  -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,  -671,
    -671,  -671,  -671,  -671,  -671,  -671,  -671,   415,  -210,  -390,
     -50,  -506,   167,  -475,  -671,  -671,  -671,  -227,  -671,   755,
    -671,  -671,  -671,  -671,  -358,  -671,  -671,   -36,  -671,  -671,
    -671,  -671,  -671,  -671,   757,  -671,  -671,  -671,  -671,  -671,
    -671,  -671,  -671,   463,  -236,  -671,  -671,  -671,    -3,  -671,
    -671,  -671,   885,   830,  1103,  1007,  -671,  -671,    51,   275,
     151,  -671,  -670,   152,  -671,  -620,  -671,  -357,  -494,  -671,
    -671,  -671,    30,  -380,   910,    61,  -671,  -671,   -46,   -79,
      59,    -9,   780,  1454
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -505
static const yytype_int16 yytable[] =
{
     237,   237,   254,   222,   237,   207,   368,   398,   236,   236,
     227,   250,   236,   190,   531,   603,   272,   306,   112,   112,
     251,   285,   427,   577,   364,   207,   556,   788,   112,   260,
     262,   244,   371,   498,   245,   237,   237,   619,   240,   240,
     554,   593,   240,   275,   304,   305,   242,   242,   191,   191,
     242,    97,   241,   241,   222,   599,   241,   -87,   589,   467,
     292,   388,   472,   681,   682,   -86,   112,   217,   220,   191,
     323,   437,   606,   -88,   785,   619,   710,   789,   445,   698,
     413,   701,   282,   303,   361,   112,   719,   -87,   280,   313,
     314,   250,   355,   362,   437,   191,   677,   679,   630,   -79,
     353,     3,   374,   355,  -328,   361,   313,   314,   267,   474,
    -435,   -89,   438,   439,  -435,  -429,   344,   345,   346,   347,
     348,   734,   397,   -84,   258,  -481,  -428,   649,   492,   -84,
     360,   255,   766,   -84,   600,   438,   439,   440,   772,   372,
     631,   -91,   354,   652,  -328,   268,  -328,   255,  -482,   -91,
     -87,  -328,   475,   -91,   389,   -78,   -76,  -429,   -86,   657,
     218,   219,   788,   218,   219,   297,   -88,   418,  -428,   444,
     -90,   -87,   -83,  -484,   399,   -86,   -87,   -87,   -79,   818,
     218,   219,  -423,   -79,   -86,   -86,   366,   356,   -90,   -79,
     367,  -484,   -88,   -88,   658,   -89,   450,   686,   356,   299,
     -79,   -79,   255,  -484,   564,  -426,   249,   -84,   775,   292,
     577,   222,   865,   -81,   315,  -485,   692,   237,   237,   304,
    -481,   565,  -423,   437,  -423,   -76,   359,   444,  -481,  -423,
     237,  -430,   237,   237,   -78,   653,   272,   -91,   236,   -78,
     236,   424,   717,  -482,   191,  -426,   -90,  -426,   718,    62,
     323,  -482,  -426,   317,   -90,   -83,   -78,   -78,   449,   -86,
     420,   -86,   422,   425,   438,   439,   465,   -88,   240,   -82,
     240,   240,   -82,  -430,   552,   800,   242,   -78,   242,   242,
     404,   405,   241,   318,   241,   426,   272,   322,   493,   -88,
     833,   834,   706,  -431,   -85,   835,   270,   846,   346,   347,
     348,   237,   677,   679,    55,   -85,   496,   499,   500,   501,
     502,   503,   504,   505,   506,   507,   508,   509,   510,   511,
     512,   513,   246,   515,   516,   517,   518,   519,   520,   521,
     522,   523,   524,   112,   237,  -431,   611,   456,   806,   458,
     488,   460,   275,   541,   543,   495,   628,   400,   632,   401,
     619,   452,  -433,   490,   478,   270,   453,   619,   490,   483,
     237,  -432,   547,   249,   479,   480,   481,   482,   275,   643,
     644,   881,   455,   -88,   301,   302,   586,   453,   237,   -80,
     541,   543,   540,   542,   384,   237,   275,   724,   387,   -85,
     528,   -80,   237,  -488,  -433,   602,   602,   237,   237,   392,
     275,   237,   741,  -432,   671,   622,   634,   -77,   218,   219,
     529,  -423,   534,   535,   536,   537,   528,    77,   636,    77,
     113,   113,   396,   -86,   830,   213,   213,   213,   641,   207,
     229,   213,   213,   577,   528,   213,   237,   596,   555,   237,
     403,   237,   213,   626,   616,   847,   -78,   237,   528,  -426,
    -488,   627,   381,  -423,   529,   275,   534,   457,   -80,   437,
     655,   656,   453,   -80,    77,   213,   213,   709,   286,   401,
     555,   237,   437,   191,   406,   213,   619,   429,   689,   413,
     -80,   -80,  -284,   528,   866,   430,   431,   286,  -488,   410,
    -488,  -426,  -488,   433,   382,   787,  -484,  -488,   665,   414,
     438,   439,   466,   528,   555,   436,   558,   417,   559,   560,
     561,   562,   619,   438,   439,   470,   270,   293,   668,   296,
     446,   213,  -284,    77,  -284,   555,   435,   669,  -485,  -284,
     447,   349,   675,   885,   678,   680,   323,   684,   563,   564,
     -64,   731,   668,   313,   314,   672,   323,   369,   370,   563,
     564,   451,   738,   704,   712,   709,   565,   459,   767,   566,
     693,   336,   337,   461,   -88,   237,   270,   565,   375,   393,
     566,   350,   476,   351,   469,   544,   207,   -85,   352,   237,
     -65,   -73,   842,   473,   703,   567,   407,   -80,   844,   341,
     342,   343,   344,   345,   346,   347,   348,   237,   477,   580,
     -77,   237,   442,   494,   685,   237,   581,   585,   376,   394,
     377,   395,   781,   237,   401,   352,   352,   490,   598,   191,
     191,   740,   610,    77,   620,   633,   408,   629,   409,   754,
     462,   642,   638,   352,   688,   -73,   709,   584,  -434,   213,
     213,  -277,   394,   654,   443,   592,   237,   594,   761,   352,
     764,   765,  -265,   227,   807,   769,   771,   809,   810,   616,
     662,   213,   817,   213,   213,   728,  -285,   663,   670,  -286,
     463,   112,   464,    77,   237,   820,   777,   352,  -434,   690,
    -434,  -277,   637,  -277,    77,  -434,   687,   694,  -277,   763,
     697,   742,   743,   699,   768,   770,   700,   702,   716,   764,
     868,   769,   771,   720,   721,   723,  -285,   237,  -285,  -286,
     234,  -286,   437,  -285,   237,   468,  -286,   727,   237,   602,
     755,   237,    77,   213,   213,   213,   213,    77,   213,   213,
     730,   736,   213,   737,    77,   286,   739,   213,   855,   857,
     869,   744,   870,   237,   746,   753,   824,   352,   778,   748,
     749,   826,   750,   438,   439,   779,   812,    43,    44,   308,
     309,   310,   311,   312,   814,   213,   782,   791,   798,   795,
     813,   799,   801,   819,   213,   213,   815,   874,   875,   821,
     831,   323,  -266,   879,   826,   832,   237,   849,   850,   854,
     825,   213,   852,    77,   213,   237,   336,   337,   856,   859,
     237,   860,   893,   861,   112,    77,   862,   864,   867,   213,
     880,   882,   892,    77,   890,    77,   896,   323,   321,   898,
     895,   117,   889,   213,   900,   757,   343,   344,   345,   346,
     347,   348,   336,   337,   357,   888,   213,   193,   237,   358,
     237,   836,   691,   307,     0,   783,   784,     0,     0,     0,
      77,     0,   558,   237,   559,   560,   561,   562,     0,     0,
     237,   275,   237,   344,   345,   346,   347,   348,     0,     0,
       0,     0,     0,     0,     0,     0,   237,     0,   213,     0,
       0,   237,     0,     0,   877,     0,   747,    91,     0,    91,
     114,   114,   114,     0,     0,   563,   564,     0,     0,     0,
     231,     0,     0,     0,     0,     0,     0,     0,     0,   528,
       0,     0,    95,   565,    95,     0,   566,     0,     0,     0,
      95,    95,    95,     0,     0,     0,    95,    95,     0,     0,
      95,   555,     0,     0,    91,     0,     0,    95,   287,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   794,   287,     0,    95,
      95,    95,     0,   213,    77,     0,     0,     0,     0,     0,
      95,   803,   804,   805,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    77,     0,     0,   323,  -505,  -505,  -505,
    -505,   328,   329,    91,     0,  -505,  -505,     0,   210,   210,
     210,   336,   337,     0,     0,   822,     0,     0,     0,    94,
       0,    94,   115,   115,     0,   252,    95,     0,    95,     0,
     827,   828,   232,     0,   213,     0,     0,   339,   340,   341,
     342,   343,   344,   345,   346,   347,   348,     0,   273,     0,
       0,     0,   843,     0,   213,     0,     0,     0,     0,     0,
     213,     0,     0,     0,    77,     0,    94,     0,     0,     0,
     288,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   229,     0,     0,   863,     0,     0,   288,
       0,   213,     0,     0,     0,     0,   213,   213,     0,   871,
       0,   872,     0,    91,   365,     0,     0,     0,   873,     0,
       0,    74,     0,    74,     0,    93,     0,    93,     0,     0,
       0,     0,     0,     0,   228,    94,     0,     0,    95,     0,
       0,     0,     0,     0,    77,     0,     0,   213,     0,     0,
       0,     0,     0,     0,    95,    95,     0,     0,     0,    77,
      77,    77,     0,    91,     0,     0,     0,     0,    74,     0,
       0,     0,    93,     0,    91,     0,    95,     0,    95,    95,
       0,     0,     0,     0,     0,     0,     0,     0,    95,     0,
       0,     0,     0,    77,     0,     0,     0,     0,     0,    95,
       0,     0,   213,     0,     0,     0,     0,     0,    77,    77,
       0,    77,    91,     0,     0,     0,     0,    91,     0,     0,
       0,     0,     0,     0,    91,   287,   839,    74,     0,     0,
      77,    93,   210,   210,     0,    94,     0,    95,    95,    95,
      95,    95,    95,    95,    95,     0,     0,    95,     0,    95,
       0,     0,    95,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    77,     0,     0,     0,     0,     0,
       0,    77,     0,     0,     0,     0,     0,    77,     0,    77,
      95,     0,     0,    91,     0,    94,    77,     0,     0,    95,
      95,     0,     0,     0,     0,    91,    94,     0,     0,     0,
       0,     0,     0,    91,   213,    91,    95,     0,    95,    95,
       0,     0,     0,     0,     0,     0,   210,   210,   210,   210,
      95,   484,   485,     0,    95,     0,     0,    74,    95,     0,
      95,    93,     0,     0,    94,     0,     0,     0,    95,    94,
      91,     0,     0,     0,     0,     0,    94,   288,     0,     0,
       0,    95,     0,     0,     0,     0,     0,     0,  -504,     0,
       0,     0,     0,     0,     0,    95,  -504,  -504,  -504,     0,
       0,  -504,  -504,  -504,     0,  -504,     0,    74,     0,     0,
       0,    93,     0,     0,     0,  -504,     0,   557,    74,     0,
       0,     0,    93,    95,  -504,  -504,     0,  -504,  -504,  -504,
    -504,  -504,     0,     0,     0,    94,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    94,     0,     0,
       0,     0,     0,     0,     0,    94,    74,    94,     0,   273,
      93,    74,     0,     0,     0,    93,     0,     0,    74,     0,
       0,  -504,    93,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    91,     0,     0,     0,     0,     0,
       0,     0,    94,     0,  -504,     0,     0,     0,     0,     0,
       0,     0,     0,    91,     0,     0,     0,     0,    95,    95,
    -504,     0,     0,  -504,  -504,     0,     0,   249,     0,     0,
    -504,  -504,     0,     0,     0,     0,     0,    74,    95,     0,
       0,    93,     0,     0,     0,     0,     0,     0,     0,    74,
       0,     0,     0,    93,     0,     0,     0,    74,     0,    74,
       0,    93,     0,    93,     0,   279,   283,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
       0,     0,     0,     0,    91,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    74,     0,   210,     0,    93,    95,
       0,     0,     0,   231,     0,    95,     0,     0,     0,    95,
       0,     0,     0,     0,     0,     0,    94,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
       0,     0,   336,   337,     0,    94,    95,  -221,     0,     0,
       0,    95,    95,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    91,     0,   338,   210,   339,   340,
     341,   342,   343,   344,   345,   346,   347,   348,     0,    91,
      91,    91,     0,     0,     0,     0,     0,     0,  -221,    95,
       0,     0,    95,   745,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    95,    95,    95,     0,     0,     0,
       0,     0,     0,    91,     0,     0,    94,   752,    74,     0,
       0,     0,    93,     0,     0,     0,     0,     0,    91,    91,
       0,    91,     0,     0,     0,   232,     0,    74,    95,     0,
       0,    93,     0,     0,     0,     0,   840,    95,     0,     0,
      91,     0,     0,    95,    95,     0,    95,     0,     0,     0,
       0,     0,     0,     0,     0,   416,   416,     0,     0,     0,
     210,     0,     0,   428,     0,    95,     0,     0,     0,     0,
       0,     0,     0,     0,    91,     0,    94,     0,     0,     0,
       0,    91,     0,     0,     0,     0,     0,    91,     0,    91,
       0,    94,    94,    94,     0,     0,    91,     0,    74,    95,
       0,     0,    93,     0,     0,     0,    95,     0,     0,     0,
       0,     0,    95,     0,    95,     0,     0,   228,     0,     0,
       0,    95,     0,     0,     0,    94,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
      94,    94,     0,    94,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   411,   412,     0,     0,   841,     0,
       0,     0,    94,     0,   279,     0,     0,     0,    74,     0,
       0,     0,    93,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    74,    74,    74,     0,    93,    93,    93,
     279,     0,     0,     0,     0,     0,    94,     0,     0,     0,
     583,     0,     0,    94,     0,     0,     0,     0,   279,    94,
       0,    94,     0,     0,     0,     0,     0,    74,    94,     0,
       0,    93,   279,     0,     0,   416,     0,     0,     0,     0,
       0,     0,    74,    74,     0,    74,    93,    93,   489,    93,
     416,     0,     0,   497,     0,     0,     0,     0,     0,     0,
     838,     0,     0,     0,    74,     0,     0,     0,    93,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   279,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    74,     0,
       0,     0,    93,     0,     0,    74,     0,     0,     0,    93,
     660,    74,     0,    74,     0,    93,     0,    93,     0,     0,
      74,     0,     0,     0,    93,   323,   324,   325,   326,   327,
     328,   329,   497,     0,   332,   333,     0,     0,     0,     0,
     336,   337,     0,     0,   612,   614,     0,     0,   618,     0,
       0,     0,   623,     0,     0,     0,     0,     0,   676,     0,
     676,   676,   660,   660,     0,     0,   339,   340,   341,   342,
     343,   344,   345,   346,   347,   348,     0,     0,     0,   676,
       0,     0,     0,   646,     0,     0,   618,     0,   646,     0,
       0,     0,     0,     0,     0,   696,     0,   696,     0,   696,
       0,     0,     0,     0,     0,   708,   711,     0,   711,     0,
       0,     0,     0,     0,     0,     0,   711,     0,   661,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,     0,
     416,     0,    12,     0,    13,    14,    15,    98,    99,    18,
      19,   416,     0,     0,     0,   100,   101,   102,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   103,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,    43,    44,     0,     0,
       0,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   660,     0,     0,
     226,     0,   705,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,   722,     0,     0,     0,
       0,   793,     0,     0,   797,   756,     0,     0,     0,     0,
       0,     0,     0,   109,   729,     0,     0,     0,   732,     0,
       0,     0,   733,   416,     0,     0,   416,   416,     0,     0,
     614,     0,     0,     0,     0,     0,   676,   676,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   760,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   696,   696,
       0,     0,     0,   696,     0,     0,     0,     0,     0,     0,
       0,   780,     0,     0,     0,     0,     0,   711,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   416,   416,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   646,     0,     0,     0,     0,     0,
       0,   618,     0,     0,     0,     0,     0,     0,   618,     0,
       0,     0,     0,     0,     0,     0,   416,   416,     0,     0,
       0,     0,   416,     0,     0,     0,     0,     0,     0,   696,
     823,     0,     0,   279,     0,   583,   711,     0,     0,     0,
       0,   416,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   416,     0,     0,   416,     0,
       0,     0,     0,   416,     0,     0,   664,     0,     0,     0,
       0,     0,     0,   845,     0,     0,     0,     0,     0,     0,
       0,     0,   853,     0,     0,     0,     0,   858,     0,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,     0,     0,   336,   337,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   614,     0,   618,   338,     0,
     339,   340,   341,   342,   343,   344,   345,   346,   347,   348,
       0,     0,     0,     0,     0,     0,     0,   891,     0,   894,
    -221,     0,     0,  -504,     4,     0,     5,     6,     7,     8,
       9,     0,     0,   618,    10,    11,     0,     0,   899,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,    43,    44,     0,    45,    46,    47,     0,
       0,  -488,     0,     0,     0,     0,     0,     0,     0,  -488,
    -488,  -488,     0,     0,     0,  -488,  -488,    48,  -488,     0,
      49,    50,     0,    51,    52,     0,    53,  -488,    54,    55,
      56,    57,    58,     0,     0,     0,     0,  -488,  -488,     0,
    -488,  -488,  -488,  -488,  -488,     0,     0,     0,     0,     0,
      59,    60,    61,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -504,  -504,  -488,  -488,  -488,
    -488,  -488,  -488,  -488,  -488,  -488,  -488,  -488,  -488,  -488,
       0,     0,  -488,  -488,  -488,     0,   666,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   -87,  -488,     0,  -488,  -488,
    -488,  -488,  -488,  -488,  -488,  -488,  -488,  -488,     0,     0,
       0,     0,  -488,  -488,  -488,     0,  -488,  -488,   -79,  -284,
       0,  -488,     0,  -488,  -488,     0,     0,  -284,  -284,  -284,
       0,     0,     0,  -284,  -284,     0,  -284,     0,     0,     0,
       0,   664,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -284,  -284,     0,  -284,  -284,
    -284,  -284,  -284,     0,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,     0,     0,   336,
     337,     0,     0,     0,     0,  -284,  -284,  -284,  -284,  -284,
    -284,  -284,  -284,  -284,  -284,  -284,  -284,  -284,     0,     0,
    -284,  -284,  -284,   338,   667,   339,   340,   341,   342,   343,
     344,   345,   346,   347,   348,     0,     0,     0,     0,     0,
       0,     0,     0,   -89,  -284,     0,  -284,  -284,  -284,  -284,
    -284,  -284,  -284,  -284,  -284,  -284,     0,     0,     0,     0,
       0,  -284,  -284,     0,  -284,  -284,   -81,     0,     0,  -284,
       0,  -284,  -284,     4,     0,     5,     6,     7,     8,     9,
    -504,  -504,  -504,    10,    11,     0,     0,  -504,    12,     0,
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
      60,    61,     0,     0,     0,     0,     0,     4,     0,     5,
       6,     7,     8,     9,  -504,  -504,  -504,    10,    11,     0,
    -504,  -504,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,    43,    44,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    59,    60,    61,     4,     0,     5,     6,
       7,     8,     9,     0,     0,  -504,    10,    11,  -504,  -504,
    -504,    12,  -504,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,    49,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    59,    60,    61,     4,     0,     5,     6,     7,
       8,     9,     0,     0,  -504,    10,    11,  -504,  -504,  -504,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,     0,
       0,    49,    50,     0,    51,    52,     0,    53,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    59,    60,    61,     4,     0,     5,     6,     7,     8,
       9,     0,  -504,  -504,    10,    11,  -504,  -504,     0,    12,
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
       0,     0,     0,    10,    11,  -504,  -504,     0,    12,     0,
      13,    14,    15,    16,    17,    18,    19,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     0,    43,    44,     0,    45,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,     0,     0,   269,
      50,     0,    51,    52,     0,    53,     0,    54,    55,    56,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    59,
      60,    61,     0,     0,     0,     0,     0,     4,  -504,     5,
       6,     7,     8,     9,  -504,  -504,     0,    10,    11,     0,
       0,     0,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,    43,    44,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    59,    60,    61,     0,     0,     0,     0,
       0,     4,  -504,     5,     6,     7,     8,     9,  -504,  -504,
       0,    10,    11,     0,     0,     0,    12,     0,    13,    14,
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
       0,     0,  -504,     0,     0,     4,     0,     5,     6,     7,
       8,     9,  -504,  -504,  -504,    10,    11,     0,     0,     0,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,     0,
       0,    49,    50,     0,    51,    52,     0,    53,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    59,    60,    61,     0,     5,     6,     7,     8,     9,
       0,     0,     0,    10,    11,     0,  -504,  -504,    12,     0,
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
      60,    61,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,     0,     0,   403,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   209,     0,     0,   108,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    59,    60,    61,
       0,     0,     0,     0,   118,   119,   120,   121,   122,   123,
     124,   125,   218,   219,   126,   127,   128,   129,   130,     0,
       0,   131,   132,   133,   134,   135,   136,   137,     0,     0,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,    36,    37,   161,    39,     0,     0,     0,
       0,     0,     0,     0,   162,   163,     0,   164,   165,   166,
     167,   168,   169,   170,     0,     0,   171,     0,     0,     0,
     172,   173,   174,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,     0,     0,
     187,     0,     0,  -481,  -481,  -481,     0,  -481,     0,   188,
     189,  -481,  -481,     0,     0,     0,  -481,     0,  -481,  -481,
    -481,  -481,  -481,  -481,  -481,     0,  -481,     0,     0,  -481,
    -481,  -481,  -481,  -481,  -481,  -481,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -481,     0,     0,  -481,  -481,
    -481,  -481,  -481,  -481,  -481,  -481,  -481,  -481,  -481,     0,
    -481,  -481,     0,  -481,  -481,  -481,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -481,     0,     0,  -481,  -481,     0,
    -481,  -481,     0,  -481,  -481,  -481,  -481,  -481,  -481,  -481,
       0,     0,     0,     0,     0,     0,     0,  -483,  -483,  -483,
       0,  -483,     0,     0,     0,  -483,  -483,  -481,  -481,  -481,
    -483,  -481,  -483,  -483,  -483,  -483,  -483,  -483,  -483,  -481,
    -483,     0,     0,  -483,  -483,  -483,  -483,  -483,  -483,  -483,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -483,
       0,     0,  -483,  -483,  -483,  -483,  -483,  -483,  -483,  -483,
    -483,  -483,  -483,     0,  -483,  -483,     0,  -483,  -483,  -483,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -483,     0,
       0,  -483,  -483,     0,  -483,  -483,     0,  -483,  -483,  -483,
    -483,  -483,  -483,  -483,     0,     0,     0,     0,     0,     0,
       0,  -482,  -482,  -482,     0,  -482,     0,     0,     0,  -482,
    -482,  -483,  -483,  -483,  -482,  -483,  -482,  -482,  -482,  -482,
    -482,  -482,  -482,  -483,  -482,     0,     0,  -482,  -482,  -482,
    -482,  -482,  -482,  -482,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -482,     0,     0,  -482,  -482,  -482,  -482,
    -482,  -482,  -482,  -482,  -482,  -482,  -482,     0,  -482,  -482,
       0,  -482,  -482,  -482,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -482,     0,     0,  -482,  -482,     0,  -482,  -482,
       0,  -482,  -482,  -482,  -482,  -482,  -482,  -482,     0,     0,
       0,     0,     0,     0,     0,  -484,  -484,  -484,     0,  -484,
       0,     0,     0,  -484,  -484,  -482,  -482,  -482,  -484,  -482,
    -484,  -484,  -484,  -484,  -484,  -484,  -484,  -482,     0,     0,
       0,  -484,  -484,  -484,  -484,  -484,  -484,  -484,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -484,     0,     0,
    -484,  -484,  -484,  -484,  -484,  -484,  -484,  -484,  -484,  -484,
    -484,     0,  -484,  -484,     0,  -484,  -484,  -484,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -484,   715,     0,  -484,
    -484,     0,  -484,  -484,     0,  -484,  -484,  -484,  -484,  -484,
    -484,  -484,     0,     0,     0,     0,   -87,     0,     0,  -485,
    -485,  -485,     0,  -485,     0,     0,     0,  -485,  -485,  -484,
    -484,  -484,  -485,     0,  -485,  -485,  -485,  -485,  -485,  -485,
    -485,  -484,     0,     0,     0,  -485,  -485,  -485,  -485,  -485,
    -485,  -485,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -485,     0,     0,  -485,  -485,  -485,  -485,  -485,  -485,
    -485,  -485,  -485,  -485,  -485,     0,  -485,  -485,     0,  -485,
    -485,  -485,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -485,   667,     0,  -485,  -485,     0,  -485,  -485,     0,  -485,
    -485,  -485,  -485,  -485,  -485,  -485,     0,     0,     0,     0,
     -89,     0,     0,  -255,  -255,  -255,     0,  -255,     0,     0,
       0,  -255,  -255,  -485,  -485,  -485,  -255,     0,  -255,  -255,
    -255,  -255,  -255,  -255,  -255,  -485,     0,     0,     0,  -255,
    -255,  -255,  -255,  -255,  -255,  -255,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -255,     0,     0,  -255,  -255,
    -255,  -255,  -255,  -255,  -255,  -255,  -255,  -255,  -255,     0,
    -255,  -255,     0,  -255,  -255,  -255,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -255,     0,     0,  -255,  -255,     0,
    -255,  -255,     0,  -255,  -255,  -255,  -255,  -255,  -255,  -255,
       0,     0,     0,     0,     0,     0,     0,  -255,  -255,  -255,
       0,  -255,     0,     0,     0,  -255,  -255,  -255,  -255,  -255,
    -255,     0,  -255,  -255,  -255,  -255,  -255,  -255,  -255,   246,
       0,     0,     0,  -255,  -255,  -255,  -255,  -255,  -255,  -255,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -255,
       0,     0,  -255,  -255,  -255,  -255,  -255,  -255,  -255,  -255,
    -255,  -255,  -255,     0,  -255,  -255,     0,  -255,  -255,  -255,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -255,     0,
       0,  -255,  -255,     0,  -255,  -255,     0,  -255,  -255,  -255,
    -255,  -255,  -255,  -255,     0,     0,     0,     0,     0,     0,
       0,  -486,  -486,  -486,     0,  -486,     0,     0,     0,  -486,
    -486,  -255,  -255,  -255,  -486,     0,  -486,  -486,  -486,  -486,
    -486,  -486,  -486,   249,     0,     0,     0,  -486,  -486,  -486,
    -486,  -486,  -486,  -486,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -486,     0,     0,  -486,  -486,  -486,  -486,
    -486,  -486,  -486,  -486,  -486,  -486,  -486,     0,  -486,  -486,
       0,  -486,  -486,  -486,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -486,     0,     0,  -486,  -486,     0,  -486,  -486,
       0,  -486,  -486,  -486,  -486,  -486,  -486,  -486,     0,     0,
       0,     0,     0,     0,     0,  -487,  -487,  -487,     0,  -487,
       0,     0,     0,  -487,  -487,  -486,  -486,  -486,  -487,     0,
    -487,  -487,  -487,  -487,  -487,  -487,  -487,  -486,     0,     0,
       0,  -487,  -487,  -487,  -487,  -487,  -487,  -487,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -487,     0,     0,
    -487,  -487,  -487,  -487,  -487,  -487,  -487,  -487,  -487,  -487,
    -487,     0,  -487,  -487,     0,  -487,  -487,  -487,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -487,     0,     0,  -487,
    -487,     0,  -487,  -487,     0,  -487,  -487,  -487,  -487,  -487,
    -487,  -487,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -487,
    -487,  -487,   118,   119,   120,   121,   122,   123,   124,   125,
       0,  -487,   126,   127,   128,   129,   130,     0,     0,   131,
     132,   133,   134,   135,   136,   137,     0,     0,   138,   139,
     140,   195,   196,   197,   198,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   199,   200,   201,
     160,   289,   290,   202,   291,     0,     0,     0,     0,     0,
       0,     0,   162,   163,     0,   164,   165,   166,   167,   168,
     169,   170,     0,     0,   171,     0,     0,     0,   172,   173,
     174,   175,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   176,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,     0,     0,   187,   118,
     119,   120,   121,   122,   123,   124,   125,   188,     0,   126,
     127,   128,   129,   130,     0,     0,   131,   132,   133,   134,
     135,   136,   137,     0,     0,   138,   139,   140,   195,   196,
     197,   198,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   199,   200,   201,   160,   253,     0,
     202,     0,     0,     0,     0,     0,     0,     0,     0,   162,
     163,     0,   164,   165,   166,   167,   168,   169,   170,     0,
       0,   171,     0,     0,     0,   172,   173,   174,   175,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   176,
       0,   203,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,     0,     0,   187,   118,   119,   120,   121,
     122,   123,   124,   125,   188,     0,   126,   127,   128,   129,
     130,     0,     0,   131,   132,   133,   134,   135,   136,   137,
       0,     0,   138,   139,   140,   195,   196,   197,   198,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   199,   200,   201,   160,     0,     0,   202,     0,     0,
       0,     0,     0,     0,     0,     0,   162,   163,     0,   164,
     165,   166,   167,   168,   169,   170,     0,     0,   171,     0,
       0,     0,   172,   173,   174,   175,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,     0,   203,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
       0,     0,   187,   118,   119,   120,   121,   122,   123,   124,
     125,   188,     0,   126,   127,   128,   129,   130,     0,     0,
     131,   132,   133,   134,   135,   136,   137,     0,     0,   138,
     139,   140,   195,   196,   197,   198,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   199,   200,
     201,   160,     0,     0,   202,     0,     0,     0,     0,     0,
       0,     0,     0,   162,   163,     0,   164,   165,   166,   167,
     168,   169,   170,     0,     0,   171,     0,     0,     0,   172,
     173,   174,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,     0,     0,   187,
       0,     5,     6,     7,     0,     9,     0,     0,   188,    10,
      11,     0,     0,     0,    12,     0,    13,    14,    15,    98,
      99,    18,    19,     0,     0,     0,     0,   100,    21,    22,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   209,     0,     0,   108,    50,     0,    51,    52,
       0,   233,   234,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     8,     9,    59,   235,    61,    10,    11,
       0,     0,     0,    12,   419,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,    43,    44,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    48,     0,     0,    49,    50,     0,    51,    52,     0,
      53,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    59,    60,    61,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
      20,    21,    22,    23,    24,    25,    26,     0,     0,    27,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,    43,    44,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,   621,   234,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,    60,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,    21,    22,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   209,
       0,     0,   108,    50,     0,    51,    52,     0,   233,   234,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,    21,
      22,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   209,     0,     0,   108,   421,     0,    51,
      52,     0,   233,   234,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    59,   235,    61,    12,
       0,    13,    14,    15,    98,    99,    18,    19,     0,     0,
       0,     0,   100,   101,   102,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,    43,    44,     0,    45,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   209,     0,     0,
     108,    50,     0,    51,    52,     0,   613,   234,    54,    55,
      56,    57,    58,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      59,   235,    61,    12,     0,    13,    14,    15,    98,    99,
      18,    19,     0,     0,     0,     0,   100,   101,   102,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,    43,    44,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   209,     0,     0,   108,    50,     0,    51,    52,     0,
     617,   234,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    59,   235,    61,    12,     0,    13,
      14,    15,    98,    99,    18,    19,     0,     0,     0,     0,
     100,    21,    22,    23,    24,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,    43,    44,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,   613,   234,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   235,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   209,
       0,     0,   108,    50,     0,    51,    52,     0,   808,   234,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   209,     0,     0,   108,    50,     0,    51,
      52,     0,   811,   234,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    59,   235,    61,    12,
       0,    13,    14,    15,    98,    99,    18,    19,     0,     0,
       0,     0,   100,   101,   102,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,    43,    44,     0,    45,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   209,     0,     0,
     108,    50,     0,    51,    52,     0,   816,   234,    54,    55,
      56,    57,    58,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      59,   235,    61,    12,     0,    13,    14,    15,    98,    99,
      18,    19,     0,     0,     0,     0,   100,   101,   102,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,    43,    44,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   209,     0,     0,   108,    50,     0,    51,    52,     0,
     876,   234,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    59,   235,    61,    12,     0,    13,
      14,    15,    98,    99,    18,    19,     0,     0,     0,     0,
     100,   101,   102,    23,    24,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,    43,    44,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,   878,   234,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   235,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   209,
       0,     0,   108,    50,     0,    51,    52,     0,   897,   234,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   209,     0,     0,   108,    50,     0,    51,
      52,     0,     0,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    59,    60,    61,    12,
       0,    13,    14,    15,    98,    99,    18,    19,     0,     0,
       0,     0,   100,    21,    22,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,    43,    44,     0,    45,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   209,     0,     0,
     108,    50,     0,    51,    52,     0,   274,     0,    54,    55,
      56,    57,    58,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      59,   235,    61,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,    43,    44,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   209,     0,     0,   108,    50,     0,    51,    52,     0,
     487,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    59,   235,    61,    12,     0,    13,
      14,    15,    98,    99,    18,    19,     0,     0,     0,     0,
     100,   101,   102,    23,    24,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,    43,    44,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,   595,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   235,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   209,
       0,     0,   108,    50,     0,    51,    52,     0,   645,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   209,     0,     0,   108,    50,     0,    51,
      52,     0,   487,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    59,   235,    61,    12,
       0,    13,    14,    15,    98,    99,    18,    19,     0,     0,
       0,     0,   100,   101,   102,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,    43,    44,     0,    45,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   209,     0,     0,
     108,    50,     0,    51,    52,     0,   759,     0,    54,    55,
      56,    57,    58,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      59,   235,    61,    12,     0,    13,    14,    15,    98,    99,
      18,    19,     0,     0,     0,     0,   100,   101,   102,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,    43,    44,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   209,     0,     0,   108,    50,     0,    51,    52,     0,
     802,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    59,   235,    61,    12,     0,    13,
      14,    15,    98,    99,    18,    19,     0,     0,     0,     0,
     100,   101,   102,    23,    24,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    29,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,    43,    44,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   235,
      61,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   209,
       0,     0,   108,    50,     0,    51,    52,     0,     0,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,     0,     0,    32,    33,   104,
      35,    36,    37,   105,    39,    40,    41,    42,     0,    43,
      44,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     106,     0,     0,   107,     0,     0,   108,    50,     0,    51,
      52,     0,     0,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   109,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   226,     0,     0,    49,    50,     0,    51,
      52,     0,    53,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   109,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   284,     0,     0,   319,    50,     0,    51,
      52,     0,   320,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   109,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,     0,     0,    32,    33,   104,
      35,    36,    37,   105,    39,    40,    41,    42,     0,    43,
      44,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   107,     0,     0,   108,    50,     0,    51,
      52,     0,     0,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   109,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   284,     0,     0,   108,    50,     0,    51,
      52,     0,     0,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   109,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   837,     0,     0,   108,    50,     0,    51,
      52,     0,     0,     0,    54,    55,    56,    57,    58,     0,
       0,   263,   264,     0,     0,   265,     0,     0,     0,     0,
       0,     0,     0,     0,   162,   163,   109,   164,   165,   166,
     167,   168,   169,   170,     0,     0,   171,     0,     0,     0,
     172,   173,   174,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   525,   526,
     187,     0,   527,     0,     0,     0,     0,     0,     0,   188,
       0,   162,   163,     0,   164,   165,   166,   167,   168,   169,
     170,     0,     0,   171,     0,     0,     0,   172,   173,   174,
     175,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   176,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   532,   264,   187,     0,   533,
       0,     0,     0,     0,     0,     0,   188,     0,   162,   163,
       0,   164,   165,   166,   167,   168,   169,   170,     0,     0,
     171,     0,     0,     0,   172,   173,   174,   175,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   176,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   550,   526,   187,     0,   551,     0,     0,     0,
       0,     0,     0,   188,     0,   162,   163,     0,   164,   165,
     166,   167,   168,   169,   170,     0,     0,   171,     0,     0,
       0,   172,   173,   174,   175,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   176,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   587,
     526,   187,     0,   588,     0,     0,     0,     0,     0,     0,
     188,     0,   162,   163,     0,   164,   165,   166,   167,   168,
     169,   170,     0,     0,   171,     0,     0,     0,   172,   173,
     174,   175,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   176,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   590,   264,   187,     0,
     591,     0,     0,     0,     0,     0,     0,   188,     0,   162,
     163,     0,   164,   165,   166,   167,   168,   169,   170,     0,
       0,   171,     0,     0,     0,   172,   173,   174,   175,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   176,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   604,   526,   187,     0,   605,     0,     0,
       0,     0,     0,     0,   188,     0,   162,   163,     0,   164,
     165,   166,   167,   168,   169,   170,     0,     0,   171,     0,
       0,     0,   172,   173,   174,   175,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     607,   264,   187,     0,   608,     0,     0,     0,     0,     0,
       0,   188,     0,   162,   163,     0,   164,   165,   166,   167,
     168,   169,   170,     0,     0,   171,     0,     0,     0,   172,
     173,   174,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   647,   526,   187,
       0,   648,     0,     0,     0,     0,     0,     0,   188,     0,
     162,   163,     0,   164,   165,   166,   167,   168,   169,   170,
       0,     0,   171,     0,     0,     0,   172,   173,   174,   175,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     176,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   650,   264,   187,     0,   651,     0,
       0,     0,     0,     0,     0,   188,     0,   162,   163,     0,
     164,   165,   166,   167,   168,   169,   170,     0,     0,   171,
       0,     0,     0,   172,   173,   174,   175,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   176,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   883,   526,   187,     0,   884,     0,     0,     0,     0,
       0,     0,   188,     0,   162,   163,     0,   164,   165,   166,
     167,   168,   169,   170,     0,     0,   171,     0,     0,     0,
     172,   173,   174,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   886,   264,
     187,     0,   887,     0,     0,     0,     0,     0,     0,   188,
       0,   162,   163,     0,   164,   165,   166,   167,   168,   169,
     170,     0,     0,   171,     0,     0,     0,   172,   173,   174,
     175,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   176,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,     0,     0,   187,     0,     0,
       0,     0,     0,     0,     0,     0,   188,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
       0,     0,   336,   337,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   338,     0,   339,   340,
     341,   342,   343,   344,   345,   346,   347,   348,     0,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,     0,   255,   336,   337,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   338,     0,
     339,   340,   341,   342,   343,   344,   345,   346,   347,   348,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     610,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,     0,     0,   336,   337,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,     0,     0,   336,   337,     0,     0,     0,     0,     0,
     338,   762,   339,   340,   341,   342,   343,   344,   345,   346,
     347,   348,     0,     0,     0,     0,     0,   338,     0,   339,
     340,   341,   342,   343,   344,   345,   346,   347,   348,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,   333,
    -505,  -505,     0,     0,   336,   337,   323,   324,   325,   326,
     327,   328,   329,   330,     0,   332,   333,     0,     0,     0,
       0,   336,   337,     0,     0,     0,     0,     0,     0,     0,
     339,   340,   341,   342,   343,   344,   345,   346,   347,   348,
       0,     0,     0,     0,     0,     0,     0,   339,   340,   341,
     342,   343,   344,   345,   346,   347,   348
};

static const yytype_int16 yycheck[] =
{
      16,    17,    28,    12,    20,     8,   107,   217,    16,    17,
      15,    22,    20,     7,   350,   405,    49,    63,     5,     6,
      22,    53,   249,   380,   103,    28,   378,   697,    15,    45,
      46,    17,     1,   322,    20,    51,    52,   417,    16,    17,
     376,   399,    20,    51,    60,    61,    16,    17,     7,     8,
      20,     4,    16,    17,    63,    27,    20,    13,   394,   295,
      54,    13,   298,   538,   539,    13,    53,    11,    12,    28,
      66,    59,   408,    13,   694,   455,   582,   697,   269,   573,
      86,   575,    52,    60,    95,    72,   592,    25,    52,    36,
      37,   102,    26,    95,    59,    54,   536,   537,    83,    25,
      83,     0,   111,    26,    83,   116,    36,    37,    47,    59,
      85,    25,   100,   101,    85,    83,   112,   113,   114,   115,
     116,   127,    85,    25,   121,    26,    83,   463,   319,   104,
      85,   132,   669,   104,   106,   100,   101,   102,   675,   108,
     125,   104,   125,   131,   123,    54,   125,   132,    26,   104,
     106,   130,   102,    25,   106,    25,   127,   125,   106,    74,
     132,   133,   832,   132,   133,   131,   106,   242,   125,    85,
      25,   109,   127,   129,   220,    25,   132,   133,   104,   126,
     132,   133,    83,   109,   132,   133,    50,   121,   104,   127,
      54,   129,   132,   133,   109,   109,   275,   549,   121,   131,
     126,   127,   132,   129,    94,    83,   129,   109,   683,   203,
     567,   220,   832,   127,    28,   129,   568,   233,   234,   235,
     121,   111,   123,    59,   125,   127,    85,    85,   129,   130,
     246,    83,   248,   249,   104,   471,   269,   109,   246,   109,
     248,   249,    85,   121,   203,   123,   104,   125,    85,     2,
      66,   129,   130,   104,   109,   127,   126,   127,   274,   109,
     246,   104,   248,   249,   100,   101,   102,   104,   246,   127,
     248,   249,   127,   125,   375,    85,   246,   127,   248,   249,
     224,   225,   246,   127,   248,   249,   319,   104,   320,    25,
     784,   785,   581,    83,   104,   789,    49,   803,   114,   115,
     116,   317,   742,   743,    96,    25,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   129,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   320,   350,   125,   411,   278,   728,   280,
     317,   282,   350,   359,   360,   322,   425,    15,   427,    17,
     730,   127,    83,   317,   307,   108,   132,   737,   322,   312,
     376,    83,   371,   129,   308,   309,   310,   311,   376,   448,
     449,   865,   127,   109,    57,    58,   392,   132,   394,    25,
     396,   397,   359,   360,   127,   401,   394,   597,    54,   109,
     349,   127,   408,    26,   125,   404,   405,   413,   414,   104,
     408,   417,   629,   125,    85,   421,   432,   127,   132,   133,
     349,    83,   351,   352,   353,   354,   375,     2,   434,     4,
       5,     6,    85,   104,   776,    10,    11,    12,   444,   432,
      15,    16,    17,   790,   393,    20,   452,   401,   377,   455,
     133,   457,    27,   421,   414,   803,   127,   463,   407,    83,
      83,   421,    83,   125,   393,   463,   395,   127,   104,    59,
     476,   477,   132,   109,    49,    50,    51,    15,    53,    17,
     409,   487,    59,   432,    54,    60,   856,    52,   557,    86,
     126,   127,    83,   442,   836,    60,    61,    72,   121,    25,
     123,   125,   125,   129,   125,    50,   129,   130,   514,   127,
     100,   101,   102,   462,   443,   258,    50,   127,    52,    53,
      54,    55,   892,   100,   101,   102,   269,    54,   529,    56,
     126,   106,   123,   108,   125,   464,   121,   529,   129,   130,
     126,    83,   534,   869,   536,   537,    66,   546,    93,    94,
     126,   616,   553,    36,    37,    85,    66,    57,    58,    93,
      94,   124,   627,   579,    14,    15,   111,   122,    85,   114,
     569,    81,    82,    54,   104,   581,   319,   111,    83,    83,
     114,   123,    66,   125,   102,    92,   579,   104,   130,   595,
     126,   127,   792,   102,   578,   129,    83,   127,   798,   109,
     110,   111,   112,   113,   114,   115,   116,   613,    66,    10,
     127,   617,    83,   127,   548,   621,     8,    13,   123,   123,
     125,   125,   691,   629,    17,   130,   130,   581,    10,   578,
     579,   629,   127,   208,   126,   126,   123,   127,   125,   645,
      83,   126,   122,   130,    10,   127,    15,   390,    83,   224,
     225,    83,   123,   131,   125,   398,   662,   400,   664,   130,
     666,   667,   127,   658,   729,   671,   672,   732,   733,   629,
     127,   246,   741,   248,   249,   609,    83,   126,   124,    83,
     123,   658,   125,   258,   690,   754,   685,   130,   123,   104,
     125,   123,   435,   125,   269,   130,   124,   127,   130,   666,
     127,   630,   631,    50,   671,   672,   127,    50,   124,   715,
      83,   717,   718,    10,    10,   127,   123,   723,   125,   123,
      94,   125,    59,   130,   730,    62,   130,   124,   734,   728,
     124,   737,   307,   308,   309,   310,   311,   312,   313,   314,
     127,   126,   317,   127,   319,   320,   126,   322,   813,   814,
     123,   126,   125,   759,   122,   126,   762,   130,    10,    52,
      53,   767,    55,   100,   101,   126,   734,    60,    61,    39,
      40,    41,    42,    43,   734,   350,    10,    86,   712,     9,
     734,    10,    10,   122,   359,   360,   126,   852,   853,   109,
     126,    66,   127,   858,   800,   127,   802,    10,    10,   127,
     767,   376,   808,   378,   379,   811,    81,    82,   127,   126,
     816,    10,   877,    10,   791,   390,   122,    10,    54,   394,
     122,    10,   127,   398,    10,   400,   891,    66,    72,   894,
     124,     6,   872,   408,   899,   658,   111,   112,   113,   114,
     115,   116,    81,    82,    79,   871,   421,     7,   854,    82,
     856,   790,   567,    63,    -1,   694,   694,    -1,    -1,    -1,
     435,    -1,    50,   869,    52,    53,    54,    55,    -1,    -1,
     876,   869,   878,   112,   113,   114,   115,   116,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   892,    -1,   463,    -1,
      -1,   897,    -1,    -1,   854,    -1,   639,     2,    -1,     4,
       5,     6,     7,    -1,    -1,    93,    94,    -1,    -1,    -1,
      15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   868,
      -1,    -1,     2,   111,     4,    -1,   114,    -1,    -1,    -1,
      10,    11,    12,    -1,    -1,    -1,    16,    17,    -1,    -1,
      20,   870,    -1,    -1,    49,    -1,    -1,    27,    53,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   709,    72,    -1,    49,
      50,    51,    -1,   548,   549,    -1,    -1,    -1,    -1,    -1,
      60,   724,   725,   726,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   568,    -1,    -1,    66,    67,    68,    69,
      70,    71,    72,   108,    -1,    75,    76,    -1,    10,    11,
      12,    81,    82,    -1,    -1,   758,    -1,    -1,    -1,     2,
      -1,     4,     5,     6,    -1,    27,   106,    -1,   108,    -1,
     773,   774,    15,    -1,   609,    -1,    -1,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,    -1,    50,    -1,
      -1,    -1,   795,    -1,   629,    -1,    -1,    -1,    -1,    -1,
     635,    -1,    -1,    -1,   639,    -1,    49,    -1,    -1,    -1,
      53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   658,    -1,    -1,   829,    -1,    -1,    72,
      -1,   666,    -1,    -1,    -1,    -1,   671,   672,    -1,   842,
      -1,   844,    -1,   208,   106,    -1,    -1,    -1,   851,    -1,
      -1,     2,    -1,     4,    -1,     2,    -1,     4,    -1,    -1,
      -1,    -1,    -1,    -1,    15,   108,    -1,    -1,   208,    -1,
      -1,    -1,    -1,    -1,   709,    -1,    -1,   712,    -1,    -1,
      -1,    -1,    -1,    -1,   224,   225,    -1,    -1,    -1,   724,
     725,   726,    -1,   258,    -1,    -1,    -1,    -1,    49,    -1,
      -1,    -1,    49,    -1,   269,    -1,   246,    -1,   248,   249,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   258,    -1,
      -1,    -1,    -1,   758,    -1,    -1,    -1,    -1,    -1,   269,
      -1,    -1,   767,    -1,    -1,    -1,    -1,    -1,   773,   774,
      -1,   776,   307,    -1,    -1,    -1,    -1,   312,    -1,    -1,
      -1,    -1,    -1,    -1,   319,   320,   791,   108,    -1,    -1,
     795,   108,   224,   225,    -1,   208,    -1,   307,   308,   309,
     310,   311,   312,   313,   314,    -1,    -1,   317,    -1,   319,
      -1,    -1,   322,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   829,    -1,    -1,    -1,    -1,    -1,
      -1,   836,    -1,    -1,    -1,    -1,    -1,   842,    -1,   844,
     350,    -1,    -1,   378,    -1,   258,   851,    -1,    -1,   359,
     360,    -1,    -1,    -1,    -1,   390,   269,    -1,    -1,    -1,
      -1,    -1,    -1,   398,   869,   400,   376,    -1,   378,   379,
      -1,    -1,    -1,    -1,    -1,    -1,   308,   309,   310,   311,
     390,   313,   314,    -1,   394,    -1,    -1,   208,   398,    -1,
     400,   208,    -1,    -1,   307,    -1,    -1,    -1,   408,   312,
     435,    -1,    -1,    -1,    -1,    -1,   319,   320,    -1,    -1,
      -1,   421,    -1,    -1,    -1,    -1,    -1,    -1,     0,    -1,
      -1,    -1,    -1,    -1,    -1,   435,     8,     9,    10,    -1,
      -1,    13,    14,    15,    -1,    17,    -1,   258,    -1,    -1,
      -1,   258,    -1,    -1,    -1,    27,    -1,   379,   269,    -1,
      -1,    -1,   269,   463,    36,    37,    -1,    39,    40,    41,
      42,    43,    -1,    -1,    -1,   378,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   390,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   398,   307,   400,    -1,   421,
     307,   312,    -1,    -1,    -1,   312,    -1,    -1,   319,    -1,
      -1,    83,   319,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   549,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   435,    -1,   106,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   568,    -1,    -1,    -1,    -1,   548,   549,
     122,    -1,    -1,   125,   126,    -1,    -1,   129,    -1,    -1,
     132,   133,    -1,    -1,    -1,    -1,    -1,   378,   568,    -1,
      -1,   378,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   390,
      -1,    -1,    -1,   390,    -1,    -1,    -1,   398,    -1,   400,
      -1,   398,    -1,   400,    -1,    51,    52,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   609,
      -1,    -1,    -1,    -1,   639,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   435,    -1,   548,    -1,   435,   629,
      -1,    -1,    -1,   658,    -1,   635,    -1,    -1,    -1,   639,
      -1,    -1,    -1,    -1,    -1,    -1,   549,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      -1,    -1,    81,    82,    -1,   568,   666,    86,    -1,    -1,
      -1,   671,   672,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   709,    -1,   105,   609,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,    -1,   724,
     725,   726,    -1,    -1,    -1,    -1,    -1,    -1,   127,   709,
      -1,    -1,   712,   635,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   724,   725,   726,    -1,    -1,    -1,
      -1,    -1,    -1,   758,    -1,    -1,   639,   640,   549,    -1,
      -1,    -1,   549,    -1,    -1,    -1,    -1,    -1,   773,   774,
      -1,   776,    -1,    -1,    -1,   658,    -1,   568,   758,    -1,
      -1,   568,    -1,    -1,    -1,    -1,   791,   767,    -1,    -1,
     795,    -1,    -1,   773,   774,    -1,   776,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   241,   242,    -1,    -1,    -1,
     712,    -1,    -1,   249,    -1,   795,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   829,    -1,   709,    -1,    -1,    -1,
      -1,   836,    -1,    -1,    -1,    -1,    -1,   842,    -1,   844,
      -1,   724,   725,   726,    -1,    -1,   851,    -1,   639,   829,
      -1,    -1,   639,    -1,    -1,    -1,   836,    -1,    -1,    -1,
      -1,    -1,   842,    -1,   844,    -1,    -1,   658,    -1,    -1,
      -1,   851,    -1,    -1,    -1,   758,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   869,
     773,   774,    -1,   776,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   233,   234,    -1,    -1,   791,    -1,
      -1,    -1,   795,    -1,   350,    -1,    -1,    -1,   709,    -1,
      -1,    -1,   709,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   724,   725,   726,    -1,   724,   725,   726,
     376,    -1,    -1,    -1,    -1,    -1,   829,    -1,    -1,    -1,
     386,    -1,    -1,   836,    -1,    -1,    -1,    -1,   394,   842,
      -1,   844,    -1,    -1,    -1,    -1,    -1,   758,   851,    -1,
      -1,   758,   408,    -1,    -1,   411,    -1,    -1,    -1,    -1,
      -1,    -1,   773,   774,    -1,   776,   773,   774,   317,   776,
     426,    -1,    -1,   322,    -1,    -1,    -1,    -1,    -1,    -1,
     791,    -1,    -1,    -1,   795,    -1,    -1,    -1,   795,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   463,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   829,    -1,
      -1,    -1,   829,    -1,    -1,   836,    -1,    -1,    -1,   836,
     486,   842,    -1,   844,    -1,   842,    -1,   844,    -1,    -1,
     851,    -1,    -1,    -1,   851,    66,    67,    68,    69,    70,
      71,    72,   401,    -1,    75,    76,    -1,    -1,    -1,    -1,
      81,    82,    -1,    -1,   413,   414,    -1,    -1,   417,    -1,
      -1,    -1,   421,    -1,    -1,    -1,    -1,    -1,   534,    -1,
     536,   537,   538,   539,    -1,    -1,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,    -1,    -1,    -1,   555,
      -1,    -1,    -1,   452,    -1,    -1,   455,    -1,   457,    -1,
      -1,    -1,    -1,    -1,    -1,   571,    -1,   573,    -1,   575,
      -1,    -1,    -1,    -1,    -1,   581,   582,    -1,   584,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   592,    -1,   487,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,    -1,
     616,    -1,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,   627,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    60,    61,    -1,    -1,
      -1,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   683,    -1,    -1,
      84,    -1,   581,    87,    88,    -1,    90,    91,    -1,    93,
      -1,    95,    96,    97,    98,    99,   595,    -1,    -1,    -1,
      -1,   707,    -1,    -1,   710,   109,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   117,   613,    -1,    -1,    -1,   617,    -1,
      -1,    -1,   621,   729,    -1,    -1,   732,   733,    -1,    -1,
     629,    -1,    -1,    -1,    -1,    -1,   742,   743,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   662,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   784,   785,
      -1,    -1,    -1,   789,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   690,    -1,    -1,    -1,    -1,    -1,   803,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   813,   814,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   723,    -1,    -1,    -1,    -1,    -1,
      -1,   730,    -1,    -1,    -1,    -1,    -1,    -1,   737,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   852,   853,    -1,    -1,
      -1,    -1,   858,    -1,    -1,    -1,    -1,    -1,    -1,   865,
     759,    -1,    -1,   869,    -1,   871,   872,    -1,    -1,    -1,
      -1,   877,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   891,    -1,    -1,   894,    -1,
      -1,    -1,    -1,   899,    -1,    -1,    43,    -1,    -1,    -1,
      -1,    -1,    -1,   802,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   811,    -1,    -1,    -1,    -1,   816,    -1,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    -1,    -1,    81,    82,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   854,    -1,   856,   105,    -1,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   876,    -1,   878,
     127,    -1,    -1,     0,     1,    -1,     3,     4,     5,     6,
       7,    -1,    -1,   892,    11,    12,    -1,    -1,   897,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    63,    64,    65,    -1,
      -1,     0,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     8,
       9,    10,    -1,    -1,    -1,    14,    15,    84,    17,    -1,
      87,    88,    -1,    90,    91,    -1,    93,    26,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    36,    37,    -1,
      39,    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,
     117,   118,   119,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   132,   133,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      -1,    -1,    81,    82,    83,    -1,    85,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   104,   105,    -1,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,    -1,    -1,
      -1,    -1,   121,   122,   123,    -1,   125,   126,   127,     0,
      -1,   130,    -1,   132,   133,    -1,    -1,     8,     9,    10,
      -1,    -1,    -1,    14,    15,    -1,    17,    -1,    -1,    -1,
      -1,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    36,    37,    -1,    39,    40,
      41,    42,    43,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    -1,    -1,    81,
      82,    -1,    -1,    -1,    -1,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    -1,    -1,
      81,    82,    83,   105,    85,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   104,   105,    -1,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,    -1,    -1,    -1,    -1,
      -1,   122,   123,    -1,   125,   126,   127,    -1,    -1,   130,
      -1,   132,   133,     1,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    -1,    -1,    15,    16,    -1,
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
     118,   119,    -1,    -1,    -1,    -1,    -1,     1,    -1,     3,
       4,     5,     6,     7,   132,   133,    10,    11,    12,    -1,
      14,    15,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    60,    61,    -1,    63,
      64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    -1,    -1,    87,    88,    -1,    90,    91,    -1,    93,
      -1,    95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   117,   118,   119,     1,    -1,     3,     4,
       5,     6,     7,    -1,    -1,    10,    11,    12,   132,   133,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    87,    88,    -1,    90,    91,    -1,    93,    -1,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   117,   118,   119,     1,    -1,     3,     4,     5,
       6,     7,    -1,    -1,    10,    11,    12,   132,   133,    15,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,
      -1,    87,    88,    -1,    90,    91,    -1,    93,    -1,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   117,   118,   119,     1,    -1,     3,     4,     5,     6,
       7,    -1,     9,    10,    11,    12,   132,   133,    -1,    16,
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
      -1,    -1,    -1,    11,    12,   132,   133,    -1,    16,    -1,
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
     118,   119,    -1,    -1,    -1,    -1,    -1,     1,   126,     3,
       4,     5,     6,     7,   132,   133,    -1,    11,    12,    -1,
      -1,    -1,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    60,    61,    -1,    63,
      64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    -1,    -1,    87,    88,    -1,    90,    91,    -1,    93,
      -1,    95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   117,   118,   119,    -1,    -1,    -1,    -1,
      -1,     1,   126,     3,     4,     5,     6,     7,   132,   133,
      -1,    11,    12,    -1,    -1,    -1,    16,    -1,    18,    19,
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
      -1,    -1,   122,    -1,    -1,     1,    -1,     3,     4,     5,
       6,     7,   132,   133,    10,    11,    12,    -1,    -1,    -1,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,
      -1,    87,    88,    -1,    90,    91,    -1,    93,    -1,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   117,   118,   119,    -1,     3,     4,     5,     6,     7,
      -1,    -1,    -1,    11,    12,    -1,   132,   133,    16,    -1,
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
     118,   119,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,    -1,    -1,   133,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    -1,    -1,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,   132,   133,    13,    14,    15,    16,    17,    -1,
      -1,    20,    21,    22,    23,    24,    25,    26,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    63,    64,    -1,    66,    67,    68,
      69,    70,    71,    72,    -1,    -1,    75,    -1,    -1,    -1,
      79,    80,    81,    82,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,    -1,    -1,
     119,    -1,    -1,     3,     4,     5,    -1,     7,    -1,   128,
     129,    11,    12,    -1,    -1,    -1,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    26,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    93,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   117,   118,   119,
      16,   121,    18,    19,    20,    21,    22,    23,    24,   129,
      26,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,
      -1,    87,    88,    -1,    90,    91,    -1,    93,    94,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   117,   118,   119,    16,   121,    18,    19,    20,    21,
      22,    23,    24,   129,    26,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,    91,
      -1,    93,    94,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   117,   118,   119,    16,   121,
      18,    19,    20,    21,    22,    23,    24,   129,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    -1,    87,
      88,    -1,    90,    91,    -1,    93,    94,    95,    96,    97,
      98,    99,    -1,    -1,    -1,    -1,   104,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   117,
     118,   119,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,   129,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    60,    61,    -1,    63,
      64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    85,    -1,    87,    88,    -1,    90,    91,    -1,    93,
      94,    95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,
     104,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   117,   118,   119,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,   129,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,
      90,    91,    -1,    93,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   117,   118,   119,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,   129,
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
      22,    23,    24,   129,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,    91,
      -1,    93,    94,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   117,   118,   119,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,   129,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,
      88,    -1,    90,    91,    -1,    93,    94,    95,    96,    97,
      98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,
     118,   119,     3,     4,     5,     6,     7,     8,     9,    10,
      -1,   129,    13,    14,    15,    16,    17,    -1,    -1,    20,
      21,    22,    23,    24,    25,    26,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    63,    64,    -1,    66,    67,    68,    69,    70,
      71,    72,    -1,    -1,    75,    -1,    -1,    -1,    79,    80,
      81,    82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,    -1,    -1,   119,     3,
       4,     5,     6,     7,     8,     9,    10,   128,    -1,    13,
      14,    15,    16,    17,    -1,    -1,    20,    21,    22,    23,
      24,    25,    26,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    -1,
      54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    63,
      64,    -1,    66,    67,    68,    69,    70,    71,    72,    -1,
      -1,    75,    -1,    -1,    -1,    79,    80,    81,    82,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    93,
      -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,    -1,    -1,   119,     3,     4,     5,     6,
       7,     8,     9,    10,   128,    -1,    13,    14,    15,    16,
      17,    -1,    -1,    20,    21,    22,    23,    24,    25,    26,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    -1,    -1,    54,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    64,    -1,    66,
      67,    68,    69,    70,    71,    72,    -1,    -1,    75,    -1,
      -1,    -1,    79,    80,    81,    82,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    93,    -1,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
      -1,    -1,   119,     3,     4,     5,     6,     7,     8,     9,
      10,   128,    -1,    13,    14,    15,    16,    17,    -1,    -1,
      20,    21,    22,    23,    24,    25,    26,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    -1,    -1,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    63,    64,    -1,    66,    67,    68,    69,
      70,    71,    72,    -1,    -1,    75,    -1,    -1,    -1,    79,
      80,    81,    82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,    -1,    -1,   119,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,   128,    11,
      12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,    91,
      -1,    93,    94,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,   117,   118,   119,    11,    12,
      -1,    -1,    -1,    16,   126,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      -1,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
      63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    -1,    -1,    87,    88,    -1,    90,    91,    -1,
      93,    -1,    95,    96,    97,    98,    99,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   117,   118,   119,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    38,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,
      -1,    90,    91,    -1,    93,    94,    95,    96,    97,    98,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   117,   118,
     119,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    87,    88,    -1,    90,    91,    -1,    93,    94,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   117,   118,   119,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    93,    94,    95,    96,    97,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   117,   118,   119,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    63,    64,    65,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,
      87,    88,    -1,    90,    91,    -1,    93,    94,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     117,   118,   119,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
      63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    -1,    -1,    87,    88,    -1,    90,    91,    -1,
      93,    94,    95,    96,    97,    98,    99,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   117,   118,   119,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
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
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    87,    88,    -1,    90,    91,    -1,    93,    94,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   117,   118,   119,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    93,    94,    95,    96,    97,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   117,   118,   119,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    63,    64,    65,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,
      87,    88,    -1,    90,    91,    -1,    93,    94,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     117,   118,   119,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
      63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    -1,    -1,    87,    88,    -1,    90,    91,    -1,
      93,    94,    95,    96,    97,    98,    99,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   117,   118,   119,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
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
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    87,    88,    -1,    90,    91,    -1,    93,    94,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   117,   118,   119,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    -1,    -1,    95,    96,    97,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   117,   118,   119,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    63,    64,    65,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,
      87,    88,    -1,    90,    91,    -1,    93,    -1,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     117,   118,   119,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
      63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    -1,    -1,    87,    88,    -1,    90,    91,    -1,
      93,    -1,    95,    96,    97,    98,    99,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   117,   118,   119,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,
      -1,    90,    91,    -1,    93,    -1,    95,    96,    97,    98,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   117,   118,
     119,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    87,    88,    -1,    90,    91,    -1,    93,    -1,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   117,   118,   119,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    93,    -1,    95,    96,    97,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   117,   118,   119,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    63,    64,    65,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,
      87,    88,    -1,    90,    91,    -1,    93,    -1,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     117,   118,   119,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
      63,    64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    -1,    -1,    87,    88,    -1,    90,    91,    -1,
      93,    -1,    95,    96,    97,    98,    99,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   117,   118,   119,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      -1,    60,    61,    -1,    63,    64,    65,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,    88,
      -1,    90,    91,    -1,    -1,    -1,    95,    96,    97,    98,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   117,   118,
     119,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    87,    88,    -1,    90,    91,    -1,    -1,    -1,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   117,   118,   119,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      81,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    -1,    -1,    95,    96,    97,    98,    99,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,   117,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    93,    -1,    95,    96,    97,    98,    99,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,   117,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    93,    -1,    95,    96,    97,    98,    99,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,   117,    18,    19,    20,
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
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    87,    88,    -1,    90,
      91,    -1,    -1,    -1,    95,    96,    97,    98,    99,    -1,
      -1,    50,    51,    -1,    -1,    54,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    63,    64,   117,    66,    67,    68,
      69,    70,    71,    72,    -1,    -1,    75,    -1,    -1,    -1,
      79,    80,    81,    82,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,    50,    51,
     119,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,   128,
      -1,    63,    64,    -1,    66,    67,    68,    69,    70,    71,
      72,    -1,    -1,    75,    -1,    -1,    -1,    79,    80,    81,
      82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,    50,    51,   119,    -1,    54,
      -1,    -1,    -1,    -1,    -1,    -1,   128,    -1,    63,    64,
      -1,    66,    67,    68,    69,    70,    71,    72,    -1,    -1,
      75,    -1,    -1,    -1,    79,    80,    81,    82,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    93,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,    50,    51,   119,    -1,    54,    -1,    -1,    -1,
      -1,    -1,    -1,   128,    -1,    63,    64,    -1,    66,    67,
      68,    69,    70,    71,    72,    -1,    -1,    75,    -1,    -1,
      -1,    79,    80,    81,    82,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,    50,
      51,   119,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,
     128,    -1,    63,    64,    -1,    66,    67,    68,    69,    70,
      71,    72,    -1,    -1,    75,    -1,    -1,    -1,    79,    80,
      81,    82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,    50,    51,   119,    -1,
      54,    -1,    -1,    -1,    -1,    -1,    -1,   128,    -1,    63,
      64,    -1,    66,    67,    68,    69,    70,    71,    72,    -1,
      -1,    75,    -1,    -1,    -1,    79,    80,    81,    82,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    93,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,    50,    51,   119,    -1,    54,    -1,    -1,
      -1,    -1,    -1,    -1,   128,    -1,    63,    64,    -1,    66,
      67,    68,    69,    70,    71,    72,    -1,    -1,    75,    -1,
      -1,    -1,    79,    80,    81,    82,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
      50,    51,   119,    -1,    54,    -1,    -1,    -1,    -1,    -1,
      -1,   128,    -1,    63,    64,    -1,    66,    67,    68,    69,
      70,    71,    72,    -1,    -1,    75,    -1,    -1,    -1,    79,
      80,    81,    82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,    50,    51,   119,
      -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,   128,    -1,
      63,    64,    -1,    66,    67,    68,    69,    70,    71,    72,
      -1,    -1,    75,    -1,    -1,    -1,    79,    80,    81,    82,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,    50,    51,   119,    -1,    54,    -1,
      -1,    -1,    -1,    -1,    -1,   128,    -1,    63,    64,    -1,
      66,    67,    68,    69,    70,    71,    72,    -1,    -1,    75,
      -1,    -1,    -1,    79,    80,    81,    82,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    93,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,    50,    51,   119,    -1,    54,    -1,    -1,    -1,    -1,
      -1,    -1,   128,    -1,    63,    64,    -1,    66,    67,    68,
      69,    70,    71,    72,    -1,    -1,    75,    -1,    -1,    -1,
      79,    80,    81,    82,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,    50,    51,
     119,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,   128,
      -1,    63,    64,    -1,    66,    67,    68,    69,    70,    71,
      72,    -1,    -1,    75,    -1,    -1,    -1,    79,    80,    81,
      82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,    -1,    -1,   119,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   128,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      -1,    -1,    81,    82,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   105,    -1,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,    -1,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    -1,   132,    81,    82,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   105,    -1,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     127,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    -1,    -1,    81,    82,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    -1,    -1,    81,    82,    -1,    -1,    -1,    -1,    -1,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,    -1,    -1,    -1,    -1,    -1,   105,    -1,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    -1,    -1,    81,    82,    66,    67,    68,    69,
      70,    71,    72,    73,    -1,    75,    76,    -1,    -1,    -1,
      -1,    81,    82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116
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
     153,   154,   155,   156,   157,   166,   184,   201,   211,   212,
     225,   226,   227,   228,   229,   230,   231,   234,   242,   244,
     245,   246,   247,   248,   249,   268,   277,   140,    21,    22,
      29,    30,    31,    45,    50,    54,    81,    84,    87,   117,
     158,   159,   184,   201,   246,   249,   268,   159,     3,     4,
       5,     6,     7,     8,     9,    10,    13,    14,    15,    16,
      17,    20,    21,    22,    23,    24,    25,    26,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    54,    63,    64,    66,    67,    68,    69,    70,    71,
      72,    75,    79,    80,    81,    82,    93,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   119,   128,   129,
     160,   164,   165,   247,   263,    32,    33,    34,    35,    48,
      49,    50,    54,    95,   160,   161,   162,   242,   185,    84,
     143,   144,   157,   201,   246,   248,   249,   144,   132,   133,
     144,   272,   275,   276,   188,   190,    84,   151,   157,   201,
     206,   246,   249,    93,    94,   118,   150,   166,   169,   173,
     180,   182,   266,   267,   173,   173,   129,   175,   176,   129,
     171,   175,   143,    52,   161,   132,   273,   142,   121,   237,
     166,   201,   166,    50,    51,    54,   164,   269,    54,    87,
     138,   152,   153,   143,    93,   150,   170,   182,   266,   277,
     182,   265,   266,   277,    84,   156,   201,   246,   249,    52,
      53,    55,   160,   237,   243,   236,   237,   131,   232,   131,
     235,    57,    58,   145,   166,   166,   272,   276,    39,    40,
      41,    42,    43,    36,    37,    28,   208,   104,   127,    87,
      93,   154,   104,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    81,    82,   105,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,    83,
     123,   125,   130,    83,   125,    26,   121,   213,   228,    85,
      85,   171,   175,   213,   273,   143,    50,    54,   158,    57,
      58,     1,   108,   250,   275,    83,   123,   125,   197,   264,
     198,    83,   125,   271,   127,   137,   138,    54,    13,   106,
     202,   275,   104,    83,   123,   125,    85,    85,   202,   272,
      15,    17,   218,   133,   144,   144,    54,    83,   123,   125,
      25,   169,   169,    86,   127,   181,   277,   127,   181,   126,
     173,    88,   173,   177,   150,   173,   182,   211,   277,    52,
      60,    61,   141,   129,   167,   121,   138,    59,   100,   101,
     102,   238,    83,   125,    85,   152,   126,   126,   186,   166,
     273,   124,   127,   132,   274,   127,   274,   127,   274,   122,
     274,    54,    83,   123,   125,   102,   102,   238,    62,   102,
     102,   233,   238,   102,    59,   102,    66,    66,   140,   144,
     144,   144,   144,   140,   143,   143,   209,    93,   145,   169,
     182,   183,   152,   156,   127,   145,   166,   169,   183,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   168,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,    50,    51,    54,   164,   269,
     270,   170,    50,    54,   269,   269,   269,   269,   216,   214,
     145,   166,   145,   166,    92,   147,   195,   275,   251,   194,
      50,    54,   158,   269,   170,   269,   137,   143,    50,    52,
      53,    54,    55,    93,    94,   111,   114,   129,   252,   253,
     254,   255,   256,   257,   258,   259,   260,   261,   199,   163,
      10,     8,   221,   277,   138,    13,   166,    50,    54,   170,
      50,    54,   138,   218,   138,    93,   182,   219,    10,    27,
     106,   203,   275,   203,    50,    54,   170,    50,    54,   192,
     127,   181,   169,    93,   169,   180,   266,    93,   169,   267,
     126,    93,   166,   169,   174,   178,   180,   266,   273,   127,
      83,   125,   273,   126,   161,   187,   166,   138,   122,   240,
     239,   166,   126,   273,   273,    93,   169,    50,    54,   170,
      50,    54,   131,   238,   131,   166,   166,    74,   109,   207,
     277,   169,   127,   126,    43,   166,    85,    85,   171,   175,
     124,    85,    85,   171,   172,   175,   277,   172,   175,   172,
     175,   207,   207,   148,   275,   144,   137,   124,    10,   273,
     104,   253,   137,   275,   127,   262,   277,   127,   262,    50,
     127,   262,    50,   160,   161,   169,   183,   222,   277,    15,
     205,   277,    14,   204,   205,    85,   124,    85,    85,   205,
      10,    10,   169,   127,   202,   189,   191,   124,   144,   169,
     127,   181,   169,   169,   127,   179,   126,   127,   181,   126,
     150,   211,   269,   269,   126,   143,   122,   138,    52,    53,
      55,   241,   249,   126,   166,   124,   109,   206,   210,    93,
     169,   166,   106,   145,   166,   166,   147,    85,   145,   166,
     145,   166,   147,   217,   215,   207,   196,   275,    10,   126,
     169,   273,    10,   254,   257,   259,   261,    50,   256,   259,
     200,    86,   223,   277,   138,     9,   224,   277,   144,    10,
      85,    10,    93,   138,   138,   138,   203,   181,    93,   181,
     181,    93,   180,   182,   266,   126,    93,   273,   126,   122,
     273,   109,   138,   169,   166,   145,   166,   138,   138,   149,
     137,   126,   127,   262,   262,   262,   252,    84,   157,   201,
     246,   249,   202,   138,   202,   169,   205,   218,   220,    10,
      10,   193,   166,   169,   127,   181,   127,   181,   169,   126,
      10,    10,   122,   138,    10,   259,   137,    54,    83,   123,
     125,   138,   138,   138,   181,   181,    93,   266,    93,   181,
     122,   262,    10,    50,    54,   170,    50,    54,   221,   204,
      10,   169,   127,   181,   169,   124,   181,    93,   181,   169,
     181
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
#line 429 "grammar.y"
    {
                        vps->lex_state = EXPR_BEG;
                        vps->variables = var_table_create();
                        // rb_funcall(vps->self, rb_intern("local_init"), 0);
                        // top_local_init();
                        class_nest = 0;
                        // if (ruby_class == rb_cObject) class_nest = 0;
                        // else class_nest = 1;
                    ;}
    break;

  case 3:
#line 439 "grammar.y"
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
                        vps->top = block_append(parse_state, vps->top, (yyvsp[(2) - (2)].node)); 
                        //vps->top = NEW_SCOPE(block_append(vps->top, $2)); 
                        // rb_funcall(vps->self, rb_intern("local_finish"), 0);
                        
                        // top_local_setup();
                        class_nest = 0;
                    ;}
    break;

  case 4:
#line 464 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (4)].node);
                        if ((yyvsp[(2) - (4)].node)) {
                            (yyval.node) = NEW_RESCUE((yyvsp[(1) - (4)].node), (yyvsp[(2) - (4)].node), (yyvsp[(3) - (4)].node));
                        }
                        else if ((yyvsp[(3) - (4)].node)) {
                            rb_warn("else without rescue is useless");
                            (yyval.node) = block_append(parse_state, (yyval.node), (yyvsp[(3) - (4)].node));
                        }
                        if ((yyvsp[(4) - (4)].node)) {
                            (yyval.node) = NEW_ENSURE((yyval.node), (yyvsp[(4) - (4)].node));
                        }
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 5:
#line 481 "grammar.y"
    {
                        void_stmts((yyvsp[(1) - (2)].node), parse_state);
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 7:
#line 489 "grammar.y"
    {
                        (yyval.node) = newline_node(parse_state, (yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 8:
#line 493 "grammar.y"
    {
                        (yyval.node) = block_append(parse_state, (yyvsp[(1) - (3)].node), newline_node(parse_state, (yyvsp[(3) - (3)].node)));
                    ;}
    break;

  case 9:
#line 497 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 10:
#line 502 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 11:
#line 503 "grammar.y"
    {
                        (yyval.node) = NEW_ALIAS((yyvsp[(2) - (4)].id), (yyvsp[(4) - (4)].id));
                    ;}
    break;

  case 12:
#line 507 "grammar.y"
    {
                        (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 13:
#line 511 "grammar.y"
    {
                        char buf[3];

                        sprintf(buf, "$%c", (char)(yyvsp[(3) - (3)].node)->nd_nth);
                        (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), rb_intern(buf));
                    ;}
    break;

  case 14:
#line 518 "grammar.y"
    {
                        yyerror("can't make alias for the number variables");
                        (yyval.node) = 0;
                    ;}
    break;

  case 15:
#line 523 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 16:
#line 527 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(3) - (3)].node), parse_state), (yyvsp[(1) - (3)].node), 0);
                        fixpos((yyval.node), (yyvsp[(3) - (3)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            (yyval.node)->nd_else = (yyval.node)->nd_body;
                            (yyval.node)->nd_body = 0;
                        }
                    ;}
    break;

  case 17:
#line 536 "grammar.y"
    {
                        (yyval.node) = NEW_UNLESS(cond((yyvsp[(3) - (3)].node), parse_state), (yyvsp[(1) - (3)].node), 0);
                        fixpos((yyval.node), (yyvsp[(3) - (3)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            (yyval.node)->nd_body = (yyval.node)->nd_else;
                            (yyval.node)->nd_else = 0;
                        }
                    ;}
    break;

  case 18:
#line 545 "grammar.y"
    {
                        if ((yyvsp[(1) - (3)].node) && nd_type((yyvsp[(1) - (3)].node)) == NODE_BEGIN) {
                            (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (3)].node), parse_state), (yyvsp[(1) - (3)].node)->nd_body, 0);
                        }
                        else {
                            (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (3)].node), parse_state), (yyvsp[(1) - (3)].node), 1);
                        }
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            nd_set_type((yyval.node), NODE_UNTIL);
                        }
                    ;}
    break;

  case 19:
#line 557 "grammar.y"
    {
                        if ((yyvsp[(1) - (3)].node) && nd_type((yyvsp[(1) - (3)].node)) == NODE_BEGIN) {
                            (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (3)].node), parse_state), (yyvsp[(1) - (3)].node)->nd_body, 0);
                        }
                        else {
                            (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (3)].node), parse_state), (yyvsp[(1) - (3)].node), 1);
                        }
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            nd_set_type((yyval.node), NODE_WHILE);
                        }
                    ;}
    break;

  case 20:
#line 569 "grammar.y"
    {
                        (yyval.node) = NEW_RESCUE((yyvsp[(1) - (3)].node), NEW_RESBODY(0,(yyvsp[(3) - (3)].node),0), 0);
                    ;}
    break;

  case 21:
#line 573 "grammar.y"
    {
                        if (in_def || in_single) {
                            yyerror("BEGIN in method");
                        }
                        local_push(0);
                    ;}
    break;

  case 22:
#line 580 "grammar.y"
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
#line 589 "grammar.y"
    {
                        if (in_def || in_single) {
                            rb_warn("END in method; use at_exit");
                        }

                        (yyval.node) = NEW_ITER(0, NEW_POSTEXE(), (yyvsp[(3) - (4)].node));
                    ;}
    break;

  case 24:
#line 597 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 25:
#line 601 "grammar.y"
    {
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyvsp[(1) - (3)].node)->nd_value = ((yyvsp[(1) - (3)].node)->nd_head) ? NEW_TO_ARY((yyvsp[(3) - (3)].node)) : NEW_ARRAY((yyvsp[(3) - (3)].node));
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 26:
#line 607 "grammar.y"
    {
                        value_expr((yyvsp[(3) - (3)].node));
                        if ((yyvsp[(1) - (3)].node)) {
                            ID vid = (yyvsp[(1) - (3)].node)->nd_vid;
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
                                (yyval.node)->nd_value = call_op(gettable(vid),(yyvsp[(2) - (3)].id),1,(yyvsp[(3) - (3)].node), parse_state);
                            }
                        }
                        else {
                            (yyval.node) = 0;
                        }
                    ;}
    break;

  case 27:
#line 632 "grammar.y"
    {
                        NODE *args;

                        value_expr((yyvsp[(6) - (6)].node));
                        args = NEW_LIST((yyvsp[(6) - (6)].node));
                        if ((yyvsp[(3) - (6)].node) && nd_type((yyvsp[(3) - (6)].node)) != NODE_ARRAY)
                            (yyvsp[(3) - (6)].node) = NEW_LIST((yyvsp[(3) - (6)].node));
                        (yyvsp[(3) - (6)].node) = list_append(parse_state, (yyvsp[(3) - (6)].node), NEW_NIL());
                        list_concat(args, (yyvsp[(3) - (6)].node));
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
#line 651 "grammar.y"
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
#line 663 "grammar.y"
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
#line 675 "grammar.y"
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
#line 687 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (3)].node));
                        (yyval.node) = 0;
                    ;}
    break;

  case 32:
#line 692 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), NEW_SVALUE((yyvsp[(3) - (3)].node)), parse_state);
                    ;}
    break;

  case 33:
#line 696 "grammar.y"
    {
                        (yyvsp[(1) - (3)].node)->nd_value = ((yyvsp[(1) - (3)].node)->nd_head) ? NEW_TO_ARY((yyvsp[(3) - (3)].node)) : NEW_ARRAY((yyvsp[(3) - (3)].node));
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 34:
#line 701 "grammar.y"
    {
                        (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 37:
#line 710 "grammar.y"
    {
                        (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 38:
#line 714 "grammar.y"
    {
                        (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 39:
#line 718 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), parse_state));
                    ;}
    break;

  case 40:
#line 722 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), parse_state));
                    ;}
    break;

  case 42:
#line 729 "grammar.y"
    {
                        value_expr((yyval.node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 45:
#line 738 "grammar.y"
    {
                        (yyval.node) = NEW_RETURN(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 46:
#line 742 "grammar.y"
    {
                        (yyval.node) = NEW_BREAK(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 47:
#line 746 "grammar.y"
    {
                        (yyval.node) = NEW_NEXT(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 49:
#line 753 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 50:
#line 757 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 51:
#line 763 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 52:
#line 767 "grammar.y"
    { (yyval.vars) = vps->block_vars; ;}
    break;

  case 53:
#line 770 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 54:
#line 777 "grammar.y"
    {
                        (yyval.node) = new_fcall(parse_state, (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                   ;}
    break;

  case 55:
#line 782 "grammar.y"
    {
                        (yyval.node) = new_fcall(parse_state, (yyvsp[(1) - (3)].id), (yyvsp[(2) - (3)].node));
                        if ((yyvsp[(3) - (3)].node)) {
                            if (nd_type((yyval.node)) == NODE_BLOCK_PASS) {
                                rb_compile_error("both block arg and actual block given");
                            }
                            (yyvsp[(3) - (3)].node)->nd_iter = (yyval.node);
                            (yyval.node) = (yyvsp[(3) - (3)].node);
                        }
                        fixpos((yyval.node), (yyvsp[(2) - (3)].node));
                   ;}
    break;

  case 56:
#line 794 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 57:
#line 799 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].node));
                        if ((yyvsp[(5) - (5)].node)) {
                            if (nd_type((yyval.node)) == NODE_BLOCK_PASS) {
                                rb_compile_error("both block arg and actual block given");
                            }
                            (yyvsp[(5) - (5)].node)->nd_iter = (yyval.node);
                            (yyval.node) = (yyvsp[(5) - (5)].node);
                        }
                        fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                   ;}
    break;

  case 58:
#line 811 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 59:
#line 816 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].node));
                        if ((yyvsp[(5) - (5)].node)) {
                            if (nd_type((yyval.node)) == NODE_BLOCK_PASS) {
                                rb_compile_error("both block arg and actual block given");
                            }
                            (yyvsp[(5) - (5)].node)->nd_iter = (yyval.node);
                            (yyval.node) = (yyvsp[(5) - (5)].node);
                        }
                        fixpos((yyval.node), (yyvsp[(1) - (5)].node));
                   ;}
    break;

  case 60:
#line 828 "grammar.y"
    {
                        (yyval.node) = new_super(parse_state, (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 61:
#line 833 "grammar.y"
    {
                        (yyval.node) = new_yield(parse_state, (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 63:
#line 841 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 65:
#line 848 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(NEW_LIST((yyvsp[(2) - (3)].node)), 0);
                    ;}
    break;

  case 66:
#line 854 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (1)].node), 0);
                    ;}
    break;

  case 67:
#line 858 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(list_append(parse_state, (yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node)), 0);
                    ;}
    break;

  case 68:
#line 862 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 69:
#line 866 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (2)].node), -1);
                    ;}
    break;

  case 70:
#line 870 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(0, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 71:
#line 874 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(0, -1);
                    ;}
    break;

  case 73:
#line 881 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 74:
#line 887 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 75:
#line 891 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, (yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 76:
#line 897 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, parse_state);
                    ;}
    break;

  case 77:
#line 901 "grammar.y"
    {
                        (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), parse_state);
                    ;}
    break;

  case 78:
#line 905 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), parse_state);
                    ;}
    break;

  case 79:
#line 909 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), parse_state);
                    ;}
    break;

  case 80:
#line 913 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), parse_state);
                    ;}
    break;

  case 81:
#line 917 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                    ;}
    break;

  case 82:
#line 923 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                    ;}
    break;

  case 83:
#line 929 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (1)].node));
                        (yyval.node) = 0;
                    ;}
    break;

  case 84:
#line 936 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, parse_state);
                    ;}
    break;

  case 85:
#line 940 "grammar.y"
    {
                        (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), parse_state);
                    ;}
    break;

  case 86:
#line 944 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), parse_state);
                    ;}
    break;

  case 87:
#line 948 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), parse_state);
                    ;}
    break;

  case 88:
#line 952 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), parse_state);
                    ;}
    break;

  case 89:
#line 956 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                    ;}
    break;

  case 90:
#line 962 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                    ;}
    break;

  case 91:
#line 968 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (1)].node));
                        (yyval.node) = 0;
                    ;}
    break;

  case 92:
#line 975 "grammar.y"
    {
                        yyerror("class/module name must be CONSTANT");
                    ;}
    break;

  case 94:
#line 982 "grammar.y"
    {
                        (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 95:
#line 986 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2(0, (yyval.node));
                    ;}
    break;

  case 96:
#line 990 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 100:
#line 999 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = convert_op((yyvsp[(1) - (1)].id));
                    ;}
    break;

  case 101:
#line 1004 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = (yyvsp[(1) - (1)].id);
                    ;}
    break;

  case 104:
#line 1015 "grammar.y"
    {
                        (yyval.node) = NEW_UNDEF((yyvsp[(1) - (1)].id));
                    ;}
    break;

  case 105:
#line 1018 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 106:
#line 1019 "grammar.y"
    {
                        (yyval.node) = block_append(parse_state, (yyvsp[(1) - (4)].node), NEW_UNDEF((yyvsp[(4) - (4)].id)));
                    ;}
    break;

  case 107:
#line 1024 "grammar.y"
    { (yyval.id) = '|'; ;}
    break;

  case 108:
#line 1025 "grammar.y"
    { (yyval.id) = '^'; ;}
    break;

  case 109:
#line 1026 "grammar.y"
    { (yyval.id) = '&'; ;}
    break;

  case 110:
#line 1027 "grammar.y"
    { (yyval.id) = tCMP; ;}
    break;

  case 111:
#line 1028 "grammar.y"
    { (yyval.id) = tEQ; ;}
    break;

  case 112:
#line 1029 "grammar.y"
    { (yyval.id) = tNEQ; ;}
    break;

  case 113:
#line 1030 "grammar.y"
    { (yyval.id) = tEQQ; ;}
    break;

  case 114:
#line 1031 "grammar.y"
    { (yyval.id) = tMATCH; ;}
    break;

  case 115:
#line 1032 "grammar.y"
    { (yyval.id) = '>'; ;}
    break;

  case 116:
#line 1033 "grammar.y"
    { (yyval.id) = tGEQ; ;}
    break;

  case 117:
#line 1034 "grammar.y"
    { (yyval.id) = '<'; ;}
    break;

  case 118:
#line 1035 "grammar.y"
    { (yyval.id) = tLEQ; ;}
    break;

  case 119:
#line 1036 "grammar.y"
    { (yyval.id) = tLSHFT; ;}
    break;

  case 120:
#line 1037 "grammar.y"
    { (yyval.id) = tRSHFT; ;}
    break;

  case 121:
#line 1038 "grammar.y"
    { (yyval.id) = '+'; ;}
    break;

  case 122:
#line 1039 "grammar.y"
    { (yyval.id) = '-'; ;}
    break;

  case 123:
#line 1040 "grammar.y"
    { (yyval.id) = '*'; ;}
    break;

  case 124:
#line 1041 "grammar.y"
    { (yyval.id) = '*'; ;}
    break;

  case 125:
#line 1042 "grammar.y"
    { (yyval.id) = '/'; ;}
    break;

  case 126:
#line 1043 "grammar.y"
    { (yyval.id) = '%'; ;}
    break;

  case 127:
#line 1044 "grammar.y"
    { (yyval.id) = tPOW; ;}
    break;

  case 128:
#line 1045 "grammar.y"
    { (yyval.id) = '~'; ;}
    break;

  case 129:
#line 1046 "grammar.y"
    { (yyval.id) = tUPLUS; ;}
    break;

  case 130:
#line 1047 "grammar.y"
    { (yyval.id) = tUMINUS; ;}
    break;

  case 131:
#line 1048 "grammar.y"
    { (yyval.id) = tAREF; ;}
    break;

  case 132:
#line 1049 "grammar.y"
    { (yyval.id) = tASET; ;}
    break;

  case 133:
#line 1050 "grammar.y"
    { (yyval.id) = '`'; ;}
    break;

  case 175:
#line 1063 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 176:
#line 1067 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (5)].node), NEW_RESCUE((yyvsp[(3) - (5)].node), NEW_RESBODY(0,(yyvsp[(5) - (5)].node),0), 0), parse_state);
                    ;}
    break;

  case 177:
#line 1071 "grammar.y"
    {
                        value_expr((yyvsp[(3) - (3)].node));
                        if ((yyvsp[(1) - (3)].node)) {
                            ID vid = (yyvsp[(1) - (3)].node)->nd_vid;
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
                                (yyval.node)->nd_value = call_op(gettable(vid),(yyvsp[(2) - (3)].id),1,(yyvsp[(3) - (3)].node), parse_state);
                            }
                        }
                        else {
                            (yyval.node) = 0;
                        }
                    ;}
    break;

  case 178:
#line 1096 "grammar.y"
    {
                        NODE *args;

                        value_expr((yyvsp[(6) - (6)].node));
                        args = NEW_LIST((yyvsp[(6) - (6)].node));
                        if ((yyvsp[(3) - (6)].node) && nd_type((yyvsp[(3) - (6)].node)) != NODE_ARRAY)
                            (yyvsp[(3) - (6)].node) = NEW_LIST((yyvsp[(3) - (6)].node));
                        (yyvsp[(3) - (6)].node) = list_append(parse_state, (yyvsp[(3) - (6)].node), NEW_NIL());
                        list_concat(args, (yyvsp[(3) - (6)].node));
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

  case 179:
#line 1115 "grammar.y"
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

  case 180:
#line 1127 "grammar.y"
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
#line 1139 "grammar.y"
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
#line 1151 "grammar.y"
    {
                        yyerror("constant re-assignment");
                        (yyval.node) = 0;
                    ;}
    break;

  case 183:
#line 1156 "grammar.y"
    {
                        yyerror("constant re-assignment");
                        (yyval.node) = 0;
                    ;}
    break;

  case 184:
#line 1161 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (3)].node));
                        (yyval.node) = 0;
                    ;}
    break;

  case 185:
#line 1166 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (3)].node));
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyval.node) = NEW_DOT2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 186:
#line 1172 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (3)].node));
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyval.node) = NEW_DOT3((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 187:
#line 1178 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '+', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 188:
#line 1182 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '-', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 189:
#line 1186 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '*', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 190:
#line 1190 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '/', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 191:
#line 1194 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '%', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 192:
#line 1198 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tPOW, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 193:
#line 1202 "grammar.y"
    {
                        (yyval.node) = call_op(call_op((yyvsp[(2) - (4)].node), tPOW, 1, (yyvsp[(4) - (4)].node), parse_state), tUMINUS, 0, 0, parse_state);
                    ;}
    break;

  case 194:
#line 1206 "grammar.y"
    {
                        (yyval.node) = call_op(call_op((yyvsp[(2) - (4)].node), tPOW, 1, (yyvsp[(4) - (4)].node), parse_state), tUMINUS, 0, 0, parse_state);
                    ;}
    break;

  case 195:
#line 1210 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(2) - (2)].node), tUPLUS, 0, 0, parse_state);
                    ;}
    break;

  case 196:
#line 1214 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(2) - (2)].node), tUMINUS, 0, 0, parse_state);
                    ;}
    break;

  case 197:
#line 1218 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '|', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 198:
#line 1222 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '^', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 199:
#line 1226 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '&', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 200:
#line 1230 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tCMP, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 201:
#line 1234 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '>', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 202:
#line 1238 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tGEQ, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 203:
#line 1242 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '<', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 204:
#line 1246 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tLEQ, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 205:
#line 1250 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tEQ, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 206:
#line 1254 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tEQQ, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 207:
#line 1258 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tNEQ, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 208:
#line 1262 "grammar.y"
    {
                        (yyval.node) = match_gen((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 209:
#line 1266 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(match_gen((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state));
                    ;}
    break;

  case 210:
#line 1270 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), parse_state));
                    ;}
    break;

  case 211:
#line 1274 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(2) - (2)].node), '~', 0, 0, parse_state);
                    ;}
    break;

  case 212:
#line 1278 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tLSHFT, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 213:
#line 1282 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tRSHFT, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 214:
#line 1286 "grammar.y"
    {
                        (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 215:
#line 1290 "grammar.y"
    {
                        (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 216:
#line 1293 "grammar.y"
    {vps->in_defined = 1;;}
    break;

  case 217:
#line 1294 "grammar.y"
    {
                        vps->in_defined = 0;
                        (yyval.node) = NEW_DEFINED((yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 218:
#line 1298 "grammar.y"
    {vps->ternary_colon++;;}
    break;

  case 219:
#line 1299 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(1) - (6)].node), parse_state), (yyvsp[(4) - (6)].node), (yyvsp[(6) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (6)].node));
                        vps->ternary_colon--;
                    ;}
    break;

  case 220:
#line 1305 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 221:
#line 1311 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (1)].node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 223:
#line 1319 "grammar.y"
    {
                        rb_warn("parenthesize argument(s) for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 224:
#line 1324 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 225:
#line 1328 "grammar.y"
    {
                        value_expr((yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_concat(parse_state, (yyvsp[(1) - (5)].node), (yyvsp[(4) - (5)].node));
                    ;}
    break;

  case 226:
#line 1333 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                    ;}
    break;

  case 227:
#line 1337 "grammar.y"
    {
                        value_expr((yyvsp[(2) - (3)].node));
                        (yyval.node) = NEW_NEWLINE(NEW_SPLAT((yyvsp[(2) - (3)].node)));
                    ;}
    break;

  case 228:
#line 1344 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 229:
#line 1348 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                    ;}
    break;

  case 230:
#line 1352 "grammar.y"
    {
                        rb_warn("parenthesize argument for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(2) - (4)].node));
                    ;}
    break;

  case 231:
#line 1357 "grammar.y"
    {
                        rb_warn("parenthesize argument for future version");
                        (yyval.node) = list_append(parse_state, (yyvsp[(2) - (6)].node), (yyvsp[(4) - (6)].node));
                    ;}
    break;

  case 234:
#line 1368 "grammar.y"
    {
                        rb_warn("parenthesize argument(s) for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 235:
#line 1373 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 236:
#line 1377 "grammar.y"
    {
                        (yyval.node) = arg_concat(parse_state, (yyvsp[(1) - (5)].node), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 237:
#line 1382 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_POSITIONAL((yyvsp[(1) - (2)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 238:
#line 1387 "grammar.y"
    {
                        (yyval.node) = arg_concat(parse_state, NEW_LIST(NEW_POSITIONAL((yyvsp[(1) - (5)].node))), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 239:
#line 1392 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, (yyvsp[(1) - (4)].node), NEW_POSITIONAL((yyvsp[(3) - (4)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 240:
#line 1397 "grammar.y"
    {
                        value_expr((yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_concat(parse_state, list_append(parse_state, (yyvsp[(1) - (7)].node), NEW_POSITIONAL((yyvsp[(3) - (7)].node))), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 241:
#line 1403 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(NEW_SPLAT((yyvsp[(2) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 243:
#line 1410 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(list_concat(NEW_LIST((yyvsp[(1) - (4)].node)),(yyvsp[(3) - (4)].node)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 244:
#line 1414 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 245:
#line 1418 "grammar.y"
    {
                        (yyval.node) = arg_concat(parse_state, NEW_LIST((yyvsp[(1) - (5)].node)), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 246:
#line 1423 "grammar.y"
    {
            (yyval.node) = arg_concat(parse_state, list_concat(NEW_LIST((yyvsp[(1) - (7)].node)),(yyvsp[(3) - (7)].node)), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 247:
#line 1428 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_POSITIONAL((yyvsp[(1) - (2)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 248:
#line 1433 "grammar.y"
    {
                        (yyval.node) = arg_concat(parse_state, NEW_LIST(NEW_POSITIONAL((yyvsp[(1) - (5)].node))), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 249:
#line 1438 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, NEW_LIST((yyvsp[(1) - (4)].node)), NEW_POSITIONAL((yyvsp[(3) - (4)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 250:
#line 1443 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, list_concat(NEW_LIST((yyvsp[(1) - (6)].node)),(yyvsp[(3) - (6)].node)), NEW_POSITIONAL((yyvsp[(5) - (6)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(6) - (6)].node));
                    ;}
    break;

  case 251:
#line 1448 "grammar.y"
    {
                        (yyval.node) = arg_concat(parse_state, list_append(parse_state, NEW_LIST((yyvsp[(1) - (7)].node)), NEW_POSITIONAL((yyvsp[(3) - (7)].node))), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 252:
#line 1453 "grammar.y"
    {
                        (yyval.node) = arg_concat(parse_state, list_append(parse_state, list_concat(NEW_LIST((yyvsp[(1) - (9)].node)), (yyvsp[(3) - (9)].node)), NEW_POSITIONAL((yyvsp[(5) - (9)].node))), (yyvsp[(8) - (9)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(9) - (9)].node));
                    ;}
    break;

  case 253:
#line 1458 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(NEW_SPLAT((yyvsp[(2) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 255:
#line 1464 "grammar.y"
    {
                        (yyval.num) = vps->cmdarg_stack;
                        CMDARG_PUSH(1);
                    ;}
    break;

  case 256:
#line 1469 "grammar.y"
    {
                        /* CMDARG_POP() */
                        vps->cmdarg_stack = (yyvsp[(1) - (2)].num);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 258:
#line 1477 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 259:
#line 1478 "grammar.y"
    {
                        rb_warn("don't put space before argument parentheses");
                        (yyval.node) = 0;
                    ;}
    break;

  case 260:
#line 1482 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 261:
#line 1483 "grammar.y"
    {
                        rb_warn("don't put space before argument parentheses");
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                    ;}
    break;

  case 262:
#line 1490 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK_PASS((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 263:
#line 1496 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 265:
#line 1503 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 266:
#line 1507 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 267:
#line 1513 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 268:
#line 1517 "grammar.y"
    {
                        (yyval.node) = arg_concat(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 269:
#line 1521 "grammar.y"
    {
                        (yyval.node) = NEW_SPLAT((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 278:
#line 1535 "grammar.y"
    {
                        (yyval.node) = NEW_FCALL((yyvsp[(1) - (1)].id), 0);
                    ;}
    break;

  case 279:
#line 1539 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                    ;}
    break;

  case 280:
#line 1544 "grammar.y"
    {
                        if ((yyvsp[(3) - (4)].node) == NULL)
                            (yyval.node) = NEW_NIL();
                        else
                            (yyval.node) = NEW_BEGIN((yyvsp[(3) - (4)].node));
                        nd_set_line((yyval.node), (yyvsp[(1) - (4)].num));
                    ;}
    break;

  case 281:
#line 1551 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 282:
#line 1552 "grammar.y"
    {
                        rb_warning("(...) interpreted as grouped expression");
                        (yyval.node) = (yyvsp[(2) - (5)].node);
                    ;}
    break;

  case 283:
#line 1557 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 284:
#line 1561 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 285:
#line 1565 "grammar.y"
    {
                        (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 286:
#line 1569 "grammar.y"
    {
                        if ((yyvsp[(1) - (4)].node) && nd_type((yyvsp[(1) - (4)].node)) == NODE_SELF) {
                            (yyval.node) = NEW_FCALL(convert_op(tAREF), (yyvsp[(3) - (4)].node));
                        } else {
                            (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), convert_op(tAREF), (yyvsp[(3) - (4)].node));
                        }
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 287:
#line 1578 "grammar.y"
    {
                        if ((yyvsp[(2) - (3)].node) == 0) {
                            (yyval.node) = NEW_ZARRAY(); /* zero length array*/
                        }
                        else {
                            (yyval.node) = (yyvsp[(2) - (3)].node);
                        }
                    ;}
    break;

  case 288:
#line 1587 "grammar.y"
    {
                        (yyval.node) = NEW_HASH((yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 289:
#line 1591 "grammar.y"
    {
                        (yyval.node) = NEW_RETURN(0);
                    ;}
    break;

  case 290:
#line 1595 "grammar.y"
    {
                        (yyval.node) = new_yield(parse_state, (yyvsp[(3) - (4)].node));
                    ;}
    break;

  case 291:
#line 1599 "grammar.y"
    {
                        (yyval.node) = NEW_YIELD(0, Qfalse);
                    ;}
    break;

  case 292:
#line 1603 "grammar.y"
    {
                        (yyval.node) = NEW_YIELD(0, Qfalse);
                    ;}
    break;

  case 293:
#line 1606 "grammar.y"
    {vps->in_defined = 1;;}
    break;

  case 294:
#line 1607 "grammar.y"
    {
                        vps->in_defined = 0;
                        (yyval.node) = NEW_DEFINED((yyvsp[(5) - (6)].node));
                    ;}
    break;

  case 295:
#line 1612 "grammar.y"
    {
                        (yyvsp[(2) - (2)].node)->nd_iter = NEW_FCALL((yyvsp[(1) - (2)].id), 0);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                        fixpos((yyvsp[(2) - (2)].node)->nd_iter, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 297:
#line 1619 "grammar.y"
    {
                        if ((yyvsp[(1) - (2)].node) && nd_type((yyvsp[(1) - (2)].node)) == NODE_BLOCK_PASS) {
                            rb_compile_error("both block arg and actual block given");
                        }
                        (yyvsp[(2) - (2)].node)->nd_iter = (yyvsp[(1) - (2)].node);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                        fixpos((yyval.node), (yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 298:
#line 1631 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(2) - (6)].node), parse_state), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (6)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            NODE *tmp = (yyval.node)->nd_body;
                            (yyval.node)->nd_body = (yyval.node)->nd_else;
                            (yyval.node)->nd_else = tmp;
                        }
                    ;}
    break;

  case 299:
#line 1644 "grammar.y"
    {
                        (yyval.node) = NEW_UNLESS(cond((yyvsp[(2) - (6)].node), parse_state), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (6)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            NODE *tmp = (yyval.node)->nd_body;
                            (yyval.node)->nd_body = (yyval.node)->nd_else;
                            (yyval.node)->nd_else = tmp;
                        }
                    ;}
    break;

  case 300:
#line 1653 "grammar.y"
    {COND_PUSH(1);;}
    break;

  case 301:
#line 1653 "grammar.y"
    {COND_POP();;}
    break;

  case 302:
#line 1656 "grammar.y"
    {
                        (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (7)].node), parse_state), (yyvsp[(6) - (7)].node), 1);
                        fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            nd_set_type((yyval.node), NODE_UNTIL);
                        }
                    ;}
    break;

  case 303:
#line 1663 "grammar.y"
    {COND_PUSH(1);;}
    break;

  case 304:
#line 1663 "grammar.y"
    {COND_POP();;}
    break;

  case 305:
#line 1666 "grammar.y"
    {
                        (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (7)].node), parse_state), (yyvsp[(6) - (7)].node), 1);
                        fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            nd_set_type((yyval.node), NODE_WHILE);
                        }
                    ;}
    break;

  case 306:
#line 1676 "grammar.y"
    {
                        (yyval.node) = NEW_CASE((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (5)].node));
                    ;}
    break;

  case 307:
#line 1681 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(3) - (4)].node);
                    ;}
    break;

  case 308:
#line 1685 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(4) - (5)].node);
                    ;}
    break;

  case 309:
#line 1688 "grammar.y"
    {COND_PUSH(1);;}
    break;

  case 310:
#line 1688 "grammar.y"
    {COND_POP();;}
    break;

  case 311:
#line 1691 "grammar.y"
    {
                        (yyval.node) = NEW_FOR((yyvsp[(2) - (9)].node), (yyvsp[(5) - (9)].node), (yyvsp[(8) - (9)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (9)].node));
                    ;}
    break;

  case 312:
#line 1696 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("class definition in method body");
                        class_nest++;
                        local_push(0);
                        (yyval.num) = ruby_sourceline;
                    ;}
    break;

  case 313:
#line 1705 "grammar.y"
    {
                        (yyval.node) = NEW_CLASS((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(3) - (6)].node));
                        nd_set_line((yyval.node), (yyvsp[(4) - (6)].num));
                        local_pop();
                        class_nest--;
                    ;}
    break;

  case 314:
#line 1712 "grammar.y"
    {
                        (yyval.num) = in_def;
                        in_def = 0;
                    ;}
    break;

  case 315:
#line 1717 "grammar.y"
    {
                        (yyval.num) = in_single;
                        in_single = 0;
                        class_nest++;
                        local_push(0);
                    ;}
    break;

  case 316:
#line 1725 "grammar.y"
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
#line 1734 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("module definition in method body");
                        class_nest++;
                        local_push(0);
                        (yyval.num) = ruby_sourceline;
                    ;}
    break;

  case 318:
#line 1743 "grammar.y"
    {
                        (yyval.node) = NEW_MODULE((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
                        nd_set_line((yyval.node), (yyvsp[(3) - (5)].num));
                        local_pop();
                        class_nest--;
                    ;}
    break;

  case 319:
#line 1750 "grammar.y"
    {
                        (yyval.id) = cur_mid;
                        cur_mid = (yyvsp[(2) - (2)].id);
                        in_def++;
                        local_push(0);
                    ;}
    break;

  case 320:
#line 1759 "grammar.y"
    {
                        if (!(yyvsp[(5) - (6)].node)) (yyvsp[(5) - (6)].node) = NEW_NIL();
                        (yyval.node) = NEW_DEFN((yyvsp[(2) - (6)].id), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node), NOEX_PRIVATE);
                        fixpos((yyval.node), (yyvsp[(4) - (6)].node));
                        local_pop();
                        in_def--;
                        cur_mid = (yyvsp[(3) - (6)].id);
                    ;}
    break;

  case 321:
#line 1767 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 322:
#line 1768 "grammar.y"
    {
                        in_single++;
                        local_push(0);
                        vps->lex_state = EXPR_END; /* force for args */
                    ;}
    break;

  case 323:
#line 1776 "grammar.y"
    {
                        (yyval.node) = NEW_DEFS((yyvsp[(2) - (9)].node), (yyvsp[(5) - (9)].id), (yyvsp[(7) - (9)].node), (yyvsp[(8) - (9)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (9)].node));
                        local_pop();
                        in_single--;
                    ;}
    break;

  case 324:
#line 1783 "grammar.y"
    {
                        (yyval.node) = NEW_BREAK(0);
                    ;}
    break;

  case 325:
#line 1787 "grammar.y"
    {
                        (yyval.node) = NEW_NEXT(0);
                    ;}
    break;

  case 326:
#line 1791 "grammar.y"
    {
                        (yyval.node) = NEW_REDO();
                    ;}
    break;

  case 327:
#line 1795 "grammar.y"
    {
                        (yyval.node) = NEW_RETRY();
                    ;}
    break;

  case 328:
#line 1801 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (1)].node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 337:
#line 1822 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(2) - (5)].node), parse_state), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (5)].node));
                    ;}
    break;

  case 339:
#line 1830 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 343:
#line 1841 "grammar.y"
    {
                        (yyval.node) = (NODE*)1;
                    ;}
    break;

  case 344:
#line 1845 "grammar.y"
    {
                        (yyval.node) = (NODE*)1;
                    ;}
    break;

  case 345:
#line 1849 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 346:
#line 1855 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 347:
#line 1860 "grammar.y"
    {
                      (yyval.vars) = vps->block_vars;
                    ;}
    break;

  case 348:
#line 1865 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 349:
#line 1872 "grammar.y"
    {
                        if ((yyvsp[(1) - (2)].node) && nd_type((yyvsp[(1) - (2)].node)) == NODE_BLOCK_PASS) {
                            rb_compile_error("both block arg and actual block given");
                        }
                        (yyvsp[(2) - (2)].node)->nd_iter = (yyvsp[(1) - (2)].node);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                        fixpos((yyval.node), (yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 350:
#line 1881 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 351:
#line 1885 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 352:
#line 1891 "grammar.y"
    {
                        (yyval.node) = new_fcall(parse_state, (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 353:
#line 1896 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 354:
#line 1901 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 355:
#line 1906 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), 0);
                    ;}
    break;

  case 356:
#line 1910 "grammar.y"
    {
                        (yyval.node) = NEW_CALL((yyvsp[(1) - (3)].node), rb_intern("get_reference"), NEW_LIST(NEW_LIT(ID2SYM((yyvsp[(3) - (3)].id)))));
                    ;}
    break;

  case 357:
#line 1914 "grammar.y"
    {
                        (yyval.node) = NEW_FCALL(rb_intern("get_reference"), NEW_LIST(NEW_LIT(ID2SYM((yyvsp[(2) - (2)].id)))));
                    ;}
    break;

  case 358:
#line 1918 "grammar.y"
    {
                        (yyval.node) = new_super(parse_state, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 359:
#line 1922 "grammar.y"
    {
                        (yyval.node) = NEW_ZSUPER();
                    ;}
    break;

  case 360:
#line 1928 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 361:
#line 1932 "grammar.y"
    { (yyval.vars) = vps->block_vars; ;}
    break;

  case 362:
#line 1934 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 363:
#line 1939 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 364:
#line 1943 "grammar.y"
    { (yyval.vars) = vps->block_vars; ;}
    break;

  case 365:
#line 1945 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 366:
#line 1954 "grammar.y"
    {
                        (yyval.node) = NEW_WHEN((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 368:
#line 1960 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, (yyvsp[(1) - (4)].node), NEW_WHEN((yyvsp[(4) - (4)].node), 0, 0));
                    ;}
    break;

  case 369:
#line 1964 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_WHEN((yyvsp[(2) - (2)].node), 0, 0));
                    ;}
    break;

  case 372:
#line 1976 "grammar.y"
    {
                        if ((yyvsp[(3) - (6)].node)) {
                            (yyvsp[(3) - (6)].node) = node_assign((yyvsp[(3) - (6)].node), NEW_GVAR(rb_intern("$!")), parse_state);
                            (yyvsp[(5) - (6)].node) = block_append(parse_state, (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node));
                        }
                        (yyval.node) = NEW_RESBODY((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (6)].node)?(yyvsp[(2) - (6)].node):(yyvsp[(5) - (6)].node));
                    ;}
    break;

  case 374:
#line 1988 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 377:
#line 1996 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 379:
#line 2003 "grammar.y"
    {
                        if ((yyvsp[(2) - (2)].node))
                            (yyval.node) = (yyvsp[(2) - (2)].node);
                        else
                            /* place holder */
                            (yyval.node) = NEW_NIL();
                    ;}
    break;

  case 382:
#line 2015 "grammar.y"
    {
                        (yyval.node) = NEW_LIT(ID2SYM((yyvsp[(1) - (1)].id)));
                    ;}
    break;

  case 384:
#line 2022 "grammar.y"
    {
                        NODE *node = (yyvsp[(1) - (1)].node);
                        if (!node) {
                            node = NEW_STR(string_new(0, 0));
                        }
                        else {
                            node = evstr2dstr(parse_state, node);
                        }
                        (yyval.node) = node;
                    ;}
    break;

  case 386:
#line 2036 "grammar.y"
    {
                        (yyval.node) = literal_concat(parse_state, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 387:
#line 2042 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 388:
#line 2048 "grammar.y"
    {
                        ID code = (yyvsp[(1) - (3)].id);
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
                        if(code) {
                            node->u2.id = code;
                        } else {
                            node->u2.id = 0;
                        }
                        (yyval.node) = node;
                    ;}
    break;

  case 389:
#line 2077 "grammar.y"
    {
                        int options = (yyvsp[(3) - (3)].num);
                        NODE *node = (yyvsp[(2) - (3)].node);
                        if (!node) {
                            node = NEW_REGEX(string_new2(""), options & ~RE_OPTION_ONCE);
                        }
                        else switch (nd_type(node)) {
                          case NODE_STR:
                            {
                                nd_set_type(node, NODE_REGEX);
                                node->nd_cnt = options & ~RE_OPTION_ONCE;
                                /*
                                node->nd_lit = rb_reg_new(RSTRING(src)->ptr,
                                                          RSTRING(src)->len,
                                                          options & ~RE_OPTION_ONCE);
                                */
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

  case 390:
#line 2112 "grammar.y"
    {
                        (yyval.node) = NEW_ZARRAY();
                    ;}
    break;

  case 391:
#line 2116 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 392:
#line 2122 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 393:
#line 2126 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, (yyvsp[(1) - (3)].node), evstr2dstr(parse_state, (yyvsp[(2) - (3)].node)));
                    ;}
    break;

  case 395:
#line 2133 "grammar.y"
    {
                        (yyval.node) = literal_concat(parse_state, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 396:
#line 2139 "grammar.y"
    {
                        (yyval.node) = NEW_ZARRAY();
                    ;}
    break;

  case 397:
#line 2143 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 398:
#line 2149 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 399:
#line 2153 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, (yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 400:
#line 2159 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 401:
#line 2163 "grammar.y"
    {
                        (yyval.node) = literal_concat(parse_state, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 402:
#line 2169 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 403:
#line 2173 "grammar.y"
    {
                        (yyval.node) = literal_concat(parse_state, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 405:
#line 2180 "grammar.y"
    {
                        (yyval.node) = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                    ;}
    break;

  case 406:
#line 2186 "grammar.y"
    {
                        lex_strterm = (yyvsp[(2) - (3)].node);
                        (yyval.node) = NEW_EVSTR((yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 407:
#line 2191 "grammar.y"
    {
                        (yyval.node) = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                        COND_PUSH(0);
                        CMDARG_PUSH(0);
                    ;}
    break;

  case 408:
#line 2199 "grammar.y"
    {
                        lex_strterm = (yyvsp[(2) - (4)].node);
                        COND_LEXPOP();
                        CMDARG_LEXPOP();
                        if (((yyval.node) = (yyvsp[(3) - (4)].node)) && nd_type((yyval.node)) == NODE_NEWLINE) {
                            (yyval.node) = (yyval.node)->nd_next;
                            // rb_gc_force_recycle((VALUE)$3);
                        }
                        (yyval.node) = new_evstr(parse_state, (yyval.node));
                    ;}
    break;

  case 409:
#line 2211 "grammar.y"
    {(yyval.node) = NEW_GVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 410:
#line 2212 "grammar.y"
    {(yyval.node) = NEW_IVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 411:
#line 2213 "grammar.y"
    {(yyval.node) = NEW_CVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 413:
#line 2218 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = (yyvsp[(2) - (2)].id);
                    ;}
    break;

  case 418:
#line 2231 "grammar.y"
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
                                if (strlen(bdatae((yyval.node)->nd_str,"")) == blength((yyval.node)->nd_str)) {
                                  ID tmp = rb_intern(bdata((yyval.node)->nd_str));
                                  bdestroy((yyval.node)->nd_str);
                                  (yyval.node)->nd_lit = tmp;
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

  case 421:
#line 2264 "grammar.y"
    {
                        (yyval.node) = NEW_NEGATE((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 422:
#line 2268 "grammar.y"
    {
                        (yyval.node) = NEW_NEGATE((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 428:
#line 2278 "grammar.y"
    {(yyval.id) = kNIL;;}
    break;

  case 429:
#line 2279 "grammar.y"
    {(yyval.id) = kSELF;;}
    break;

  case 430:
#line 2280 "grammar.y"
    {(yyval.id) = kTRUE;;}
    break;

  case 431:
#line 2281 "grammar.y"
    {(yyval.id) = kFALSE;;}
    break;

  case 432:
#line 2282 "grammar.y"
    {(yyval.id) = k__FILE__;;}
    break;

  case 433:
#line 2283 "grammar.y"
    {(yyval.id) = k__LINE__;;}
    break;

  case 434:
#line 2287 "grammar.y"
    {
                        (yyval.node) = gettable((yyvsp[(1) - (1)].id));
                    ;}
    break;

  case 435:
#line 2293 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, parse_state);
                    ;}
    break;

  case 438:
#line 2303 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 439:
#line 2307 "grammar.y"
    {
                        vps->lex_state = EXPR_BEG;
                    ;}
    break;

  case 440:
#line 2311 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(3) - (4)].node);
                    ;}
    break;

  case 441:
#line 2314 "grammar.y"
    {yyerrok; (yyval.node) = 0;;}
    break;

  case 442:
#line 2318 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                        vps->lex_state = EXPR_BEG;
                    ;}
    break;

  case 443:
#line 2323 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 444:
#line 2329 "grammar.y"
    {
                        // printf("rest + all = %d\n", $5);
                        (yyval.node) = block_append(parse_state, NEW_ARGS((yyvsp[(1) - (6)].num), (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].id)), (yyvsp[(6) - (6)].node));
                    ;}
    break;

  case 445:
#line 2334 "grammar.y"
    {
                        (yyval.node) = block_append(parse_state, NEW_ARGS((yyvsp[(1) - (4)].num), (yyvsp[(3) - (4)].node), -1), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 446:
#line 2338 "grammar.y"
    {
                        // printf("arg + rest = %d\n", $3);
                        (yyval.node) = block_append(parse_state, NEW_ARGS((yyvsp[(1) - (4)].num), 0, (yyvsp[(3) - (4)].id)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 447:
#line 2343 "grammar.y"
    {
                        (yyval.node) = block_append(parse_state, NEW_ARGS((yyvsp[(1) - (2)].num), 0, -1), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 448:
#line 2347 "grammar.y"
    {
                        // printf("rest + opt = %d\n", $3);
                        (yyval.node) = block_append(parse_state, NEW_ARGS(0, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 449:
#line 2352 "grammar.y"
    {
                        (yyval.node) = block_append(parse_state, NEW_ARGS(0, (yyvsp[(1) - (2)].node), -1), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 450:
#line 2356 "grammar.y"
    {
                        // printf("rest only = %d\n", $1);
                        (yyval.node) = block_append(parse_state, NEW_ARGS(0, 0, (yyvsp[(1) - (2)].id)), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 451:
#line 2361 "grammar.y"
    {
                        (yyval.node) = block_append(parse_state, NEW_ARGS(0, 0, -1), (yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 452:
#line 2365 "grammar.y"
    {
                        (yyval.node) = NEW_ARGS(0, 0, -1);
                    ;}
    break;

  case 453:
#line 2371 "grammar.y"
    {
                        yyerror("formal argument cannot be a constant");
                    ;}
    break;

  case 454:
#line 2375 "grammar.y"
    {
                        yyerror("formal argument cannot be an instance variable");
                    ;}
    break;

  case 455:
#line 2379 "grammar.y"
    {
                        yyerror("formal argument cannot be a global variable");
                    ;}
    break;

  case 456:
#line 2383 "grammar.y"
    {
                        yyerror("formal argument cannot be a class variable");
                    ;}
    break;

  case 457:
#line 2387 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(1) - (1)].id)))
                            yyerror("formal argument must be local variable");
                        else if (local_id((yyvsp[(1) - (1)].id)))
                            yyerror("duplicate argument name");
                        local_cnt((yyvsp[(1) - (1)].id));
                        (yyval.num) = 1;
                    ;}
    break;

  case 459:
#line 2399 "grammar.y"
    {
                        (yyval.num) += 1;
                    ;}
    break;

  case 460:
#line 2405 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(1) - (3)].id)))
                            yyerror("formal argument must be local variable");
                        else if (local_id((yyvsp[(1) - (3)].id)))
                            yyerror("duplicate optional argument name");
                        (yyval.node) = assignable((yyvsp[(1) - (3)].id), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 461:
#line 2415 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK((yyvsp[(1) - (1)].node));
                        (yyval.node)->nd_end = (yyval.node);
                    ;}
    break;

  case 462:
#line 2420 "grammar.y"
    {
                        (yyval.node) = block_append(parse_state, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 465:
#line 2430 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(2) - (2)].id)))
                            yyerror("rest argument must be local variable");
                        else if (local_id((yyvsp[(2) - (2)].id)))
                            yyerror("duplicate rest argument name");
                        (yyval.id) = local_cnt((yyvsp[(2) - (2)].id)) + 1;
                    ;}
    break;

  case 466:
#line 2438 "grammar.y"
    {
                        //$$ = -2;
                        (yyval.id) = 0;
                    ;}
    break;

  case 469:
#line 2449 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(2) - (2)].id)))
                            yyerror("block argument must be local variable");
                        else if (local_id((yyvsp[(2) - (2)].id)))
                            yyerror("duplicate block argument name");
                        (yyval.node) = NEW_BLOCK_ARG((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 470:
#line 2459 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 472:
#line 2466 "grammar.y"
    {
                        if (nd_type((yyvsp[(1) - (1)].node)) == NODE_SELF) {
                            (yyval.node) = NEW_SELF();
                        }
                        else {
                            (yyval.node) = (yyvsp[(1) - (1)].node);
                            value_expr((yyval.node));
                        }
                    ;}
    break;

  case 473:
#line 2475 "grammar.y"
    {vps->lex_state = EXPR_BEG;;}
    break;

  case 474:
#line 2476 "grammar.y"
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

  case 476:
#line 2502 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 477:
#line 2506 "grammar.y"
    {
                        if ((yyvsp[(1) - (2)].node)->nd_alen%2 != 0) {
                            yyerror("odd number list for Hash");
                        }
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 479:
#line 2516 "grammar.y"
    {
                        (yyval.node) = list_concat((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 480:
#line 2522 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, NEW_LIST((yyvsp[(1) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 500:
#line 2560 "grammar.y"
    {yyerrok;;}
    break;

  case 503:
#line 2565 "grammar.y"
    {yyerrok;;}
    break;

  case 504:
#line 2568 "grammar.y"
    {(yyval.node) = 0;;}
    break;


/* Line 1267 of yacc.c.  */
#line 7413 "grammar.c"
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


#line 2570 "grammar.y"


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

/*
static VALUE (*lex_gets)();
static VALUE lex_input; 
static VALUE lex_lastline;
static char *lex_pbeg;
static char *lex_p;
static char *lex_pend;
*/


static int
syd_yyerror(msg, parse_state)
    const char *msg;
    rb_parse_state *parse_state;
{
    // char *p, *pe, *buf;
    // int len, i, col;
    
    create_error(parse_state, (char *)msg);
    
    // printf("FIXME ERROR: %s, %d ('%s', %d)\n", msg, parse_state->lex_state, parse_state->lex_lastline->str, col);
    /*
    
    rb_compile_error("%s", msg);
    p = parse_state->lex_p;
    while (parse_state->lex_pbeg <= p) {
        if (*p == '\n') break;
        p--;
    }
    p++;

    pe = parse_state->lex_p;
    while (pe < parse_state->lex_pend) {
        if (*pe == '\n') break;
        pe++;
    }

    len = pe - p;
    if (len > 4) {
        buf = ALLOCA_N(char, len+2);
        MEMCPY(buf, p, char, len);
        buf[len] = '\0';
        rb_compile_error_append("%s", buf);

        i = parse_state->lex_p - p;
        p = buf; pe = p + len;

        while (p < pe) {
            if (*p != '\t') *p = ' ';
            p++;
        }
        buf[i] = '^';
        buf[i+1] = '\0';
        rb_compile_error_append("%s", buf);
    }
    */

    return 1;
}

/*
static int heredoc_end;
static int command_start = Qtrue;
*/
/* :int:
int ruby_in_compile = 0;
*/

static int
yycompile(parse_state, f, line)
    rb_parse_state *parse_state;
    char *f;
    int line;
{
    int n;
    // struct RVarmap *vp, *vars = ruby_dyna_vars;

    // ruby_in_compile = 1;
    /*
    if (!compile_for_eval && rb_safe_level() == 0 &&
        rb_const_defined(rb_cObject, rb_intern("SCRIPT_LINES__"))) {
        VALUE hash, fname;

        hash = rb_const_get(rb_cObject, rb_intern("SCRIPT_LINES__"));
        if (TYPE(hash) == T_HASH) {
            fname = string_new2(f);
            ruby_debug_lines = rb_hash_aref(hash, fname);
            if (NIL_P(ruby_debug_lines)) {
                ruby_debug_lines = rb_ary_new();
                rb_hash_aset(hash, fname, ruby_debug_lines);
            }
        }
        if (line > 1) {
            VALUE str = string_new(0,0);
            while (line > 1) {
                rb_ary_push(ruby_debug_lines, str);
                line--;
            }
        }
    }
    */
    
    /* Setup an initial empty scope. */
    /*
    syd_scope = (struct SCOPE*)rb_newobj();
    OBJSETUP(syd_scope, 0, T_SCOPE);
    syd_scope->local_tbl = 0;
    syd_scope->local_vars = 0;
    syd_scope->flags = 0;
*/
    // ruby__end__seen = 0;
    // ruby_eval_tree = 0;
    heredoc_end = 0;
    lex_strterm = 0;
    // ruby_current_node = 0;
    ruby_sourcefile = f;
    n = yyparse(parse_state);
    ruby_debug_lines = 0;
    compile_for_eval = 0;
    // ruby_in_compile = 0;
    parse_state->cond_stack = 0;
    parse_state->cmdarg_stack = 0;
    command_start = 1;
    class_nest = 0;
    in_single = 0;
    in_def = 0;
    cur_mid = 0;

    lex_strterm = 0;

    /*
    vp = ruby_dyna_vars;
    ruby_dyna_vars = vars;
    lex_strterm = 0;
    while (vp && vp != vars) {
        struct RVarmap *tmp = vp;
        vp = vp->next;
        rb_gc_force_recycle((VALUE)tmp);
    }
    */
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

    return true;
}

void syd_add_to_parse_tree(STATE, OBJECT ary,
      NODE * n, int newlines, ID * locals, int line_numbers);

static OBJECT convert_to_sexp(STATE, NODE *node, int newlines) {
  OBJECT ary;
  ary = array_new(state, 1);
  syd_add_to_parse_tree(state, ary, node, newlines, NULL, FALSE);
  return array_get(state, ary, 0);
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

OBJECT
syd_compile_string(STATE, const char *f, bstring s, int line, int newlines)
{
    int n;
    rb_parse_state *parse_state;
    OBJECT ret;
    parse_state = alloc_parse_state();
    parse_state->state = state;
    parse_state->lex_string = s;
    parse_state->lex_gets = lex_get_str;
    parse_state->lex_pbeg = 0;
    parse_state->lex_p = 0;
    parse_state->lex_pend = 0;
    ruby_sourceline = line - 1;
    compile_for_eval = 1;
    
    n = yycompile(parse_state, f, line);
    // ruby_eval_tree = parse_state->top;
    // if (n != 0) ruby_eval_tree_begin = 0;
    
    if(!n) {
        ret = convert_to_sexp(state, parse_state->top, newlines);
    } else {
        ret = parse_state->error;
    }
    pt_free(parse_state);
    free(parse_state);
    return ret;
}

static bool parse_io_gets(rb_parse_state *parse_state) {
  if(feof(parse_state->lex_io)) {
    return false;
  }

  while(true) {
    char *ptr, buf[128];
    int read;

    ptr = fgets(buf, sizeof(buf), parse_state->lex_io);
    if(!ptr) {
      return false;
    }

    read = strlen(ptr);
    bcatblk(parse_state->line_buffer, ptr, read);

    /* check whether we read a full line */
    if(!(read == (sizeof(buf) - 1) && ptr[read] != '\r')) {
      break;
    }
  }

  return true;
}

OBJECT
syd_compile_file(STATE, const char *f, FILE *file, int start, int newlines)
{
    int n;
    OBJECT ret;
    rb_parse_state *parse_state;
    parse_state = alloc_parse_state();
    parse_state->state = state;
    parse_state->lex_io = file;
    parse_state->lex_gets = parse_io_gets;
    parse_state->lex_pbeg = 0;
    parse_state->lex_p = 0;
    parse_state->lex_pend = 0;
    parse_state->error = Qfalse;
    ruby_sourceline = start - 1;

    n = yycompile(parse_state, f, start);
    
    if(!n) {
        // ruby_eval_tree = parse_state->top;
        ret = convert_to_sexp(state, parse_state->top, newlines);
    } else {
        // ruby_eval_tree_begin = 0;
        ret = parse_state->error;
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
        parse_state->lex_pbeg = parse_state->lex_p = bdata(v);
        parse_state->lex_pend = parse_state->lex_p + blength(v);

        if(parse_state->lex_lastline) {
          bassign(parse_state->lex_lastline, v);
        } else {
          parse_state->lex_lastline = bstrcpy(v);
        }
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
pushback(c, parse_state)
    int c;
    rb_parse_state *parse_state;
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
tokadd_escape(term, parse_state)
    int term;
    rb_parse_state *parse_state;
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
        rb_compile_error("unknown regexp option%s - %s",
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

static int tokadd_string(int func, int term, int paren, int *nest, rb_parse_state *parse_state)
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
            rb_compile_error("symbol cannot contain '\\0'");
            continue;
        }
        tokadd((char)c, parse_state);
    }
    return c;
}

#define NEW_STRTERM(func, term, paren) \
        syd_node_newnode(parse_state, NODE_STRTERM, (func), (term) | ((paren) << (CHAR_BIT * 2)), 0)
#define pslval ((YYSTYPE *)parse_state->lval)
static int
parse_string(quote, parse_state)
    NODE *quote;
    rb_parse_state *parse_state;
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
    if (tokadd_string(func, term, paren, (int *)&quote->nd_nest, parse_state) == -1) {
        ruby_sourceline = nd_line(quote);
        rb_compile_error("unterminated string meets end of file");
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
    int c = nextc(), term, func = 0, len;

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
            do { tokadd((char)c, parse_state); } while (--len > 0 && (c = nextc()) != -1);
        }
        /* Ack! end of file or end of string. */
        if (c == -1) {
            rb_compile_error("unterminated here document identifier");
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
    lex_strterm = syd_node_newnode(parse_state, NODE_HEREDOC,
                                  (OBJECT)string_new(tok(), toklen()),  /* nd_lit */
                                  len,                          /* nd_nth */
                                  (OBJECT)str);    /* nd_orig */
    return term == '`' ? tXSTRING_BEG : tSTRING_BEG;
}

static void
heredoc_restore(here, parse_state)
    NODE *here;
    rb_parse_state *parse_state;
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
//    rb_gc_force_recycle((VALUE)here);
}

static int
whole_match_p(eos, len, indent, parse_state)
    char *eos;
    int len, indent;
    rb_parse_state *parse_state;
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
here_document(here, parse_state)
    NODE *here;
    rb_parse_state *parse_state;
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
        rb_compile_error("can't find string \"%s\" anywhere before EOF", eos);
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
            if (str)
                bcatblk(str, p, pend - p);
            else
                str = blk2bstr(p, pend - p);
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

#include "grammar_lex.c.tab"

static void
arg_ambiguous()
{
    rb_warning("ambiguous first argument; put parentheses or even spaces");
}

#define IS_ARG() (parse_state->lex_state == EXPR_ARG || parse_state->lex_state == EXPR_CMDARG)

static int
yylex(YYSTYPE *yylval, void *vstate)
{
    register int c;
    int space_seen = 0;
    int cmd_state, comment_column;
    struct rb_parse_state *parse_state;
    bstring cur_line;
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
                // rb_gc_force_recycle((VALUE)lex_strterm);
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
        // printf("hit comment.. %x\n", parse_state->comments);
        if(parse_state->comments) {
            comment_column = parse_state->column;
            cur_line = bfromcstralloc(50, "");
            
            while((c = nextc()) != '\n' && c != -1) {
              bconchar(cur_line, c);
            }
            
            // FIXME: used to have the file and column too, but took it out.
            g_ptr_array_add(parse_state->comments, cur_line);
                // rb_ary_new3(3, INT2NUM(ruby_sourceline), INT2NUM(comment_column),
                //    cur_line));
            
            if(c == -1) {
                return 0;
            }
        } else {
            while ((c = nextc()) != '\n') {
                if (c == -1)
                    return 0;
            }
        }
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
                        rb_compile_error("embedded document meets end of file");
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
            rb_compile_error("incomplete character syntax");
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
        pslval->node = NEW_FIXNUM(c);
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
                sprintf(tmp, "trailing `%c' in number", nondigit);
                yyerror(tmp);
            }
            if (is_float) {
                /* Some implementations of strtod() don't guarantee to
                 * set errno, so we need to reset it ourselves.
                 */
                errno = 0;

                strtod(tok(), 0);
                if (errno == ERANGE) {
                    rb_warn("Float %s out of range", tok());
                    errno = 0;
                }
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
            lex_strterm = NEW_STRTERM(str_ssym, c, 0);
            break;
          case '"':
            lex_strterm = NEW_STRTERM(str_dsym, c, 0);
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
            int term;
            int paren;
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
                rb_compile_error("unterminated quoted string meets end of file");
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
                pslval->id = rb_intern(tmpstr);
                return tXSTRING_BEG;

              default:
                lex_strterm = NEW_STRTERM(str_xquote, term, paren);
                tmpstr[0] = c;
                tmpstr[1] = 0;
                pslval->id = rb_intern(tmpstr);
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
            pslval->id = rb_intern(tok());
            return tGVAR;

          case '-':
            tokadd('$', parse_state);
            tokadd((char)c, parse_state);
            c = nextc();
            tokadd((char)c, parse_state);
            tokfix();
            pslval->id = rb_intern(tok());
            /* xxx shouldn't check if valid option variable */
            return tGVAR;

          case '&':             /* $&: last match */
          case '`':             /* $`: string before last match */
          case '\'':            /* $': string after last match */
          case '+':             /* $+: string matches last paren. */
            pslval->node = NEW_BACK_REF(c);
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
            tokfix();
            pslval->node = NEW_NTH_REF(atoi(tok()+1));
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
                rb_compile_error("`@%c' is not allowed as an instance variable name", c);
            }
            else {
                rb_compile_error("`@@%c' is not allowed as a class variable name", c);
            }
        }
        if (!is_identchar(c)) {
            pushback(c, parse_state);
            return '@';
        }
        break;

      case '_':
        if (was_bol() && whole_match_p("__END__", 7, 0, parse_state)) {
            // ruby__end__seen = 1;
            parse_state->lex_lastline = 0;
            return -1;
        }
        newtok(parse_state);
        break;

      default:
        if (!is_identchar(c)) {
            rb_compile_error("Invalid char `\\%03o' in expression", c);
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
                kw = syd_reserved_word(tok(), toklen());
                if (kw) {
                    enum lex_state state = parse_state->lex_state;
                    parse_state->lex_state = kw->state;
                    if (state == EXPR_FNAME) {
                        pslval->id = rb_intern(kw->name);
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
        pslval->id = rb_intern(tok());
        /*
        
        FIXME: NO clue what this does.
        
        if (is_local_id(pslval->id) &&
            ((syd_dyna_in_block(parse_state) && 
                      rb_dvar_defined(pslval->id)) || local_id(pslval->id))) {
            parse_state->lex_state = EXPR_END;
        }
        */
        return result;
    }
}


static NODE*
syd_node_newnode(rb_parse_state *st, enum node_type type,
                 OBJECT a0, OBJECT a1, OBJECT a2)
{
    NODE *n = (NODE*)pt_allocate(st, sizeof(NODE));
    // NODE *n = (NODE*)rb_newobj();

    // n->flags |= T_NODE;
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
newline_node(parse_state, node)
    rb_parse_state *parse_state;
    NODE *node;
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
fixpos(node, orig)
    NODE *node, *orig;
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
      printf("%s:%i: warning: %s\n", ruby_sourcefile, ruby_sourceline, mesg);
      ruby_sourceline = line;
    }
}

static NODE*
block_append(parse_state, head, tail)
    rb_parse_state *parse_state;
    NODE *head, *tail;
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
        bdestroy(h->nd_str);
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

    if (RTEST(ruby_verbose)) {
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
list_append(parse_state, list, item)
    rb_parse_state *parse_state;
    NODE *list, *item;
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
list_concat(head, tail)
    NODE *head, *tail;
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
literal_concat(parse_state, head, tail)
    rb_parse_state *parse_state;
    NODE *head, *tail;
{
    enum node_type htype;

    if (!head) return tail;
    if (!tail) return head;

    htype = nd_type(head);
    if (htype == NODE_EVSTR) {
        NODE *node = NEW_DSTR(string_new(0, 0));
        head = list_append(parse_state, node, head);
    }
    switch (nd_type(tail)) {
      case NODE_STR:
        if (htype == NODE_STR) {
            bconcat(head->nd_str, tail->nd_str);
			bdestroy(tail->nd_str);
            // rb_gc_force_recycle((VALUE)tail);
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
            // rb_gc_force_recycle((VALUE)head);
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
evstr2dstr(parse_state, node)
    rb_parse_state *parse_state;
    NODE *node;
{
    if (nd_type(node) == NODE_EVSTR) {
        node = list_append(parse_state, NEW_DSTR(string_new(0, 0)), node);
    }
    return node;
}

static NODE *
new_evstr(parse_state, node)
    rb_parse_state *parse_state;
    NODE *node;
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
    ID token;
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

static ID convert_op(ID id) {
    int i;
    for(i = 0; op_tbl[i].token; i++) {
        if(op_tbl[i].token == id) {
            return rb_intern(op_tbl[i].name);
        }
    }
    return id;
}

static NODE *
call_op(recv, id, narg, arg1, parse_state)
    NODE *recv;
    ID id;
    int narg;
    NODE *arg1;
    rb_parse_state *parse_state;
{
    // int i;
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
match_gen(node1, node2, parse_state)
    NODE *node1;
    NODE *node2;
    rb_parse_state *parse_state;
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
syd_gettable(parse_state, id)
    rb_parse_state *parse_state;
    ID id;
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
        return NEW_STR(string_new2(ruby_sourcefile));
    }
    else if (id == k__LINE__) {
        return NEW_FIXNUM(ruby_sourceline);
    }
    else if (is_local_id(id)) {
        if (local_id(id)) return NEW_LVAR(id);
        /* method call without arguments */
#if 0
        /* Rite will warn this */
        rb_warn("ambiguous identifier; %s() or self.%s is better for method call",
                rb_id2name(id), rb_id2name(id));
#endif
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
    rb_compile_error("identifier is not valid 1\n");
    return 0;
}

static void
reset_block(rb_parse_state *parse_state) {
  if(!parse_state->block_vars) {
    parse_state->block_vars = var_table_create();
  } else {
    parse_state->block_vars = var_table_push(parse_state->block_vars);
  }
}

static NODE *
extract_block_vars(rb_parse_state *parse_state, NODE* node, var_table *vars)
{
    int i;
    // struct RVarmap *post = ruby_dyna_vars;
    NODE *var, *out = node;
        
    if (!node) goto out;
    if(vars->size == 0) goto out;
        
    var = NULL;
    // if (!node || !post || pre == post) return node;
    for(i = 0; i < vars->size; i++) {
        var = NEW_DASGN_CURR(vars->data[i], var);
    }
    /*
    for (var = 0; post != pre && post->id; post = post->next) {
        var = NEW_DASGN_CURR(post->id, var);
    }
    */
    out = block_append(parse_state, var, node);

out:
  assert(vars == parse_state->block_vars);
  parse_state->block_vars = var_table_pop(parse_state->block_vars);

  return out;
}

static NODE*
assignable(id, val, parse_state)
    ID id;
    NODE *val;
    rb_parse_state *parse_state;
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
        if(parse_state->block_vars) {
          var_table_add(parse_state->block_vars, id);
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
        rb_compile_error("identifier is not valid 2 (%d)\n", id);
    }
    return 0;
}

static NODE *
aryset(recv, idx, parse_state)
    NODE *recv, *idx;
    rb_parse_state *parse_state;
{
    if (recv && nd_type(recv) == NODE_SELF)
        recv = (NODE *)1;
    else
        value_expr(recv);
    return NEW_ATTRASGN(recv, convert_op(tASET), idx);
}


static ID
rb_id_attrset(id)
    ID id;
{
    id &= ~ID_SCOPE_MASK;
    id |= ID_ATTRSET;
    return id;
}

static NODE *
attrset(recv, id, parse_state)
    NODE *recv;
    ID id;
    rb_parse_state *parse_state;
{
    if (recv && nd_type(recv) == NODE_SELF)
        recv = (NODE *)1;
    else
        value_expr(recv);
    return NEW_ATTRASGN(recv, rb_id_attrset(id), 0);
}

static void
rb_backref_error(node)
    NODE *node;
{
    switch (nd_type(node)) {
      case NODE_NTH_REF:
        rb_compile_error("Can't set variable $%u", node->nd_nth);
        break;
      case NODE_BACK_REF:
        rb_compile_error("Can't set variable $%c", (int)node->nd_nth);
        break;
    }
}

static NODE *
arg_concat(parse_state, node1, node2)
    rb_parse_state *parse_state;
    NODE *node1;
    NODE *node2;
{
    if (!node2) return node1;
    return NEW_ARGSCAT(node1, node2);
}

static NODE *
arg_add(parse_state, node1, node2)
    rb_parse_state *parse_state;
    NODE *node1;
    NODE *node2;
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
node_assign(lhs, rhs, parse_state)
    NODE *lhs, *rhs;
    rb_parse_state *parse_state;
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
value_expr0(node, parse_state)
    NODE *node;
    rb_parse_state *parse_state;
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
void_expr0(node)
    NODE *node;
{
  const char *useless = NULL;

    if (!RTEST(ruby_verbose)) return;

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
            useless = ""; // rb_id2name(node->nd_mid);
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
void_stmts(node, parse_state)
    NODE *node;
    rb_parse_state *parse_state;
{
    if (!RTEST(ruby_verbose)) return;
    if (!node) return;
    if (nd_type(node) != NODE_BLOCK) return;

    for (;;) {
        if (!node->nd_next) return;
        void_expr(node->nd_head);
        node = node->nd_next;
    }
}

static NODE *
remove_begin(node)
    NODE *node;
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
assign_in_cond(node, parse_state)
    NODE *node;
    rb_parse_state *parse_state;
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
#if 0
    if (assign_in_cond(node->nd_value) == 0) {
        parser_warning(parse_state, node->nd_value, "assignment in condition");
    }
#endif
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
warn_unless_e_option(ps, node, str)
    rb_parse_state *ps;
    NODE *node;
    const char *str;
{
    if (!e_option_supplied()) parser_warning(ps, node, str);
}

static NODE *cond0();

static NODE*
range_op(node, parse_state)
    NODE *node;
    rb_parse_state *parse_state;
{
    enum node_type type;

    if (!e_option_supplied()) return node;
    if (node == 0) return 0;

    value_expr(node);
    node = cond0(node, parse_state);
    type = nd_type(node);
    if (type == NODE_NEWLINE) {
        node = node->nd_next;
        type = nd_type(node);
    }
    if (type == NODE_LIT && FIXNUM_P(node->nd_lit)) {
        warn_unless_e_option(parse_state, node, "integer literal in conditional range");
        return call_op(node,tEQ,1,NEW_GVAR(rb_intern("$.")), parse_state);
    }
    return node;
}

static int
literal_node(node)
    NODE *node;
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
cond0(node, parse_state)
    NODE *node;
    rb_parse_state *parse_state;
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
        return NEW_MATCH2(node, NEW_GVAR(rb_intern("$_")));

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
        node->nd_cnt = local_append(internal_id());
        if (!e_option_supplied()) {
            int b = literal_node(node->nd_beg);
            int e = literal_node(node->nd_end);
            if ((b == 1 && e == 1) || (b + e >= 2 && RTEST(ruby_verbose))) {
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
cond(node, parse_state)
    NODE *node;
    rb_parse_state *parse_state;
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
logop(type, left, right, parse_state)
    enum node_type type;
    NODE *left, *right;
    rb_parse_state *parse_state;
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
cond_negative(nodep)
    NODE **nodep;
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
no_blockarg(node)
    NODE *node;
{
    if (node && nd_type(node) == NODE_BLOCK_PASS) {
        rb_compile_error("block argument should not be given");
    }
}

static NODE *
ret_args(parse_state, node)
    rb_parse_state *parse_state;
    NODE *node;
{
    if (node) {
        no_blockarg(node);
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
new_yield(parse_state, node)
    rb_parse_state *parse_state;
    NODE *node;
{
    long state = Qtrue;

    if (node) {
        no_blockarg(node);
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

/*
static NODE*
negate_lit(node)
    NODE *node;
{
    switch (TYPE(node->nd_lit)) {
      case T_FIXNUM:
        node->nd_lit = LONG2FIX(-FIX2LONG(node->nd_lit));
        break;
      case T_BIGNUM:
        node->nd_lit = rb_funcall(node->nd_lit,rb_intern("-@"),0,0);
        break;
      case T_FLOAT:
        RFLOAT(node->nd_lit)->value = -RFLOAT(node->nd_lit)->value;
        break;
      default:
        break;
    }
    return node;
}
*/

static NODE *
arg_blk_pass(node1, node2)
    NODE *node1;
    NODE *node2;
{
    if (node2) {
        node2->nd_head = node1;
        return node2;
    }
    return node1;
}

static NODE*
arg_prepend(parse_state, node1, node2)
    rb_parse_state *parse_state;
    NODE *node1, *node2;
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
new_call(parse_state, r,m,a)
    rb_parse_state *parse_state;
    NODE *r;
    ID m;
    NODE *a;
{
    if (a && nd_type(a) == NODE_BLOCK_PASS) {
        a->nd_iter = NEW_CALL(r,convert_op(m),a->nd_head);
        return a;
    }
    return NEW_CALL(r,convert_op(m),a);
}

static NODE*
new_fcall(parse_state, m,a)
    rb_parse_state *parse_state;
    ID m;
    NODE *a;
{
    if (a && nd_type(a) == NODE_BLOCK_PASS) {
        a->nd_iter = NEW_FCALL(m,a->nd_head);
        return a;
    }
    return NEW_FCALL(m,a);
}

static NODE*
new_super(parse_state,a)
    rb_parse_state *parse_state;
    NODE *a;
{
    if (a && nd_type(a) == NODE_BLOCK_PASS) {
        a->nd_iter = NEW_SUPER(a->nd_head);
        return a;
    }
    return NEW_SUPER(a);
}


static void
syd_local_push(rb_parse_state *st, int top)
{
    st->variables = var_table_push(st->variables);
    //rb_funcall(st->self, rb_intern("local_push"), 1, top ? Qtrue : Qfalse);
    /*
    struct local_vars *local;

    local = ALLOC(struct local_vars);
    local->prev = lvtbl;
    local->nofree = 0;
    local->cnt = 0;
    local->tbl = 0;
    local->dlev = 0;
    local->dyna_vars = ruby_dyna_vars;
    lvtbl = local;
    if (!top) {
        // preserve reference for GC, but link should be cut. 
        rb_dvar_push(0, (VALUE)ruby_dyna_vars);
        ruby_dyna_vars->next = 0;
    }
    */
}

static void
syd_local_pop(rb_parse_state *st)
{
    st->variables = var_table_pop(st->variables);
    // rb_funcall(st->self, rb_intern("local_pop"), 0);
    /*
    struct local_vars *local = lvtbl->prev;

    if (lvtbl->tbl) {
        if (!lvtbl->nofree) free(lvtbl->tbl);
        else lvtbl->tbl[0] = lvtbl->cnt;
    }
    ruby_dyna_vars = lvtbl->dyna_vars;
    free(lvtbl);
    lvtbl = local;
    */
}


static ID*
syd_local_tbl(rb_parse_state *st)
{
    ID *lcl_tbl;
    // ID tmp;
    var_table *tbl;
    int i, len;
    tbl = st->variables;
    len = tbl->size;
    // printf("Converting local table with %d entries.\n", len);
    lcl_tbl = pt_allocate(st, sizeof(ID) * (len + 3));
    lcl_tbl[0] = (ID)len;
    lcl_tbl[1] = '_';
    lcl_tbl[2] = '~';
    for(i = 0; i < len; i++) {
        lcl_tbl[i + 3] = tbl->data[i];
    }
    // printf("Created table %x\n", lcl_tbl);
    return lcl_tbl;
}

static int
syd_local_append(rb_parse_state *st, ID id)
{
  return var_table_add(st->variables, id);
  /*
    VALUE out;
    out = rb_funcall(st->self, rb_intern("local_append"), 1, ID2SYM(id));
    return NUM2INT(out);

    if (lvtbl->tbl == 0) {
        lvtbl->tbl = ALLOC_N(ID, 4);
        lvtbl->tbl[0] = 0;
        lvtbl->tbl[1] = '_';
        lvtbl->tbl[2] = '~';
        lvtbl->cnt = 2;
        if (id == '_') return 0;
        if (id == '~') return 1;
    }
    else {
        REALLOC_N(lvtbl->tbl, ID, lvtbl->cnt+2);
    }

    lvtbl->tbl[lvtbl->cnt+1] = id;
    return lvtbl->cnt++;
    */
}

static int
syd_local_cnt(rb_parse_state *st, ID id)
{
    int idx;
    /* Leave these hardcoded here because they arne't REALLY ids at all. */
    if(id == '_') {
        return 0;
    } else if(id == '~') {
        return 1;
    }
    
    idx = var_table_find(st->variables, id);
    if(idx >= 0) return idx + 2;
    
    return var_table_add(st->variables, id);
    
    /*
    out = rb_funcall(st->self, rb_intern("local_cnt"), 1, ID2SYM(id));
    
    return NUM2INT(out);
    
    int cnt, max;

    if (id == 0) return lvtbl->cnt;

    for (cnt=1, max=lvtbl->cnt+1; cnt<max;cnt++) {
        if (lvtbl->tbl[cnt] == id) return cnt-1;
    }
    return local_append(id);
    */
}

static int
syd_local_id(rb_parse_state *st, ID id)
{
    if(var_table_find(st->variables, id) >= 0) return 1;
    return 0;
    /*
    VALUE out;
    out = rb_funcall(st->self, rb_intern("local_defined?"), 1, ID2SYM(id));
    return out;
    
    int i, max;

    if (lvtbl == 0) return Qfalse;
    for (i=3, max=lvtbl->cnt+1; i<max; i++) {
        if (lvtbl->tbl[i] == id) return Qtrue;
    }
    return Qfalse;
    */
}

/*
static void
top_local_init()
{
    local_push(1);
    lvtbl->cnt = ruby_scope->local_tbl?ruby_scope->local_tbl[0]:0;
    if (lvtbl->cnt > 0) {
        lvtbl->tbl = ALLOC_N(ID, lvtbl->cnt+3);
        MEMCPY(lvtbl->tbl, ruby_scope->local_tbl, ID, lvtbl->cnt+1);
    }
    else {
        lvtbl->tbl = 0;
    }
    if (ruby_dyna_vars)
        lvtbl->dlev = 1;
    else
        lvtbl->dlev = 0;
}

static void
top_local_setup()
{
    int len = lvtbl->cnt;
    int i;

    if (len > 0) {
        i = ruby_scope->local_tbl?ruby_scope->local_tbl[0]:0;

        if (i < len) {
            if (i == 0 || (ruby_scope->flags & SCOPE_MALLOC) == 0) {
                VALUE *vars = ALLOC_N(VALUE, len+1);
                if (ruby_scope->local_vars) {
                    *vars++ = ruby_scope->local_vars[-1];
                    MEMCPY(vars, ruby_scope->local_vars, VALUE, i);
                    rb_mem_clear(vars+i, len-i);
                }
                else {
                    *vars++ = 0;
                    rb_mem_clear(vars, len);
                }
                ruby_scope->local_vars = vars;
                ruby_scope->flags |= SCOPE_MALLOC;
            }
            else {
                VALUE *vars = ruby_scope->local_vars-1;
                REALLOC_N(vars, VALUE, len+1);
                ruby_scope->local_vars = vars+1;
                rb_mem_clear(ruby_scope->local_vars+i, len-i);
            }
            if (ruby_scope->local_tbl && ruby_scope->local_vars[-1] == 0) {
                free(ruby_scope->local_tbl);
            }
            ruby_scope->local_vars[-1] = 0;
            ruby_scope->local_tbl = local_tbl();
        }
    }
    local_pop();
}
*/
/* 
 * Sets ruby_dyna_vars to a new set of dynamic vars.
 * Increments dlev in the local table so thot dyna_in_block()
 * knows that there are potential dynamic vars available.
 *
static VALUE
syd_dyna_push(rb_parse_state *st)
{
    return rb_funcall(st->self, rb_intern("dyna_push"), 0);
}

static void
syd_dyna_pop(rb_parse_state *st, VALUE vars)
{
    rb_funcall(st->self, rb_intern("dyna_pop"), 1, vars);
    lvtbl->dlev--;
    ruby_dyna_vars = vars;
}

static VALUE
syd_dvar_defined(rb_parse_state *st, ID id)
{
    return rb_funcall(st->self, rb_intern("dvar_defined"), 1, ID2SYM(id));
}

static VALUE
syd_dvar_curr(rb_parse_state *st, ID id) {
    return rb_funcall(st->self, rb_intern("dvar_curr"), 1, ID2SYM(id));
}

static void
syd_dvar_push(rb_parse_state *st, ID id, VALUE val) {
    rb_funcall(st->self, rb_intern("dvar_push"), 2, ID2SYM(id), val);
}

static int
syd_dyna_in_block(rb_parse_state *st)
{
    VALUE out;
    out = rb_funcall(st->self, rb_intern("dyna_in_block"), 0);
    if(RTEST(out)) {
        return 1;
    } else {
        return 0;
    }
    // return (lvtbl->dlev > 0);
}


static VALUE
syd_dyna_vars(rb_parse_state *st) {
    return rb_funcall(st->self, rb_intern("dyna_vars"), 0);
}
*/

/*
void
rb_parser_append_print()
{
    ruby_eval_tree =
        block_append(ruby_eval_tree,
                     NEW_FCALL(rb_intern("print"),
                               NEW_ARRAY(NEW_GVAR(rb_intern("$_")))));
}

void
rb_parser_while_loop(chop, split)
    int chop, split;
{
    if (split) {
        ruby_eval_tree =
            block_append(NEW_GASGN(rb_intern("$F"),
                                   NEW_CALL(NEW_GVAR(rb_intern("$_")),
                                            rb_intern("split"), 0)),
                                   ruby_eval_tree);
    }
    if (chop) {
        ruby_eval_tree =
            block_append(NEW_CALL(NEW_GVAR(rb_intern("$_")),
                                  rb_intern("chop!"), 0), ruby_eval_tree);
    }
    ruby_eval_tree = NEW_OPT_N(ruby_eval_tree);
}

*/


static ID syd_last_id;

#if 0
/* FIXME:
 * Why is this function not called?
 * Someone look into this and either put this to use or kill it.
 */
void setup_parser() {
    syd_last_id = tLAST_TOKEN;
}
#endif

/*
void
Init_sym()
{
    STATE_VAR(last_id) = tLAST_TOKEN;
    sym_tbl = st_init_strtable_with_size(200);
    sym_rev_tbl = st_init_numtable_with_size(200);
}
*/
static ID
internal_id()
{
    return ID_INTERNAL | (++syd_last_id << ID_SCOPE_SHIFT);
}

static ID
rb_intern(const char *name)
{
    const char *m = name;
    ID id, pre, qrk, bef;
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
            /*
            char *buf = (char*)malloc(sizeof(char) * (last+1));

            strncpy(buf, name, last);
            buf[last] = '\0';
            id = rb_intern(buf);
            free(buf);
            if (id > tLAST_TOKEN && !is_attrset_id(id)) {
                id = rb_id_attrset(id);
                goto id_regist;
            }
            */
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
    qrk = (ID)g_quark_from_string(name);
    pre = qrk + tLAST_TOKEN;
    bef = id;
    id |= ( pre << ID_SCOPE_SHIFT );
    // printf("Registered '%s' as %d (%d, %d, %d).\n", name, id, qrk, tLAST_TOKEN, bef);
    return id;
}

GQuark id_to_quark(ID id) {
  GQuark qrk;
  
  qrk = (GQuark)((id >> ID_SCOPE_SHIFT) - tLAST_TOKEN);
  // printf("ID %d == %d\n", id, qrk);
  return qrk;
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
    char *tmp;

    while (len-- && *s && (tmp = strchr(hexdigit, *s))) {
        retval <<= 4;
        retval |= (tmp - hexdigit) & 15;
        s++;
    }
    *retlen = s - start;
    return retval;
}

const char *op_to_name(ID id) {
  if(id < tLAST_TOKEN) {
    int i = 0;

    for (i=0; op_tbl[i].token; i++) {
        if (op_tbl[i].token == id)
            return op_tbl[i].name;
    }
  }
  return NULL;
}

/*
char *
rb_id2name(id)
    ID id;
{
    char *name;

    if (id < tLAST_TOKEN) {
        int i = 0;

        for (i=0; op_tbl[i].token; i++) {
            if (op_tbl[i].token == id)
                return op_tbl[i].name;
        }
    }
    
    name = g_quark_to_string(id >> ID_SCOPE_SHIFT);
    if(name) return name;

    if (is_attrset_id(id)) {
        ID id2 = (id & ~ID_SCOPE_MASK) | ID_LOCAL;

      again:
        name = rb_id2name(id2);
        if (name) {
            char *buf = ALLOCA_N(char, strlen(name)+2);

            strcpy(buf, name);
            strcat(buf, "=");
            rb_intern(buf);
            return rb_id2name(id);
        }
        if (is_local_id(id2)) {
            id2 = (id & ~ID_SCOPE_MASK) | ID_CONST;
            goto again;
        }
    }
    return 0;
}

static int
symbols_i(key, value, ary)
    char *key;
    ID value;
    VALUE ary;
{
    rb_ary_push(ary, ID2SYM(value));
    return ST_CONTINUE;
}
*/
/*
 *  call-seq:
 *     Symbol.all_symbols    => array
 *  
 *  Returns an array of all the symbols currently in Ruby's symbol
 *  table.
 *     
 *     Symbol.all_symbols.size    #=> 903
 *     Symbol.all_symbols[1,20]   #=> [:floor, :ARGV, :Binding, :symlink,
 *                                     :chown, :EOFError, :$;, :String, 
 *                                     :LOCK_SH, :"setuid?", :$<, 
 *                                     :default_proc, :compact, :extend, 
 *                                     :Tms, :getwd, :$=, :ThreadGroup,
 *                                     :wait2, :$>]
 

VALUE
rb_sym_all_symbols()
{
    VALUE ary = rb_ary_new2(sym_tbl->num_entries);

    st_foreach(sym_tbl, symbols_i, ary);
    return ary;
}

int
rb_is_const_id(id)
    ID id;
{
    if (is_const_id(id)) return Qtrue;
    return Qfalse;
}

int
rb_is_class_id(id)
    ID id;
{
    if (is_class_id(id)) return Qtrue;
    return Qfalse;
}

int
rb_is_instance_id(id)
    ID id;
{
    if (is_instance_id(id)) return Qtrue;
    return Qfalse;
}

int
rb_is_local_id(id)
    ID id;
{
    if (is_local_id(id)) return Qtrue;
    return Qfalse;
}

int
rb_is_junk_id(id)
    ID id;
{
    if (is_junk_id(id)) return Qtrue;
    return Qfalse;
}


static void
special_local_set(c, val)
    char c;
    VALUE val;
{
    int cnt;

    top_local_init();
    cnt = local_cnt(c);
    top_local_setup();
    ruby_scope->local_vars[cnt] = val;
}


VALUE
rb_backref_get()
{
    VALUE *var = rb_svar(1);
    if (var) {
        return *var;
    }
    return Qnil;
}

void
rb_backref_set(val)
    VALUE val;
{
    VALUE *var = rb_svar(1);
    if (var) {
        *var = val;
    }
    else {
        special_local_set('~', val);
    }
}

VALUE
rb_lastline_get()
{
    VALUE *var = rb_svar(0);
    if (var) {
        return *var;
    }
    return Qnil;
}

void
rb_lastline_set(val)
    VALUE val;
{
    VALUE *var = rb_svar(0);
    if (var) {
        *var = val;
    }
    else {
        special_local_set('_', val);
    }
}
*/


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
     tKEYSYM = 318,
     tUPLUS = 319,
     tUMINUS = 320,
     tUBS = 321,
     tPOW = 322,
     tCMP = 323,
     tEQ = 324,
     tEQQ = 325,
     tNEQ = 326,
     tGEQ = 327,
     tLEQ = 328,
     tANDOP = 329,
     tOROP = 330,
     tMATCH = 331,
     tNMATCH = 332,
     tDOT2 = 333,
     tDOT3 = 334,
     tAREF = 335,
     tASET = 336,
     tLSHFT = 337,
     tRSHFT = 338,
     tCOLON2 = 339,
     tCOLON3 = 340,
     tOP_ASGN = 341,
     tASSOC = 342,
     tLPAREN = 343,
     tLPAREN_ARG = 344,
     tRPAREN = 345,
     tLBRACK = 346,
     tLBRACE = 347,
     tLBRACE_ARG = 348,
     tSTAR = 349,
     tAMPER = 350,
     tSYMBEG = 351,
     tSTRING_BEG = 352,
     tREGEXP_BEG = 353,
     tWORDS_BEG = 354,
     tQWORDS_BEG = 355,
     tSTRING_DBEG = 356,
     tSTRING_DVAR = 357,
     tSTRING_END = 358,
     tLOWEST = 359,
     tUMINUS_NUM = 360,
     tLAST_TOKEN = 361
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
#define tKEYSYM 318
#define tUPLUS 319
#define tUMINUS 320
#define tUBS 321
#define tPOW 322
#define tCMP 323
#define tEQ 324
#define tEQQ 325
#define tNEQ 326
#define tGEQ 327
#define tLEQ 328
#define tANDOP 329
#define tOROP 330
#define tMATCH 331
#define tNMATCH 332
#define tDOT2 333
#define tDOT3 334
#define tAREF 335
#define tASET 336
#define tLSHFT 337
#define tRSHFT 338
#define tCOLON2 339
#define tCOLON3 340
#define tOP_ASGN 341
#define tASSOC 342
#define tLPAREN 343
#define tLPAREN_ARG 344
#define tRPAREN 345
#define tLBRACK 346
#define tLBRACE 347
#define tLBRACE_ARG 348
#define tSTAR 349
#define tAMPER 350
#define tSYMBEG 351
#define tSTRING_BEG 352
#define tREGEXP_BEG 353
#define tWORDS_BEG 354
#define tQWORDS_BEG 355
#define tSTRING_DBEG 356
#define tSTRING_DVAR 357
#define tSTRING_END 358
#define tLOWEST 359
#define tUMINUS_NUM 360
#define tLAST_TOKEN 361




/* Copy the first part of user declarations.  */
#line 13 "grammar.y"


#define YYDEBUG 1
#define YYERROR_VERBOSE 1

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#include "grammer_internal.h"
#include "grammer_runtime.h"
#include "array.h"

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

#define string_new(ptr, len) g_string_new_len(ptr, len)
#define string_new2(ptr) g_string_new(ptr)
#define g_string_append_string(h,t) g_string_append_len(h, t->str, t->len)

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
 
    int func, term, paren, *nest;
    rb_parse_state *parse_state;
 
//static void syd_rb_warn(char *fmt, ...);
#define rb_warn printf
#define rb_warning printf

//static void syd_compile_error(char *fmt, ...);
#define rb_compile_error printf

ID rb_intern(char *name);
ID rb_id_attrset(ID);

static NODE *
extract_block_vars(rb_parse_state *parse_state, NODE* node, var_table *vars);
rb_parse_state *alloc_parse_state();
unsigned long scan_oct(const char *start, int len, int *retlen);
unsigned long scan_hex(const char *start, int len, int *retlen);

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
#define SIGN_EXTEND(x,n) ((((1<<(n))-1)^((x)&~(~0<<(n))))-((1<<(n))-1))
#define nd_func u1.id
#if SIZEOF_SHORT == 2
#define nd_term(node) ((signed short)(node)->u2.id)
#else
#define nd_term(node) SIGN_EXTEND((node)->u2.id, (CHAR_BIT*2))
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
#line 273 "grammar.y"
{
    NODE *node;
    ID id;
    int num;
    var_table *vars;
}
/* Line 193 of yacc.c.  */
#line 573 "grammar.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 586 "grammar.c"

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
# if YYENABLE_NLS
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
#define YYLAST   9913

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  135
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  146
/* YYNRULES -- Number of rules.  */
#define YYNRULES  506
/* YYNRULES -- Number of states.  */
#define YYNSTATES  904

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   361

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     133,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,   132,   119,     2,     2,     2,   117,   112,     2,
     130,   127,   115,   113,   128,   114,   126,   116,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   107,   134,
     109,   105,   108,   106,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   124,   131,   125,   111,     2,   129,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   122,   110,   123,   120,     2,     2,     2,
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
     118,   121
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
     482,   484,   486,   488,   490,   494,   500,   504,   511,   517,
     523,   529,   535,   540,   544,   548,   552,   556,   560,   564,
     568,   572,   576,   581,   586,   589,   592,   596,   600,   604,
     608,   612,   616,   620,   624,   628,   632,   636,   640,   644,
     647,   650,   654,   658,   662,   666,   667,   672,   673,   680,
     682,   684,   686,   689,   692,   698,   701,   705,   709,   714,
     719,   726,   728,   730,   732,   735,   741,   744,   750,   755,
     763,   767,   769,   774,   778,   784,   792,   795,   801,   806,
     813,   821,   831,   835,   837,   838,   841,   843,   844,   848,
     849,   854,   857,   860,   862,   864,   868,   872,   877,   880,
     882,   884,   886,   888,   890,   892,   894,   896,   898,   899,
     904,   905,   911,   915,   919,   922,   927,   931,   935,   937,
     942,   946,   948,   949,   956,   959,   961,   964,   971,   978,
     979,   980,   988,   989,   990,   998,  1004,  1009,  1015,  1016,
    1017,  1027,  1028,  1035,  1036,  1037,  1046,  1047,  1053,  1054,
    1061,  1062,  1063,  1073,  1075,  1077,  1079,  1081,  1083,  1085,
    1087,  1089,  1092,  1094,  1096,  1098,  1100,  1106,  1108,  1111,
    1113,  1115,  1117,  1120,  1122,  1126,  1127,  1128,  1135,  1138,
    1143,  1148,  1151,  1156,  1161,  1165,  1169,  1172,  1175,  1177,
    1178,  1179,  1186,  1187,  1188,  1195,  1201,  1203,  1208,  1211,
    1213,  1215,  1222,  1224,  1226,  1228,  1230,  1233,  1235,  1238,
    1240,  1242,  1244,  1246,  1248,  1250,  1253,  1257,  1261,  1265,
    1269,  1273,  1274,  1278,  1280,  1283,  1287,  1291,  1292,  1296,
    1297,  1300,  1301,  1304,  1306,  1307,  1311,  1312,  1317,  1319,
    1321,  1323,  1325,  1328,  1330,  1332,  1334,  1336,  1340,  1342,
    1344,  1347,  1350,  1352,  1354,  1356,  1358,  1360,  1362,  1364,
    1366,  1368,  1370,  1372,  1374,  1376,  1378,  1380,  1382,  1383,
    1388,  1391,  1396,  1399,  1406,  1411,  1416,  1419,  1424,  1427,
    1430,  1432,  1433,  1435,  1437,  1439,  1441,  1443,  1445,  1449,
    1453,  1455,  1459,  1461,  1463,  1466,  1468,  1470,  1472,  1475,
    1478,  1480,  1482,  1483,  1489,  1491,  1494,  1497,  1499,  1503,
    1505,  1507,  1511,  1514,  1516,  1518,  1520,  1522,  1524,  1526,
    1528,  1530,  1532,  1534,  1536,  1538,  1539,  1541,  1542,  1544,
    1545,  1547,  1549,  1551,  1553,  1555,  1558
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     136,     0,    -1,    -1,   137,   139,    -1,   139,   222,   206,
     225,    -1,   140,   275,    -1,   280,    -1,   141,    -1,   140,
     279,   141,    -1,     1,   141,    -1,    -1,    44,   162,   142,
     162,    -1,    44,    52,    52,    -1,    44,    52,    61,    -1,
      44,    52,    60,    -1,     6,   163,    -1,   141,    39,   145,
      -1,   141,    40,   145,    -1,   141,    41,   145,    -1,   141,
      42,   145,    -1,   141,    43,   141,    -1,    -1,    46,   143,
     122,   139,   123,    -1,    47,   122,   139,   123,    -1,   158,
     105,   146,    -1,   152,   105,   146,    -1,   249,    86,   146,
      -1,   202,   124,   171,   125,    86,   146,    -1,   202,   126,
      50,    86,   146,    -1,   202,   126,    54,    86,   146,    -1,
     202,    84,    50,    86,   146,    -1,   250,    86,   146,    -1,
     158,   105,   184,    -1,   152,   105,   170,    -1,   152,   105,
     184,    -1,   144,    -1,   146,    -1,   144,    36,   144,    -1,
     144,    37,   144,    -1,    38,   144,    -1,   119,   146,    -1,
     167,    -1,   144,    -1,   151,    -1,   147,    -1,    29,   174,
      -1,    21,   174,    -1,    22,   174,    -1,   212,    -1,   212,
     126,   272,   176,    -1,   212,    84,   272,   176,    -1,    -1,
      -1,    93,   149,   208,   150,   139,   123,    -1,   271,   176,
      -1,   271,   176,   148,    -1,   202,   126,   272,   176,    -1,
     202,   126,   272,   176,   148,    -1,   202,    84,   272,   176,
      -1,   202,    84,   272,   176,   148,    -1,    31,   176,    -1,
      30,   176,    -1,   154,    -1,    88,   153,   127,    -1,   154,
      -1,    88,   153,   127,    -1,   156,    -1,   156,   155,    -1,
     156,    94,   157,    -1,   156,    94,    -1,    94,   157,    -1,
      94,    -1,   157,    -1,    88,   153,   127,    -1,   155,   128,
      -1,   156,   155,   128,    -1,   247,    -1,   202,   124,   171,
     125,    -1,   202,   126,    50,    -1,   202,    84,    50,    -1,
     202,   126,    54,    -1,   202,    84,    54,    -1,    85,    54,
      -1,   250,    -1,   247,    -1,   202,   124,   171,   125,    -1,
     202,   126,    50,    -1,   202,    84,    50,    -1,   202,   126,
      54,    -1,   202,    84,    54,    -1,    85,    54,    -1,   250,
      -1,    50,    -1,    54,    -1,    85,   159,    -1,   159,    -1,
     202,    84,   159,    -1,    50,    -1,    54,    -1,    51,    -1,
     165,    -1,   166,    -1,   161,    -1,   243,    -1,   162,    -1,
      -1,   163,   128,   164,   162,    -1,   110,    -1,   111,    -1,
     112,    -1,    68,    -1,    69,    -1,    70,    -1,    76,    -1,
     108,    -1,    72,    -1,   109,    -1,    73,    -1,    82,    -1,
      83,    -1,   113,    -1,   114,    -1,   115,    -1,    94,    -1,
     116,    -1,   117,    -1,    67,    -1,   120,    -1,    64,    -1,
      65,    -1,    80,    -1,    81,    -1,   129,    -1,    48,    -1,
      49,    -1,    46,    -1,    47,    -1,    44,    -1,    36,    -1,
       7,    -1,    21,    -1,    16,    -1,     3,    -1,     5,    -1,
      45,    -1,    26,    -1,    15,    -1,    14,    -1,    10,    -1,
       9,    -1,    35,    -1,    20,    -1,    25,    -1,     4,    -1,
      22,    -1,    33,    -1,    38,    -1,    37,    -1,    23,    -1,
       8,    -1,    24,    -1,    29,    -1,    32,    -1,    31,    -1,
      13,    -1,    34,    -1,     6,    -1,    17,    -1,    30,    -1,
      39,    -1,    40,    -1,    41,    -1,    42,    -1,    43,    -1,
     158,   105,   167,    -1,   158,   105,   167,    43,   167,    -1,
     249,    86,   167,    -1,   202,   124,   171,   125,    86,   167,
      -1,   202,   126,    50,    86,   167,    -1,   202,   126,    54,
      86,   167,    -1,   202,    84,    50,    86,   167,    -1,   202,
      84,    54,    86,   167,    -1,    85,    54,    86,   167,    -1,
     250,    86,   167,    -1,   167,    78,   167,    -1,   167,    79,
     167,    -1,   167,   113,   167,    -1,   167,   114,   167,    -1,
     167,   115,   167,    -1,   167,   116,   167,    -1,   167,   117,
     167,    -1,   167,    67,   167,    -1,   118,    57,    67,   167,
      -1,   118,    58,    67,   167,    -1,    64,   167,    -1,    65,
     167,    -1,   167,   110,   167,    -1,   167,   111,   167,    -1,
     167,   112,   167,    -1,   167,    68,   167,    -1,   167,   108,
     167,    -1,   167,    72,   167,    -1,   167,   109,   167,    -1,
     167,    73,   167,    -1,   167,    69,   167,    -1,   167,    70,
     167,    -1,   167,    71,   167,    -1,   167,    76,   167,    -1,
     167,    77,   167,    -1,   119,   167,    -1,   120,   167,    -1,
     167,    82,   167,    -1,   167,    83,   167,    -1,   167,    74,
     167,    -1,   167,    75,   167,    -1,    -1,    45,   276,   168,
     167,    -1,    -1,   167,   106,   169,   167,   107,   167,    -1,
     185,    -1,   167,    -1,   280,    -1,   151,   276,    -1,   183,
     277,    -1,   183,   128,    94,   167,   276,    -1,   267,   277,
      -1,    94,   167,   276,    -1,   130,   280,   127,    -1,   130,
     174,   276,   127,    -1,   130,   212,   276,   127,    -1,   130,
     183,   128,   212,   276,   127,    -1,   280,    -1,   172,    -1,
     151,    -1,   183,   182,    -1,   183,   128,    94,   170,   182,
      -1,   267,   182,    -1,   267,   128,    94,   170,   182,    -1,
     183,   128,   267,   182,    -1,   183,   128,   267,   128,    94,
     167,   182,    -1,    94,   170,   182,    -1,   181,    -1,   170,
     128,   183,   182,    -1,   170,   128,   181,    -1,   170,   128,
      94,   170,   182,    -1,   170,   128,   183,   128,    94,   170,
     182,    -1,   267,   182,    -1,   267,   128,    94,   170,   182,
      -1,   170,   128,   267,   182,    -1,   170,   128,   183,   128,
     267,   182,    -1,   170,   128,   267,   128,    94,   170,   182,
      -1,   170,   128,   183,   128,   267,   128,    94,   170,   182,
      -1,    94,   170,   182,    -1,   181,    -1,    -1,   177,   178,
      -1,   174,    -1,    -1,    89,   179,   127,    -1,    -1,    89,
     175,   180,   127,    -1,    95,   170,    -1,   128,   181,    -1,
     280,    -1,   170,    -1,   183,   128,   170,    -1,   183,   128,
     170,    -1,   183,   128,    94,   170,    -1,    94,   170,    -1,
     226,    -1,   227,    -1,   230,    -1,   231,    -1,   232,    -1,
     235,    -1,   248,    -1,   250,    -1,    51,    -1,    -1,     7,
     186,   138,    10,    -1,    -1,    89,   144,   187,   276,   127,
      -1,    88,   139,   127,    -1,   202,    84,    54,    -1,    85,
      54,    -1,   202,   124,   171,   125,    -1,    91,   171,   125,
      -1,    92,   266,   123,    -1,    29,    -1,    30,   130,   174,
     127,    -1,    30,   130,   127,    -1,    30,    -1,    -1,    45,
     276,   130,   188,   144,   127,    -1,   271,   214,    -1,   213,
      -1,   213,   214,    -1,    11,   145,   203,   139,   205,    10,
      -1,    12,   145,   203,   139,   206,    10,    -1,    -1,    -1,
      18,   189,   145,   204,   190,   139,    10,    -1,    -1,    -1,
      19,   191,   145,   204,   192,   139,    10,    -1,    16,   145,
     275,   219,    10,    -1,    16,   275,   219,    10,    -1,    16,
     275,    15,   139,    10,    -1,    -1,    -1,    20,   207,    25,
     193,   145,   204,   194,   139,    10,    -1,    -1,     3,   160,
     251,   195,   138,    10,    -1,    -1,    -1,     3,    82,   144,
     196,   278,   197,   138,    10,    -1,    -1,     4,   160,   198,
     138,    10,    -1,    -1,     5,   161,   199,   253,   138,    10,
      -1,    -1,    -1,     5,   264,   274,   200,   161,   201,   253,
     138,    10,    -1,    21,    -1,    22,    -1,    23,    -1,    24,
      -1,   185,    -1,   278,    -1,   107,    -1,    13,    -1,   278,
      13,    -1,   278,    -1,   107,    -1,    27,    -1,   206,    -1,
      14,   145,   203,   139,   205,    -1,   280,    -1,    15,   139,
      -1,   158,    -1,   152,    -1,   280,    -1,   110,   110,    -1,
      75,    -1,   110,   207,   110,    -1,    -1,    -1,    28,   210,
     208,   211,   139,    10,    -1,   151,   209,    -1,   212,   126,
     272,   173,    -1,   212,    84,   272,   173,    -1,   271,   172,
      -1,   202,   126,   272,   173,    -1,   202,    84,   272,   172,
      -1,   202,    84,   273,    -1,   202,   131,   272,    -1,    66,
     272,    -1,    31,   172,    -1,    31,    -1,    -1,    -1,   122,
     215,   208,   216,   139,   123,    -1,    -1,    -1,    26,   217,
     208,   218,   139,    10,    -1,    17,   220,   203,   139,   221,
      -1,   183,    -1,   183,   128,    94,   170,    -1,    94,   170,
      -1,   206,    -1,   219,    -1,     8,   223,   224,   203,   139,
     222,    -1,   280,    -1,   170,    -1,   184,    -1,   280,    -1,
      87,   158,    -1,   280,    -1,     9,   139,    -1,   280,    -1,
     246,    -1,   243,    -1,   245,    -1,   228,    -1,   229,    -1,
     228,   229,    -1,    97,   237,   103,    -1,    56,   238,   103,
      -1,    98,   238,    62,    -1,    99,   132,   103,    -1,    99,
     233,   103,    -1,    -1,   233,   234,   132,    -1,   239,    -1,
     234,   239,    -1,   100,   132,   103,    -1,   100,   236,   103,
      -1,    -1,   236,    59,   132,    -1,    -1,   237,   239,    -1,
      -1,   238,   239,    -1,    59,    -1,    -1,   102,   240,   242,
      -1,    -1,   101,   241,   139,   123,    -1,    52,    -1,    53,
      -1,    55,    -1,   250,    -1,    96,   244,    -1,   161,    -1,
      53,    -1,    52,    -1,    55,    -1,    96,   238,   103,    -1,
      57,    -1,    58,    -1,   118,    57,    -1,   118,    58,    -1,
      50,    -1,    53,    -1,    52,    -1,    54,    -1,    55,    -1,
      33,    -1,    32,    -1,    34,    -1,    35,    -1,    49,    -1,
      48,    -1,   247,    -1,   247,    -1,    60,    -1,    61,    -1,
     278,    -1,    -1,   109,   252,   145,   278,    -1,     1,   278,
      -1,   130,   254,   276,   127,    -1,   254,   278,    -1,   256,
     128,   258,   128,   260,   263,    -1,   256,   128,   258,   263,
      -1,   256,   128,   260,   263,    -1,   256,   263,    -1,   258,
     128,   260,   263,    -1,   258,   263,    -1,   260,   263,    -1,
     262,    -1,    -1,    54,    -1,    53,    -1,    52,    -1,    55,
      -1,    50,    -1,   255,    -1,   256,   128,   255,    -1,    50,
     105,   170,    -1,   257,    -1,   258,   128,   257,    -1,   115,
      -1,    94,    -1,   259,    50,    -1,   259,    -1,   112,    -1,
      95,    -1,   261,    50,    -1,   128,   262,    -1,   280,    -1,
     248,    -1,    -1,   130,   265,   144,   276,   127,    -1,   280,
      -1,   267,   277,    -1,   183,   277,    -1,   268,    -1,   267,
     128,   268,    -1,   269,    -1,   270,    -1,   170,    87,   170,
      -1,    63,   170,    -1,    50,    -1,    54,    -1,    51,    -1,
      50,    -1,    54,    -1,    51,    -1,   165,    -1,    50,    -1,
      51,    -1,   165,    -1,   126,    -1,    84,    -1,    -1,   279,
      -1,    -1,   133,    -1,    -1,   133,    -1,   128,    -1,   134,
      -1,   133,    -1,   278,    -1,   279,   134,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   411,   411,   411,   442,   462,   469,   470,   474,   478,
     484,   484,   488,   492,   499,   504,   508,   517,   526,   538,
     550,   555,   554,   570,   578,   582,   588,   613,   632,   644,
     656,   668,   673,   677,   682,   687,   690,   691,   695,   699,
     703,   707,   710,   717,   718,   719,   723,   727,   733,   734,
     738,   745,   749,   744,   758,   763,   775,   780,   792,   797,
     809,   814,   821,   822,   828,   829,   835,   839,   843,   847,
     851,   855,   861,   862,   868,   872,   878,   882,   886,   890,
     894,   898,   904,   910,   917,   921,   925,   929,   933,   937,
     943,   949,   956,   960,   963,   967,   971,   977,   978,   979,
     980,   985,   992,   993,   996,  1000,  1000,  1006,  1007,  1008,
    1009,  1010,  1011,  1012,  1013,  1014,  1015,  1016,  1017,  1018,
    1019,  1020,  1021,  1022,  1023,  1024,  1025,  1026,  1027,  1028,
    1029,  1030,  1031,  1034,  1034,  1034,  1034,  1035,  1035,  1035,
    1035,  1035,  1035,  1035,  1036,  1036,  1036,  1036,  1036,  1036,
    1036,  1037,  1037,  1037,  1037,  1037,  1037,  1038,  1038,  1038,
    1038,  1038,  1038,  1038,  1039,  1039,  1039,  1039,  1039,  1040,
    1040,  1040,  1040,  1040,  1043,  1047,  1051,  1076,  1095,  1107,
    1119,  1131,  1136,  1141,  1146,  1152,  1158,  1162,  1166,  1170,
    1174,  1178,  1182,  1186,  1190,  1199,  1203,  1207,  1211,  1215,
    1219,  1223,  1227,  1231,  1235,  1239,  1243,  1247,  1251,  1255,
    1259,  1263,  1267,  1271,  1275,  1279,  1279,  1284,  1284,  1290,
    1296,  1303,  1304,  1309,  1313,  1318,  1322,  1329,  1333,  1337,
    1342,  1349,  1350,  1353,  1358,  1362,  1367,  1372,  1377,  1382,
    1388,  1392,  1395,  1399,  1403,  1408,  1413,  1418,  1423,  1428,
    1433,  1438,  1443,  1447,  1450,  1450,  1462,  1463,  1463,  1468,
    1468,  1475,  1481,  1485,  1488,  1492,  1498,  1502,  1506,  1512,
    1513,  1514,  1515,  1516,  1517,  1518,  1519,  1520,  1525,  1524,
    1537,  1537,  1542,  1546,  1550,  1554,  1563,  1572,  1576,  1580,
    1584,  1588,  1592,  1592,  1597,  1603,  1604,  1613,  1626,  1639,
    1639,  1639,  1649,  1649,  1649,  1659,  1666,  1670,  1674,  1674,
    1674,  1682,  1681,  1698,  1703,  1697,  1720,  1719,  1736,  1735,
    1753,  1754,  1753,  1768,  1772,  1776,  1780,  1786,  1793,  1794,
    1795,  1796,  1799,  1800,  1801,  1804,  1805,  1814,  1815,  1821,
    1822,  1825,  1826,  1830,  1834,  1841,  1846,  1840,  1857,  1866,
    1870,  1876,  1881,  1886,  1891,  1895,  1899,  1903,  1907,  1914,
    1918,  1913,  1925,  1929,  1924,  1937,  1944,  1945,  1949,  1955,
    1956,  1959,  1970,  1973,  1977,  1978,  1981,  1985,  1988,  1996,
    1999,  2000,  2004,  2007,  2020,  2021,  2027,  2033,  2062,  2097,
    2101,  2108,  2111,  2117,  2118,  2124,  2128,  2135,  2138,  2145,
    2148,  2155,  2158,  2164,  2166,  2165,  2177,  2176,  2197,  2198,
    2199,  2200,  2203,  2210,  2211,  2212,  2213,  2216,  2244,  2245,
    2246,  2250,  2256,  2257,  2258,  2259,  2260,  2261,  2262,  2263,
    2264,  2265,  2266,  2269,  2275,  2281,  2282,  2285,  2290,  2289,
    2297,  2300,  2305,  2311,  2316,  2320,  2325,  2329,  2334,  2338,
    2343,  2348,  2353,  2357,  2361,  2365,  2369,  2380,  2381,  2387,
    2397,  2402,  2408,  2409,  2412,  2420,  2426,  2427,  2430,  2440,
    2444,  2447,  2457,  2457,  2482,  2483,  2487,  2496,  2497,  2503,
    2505,  2509,  2515,  2521,  2522,  2523,  2526,  2527,  2528,  2529,
    2532,  2533,  2534,  2537,  2538,  2541,  2542,  2545,  2546,  2549,
    2550,  2551,  2554,  2555,  2558,  2559,  2562
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
  "tSTRING_CONTENT", "tNTH_REF", "tBACK_REF", "tREGEXP_END", "tKEYSYM",
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
  "$accept", "program", "@1", "bodystmt", "compstmt", "stmts", "stmt",
  "@2", "@3", "expr", "expr_value", "command_call", "block_command",
  "cmd_brace_block", "@4", "@5", "command", "mlhs", "mlhs_entry",
  "mlhs_basic", "mlhs_item", "mlhs_head", "mlhs_node", "lhs", "cname",
  "cpath", "fname", "fitem", "undef_list", "@6", "op", "reswords", "arg",
  "@7", "@8", "arg_value", "aref_args", "paren_args", "opt_paren_args",
  "call_args", "call_args2", "command_args", "@9", "open_args", "@10",
  "@11", "block_arg", "opt_block_arg", "args", "mrhs", "primary", "@12",
  "@13", "@14", "@15", "@16", "@17", "@18", "@19", "@20", "@21", "@22",
  "@23", "@24", "@25", "@26", "@27", "primary_value", "then", "do",
  "if_tail", "opt_else", "block_var", "opt_block_var", "do_block", "@28",
  "@29", "block_call", "method_call", "brace_block", "@30", "@31", "@32",
  "@33", "case_body", "when_args", "cases", "opt_rescue", "exc_list",
  "exc_var", "opt_ensure", "literal", "strings", "string", "string1",
  "xstring", "regexp", "words", "word_list", "word", "qwords",
  "qword_list", "string_contents", "xstring_contents", "string_content",
  "@34", "@35", "string_dvar", "symbol", "sym", "dsym", "numeric",
  "variable", "var_ref", "var_lhs", "backref", "superclass", "@36",
  "f_arglist", "f_args", "f_norm_arg", "f_arg", "f_opt", "f_optarg",
  "restarg_mark", "f_rest_arg", "blkarg_mark", "f_block_arg",
  "opt_f_block_arg", "singleton", "@37", "assoc_list", "assocs", "assoc",
  "assoc1", "assoc2", "operation", "operation2", "operation3",
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
     355,   356,   357,   358,   359,    61,    63,    58,    62,    60,
     124,    94,    38,    43,    45,    42,    47,    37,   360,    33,
     126,   361,   123,   125,    91,    93,    46,    41,    44,    96,
      40,    92,    32,    10,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   135,   137,   136,   138,   139,   140,   140,   140,   140,
     142,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   143,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   144,   144,   144,   144,
     144,   144,   145,   146,   146,   146,   146,   146,   147,   147,
     147,   149,   150,   148,   151,   151,   151,   151,   151,   151,
     151,   151,   152,   152,   153,   153,   154,   154,   154,   154,
     154,   154,   155,   155,   156,   156,   157,   157,   157,   157,
     157,   157,   157,   157,   158,   158,   158,   158,   158,   158,
     158,   158,   159,   159,   160,   160,   160,   161,   161,   161,
     161,   161,   162,   162,   163,   164,   163,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   168,   167,   169,   167,   167,
     170,   171,   171,   171,   171,   171,   171,   172,   172,   172,
     172,   173,   173,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   177,   176,   178,   179,   178,   180,
     178,   181,   182,   182,   183,   183,   184,   184,   184,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   186,   185,
     187,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   188,   185,   185,   185,   185,   185,   185,   189,
     190,   185,   191,   192,   185,   185,   185,   185,   193,   194,
     185,   195,   185,   196,   197,   185,   198,   185,   199,   185,
     200,   201,   185,   185,   185,   185,   185,   202,   203,   203,
     203,   203,   204,   204,   204,   205,   205,   206,   206,   207,
     207,   208,   208,   208,   208,   210,   211,   209,   212,   212,
     212,   213,   213,   213,   213,   213,   213,   213,   213,   215,
     216,   214,   217,   218,   214,   219,   220,   220,   220,   221,
     221,   222,   222,   223,   223,   223,   224,   224,   225,   225,
     226,   226,   226,   227,   228,   228,   229,   230,   231,   232,
     232,   233,   233,   234,   234,   235,   235,   236,   236,   237,
     237,   238,   238,   239,   240,   239,   241,   239,   242,   242,
     242,   242,   243,   244,   244,   244,   244,   245,   246,   246,
     246,   246,   247,   247,   247,   247,   247,   247,   247,   247,
     247,   247,   247,   248,   249,   250,   250,   251,   252,   251,
     251,   253,   253,   254,   254,   254,   254,   254,   254,   254,
     254,   254,   255,   255,   255,   255,   255,   256,   256,   257,
     258,   258,   259,   259,   260,   260,   261,   261,   262,   263,
     263,   264,   265,   264,   266,   266,   266,   267,   267,   268,
     268,   269,   270,   271,   271,   271,   272,   272,   272,   272,
     273,   273,   273,   274,   274,   275,   275,   276,   276,   277,
     277,   277,   278,   278,   279,   279,   280
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
       1,     1,     1,     1,     3,     5,     3,     6,     5,     5,
       5,     5,     4,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     4,     4,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     3,     3,     3,     3,     0,     4,     0,     6,     1,
       1,     1,     2,     2,     5,     2,     3,     3,     4,     4,
       6,     1,     1,     1,     2,     5,     2,     5,     4,     7,
       3,     1,     4,     3,     5,     7,     2,     5,     4,     6,
       7,     9,     3,     1,     0,     2,     1,     0,     3,     0,
       4,     2,     2,     1,     1,     3,     3,     4,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     4,
       0,     5,     3,     3,     2,     4,     3,     3,     1,     4,
       3,     1,     0,     6,     2,     1,     2,     6,     6,     0,
       0,     7,     0,     0,     7,     5,     4,     5,     0,     0,
       9,     0,     6,     0,     0,     8,     0,     5,     0,     6,
       0,     0,     9,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     5,     1,     2,     1,
       1,     1,     2,     1,     3,     0,     0,     6,     2,     4,
       4,     2,     4,     4,     3,     3,     2,     2,     1,     0,
       0,     6,     0,     0,     6,     5,     1,     4,     2,     1,
       1,     6,     1,     1,     1,     1,     2,     1,     2,     1,
       1,     1,     1,     1,     1,     2,     3,     3,     3,     3,
       3,     0,     3,     1,     2,     3,     3,     0,     3,     0,
       2,     0,     2,     1,     0,     3,     0,     4,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     3,     1,     1,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     4,
       2,     4,     2,     6,     4,     4,     2,     4,     2,     2,
       1,     0,     1,     1,     1,     1,     1,     1,     3,     3,
       1,     3,     1,     1,     2,     1,     1,     1,     2,     2,
       1,     1,     0,     5,     1,     2,     2,     1,     3,     1,
       1,     3,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     1,     0,     1,     0,
       1,     1,     1,     1,     1,     2,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     0,     1,     0,     0,     0,     0,     0,   278,
       0,     0,   495,   299,   302,     0,   323,   324,   325,   326,
     288,   291,   358,   428,   427,   429,   430,     0,     0,   497,
      21,     0,   432,   431,   422,   485,   424,   423,   425,   426,
     401,   418,   419,   435,   436,     0,     0,     0,     0,     0,
       0,   506,   506,    71,   401,   399,   401,   391,   397,     0,
       0,     0,     3,   495,     7,    35,    36,    44,    43,     0,
      62,     0,    66,    72,     0,    41,   219,     0,    48,   295,
     269,   270,   383,   384,   271,   272,   273,   274,   381,   382,
     380,   433,   275,     0,   276,   254,     6,     9,   323,   324,
     288,   291,   358,   497,    92,    93,     0,     0,     0,     0,
      95,     0,   327,     0,   433,   276,     0,   316,   142,   153,
     143,   166,   139,   159,   149,   148,   164,   147,   146,   141,
     167,   151,   140,   154,   158,   160,   152,   145,   161,   168,
     163,   162,   155,   165,   150,   138,   157,   156,   169,   170,
     171,   172,   173,   137,   144,   135,   136,   133,   134,    97,
      99,    98,   128,   129,   126,   110,   111,   112,   115,   117,
     113,   130,   131,   118,   119,   123,   114,   116,   107,   108,
     109,   120,   121,   122,   124,   125,   127,   132,   472,   318,
     100,   101,   471,     0,   162,   155,   165,   150,   133,   134,
      97,    98,     0,   102,   104,    15,   103,     0,     0,    42,
       0,     0,     0,   433,     0,   276,     0,   503,   502,   495,
       0,   504,   496,     0,     0,     0,   340,   339,     0,     0,
     433,   276,     0,     0,     0,     0,   233,   220,   264,    46,
     241,   506,   506,   477,   479,   480,    47,    45,     0,    61,
       0,   506,   357,    60,    39,     0,    10,   498,   215,     0,
       0,     0,   194,     0,   195,   486,   488,   487,   489,   356,
     284,     0,     0,     0,    62,   280,     0,   497,     0,   499,
     499,   221,   499,     0,   499,   474,     0,    70,     0,    76,
      83,   415,   414,   416,   413,     0,   412,     0,     0,     0,
       0,     0,     0,   420,   421,    40,   209,   210,     5,   496,
       0,     0,     0,     0,     0,     0,     0,   345,   348,     0,
      74,     0,    69,    67,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     217,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   506,     0,     0,     0,     0,   362,   359,   296,
     385,     0,     0,   351,    54,   294,     0,   313,    92,    93,
      94,   420,   421,     0,   438,   311,   437,     0,   506,     0,
       0,     0,   451,   494,   493,   320,   105,     0,   506,   284,
     330,   329,     0,   328,     0,     0,   506,     0,     0,     0,
       0,     0,     0,     0,     0,   505,     0,     0,   284,     0,
     506,     0,   308,   482,   506,   261,     0,     0,   234,   263,
       0,   236,   290,     0,   257,   256,   255,   233,   497,   506,
     497,     0,    12,    14,    13,     0,   292,     0,     0,     0,
     403,   406,   404,   387,   402,     0,     0,     0,     0,   282,
      63,   497,   497,   222,   286,   501,   500,   223,   501,   225,
     501,   476,   287,   475,    82,     0,   506,     0,   417,   386,
     400,   388,   389,   390,     0,   393,   395,     0,   396,     0,
       0,     8,    16,    17,    18,    19,    20,    37,    38,   506,
       0,    25,    33,     0,    34,     0,    68,    75,    24,   174,
     264,    32,   191,   199,   204,   205,   206,   201,   203,   213,
     214,   207,   208,   184,   185,   211,   212,     0,   200,   202,
     196,   197,   198,   186,   187,   188,   189,   190,   486,   491,
     487,   492,   254,   354,     0,   486,   487,   506,   355,   254,
     254,   506,   506,    26,   176,    31,   183,    51,    55,     0,
     440,     0,     0,    92,    93,    96,     0,     0,   506,     0,
     497,   456,   454,   453,   452,   455,   463,   467,   466,   462,
     451,     0,     0,   457,   506,   460,   506,   465,   506,     0,
     450,     0,     0,   279,   506,   506,   372,   506,   331,   174,
     490,   283,     0,   486,   487,   506,     0,     0,     0,   366,
       0,   306,   334,   333,   300,   332,   303,   490,   283,     0,
     486,   487,     0,     0,   240,   481,     0,   265,   262,   506,
       0,     0,   478,   289,     0,    41,     0,   259,     0,   253,
     506,     0,     0,     0,     0,     0,   227,    11,     0,   216,
       0,    23,     0,     0,   182,    63,     0,   226,     0,   265,
     490,    81,     0,   486,   487,   392,   394,   398,   192,   193,
     343,     0,   346,   341,   268,     0,    73,     0,     0,     0,
       0,   353,    58,   285,     0,     0,   232,   352,    56,   231,
     350,    50,   349,    49,   363,   360,   506,   314,     0,     0,
     285,   317,     0,     0,   497,     0,   442,     0,   446,   470,
       0,   448,   464,     0,   449,   468,   321,   106,   373,   374,
     506,   375,     0,   506,   337,     0,     0,   335,     0,   285,
       0,     0,     0,   305,   307,   368,     0,     0,     0,     0,
     285,     0,   506,     0,   238,   506,   506,     0,     0,   258,
       0,   246,   228,     0,   497,   506,   506,   229,     0,    22,
       0,   408,   409,   410,   405,   411,   281,   497,    77,   342,
       0,     0,     0,   266,   175,     0,    30,   180,   181,    59,
       0,    28,   178,    29,   179,    57,     0,     0,    52,     0,
     439,   312,   473,   459,     0,   319,   458,   506,   506,   469,
       0,   461,   506,   451,     0,     0,   377,   338,     0,     4,
     379,     0,   297,     0,   298,     0,   506,     0,     0,   309,
     235,     0,   237,   252,     0,   243,   506,   506,   260,     0,
       0,   293,   407,   224,   344,     0,   267,   218,    27,   177,
       0,     0,     0,     0,   441,     0,   444,   445,   447,     0,
       0,   376,     0,    84,    91,     0,   378,     0,   367,   369,
     370,   365,   301,   304,     0,   506,   506,     0,   242,     0,
     248,   506,   230,   347,   364,   361,     0,   315,   506,     0,
      90,     0,   506,     0,   506,   506,     0,   239,   244,     0,
     506,     0,   247,    53,   443,   322,   490,    89,     0,   486,
     487,   371,   336,   310,   506,     0,   249,   506,    85,   245,
       0,   250,   506,   251
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,   387,   388,    63,    64,   435,   259,    65,
     210,    66,    67,   548,   686,   832,    68,    69,   273,    70,
      71,    72,    73,   211,   110,   111,   203,   204,   205,   582,
     268,   191,    75,   437,   517,   238,   278,   676,   677,   239,
     627,   249,   250,   426,   628,   738,   618,   418,   279,   494,
      76,   207,   451,   638,   223,   728,   224,   729,   612,   854,
     552,   549,   779,   380,   382,   581,   793,   263,   392,   604,
     716,   717,   229,   662,   318,   489,   761,    78,    79,   365,
     542,   777,   541,   776,   404,   600,   851,   585,   710,   795,
     799,    80,    81,    82,    83,    84,    85,    86,   300,   474,
      87,   302,   297,   261,   444,   643,   642,   754,    88,   296,
      89,    90,   213,    92,   214,   215,   375,   551,   571,   572,
     573,   574,   575,   576,   577,   578,   579,   789,   698,   193,
     381,   283,   280,   243,   244,   245,   116,   556,   533,   385,
     220,   258,   457,   393,   222,    96
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -650
static const yytype_int16 yypact[] =
{
    -650,    88,  2473,  -650,  5874,  8495,  8798,  4318,  5527,  -650,
    7344,  7344,  4196,  -650,  -650,  8596,  6084,  6084,  -650,  -650,
    6084,  4858,  4963,  -650,  -650,  -650,  -650,  7344,  5409,    42,
    -650,   -11,  -650,  -650,  4438,  2586,  -650,  -650,  4543,  -650,
    -650,  -650,  -650,  -650,  -650,  8184,  8184,  2149,   113,  3583,
    7344,  7239,  7449,  8899,  5291,  -650,  -650,   121,   144,   410,
    8289,  8184,  -650,   354,   657,   458,  -650,  -650,   258,   197,
    -650,   213,  8697,  -650,   246,  9728,    -1,   435,    43,    79,
    -650,  -650,   276,  -650,  -650,  -650,  -650,  -650,  -650,  -650,
    -650,    38,  -650,   278,   122,    75,  -650,   657,  -650,  -650,
    -650,   260,   266,    42,   367,   504,  7344,    52,  3708,   466,
    -650,    59,  -650,   452,  -650,  -650,    75,  -650,  -650,  -650,
    -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,
    -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,
    -650,   103,   170,   180,   279,  -650,  -650,  -650,  -650,  -650,
    -650,  -650,  -650,  -650,  -650,  -650,  -650,   297,   323,   327,
    -650,   373,  -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,
    -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,
    -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,
    -650,  -650,  -650,   402,  -650,  -650,  -650,  -650,  -650,  -650,
    -650,  -650,  5291,  -650,  -650,   271,  -650,  2978,   358,   458,
      55,   357,   496,    21,   391,    34,    55,  -650,  -650,   354,
     140,  -650,   341,  7344,  7344,   436,  -650,  -650,   537,   475,
      96,   120,  8184,  8184,  8184,  8184,  -650,  9728,   421,  -650,
    -650,   390,   403,  -650,  -650,  -650,  -650,  -650,  5764,  -650,
    6189,  6084,  -650,  -650,  -650,   176,  -650,  -650,   416,   425,
    3833,   265,  -650,   541,   482,  -650,  -650,  -650,  -650,  -650,
     264,  3583,   406,   423,   424,   458,  8184,    42,   427,   -14,
      19,  -650,    37,   434,    19,  -650,   508,  -650,   555,   563,
     585,  -650,  -650,  -650,  -650,   283,  -650,   381,   182,   461,
     411,   464,    12,   502,   519,  -650,  -650,  -650,  -650,  4077,
    7344,  7344,  7344,  7344,  5874,  7344,  7344,  -650,  -650,  7554,
    -650,  3583,  8899,   447,  7554,  8184,  8184,  8184,  8184,  8184,
    8184,  8184,  8184,  8184,  8184,  8184,  8184,  8184,  8184,  8184,
    -650,  8184,  8184,  8184,  8184,  8184,  8184,  8184,  8184,  8184,
    8184,  2223,  7239,  2341,  2149,  2149,  2149,  -650,  -650,  -650,
    -650,  8289,  8289,  -650,   494,  -650,   416,   458,  -650,   596,
    -650,  -650,  -650,   354,  -650,  -650,  -650,  9052,  7239,  2149,
    2978,  7344,   819,  -650,  -650,  -650,  -650,   579,   583,    73,
    -650,  -650,  3103,   580,  8184,  9120,  7239,  9188,  8184,  8184,
    3343,   575,  3958,  7659,   587,  -650,    57,    57,   135,  9256,
    7239,  9324,  -650,  -650,   471,  -650,  8184,  6294,  -650,  -650,
    6399,  -650,  -650,   480,  5979,  -650,  -650,   258,    42,   478,
      28,   492,  -650,  -650,  -650,  5527,  -650,  8184,  3833,   506,
    -650,  -650,  -650,  -650,  -650,  9120,  9188,  8184,   510,  -650,
     503,    42,  9659,  -650,  -650,  7764,  -650,  -650,  7449,  -650,
    8184,  -650,  -650,  -650,   596,  9392,  7239,  9460,  -650,  -650,
    -650,  -650,  -650,  -650,    39,  -650,  -650,   509,  -650,  8184,
    8184,   657,  -650,  -650,  -650,  -650,  -650,  -650,  -650,   156,
    8184,  -650,   515,   522,  -650,   525,  -650,  -650,  -650,  1581,
    -650,  -650,   482,  1715,  1715,  1715,  1715,   957,   957,  9796,
    9779,  1715,  1715,  1822,  1822,    18,    18,  8184,   957,   957,
     870,   870,   389,    33,    33,   482,   482,   482,  2715,  5068,
    2844,  5173,   266,  -650,   517,   267,   317,  4963,  -650,  1464,
    1464,   156,   156,  -650,  9728,  -650,  9728,  -650,  -650,   354,
    -650,  7344,  2978,   514,   479,  -650,   266,   530,   266,   650,
      44,   557,  -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,
     886,  2978,   354,  -650,   536,  -650,   538,   620,   543,   623,
    -650,  5645,  5527,  -650,  7869,   659,  -650,   523,  -650,  2081,
    4648,  4753,   550,   196,   254,   659,   672,   675,  8184,   578,
      55,  -650,  -650,  -650,  -650,  -650,  -650,    84,    85,   582,
     147,   149,  7344,   613,  -650,  -650,  8184,   421,  -650,   590,
    8184,   421,  -650,  -650,  8184,  2694,    16,  -650,   592,  -650,
     595,   598,  6504,  2149,  2149,   605,  -650,  -650,  7344,  9728,
     610,  -650,  3833,   770,  9728,   417,   607,  -650,  8184,  -650,
      93,   479,   611,    53,   128,  -650,  -650,  -650,   482,   482,
    -650,  8394,  -650,  -650,  -650,  7974,  -650,  8184,  9711,  8289,
    8184,  -650,   494,   368,  8289,  8289,  -650,  -650,   494,  -650,
    -650,  -650,  -650,  -650,  -650,  -650,   156,  -650,   354,   727,
    -650,  -650,   617,  8184,    42,   730,  -650,   886,  -650,  -650,
     315,  -650,  -650,    -4,  -650,  -650,  -650,  -650,   515,  -650,
     658,  -650,  3463,   738,  -650,  7344,   740,  -650,  8184,   293,
    8184,  8184,   741,  -650,  -650,  -650,  8079,  3223,  3958,  3958,
     151,    57,   471,  6609,  -650,   471,   471,  6714,   621,  -650,
    6819,  -650,  -650,   258,    28,   266,   266,  -650,    80,  -650,
     629,  -650,  -650,  -650,  -650,  -650,  -650,  9659,   646,  -650,
     651,  3958,  8184,   635,  9728,  8184,  -650,  9728,  9728,  -650,
    8289,  -650,  9728,  -650,  9728,  -650,  3958,  3833,  -650,  2978,
    -650,  -650,  -650,  -650,   630,  -650,  -650,   636,   543,  -650,
     557,  -650,   543,   819,  9000,    55,  -650,  -650,  3958,  -650,
    -650,    55,  -650,  8184,  -650,  8184,   426,   756,   758,  -650,
    -650,  8184,  -650,  -650,  8184,  -650,   641,   652,  -650,  8184,
     654,  -650,  -650,  -650,  -650,   764,  -650,  9728,  -650,  9728,
     772,   660,  3833,   774,  -650,   315,  -650,  -650,  -650,  2978,
     731,  -650,   647,   563,   585,  2978,  -650,  3103,  -650,  -650,
    -650,  -650,  -650,  -650,  3958,  2823,   471,  6924,  -650,  7029,
    -650,   471,  -650,  -650,  -650,  -650,   669,  -650,   543,   783,
     596,  9528,  7239,  9596,   583,   523,   786,  -650,  -650,  8184,
     674,  8184,  -650,  -650,  -650,  -650,    49,   479,   679,    66,
      77,  -650,  -650,  -650,   471,  7134,  -650,   471,   646,  -650,
    8184,  -650,   471,  -650
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -650,  -650,  -650,  -313,    14,  -650,    29,  -650,  -650,    45,
      64,   -57,  -650,  -407,  -650,  -650,    -8,   -13,  -175,   -24,
     734,  -650,   -27,   784,   -87,   801,     0,   -22,  -650,  -650,
      20,  -650,   -16,  -650,  -650,  1588,  -337,    -3,  -446,    41,
    -650,     2,  -650,  -650,  -650,  -650,    22,    65,    30,  -290,
      17,  -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,  -650,
    -650,  -650,  -650,  -650,  -650,  -650,  -650,   505,  -211,  -394,
     -67,  -508,   148,  -477,  -650,  -650,  -650,  -241,  -650,   749,
    -650,  -650,  -650,  -650,  -387,  -650,  -650,   -64,  -650,  -650,
    -650,  -650,  -650,  -650,   750,  -650,  -650,  -650,  -650,  -650,
    -650,  -650,  -650,   405,  -248,  -650,  -650,  -650,     3,  -650,
    -650,  -650,   686,   806,  1286,  1147,  -650,  -650,    46,   268,
     137,  -650,  -649,   138,  -650,  -631,  -650,  -345,  -440,  -650,
    -650,  -650,     1,  -316,  -650,  -650,  1001,    81,  -650,  -650,
     -23,   143,   290,    74,   773,  1492
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -507
static const yytype_int16 yytable[] =
{
     237,   237,   226,   305,   237,   400,   256,   189,   236,   236,
     430,   206,   236,   606,   596,   534,    62,   242,   242,   252,
     370,   242,   112,   112,   253,   274,   287,   190,   190,   262,
     264,   206,   112,    97,   501,   237,   237,   580,   240,   240,
     308,   557,   240,   277,   306,   307,   241,   241,   190,   470,
     241,   791,   475,   284,   294,   209,   209,   209,   246,   592,
     373,   247,   -87,   272,   684,   685,   788,   559,   390,   792,
     112,   477,   254,   609,   190,   216,   219,   713,   -78,   -86,
     315,   316,   282,  -327,   602,   325,   221,   722,     3,   112,
     -88,   567,   363,   680,   682,   275,   448,   364,   440,   252,
     325,   357,   368,   416,   622,   357,   369,  -434,   568,   -87,
     -89,   260,   633,   363,   455,   478,   315,   316,   -79,   456,
     399,   -84,   272,  -327,  -434,  -327,   -84,   355,   269,   652,
    -327,   346,   347,   348,   349,   350,   701,   221,   704,   -91,
     441,   442,   622,   -84,   737,   -91,   495,   458,   348,   349,
     350,   367,   456,   -80,   634,   402,   -87,   403,   -78,   447,
     -90,   257,   391,   -78,   603,   460,   -76,   270,   374,   356,
     456,   655,   -86,   -86,   -88,   257,   -85,   257,   -90,  -486,
     -78,   -78,   -87,   -87,   -88,   376,   791,  -428,   217,   218,
     217,   218,   217,   218,   -87,   -89,   401,   358,   -79,   -86,
     -86,   358,   294,   -79,   868,   251,   -84,   821,   362,   778,
     -88,   -88,   -79,   -81,  -486,  -487,   237,   237,   237,   306,
     -79,   -79,   190,  -486,   -76,   580,   656,   -91,   432,  -428,
     -91,   660,   237,   -80,   237,   237,   433,   434,   -80,   689,
     236,   440,   236,   427,   471,   -90,   366,   274,   -83,   242,
     -83,   242,   242,   299,  -427,   -80,   -80,   -86,   695,   -88,
     452,   -85,   491,   -82,  -429,   769,   661,   498,   209,   209,
     240,   775,   240,   240,   439,   -78,   301,   -80,   241,   -77,
     241,   429,   720,   441,   442,   272,   317,   406,   407,   423,
     555,   425,   428,   221,   709,   496,  -427,   274,   849,   680,
     682,   -86,   319,   237,   543,   545,  -429,   421,   499,   502,
     503,   504,   505,   506,   507,   508,   509,   510,   511,   512,
     513,   514,   515,   516,   440,   518,   519,   520,   521,   522,
     523,   524,   525,   526,   527,   272,   237,   809,   481,   112,
     721,   320,   440,   486,   277,   544,   546,   836,   837,   493,
     447,   324,   838,   674,   493,   209,   209,   209,   209,   -88,
     487,   488,   237,  -430,   361,   790,   441,   442,   443,   -90,
     277,   531,   -86,    55,   482,   483,   484,   485,   589,   803,
     237,  -432,   544,   546,   441,   442,   468,   237,   277,   727,
     248,   744,   -82,  -483,   237,   -78,   251,   531,   -85,   386,
     237,   237,   277,   675,   237,  -430,   587,  -431,   625,   566,
     567,  -422,   389,   637,   595,   531,   597,   622,   619,   850,
     453,   639,   -88,  -432,   622,   630,   560,   568,   884,   531,
     569,   644,   532,   599,   537,   538,   539,   540,   206,   237,
     440,   712,   237,   403,   237,   -80,   629,   550,   580,  -431,
     237,  -422,   640,  -422,   770,   190,   325,  -425,   277,   295,
     558,   298,   394,   658,   659,   531,   833,   303,   304,   275,
     440,   338,   339,   -85,   237,   405,   532,   398,   537,   614,
     605,   605,   441,   442,   469,   531,   383,   217,   218,  -483,
     408,  -422,   558,  -422,   315,   316,   -77,  -483,  -422,  -425,
     412,   668,   346,   347,   348,   349,   350,    77,   416,    77,
     113,   113,   441,   442,   473,   212,   212,   212,   417,   351,
     228,   212,   212,   371,   372,   212,   869,   558,   384,   671,
    -484,   420,   212,   449,   672,   888,   377,   715,   712,   678,
    -490,   681,   683,   622,   -65,   -73,   436,   438,   558,   325,
     450,   -64,   454,   671,    77,   212,   212,   462,   288,   352,
     707,   353,   464,  -283,   472,   212,   354,   476,   237,   479,
     459,   631,   461,   635,   463,   497,   378,   288,   379,   622,
     395,   706,   237,   354,   845,   206,   480,   547,  -425,   583,
     847,   584,   403,   588,   646,   647,   209,   601,  -490,   613,
     237,   190,   190,  -283,   237,  -283,   632,   623,   237,  -487,
    -283,   212,   766,    77,   493,   688,   237,   771,   773,   636,
     396,   409,   397,   687,   743,   445,  -484,   354,  -425,   641,
    -425,   -73,   757,   619,  -484,  -425,  -490,   645,  -490,   465,
    -490,   657,   673,  -264,  -486,  -490,   696,  -433,   226,   237,
     665,   764,   666,   767,   768,   690,   750,   209,   772,   774,
     691,   410,   693,   411,   697,   396,   700,   446,   354,  -276,
     702,   703,   354,   705,   712,   719,   731,   237,   112,   466,
    -284,   467,   723,   748,   734,   724,   354,  -433,    91,  -433,
      91,   114,   114,   114,  -433,   741,   310,   311,   312,   313,
     314,   230,   767,   692,   772,   774,   726,   730,   234,  -276,
     237,  -276,    77,   828,   745,   746,  -276,   237,   733,   739,
    -284,   237,  -284,   740,   237,   742,   797,  -284,   212,   212,
    -285,   871,   747,   749,   756,    91,   758,   781,   817,   289,
     785,   806,   807,   808,   782,   794,   237,   798,   818,   827,
     802,   804,   822,   212,   829,   212,   212,   834,   289,   815,
     209,   824,   780,  -265,   835,    77,   852,   816,   853,   857,
    -285,   872,  -285,   873,   863,   825,    77,  -285,   354,   801,
     859,   862,   864,   865,   867,   870,    74,   829,    74,   237,
     830,   831,   883,   885,    91,   855,   893,   810,   237,   227,
     812,   813,   895,   237,   898,   605,   323,   117,   892,   760,
     891,   112,   846,   192,    77,   212,   212,   212,   212,    77,
     212,   212,   751,   752,   212,   753,    77,   288,   359,   212,
      43,    44,   360,    74,   786,   787,   309,   784,   694,   839,
       0,   237,     0,   237,     0,     0,   866,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   237,   212,   880,   874,
       0,   875,     0,   237,   277,   237,   212,   212,   876,   561,
       0,   562,   563,   564,   565,     0,     0,     0,     0,   237,
       0,   858,   860,   212,   237,    77,   212,   820,     0,     0,
       0,   531,    74,    91,     0,     0,     0,    77,     0,     0,
     823,   212,     0,     0,     0,    77,     0,    77,     0,     0,
       0,     0,     0,   566,   567,   212,     0,     0,     0,     0,
     877,   878,     0,     0,     0,     0,   882,     0,     0,   212,
       0,   568,     0,     0,   569,     0,   561,   325,   562,   563,
     564,   565,     0,    77,     0,   896,    91,     0,     0,   570,
       0,     0,   338,   339,   558,     0,     0,    91,     0,   899,
       0,     0,   901,     0,     0,     0,     0,   903,     0,     0,
       0,   212,     0,     0,     0,     0,     0,     0,     0,     0,
     566,   567,   345,   346,   347,   348,   349,   350,     0,     0,
       0,    74,     0,     0,     0,    91,     0,     0,   568,     0,
      91,   569,     0,    95,     0,    95,     0,    91,   289,     0,
       0,    95,    95,    95,     0,     0,     0,    95,    95,     0,
       0,    95,     0,     0,   325,     0,     0,     0,    95,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   338,
     339,     0,     0,     0,    74,     0,     0,     0,     0,     0,
      95,    95,    95,     0,     0,    74,   212,    77,     0,     0,
       0,    95,     0,     0,     0,     0,    91,   343,   344,   345,
     346,   347,   348,   349,   350,     0,    77,     0,    91,     0,
       0,     0,     0,     0,     0,     0,    91,     0,    91,     0,
       0,     0,     0,    74,     0,     0,     0,     0,    74,     0,
       0,     0,     0,     0,     0,    74,     0,    95,     0,    95,
       0,     0,     0,     0,     0,     0,     0,   212,     0,     0,
       0,     0,     0,     0,    91,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   212,     0,     0,
       0,     0,     0,   212,     0,     0,     0,    77,     0,    94,
       0,    94,   115,   115,     0,     0,     0,     0,     0,     0,
       0,     0,   231,     0,    74,     0,   228,     0,     0,     0,
       0,     0,     0,     0,   212,     0,    74,     0,     0,   212,
     212,     0,     0,     0,    74,     0,    74,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    94,     0,     0,     0,
     290,     0,     0,     0,     0,     0,     0,     0,    95,     0,
       0,     0,     0,     0,     0,     0,     0,    77,     0,   290,
     212,     0,    74,     0,    95,    95,     0,     0,     0,     0,
       0,     0,    77,    77,    77,     0,     0,     0,    91,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
       0,    95,    95,     0,     0,    94,     0,    91,     0,     0,
       0,    95,     0,     0,     0,     0,    77,     0,     0,     0,
       0,     0,    95,     0,     0,   212,     0,     0,     0,     0,
       0,    77,    77,     0,    77,     0,     0,     0,    93,     0,
      93,     0,     0,     0,     0,     0,     0,     0,     0,   842,
       0,     0,     0,    77,     0,     0,     0,     0,     0,     0,
      95,    95,    95,    95,    95,    95,    95,    95,     0,     0,
      95,     0,    95,     0,     0,    95,     0,     0,    91,     0,
       0,     0,     0,     0,     0,    93,    74,    77,     0,     0,
       0,     0,     0,     0,    77,     0,     0,   230,     0,     0,
      77,     0,    77,    95,    94,    74,     0,     0,     0,    77,
       0,     0,    95,    95,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   212,     0,    95,
       0,    95,    95,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    95,    93,     0,     0,    95,    91,     0,
       0,    95,     0,    95,     0,     0,     0,    94,     0,     0,
       0,    95,     0,    91,    91,    91,     0,     0,    94,     0,
       0,     0,     0,     0,     0,    95,    74,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
       0,     0,     0,     0,     0,   227,     0,    91,     0,     0,
       0,     0,     0,     0,     0,     0,    94,     0,     0,     0,
       0,    94,    91,    91,  -506,    91,     0,    95,    94,   290,
       0,     0,  -506,  -506,  -506,     0,     0,  -506,  -506,  -506,
     843,  -506,     0,     0,    91,     0,     0,     0,     0,     0,
       0,  -506,     0,    93,     0,     0,    74,     0,     0,     0,
    -506,  -506,     0,  -506,  -506,  -506,  -506,  -506,     0,     0,
       0,    74,    74,    74,     0,     0,     0,     0,    91,     0,
       0,     0,     0,     0,     0,    91,     0,    94,     0,     0,
       0,    91,     0,    91,     0,     0,     0,     0,     0,    94,
      91,     0,     0,   281,   285,    74,    93,    94,  -506,    94,
       0,     0,    95,    95,     0,     0,     0,    93,     0,     0,
      74,    74,     0,    74,     0,     0,     0,     0,     0,     0,
       0,  -506,    95,     0,     0,     0,     0,     0,   841,     0,
       0,     0,    74,     0,     0,    94,     0,  -506,     0,     0,
    -506,  -506,     0,     0,   251,    93,     0,  -506,  -506,     0,
      93,     0,     0,     0,     0,     0,     0,    93,     0,     0,
       0,     0,     0,    95,     0,     0,    74,     0,     0,     0,
       0,     0,     0,    74,   667,     0,     0,     0,     0,    74,
       0,    74,     0,    95,     0,     0,     0,     0,    74,    95,
       0,     0,     0,    95,     0,     0,     0,     0,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,     0,     0,   338,   339,     0,    93,     0,     0,     0,
      95,     0,     0,     0,     0,    95,    95,     0,    93,     0,
       0,     0,     0,     0,     0,     0,    93,   340,    93,   341,
     342,   343,   344,   345,   346,   347,   348,   349,   350,    94,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -220,
       0,     0,     0,    95,     0,     0,    95,     0,    94,     0,
       0,     0,     0,     0,    93,     0,     0,     0,    95,    95,
      95,     0,     0,   419,   419,     0,     0,     0,     0,     0,
       0,     0,     0,   431,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    95,     0,     0,     0,     0,     0,     0,     0,
       0,    95,     0,     0,     0,     0,     0,    95,    95,     0,
      95,     0,   325,  -507,  -507,  -507,  -507,   330,   331,    94,
     755,  -507,  -507,     0,     0,     0,     0,   338,   339,    95,
       0,     0,     0,     0,     0,     0,     0,     0,   231,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     413,   414,   415,   341,   342,   343,   344,   345,   346,   347,
     348,   349,   350,    95,     0,     0,     0,     0,    93,     0,
      95,     0,     0,     0,   281,     0,    95,     0,    95,     0,
       0,     0,     0,     0,     0,    95,     0,    93,     0,    94,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     281,     0,     0,    95,    94,    94,    94,     0,     0,     0,
     586,     0,     0,     0,     0,     0,     0,     0,   281,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
    -507,  -507,   281,     0,   338,   339,   419,   492,    94,     0,
       0,     0,   500,     0,     0,     0,     0,     0,     0,     0,
       0,   419,     0,    94,    94,     0,    94,     0,    93,     0,
     341,   342,   343,   344,   345,   346,   347,   348,   349,   350,
       0,   844,     0,     0,     0,    94,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   281,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    94,
       0,   663,     0,     0,     0,     0,    94,     0,     0,     0,
       0,   500,    94,     0,    94,     0,     0,     0,    93,     0,
       0,    94,     0,     0,   615,   617,     0,     0,   621,     0,
       0,     0,   626,    93,    93,    93,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   679,
       0,   679,   679,   663,   663,     0,     0,     0,     0,     0,
       0,     0,     0,   649,     0,     0,   621,    93,   649,     0,
     679,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    93,    93,     0,    93,   699,     0,   699,     0,
     699,     0,     0,     0,     0,     0,   711,   714,   664,   714,
       0,     0,     0,     0,    93,     0,     0,   714,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   419,     0,     0,     0,     0,     0,     0,    93,     0,
       0,     0,   419,     0,   667,    93,     0,     0,     0,     0,
       0,    93,     0,    93,     0,     0,     0,     0,     0,     0,
      93,     0,     0,     0,     0,     0,     0,     0,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,     0,     0,   338,   339,     0,     0,     0,     0,     0,
       0,     0,   708,     0,     0,     0,     0,     0,   663,     0,
       0,     0,     0,     0,     0,     0,   725,   340,     0,   341,
     342,   343,   344,   345,   346,   347,   348,   349,   350,   265,
     266,     0,   796,   267,   732,   800,     0,     0,   735,     0,
       0,     0,   736,   162,   163,     0,   164,   165,   166,   167,
     617,   168,   169,     0,   419,   170,     0,   419,   419,   171,
     172,   173,   174,     0,     0,     0,     0,   679,   679,     0,
       0,     0,     0,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   763,     0,     0,     0,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,     0,     0,   186,
       0,     0,     0,   528,   529,     0,     0,   530,   187,   699,
     699,   783,     0,     0,   699,     0,     0,   162,   163,     0,
     164,   165,   166,   167,     0,   168,   169,     0,   714,   170,
       0,     0,     0,   171,   172,   173,   174,     0,   419,   419,
       0,     0,     0,     0,   649,     0,     0,   175,     0,     0,
       0,   621,     0,     0,     0,     0,     0,     0,   621,     0,
       0,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,     0,     0,   186,     0,     0,     0,   419,   419,     0,
     826,     0,   187,   419,     0,     0,     0,     0,     0,     0,
     699,     0,     0,     0,   281,     0,   586,   714,     0,     0,
       0,     0,   419,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   419,     0,     0,   419,
       0,   535,   266,   848,   419,   536,     0,     0,     0,     0,
       0,     0,   856,     0,     0,   162,   163,   861,   164,   165,
     166,   167,     0,   168,   169,     0,     0,   170,     0,     0,
       0,   171,   172,   173,   174,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   175,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   617,     0,   621,     0,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,     0,
       0,   186,     0,     0,     0,     0,     0,   894,     0,   897,
     187,     0,     0,  -506,     4,     0,     5,     6,     7,     8,
       9,     0,     0,   621,    10,    11,     0,     0,   902,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,    43,    44,     0,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,     0,
       0,    49,    50,     0,    51,    52,     0,    53,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -277,     0,     0,     0,
       0,    59,    60,    61,  -277,  -277,  -277,     0,     0,  -277,
    -277,  -277,     0,  -277,     0,     0,  -506,  -506,     0,     0,
       0,     0,     0,  -277,  -277,     0,     0,     0,     0,     0,
       0,     0,  -277,  -277,     0,  -277,  -277,  -277,  -277,  -277,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -277,  -277,  -277,  -277,  -277,  -277,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,     0,     0,  -277,  -277,
    -277,     0,     0,  -277,     0,     0,     0,     0,     0,  -277,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,
    -277,  -277,  -277,  -277,     0,     0,     0,     0,     0,  -277,
    -277,  -277,  -277,  -277,  -277,  -490,     0,  -277,     0,  -277,
    -277,     0,     0,  -490,  -490,  -490,     0,     0,     0,  -490,
    -490,     0,  -490,     0,     0,     0,     0,     0,     0,     0,
       0,  -490,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -490,  -490,     0,  -490,  -490,  -490,  -490,  -490,     0,
       0,   325,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,     0,     0,   338,   339,     0,     0,
       0,  -220,  -490,  -490,  -490,  -490,  -490,  -490,  -490,  -490,
    -490,  -490,  -490,  -490,  -490,     0,     0,  -490,  -490,  -490,
     340,   669,   341,   342,   343,   344,   345,   346,   347,   348,
     349,   350,     0,     0,     0,     0,     0,     0,     0,     0,
     -87,  -490,  -220,  -490,  -490,  -490,  -490,  -490,  -490,  -490,
    -490,  -490,  -490,     0,     0,     0,     0,  -490,  -490,  -490,
       0,  -490,  -490,   -79,  -283,     0,  -490,     0,  -490,  -490,
       0,     0,  -283,  -283,  -283,     0,     0,     0,  -283,  -283,
       0,  -283,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -283,  -283,     0,  -283,  -283,  -283,  -283,  -283,     0,     0,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,     0,     0,   338,   339,     0,     0,     0,
       0,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,  -283,  -283,     0,     0,  -283,  -283,  -283,   340,
     670,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     350,     0,     0,     0,     0,     0,     0,     0,     0,   -89,
    -283,   613,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,     0,     0,     0,     0,     0,  -283,  -283,     0,
    -283,  -283,   -81,     0,     0,  -283,     0,  -283,  -283,     4,
       0,     5,     6,     7,     8,     9,  -506,  -506,  -506,    10,
      11,     0,     0,  -506,    12,     0,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,    27,     0,     0,     0,
       0,     0,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,    43,    44,
       0,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,     0,     0,    49,    50,     0,    51,
      52,     0,    53,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    59,    60,    61,     0,
       0,     0,     0,     0,     4,     0,     5,     6,     7,     8,
       9,  -506,  -506,  -506,    10,    11,     0,  -506,  -506,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,    43,    44,     0,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,     0,
       0,    49,    50,     0,    51,    52,     0,    53,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    59,    60,    61,     4,     0,     5,     6,     7,     8,
       9,     0,     0,  -506,    10,    11,  -506,  -506,  -506,    12,
    -506,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,    43,    44,     0,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,     0,
       0,    49,    50,     0,    51,    52,     0,    53,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    59,    60,    61,     4,     0,     5,     6,     7,     8,
       9,     0,     0,  -506,    10,    11,  -506,  -506,  -506,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,    43,    44,     0,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,     0,
       0,    49,    50,     0,    51,    52,     0,    53,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    59,    60,    61,     4,     0,     5,     6,     7,     8,
       9,     0,  -506,  -506,    10,    11,  -506,  -506,     0,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,    43,    44,     0,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,     0,
       0,    49,    50,     0,    51,    52,     0,    53,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    59,    60,    61,     4,     0,     5,     6,     7,     8,
       9,     0,     0,     0,    10,    11,  -506,  -506,     0,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,    43,    44,     0,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,     0,
       0,   271,    50,     0,    51,    52,     0,    53,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    59,    60,    61,     0,     0,     0,     0,     0,     4,
    -506,     5,     6,     7,     8,     9,  -506,  -506,     0,    10,
      11,     0,     0,     0,    12,     0,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,    27,     0,     0,     0,
       0,     0,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,    43,    44,
       0,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,     0,     0,    49,    50,     0,    51,
      52,     0,    53,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    59,    60,    61,     0,
       0,     0,     0,     0,     4,  -506,     5,     6,     7,     8,
       9,  -506,  -506,     0,    10,    11,     0,     0,     0,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,    43,    44,     0,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,     0,
       0,    49,    50,     0,    51,    52,     0,    53,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    59,    60,    61,     0,     0,  -506,     0,     0,     4,
       0,     5,     6,     7,     8,     9,  -506,  -506,  -506,    10,
      11,     0,     0,     0,    12,     0,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,    27,     0,     0,     0,
       0,     0,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,    43,    44,
       0,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,     0,     0,    49,    50,     0,    51,
      52,     0,    53,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    59,    60,    61,     0,
       5,     6,     7,     8,     9,     0,     0,     0,    10,    11,
       0,  -506,  -506,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,    43,    44,     0,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,     0,     0,    49,    50,     0,    51,    52,
       0,    53,     0,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    59,    60,    61,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,     0,
       0,   405,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,    43,    44,     0,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   208,     0,     0,   108,    50,     0,    51,    52,     0,
       0,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    59,    60,    61,     0,     0,     0,
       0,   118,   119,   120,   121,   122,   123,   124,   125,   217,
     218,   126,   127,   128,   129,   130,     0,     0,   131,   132,
     133,   134,   135,   136,   137,     0,     0,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
      36,    37,   161,    39,     0,     0,     0,     0,     0,     0,
       0,     0,   162,   163,     0,   164,   165,   166,   167,     0,
     168,   169,     0,     0,   170,     0,     0,     0,   171,   172,
     173,   174,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,     0,     0,   186,     0,
       0,  -483,  -483,  -483,     0,  -483,     0,   187,   188,  -483,
    -483,     0,     0,     0,  -483,     0,  -483,  -483,  -483,  -483,
    -483,  -483,  -483,     0,  -483,     0,     0,  -483,  -483,  -483,
    -483,  -483,  -483,  -483,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -483,     0,     0,  -483,  -483,  -483,  -483,
    -483,  -483,  -483,  -483,  -483,  -483,  -483,     0,  -483,  -483,
       0,  -483,  -483,  -483,  -483,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -483,     0,     0,  -483,  -483,     0,  -483,
    -483,     0,  -483,  -483,  -483,  -483,  -483,  -483,  -483,     0,
       0,     0,     0,     0,     0,     0,  -484,  -484,  -484,     0,
    -484,     0,     0,     0,  -484,  -484,  -483,  -483,  -483,  -484,
    -483,  -484,  -484,  -484,  -484,  -484,  -484,  -484,  -483,  -484,
       0,     0,  -484,  -484,  -484,  -484,  -484,  -484,  -484,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -484,     0,
       0,  -484,  -484,  -484,  -484,  -484,  -484,  -484,  -484,  -484,
    -484,  -484,     0,  -484,  -484,     0,  -484,  -484,  -484,  -484,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -484,     0,
       0,  -484,  -484,     0,  -484,  -484,     0,  -484,  -484,  -484,
    -484,  -484,  -484,  -484,     0,     0,     0,     0,     0,     0,
       0,  -486,  -486,  -486,     0,  -486,     0,     0,     0,  -486,
    -486,  -484,  -484,  -484,  -486,  -484,  -486,  -486,  -486,  -486,
    -486,  -486,  -486,  -484,     0,     0,     0,  -486,  -486,  -486,
    -486,  -486,  -486,  -486,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -486,     0,     0,  -486,  -486,  -486,  -486,
    -486,  -486,  -486,  -486,  -486,  -486,  -486,     0,  -486,  -486,
       0,  -486,  -486,  -486,  -486,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -486,   718,     0,  -486,  -486,     0,  -486,
    -486,     0,  -486,  -486,  -486,  -486,  -486,  -486,  -486,     0,
       0,     0,     0,   -87,     0,     0,  -487,  -487,  -487,     0,
    -487,     0,     0,     0,  -487,  -487,  -486,  -486,  -486,  -487,
       0,  -487,  -487,  -487,  -487,  -487,  -487,  -487,  -486,     0,
       0,     0,  -487,  -487,  -487,  -487,  -487,  -487,  -487,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -487,     0,
       0,  -487,  -487,  -487,  -487,  -487,  -487,  -487,  -487,  -487,
    -487,  -487,     0,  -487,  -487,     0,  -487,  -487,  -487,  -487,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -487,   670,
       0,  -487,  -487,     0,  -487,  -487,     0,  -487,  -487,  -487,
    -487,  -487,  -487,  -487,     0,     0,     0,     0,   -89,     0,
       0,  -254,  -254,  -254,     0,  -254,     0,     0,     0,  -254,
    -254,  -487,  -487,  -487,  -254,     0,  -254,  -254,  -254,  -254,
    -254,  -254,  -254,  -487,     0,     0,     0,  -254,  -254,  -254,
    -254,  -254,  -254,  -254,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -254,     0,     0,  -254,  -254,  -254,  -254,
    -254,  -254,  -254,  -254,  -254,  -254,  -254,     0,  -254,  -254,
       0,  -254,  -254,  -254,  -254,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -254,     0,     0,  -254,  -254,     0,  -254,
    -254,     0,  -254,  -254,  -254,  -254,  -254,  -254,  -254,     0,
       0,     0,     0,     0,     0,     0,  -254,  -254,  -254,     0,
    -254,     0,     0,     0,  -254,  -254,  -254,  -254,  -254,  -254,
       0,  -254,  -254,  -254,  -254,  -254,  -254,  -254,   248,     0,
       0,     0,  -254,  -254,  -254,  -254,  -254,  -254,  -254,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -254,     0,
       0,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,
    -254,  -254,     0,  -254,  -254,     0,  -254,  -254,  -254,  -254,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -254,     0,
       0,  -254,  -254,     0,  -254,  -254,     0,  -254,  -254,  -254,
    -254,  -254,  -254,  -254,     0,     0,     0,     0,     0,     0,
       0,  -488,  -488,  -488,     0,  -488,     0,     0,     0,  -488,
    -488,  -254,  -254,  -254,  -488,     0,  -488,  -488,  -488,  -488,
    -488,  -488,  -488,   251,     0,     0,     0,  -488,  -488,  -488,
    -488,  -488,  -488,  -488,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -488,     0,     0,  -488,  -488,  -488,  -488,
    -488,  -488,  -488,  -488,  -488,  -488,  -488,     0,  -488,  -488,
       0,  -488,  -488,  -488,  -488,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -488,     0,     0,  -488,  -488,     0,  -488,
    -488,     0,  -488,  -488,  -488,  -488,  -488,  -488,  -488,     0,
       0,     0,     0,     0,     0,     0,  -489,  -489,  -489,     0,
    -489,     0,     0,     0,  -489,  -489,  -488,  -488,  -488,  -489,
       0,  -489,  -489,  -489,  -489,  -489,  -489,  -489,  -488,     0,
       0,     0,  -489,  -489,  -489,  -489,  -489,  -489,  -489,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -489,     0,
       0,  -489,  -489,  -489,  -489,  -489,  -489,  -489,  -489,  -489,
    -489,  -489,     0,  -489,  -489,     0,  -489,  -489,  -489,  -489,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -489,     0,
       0,  -489,  -489,     0,  -489,  -489,     0,  -489,  -489,  -489,
    -489,  -489,  -489,  -489,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -489,  -489,  -489,   118,   119,   120,   121,   122,   123,
     124,   125,     0,  -489,   126,   127,   128,   129,   130,     0,
       0,   131,   132,   133,   134,   135,   136,   137,     0,     0,
     138,   139,   140,   194,   195,   196,   197,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   198,
     199,   200,   160,   291,   292,   201,   293,     0,     0,     0,
       0,     0,     0,     0,     0,   162,   163,     0,   164,   165,
     166,   167,     0,   168,   169,     0,     0,   170,     0,     0,
       0,   171,   172,   173,   174,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   175,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,     0,
       0,   186,   118,   119,   120,   121,   122,   123,   124,   125,
     187,     0,   126,   127,   128,   129,   130,     0,     0,   131,
     132,   133,   134,   135,   136,   137,     0,     0,   138,   139,
     140,   194,   195,   196,   197,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   198,   199,   200,
     160,   255,     0,   201,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   162,   163,     0,   164,   165,   166,   167,
       0,   168,   169,     0,     0,   170,     0,     0,     0,   171,
     172,   173,   174,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   175,     0,   202,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,     0,     0,   186,
     118,   119,   120,   121,   122,   123,   124,   125,   187,     0,
     126,   127,   128,   129,   130,     0,     0,   131,   132,   133,
     134,   135,   136,   137,     0,     0,   138,   139,   140,   194,
     195,   196,   197,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   198,   199,   200,   160,     0,
       0,   201,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   162,   163,     0,   164,   165,   166,   167,     0,   168,
     169,     0,     0,   170,     0,     0,     0,   171,   172,   173,
     174,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   175,     0,   202,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,     0,     0,   186,   118,   119,
     120,   121,   122,   123,   124,   125,   187,     0,   126,   127,
     128,   129,   130,     0,     0,   131,   132,   133,   134,   135,
     136,   137,     0,     0,   138,   139,   140,   194,   195,   196,
     197,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   198,   199,   200,   160,     0,     0,   201,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   162,
     163,     0,   164,   165,   166,   167,     0,   168,   169,     0,
       0,   170,     0,     0,     0,   171,   172,   173,   174,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   175,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,     0,     0,   186,     0,     5,     6,     7,
       0,     9,     0,     0,   187,    10,    11,     0,     0,     0,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,    21,    22,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,   232,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   208,
       0,     0,   108,    50,     0,    51,    52,     0,   233,   234,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       8,     9,    59,   235,    61,    10,    11,     0,     0,     0,
      12,   422,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,    49,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,    60,    61,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,   232,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   208,     0,     0,   108,    50,     0,
      51,    52,     0,   624,   234,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,    60,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,    21,    22,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,   232,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   208,
       0,     0,   108,    50,     0,    51,    52,     0,   233,   234,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,    21,
      22,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,   232,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   208,     0,     0,   108,   424,     0,
      51,    52,     0,   233,   234,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   235,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,   232,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   208,
       0,     0,   108,    50,     0,    51,    52,     0,   616,   234,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,   232,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   208,     0,     0,   108,    50,     0,
      51,    52,     0,   620,   234,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   235,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,    21,    22,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,   232,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   208,
       0,     0,   108,    50,     0,    51,    52,     0,   616,   234,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,   232,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   208,     0,     0,   108,    50,     0,
      51,    52,     0,   811,   234,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   235,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,   232,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   208,
       0,     0,   108,    50,     0,    51,    52,     0,   814,   234,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,   232,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   208,     0,     0,   108,    50,     0,
      51,    52,     0,   819,   234,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   235,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,   232,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   208,
       0,     0,   108,    50,     0,    51,    52,     0,   879,   234,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,   232,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   208,     0,     0,   108,    50,     0,
      51,    52,     0,   881,   234,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   235,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,   232,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   208,
       0,     0,   108,    50,     0,    51,    52,     0,   900,   234,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,    21,
      22,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,   232,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   208,     0,     0,   108,    50,     0,
      51,    52,     0,   276,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   235,    61,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   208,
       0,     0,   108,    50,     0,    51,    52,     0,     0,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,    60,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,   232,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   208,     0,     0,   108,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   235,    61,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   208,
       0,     0,   108,    50,     0,    51,    52,     0,   490,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   208,     0,     0,   108,    50,     0,
      51,    52,     0,   598,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   235,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   208,
       0,     0,   108,    50,     0,    51,    52,     0,   648,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   208,     0,     0,   108,    50,     0,
      51,    52,     0,   490,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   235,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   208,
       0,     0,   108,    50,     0,    51,    52,     0,   762,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   208,     0,     0,   108,    50,     0,
      51,    52,     0,   805,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   235,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   208,
       0,     0,   108,    50,     0,    51,    52,     0,     0,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    59,   235,    61,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   208,     0,     0,   108,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   235,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   103,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,     0,     0,     0,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   225,
       0,     0,    49,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     5,     6,
       7,     0,     9,     0,   759,     0,    10,    11,     0,     0,
       0,    12,   109,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     103,     0,     0,    32,    33,   104,    35,    36,    37,   105,
      39,    40,    41,    42,     0,    43,    44,     0,     0,     0,
       0,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   106,     0,     0,
     107,     0,     0,   108,    50,     0,    51,    52,     0,     0,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,     0,
       0,     0,    12,   109,    13,    14,    15,    98,    99,    18,
      19,     0,     0,     0,     0,   100,   101,   102,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   103,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,    43,    44,     0,     0,
       0,     0,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   225,     0,     0,    49,    50,     0,    51,    52,     0,
      53,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
       0,     0,     0,    12,   109,    13,    14,    15,    98,    99,
      18,    19,     0,     0,     0,     0,   100,   101,   102,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   103,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,    43,    44,     0,
       0,     0,     0,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   286,     0,     0,   321,    50,     0,    51,    52,
       0,   322,     0,    54,    55,    56,    57,    58,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,     0,     0,     0,    12,   109,    13,    14,    15,    98,
      99,    18,    19,     0,     0,     0,     0,   100,   101,   102,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   103,     0,     0,    32,    33,   104,    35,
      36,    37,   105,    39,    40,    41,    42,     0,    43,    44,
       0,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   107,     0,     0,   108,    50,     0,    51,
      52,     0,     0,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   109,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,     0,     0,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,     0,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   286,     0,     0,   108,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,     0,     0,     0,    12,   109,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   103,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,     0,     0,     0,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   840,     0,     0,   108,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,     0,   553,   529,     0,     0,   554,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   162,   163,   109,   164,
     165,   166,   167,     0,   168,   169,     0,     0,   170,     0,
       0,     0,   171,   172,   173,   174,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   175,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     590,   529,   186,     0,   591,     0,     0,     0,     0,     0,
       0,   187,     0,     0,   162,   163,     0,   164,   165,   166,
     167,     0,   168,   169,     0,     0,   170,     0,     0,     0,
     171,   172,   173,   174,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   175,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   593,   266,
     186,     0,   594,     0,     0,     0,     0,     0,     0,   187,
       0,     0,   162,   163,     0,   164,   165,   166,   167,     0,
     168,   169,     0,     0,   170,     0,     0,     0,   171,   172,
     173,   174,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   607,   529,   186,     0,
     608,     0,     0,     0,     0,     0,     0,   187,     0,     0,
     162,   163,     0,   164,   165,   166,   167,     0,   168,   169,
       0,     0,   170,     0,     0,     0,   171,   172,   173,   174,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     175,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   610,   266,   186,     0,   611,     0,
       0,     0,     0,     0,     0,   187,     0,     0,   162,   163,
       0,   164,   165,   166,   167,     0,   168,   169,     0,     0,
     170,     0,     0,     0,   171,   172,   173,   174,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   175,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   650,   529,   186,     0,   651,     0,     0,     0,
       0,     0,     0,   187,     0,     0,   162,   163,     0,   164,
     165,   166,   167,     0,   168,   169,     0,     0,   170,     0,
       0,     0,   171,   172,   173,   174,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   175,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     653,   266,   186,     0,   654,     0,     0,     0,     0,     0,
       0,   187,     0,     0,   162,   163,     0,   164,   165,   166,
     167,     0,   168,   169,     0,     0,   170,     0,     0,     0,
     171,   172,   173,   174,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   175,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   886,   529,
     186,     0,   887,     0,     0,     0,     0,     0,     0,   187,
       0,     0,   162,   163,     0,   164,   165,   166,   167,     0,
     168,   169,     0,     0,   170,     0,     0,     0,   171,   172,
     173,   174,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   889,   266,   186,     0,
     890,     0,     0,     0,     0,     0,     0,   187,     0,     0,
     162,   163,     0,   164,   165,   166,   167,     0,   168,   169,
       0,     0,   170,     0,     0,     0,   171,   172,   173,   174,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     175,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,     0,     0,   186,     0,     0,     0,
       0,     0,     0,     0,     0,   187,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,     0,
       0,   338,   339,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   340,     0,   341,   342,   343,
     344,   345,   346,   347,   348,   349,   350,     0,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,     0,   257,   338,   339,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,     0,     0,
     338,   339,     0,     0,     0,     0,     0,   340,   765,   341,
     342,   343,   344,   345,   346,   347,   348,   349,   350,     0,
       0,     0,     0,     0,   340,     0,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   325,   326,   327,   328,
     329,   330,   331,   332,     0,   334,   335,     0,     0,     0,
       0,   338,   339,   325,   326,   327,   328,   329,   330,   331,
       0,     0,   334,   335,     0,     0,     0,     0,   338,   339,
       0,     0,     0,     0,     0,     0,     0,   341,   342,   343,
     344,   345,   346,   347,   348,   349,   350,     0,     0,     0,
       0,     0,     0,     0,   341,   342,   343,   344,   345,   346,
     347,   348,   349,   350
};

static const yytype_int16 yycheck[] =
{
      16,    17,    15,    60,    20,   216,    28,     7,    16,    17,
     251,     8,    20,   407,   401,   352,     2,    16,    17,    22,
     107,    20,     5,     6,    22,    49,    53,     7,     8,    45,
      46,    28,    15,     4,   324,    51,    52,   382,    16,    17,
      63,   378,    20,    51,    60,    61,    16,    17,    28,   297,
      20,   700,   300,    52,    54,    10,    11,    12,    17,   396,
       1,    20,    13,    49,   541,   542,   697,   380,    13,   700,
      53,    59,    27,   410,    54,    11,    12,   585,    25,    13,
      36,    37,    52,    84,    27,    67,    12,   595,     0,    72,
      13,    95,    95,   539,   540,    50,   271,    95,    59,   102,
      67,    26,    50,    87,   420,    26,    54,    86,   112,    25,
      25,   122,    84,   116,   128,   103,    36,    37,    25,   133,
      86,    25,   108,   124,    86,   126,   105,    84,    47,   466,
     131,   113,   114,   115,   116,   117,   576,    63,   578,   105,
     101,   102,   458,   105,   128,    25,   321,   128,   115,   116,
     117,   106,   133,    25,   126,    15,   107,    17,   105,    86,
      25,   133,   107,   110,   107,   128,   128,    54,   109,   126,
     133,   132,    25,   107,    25,   133,    25,   133,   105,   130,
     127,   128,   133,   134,   107,   111,   835,    84,   133,   134,
     133,   134,   133,   134,   110,   110,   219,   122,   105,   133,
     134,   122,   202,   110,   835,   130,   110,   127,    86,   686,
     133,   134,   128,   128,   130,   130,   232,   233,   234,   235,
     127,   128,   202,   130,   128,   570,   474,   105,    52,   126,
     110,    75,   248,   105,   250,   251,    60,    61,   110,   552,
     248,    59,   250,   251,    62,   110,   103,   271,   128,   248,
     128,   250,   251,   132,    84,   127,   128,   110,   571,   110,
     276,   110,   319,   128,    84,   672,   110,   324,   223,   224,
     248,   678,   250,   251,   260,   128,   132,   128,   248,   128,
     250,   251,    86,   101,   102,   271,    28,   223,   224,   248,
     377,   250,   251,   219,   584,   322,   126,   321,   806,   745,
     746,   105,   105,   319,   361,   362,   126,   242,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,    59,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   321,   352,   731,   309,   322,
      86,   128,    59,   314,   352,   361,   362,   787,   788,   319,
      86,   105,   792,    86,   324,   310,   311,   312,   313,   105,
     315,   316,   378,    84,    86,    50,   101,   102,   103,   105,
     378,   351,   105,    97,   310,   311,   312,   313,   394,    86,
     396,    84,   398,   399,   101,   102,   103,   403,   396,   600,
     130,   632,   128,    26,   410,   128,   130,   377,   105,   128,
     416,   417,   410,    86,   420,   126,   392,    84,   424,    94,
      95,    84,    54,   435,   400,   395,   402,   733,   417,   806,
     277,   437,   105,   126,   740,   424,   381,   112,   868,   409,
     115,   447,   351,   403,   353,   354,   355,   356,   435,   455,
      59,    15,   458,    17,   460,   128,   424,   373,   793,   126,
     466,    84,   438,   126,    86,   435,    67,    84,   466,    54,
     379,    56,   105,   479,   480,   445,   779,    57,    58,   424,
      59,    82,    83,   105,   490,   134,   395,    86,   397,   414,
     406,   407,   101,   102,   103,   465,    84,   133,   134,   122,
      54,   124,   411,   126,    36,    37,   128,   130,   131,   126,
      25,   517,   113,   114,   115,   116,   117,     2,    87,     4,
       5,     6,   101,   102,   103,    10,    11,    12,   128,    84,
      15,    16,    17,    57,    58,    20,   839,   446,   126,   532,
      26,   128,    27,   127,   532,   872,    84,    14,    15,   537,
      26,   539,   540,   859,   127,   128,   130,   122,   467,    67,
     127,   127,   125,   556,    49,    50,    51,   123,    53,   124,
     582,   126,    54,    84,   103,    60,   131,   103,   584,    67,
     280,   428,   282,   430,   284,   128,   124,    72,   126,   895,
      84,   581,   598,   131,   795,   582,    67,    93,    84,    10,
     801,     8,    17,    13,   451,   452,   551,    10,    84,   128,
     616,   581,   582,   124,   620,   126,   128,   127,   624,   130,
     131,   106,   669,   108,   584,   551,   632,   674,   675,   127,
     124,    84,   126,   549,   632,    84,   122,   131,   124,   123,
     126,   128,   648,   632,   130,   131,   122,   127,   124,    84,
     126,   132,   125,   128,   130,   131,   572,    84,   661,   665,
     128,   667,   127,   669,   670,   125,   642,   612,   674,   675,
      10,   124,   105,   126,   128,   124,   128,   126,   131,    84,
      50,   128,   131,    50,    15,   125,   612,   693,   661,   124,
      84,   126,    10,   638,   619,    10,   131,   124,     2,   126,
       4,     5,     6,     7,   131,   630,    39,    40,    41,    42,
      43,    15,   718,   560,   720,   721,   128,   125,    95,   124,
     726,   126,   207,   770,   633,   634,   131,   733,   128,   127,
     124,   737,   126,   128,   740,   127,   712,   131,   223,   224,
      84,    84,   127,   123,   127,    49,   125,    10,   737,    53,
      10,   727,   728,   729,   127,    87,   762,     9,   127,   765,
      10,    10,   123,   248,   770,   250,   251,   127,    72,   737,
     715,   110,   688,   128,   128,   260,    10,   737,    10,   128,
     124,   124,   126,   126,    10,   761,   271,   131,   131,   715,
     128,   127,    10,   123,    10,    54,     2,   803,     4,   805,
     776,   777,   123,    10,   108,   811,    10,   732,   814,    15,
     735,   736,   128,   819,   125,   731,    72,     6,   875,   661,
     874,   794,   798,     7,   309,   310,   311,   312,   313,   314,
     315,   316,    52,    53,   319,    55,   321,   322,    79,   324,
      60,    61,    82,    49,   697,   697,    63,   694,   570,   793,
      -1,   857,    -1,   859,    -1,    -1,   832,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   872,   352,   857,   845,
      -1,   847,    -1,   879,   872,   881,   361,   362,   854,    50,
      -1,    52,    53,    54,    55,    -1,    -1,    -1,    -1,   895,
      -1,   816,   817,   378,   900,   380,   381,   744,    -1,    -1,
      -1,   871,   108,   207,    -1,    -1,    -1,   392,    -1,    -1,
     757,   396,    -1,    -1,    -1,   400,    -1,   402,    -1,    -1,
      -1,    -1,    -1,    94,    95,   410,    -1,    -1,    -1,    -1,
     855,   856,    -1,    -1,    -1,    -1,   861,    -1,    -1,   424,
      -1,   112,    -1,    -1,   115,    -1,    50,    67,    52,    53,
      54,    55,    -1,   438,    -1,   880,   260,    -1,    -1,   130,
      -1,    -1,    82,    83,   873,    -1,    -1,   271,    -1,   894,
      -1,    -1,   897,    -1,    -1,    -1,    -1,   902,    -1,    -1,
      -1,   466,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    95,   112,   113,   114,   115,   116,   117,    -1,    -1,
      -1,   207,    -1,    -1,    -1,   309,    -1,    -1,   112,    -1,
     314,   115,    -1,     2,    -1,     4,    -1,   321,   322,    -1,
      -1,    10,    11,    12,    -1,    -1,    -1,    16,    17,    -1,
      -1,    20,    -1,    -1,    67,    -1,    -1,    -1,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,
      83,    -1,    -1,    -1,   260,    -1,    -1,    -1,    -1,    -1,
      49,    50,    51,    -1,    -1,   271,   551,   552,    -1,    -1,
      -1,    60,    -1,    -1,    -1,    -1,   380,   110,   111,   112,
     113,   114,   115,   116,   117,    -1,   571,    -1,   392,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   400,    -1,   402,    -1,
      -1,    -1,    -1,   309,    -1,    -1,    -1,    -1,   314,    -1,
      -1,    -1,    -1,    -1,    -1,   321,    -1,   106,    -1,   108,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   612,    -1,    -1,
      -1,    -1,    -1,    -1,   438,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   632,    -1,    -1,
      -1,    -1,    -1,   638,    -1,    -1,    -1,   642,    -1,     2,
      -1,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    15,    -1,   380,    -1,   661,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   669,    -1,   392,    -1,    -1,   674,
     675,    -1,    -1,    -1,   400,    -1,   402,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,
      53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   207,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   712,    -1,    72,
     715,    -1,   438,    -1,   223,   224,    -1,    -1,    -1,    -1,
      -1,    -1,   727,   728,   729,    -1,    -1,    -1,   552,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   248,
      -1,   250,   251,    -1,    -1,   108,    -1,   571,    -1,    -1,
      -1,   260,    -1,    -1,    -1,    -1,   761,    -1,    -1,    -1,
      -1,    -1,   271,    -1,    -1,   770,    -1,    -1,    -1,    -1,
      -1,   776,   777,    -1,   779,    -1,    -1,    -1,     2,    -1,
       4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   794,
      -1,    -1,    -1,   798,    -1,    -1,    -1,    -1,    -1,    -1,
     309,   310,   311,   312,   313,   314,   315,   316,    -1,    -1,
     319,    -1,   321,    -1,    -1,   324,    -1,    -1,   642,    -1,
      -1,    -1,    -1,    -1,    -1,    49,   552,   832,    -1,    -1,
      -1,    -1,    -1,    -1,   839,    -1,    -1,   661,    -1,    -1,
     845,    -1,   847,   352,   207,   571,    -1,    -1,    -1,   854,
      -1,    -1,   361,   362,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   872,    -1,   378,
      -1,   380,   381,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   392,   108,    -1,    -1,   396,   712,    -1,
      -1,   400,    -1,   402,    -1,    -1,    -1,   260,    -1,    -1,
      -1,   410,    -1,   727,   728,   729,    -1,    -1,   271,    -1,
      -1,    -1,    -1,    -1,    -1,   424,   642,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   438,
      -1,    -1,    -1,    -1,    -1,   661,    -1,   761,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   309,    -1,    -1,    -1,
      -1,   314,   776,   777,     0,   779,    -1,   466,   321,   322,
      -1,    -1,     8,     9,    10,    -1,    -1,    13,    14,    15,
     794,    17,    -1,    -1,   798,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    -1,   207,    -1,    -1,   712,    -1,    -1,    -1,
      36,    37,    -1,    39,    40,    41,    42,    43,    -1,    -1,
      -1,   727,   728,   729,    -1,    -1,    -1,    -1,   832,    -1,
      -1,    -1,    -1,    -1,    -1,   839,    -1,   380,    -1,    -1,
      -1,   845,    -1,   847,    -1,    -1,    -1,    -1,    -1,   392,
     854,    -1,    -1,    51,    52,   761,   260,   400,    84,   402,
      -1,    -1,   551,   552,    -1,    -1,    -1,   271,    -1,    -1,
     776,   777,    -1,   779,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   107,   571,    -1,    -1,    -1,    -1,    -1,   794,    -1,
      -1,    -1,   798,    -1,    -1,   438,    -1,   123,    -1,    -1,
     126,   127,    -1,    -1,   130,   309,    -1,   133,   134,    -1,
     314,    -1,    -1,    -1,    -1,    -1,    -1,   321,    -1,    -1,
      -1,    -1,    -1,   612,    -1,    -1,   832,    -1,    -1,    -1,
      -1,    -1,    -1,   839,    43,    -1,    -1,    -1,    -1,   845,
      -1,   847,    -1,   632,    -1,    -1,    -1,    -1,   854,   638,
      -1,    -1,    -1,   642,    -1,    -1,    -1,    -1,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    -1,    -1,    82,    83,    -1,   380,    -1,    -1,    -1,
     669,    -1,    -1,    -1,    -1,   674,   675,    -1,   392,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   400,   106,   402,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   552,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,
      -1,    -1,    -1,   712,    -1,    -1,   715,    -1,   571,    -1,
      -1,    -1,    -1,    -1,   438,    -1,    -1,    -1,   727,   728,
     729,    -1,    -1,   241,   242,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   251,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   761,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   770,    -1,    -1,    -1,    -1,    -1,   776,   777,    -1,
     779,    -1,    67,    68,    69,    70,    71,    72,    73,   642,
     643,    76,    77,    -1,    -1,    -1,    -1,    82,    83,   798,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   661,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     232,   233,   234,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   832,    -1,    -1,    -1,    -1,   552,    -1,
     839,    -1,    -1,    -1,   352,    -1,   845,    -1,   847,    -1,
      -1,    -1,    -1,    -1,    -1,   854,    -1,   571,    -1,   712,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     378,    -1,    -1,   872,   727,   728,   729,    -1,    -1,    -1,
     388,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   396,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,   410,    -1,    82,    83,   414,   319,   761,    -1,
      -1,    -1,   324,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   429,    -1,   776,   777,    -1,   779,    -1,   642,    -1,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
      -1,   794,    -1,    -1,    -1,   798,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   466,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   832,
      -1,   489,    -1,    -1,    -1,    -1,   839,    -1,    -1,    -1,
      -1,   403,   845,    -1,   847,    -1,    -1,    -1,   712,    -1,
      -1,   854,    -1,    -1,   416,   417,    -1,    -1,   420,    -1,
      -1,    -1,   424,   727,   728,   729,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   537,
      -1,   539,   540,   541,   542,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   455,    -1,    -1,   458,   761,   460,    -1,
     558,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   776,   777,    -1,   779,   574,    -1,   576,    -1,
     578,    -1,    -1,    -1,    -1,    -1,   584,   585,   490,   587,
      -1,    -1,    -1,    -1,   798,    -1,    -1,   595,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   619,    -1,    -1,    -1,    -1,    -1,    -1,   832,    -1,
      -1,    -1,   630,    -1,    43,   839,    -1,    -1,    -1,    -1,
      -1,   845,    -1,   847,    -1,    -1,    -1,    -1,    -1,    -1,
     854,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    -1,    -1,    82,    83,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   584,    -1,    -1,    -1,    -1,    -1,   686,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   598,   106,    -1,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,    50,
      51,    -1,   710,    54,   616,   713,    -1,    -1,   620,    -1,
      -1,    -1,   624,    64,    65,    -1,    67,    68,    69,    70,
     632,    72,    73,    -1,   732,    76,    -1,   735,   736,    80,
      81,    82,    83,    -1,    -1,    -1,    -1,   745,   746,    -1,
      -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   665,    -1,    -1,    -1,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,    -1,    -1,   120,
      -1,    -1,    -1,    50,    51,    -1,    -1,    54,   129,   787,
     788,   693,    -1,    -1,   792,    -1,    -1,    64,    65,    -1,
      67,    68,    69,    70,    -1,    72,    73,    -1,   806,    76,
      -1,    -1,    -1,    80,    81,    82,    83,    -1,   816,   817,
      -1,    -1,    -1,    -1,   726,    -1,    -1,    94,    -1,    -1,
      -1,   733,    -1,    -1,    -1,    -1,    -1,    -1,   740,    -1,
      -1,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,    -1,    -1,   120,    -1,    -1,    -1,   855,   856,    -1,
     762,    -1,   129,   861,    -1,    -1,    -1,    -1,    -1,    -1,
     868,    -1,    -1,    -1,   872,    -1,   874,   875,    -1,    -1,
      -1,    -1,   880,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   894,    -1,    -1,   897,
      -1,    50,    51,   805,   902,    54,    -1,    -1,    -1,    -1,
      -1,    -1,   814,    -1,    -1,    64,    65,   819,    67,    68,
      69,    70,    -1,    72,    73,    -1,    -1,    76,    -1,    -1,
      -1,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   857,    -1,   859,    -1,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,    -1,
      -1,   120,    -1,    -1,    -1,    -1,    -1,   879,    -1,   881,
     129,    -1,    -1,     0,     1,    -1,     3,     4,     5,     6,
       7,    -1,    -1,   895,    11,    12,    -1,    -1,   900,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    -1,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
      -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,    96,
      97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     0,    -1,    -1,    -1,
      -1,   118,   119,   120,     8,     9,    10,    -1,    -1,    13,
      14,    15,    -1,    17,    -1,    -1,   133,   134,    -1,    -1,
      -1,    -1,    -1,    27,    28,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    36,    37,    -1,    39,    40,    41,    42,    43,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    -1,    -1,    82,    83,
      84,    -1,    -1,    87,    -1,    -1,    -1,    -1,    -1,    93,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,    -1,    -1,    -1,    -1,    -1,   123,
     124,   125,   126,   127,   128,     0,    -1,   131,    -1,   133,
     134,    -1,    -1,     8,     9,    10,    -1,    -1,    -1,    14,
      15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    36,    37,    -1,    39,    40,    41,    42,    43,    -1,
      -1,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    -1,    -1,    82,    83,    -1,    -1,
      -1,    87,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    -1,    -1,    82,    83,    84,
     106,    86,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     105,   106,   128,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    -1,    -1,    -1,    -1,   122,   123,   124,
      -1,   126,   127,   128,     0,    -1,   131,    -1,   133,   134,
      -1,    -1,     8,     9,    10,    -1,    -1,    -1,    14,    15,
      -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      36,    37,    -1,    39,    40,    41,    42,    43,    -1,    -1,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    -1,    -1,    82,    83,    -1,    -1,    -1,
      -1,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    -1,    -1,    82,    83,    84,   106,
      86,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   105,
     106,   128,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,    -1,    -1,    -1,    -1,    -1,   123,   124,    -1,
     126,   127,   128,    -1,    -1,   131,    -1,   133,   134,     1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    -1,    -1,    15,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
      92,    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   118,   119,   120,    -1,
      -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,
       7,   133,   134,    10,    11,    12,    -1,    14,    15,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    -1,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
      -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,    96,
      97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   118,   119,   120,     1,    -1,     3,     4,     5,     6,
       7,    -1,    -1,    10,    11,    12,   133,   134,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    -1,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
      -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,    96,
      97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   118,   119,   120,     1,    -1,     3,     4,     5,     6,
       7,    -1,    -1,    10,    11,    12,   133,   134,    15,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    -1,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
      -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,    96,
      97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   118,   119,   120,     1,    -1,     3,     4,     5,     6,
       7,    -1,     9,    10,    11,    12,   133,   134,    -1,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    -1,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
      -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,    96,
      97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   118,   119,   120,     1,    -1,     3,     4,     5,     6,
       7,    -1,    -1,    -1,    11,    12,   133,   134,    -1,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    -1,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
      -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,    96,
      97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   118,   119,   120,    -1,    -1,    -1,    -1,    -1,     1,
     127,     3,     4,     5,     6,     7,   133,   134,    -1,    11,
      12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
      92,    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   118,   119,   120,    -1,
      -1,    -1,    -1,    -1,     1,   127,     3,     4,     5,     6,
       7,   133,   134,    -1,    11,    12,    -1,    -1,    -1,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    -1,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
      -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,    96,
      97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   118,   119,   120,    -1,    -1,   123,    -1,    -1,     1,
      -1,     3,     4,     5,     6,     7,   133,   134,    10,    11,
      12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
      92,    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   118,   119,   120,    -1,
       3,     4,     5,     6,     7,    -1,    -1,    -1,    11,    12,
      -1,   133,   134,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      -1,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
      -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,
      -1,    94,    -1,    96,    97,    98,    99,   100,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   118,   119,   120,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,    -1,
      -1,   134,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    60,    61,    -1,    -1,
      64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,
      -1,    -1,    96,    97,    98,    99,   100,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   118,   119,   120,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,   133,
     134,    13,    14,    15,    16,    17,    -1,    -1,    20,    21,
      22,    23,    24,    25,    26,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    65,    -1,    67,    68,    69,    70,    -1,
      72,    73,    -1,    -1,    76,    -1,    -1,    -1,    80,    81,
      82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,    -1,    -1,   120,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,   129,   130,    11,
      12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    26,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
      92,    -1,    94,    95,    96,    97,    98,    99,   100,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
     122,    18,    19,    20,    21,    22,    23,    24,   130,    26,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    63,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
      -1,    88,    89,    -1,    91,    92,    -1,    94,    95,    96,
      97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   118,   119,   120,    16,   122,    18,    19,    20,    21,
      22,    23,    24,   130,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    85,    86,    -1,    88,    89,    -1,    91,
      92,    -1,    94,    95,    96,    97,    98,    99,   100,    -1,
      -1,    -1,    -1,   105,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,   130,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    63,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    86,
      -1,    88,    89,    -1,    91,    92,    -1,    94,    95,    96,
      97,    98,    99,   100,    -1,    -1,    -1,    -1,   105,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   118,   119,   120,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,   130,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
      92,    -1,    94,    95,    96,    97,    98,    99,   100,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,   130,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    63,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
      -1,    88,    89,    -1,    91,    92,    -1,    94,    95,    96,
      97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   118,   119,   120,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,   130,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
      92,    -1,    94,    95,    96,    97,    98,    99,   100,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,   130,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    61,    -1,    63,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
      -1,    88,    89,    -1,    91,    92,    -1,    94,    95,    96,
      97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   118,   119,   120,     3,     4,     5,     6,     7,     8,
       9,    10,    -1,   130,    13,    14,    15,    16,    17,    -1,
      -1,    20,    21,    22,    23,    24,    25,    26,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    64,    65,    -1,    67,    68,
      69,    70,    -1,    72,    73,    -1,    -1,    76,    -1,    -1,
      -1,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,    -1,
      -1,   120,     3,     4,     5,     6,     7,     8,     9,    10,
     129,    -1,    13,    14,    15,    16,    17,    -1,    -1,    20,
      21,    22,    23,    24,    25,    26,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    64,    65,    -1,    67,    68,    69,    70,
      -1,    72,    73,    -1,    -1,    76,    -1,    -1,    -1,    80,
      81,    82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    94,    -1,    96,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,    -1,    -1,   120,
       3,     4,     5,     6,     7,     8,     9,    10,   129,    -1,
      13,    14,    15,    16,    17,    -1,    -1,    20,    21,    22,
      23,    24,    25,    26,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    -1,
      -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    65,    -1,    67,    68,    69,    70,    -1,    72,
      73,    -1,    -1,    76,    -1,    -1,    -1,    80,    81,    82,
      83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    94,    -1,    96,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,    -1,    -1,   120,     3,     4,
       5,     6,     7,     8,     9,    10,   129,    -1,    13,    14,
      15,    16,    17,    -1,    -1,    20,    21,    22,    23,    24,
      25,    26,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    -1,    -1,    54,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,
      65,    -1,    67,    68,    69,    70,    -1,    72,    73,    -1,
      -1,    76,    -1,    -1,    -1,    80,    81,    82,    83,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    -1,    -1,   120,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,   129,    11,    12,    -1,    -1,    -1,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    95,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,   118,   119,   120,    11,    12,    -1,    -1,    -1,
      16,   127,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   118,   119,   120,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    95,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    95,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   118,   119,   120,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    95,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    95,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   118,   119,   120,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    95,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    95,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   118,   119,   120,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    95,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    95,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   118,   119,   120,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    95,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    95,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   118,   119,   120,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    95,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    95,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   118,   119,   120,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    -1,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    -1,    -1,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   118,   119,   120,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    -1,    -1,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   118,   119,   120,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    -1,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   118,   119,   120,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    -1,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   118,   119,   120,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    -1,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    -1,    -1,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   118,   119,   120,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    -1,    -1,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,
      96,    97,    98,    99,   100,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,   110,    -1,    11,    12,    -1,    -1,
      -1,    16,   118,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    -1,    -1,
      -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    -1,
      -1,    96,    97,    98,    99,   100,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,    -1,
      -1,    -1,    16,   118,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    60,    61,    -1,    -1,
      -1,    -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,
      94,    -1,    96,    97,    98,    99,   100,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
      -1,    -1,    -1,    16,   118,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
      -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,
      -1,    94,    -1,    96,    97,    98,    99,   100,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,    -1,    -1,    -1,    16,   118,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
      92,    -1,    -1,    -1,    96,    97,    98,    99,   100,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,   118,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    -1,    -1,    96,    97,    98,    99,   100,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,    -1,    -1,    -1,    16,   118,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      -1,    91,    92,    -1,    -1,    -1,    96,    97,    98,    99,
     100,    -1,    50,    51,    -1,    -1,    54,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    64,    65,   118,    67,
      68,    69,    70,    -1,    72,    73,    -1,    -1,    76,    -1,
      -1,    -1,    80,    81,    82,    83,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
      50,    51,   120,    -1,    54,    -1,    -1,    -1,    -1,    -1,
      -1,   129,    -1,    -1,    64,    65,    -1,    67,    68,    69,
      70,    -1,    72,    73,    -1,    -1,    76,    -1,    -1,    -1,
      80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,    50,    51,
     120,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,   129,
      -1,    -1,    64,    65,    -1,    67,    68,    69,    70,    -1,
      72,    73,    -1,    -1,    76,    -1,    -1,    -1,    80,    81,
      82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,    50,    51,   120,    -1,
      54,    -1,    -1,    -1,    -1,    -1,    -1,   129,    -1,    -1,
      64,    65,    -1,    67,    68,    69,    70,    -1,    72,    73,
      -1,    -1,    76,    -1,    -1,    -1,    80,    81,    82,    83,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,    50,    51,   120,    -1,    54,    -1,
      -1,    -1,    -1,    -1,    -1,   129,    -1,    -1,    64,    65,
      -1,    67,    68,    69,    70,    -1,    72,    73,    -1,    -1,
      76,    -1,    -1,    -1,    80,    81,    82,    83,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,    50,    51,   120,    -1,    54,    -1,    -1,    -1,
      -1,    -1,    -1,   129,    -1,    -1,    64,    65,    -1,    67,
      68,    69,    70,    -1,    72,    73,    -1,    -1,    76,    -1,
      -1,    -1,    80,    81,    82,    83,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
      50,    51,   120,    -1,    54,    -1,    -1,    -1,    -1,    -1,
      -1,   129,    -1,    -1,    64,    65,    -1,    67,    68,    69,
      70,    -1,    72,    73,    -1,    -1,    76,    -1,    -1,    -1,
      80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,    50,    51,
     120,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,   129,
      -1,    -1,    64,    65,    -1,    67,    68,    69,    70,    -1,
      72,    73,    -1,    -1,    76,    -1,    -1,    -1,    80,    81,
      82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,    50,    51,   120,    -1,
      54,    -1,    -1,    -1,    -1,    -1,    -1,   129,    -1,    -1,
      64,    65,    -1,    67,    68,    69,    70,    -1,    72,    73,
      -1,    -1,    76,    -1,    -1,    -1,    80,    81,    82,    83,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,    -1,    -1,   120,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   129,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    -1,
      -1,    82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   106,    -1,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,    -1,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    -1,   133,    82,    83,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    -1,    -1,
      82,    83,    -1,    -1,    -1,    -1,    -1,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,    -1,
      -1,    -1,    -1,    -1,   106,    -1,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,    67,    68,    69,    70,
      71,    72,    73,    74,    -1,    76,    77,    -1,    -1,    -1,
      -1,    82,    83,    67,    68,    69,    70,    71,    72,    73,
      -1,    -1,    76,    77,    -1,    -1,    -1,    -1,    82,    83,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   136,   137,     0,     1,     3,     4,     5,     6,     7,
      11,    12,    16,    18,    19,    20,    21,    22,    23,    24,
      29,    30,    31,    32,    33,    34,    35,    38,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    60,    61,    64,    65,    66,    85,    88,
      89,    91,    92,    94,    96,    97,    98,    99,   100,   118,
     119,   120,   139,   140,   141,   144,   146,   147,   151,   152,
     154,   155,   156,   157,   158,   167,   185,   202,   212,   213,
     226,   227,   228,   229,   230,   231,   232,   235,   243,   245,
     246,   247,   248,   249,   250,   271,   280,   141,    21,    22,
      29,    30,    31,    45,    50,    54,    82,    85,    88,   118,
     159,   160,   185,   202,   247,   250,   271,   160,     3,     4,
       5,     6,     7,     8,     9,    10,    13,    14,    15,    16,
      17,    20,    21,    22,    23,    24,    25,    26,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    54,    64,    65,    67,    68,    69,    70,    72,    73,
      76,    80,    81,    82,    83,    94,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   120,   129,   130,   161,
     165,   166,   248,   264,    32,    33,    34,    35,    48,    49,
      50,    54,    96,   161,   162,   163,   243,   186,    85,   144,
     145,   158,   202,   247,   249,   250,   145,   133,   134,   145,
     275,   278,   279,   189,   191,    85,   152,   158,   202,   207,
     247,   250,    63,    94,    95,   119,   151,   167,   170,   174,
     181,   183,   267,   268,   269,   270,   174,   174,   130,   176,
     177,   130,   172,   176,   144,    52,   162,   133,   276,   143,
     122,   238,   167,   202,   167,    50,    51,    54,   165,   272,
      54,    88,   139,   153,   154,   144,    94,   151,   171,   183,
     267,   280,   183,   266,   267,   280,    85,   157,   202,   247,
     250,    52,    53,    55,   161,   238,   244,   237,   238,   132,
     233,   132,   236,    57,    58,   146,   167,   167,   275,   279,
      39,    40,    41,    42,    43,    36,    37,    28,   209,   105,
     128,    88,    94,   155,   105,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    82,    83,
     106,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,    84,   124,   126,   131,    84,   126,    26,   122,   214,
     229,    86,    86,   172,   176,   214,   276,   144,    50,    54,
     159,    57,    58,     1,   109,   251,   278,    84,   124,   126,
     198,   265,   199,    84,   126,   274,   128,   138,   139,    54,
      13,   107,   203,   278,   105,    84,   124,   126,    86,    86,
     203,   275,    15,    17,   219,   134,   145,   145,    54,    84,
     124,   126,    25,   170,   170,   170,    87,   128,   182,   280,
     128,   182,   127,   174,    89,   174,   178,   151,   174,   183,
     212,   280,    52,    60,    61,   142,   130,   168,   122,   139,
      59,   101,   102,   103,   239,    84,   126,    86,   153,   127,
     127,   187,   167,   276,   125,   128,   133,   277,   128,   277,
     128,   277,   123,   277,    54,    84,   124,   126,   103,   103,
     239,    62,   103,   103,   234,   239,   103,    59,   103,    67,
      67,   141,   145,   145,   145,   145,   141,   144,   144,   210,
      94,   146,   170,   183,   184,   153,   157,   128,   146,   167,
     170,   184,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   169,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,    50,    51,
      54,   165,   272,   273,   171,    50,    54,   272,   272,   272,
     272,   217,   215,   146,   167,   146,   167,    93,   148,   196,
     278,   252,   195,    50,    54,   159,   272,   171,   272,   138,
     144,    50,    52,    53,    54,    55,    94,    95,   112,   115,
     130,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   200,   164,    10,     8,   222,   280,   139,    13,   167,
      50,    54,   171,    50,    54,   139,   219,   139,    94,   183,
     220,    10,    27,   107,   204,   278,   204,    50,    54,   171,
      50,    54,   193,   128,   182,   170,    94,   170,   181,   267,
      94,   170,   268,   127,    94,   167,   170,   175,   179,   181,
     267,   276,   128,    84,   126,   276,   127,   162,   188,   167,
     139,   123,   241,   240,   167,   127,   276,   276,    94,   170,
      50,    54,   171,    50,    54,   132,   239,   132,   167,   167,
      75,   110,   208,   280,   170,   128,   127,    43,   167,    86,
      86,   172,   176,   125,    86,    86,   172,   173,   176,   280,
     173,   176,   173,   176,   208,   208,   149,   278,   145,   138,
     125,    10,   276,   105,   254,   138,   278,   128,   263,   280,
     128,   263,    50,   128,   263,    50,   161,   162,   170,   184,
     223,   280,    15,   206,   280,    14,   205,   206,    86,   125,
      86,    86,   206,    10,    10,   170,   128,   203,   190,   192,
     125,   145,   170,   128,   182,   170,   170,   128,   180,   127,
     128,   182,   127,   151,   212,   272,   272,   127,   144,   123,
     139,    52,    53,    55,   242,   250,   127,   167,   125,   110,
     207,   211,    94,   170,   167,   107,   146,   167,   167,   148,
      86,   146,   167,   146,   167,   148,   218,   216,   208,   197,
     278,    10,   127,   170,   276,    10,   255,   258,   260,   262,
      50,   257,   260,   201,    87,   224,   280,   139,     9,   225,
     280,   145,    10,    86,    10,    94,   139,   139,   139,   204,
     182,    94,   182,   182,    94,   181,   183,   267,   127,    94,
     276,   127,   123,   276,   110,   139,   170,   167,   146,   167,
     139,   139,   150,   138,   127,   128,   263,   263,   263,   253,
      85,   158,   202,   247,   250,   203,   139,   203,   170,   206,
     219,   221,    10,    10,   194,   167,   170,   128,   182,   128,
     182,   170,   127,    10,    10,   123,   139,    10,   260,   138,
      54,    84,   124,   126,   139,   139,   139,   182,   182,    94,
     267,    94,   182,   123,   263,    10,    50,    54,   171,    50,
      54,   222,   205,    10,   170,   128,   182,   170,   125,   182,
      94,   182,   170,   182
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
# if YYLTYPE_IS_TRIVIAL
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
#line 411 "grammar.y"
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
#line 421 "grammar.y"
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
#line 446 "grammar.y"
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
#line 463 "grammar.y"
    {
                        void_stmts((yyvsp[(1) - (2)].node), parse_state);
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 7:
#line 471 "grammar.y"
    {
                        (yyval.node) = newline_node(parse_state, (yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 8:
#line 475 "grammar.y"
    {
                        (yyval.node) = block_append(parse_state, (yyvsp[(1) - (3)].node), newline_node(parse_state, (yyvsp[(3) - (3)].node)));
                    ;}
    break;

  case 9:
#line 479 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 10:
#line 484 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 11:
#line 485 "grammar.y"
    {
                        (yyval.node) = NEW_ALIAS((yyvsp[(2) - (4)].id), (yyvsp[(4) - (4)].id));
                    ;}
    break;

  case 12:
#line 489 "grammar.y"
    {
                        (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 13:
#line 493 "grammar.y"
    {
                        char buf[3];

                        sprintf(buf, "$%c", (char)(yyvsp[(3) - (3)].node)->nd_nth);
                        (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), rb_intern(buf));
                    ;}
    break;

  case 14:
#line 500 "grammar.y"
    {
                        yyerror("can't make alias for the number variables");
                        (yyval.node) = 0;
                    ;}
    break;

  case 15:
#line 505 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 16:
#line 509 "grammar.y"
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
#line 518 "grammar.y"
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
#line 527 "grammar.y"
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
#line 539 "grammar.y"
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
#line 551 "grammar.y"
    {
                        (yyval.node) = NEW_RESCUE((yyvsp[(1) - (3)].node), NEW_RESBODY(0,(yyvsp[(3) - (3)].node),0), 0);
                    ;}
    break;

  case 21:
#line 555 "grammar.y"
    {
                        if (in_def || in_single) {
                            yyerror("BEGIN in method");
                        }
                        local_push(0);
                    ;}
    break;

  case 22:
#line 562 "grammar.y"
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
#line 571 "grammar.y"
    {
                        if (in_def || in_single) {
                            rb_warn("END in method; use at_exit");
                        }

                        (yyval.node) = NEW_ITER(0, NEW_POSTEXE(), (yyvsp[(3) - (4)].node));
                    ;}
    break;

  case 24:
#line 579 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 25:
#line 583 "grammar.y"
    {
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyvsp[(1) - (3)].node)->nd_value = ((yyvsp[(1) - (3)].node)->nd_head) ? NEW_TO_ARY((yyvsp[(3) - (3)].node)) : NEW_ARRAY((yyvsp[(3) - (3)].node));
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 26:
#line 589 "grammar.y"
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
#line 614 "grammar.y"
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
#line 633 "grammar.y"
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
#line 645 "grammar.y"
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
#line 657 "grammar.y"
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
#line 669 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (3)].node));
                        (yyval.node) = 0;
                    ;}
    break;

  case 32:
#line 674 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), NEW_SVALUE((yyvsp[(3) - (3)].node)), parse_state);
                    ;}
    break;

  case 33:
#line 678 "grammar.y"
    {
                        (yyvsp[(1) - (3)].node)->nd_value = ((yyvsp[(1) - (3)].node)->nd_head) ? NEW_TO_ARY((yyvsp[(3) - (3)].node)) : NEW_ARRAY((yyvsp[(3) - (3)].node));
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 34:
#line 683 "grammar.y"
    {
                        (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 37:
#line 692 "grammar.y"
    {
                        (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 38:
#line 696 "grammar.y"
    {
                        (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 39:
#line 700 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), parse_state));
                    ;}
    break;

  case 40:
#line 704 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), parse_state));
                    ;}
    break;

  case 42:
#line 711 "grammar.y"
    {
                        value_expr((yyval.node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 45:
#line 720 "grammar.y"
    {
                        (yyval.node) = NEW_RETURN(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 46:
#line 724 "grammar.y"
    {
                        (yyval.node) = NEW_BREAK(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 47:
#line 728 "grammar.y"
    {
                        (yyval.node) = NEW_NEXT(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 49:
#line 735 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 50:
#line 739 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 51:
#line 745 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 52:
#line 749 "grammar.y"
    { (yyval.vars) = vps->block_vars; ;}
    break;

  case 53:
#line 752 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 54:
#line 759 "grammar.y"
    {
                        (yyval.node) = new_fcall(parse_state, (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                   ;}
    break;

  case 55:
#line 764 "grammar.y"
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
#line 776 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 57:
#line 781 "grammar.y"
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
#line 793 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 59:
#line 798 "grammar.y"
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
#line 810 "grammar.y"
    {
                        (yyval.node) = new_super(parse_state, (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 61:
#line 815 "grammar.y"
    {
                        (yyval.node) = new_yield(parse_state, (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 63:
#line 823 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 65:
#line 830 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(NEW_LIST((yyvsp[(2) - (3)].node)), 0);
                    ;}
    break;

  case 66:
#line 836 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (1)].node), 0);
                    ;}
    break;

  case 67:
#line 840 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(list_append(parse_state, (yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node)), 0);
                    ;}
    break;

  case 68:
#line 844 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 69:
#line 848 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (2)].node), -1);
                    ;}
    break;

  case 70:
#line 852 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(0, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 71:
#line 856 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(0, -1);
                    ;}
    break;

  case 73:
#line 863 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 74:
#line 869 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 75:
#line 873 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, (yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 76:
#line 879 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, parse_state);
                    ;}
    break;

  case 77:
#line 883 "grammar.y"
    {
                        (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), parse_state);
                    ;}
    break;

  case 78:
#line 887 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), parse_state);
                    ;}
    break;

  case 79:
#line 891 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), parse_state);
                    ;}
    break;

  case 80:
#line 895 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), parse_state);
                    ;}
    break;

  case 81:
#line 899 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                    ;}
    break;

  case 82:
#line 905 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                    ;}
    break;

  case 83:
#line 911 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (1)].node));
                        (yyval.node) = 0;
                    ;}
    break;

  case 84:
#line 918 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, parse_state);
                    ;}
    break;

  case 85:
#line 922 "grammar.y"
    {
                        (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), parse_state);
                    ;}
    break;

  case 86:
#line 926 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), parse_state);
                    ;}
    break;

  case 87:
#line 930 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), parse_state);
                    ;}
    break;

  case 88:
#line 934 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), parse_state);
                    ;}
    break;

  case 89:
#line 938 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                    ;}
    break;

  case 90:
#line 944 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                    ;}
    break;

  case 91:
#line 950 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (1)].node));
                        (yyval.node) = 0;
                    ;}
    break;

  case 92:
#line 957 "grammar.y"
    {
                        yyerror("class/module name must be CONSTANT");
                    ;}
    break;

  case 94:
#line 964 "grammar.y"
    {
                        (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 95:
#line 968 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2(0, (yyval.node));
                    ;}
    break;

  case 96:
#line 972 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 100:
#line 981 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = convert_op((yyvsp[(1) - (1)].id));
                    ;}
    break;

  case 101:
#line 986 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = (yyvsp[(1) - (1)].id);
                    ;}
    break;

  case 104:
#line 997 "grammar.y"
    {
                        (yyval.node) = NEW_UNDEF((yyvsp[(1) - (1)].id));
                    ;}
    break;

  case 105:
#line 1000 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 106:
#line 1001 "grammar.y"
    {
                        (yyval.node) = block_append(parse_state, (yyvsp[(1) - (4)].node), NEW_UNDEF((yyvsp[(4) - (4)].id)));
                    ;}
    break;

  case 107:
#line 1006 "grammar.y"
    { (yyval.id) = '|'; ;}
    break;

  case 108:
#line 1007 "grammar.y"
    { (yyval.id) = '^'; ;}
    break;

  case 109:
#line 1008 "grammar.y"
    { (yyval.id) = '&'; ;}
    break;

  case 110:
#line 1009 "grammar.y"
    { (yyval.id) = tCMP; ;}
    break;

  case 111:
#line 1010 "grammar.y"
    { (yyval.id) = tEQ; ;}
    break;

  case 112:
#line 1011 "grammar.y"
    { (yyval.id) = tEQQ; ;}
    break;

  case 113:
#line 1012 "grammar.y"
    { (yyval.id) = tMATCH; ;}
    break;

  case 114:
#line 1013 "grammar.y"
    { (yyval.id) = '>'; ;}
    break;

  case 115:
#line 1014 "grammar.y"
    { (yyval.id) = tGEQ; ;}
    break;

  case 116:
#line 1015 "grammar.y"
    { (yyval.id) = '<'; ;}
    break;

  case 117:
#line 1016 "grammar.y"
    { (yyval.id) = tLEQ; ;}
    break;

  case 118:
#line 1017 "grammar.y"
    { (yyval.id) = tLSHFT; ;}
    break;

  case 119:
#line 1018 "grammar.y"
    { (yyval.id) = tRSHFT; ;}
    break;

  case 120:
#line 1019 "grammar.y"
    { (yyval.id) = '+'; ;}
    break;

  case 121:
#line 1020 "grammar.y"
    { (yyval.id) = '-'; ;}
    break;

  case 122:
#line 1021 "grammar.y"
    { (yyval.id) = '*'; ;}
    break;

  case 123:
#line 1022 "grammar.y"
    { (yyval.id) = '*'; ;}
    break;

  case 124:
#line 1023 "grammar.y"
    { (yyval.id) = '/'; ;}
    break;

  case 125:
#line 1024 "grammar.y"
    { (yyval.id) = '%'; ;}
    break;

  case 126:
#line 1025 "grammar.y"
    { (yyval.id) = tPOW; ;}
    break;

  case 127:
#line 1026 "grammar.y"
    { (yyval.id) = '~'; ;}
    break;

  case 128:
#line 1027 "grammar.y"
    { (yyval.id) = tUPLUS; ;}
    break;

  case 129:
#line 1028 "grammar.y"
    { (yyval.id) = tUMINUS; ;}
    break;

  case 130:
#line 1029 "grammar.y"
    { (yyval.id) = tAREF; ;}
    break;

  case 131:
#line 1030 "grammar.y"
    { (yyval.id) = tASET; ;}
    break;

  case 132:
#line 1031 "grammar.y"
    { (yyval.id) = '`'; ;}
    break;

  case 174:
#line 1044 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 175:
#line 1048 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (5)].node), NEW_RESCUE((yyvsp[(3) - (5)].node), NEW_RESBODY(0,(yyvsp[(5) - (5)].node),0), 0), parse_state);
                    ;}
    break;

  case 176:
#line 1052 "grammar.y"
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

  case 177:
#line 1077 "grammar.y"
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

  case 178:
#line 1096 "grammar.y"
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

  case 179:
#line 1108 "grammar.y"
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

  case 181:
#line 1132 "grammar.y"
    {
                        yyerror("constant re-assignment");
                        (yyval.node) = 0;
                    ;}
    break;

  case 182:
#line 1137 "grammar.y"
    {
                        yyerror("constant re-assignment");
                        (yyval.node) = 0;
                    ;}
    break;

  case 183:
#line 1142 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (3)].node));
                        (yyval.node) = 0;
                    ;}
    break;

  case 184:
#line 1147 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (3)].node));
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyval.node) = NEW_DOT2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 185:
#line 1153 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (3)].node));
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyval.node) = NEW_DOT3((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 186:
#line 1159 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '+', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 187:
#line 1163 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '-', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 188:
#line 1167 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '*', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 189:
#line 1171 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '/', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 190:
#line 1175 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '%', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 191:
#line 1179 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tPOW, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 192:
#line 1183 "grammar.y"
    {
                        (yyval.node) = call_op(call_op((yyvsp[(2) - (4)].node), tPOW, 1, (yyvsp[(4) - (4)].node), parse_state), tUMINUS, 0, 0, parse_state);
                    ;}
    break;

  case 193:
#line 1187 "grammar.y"
    {
                        (yyval.node) = call_op(call_op((yyvsp[(2) - (4)].node), tPOW, 1, (yyvsp[(4) - (4)].node), parse_state), tUMINUS, 0, 0, parse_state);
                    ;}
    break;

  case 194:
#line 1191 "grammar.y"
    {
                        if ((yyvsp[(2) - (2)].node) && nd_type((yyvsp[(2) - (2)].node)) == NODE_LIT) {
                            (yyval.node) = (yyvsp[(2) - (2)].node);
                        }
                        else {
                            (yyval.node) = call_op((yyvsp[(2) - (2)].node), tUPLUS, 0, 0, parse_state);
                        }
                    ;}
    break;

  case 195:
#line 1200 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(2) - (2)].node), tUMINUS, 0, 0, parse_state);
                    ;}
    break;

  case 196:
#line 1204 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '|', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 197:
#line 1208 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '^', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 198:
#line 1212 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '&', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 199:
#line 1216 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tCMP, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 200:
#line 1220 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '>', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 201:
#line 1224 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tGEQ, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 202:
#line 1228 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '<', 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 203:
#line 1232 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tLEQ, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 204:
#line 1236 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tEQ, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 205:
#line 1240 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tEQQ, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 206:
#line 1244 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(call_op((yyvsp[(1) - (3)].node), tEQ, 1, (yyvsp[(3) - (3)].node), parse_state));
                    ;}
    break;

  case 207:
#line 1248 "grammar.y"
    {
                        (yyval.node) = match_gen((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 208:
#line 1252 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(match_gen((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state));
                    ;}
    break;

  case 209:
#line 1256 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), parse_state));
                    ;}
    break;

  case 210:
#line 1260 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(2) - (2)].node), '~', 0, 0, parse_state);
                    ;}
    break;

  case 211:
#line 1264 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tLSHFT, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 212:
#line 1268 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tRSHFT, 1, (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 213:
#line 1272 "grammar.y"
    {
                        (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 214:
#line 1276 "grammar.y"
    {
                        (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 215:
#line 1279 "grammar.y"
    {vps->in_defined = 1;;}
    break;

  case 216:
#line 1280 "grammar.y"
    {
                        vps->in_defined = 0;
                        (yyval.node) = NEW_DEFINED((yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 217:
#line 1284 "grammar.y"
    {vps->ternary_colon++;;}
    break;

  case 218:
#line 1285 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(1) - (6)].node), parse_state), (yyvsp[(4) - (6)].node), (yyvsp[(6) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (6)].node));
                        vps->ternary_colon--;
                    ;}
    break;

  case 219:
#line 1291 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 220:
#line 1297 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (1)].node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 222:
#line 1305 "grammar.y"
    {
                        rb_warn("parenthesize argument(s) for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 223:
#line 1310 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 224:
#line 1314 "grammar.y"
    {
                        value_expr((yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_concat(parse_state, (yyvsp[(1) - (5)].node), (yyvsp[(4) - (5)].node));
                    ;}
    break;

  case 225:
#line 1319 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                    ;}
    break;

  case 226:
#line 1323 "grammar.y"
    {
                        value_expr((yyvsp[(2) - (3)].node));
                        (yyval.node) = NEW_NEWLINE(NEW_SPLAT((yyvsp[(2) - (3)].node)));
                    ;}
    break;

  case 227:
#line 1330 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 228:
#line 1334 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                    ;}
    break;

  case 229:
#line 1338 "grammar.y"
    {
                        rb_warn("parenthesize argument for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(2) - (4)].node));
                    ;}
    break;

  case 230:
#line 1343 "grammar.y"
    {
                        rb_warn("parenthesize argument for future version");
                        (yyval.node) = list_append(parse_state, (yyvsp[(2) - (6)].node), (yyvsp[(4) - (6)].node));
                    ;}
    break;

  case 233:
#line 1354 "grammar.y"
    {
                        rb_warn("parenthesize argument(s) for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 234:
#line 1359 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 235:
#line 1363 "grammar.y"
    {
                        (yyval.node) = arg_concat(parse_state, (yyvsp[(1) - (5)].node), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 236:
#line 1368 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_POSITIONAL((yyvsp[(1) - (2)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 237:
#line 1373 "grammar.y"
    {
                        (yyval.node) = arg_concat(parse_state, NEW_LIST(NEW_POSITIONAL((yyvsp[(1) - (5)].node))), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 238:
#line 1378 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, (yyvsp[(1) - (4)].node), NEW_POSITIONAL((yyvsp[(3) - (4)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 239:
#line 1383 "grammar.y"
    {
                        value_expr((yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_concat(parse_state, list_append(parse_state, (yyvsp[(1) - (7)].node), NEW_POSITIONAL((yyvsp[(3) - (7)].node))), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 240:
#line 1389 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(NEW_SPLAT((yyvsp[(2) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 242:
#line 1396 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(list_concat(NEW_LIST((yyvsp[(1) - (4)].node)),(yyvsp[(3) - (4)].node)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 243:
#line 1400 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 244:
#line 1404 "grammar.y"
    {
                        (yyval.node) = arg_concat(parse_state, NEW_LIST((yyvsp[(1) - (5)].node)), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 245:
#line 1409 "grammar.y"
    {
            (yyval.node) = arg_concat(parse_state, list_concat(NEW_LIST((yyvsp[(1) - (7)].node)),(yyvsp[(3) - (7)].node)), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 246:
#line 1414 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_POSITIONAL((yyvsp[(1) - (2)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 247:
#line 1419 "grammar.y"
    {
                        (yyval.node) = arg_concat(parse_state, NEW_LIST(NEW_POSITIONAL((yyvsp[(1) - (5)].node))), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 248:
#line 1424 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, NEW_LIST((yyvsp[(1) - (4)].node)), NEW_POSITIONAL((yyvsp[(3) - (4)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 249:
#line 1429 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, list_concat(NEW_LIST((yyvsp[(1) - (6)].node)),(yyvsp[(3) - (6)].node)), NEW_POSITIONAL((yyvsp[(5) - (6)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(6) - (6)].node));
                    ;}
    break;

  case 250:
#line 1434 "grammar.y"
    {
                        (yyval.node) = arg_concat(parse_state, list_append(parse_state, NEW_LIST((yyvsp[(1) - (7)].node)), NEW_POSITIONAL((yyvsp[(3) - (7)].node))), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 251:
#line 1439 "grammar.y"
    {
                        (yyval.node) = arg_concat(parse_state, list_append(parse_state, list_concat(NEW_LIST((yyvsp[(1) - (9)].node)), (yyvsp[(3) - (9)].node)), NEW_POSITIONAL((yyvsp[(5) - (9)].node))), (yyvsp[(8) - (9)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(9) - (9)].node));
                    ;}
    break;

  case 252:
#line 1444 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(NEW_SPLAT((yyvsp[(2) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 254:
#line 1450 "grammar.y"
    {
                        (yyval.num) = vps->cmdarg_stack;
                        CMDARG_PUSH(1);
                    ;}
    break;

  case 255:
#line 1455 "grammar.y"
    {
                        /* CMDARG_POP() */
                        vps->cmdarg_stack = (yyvsp[(1) - (2)].num);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 257:
#line 1463 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 258:
#line 1464 "grammar.y"
    {
                        rb_warn("don't put space before argument parentheses");
                        (yyval.node) = 0;
                    ;}
    break;

  case 259:
#line 1468 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 260:
#line 1469 "grammar.y"
    {
                        rb_warn("don't put space before argument parentheses");
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                    ;}
    break;

  case 261:
#line 1476 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK_PASS((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 262:
#line 1482 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 264:
#line 1489 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 265:
#line 1493 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 266:
#line 1499 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 267:
#line 1503 "grammar.y"
    {
                        (yyval.node) = arg_concat(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 268:
#line 1507 "grammar.y"
    {
                        (yyval.node) = NEW_SPLAT((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 277:
#line 1521 "grammar.y"
    {
                        (yyval.node) = NEW_FCALL((yyvsp[(1) - (1)].id), 0);
                    ;}
    break;

  case 278:
#line 1525 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                    ;}
    break;

  case 279:
#line 1530 "grammar.y"
    {
                        if ((yyvsp[(3) - (4)].node) == NULL)
                            (yyval.node) = NEW_NIL();
                        else
                            (yyval.node) = NEW_BEGIN((yyvsp[(3) - (4)].node));
                        nd_set_line((yyval.node), (yyvsp[(1) - (4)].num));
                    ;}
    break;

  case 280:
#line 1537 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 281:
#line 1538 "grammar.y"
    {
                        rb_warning("(...) interpreted as grouped expression");
                        (yyval.node) = (yyvsp[(2) - (5)].node);
                    ;}
    break;

  case 282:
#line 1543 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 283:
#line 1547 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 284:
#line 1551 "grammar.y"
    {
                        (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 285:
#line 1555 "grammar.y"
    {
                        if ((yyvsp[(1) - (4)].node) && nd_type((yyvsp[(1) - (4)].node)) == NODE_SELF) {
                            (yyval.node) = NEW_FCALL(convert_op(tAREF), (yyvsp[(3) - (4)].node));
                        } else {
                            (yyval.node) = NEW_CALL((yyvsp[(1) - (4)].node), convert_op(tAREF), (yyvsp[(3) - (4)].node));
                        }
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 286:
#line 1564 "grammar.y"
    {
                        if ((yyvsp[(2) - (3)].node) == 0) {
                            (yyval.node) = NEW_ZARRAY(); /* zero length array*/
                        }
                        else {
                            (yyval.node) = (yyvsp[(2) - (3)].node);
                        }
                    ;}
    break;

  case 287:
#line 1573 "grammar.y"
    {
                        (yyval.node) = NEW_HASH((yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 288:
#line 1577 "grammar.y"
    {
                        (yyval.node) = NEW_RETURN(0);
                    ;}
    break;

  case 289:
#line 1581 "grammar.y"
    {
                        (yyval.node) = new_yield(parse_state, (yyvsp[(3) - (4)].node));
                    ;}
    break;

  case 290:
#line 1585 "grammar.y"
    {
                        (yyval.node) = NEW_YIELD(0, Qfalse);
                    ;}
    break;

  case 291:
#line 1589 "grammar.y"
    {
                        (yyval.node) = NEW_YIELD(0, Qfalse);
                    ;}
    break;

  case 292:
#line 1592 "grammar.y"
    {vps->in_defined = 1;;}
    break;

  case 293:
#line 1593 "grammar.y"
    {
                        vps->in_defined = 0;
                        (yyval.node) = NEW_DEFINED((yyvsp[(5) - (6)].node));
                    ;}
    break;

  case 294:
#line 1598 "grammar.y"
    {
                        (yyvsp[(2) - (2)].node)->nd_iter = NEW_FCALL((yyvsp[(1) - (2)].id), 0);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                        fixpos((yyvsp[(2) - (2)].node)->nd_iter, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 296:
#line 1605 "grammar.y"
    {
                        if ((yyvsp[(1) - (2)].node) && nd_type((yyvsp[(1) - (2)].node)) == NODE_BLOCK_PASS) {
                            rb_compile_error("both block arg and actual block given");
                        }
                        (yyvsp[(2) - (2)].node)->nd_iter = (yyvsp[(1) - (2)].node);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                        fixpos((yyval.node), (yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 297:
#line 1617 "grammar.y"
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

  case 298:
#line 1630 "grammar.y"
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

  case 299:
#line 1639 "grammar.y"
    {COND_PUSH(1);;}
    break;

  case 300:
#line 1639 "grammar.y"
    {COND_POP();;}
    break;

  case 301:
#line 1642 "grammar.y"
    {
                        (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (7)].node), parse_state), (yyvsp[(6) - (7)].node), 1);
                        fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            nd_set_type((yyval.node), NODE_UNTIL);
                        }
                    ;}
    break;

  case 302:
#line 1649 "grammar.y"
    {COND_PUSH(1);;}
    break;

  case 303:
#line 1649 "grammar.y"
    {COND_POP();;}
    break;

  case 304:
#line 1652 "grammar.y"
    {
                        (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (7)].node), parse_state), (yyvsp[(6) - (7)].node), 1);
                        fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            nd_set_type((yyval.node), NODE_WHILE);
                        }
                    ;}
    break;

  case 305:
#line 1662 "grammar.y"
    {
                        (yyval.node) = NEW_CASE((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (5)].node));
                    ;}
    break;

  case 306:
#line 1667 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(3) - (4)].node);
                    ;}
    break;

  case 307:
#line 1671 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(4) - (5)].node);
                    ;}
    break;

  case 308:
#line 1674 "grammar.y"
    {COND_PUSH(1);;}
    break;

  case 309:
#line 1674 "grammar.y"
    {COND_POP();;}
    break;

  case 310:
#line 1677 "grammar.y"
    {
                        (yyval.node) = NEW_FOR((yyvsp[(2) - (9)].node), (yyvsp[(5) - (9)].node), (yyvsp[(8) - (9)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (9)].node));
                    ;}
    break;

  case 311:
#line 1682 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("class definition in method body");
                        class_nest++;
                        local_push(0);
                        (yyval.num) = ruby_sourceline;
                    ;}
    break;

  case 312:
#line 1691 "grammar.y"
    {
                        (yyval.node) = NEW_CLASS((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(3) - (6)].node));
                        nd_set_line((yyval.node), (yyvsp[(4) - (6)].num));
                        local_pop();
                        class_nest--;
                    ;}
    break;

  case 313:
#line 1698 "grammar.y"
    {
                        (yyval.num) = in_def;
                        in_def = 0;
                    ;}
    break;

  case 314:
#line 1703 "grammar.y"
    {
                        (yyval.num) = in_single;
                        in_single = 0;
                        class_nest++;
                        local_push(0);
                    ;}
    break;

  case 315:
#line 1711 "grammar.y"
    {
                        (yyval.node) = NEW_SCLASS((yyvsp[(3) - (8)].node), (yyvsp[(7) - (8)].node));
                        fixpos((yyval.node), (yyvsp[(3) - (8)].node));
                        local_pop();
                        class_nest--;
                        in_def = (yyvsp[(4) - (8)].num);
                        in_single = (yyvsp[(6) - (8)].num);
                    ;}
    break;

  case 316:
#line 1720 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("module definition in method body");
                        class_nest++;
                        local_push(0);
                        (yyval.num) = ruby_sourceline;
                    ;}
    break;

  case 317:
#line 1729 "grammar.y"
    {
                        (yyval.node) = NEW_MODULE((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
                        nd_set_line((yyval.node), (yyvsp[(3) - (5)].num));
                        local_pop();
                        class_nest--;
                    ;}
    break;

  case 318:
#line 1736 "grammar.y"
    {
                        (yyval.id) = cur_mid;
                        cur_mid = (yyvsp[(2) - (2)].id);
                        in_def++;
                        local_push(0);
                    ;}
    break;

  case 319:
#line 1745 "grammar.y"
    {
                        if (!(yyvsp[(5) - (6)].node)) (yyvsp[(5) - (6)].node) = NEW_NIL();
                        (yyval.node) = NEW_DEFN((yyvsp[(2) - (6)].id), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node), NOEX_PRIVATE);
                        fixpos((yyval.node), (yyvsp[(4) - (6)].node));
                        local_pop();
                        in_def--;
                        cur_mid = (yyvsp[(3) - (6)].id);
                    ;}
    break;

  case 320:
#line 1753 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 321:
#line 1754 "grammar.y"
    {
                        in_single++;
                        local_push(0);
                        vps->lex_state = EXPR_END; /* force for args */
                    ;}
    break;

  case 322:
#line 1762 "grammar.y"
    {
                        (yyval.node) = NEW_DEFS((yyvsp[(2) - (9)].node), (yyvsp[(5) - (9)].id), (yyvsp[(7) - (9)].node), (yyvsp[(8) - (9)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (9)].node));
                        local_pop();
                        in_single--;
                    ;}
    break;

  case 323:
#line 1769 "grammar.y"
    {
                        (yyval.node) = NEW_BREAK(0);
                    ;}
    break;

  case 324:
#line 1773 "grammar.y"
    {
                        (yyval.node) = NEW_NEXT(0);
                    ;}
    break;

  case 325:
#line 1777 "grammar.y"
    {
                        (yyval.node) = NEW_REDO();
                    ;}
    break;

  case 326:
#line 1781 "grammar.y"
    {
                        (yyval.node) = NEW_RETRY();
                    ;}
    break;

  case 327:
#line 1787 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (1)].node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 336:
#line 1808 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(2) - (5)].node), parse_state), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (5)].node));
                    ;}
    break;

  case 338:
#line 1816 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 342:
#line 1827 "grammar.y"
    {
                        (yyval.node) = (NODE*)1;
                    ;}
    break;

  case 343:
#line 1831 "grammar.y"
    {
                        (yyval.node) = (NODE*)1;
                    ;}
    break;

  case 344:
#line 1835 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 345:
#line 1841 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 346:
#line 1846 "grammar.y"
    {
                      (yyval.vars) = vps->block_vars;
                    ;}
    break;

  case 347:
#line 1851 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 348:
#line 1858 "grammar.y"
    {
                        if ((yyvsp[(1) - (2)].node) && nd_type((yyvsp[(1) - (2)].node)) == NODE_BLOCK_PASS) {
                            rb_compile_error("both block arg and actual block given");
                        }
                        (yyvsp[(2) - (2)].node)->nd_iter = (yyvsp[(1) - (2)].node);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                        fixpos((yyval.node), (yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 349:
#line 1867 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 350:
#line 1871 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 351:
#line 1877 "grammar.y"
    {
                        (yyval.node) = new_fcall(parse_state, (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 352:
#line 1882 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 353:
#line 1887 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 354:
#line 1892 "grammar.y"
    {
                        (yyval.node) = new_call(parse_state, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), 0);
                    ;}
    break;

  case 355:
#line 1896 "grammar.y"
    {
                        (yyval.node) = NEW_CALL((yyvsp[(1) - (3)].node), rb_intern("get_reference"), NEW_LIST(NEW_LIT(ID2SYM((yyvsp[(3) - (3)].id)))));
                    ;}
    break;

  case 356:
#line 1900 "grammar.y"
    {
                        (yyval.node) = NEW_FCALL(rb_intern("get_reference"), NEW_LIST(NEW_LIT(ID2SYM((yyvsp[(2) - (2)].id)))));
                    ;}
    break;

  case 357:
#line 1904 "grammar.y"
    {
                        (yyval.node) = new_super(parse_state, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 358:
#line 1908 "grammar.y"
    {
                        (yyval.node) = NEW_ZSUPER();
                    ;}
    break;

  case 359:
#line 1914 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 360:
#line 1918 "grammar.y"
    { (yyval.vars) = vps->block_vars; ;}
    break;

  case 361:
#line 1920 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 362:
#line 1925 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 363:
#line 1929 "grammar.y"
    { (yyval.vars) = vps->block_vars; ;}
    break;

  case 364:
#line 1931 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 365:
#line 1940 "grammar.y"
    {
                        (yyval.node) = NEW_WHEN((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 367:
#line 1946 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, (yyvsp[(1) - (4)].node), NEW_WHEN((yyvsp[(4) - (4)].node), 0, 0));
                    ;}
    break;

  case 368:
#line 1950 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_WHEN((yyvsp[(2) - (2)].node), 0, 0));
                    ;}
    break;

  case 371:
#line 1962 "grammar.y"
    {
                        if ((yyvsp[(3) - (6)].node)) {
                            (yyvsp[(3) - (6)].node) = node_assign((yyvsp[(3) - (6)].node), NEW_GVAR(rb_intern("$!")), parse_state);
                            (yyvsp[(5) - (6)].node) = block_append(parse_state, (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node));
                        }
                        (yyval.node) = NEW_RESBODY((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (6)].node)?(yyvsp[(2) - (6)].node):(yyvsp[(5) - (6)].node));
                    ;}
    break;

  case 373:
#line 1974 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 376:
#line 1982 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 378:
#line 1989 "grammar.y"
    {
                        if ((yyvsp[(2) - (2)].node))
                            (yyval.node) = (yyvsp[(2) - (2)].node);
                        else
                            /* place holder */
                            (yyval.node) = NEW_NIL();
                    ;}
    break;

  case 381:
#line 2001 "grammar.y"
    {
                        (yyval.node) = NEW_LIT(ID2SYM((yyvsp[(1) - (1)].id)));
                    ;}
    break;

  case 383:
#line 2008 "grammar.y"
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

  case 385:
#line 2022 "grammar.y"
    {
                        (yyval.node) = literal_concat(parse_state, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 386:
#line 2028 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 387:
#line 2034 "grammar.y"
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

  case 388:
#line 2063 "grammar.y"
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

  case 389:
#line 2098 "grammar.y"
    {
                        (yyval.node) = NEW_ZARRAY();
                    ;}
    break;

  case 390:
#line 2102 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 391:
#line 2108 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 392:
#line 2112 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, (yyvsp[(1) - (3)].node), evstr2dstr(parse_state, (yyvsp[(2) - (3)].node)));
                    ;}
    break;

  case 394:
#line 2119 "grammar.y"
    {
                        (yyval.node) = literal_concat(parse_state, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 395:
#line 2125 "grammar.y"
    {
                        (yyval.node) = NEW_ZARRAY();
                    ;}
    break;

  case 396:
#line 2129 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 397:
#line 2135 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 398:
#line 2139 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, (yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 399:
#line 2145 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 400:
#line 2149 "grammar.y"
    {
                        (yyval.node) = literal_concat(parse_state, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 401:
#line 2155 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 402:
#line 2159 "grammar.y"
    {
                        (yyval.node) = literal_concat(parse_state, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 404:
#line 2166 "grammar.y"
    {
                        (yyval.node) = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                    ;}
    break;

  case 405:
#line 2172 "grammar.y"
    {
                        lex_strterm = (yyvsp[(2) - (3)].node);
                        (yyval.node) = NEW_EVSTR((yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 406:
#line 2177 "grammar.y"
    {
                        (yyval.node) = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                        COND_PUSH(0);
                        CMDARG_PUSH(0);
                    ;}
    break;

  case 407:
#line 2185 "grammar.y"
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

  case 408:
#line 2197 "grammar.y"
    {(yyval.node) = NEW_GVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 409:
#line 2198 "grammar.y"
    {(yyval.node) = NEW_IVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 410:
#line 2199 "grammar.y"
    {(yyval.node) = NEW_CVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 412:
#line 2204 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = (yyvsp[(2) - (2)].id);
                    ;}
    break;

  case 417:
#line 2217 "grammar.y"
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
                                /*
                                if (strlen($$->nd_str->str) == $$->nd_str->len) {
                                    $$->nd_lit = rb_intern($$->nd_str->str);
                                    nd_set_type($$, NODE_LIT);
                                    break;
                                }
                                */
                                /* fall through */
                              default:
                                (yyval.node) = NEW_NODE(NODE_DSYM, string_new(0, 0), 1, NEW_LIST((yyval.node)));
                                break;
                            }
                        }
                    ;}
    break;

  case 420:
#line 2247 "grammar.y"
    {
                        (yyval.node) = NEW_NEGATE((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 421:
#line 2251 "grammar.y"
    {
                        (yyval.node) = NEW_NEGATE((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 427:
#line 2261 "grammar.y"
    {(yyval.id) = kNIL;;}
    break;

  case 428:
#line 2262 "grammar.y"
    {(yyval.id) = kSELF;;}
    break;

  case 429:
#line 2263 "grammar.y"
    {(yyval.id) = kTRUE;;}
    break;

  case 430:
#line 2264 "grammar.y"
    {(yyval.id) = kFALSE;;}
    break;

  case 431:
#line 2265 "grammar.y"
    {(yyval.id) = k__FILE__;;}
    break;

  case 432:
#line 2266 "grammar.y"
    {(yyval.id) = k__LINE__;;}
    break;

  case 433:
#line 2270 "grammar.y"
    {
                        (yyval.node) = gettable((yyvsp[(1) - (1)].id));
                    ;}
    break;

  case 434:
#line 2276 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, parse_state);
                    ;}
    break;

  case 437:
#line 2286 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 438:
#line 2290 "grammar.y"
    {
                        vps->lex_state = EXPR_BEG;
                    ;}
    break;

  case 439:
#line 2294 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(3) - (4)].node);
                    ;}
    break;

  case 440:
#line 2297 "grammar.y"
    {yyerrok; (yyval.node) = 0;;}
    break;

  case 441:
#line 2301 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                        vps->lex_state = EXPR_BEG;
                    ;}
    break;

  case 442:
#line 2306 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 443:
#line 2312 "grammar.y"
    {
                        // printf("rest + all = %d\n", $5);
                        (yyval.node) = block_append(parse_state, NEW_ARGS((yyvsp[(1) - (6)].num), (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].id)), (yyvsp[(6) - (6)].node));
                    ;}
    break;

  case 444:
#line 2317 "grammar.y"
    {
                        (yyval.node) = block_append(parse_state, NEW_ARGS((yyvsp[(1) - (4)].num), (yyvsp[(3) - (4)].node), -1), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 445:
#line 2321 "grammar.y"
    {
                        // printf("arg + rest = %d\n", $3);
                        (yyval.node) = block_append(parse_state, NEW_ARGS((yyvsp[(1) - (4)].num), 0, (yyvsp[(3) - (4)].id)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 446:
#line 2326 "grammar.y"
    {
                        (yyval.node) = block_append(parse_state, NEW_ARGS((yyvsp[(1) - (2)].num), 0, -1), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 447:
#line 2330 "grammar.y"
    {
                        // printf("rest + opt = %d\n", $3);
                        (yyval.node) = block_append(parse_state, NEW_ARGS(0, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 448:
#line 2335 "grammar.y"
    {
                        (yyval.node) = block_append(parse_state, NEW_ARGS(0, (yyvsp[(1) - (2)].node), -1), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 449:
#line 2339 "grammar.y"
    {
                        // printf("rest only = %d\n", $1);
                        (yyval.node) = block_append(parse_state, NEW_ARGS(0, 0, (yyvsp[(1) - (2)].id)), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 450:
#line 2344 "grammar.y"
    {
                        (yyval.node) = block_append(parse_state, NEW_ARGS(0, 0, -1), (yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 451:
#line 2348 "grammar.y"
    {
                        (yyval.node) = NEW_ARGS(0, 0, -1);
                    ;}
    break;

  case 452:
#line 2354 "grammar.y"
    {
                        yyerror("formal argument cannot be a constant");
                    ;}
    break;

  case 453:
#line 2358 "grammar.y"
    {
                        yyerror("formal argument cannot be an instance variable");
                    ;}
    break;

  case 454:
#line 2362 "grammar.y"
    {
                        yyerror("formal argument cannot be a global variable");
                    ;}
    break;

  case 455:
#line 2366 "grammar.y"
    {
                        yyerror("formal argument cannot be a class variable");
                    ;}
    break;

  case 456:
#line 2370 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(1) - (1)].id)))
                            yyerror("formal argument must be local variable");
                        else if (local_id((yyvsp[(1) - (1)].id)))
                            yyerror("duplicate argument name");
                        local_cnt((yyvsp[(1) - (1)].id));
                        (yyval.num) = 1;
                    ;}
    break;

  case 458:
#line 2382 "grammar.y"
    {
                        (yyval.num) += 1;
                    ;}
    break;

  case 459:
#line 2388 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(1) - (3)].id)))
                            yyerror("formal argument must be local variable");
                        else if (local_id((yyvsp[(1) - (3)].id)))
                            yyerror("duplicate optional argument name");
                        (yyval.node) = assignable((yyvsp[(1) - (3)].id), (yyvsp[(3) - (3)].node), parse_state);
                    ;}
    break;

  case 460:
#line 2398 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK((yyvsp[(1) - (1)].node));
                        (yyval.node)->nd_end = (yyval.node);
                    ;}
    break;

  case 461:
#line 2403 "grammar.y"
    {
                        (yyval.node) = block_append(parse_state, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 464:
#line 2413 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(2) - (2)].id)))
                            yyerror("rest argument must be local variable");
                        else if (local_id((yyvsp[(2) - (2)].id)))
                            yyerror("duplicate rest argument name");
                        (yyval.id) = local_cnt((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 465:
#line 2421 "grammar.y"
    {
                        (yyval.id) = -2;
                    ;}
    break;

  case 468:
#line 2431 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(2) - (2)].id)))
                            yyerror("block argument must be local variable");
                        else if (local_id((yyvsp[(2) - (2)].id)))
                            yyerror("duplicate block argument name");
                        (yyval.node) = NEW_BLOCK_ARG((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 469:
#line 2441 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 471:
#line 2448 "grammar.y"
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

  case 472:
#line 2457 "grammar.y"
    {vps->lex_state = EXPR_BEG;;}
    break;

  case 473:
#line 2458 "grammar.y"
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

  case 475:
#line 2484 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 476:
#line 2488 "grammar.y"
    {
                        if ((yyvsp[(1) - (2)].node)->nd_alen%2 != 0) {
                            yyerror("odd number list for Hash");
                        }
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 478:
#line 2498 "grammar.y"
    {
                        (yyval.node) = list_concat((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 479:
#line 2504 "grammar.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 480:
#line 2506 "grammar.y"
    { (yyval.node)  = (yyvsp[(1) - (1)].node); ;}
    break;

  case 481:
#line 2510 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, NEW_LIST((yyvsp[(1) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 482:
#line 2516 "grammar.y"
    {
                        (yyval.node) = list_append(parse_state, NEW_LIST(NEW_LIT(ID2SYM((yyvsp[(1) - (2)].id)))), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 502:
#line 2554 "grammar.y"
    {yyerrok;;}
    break;

  case 505:
#line 2559 "grammar.y"
    {yyerrok;;}
    break;

  case 506:
#line 2562 "grammar.y"
    {(yyval.node) = 0;;}
    break;


/* Line 1267 of yacc.c.  */
#line 7433 "grammar.c"
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


#line 2564 "grammar.y"


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

#undef string_new
#define string_new(ptr, len) g_string_new_len(ptr, len)

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

static GString*
lex_get_str(parse_state)
    rb_parse_state *parse_state;
{
    char *str;
    char *beg, *end, *pend;
    GString *output;
    int sz;

    str = parse_state->lex_string->str;
    beg = str;
    
    if(parse_state->lex_str_used) {
        if((int)parse_state->lex_string->len == parse_state->lex_str_used) return NULL;
        beg += parse_state->lex_str_used;
    }
    
    pend = str + parse_state->lex_string->len;
    end = beg;
    
    while(end < pend) {
      if(*end++ == '\n') break;
    }
    
    sz = end - beg;
    output = g_string_new_len(beg, sz);
    parse_state->lex_str_used += sz;

    return output;
}

void syd_add_to_parse_tree(STATE, OBJECT ary,
      NODE * n, int newlines, ID * locals, int line_numbers);

static OBJECT convert_to_sexp(STATE, NODE *node, int newlines) {
  OBJECT ary;
  ary = array_new(state, 1);
  syd_add_to_parse_tree(state, ary, node, newlines, NULL, FALSE);
  return array_get(state, ary, 0);
}

static GString*
lex_getline(rb_parse_state *parse_state)
{
  GString *out;
  /* TODO: Should the current line_buffer be freed here? */
  out = (parse_state->lex_gets)(parse_state);
  parse_state->line_buffer = out;
  return out;
}

OBJECT
syd_compile_string(STATE, const char *f, GString *s, int line, int newlines)
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

static GString* parse_io_gets(rb_parse_state *parse_state) {
  GString *output;
  GError *err;
  output = g_string_new(NULL);
  err = NULL;
  g_io_channel_read_line_string(parse_state->lex_io, output, NULL, &err);
  
  return output;
}

OBJECT
syd_compile_file(STATE, const char *f, GIOChannel *file, int start, int newlines)
{
    int n;
    rb_parse_state *parse_state;
    parse_state = alloc_parse_state();

    parse_state->lex_io = file;
    parse_state->lex_gets = parse_io_gets;
    parse_state->lex_pbeg = 0;
    parse_state->lex_p = 0;
    parse_state->lex_pend = 0;
    ruby_sourceline = start - 1;

    n = yycompile(parse_state, f, start);
    
    if(!n) {
        // ruby_eval_tree = parse_state->top;
        return convert_to_sexp(state, parse_state->top, newlines);
    } else {
        // ruby_eval_tree_begin = 0;
        return Qfalse;
    }
}

#define nextc() ps_nextc(parse_state)

static inline int
ps_nextc(rb_parse_state *parse_state)
{
    int c;

    if (parse_state->lex_p == parse_state->lex_pend) {
        GString *v = lex_getline(parse_state);
        
        if (!v) return -1;
        if (heredoc_end > 0) {
            ruby_sourceline = heredoc_end;
            heredoc_end = 0;
        }
        ruby_sourceline++;
        parse_state->lex_pbeg = parse_state->lex_p = v->str;
        parse_state->lex_pend = parse_state->lex_p + v->len;
        parse_state->lex_lastline = v;
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

static void
dispose_string(str)
    OBJECT str;
{
    /* FIXME!
    free(RSTRING(str)->ptr);
    rb_gc_force_recycle(str);
    */
}

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
    lex_strterm = syd_node_newnode(parse_state, NODE_HEREDOC,
                                  (OBJECT)string_new(tok(), toklen()),  /* nd_lit */
                                  len,                          /* nd_nth */
                                  (OBJECT)parse_state->lex_lastline);    /* nd_orig */
    return term == '`' ? tXSTRING_BEG : tSTRING_BEG;
}

static void
heredoc_restore(here, parse_state)
    NODE *here;
    rb_parse_state *parse_state;
{
    GString *line = here->nd_orig;
    parse_state->lex_lastline = line;
    parse_state->lex_pbeg = line->str;
    parse_state->lex_pend = parse_state->lex_pbeg + line->len;
    parse_state->lex_p = parse_state->lex_pbeg + here->nd_nth;
    heredoc_end = ruby_sourceline;
    ruby_sourceline = nd_line(here);
    dispose_string(here->nd_lit);
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
    GString *str = NULL;

    /* eos == the heredoc ident that we found when the heredoc started */
    eos = here->nd_str->str;
    len = here->nd_str->len - 1;
    
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
            p = parse_state->lex_lastline->str;
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
                g_string_append_len(str, p, pend - p);
            else
                str = string_new(p, pend - p);
            if (pend < parse_state->lex_pend) g_string_append_len(str, "\n", 1);
            parse_state->lex_p = parse_state->lex_pend;
            if (nextc() == -1) {
                if (str) dispose_string(str);
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

#include "grammer_lex.c.tab"

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
    GString *cur_line;
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
    command_start = Qfalse;
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
            int i = 0;
            comment_column = parse_state->column;
            cur_line = string_new(0, 50);
            
            while((c = nextc()) != '\n' && c != -1) {
                cur_line->str[i++] = c;
                if(i == (int)cur_line->len) {
                    g_string_set_size(cur_line, i+50);
                }
            }
            
            g_string_set_size(cur_line, i);
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
        command_start = Qtrue;
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
        command_start = Qtrue;
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
        command_start = Qtrue;
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
                    if(parse_state->ternary_colon) {
                        result = tIDENTIFIER;
                    } else if((c = nextc()) == ':' && !peek(':')) {
                        /*
                        c = nextc();
                        printf("keysym: %s, %c\n", tok(), c);
                        pushback(c, parse_state);
                        */
                        //printf("lex_state %d, %d, %d\n", parse_state->lex_state, 
                        //    EXPR_END, EXPR_BEG);
                        switch (parse_state->lex_state) {
                          case EXPR_FNAME: case EXPR_DOT:
                            parse_state->lex_state = EXPR_ARG; break;
                          default:
                            parse_state->lex_state = EXPR_BEG; break;
                        }
                        pslval->id = rb_intern(tok());
                        return tKEYSYM;

                    } else {
                        pushback(c, parse_state);
                        result = tIDENTIFIER;
                    }
                }
            }

            if (parse_state->lex_state != EXPR_DOT) {
                struct kwtable *kw;

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


NODE*
syd_node_newnode(st, type, a0, a1, a2)
    rb_parse_state *st;
    enum node_type type;
    OBJECT a0, a1, a2;
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

#ifdef DEBUG
static enum node_type
nodetype(node)                  /* for debug */
    NODE *node;
{
    return (enum node_type)nd_type(node);
}

static int
nodeline(node)
    NODE *node;
{
    return nd_line(node);
}

#endif

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
parser_warning(node, mesg)
    NODE *node;
    const char *mesg;
{
    int line = ruby_sourceline;
    ruby_sourceline = nd_line(node);
    printf("Warning: %s\n", mesg);
    ruby_sourceline = line;
}

static void
parser_warn(node, mesg)
    NODE *node;
    const char *mesg;
{
    int line = ruby_sourceline;
    ruby_sourceline = nd_line(node);
    printf("Warning: %s\n", mesg);
    ruby_sourceline = line;
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
      case NODE_LIT:
      case NODE_STR:
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

    if (RTEST(ruby_verbose)) {
        NODE *nd = end->nd_head;
      newline:
        switch (nd_type(nd)) {
          case NODE_RETURN:
          case NODE_BREAK:
          case NODE_NEXT:
          case NODE_REDO:
          case NODE_RETRY:
            parser_warning(nd, "statement not reached");
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
            g_string_append_string(head->nd_str, tail->nd_str);
            // rb_gc_force_recycle((VALUE)tail);
        }
        else {
            list_append(parse_state, head, tail);
        }
        break;

      case NODE_DSTR:
        if (htype == NODE_STR) {
            g_string_append_string(head->nd_str, tail->nd_str);
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

static struct {
    ID token;
    char *name;
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
    {0, 0}
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
  parse_state->find_block_args++;
  parse_state->block_vars = var_table_create();
}

static NODE *
extract_block_vars(rb_parse_state *parse_state, NODE* node, var_table *vars)
{
    int i;
    // struct RVarmap *post = ruby_dyna_vars;
    NODE *var, *out;
        
    if (!node) return node;
    if(vars->size == 0) return node;
        
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
    parse_state->find_block_args--;
    out = block_append(parse_state, var, node);
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
        if(parse_state->find_block_args) {
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


ID
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
        rb_compile_error("Can't set variable $%lu", node->nd_nth);
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
            parser_warning(node, "void value expression");
            return Qfalse;

          case NODE_RETURN:
          case NODE_BREAK:
          case NODE_NEXT:
          case NODE_REDO:
          case NODE_RETRY:
            if (!cond) yyerror("void value expression");
            /* or "control never reach"? */
            return Qfalse;

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
            if (!value_expr(node->nd_body)) return Qfalse;
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
            return Qtrue;
        }
    }

    return Qtrue;
}

static void
void_expr0(node)
    NODE *node;
{
    char *useless = 0;

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
        parser_warning(node->nd_value, "assignment in condition");
    }
#endif
    return 1;
}

static int
e_option_supplied()
{
    if (strcmp(ruby_sourcefile, "-e") == 0)
        return Qtrue;
    return Qfalse;
}

static void
warn_unless_e_option(node, str)
    NODE *node;
    const char *str;
{
    if (!e_option_supplied()) parser_warn(node, str);
}

static void
warning_unless_e_option(node, str)
    NODE *node;
    const char *str;
{
    if (!e_option_supplied()) parser_warning(node, str);
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
        warn_unless_e_option(node, "integer literal in conditional range");
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
    lcl_tbl = malloc(sizeof(ID) * (len + 3));
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

int
syd_parser_stack_on_heap()
{
#if defined(YYBISON) && !defined(C_ALLOCA)
    return Qfalse;
#else
    return Qtrue;
#endif
}

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


ID syd_last_id;

void setup_parser() {
    syd_last_id = tLAST_TOKEN;
}

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

ID
rb_intern(char *name)
{
    const char *m = name;
    ID id, pre;
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
            char *buf = (char*)malloc(sizeof(char) * (last+1));

            strncpy(buf, name, last);
            buf[last] = '\0';
            id = rb_intern(buf);
            free(buf);
            if (id > tLAST_TOKEN && !is_attrset_id(id)) {
                id = rb_id_attrset(id);
                goto id_regist;
            }
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
  id_regist:
    pre = g_quark_from_string(name) + tLAST_TOKEN;
    id |= ( pre << ID_SCOPE_SHIFT );
    //printf("Registered '%s' as %d.\n", name, id);
    return id;
}

GQuark id_to_quark(ID id) {
  return((id >> ID_SCOPE_SHIFT) - tLAST_TOKEN);
}

unsigned long
scan_oct(start, len, retlen)
    const char *start;
    int len;
    int *retlen;
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

unsigned long
scan_hex(start, len, retlen)
    const char *start;
    int len;
    int *retlen;
{
    static char hexdigit[] = "0123456789abcdef0123456789ABCDEF";
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

char *op_to_name(ID id) {
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


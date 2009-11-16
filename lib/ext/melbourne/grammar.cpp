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
#line 282 "grammar.y"
{
    NODE *node;
    QUID id;
    int num;
    var_table vars;
}
/* Line 193 of yacc.c.  */
#line 584 "grammar.cpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 597 "grammar.cpp"

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
#define YYLAST   10056

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  136
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  146
/* YYNRULES -- Number of rules.  */
#define YYNRULES  508
/* YYNRULES -- Number of states.  */
#define YYNSTATES  906

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
     971,   973,   976,   983,   990,   991,   992,  1000,  1001,  1002,
    1010,  1016,  1021,  1027,  1028,  1029,  1039,  1040,  1047,  1048,
    1049,  1058,  1059,  1065,  1066,  1073,  1074,  1075,  1085,  1087,
    1089,  1091,  1093,  1095,  1097,  1099,  1101,  1104,  1106,  1108,
    1110,  1112,  1118,  1120,  1123,  1125,  1127,  1129,  1132,  1134,
    1138,  1139,  1140,  1147,  1150,  1155,  1160,  1163,  1168,  1173,
    1177,  1181,  1184,  1187,  1189,  1190,  1191,  1198,  1199,  1200,
    1207,  1213,  1215,  1220,  1223,  1225,  1227,  1234,  1236,  1238,
    1240,  1242,  1245,  1247,  1250,  1252,  1254,  1256,  1258,  1260,
    1262,  1265,  1269,  1273,  1277,  1281,  1285,  1286,  1290,  1292,
    1295,  1299,  1303,  1304,  1308,  1309,  1312,  1313,  1316,  1318,
    1319,  1323,  1324,  1329,  1331,  1333,  1335,  1337,  1340,  1342,
    1344,  1346,  1348,  1352,  1354,  1356,  1359,  1362,  1364,  1366,
    1368,  1370,  1372,  1374,  1376,  1378,  1380,  1382,  1384,  1386,
    1388,  1390,  1392,  1394,  1395,  1400,  1403,  1408,  1411,  1418,
    1423,  1428,  1431,  1436,  1439,  1442,  1444,  1445,  1447,  1449,
    1451,  1453,  1455,  1457,  1461,  1465,  1467,  1471,  1473,  1475,
    1478,  1480,  1482,  1484,  1487,  1490,  1492,  1494,  1495,  1501,
    1503,  1506,  1509,  1511,  1515,  1519,  1521,  1523,  1525,  1527,
    1529,  1531,  1533,  1535,  1537,  1539,  1541,  1543,  1544,  1546,
    1547,  1549,  1550,  1552,  1554,  1556,  1558,  1560,  1563
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     137,     0,    -1,    -1,   138,   140,   141,    -1,   140,   225,
     209,   228,    -1,   142,   276,    -1,   281,    -1,    50,    61,
      -1,   281,    -1,   143,    -1,   142,   280,   143,    -1,     1,
     143,    -1,    -1,    44,   165,   144,   165,    -1,    44,    53,
      53,    -1,    44,    53,    63,    -1,    44,    53,    62,    -1,
       6,   166,    -1,   143,    39,   147,    -1,   143,    40,   147,
      -1,   143,    41,   147,    -1,   143,    42,   147,    -1,   143,
      43,   143,    -1,    -1,    46,   145,   123,   140,   124,    -1,
      47,   123,   140,   124,    -1,   160,   106,   148,    -1,   154,
     106,   148,    -1,   252,    87,   148,    -1,   205,   125,   174,
     126,    87,   148,    -1,   205,   127,    51,    87,   148,    -1,
     205,   127,    55,    87,   148,    -1,   205,    85,    51,    87,
     148,    -1,   253,    87,   148,    -1,   160,   106,   187,    -1,
     154,   106,   173,    -1,   154,   106,   187,    -1,   146,    -1,
     148,    -1,   146,    36,   146,    -1,   146,    37,   146,    -1,
      38,   146,    -1,   120,   148,    -1,   170,    -1,   146,    -1,
     153,    -1,   149,    -1,    29,   177,    -1,    21,   177,    -1,
      22,   177,    -1,   215,    -1,   215,   127,   273,   179,    -1,
     215,    85,   273,   179,    -1,    -1,    -1,    94,   151,   211,
     152,   140,   124,    -1,   272,   179,    -1,   272,   179,   150,
      -1,   205,   127,   273,   179,    -1,   205,   127,   273,   179,
     150,    -1,   205,    85,   273,   179,    -1,   205,    85,   273,
     179,   150,    -1,    31,   179,    -1,    30,   179,    -1,   156,
      -1,    89,   155,   128,    -1,   156,    -1,    89,   155,   128,
      -1,   158,    -1,   158,   157,    -1,   158,    95,   159,    -1,
     158,    95,    -1,    95,   159,    -1,    95,    -1,   159,    -1,
      89,   155,   128,    -1,   157,   129,    -1,   158,   157,   129,
      -1,   250,    -1,   205,   125,   174,   126,    -1,   205,   127,
      51,    -1,   205,    85,    51,    -1,   205,   127,    55,    -1,
     205,    85,    55,    -1,    86,    55,    -1,   253,    -1,   250,
      -1,   205,   125,   174,   126,    -1,   205,   127,    51,    -1,
     205,    85,    51,    -1,   205,   127,    55,    -1,   205,    85,
      55,    -1,    86,    55,    -1,   253,    -1,    51,    -1,    55,
      -1,    86,   161,    -1,   161,    -1,   205,    85,   161,    -1,
      51,    -1,    55,    -1,    52,    -1,   168,    -1,   169,    -1,
     163,    -1,   246,    -1,   164,    -1,   248,    -1,   165,    -1,
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
     170,    -1,   252,    87,   170,    -1,   205,   125,   174,   126,
      87,   170,    -1,   205,   127,    51,    87,   170,    -1,   205,
     127,    55,    87,   170,    -1,   205,    85,    51,    87,   170,
      -1,   205,    85,    55,    87,   170,    -1,    86,    55,    87,
     170,    -1,   253,    87,   170,    -1,   170,    79,   170,    -1,
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
     277,   171,   170,    -1,    -1,   170,   107,   172,   170,   108,
     170,    -1,   188,    -1,   170,    -1,   281,    -1,   153,   277,
      -1,   186,   278,    -1,   186,   129,    95,   170,   277,    -1,
     270,   278,    -1,    95,   170,   277,    -1,   131,   281,   128,
      -1,   131,   177,   277,   128,    -1,   131,   215,   277,   128,
      -1,   131,   186,   129,   215,   277,   128,    -1,   281,    -1,
     175,    -1,   153,    -1,   186,   185,    -1,   186,   129,    95,
     173,   185,    -1,   270,   185,    -1,   270,   129,    95,   173,
     185,    -1,   186,   129,   270,   185,    -1,   186,   129,   270,
     129,    95,   170,   185,    -1,    95,   173,   185,    -1,   184,
      -1,   173,   129,   186,   185,    -1,   173,   129,   184,    -1,
     173,   129,    95,   173,   185,    -1,   173,   129,   186,   129,
      95,   173,   185,    -1,   270,   185,    -1,   270,   129,    95,
     173,   185,    -1,   173,   129,   270,   185,    -1,   173,   129,
     186,   129,   270,   185,    -1,   173,   129,   270,   129,    95,
     173,   185,    -1,   173,   129,   186,   129,   270,   129,    95,
     173,   185,    -1,    95,   173,   185,    -1,   184,    -1,    -1,
     180,   181,    -1,   177,    -1,    -1,    90,   182,   128,    -1,
      -1,    90,   178,   183,   128,    -1,    96,   173,    -1,   129,
     184,    -1,   281,    -1,   173,    -1,   186,   129,   173,    -1,
     186,   129,   173,    -1,   186,   129,    95,   173,    -1,    95,
     173,    -1,   229,    -1,   230,    -1,   233,    -1,   234,    -1,
     235,    -1,   238,    -1,   251,    -1,   253,    -1,    52,    -1,
      -1,     7,   189,   139,    10,    -1,    -1,    90,   146,   190,
     277,   128,    -1,    89,   140,   128,    -1,   205,    85,    55,
      -1,    86,    55,    -1,   205,   125,   174,   126,    -1,    92,
     174,   126,    -1,    93,   269,   124,    -1,    29,    -1,    30,
     131,   177,   128,    -1,    30,   131,   128,    -1,    30,    -1,
      -1,    45,   277,   131,   191,   146,   128,    -1,   272,   217,
      -1,   216,    -1,   216,   217,    -1,    11,   147,   206,   140,
     208,    10,    -1,    12,   147,   206,   140,   209,    10,    -1,
      -1,    -1,    18,   192,   147,   207,   193,   140,    10,    -1,
      -1,    -1,    19,   194,   147,   207,   195,   140,    10,    -1,
      16,   147,   276,   222,    10,    -1,    16,   276,   222,    10,
      -1,    16,   276,    15,   140,    10,    -1,    -1,    -1,    20,
     210,    25,   196,   147,   207,   197,   140,    10,    -1,    -1,
       3,   162,   254,   198,   139,    10,    -1,    -1,    -1,     3,
      83,   146,   199,   279,   200,   139,    10,    -1,    -1,     4,
     162,   201,   139,    10,    -1,    -1,     5,   163,   202,   256,
     139,    10,    -1,    -1,    -1,     5,   267,   275,   203,   163,
     204,   256,   139,    10,    -1,    21,    -1,    22,    -1,    23,
      -1,    24,    -1,   188,    -1,   279,    -1,   108,    -1,    13,
      -1,   279,    13,    -1,   279,    -1,   108,    -1,    27,    -1,
     209,    -1,    14,   147,   206,   140,   208,    -1,   281,    -1,
      15,   140,    -1,   160,    -1,   154,    -1,   281,    -1,   111,
     111,    -1,    76,    -1,   111,   210,   111,    -1,    -1,    -1,
      28,   213,   211,   214,   140,    10,    -1,   153,   212,    -1,
     215,   127,   273,   176,    -1,   215,    85,   273,   176,    -1,
     272,   175,    -1,   205,   127,   273,   176,    -1,   205,    85,
     273,   175,    -1,   205,    85,   274,    -1,   205,   132,   273,
      -1,    67,   273,    -1,    31,   175,    -1,    31,    -1,    -1,
      -1,   123,   218,   211,   219,   140,   124,    -1,    -1,    -1,
      26,   220,   211,   221,   140,    10,    -1,    17,   223,   206,
     140,   224,    -1,   186,    -1,   186,   129,    95,   173,    -1,
      95,   173,    -1,   209,    -1,   222,    -1,     8,   226,   227,
     206,   140,   225,    -1,   281,    -1,   173,    -1,   187,    -1,
     281,    -1,    88,   160,    -1,   281,    -1,     9,   140,    -1,
     281,    -1,   249,    -1,   246,    -1,   248,    -1,   231,    -1,
     232,    -1,   231,   232,    -1,    98,   240,   104,    -1,    57,
     241,   104,    -1,    99,   241,    64,    -1,   100,   133,   104,
      -1,   100,   236,   104,    -1,    -1,   236,   237,   133,    -1,
     242,    -1,   237,   242,    -1,   101,   133,   104,    -1,   101,
     239,   104,    -1,    -1,   239,    60,   133,    -1,    -1,   240,
     242,    -1,    -1,   241,   242,    -1,    60,    -1,    -1,   103,
     243,   245,    -1,    -1,   102,   244,   140,   124,    -1,    53,
      -1,    54,    -1,    56,    -1,   253,    -1,    97,   247,    -1,
     163,    -1,    54,    -1,    53,    -1,    56,    -1,    97,   241,
     104,    -1,    58,    -1,    59,    -1,   119,    58,    -1,   119,
      59,    -1,    51,    -1,    54,    -1,    53,    -1,    55,    -1,
      56,    -1,    33,    -1,    32,    -1,    34,    -1,    35,    -1,
      49,    -1,    48,    -1,   250,    -1,   250,    -1,    62,    -1,
      63,    -1,   279,    -1,    -1,   110,   255,   147,   279,    -1,
       1,   279,    -1,   131,   257,   277,   128,    -1,   257,   279,
      -1,   259,   129,   261,   129,   263,   266,    -1,   259,   129,
     261,   266,    -1,   259,   129,   263,   266,    -1,   259,   266,
      -1,   261,   129,   263,   266,    -1,   261,   266,    -1,   263,
     266,    -1,   265,    -1,    -1,    55,    -1,    54,    -1,    53,
      -1,    56,    -1,    51,    -1,   258,    -1,   259,   129,   258,
      -1,    51,   106,   173,    -1,   260,    -1,   261,   129,   260,
      -1,   116,    -1,    95,    -1,   262,    51,    -1,   262,    -1,
     113,    -1,    96,    -1,   264,    51,    -1,   129,   265,    -1,
     281,    -1,   251,    -1,    -1,   131,   268,   146,   277,   128,
      -1,   281,    -1,   270,   278,    -1,   186,   278,    -1,   271,
      -1,   270,   129,   271,    -1,   173,    88,   173,    -1,    51,
      -1,    55,    -1,    52,    -1,    51,    -1,    55,    -1,    52,
      -1,   168,    -1,    51,    -1,    52,    -1,   168,    -1,   127,
      -1,    85,    -1,    -1,   280,    -1,    -1,   134,    -1,    -1,
     134,    -1,   129,    -1,   135,    -1,   134,    -1,   279,    -1,
     280,   135,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   420,   420,   420,   445,   465,   472,   473,   478,   479,
     483,   487,   493,   493,   497,   501,   508,   513,   517,   526,
     535,   547,   559,   565,   564,   580,   588,   592,   598,   623,
     639,   651,   663,   675,   680,   684,   689,   694,   697,   698,
     702,   706,   710,   714,   717,   724,   725,   726,   730,   734,
     740,   741,   745,   752,   756,   751,   765,   770,   782,   787,
     799,   804,   816,   821,   828,   829,   835,   836,   842,   846,
     850,   854,   858,   862,   868,   869,   875,   879,   885,   889,
     893,   897,   901,   905,   911,   917,   924,   928,   932,   936,
     940,   944,   950,   956,   963,   967,   970,   974,   978,   984,
     985,   986,   987,   992,   999,  1000,  1003,  1007,  1010,  1014,
    1014,  1020,  1021,  1022,  1023,  1024,  1025,  1026,  1027,  1028,
    1029,  1030,  1031,  1032,  1033,  1034,  1035,  1036,  1037,  1038,
    1039,  1040,  1041,  1042,  1043,  1044,  1045,  1048,  1048,  1048,
    1048,  1048,  1049,  1049,  1049,  1049,  1049,  1049,  1049,  1050,
    1050,  1050,  1050,  1050,  1050,  1050,  1051,  1051,  1051,  1051,
    1051,  1051,  1052,  1052,  1052,  1052,  1052,  1052,  1052,  1053,
    1053,  1053,  1053,  1053,  1054,  1054,  1054,  1054,  1054,  1057,
    1061,  1065,  1090,  1106,  1118,  1130,  1142,  1147,  1152,  1157,
    1163,  1169,  1173,  1177,  1181,  1185,  1189,  1193,  1197,  1201,
    1210,  1214,  1218,  1222,  1226,  1230,  1234,  1238,  1242,  1246,
    1250,  1254,  1258,  1262,  1266,  1270,  1274,  1278,  1282,  1286,
    1290,  1290,  1295,  1295,  1301,  1307,  1314,  1315,  1320,  1324,
    1329,  1333,  1340,  1344,  1348,  1353,  1360,  1361,  1364,  1369,
    1373,  1378,  1383,  1388,  1393,  1399,  1403,  1406,  1410,  1414,
    1419,  1424,  1429,  1434,  1439,  1444,  1449,  1455,  1459,  1462,
    1462,  1474,  1475,  1475,  1480,  1480,  1487,  1493,  1497,  1500,
    1504,  1510,  1514,  1518,  1524,  1525,  1526,  1527,  1528,  1529,
    1530,  1531,  1532,  1537,  1536,  1549,  1549,  1554,  1559,  1563,
    1567,  1576,  1585,  1589,  1593,  1597,  1601,  1605,  1605,  1610,
    1616,  1617,  1626,  1639,  1652,  1652,  1652,  1662,  1662,  1662,
    1672,  1679,  1683,  1687,  1687,  1687,  1695,  1694,  1711,  1716,
    1710,  1733,  1732,  1749,  1748,  1766,  1767,  1766,  1781,  1785,
    1789,  1793,  1799,  1806,  1807,  1808,  1809,  1812,  1813,  1814,
    1817,  1818,  1827,  1828,  1834,  1835,  1838,  1839,  1843,  1847,
    1854,  1859,  1853,  1870,  1879,  1883,  1889,  1894,  1899,  1904,
    1908,  1913,  1918,  1922,  1929,  1933,  1928,  1940,  1944,  1939,
    1952,  1959,  1960,  1964,  1970,  1971,  1974,  1985,  1988,  1992,
    1993,  1996,  2000,  2003,  2011,  2014,  2015,  2019,  2022,  2035,
    2036,  2042,  2048,  2071,  2101,  2105,  2112,  2115,  2121,  2122,
    2128,  2132,  2139,  2142,  2149,  2152,  2159,  2162,  2168,  2170,
    2169,  2181,  2180,  2200,  2201,  2202,  2203,  2206,  2213,  2214,
    2215,  2216,  2219,  2250,  2251,  2252,  2256,  2262,  2263,  2264,
    2265,  2266,  2267,  2268,  2269,  2270,  2271,  2272,  2275,  2281,
    2287,  2288,  2291,  2296,  2295,  2303,  2306,  2312,  2318,  2322,
    2326,  2330,  2334,  2338,  2342,  2346,  2351,  2356,  2360,  2364,
    2368,  2372,  2383,  2384,  2390,  2400,  2405,  2411,  2412,  2415,
    2423,  2429,  2430,  2433,  2443,  2447,  2450,  2455,  2455,  2480,
    2481,  2485,  2494,  2495,  2501,  2507,  2508,  2509,  2512,  2513,
    2514,  2515,  2518,  2519,  2520,  2523,  2524,  2527,  2528,  2531,
    2532,  2535,  2536,  2537,  2540,  2541,  2544,  2545,  2548
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
     188,   188,   188,   188,   192,   193,   188,   194,   195,   188,
     188,   188,   188,   196,   197,   188,   198,   188,   199,   200,
     188,   201,   188,   202,   188,   203,   204,   188,   188,   188,
     188,   188,   205,   206,   206,   206,   206,   207,   207,   207,
     208,   208,   209,   209,   210,   210,   211,   211,   211,   211,
     213,   214,   212,   215,   215,   215,   216,   216,   216,   216,
     216,   216,   216,   216,   218,   219,   217,   220,   221,   217,
     222,   223,   223,   223,   224,   224,   225,   225,   226,   226,
     226,   227,   227,   228,   228,   229,   229,   229,   230,   231,
     231,   232,   233,   234,   235,   235,   236,   236,   237,   237,
     238,   238,   239,   239,   240,   240,   241,   241,   242,   243,
     242,   244,   242,   245,   245,   245,   245,   246,   247,   247,
     247,   247,   248,   249,   249,   249,   249,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   251,   252,
     253,   253,   254,   255,   254,   254,   256,   256,   257,   257,
     257,   257,   257,   257,   257,   257,   257,   258,   258,   258,
     258,   258,   259,   259,   260,   261,   261,   262,   262,   263,
     263,   264,   264,   265,   266,   266,   267,   268,   267,   269,
     269,   269,   270,   270,   271,   272,   272,   272,   273,   273,
     273,   273,   274,   274,   274,   275,   275,   276,   276,   277,
     277,   278,   278,   278,   279,   279,   280,   280,   281
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
       1,     2,     6,     6,     0,     0,     7,     0,     0,     7,
       5,     4,     5,     0,     0,     9,     0,     6,     0,     0,
       8,     0,     5,     0,     6,     0,     0,     9,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     5,     1,     2,     1,     1,     1,     2,     1,     3,
       0,     0,     6,     2,     4,     4,     2,     4,     4,     3,
       3,     2,     2,     1,     0,     0,     6,     0,     0,     6,
       5,     1,     4,     2,     1,     1,     6,     1,     1,     1,
       1,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       2,     3,     3,     3,     3,     3,     0,     3,     1,     2,
       3,     3,     0,     3,     0,     2,     0,     2,     1,     0,
       3,     0,     4,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     3,     1,     1,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     4,     2,     4,     2,     6,     4,
       4,     2,     4,     2,     2,     1,     0,     1,     1,     1,
       1,     1,     1,     3,     3,     1,     3,     1,     1,     2,
       1,     1,     1,     2,     2,     1,     1,     0,     5,     1,
       2,     2,     1,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     1,     0,
       1,     0,     1,     1,     1,     1,     1,     2,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     0,     1,     0,     0,     0,     0,     0,   283,
       0,     0,   497,   304,   307,     0,   328,   329,   330,   331,
     293,   296,   363,   433,   432,   434,   435,     0,     0,   499,
      23,     0,   437,   436,   427,   282,   429,   428,   430,   431,
     406,   423,   424,   440,   441,     0,     0,     0,     0,     0,
       0,   508,   508,    73,   406,   404,   406,   396,   402,     0,
       0,     0,   508,   497,     9,    37,    38,    46,    45,     0,
      64,     0,    68,    74,     0,    43,   224,     0,    50,   300,
     274,   275,   388,   389,   276,   277,   278,   279,   386,   387,
     385,   438,   280,     0,   281,   259,     8,    11,   328,   329,
     293,   296,   363,   499,    94,    95,     0,     0,     0,     0,
      97,     0,   332,     0,   438,   281,     0,   321,   147,   158,
     148,   171,   144,   164,   154,   153,   169,   152,   151,   146,
     172,   156,   145,   159,   163,   165,   157,   150,   166,   173,
     168,   167,   160,   170,   155,   143,   162,   161,   174,   175,
     176,   177,   178,   142,   149,   139,   140,   137,   138,   141,
      99,   101,   100,   132,   133,   130,   114,   115,   116,   119,
     121,   117,   134,   135,   122,   123,   127,   118,   120,   111,
     112,   113,   124,   125,   126,   128,   129,   131,   136,   477,
     323,   102,   103,   476,     0,   167,   160,   170,   155,   137,
     138,    99,   100,   104,   106,   108,    17,   105,   107,     0,
       0,    44,     0,     0,     0,   438,     0,   281,     0,   505,
     504,   497,     0,   506,   498,     0,     0,     0,   345,   344,
       0,     0,   438,   281,     0,     0,     0,   238,   225,   269,
      48,   246,   508,   508,   482,    49,    47,     0,    63,     0,
     508,   362,    62,    41,     0,    12,   500,   220,     0,     0,
       0,   199,     0,   200,   488,   490,   489,   491,   361,   289,
       0,     0,     0,    64,   285,     0,   499,     0,   501,   501,
     226,   501,     0,   501,   479,     0,    72,     0,    78,    85,
     420,   419,   421,   418,     0,   417,     0,     0,     0,     0,
       0,     0,   425,   426,    42,   214,   215,     0,     3,     6,
       5,   498,     0,     0,     0,     0,     0,     0,     0,   350,
     353,     0,    76,     0,    71,    69,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   222,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   508,     0,     0,     0,     0,   367,
     364,   301,   390,     0,     0,   356,    56,   299,     0,   318,
      94,    95,    96,   425,   426,     0,   443,   316,   442,     0,
     508,     0,     0,     0,   456,   496,   495,   325,   109,     0,
     508,   289,   335,   334,     0,   333,     0,     0,   508,     0,
       0,     0,     0,     0,     0,     0,     0,   507,     0,     0,
     289,     0,   508,     0,   313,   508,   266,     0,     0,   239,
     268,     0,   241,   295,     0,   262,   261,   260,   238,   499,
     508,   499,     0,    14,    16,    15,     0,   297,     0,     0,
       0,   408,   411,   409,   392,   407,     0,     0,     0,     0,
     287,    65,   499,   499,   227,   291,   503,   502,   228,   503,
     230,   503,   481,   292,   480,    84,     0,   508,     0,   422,
     391,   405,   393,   394,   395,     0,   398,   400,     0,   401,
       0,     0,     7,    10,    18,    19,    20,    21,    22,    39,
      40,   508,     0,    27,    35,     0,    36,     0,    70,    77,
      26,   179,   269,    34,   196,   204,   209,   210,   211,   206,
     208,   218,   219,   212,   213,   189,   190,   216,   217,     0,
     205,   207,   201,   202,   203,   191,   192,   193,   194,   195,
     488,   493,   489,   494,   259,   359,     0,   488,   489,   508,
     360,   259,   259,   508,   508,    28,   181,    33,   188,    53,
      57,     0,   445,     0,     0,    94,    95,    98,     0,     0,
     508,     0,   499,   461,   459,   458,   457,   460,   468,   472,
     471,   467,   456,     0,     0,   462,   508,   465,   508,   470,
     508,     0,   455,     0,     0,   284,   508,   508,   377,   508,
     336,   179,   492,   288,     0,   488,   489,   508,     0,     0,
       0,   371,     0,   311,   339,   338,   305,   337,   308,   492,
     288,     0,   488,   489,     0,     0,   245,   484,     0,   270,
     267,   508,     0,     0,   483,   294,     0,    43,     0,   264,
       0,   258,   508,     0,     0,     0,     0,     0,   232,    13,
       0,   221,     0,    25,     0,     0,   187,    65,     0,   231,
       0,   270,   492,    83,     0,   488,   489,   397,   399,   403,
     197,   198,   348,     0,   351,   346,   273,     0,    75,     0,
       0,     0,     0,   358,    60,   290,     0,     0,   237,   357,
      58,   236,   355,    52,   354,    51,   368,   365,   508,   319,
       0,     0,   290,   322,     0,     0,   499,     0,   447,     0,
     451,   475,     0,   453,   469,     0,   454,   473,   326,   110,
     378,   379,   508,   380,     0,   508,   342,     0,     0,   340,
       0,   290,     0,     0,     0,   310,   312,   373,     0,     0,
       0,     0,   290,     0,   508,     0,   243,   508,   508,     0,
       0,   263,     0,   251,   233,     0,   499,   508,   508,   234,
       0,    24,     0,   413,   414,   415,   410,   416,   286,   499,
      79,   347,     0,     0,     0,   271,   180,     0,    32,   185,
     186,    61,     0,    30,   183,    31,   184,    59,     0,     0,
      54,     0,   444,   317,   478,   464,     0,   324,   463,   508,
     508,   474,     0,   466,   508,   456,     0,     0,   382,   343,
       0,     4,   384,     0,   302,     0,   303,     0,   508,     0,
       0,   314,   240,     0,   242,   257,     0,   248,   508,   508,
     265,     0,     0,   298,   412,   229,   349,     0,   272,   223,
      29,   182,     0,     0,     0,     0,   446,     0,   449,   450,
     452,     0,     0,   381,     0,    86,    93,     0,   383,     0,
     372,   374,   375,   370,   306,   309,     0,   508,   508,     0,
     247,     0,   253,   508,   235,   352,   369,   366,     0,   320,
     508,     0,    92,     0,   508,     0,   508,   508,     0,   244,
     249,     0,   508,     0,   252,    55,   448,   327,   492,    91,
       0,   488,   489,   376,   341,   315,   508,     0,   254,   508,
      87,   250,     0,   255,   508,   256
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,   389,   390,   308,    63,    64,   436,   258,
      65,   212,    66,    67,   550,   688,   834,    68,    69,   272,
      70,    71,    72,    73,   213,   110,   111,   203,   204,   205,
     206,   584,   267,   192,    75,   438,   519,   239,   277,   678,
     679,   240,   629,   248,   249,   427,   630,   740,   620,   419,
     278,   496,    76,   209,   452,   640,   225,   730,   226,   731,
     614,   856,   554,   551,   781,   382,   384,   583,   795,   262,
     394,   606,   718,   719,   231,   664,   320,   491,   763,    78,
      79,   367,   544,   779,   543,   778,   406,   602,   853,   587,
     712,   797,   801,    80,    81,    82,    83,    84,    85,    86,
     299,   475,    87,   301,   296,   260,   445,   645,   644,   756,
      88,   295,    89,    90,   215,    92,   216,   217,   377,   553,
     573,   574,   575,   576,   577,   578,   579,   580,   581,   791,
     700,   194,   383,   282,   279,   244,   116,   558,   535,   387,
     222,   257,   458,   395,   224,    96
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -663
static const yytype_int16 yypact[] =
{
    -663,   107,  2346,  -663,  5754,  8294,  8600,  4078,  5404,  -663,
    7132,  7132,  3955,  -663,  -663,  8396,  5966,  5966,  -663,  -663,
    5966,  4729,  4835,  -663,  -663,  -663,  -663,  7132,  5285,     2,
    -663,   -11,  -663,  -663,  4199,  4305,  -663,  -663,  4411,  -663,
    -663,  -663,  -663,  -663,  -663,  7980,  7980,  8856,    97,  3337,
    7132,  7238,  7980,  8702,  5166,  -663,  -663,    49,    99,   355,
    8086,  7980,   112,   379,   799,   502,  -663,  -663,   227,   177,
    -663,   175,  8498,  -663,   260,  9802,   335,   364,   101,    76,
    -663,  -663,   285,  -663,  -663,  -663,  -663,  -663,  -663,  -663,
    -663,    11,  -663,   301,    16,    36,  -663,   799,  -663,  -663,
    -663,   267,   272,     2,   176,   367,  7132,   383,  3463,   492,
    -663,    66,  -663,   403,  -663,  -663,    36,  -663,  -663,  -663,
    -663,  -663,  -663,  -663,  -663,  -663,  -663,  -663,  -663,  -663,
    -663,  -663,  -663,  -663,  -663,  -663,  -663,  -663,  -663,  -663,
    -663,   173,   224,   254,   291,  -663,  -663,  -663,  -663,  -663,
    -663,  -663,  -663,  -663,  -663,  -663,  -663,   351,   352,  -663,
     354,  -663,   373,  -663,  -663,  -663,  -663,  -663,  -663,  -663,
    -663,  -663,  -663,  -663,  -663,  -663,  -663,  -663,  -663,  -663,
    -663,  -663,  -663,  -663,  -663,  -663,  -663,  -663,  -663,  -663,
    -663,  -663,  -663,  -663,   378,  -663,  -663,  -663,  -663,  -663,
    -663,  -663,  -663,  -663,  -663,  -663,   277,  -663,  -663,  2727,
     357,   502,    69,   321,   446,     9,   370,    29,    69,  -663,
    -663,   379,    91,  -663,   294,  7132,  7132,   417,  -663,  -663,
     487,   449,   100,   106,  7980,  7980,  7980,  -663,  9802,   389,
    -663,  -663,   358,   375,  -663,  -663,  -663,  5643,  -663,  6072,
    5966,  -663,  -663,  -663,    98,  -663,  -663,   362,   372,  3589,
     458,  -663,   490,   434,  -663,  -663,  -663,  -663,  -663,   140,
    3337,   384,   395,   399,   502,  7980,     2,   416,    55,   228,
    -663,   240,   440,   228,  -663,   521,  -663,   545,   583,   674,
    -663,  -663,  -663,  -663,   497,  -663,   525,   366,   479,   586,
     482,    37,   519,   520,  -663,  -663,  -663,   530,  -663,  -663,
    -663,  3835,  7132,  7132,  7132,  7132,  5754,  7132,  7132,  -663,
    -663,  7344,  -663,  3337,  8702,   465,  7344,  7980,  7980,  7980,
    7980,  7980,  7980,  7980,  7980,  7980,  7980,  7980,  7980,  7980,
    7980,  7980,  -663,  7980,  7980,  7980,  7980,  7980,  7980,  7980,
    7980,  7980,  7980,  8924,  7238,  8992,  8856,  8856,  8856,  -663,
    -663,  -663,  -663,  8086,  8086,  -663,   504,  -663,   362,   502,
    -663,   723,  -663,  -663,  -663,   379,  -663,  -663,  -663,  9060,
    7238,  8856,  2727,  7132,   779,  -663,  -663,  -663,  -663,   595,
     601,    86,  -663,  -663,  2853,   608,  7980,  9128,  7238,  9196,
    7980,  7980,  3095,   606,  3715,  7450,   614,  -663,    19,    19,
     125,  9264,  7238,  9332,  -663,   507,  -663,  7980,  6178,  -663,
    -663,  6284,  -663,  -663,   503,  5860,  -663,  -663,   227,     2,
     514,    -4,   516,  -663,  -663,  -663,  5404,  -663,  7980,  3589,
     509,  -663,  -663,  -663,  -663,  -663,  9128,  9196,  7980,   517,
    -663,   523,     2,  9667,  -663,  -663,  7556,  -663,  -663,  7980,
    -663,  7980,  -663,  -663,  -663,   723,  9400,  7238,  9468,  -663,
    -663,  -663,  -663,  -663,  -663,   148,  -663,  -663,   526,  -663,
    7980,  7980,  -663,   799,  -663,  -663,  -663,  -663,  -663,  -663,
    -663,   299,  7980,  -663,   528,   529,  -663,   539,  -663,  -663,
    -663,  2206,  -663,  -663,   434,  9887,  9887,  9887,  9887,   656,
     656,  9938,  9870,  9887,  9887,  9819,  9819,   548,   548,  7980,
     656,   656,   524,   524,   666,    31,    31,   434,   434,   434,
    2462,  4941,  2592,  5047,   272,  -663,   543,   143,   156,  4835,
    -663,   129,   129,   299,   299,  -663,  9802,  -663,  9802,  -663,
    -663,   379,  -663,  7132,  2727,   421,     1,  -663,   272,   552,
     272,   639,    82,   575,  -663,  -663,  -663,  -663,  -663,  -663,
    -663,  -663,   691,  2727,   379,  -663,   553,  -663,   562,   632,
     564,   644,  -663,  5523,  5404,  -663,  7662,   682,  -663,   554,
    -663,  2575,  4517,  4623,   576,   189,   200,   682,   693,   695,
    7980,   572,    69,  -663,  -663,  -663,  -663,  -663,  -663,    67,
      95,   587,   169,   248,  7132,   613,  -663,  -663,  7980,   389,
    -663,   588,  7980,   389,  -663,  -663,  7980,  9719,    26,  -663,
     592,  -663,   598,   593,  6390,  8856,  8856,   604,  -663,  -663,
    7132,  9802,   605,  -663,  3589,   808,  9802,   451,   607,  -663,
    7980,  -663,   342,     1,   590,    52,   168,  -663,  -663,  -663,
     434,   434,  -663,  8192,  -663,  -663,  -663,  7768,  -663,  7980,
    2445,  8086,  7980,  -663,   504,   313,  8086,  8086,  -663,  -663,
     504,  -663,  -663,  -663,  -663,  -663,  -663,  -663,   299,  -663,
     379,   728,  -663,  -663,   625,  7980,     2,   748,  -663,   691,
    -663,  -663,   312,  -663,  -663,     8,  -663,  -663,  -663,  -663,
     528,  -663,   673,  -663,  3216,   756,  -663,  7132,   766,  -663,
    7980,   239,  7980,  7980,   775,  -663,  -663,  -663,  7874,  2974,
    3715,  3715,   268,    19,   507,  6496,  -663,   507,   507,  6602,
     638,  -663,  6708,  -663,  -663,   227,    -4,   272,   272,  -663,
      57,  -663,   664,  -663,  -663,  -663,  -663,  -663,  -663,  9667,
     724,  -663,   679,  3715,  7980,   663,  9802,  7980,  -663,  9802,
    9802,  -663,  8086,  -663,  9802,  -663,  9802,  -663,  3715,  3589,
    -663,  2727,  -663,  -663,  -663,  -663,   665,  -663,  -663,   667,
     564,  -663,   575,  -663,   564,   779,  8804,    69,  -663,  -663,
    3715,  -663,  -663,    69,  -663,  7980,  -663,  7980,   208,   784,
     788,  -663,  -663,  7980,  -663,  -663,  7980,  -663,   681,   683,
    -663,  7980,   675,  -663,  -663,  -663,  -663,   801,  -663,  9802,
    -663,  9802,   804,   700,  3589,   805,  -663,   312,  -663,  -663,
    -663,  2727,   770,  -663,   752,   583,   674,  2727,  -663,  2853,
    -663,  -663,  -663,  -663,  -663,  -663,  3715,  9740,   507,  6814,
    -663,  6920,  -663,   507,  -663,  -663,  -663,  -663,   703,  -663,
     564,   826,   723,  9536,  7238,  9604,   601,   554,   834,  -663,
    -663,  7980,   717,  7980,  -663,  -663,  -663,  -663,    56,     1,
     721,    75,    87,  -663,  -663,  -663,   507,  7026,  -663,   507,
     724,  -663,  7980,  -663,   507,  -663
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -663,  -663,  -663,  -366,   694,  -663,  -663,    45,  -663,  -663,
    1479,    40,    23,  -663,  -569,  -663,  -663,    -8,   -13,  -199,
      14,   780,  -663,   -22,  1103,   -87,   847,    -1,  -663,   -25,
    -663,  -663,    20,  -663,   -16,  -663,  -663,  1567,  -333,   -15,
    -457,    93,  -663,    -5,  -663,  -663,  -663,  -663,    21,  -171,
      39,  -292,    17,  -663,  -663,  -663,  -663,  -663,  -663,  -663,
    -663,  -663,  -663,  -663,  -663,  -663,  -663,  -663,  -663,   505,
    -207,  -383,   -23,  -533,   197,  -483,  -663,  -663,  -663,  -240,
    -663,   793,  -663,  -663,  -663,  -663,  -384,  -663,  -663,     0,
    -663,  -663,  -663,  -663,  -663,  -663,   781,  -663,  -663,  -663,
    -663,  -663,  -663,  -663,  -663,   184,  -223,  -663,  -663,  -663,
      -3,  -663,     5,  -663,   669,   866,  1277,   914,  -663,  -663,
      85,   310,   190,  -663,  -662,   191,  -663,  -624,  -663,  -360,
    -512,  -663,  -663,  -663,    -2,  -364,   939,   127,  -663,  -663,
     -24,   -61,   266,    46,   820,  1369
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -509
static const yytype_int16 yytable[] =
{
     238,   238,   228,   255,   238,   207,   190,   251,   237,   237,
     431,   402,   237,   208,   243,   243,   561,   252,   243,   598,
     372,   536,   112,   112,   582,   207,   608,   191,   191,   261,
     263,   286,   112,   208,   503,   238,   238,   241,   241,   310,
     793,   241,   368,   276,   305,   306,   604,   559,   191,    97,
     283,   218,   221,   293,   715,   242,   242,   624,   223,   242,
     686,   687,   359,   273,   724,   594,   703,   375,   706,   -89,
     112,   449,   422,   471,   191,   790,   476,   -80,   794,   611,
     365,   635,   392,   304,   682,   684,  -288,   251,   -88,   112,
     366,   281,   -89,   317,   318,   624,  -439,   478,  -439,   327,
     -90,   365,   359,   364,   569,   771,   404,     3,   405,   223,
     245,   777,   259,   246,   417,   -86,   401,   -86,   317,   318,
     -91,   570,   -93,   636,   497,   -86,  -288,   605,  -288,  -508,
     256,   -93,  -489,  -288,   654,   -93,   256,  -508,  -508,  -508,
     -78,   479,  -508,  -508,  -508,   -85,  -508,   350,   351,   352,
     -92,   433,   269,   219,   220,   739,  -508,   378,   -80,   360,
     434,   435,   307,   -80,   -89,  -508,  -508,   250,  -508,  -508,
    -508,  -508,  -508,   448,   268,   793,   376,   393,   -89,  -508,
     -80,   -80,   298,   -88,   456,   823,   357,  -488,   691,   457,
     -89,   -89,   -92,   -82,   -88,   -90,   -81,   403,  -488,   360,
     219,   220,  -485,   219,   220,   780,   -91,   697,   441,   -88,
     -88,   -86,   582,   870,  -508,   454,   256,   -93,   238,   238,
     305,   -90,   -90,   714,   -83,   405,  -489,   448,   358,   -78,
     676,   238,   300,   238,   238,   -85,   -92,  -508,   294,   237,
     297,   237,   428,   677,   616,   243,   -92,   243,   243,   -88,
     442,   443,   658,  -508,   -84,   319,  -508,  -508,  -433,   453,
     250,  -427,   -90,  -508,  -508,   408,   409,   223,   241,   -84,
     241,   241,   -80,   -90,   -82,   851,   722,   838,   839,   -82,
     -88,   657,   840,   321,   273,   -82,   242,   723,   242,   430,
     682,   684,   557,   -87,   711,   -88,   -82,   -82,   -80,  -485,
    -433,  -427,   498,  -427,   322,   238,   -90,  -485,  -427,  -432,
     501,   504,   505,   506,   507,   508,   509,   510,   511,   512,
     513,   514,   515,   516,   517,   518,   805,   520,   521,   522,
     523,   524,   525,   526,   527,   528,   529,   273,   238,  -434,
     424,   112,   426,   429,   493,   -87,   276,   546,   548,   500,
     811,  -432,   484,   485,   486,   487,   483,   459,   886,   -90,
     495,   488,   457,   792,   238,   495,   326,   -81,   633,   461,
     637,   624,   276,   533,   457,   662,  -435,   -82,   624,   -87,
     591,  -434,   238,    55,   546,   548,   545,   547,   363,   238,
     276,   648,   649,  -486,   746,   729,   238,   -79,   247,   533,
     772,   238,   238,   250,   276,   238,   388,   568,   569,   627,
     663,   639,   391,   302,   303,   835,   621,   533,  -435,   -87,
    -332,   552,   641,   632,   852,   570,   441,   396,   571,   407,
     472,   533,   646,   207,   370,   582,  -437,  -436,   371,  -427,
     238,   208,   -79,   238,   601,   238,   631,  -492,   -81,   353,
     736,   238,  -430,   -81,   607,   607,   191,   400,  -430,   276,
    -332,   743,  -332,   385,   660,   661,   533,  -332,   442,   443,
     -81,   -81,   410,  -488,   414,   871,   238,   417,  -437,  -436,
     534,  -427,   539,   540,   541,   542,   533,   418,   379,   354,
    -486,   355,  -430,   437,  -430,   439,   356,   624,  -486,  -430,
    -430,   694,   327,   670,   421,   386,  -492,    77,   560,    77,
     113,   113,   450,   219,   220,   214,   214,   214,   441,   673,
     230,   214,   214,   451,   534,   214,   539,   -66,   380,   674,
     381,   397,   214,   624,   680,   356,   683,   685,   317,   318,
     560,   890,   455,   673,  -492,   460,  -492,   462,  -492,   464,
     373,   374,  -488,  -492,    77,   214,   214,   441,   287,   709,
     442,   443,   444,   812,   463,   214,   814,   815,   717,   714,
     238,   398,   411,   399,   560,   446,   465,   287,   356,   -67,
     -75,   207,   708,   473,   238,   441,   477,   480,   481,   208,
     847,   482,   327,   690,   499,   560,   849,   689,   549,   442,
     443,   469,   238,   191,   191,   585,   238,   340,   341,   586,
     238,   214,   412,    77,   413,   398,   327,   447,   238,   356,
     698,   590,   356,   405,   603,   495,   745,   442,   443,   470,
     466,   625,   621,   643,   759,   786,   615,   347,   348,   349,
     350,   351,   352,   634,   638,   647,   441,   860,   862,   693,
     228,   238,   -75,   766,   733,   769,   770,  -269,   667,   659,
     774,   776,   348,   349,   350,   351,   352,   668,  -438,   675,
     467,    91,   468,    91,   114,   114,   114,   356,   692,   238,
     112,   695,   699,   704,   232,   822,   879,   880,   442,   443,
     474,   702,   884,   705,   768,   707,    62,   714,   825,   773,
     775,   728,   721,   725,   769,   726,   774,   776,  -438,   235,
    -438,   898,   238,   732,    77,  -438,   760,   735,    91,   238,
     741,   744,   288,   238,   327,   901,   238,   742,   903,   751,
     214,   214,   749,   905,   327,   758,   782,   819,   783,   340,
     341,   288,   563,   271,   564,   565,   566,   567,   238,   340,
     341,   829,   214,   784,   214,   214,   831,   803,   787,  -281,
     817,   796,   747,   748,    77,   800,   820,   345,   346,   347,
     348,   349,   350,   351,   352,    77,   804,    91,   818,   607,
     348,   349,   350,   351,   352,   806,   568,   569,   824,   831,
     826,   238,  -270,   836,   854,   830,   837,   857,   855,  -281,
     238,  -281,   271,   864,   570,   238,  -281,   571,  -289,  -290,
     859,   865,   861,   112,   866,   869,    77,   214,   214,   214,
     214,    77,   214,   214,   867,   872,   214,   885,    77,   287,
     563,   214,   564,   565,   566,   567,   887,   873,   312,   313,
     314,   315,   316,   238,   895,   238,   897,   900,  -289,  -290,
    -289,  -290,   325,   117,   894,  -289,  -290,   882,   238,   214,
     762,   753,   754,   362,   755,   238,   276,   238,   214,   214,
      43,    44,   361,   193,   568,   569,   893,   874,    91,   875,
     841,   238,   696,   311,   356,   214,   238,    77,   214,   788,
     789,     0,   570,   533,     0,   571,     0,     0,     0,    77,
       0,     0,     0,   214,     0,     0,     0,    77,     0,    77,
     572,     0,     0,     0,     0,     0,    94,   214,    94,   115,
     115,     0,     0,     0,     0,     0,     0,     0,    91,   233,
     214,     0,     0,     0,     0,     0,     0,     0,     0,    91,
       0,    95,     0,    95,    77,     0,     0,     0,     0,    95,
      95,    95,     0,   440,     0,    95,    95,     0,     0,    95,
       0,     0,     0,    94,   271,     0,    95,   289,     0,     0,
       0,     0,   214,     0,     0,     0,     0,     0,     0,     0,
      91,     0,     0,     0,     0,    91,   289,     0,    95,    95,
      95,     0,    91,   288,     0,     0,     0,     0,     0,    95,
       0,     0,   560,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   271,     0,     0,
       0,     0,    94,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    95,     0,    95,     0,     0,
       0,    91,     0,     0,     0,     0,     0,     0,   214,    77,
       0,     0,     0,    91,     0,     0,     0,     0,     0,     0,
       0,    91,     0,    91,     0,     0,     0,     0,    77,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   589,     0,
       0,     0,     0,     0,     0,     0,   597,     0,   599,     0,
       0,     0,     0,     0,     0,    74,     0,    74,    91,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   229,   214,
       0,     0,     0,    94,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   642,     0,     0,     0,     0,     0,   214,
       0,     0,     0,     0,     0,   214,     0,     0,    95,    77,
       0,     0,    74,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    95,    95,     0,     0,   230,     0,
       0,     0,     0,    94,     0,     0,   214,     0,     0,     0,
       0,   214,   214,     0,    94,     0,    95,     0,    95,    95,
       0,     0,     0,     0,     0,     0,     0,     0,    95,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
       0,    74,     0,     0,     0,     0,     0,     0,     0,    77,
       0,     0,   214,    91,     0,    94,     0,     0,     0,     0,
      94,     0,     0,     0,    77,    77,    77,    94,   289,     0,
       0,     0,    91,     0,     0,     0,     0,     0,     0,     0,
      95,    95,    95,    95,    95,    95,    95,    95,     0,     0,
      95,     0,    95,     0,     0,    95,     0,     0,    77,     0,
       0,     0,     0,     0,     0,     0,     0,   214,     0,    93,
       0,    93,     0,    77,    77,     0,    77,     0,     0,     0,
       0,     0,     0,    95,     0,     0,    94,     0,     0,     0,
       0,   844,    95,    95,     0,    77,     0,     0,    94,     0,
       0,     0,    74,    91,     0,     0,    94,     0,    94,    95,
       0,    95,    95,     0,     0,     0,    93,     0,     0,     0,
       0,     0,   232,    95,     0,     0,     0,    95,   752,    77,
       0,    95,     0,    95,     0,     0,    77,     0,     0,     0,
       0,    95,    77,    94,    77,     0,     0,     0,     0,     0,
       0,    77,    74,     0,    95,     0,     0,     0,     0,     0,
       0,     0,     0,    74,     0,     0,     0,     0,    95,   214,
       0,     0,     0,    91,     0,    93,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    91,    91,
      91,     0,     0,     0,     0,     0,    95,     0,   799,     0,
       0,     0,     0,     0,    74,     0,     0,     0,     0,    74,
     280,   284,     0,   808,   809,   810,    74,     0,     0,     0,
       0,   309,    91,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    91,    91,     0,
      91,     0,     0,     0,     0,     0,     0,   827,     0,     0,
       0,     0,     0,     0,     0,   845,     0,     0,    94,    91,
       0,     0,   832,   833,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    74,    93,    94,     0,   211,
     211,   211,    95,    95,   848,     0,     0,    74,     0,     0,
       0,     0,     0,    91,     0,    74,   253,    74,     0,     0,
      91,     0,    95,     0,     0,     0,    91,     0,    91,     0,
       0,     0,     0,     0,     0,    91,     0,     0,   868,   274,
       0,     0,     0,     0,     0,     0,    93,     0,     0,     0,
       0,   876,    74,   877,     0,     0,     0,    93,     0,     0,
     878,     0,     0,    95,     0,     0,     0,     0,    94,   757,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    95,     0,     0,     0,   233,     0,    95,
       0,     0,     0,    95,     0,   369,     0,     0,    93,     0,
       0,     0,     0,    93,     0,     0,     0,     0,     0,     0,
      93,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      95,   420,   420,     0,     0,    95,    95,     0,     0,   432,
       0,     0,     0,     0,     0,     0,     0,     0,    94,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    94,    94,    94,     0,     0,     0,     0,
       0,     0,     0,    95,     0,     0,    95,    74,     0,    93,
       0,     0,     0,     0,     0,     0,     0,     0,    95,    95,
      95,    93,     0,     0,     0,     0,    74,    94,     0,    93,
       0,    93,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    94,    94,     0,    94,     0,     0,     0,     0,
       0,     0,    95,     0,   211,   211,     0,     0,     0,     0,
     846,    95,     0,     0,    94,     0,    93,    95,    95,     0,
      95,     0,     0,   280,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
       0,     0,     0,     0,     0,     0,     0,    74,    94,   280,
       0,     0,     0,     0,     0,    94,     0,     0,     0,   588,
       0,    94,     0,    94,     0,     0,   229,   280,     0,     0,
      94,     0,     0,    95,     0,     0,     0,     0,     0,     0,
      95,   280,     0,     0,   420,     0,    95,     0,    95,     0,
       0,   211,   211,   211,   211,    95,   489,   490,     0,   420,
       0,   415,   416,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    95,     0,     0,     0,    74,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    93,    74,    74,    74,     0,   280,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      93,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     665,     0,   562,     0,     0,     0,    74,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    74,    74,     0,    74,     0,     0,     0,   494,     0,
       0,     0,     0,   502,     0,     0,     0,     0,     0,   843,
       0,     0,     0,    74,   274,     0,     0,     0,   681,     0,
     681,   681,   665,   665,     0,     0,     0,     0,     0,     0,
       0,    93,     0,     0,     0,     0,     0,     0,     0,   681,
       0,     0,     0,     0,     0,     0,     0,    74,     0,     0,
       0,     0,     0,     0,    74,   701,     0,   701,     0,   701,
      74,     0,    74,     0,     0,   713,   716,     0,   716,    74,
       0,     0,     0,     0,     0,     0,   716,     0,     0,     0,
       0,     0,   502,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   617,   619,     0,     0,   623,     0,
     420,    93,   628,     0,     0,     0,     0,     0,     0,     0,
       0,   420,     0,     0,     0,     0,    93,    93,    93,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   651,     0,     0,   623,     0,   651,     0,
       0,     0,   211,     0,     0,     0,     0,     0,     0,     0,
      93,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    93,    93,   665,    93,   666,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    93,     0,     0,
       0,   798,     0,     0,   802,     0,     0,     0,     0,     0,
       0,     0,     0,   211,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   420,     0,     0,   420,   420,     0,     0,
       0,    93,     0,     0,     0,     0,   681,   681,    93,   750,
       0,     0,     0,     0,    93,     0,    93,     0,     0,     0,
       0,     0,     0,    93,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   710,     0,     0,     0,     0,   701,   701,
       0,     0,     0,   701,     0,     0,     0,   727,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   716,     0,     0,
       0,     0,     0,     0,     0,   734,     0,   420,   420,   737,
       0,     0,     0,   738,     0,     0,   211,     0,     0,     0,
       0,   619,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   420,   420,     0,     0,
       0,     0,   420,     0,   765,     0,     0,     0,     0,   701,
       0,     0,     0,   280,     0,   588,   716,     0,     0,   669,
       0,   420,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   785,     0,     0,   420,     0,     0,   420,     0,
       0,     0,     0,   420,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,   339,     0,     0,   340,
     341,     0,     0,     0,     0,   651,     0,     0,     0,     0,
       0,     0,   623,     0,     0,     0,     0,     0,     0,   623,
       0,     0,     0,   342,     0,   343,   344,   345,   346,   347,
     348,   349,   350,   351,   352,     0,     0,     0,     0,     0,
       0,   828,     0,     0,     0,  -225,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -508,     4,     0,     5,
       6,     7,     8,     9,     0,     0,     0,    10,    11,     0,
       0,     0,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,   850,    20,    21,    22,    23,    24,
      25,    26,     0,   858,    27,     0,     0,     0,   863,     0,
      28,    29,    30,    31,    32,    33,  -508,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   619,     0,   623,     0,
       0,     0,    48,     0,     0,    49,    50,     0,    51,    52,
       0,    53,     0,    54,    55,    56,    57,    58,   896,     0,
     899,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -492,     0,   623,    59,    60,    61,     0,   904,
    -492,  -492,  -492,     0,     0,     0,  -492,  -492,     0,  -492,
    -508,  -508,     0,     0,     0,     0,     0,     0,  -492,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -492,  -492,
       0,  -492,  -492,  -492,  -492,  -492,     0,     0,     0,     0,
       0,     0,  -492,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,   339,     0,     0,   340,   341,
    -492,  -492,  -492,  -492,  -492,  -492,  -492,  -492,  -492,  -492,
    -492,  -492,  -492,     0,     0,  -492,  -492,  -492,     0,   671,
       0,     0,   342,   767,   343,   344,   345,   346,   347,   348,
     349,   350,   351,   352,     0,     0,     0,     0,   -89,  -492,
       0,  -492,  -492,  -492,  -492,  -492,  -492,  -492,  -492,  -492,
    -492,     0,     0,     0,     0,  -492,  -492,  -492,     0,  -492,
    -492,   -81,  -288,     0,  -492,     0,  -492,  -492,     0,     0,
    -288,  -288,  -288,     0,     0,     0,  -288,  -288,     0,  -288,
       0,     0,     0,     0,     0,     0,     0,     0,   669,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -288,  -288,
       0,  -288,  -288,  -288,  -288,  -288,     0,     0,     0,     0,
       0,     0,  -288,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,   339,     0,     0,   340,   341,
    -288,  -288,  -288,  -288,  -288,  -288,  -288,  -288,  -288,  -288,
    -288,  -288,  -288,     0,     0,  -288,  -288,  -288,     0,   672,
       0,     0,   342,     0,   343,   344,   345,   346,   347,   348,
     349,   350,   351,   352,     0,     0,     0,     0,   -91,  -288,
       0,  -288,  -288,  -288,  -288,  -288,  -288,  -288,  -288,  -288,
    -288,     0,     0,     0,     0,     0,  -288,  -288,     0,  -288,
    -288,   -83,     0,     0,  -288,     0,  -288,  -288,     4,     0,
       5,     6,     7,     8,     9,  -508,  -508,  -508,    10,    11,
       0,     0,  -508,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,    33,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,     0,     0,    49,    50,     0,    51,
      52,     0,    53,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    59,    60,    61,     0,
       0,     0,     0,     0,     4,     0,     5,     6,     7,     8,
       9,  -508,  -508,  -508,    10,    11,     0,  -508,  -508,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,     0,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,     0,    43,    44,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,    49,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    59,    60,    61,     4,     0,     5,     6,     7,
       8,     9,     0,     0,  -508,    10,    11,  -508,  -508,  -508,
      12,  -508,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,     0,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,     0,    43,    44,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    59,    60,    61,     4,     0,     5,     6,
       7,     8,     9,     0,     0,  -508,    10,    11,  -508,  -508,
    -508,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,    28,
      29,    30,    31,    32,    33,     0,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,     0,    43,    44,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    48,     0,     0,    49,    50,     0,    51,    52,     0,
      53,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    59,    60,    61,     4,     0,     5,
       6,     7,     8,     9,     0,  -508,  -508,    10,    11,  -508,
    -508,     0,    12,     0,    13,    14,    15,    16,    17,    18,
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
       5,     6,     7,     8,     9,     0,     0,     0,    10,    11,
    -508,  -508,     0,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,    33,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,     0,    43,
      44,     0,    45,    46,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,     0,     0,   270,    50,     0,    51,
      52,     0,    53,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    59,    60,    61,     0,
       0,     0,     0,     0,     4,  -508,     5,     6,     7,     8,
       9,  -508,  -508,     0,    10,    11,     0,     0,     0,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,     0,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,     0,    43,    44,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,    49,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    59,    60,    61,     0,     0,     0,     0,     0,
       4,  -508,     5,     6,     7,     8,     9,  -508,  -508,     0,
      10,    11,     0,     0,     0,    12,     0,    13,    14,    15,
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
      61,     0,     0,  -508,     0,     0,     4,     0,     5,     6,
       7,     8,     9,  -508,  -508,  -508,    10,    11,     0,     0,
       0,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,    28,
      29,    30,    31,    32,    33,     0,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,     0,    43,    44,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    48,     0,     0,    49,    50,     0,    51,    52,     0,
      53,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    59,    60,    61,     0,     5,     6,
       7,     8,     9,     0,     0,     0,    10,    11,     0,  -508,
    -508,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,    28,
      29,    30,    31,    32,    33,     0,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,     0,    43,    44,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    48,     0,     0,    49,    50,     0,    51,    52,     0,
      53,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    59,    60,    61,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,     0,     0,
     407,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,     0,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,     0,    43,    44,     0,
      45,    46,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   210,     0,     0,   108,    50,     0,    51,    52,     0,
       0,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    59,    60,    61,     0,     0,     0,
       0,   118,   119,   120,   121,   122,   123,   124,   125,   219,
     220,   126,   127,   128,   129,   130,     0,     0,   131,   132,
     133,   134,   135,   136,   137,     0,     0,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,    36,    37,   162,    39,     0,     0,     0,     0,     0,
       0,     0,     0,   163,   164,     0,   165,   166,   167,   168,
       0,   169,   170,     0,     0,   171,     0,     0,     0,   172,
     173,   174,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,     0,     0,   187,
       0,     0,  -485,  -485,  -485,     0,  -485,     0,   188,   189,
    -485,  -485,     0,     0,     0,  -485,     0,  -485,  -485,  -485,
    -485,  -485,  -485,  -485,     0,  -485,     0,     0,  -485,  -485,
    -485,  -485,  -485,  -485,  -485,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -485,     0,     0,  -485,  -485,     0,
    -485,  -485,  -485,  -485,  -485,  -485,  -485,  -485,  -485,     0,
       0,  -485,  -485,     0,  -485,  -485,  -485,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -485,     0,     0,  -485,  -485,
       0,  -485,  -485,     0,  -485,  -485,  -485,  -485,  -485,  -485,
    -485,     0,     0,     0,     0,     0,     0,     0,  -487,  -487,
    -487,     0,  -487,     0,     0,     0,  -487,  -487,  -485,  -485,
    -485,  -487,  -485,  -487,  -487,  -487,  -487,  -487,  -487,  -487,
    -485,  -487,     0,     0,  -487,  -487,  -487,  -487,  -487,  -487,
    -487,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -487,     0,     0,  -487,  -487,     0,  -487,  -487,  -487,  -487,
    -487,  -487,  -487,  -487,  -487,     0,     0,  -487,  -487,     0,
    -487,  -487,  -487,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -487,     0,     0,  -487,  -487,     0,  -487,  -487,     0,
    -487,  -487,  -487,  -487,  -487,  -487,  -487,     0,     0,     0,
       0,     0,     0,     0,  -486,  -486,  -486,     0,  -486,     0,
       0,     0,  -486,  -486,  -487,  -487,  -487,  -486,  -487,  -486,
    -486,  -486,  -486,  -486,  -486,  -486,  -487,  -486,     0,     0,
    -486,  -486,  -486,  -486,  -486,  -486,  -486,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -486,     0,     0,  -486,
    -486,     0,  -486,  -486,  -486,  -486,  -486,  -486,  -486,  -486,
    -486,     0,     0,  -486,  -486,     0,  -486,  -486,  -486,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -486,     0,     0,
    -486,  -486,     0,  -486,  -486,     0,  -486,  -486,  -486,  -486,
    -486,  -486,  -486,     0,     0,     0,     0,     0,     0,     0,
    -488,  -488,  -488,     0,  -488,     0,     0,     0,  -488,  -488,
    -486,  -486,  -486,  -488,  -486,  -488,  -488,  -488,  -488,  -488,
    -488,  -488,  -486,     0,     0,     0,  -488,  -488,  -488,  -488,
    -488,  -488,  -488,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -488,     0,     0,  -488,  -488,     0,  -488,  -488,
    -488,  -488,  -488,  -488,  -488,  -488,  -488,     0,     0,  -488,
    -488,     0,  -488,  -488,  -488,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -488,   720,     0,  -488,  -488,     0,  -488,
    -488,     0,  -488,  -488,  -488,  -488,  -488,  -488,  -488,     0,
       0,     0,     0,   -89,     0,     0,  -489,  -489,  -489,     0,
    -489,     0,     0,     0,  -489,  -489,  -488,  -488,  -488,  -489,
       0,  -489,  -489,  -489,  -489,  -489,  -489,  -489,  -488,     0,
       0,     0,  -489,  -489,  -489,  -489,  -489,  -489,  -489,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -489,     0,
       0,  -489,  -489,     0,  -489,  -489,  -489,  -489,  -489,  -489,
    -489,  -489,  -489,     0,     0,  -489,  -489,     0,  -489,  -489,
    -489,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -489,
     672,     0,  -489,  -489,     0,  -489,  -489,     0,  -489,  -489,
    -489,  -489,  -489,  -489,  -489,     0,     0,     0,     0,   -91,
       0,     0,  -259,  -259,  -259,     0,  -259,     0,     0,     0,
    -259,  -259,  -489,  -489,  -489,  -259,     0,  -259,  -259,  -259,
    -259,  -259,  -259,  -259,  -489,     0,     0,     0,  -259,  -259,
    -259,  -259,  -259,  -259,  -259,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -259,     0,     0,  -259,  -259,     0,
    -259,  -259,  -259,  -259,  -259,  -259,  -259,  -259,  -259,     0,
       0,  -259,  -259,     0,  -259,  -259,  -259,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -259,     0,     0,  -259,  -259,
       0,  -259,  -259,     0,  -259,  -259,  -259,  -259,  -259,  -259,
    -259,     0,     0,     0,     0,     0,     0,     0,  -259,  -259,
    -259,     0,  -259,     0,     0,     0,  -259,  -259,  -259,  -259,
    -259,  -259,     0,  -259,  -259,  -259,  -259,  -259,  -259,  -259,
     247,     0,     0,     0,  -259,  -259,  -259,  -259,  -259,  -259,
    -259,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -259,     0,     0,  -259,  -259,     0,  -259,  -259,  -259,  -259,
    -259,  -259,  -259,  -259,  -259,     0,     0,  -259,  -259,     0,
    -259,  -259,  -259,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -259,     0,     0,  -259,  -259,     0,  -259,  -259,     0,
    -259,  -259,  -259,  -259,  -259,  -259,  -259,     0,     0,     0,
       0,     0,     0,     0,  -490,  -490,  -490,     0,  -490,     0,
       0,     0,  -490,  -490,  -259,  -259,  -259,  -490,     0,  -490,
    -490,  -490,  -490,  -490,  -490,  -490,   250,     0,     0,     0,
    -490,  -490,  -490,  -490,  -490,  -490,  -490,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -490,     0,     0,  -490,
    -490,     0,  -490,  -490,  -490,  -490,  -490,  -490,  -490,  -490,
    -490,     0,     0,  -490,  -490,     0,  -490,  -490,  -490,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -490,     0,     0,
    -490,  -490,     0,  -490,  -490,     0,  -490,  -490,  -490,  -490,
    -490,  -490,  -490,     0,     0,     0,     0,     0,     0,     0,
    -491,  -491,  -491,     0,  -491,     0,     0,     0,  -491,  -491,
    -490,  -490,  -490,  -491,     0,  -491,  -491,  -491,  -491,  -491,
    -491,  -491,  -490,     0,     0,     0,  -491,  -491,  -491,  -491,
    -491,  -491,  -491,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -491,     0,     0,  -491,  -491,     0,  -491,  -491,
    -491,  -491,  -491,  -491,  -491,  -491,  -491,     0,     0,  -491,
    -491,     0,  -491,  -491,  -491,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -491,     0,     0,  -491,  -491,     0,  -491,
    -491,     0,  -491,  -491,  -491,  -491,  -491,  -491,  -491,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -491,  -491,  -491,   118,
     119,   120,   121,   122,   123,   124,   125,     0,  -491,   126,
     127,   128,   129,   130,     0,     0,   131,   132,   133,   134,
     135,   136,   137,     0,     0,   138,   139,   140,   195,   196,
     197,   198,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   199,   200,   159,   201,   161,   290,
     291,   202,   292,     0,     0,     0,     0,     0,     0,     0,
       0,   163,   164,     0,   165,   166,   167,   168,     0,   169,
     170,     0,     0,   171,     0,     0,     0,   172,   173,   174,
     175,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   176,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,     0,     0,   187,   118,   119,
     120,   121,   122,   123,   124,   125,   188,     0,   126,   127,
     128,   129,   130,     0,     0,   131,   132,   133,   134,   135,
     136,   137,     0,     0,   138,   139,   140,   195,   196,   197,
     198,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   199,   200,   159,   201,   161,   254,     0,
     202,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     163,   164,     0,   165,   166,   167,   168,     0,   169,   170,
       0,     0,   171,     0,     0,     0,   172,   173,   174,   175,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     176,     0,    54,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,     0,     0,   187,   118,   119,   120,
     121,   122,   123,   124,   125,   188,     0,   126,   127,   128,
     129,   130,     0,     0,   131,   132,   133,   134,   135,   136,
     137,     0,     0,   138,   139,   140,   195,   196,   197,   198,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   199,   200,   159,   201,   161,     0,     0,   202,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   163,
     164,     0,   165,   166,   167,   168,     0,   169,   170,     0,
       0,   171,     0,     0,     0,   172,   173,   174,   175,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   176,
       0,    54,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,     0,     0,   187,   118,   119,   120,   121,
     122,   123,   124,   125,   188,     0,   126,   127,   128,   129,
     130,     0,     0,   131,   132,   133,   134,   135,   136,   137,
       0,     0,   138,   139,   140,   195,   196,   197,   198,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   199,   200,   159,   201,   161,     0,     0,   202,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   163,   164,
       0,   165,   166,   167,   168,     0,   169,   170,     0,     0,
     171,     0,     0,     0,   172,   173,   174,   175,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   176,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,     0,     0,   187,     0,     5,     6,     7,     0,
       9,     0,     0,   188,    10,    11,     0,     0,     0,    12,
       0,    13,    14,    15,    98,    99,    18,    19,     0,     0,
       0,     0,   100,    21,    22,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    32,    33,     0,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,     0,    43,    44,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   210,
       0,     0,   108,    50,     0,    51,    52,     0,   234,   235,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       8,     9,    59,   236,    61,    10,    11,     0,     0,     0,
      12,   423,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,     0,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,     0,    43,    44,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,    60,    61,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
       0,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,     0,    43,    44,     0,    45,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   210,     0,     0,   108,
      50,     0,    51,    52,     0,   626,   235,    54,    55,    56,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    59,
      60,    61,    12,     0,    13,    14,    15,    98,    99,    18,
      19,     0,     0,     0,     0,   100,    21,    22,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,     0,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   210,     0,     0,   108,    50,     0,    51,    52,
       0,   234,   235,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    59,   236,    61,    12,     0,
      13,    14,    15,    98,    99,    18,    19,     0,     0,     0,
       0,   100,    21,    22,    23,    24,    25,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,     0,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,     0,    43,    44,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   210,     0,
       0,   108,   425,     0,    51,    52,     0,   234,   235,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    59,   236,    61,    12,     0,    13,    14,    15,    98,
      99,    18,    19,     0,     0,     0,     0,   100,   101,   102,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,     0,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,     0,
      43,    44,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   210,     0,     0,   108,    50,     0,
      51,    52,     0,   618,   235,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   236,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,     0,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,     0,    43,    44,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     210,     0,     0,   108,    50,     0,    51,    52,     0,   622,
     235,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   236,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
      21,    22,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
       0,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,     0,    43,    44,     0,    45,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   210,     0,     0,   108,
      50,     0,    51,    52,     0,   618,   235,    54,    55,    56,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    59,
     236,    61,    12,     0,    13,    14,    15,    98,    99,    18,
      19,     0,     0,     0,     0,   100,   101,   102,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,     0,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   210,     0,     0,   108,    50,     0,    51,    52,
       0,   813,   235,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    59,   236,    61,    12,     0,
      13,    14,    15,    98,    99,    18,    19,     0,     0,     0,
       0,   100,   101,   102,    23,    24,    25,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,     0,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,     0,    43,    44,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   210,     0,
       0,   108,    50,     0,    51,    52,     0,   816,   235,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    59,   236,    61,    12,     0,    13,    14,    15,    98,
      99,    18,    19,     0,     0,     0,     0,   100,   101,   102,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,     0,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,     0,
      43,    44,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   210,     0,     0,   108,    50,     0,
      51,    52,     0,   821,   235,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   236,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,     0,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,     0,    43,    44,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     210,     0,     0,   108,    50,     0,    51,    52,     0,   881,
     235,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   236,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
       0,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,     0,    43,    44,     0,    45,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   210,     0,     0,   108,
      50,     0,    51,    52,     0,   883,   235,    54,    55,    56,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    59,
     236,    61,    12,     0,    13,    14,    15,    98,    99,    18,
      19,     0,     0,     0,     0,   100,   101,   102,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,     0,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   210,     0,     0,   108,    50,     0,    51,    52,
       0,   902,   235,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    59,   236,    61,    12,     0,
      13,    14,    15,    16,    17,    18,    19,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,     0,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,     0,    43,    44,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   210,     0,
       0,   108,    50,     0,    51,    52,     0,     0,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    59,    60,    61,    12,     0,    13,    14,    15,    98,
      99,    18,    19,     0,     0,     0,     0,   100,    21,    22,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,     0,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,     0,
      43,    44,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   210,     0,     0,   108,    50,     0,
      51,    52,     0,   275,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   236,    61,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,     0,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,     0,    43,    44,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     210,     0,     0,   108,    50,     0,    51,    52,     0,   492,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   236,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
       0,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,     0,    43,    44,     0,    45,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   210,     0,     0,   108,
      50,     0,    51,    52,     0,   600,     0,    54,    55,    56,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    59,
     236,    61,    12,     0,    13,    14,    15,    98,    99,    18,
      19,     0,     0,     0,     0,   100,   101,   102,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,     0,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   210,     0,     0,   108,    50,     0,    51,    52,
       0,   650,     0,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    59,   236,    61,    12,     0,
      13,    14,    15,    98,    99,    18,    19,     0,     0,     0,
       0,   100,   101,   102,    23,    24,    25,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    29,     0,     0,
      32,    33,     0,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,     0,    43,    44,     0,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   210,     0,
       0,   108,    50,     0,    51,    52,     0,   492,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     0,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,    59,   236,    61,    12,     0,    13,    14,    15,    98,
      99,    18,    19,     0,     0,     0,     0,   100,   101,   102,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     0,    32,    33,     0,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,     0,
      43,    44,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   210,     0,     0,   108,    50,     0,
      51,    52,     0,   764,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,    59,   236,    61,
      12,     0,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    32,    33,     0,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,     0,    43,    44,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     210,     0,     0,   108,    50,     0,    51,    52,     0,   807,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   236,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
       0,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,     0,    43,    44,     0,    45,    46,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   210,     0,     0,   108,
      50,     0,    51,    52,     0,     0,     0,    54,    55,    56,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    59,
     236,    61,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    32,    33,     0,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,     0,    43,    44,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   210,     0,     0,   108,    50,     0,    51,    52,
       0,     0,     0,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,    59,   236,    61,    12,     0,
      13,    14,    15,    98,    99,    18,    19,     0,     0,     0,
       0,   100,   101,   102,    23,    24,    25,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   103,     0,     0,
      32,    33,     0,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,     0,    43,    44,     0,     0,     0,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   227,     0,
       0,    49,    50,     0,    51,    52,     0,    53,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     5,     6,     7,
       0,     9,     0,   761,     0,    10,    11,     0,     0,     0,
      12,   109,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   103,
       0,     0,    32,    33,     0,   104,    35,    36,    37,   105,
      39,    40,    41,    42,     0,     0,    43,    44,     0,     0,
       0,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   106,     0,     0,
     107,     0,     0,   108,    50,     0,    51,    52,     0,     0,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,     0,
       0,     0,    12,   109,    13,    14,    15,    98,    99,    18,
      19,     0,     0,     0,     0,   100,   101,   102,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   103,     0,     0,    32,    33,     0,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,     0,    43,    44,
       0,     0,     0,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   227,     0,     0,    49,    50,     0,    51,    52,
       0,    53,     0,    54,    55,    56,    57,    58,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,     0,     0,     0,    12,   109,    13,    14,    15,    98,
      99,    18,    19,     0,     0,     0,     0,   100,   101,   102,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   103,     0,     0,    32,    33,     0,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,     0,
      43,    44,     0,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   285,     0,     0,   323,    50,     0,
      51,    52,     0,   324,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,     0,     0,     0,    12,   109,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   103,     0,     0,    32,    33,
       0,   104,    35,    36,    37,   105,    39,    40,    41,    42,
       0,     0,    43,    44,     0,     0,     0,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   107,     0,     0,   108,
      50,     0,    51,    52,     0,     0,     0,    54,    55,    56,
      57,    58,     0,     0,     0,     5,     6,     7,     0,     9,
       0,     0,     0,    10,    11,     0,     0,     0,    12,   109,
      13,    14,    15,    98,    99,    18,    19,     0,     0,     0,
       0,   100,   101,   102,    23,    24,    25,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   103,     0,     0,
      32,    33,     0,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     0,     0,    43,    44,     0,     0,     0,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   285,     0,
       0,   108,    50,     0,    51,    52,     0,     0,     0,    54,
      55,    56,    57,    58,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,     0,     0,     0,
      12,   109,    13,    14,    15,    98,    99,    18,    19,     0,
       0,     0,     0,   100,   101,   102,    23,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   103,
       0,     0,    32,    33,     0,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,     0,    43,    44,     0,     0,
       0,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     842,     0,     0,   108,    50,     0,    51,    52,     0,     0,
       0,    54,    55,    56,    57,    58,     0,   264,   265,     0,
       0,   266,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   163,   164,   109,   165,   166,   167,   168,     0,   169,
     170,     0,     0,   171,     0,     0,     0,   172,   173,   174,
     175,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   176,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   530,   531,   187,     0,   532,
       0,     0,     0,     0,     0,     0,   188,     0,     0,   163,
     164,     0,   165,   166,   167,   168,     0,   169,   170,     0,
       0,   171,     0,     0,     0,   172,   173,   174,   175,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   176,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   537,   265,   187,     0,   538,     0,     0,
       0,     0,     0,     0,   188,     0,     0,   163,   164,     0,
     165,   166,   167,   168,     0,   169,   170,     0,     0,   171,
       0,     0,     0,   172,   173,   174,   175,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   176,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   555,   531,   187,     0,   556,     0,     0,     0,     0,
       0,     0,   188,     0,     0,   163,   164,     0,   165,   166,
     167,   168,     0,   169,   170,     0,     0,   171,     0,     0,
       0,   172,   173,   174,   175,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   176,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   592,
     531,   187,     0,   593,     0,     0,     0,     0,     0,     0,
     188,     0,     0,   163,   164,     0,   165,   166,   167,   168,
       0,   169,   170,     0,     0,   171,     0,     0,     0,   172,
     173,   174,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   595,   265,   187,
       0,   596,     0,     0,     0,     0,     0,     0,   188,     0,
       0,   163,   164,     0,   165,   166,   167,   168,     0,   169,
     170,     0,     0,   171,     0,     0,     0,   172,   173,   174,
     175,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   176,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   609,   531,   187,     0,   610,
       0,     0,     0,     0,     0,     0,   188,     0,     0,   163,
     164,     0,   165,   166,   167,   168,     0,   169,   170,     0,
       0,   171,     0,     0,     0,   172,   173,   174,   175,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   176,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   612,   265,   187,     0,   613,     0,     0,
       0,     0,     0,     0,   188,     0,     0,   163,   164,     0,
     165,   166,   167,   168,     0,   169,   170,     0,     0,   171,
       0,     0,     0,   172,   173,   174,   175,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   176,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   652,   531,   187,     0,   653,     0,     0,     0,     0,
       0,     0,   188,     0,     0,   163,   164,     0,   165,   166,
     167,   168,     0,   169,   170,     0,     0,   171,     0,     0,
       0,   172,   173,   174,   175,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   176,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   655,
     265,   187,     0,   656,     0,     0,     0,     0,     0,     0,
     188,     0,     0,   163,   164,     0,   165,   166,   167,   168,
       0,   169,   170,     0,     0,   171,     0,     0,     0,   172,
     173,   174,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   888,   531,   187,
       0,   889,     0,     0,     0,     0,     0,     0,   188,     0,
       0,   163,   164,     0,   165,   166,   167,   168,     0,   169,
     170,     0,     0,   171,     0,     0,     0,   172,   173,   174,
     175,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   176,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   891,   265,   187,     0,   892,
       0,     0,     0,     0,     0,     0,   188,     0,     0,   163,
     164,     0,   165,   166,   167,   168,     0,   169,   170,     0,
       0,   171,     0,     0,     0,   172,   173,   174,   175,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   176,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,     0,     0,   187,     0,     0,     0,     0,
       0,     0,     0,     0,   188,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,     0,     0,
     340,   341,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   342,     0,   343,   344,   345,   346,
     347,   348,   349,   350,   351,   352,     0,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
       0,   256,   340,   341,     0,     0,     0,  -225,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,     0,     0,   340,   341,     0,   342,     0,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   342,  -225,   343,
     344,   345,   346,   347,   348,   349,   350,   351,   352,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   615,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,   339,     0,     0,   340,   341,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,  -509,  -509,
       0,     0,   340,   341,     0,     0,     0,     0,     0,   342,
       0,   343,   344,   345,   346,   347,   348,   349,   350,   351,
     352,     0,     0,     0,     0,     0,     0,     0,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   327,   328,
     329,   330,   331,   332,   333,   334,     0,   336,   337,     0,
       0,     0,     0,   340,   341,   327,  -509,  -509,  -509,  -509,
     332,   333,     0,     0,  -509,  -509,     0,     0,     0,     0,
     340,   341,     0,     0,     0,     0,     0,     0,     0,   343,
     344,   345,   346,   347,   348,   349,   350,   351,   352,     0,
       0,     0,     0,     0,     0,     0,   343,   344,   345,   346,
     347,   348,   349,   350,   351,   352,   327,   328,   329,   330,
     331,   332,   333,     0,     0,   336,   337,     0,     0,     0,
       0,   340,   341,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   343,   344,   345,
     346,   347,   348,   349,   350,   351,   352
};

static const yytype_int16 yycheck[] =
{
      16,    17,    15,    28,    20,     8,     7,    22,    16,    17,
     250,   218,    20,     8,    16,    17,   382,    22,    20,   403,
     107,   354,     5,     6,   384,    28,   409,     7,     8,    45,
      46,    53,    15,    28,   326,    51,    52,    16,    17,    63,
     702,    20,   103,    51,    60,    61,    27,   380,    28,     4,
      52,    11,    12,    54,   587,    16,    17,   421,    12,    20,
     543,   544,    26,    49,   597,   398,   578,     1,   580,    13,
      53,   270,   243,   296,    54,   699,   299,    25,   702,   412,
      95,    85,    13,    60,   541,   542,    85,   102,    13,    72,
      95,    52,    25,    36,    37,   459,    87,    60,    87,    68,
      13,   116,    26,    87,    96,   674,    15,     0,    17,    63,
      17,   680,   123,    20,    88,   106,    87,   106,    36,    37,
      25,   113,   106,   127,   323,    25,   125,   108,   127,     0,
     134,    25,   131,   132,   467,   106,   134,     8,     9,    10,
     129,   104,    13,    14,    15,   129,    17,   116,   117,   118,
      25,    53,    55,   134,   135,   129,    27,   111,   106,   123,
      62,    63,    50,   111,   108,    36,    37,   131,    39,    40,
      41,    42,    43,    87,    47,   837,   110,   108,   111,    50,
     128,   129,   133,   108,   129,   128,    85,   131,   554,   134,
     134,   135,   106,    25,    25,   108,   129,   221,   131,   123,
     134,   135,    26,   134,   135,   688,   111,   573,    60,   134,
     135,   111,   572,   837,    85,   276,   134,   111,   234,   235,
     236,   134,   135,    15,   129,    17,   131,    87,   127,   129,
      87,   247,   133,   249,   250,   129,   111,   108,    54,   247,
      56,   249,   250,    87,   415,   247,   106,   249,   250,   106,
     102,   103,   475,   124,   129,    28,   127,   128,    85,   275,
     131,    85,   106,   134,   135,   225,   226,   221,   247,   129,
     249,   250,   129,    25,   106,   808,    87,   789,   790,   111,
     111,   133,   794,   106,   270,   129,   247,    87,   249,   250,
     747,   748,   379,    25,   586,   106,   128,   129,   129,   123,
     127,   125,   324,   127,   129,   321,   106,   131,   132,    85,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,    87,   343,   344,   345,
     346,   347,   348,   349,   350,   351,   352,   323,   354,    85,
     247,   324,   249,   250,   321,   106,   354,   363,   364,   326,
     733,   127,   312,   313,   314,   315,   311,   129,   870,   111,
     321,   316,   134,    51,   380,   326,   106,    25,   429,   129,
     431,   735,   380,   353,   134,    76,    85,   129,   742,   111,
     396,   127,   398,    98,   400,   401,   363,   364,    87,   405,
     398,   452,   453,    26,   634,   602,   412,   129,   131,   379,
      87,   417,   418,   131,   412,   421,   129,    95,    96,   425,
     111,   436,    55,    58,    59,   781,   418,   397,   127,   106,
      85,   375,   438,   425,   808,   113,    60,   106,   116,   135,
      64,   411,   448,   436,    51,   795,    85,    85,    55,    85,
     456,   436,   129,   459,   405,   461,   425,    26,   106,    85,
     621,   467,    85,   111,   408,   409,   436,    87,    85,   467,
     125,   632,   127,    85,   480,   481,   446,   132,   102,   103,
     128,   129,    55,   131,    25,   841,   492,    88,   127,   127,
     353,   127,   355,   356,   357,   358,   466,   129,    85,   125,
     123,   127,   125,   131,   127,   123,   132,   861,   131,   132,
     127,   562,    68,   519,   129,   127,    85,     2,   381,     4,
       5,     6,   128,   134,   135,    10,    11,    12,    60,   534,
      15,    16,    17,   128,   397,    20,   399,   128,   125,   534,
     127,    85,    27,   897,   539,   132,   541,   542,    36,    37,
     413,   874,   126,   558,   123,   279,   125,   281,   127,   283,
      58,    59,   131,   132,    49,    50,    51,    60,    53,   584,
     102,   103,   104,   734,   124,    60,   737,   738,    14,    15,
     586,   125,    85,   127,   447,    85,    55,    72,   132,   128,
     129,   584,   583,   104,   600,    60,   104,    68,    68,   584,
     797,    61,    68,   553,   129,   468,   803,   551,    94,   102,
     103,   104,   618,   583,   584,    10,   622,    83,    84,     8,
     626,   106,   125,   108,   127,   125,    68,   127,   634,   132,
     574,    13,   132,    17,    10,   586,   634,   102,   103,   104,
      85,   128,   634,   124,   650,   696,   129,   113,   114,   115,
     116,   117,   118,   129,   128,   128,    60,   818,   819,    10,
     663,   667,   129,   669,   614,   671,   672,   129,   129,   133,
     676,   677,   114,   115,   116,   117,   118,   128,    85,   126,
     125,     2,   127,     4,     5,     6,     7,   132,   126,   695,
     663,   106,   129,    51,    15,   746,   857,   858,   102,   103,
     104,   129,   863,   129,   671,    51,     2,    15,   759,   676,
     677,   129,   126,    10,   720,    10,   722,   723,   125,    96,
     127,   882,   728,   126,   209,   132,   126,   129,    49,   735,
     128,   128,    53,   739,    68,   896,   742,   129,   899,   124,
     225,   226,   128,   904,    68,   128,   690,   739,    10,    83,
      84,    72,    51,    49,    53,    54,    55,    56,   764,    83,
      84,   767,   247,   128,   249,   250,   772,   717,    10,    85,
     739,    88,   635,   636,   259,     9,   128,   111,   112,   113,
     114,   115,   116,   117,   118,   270,    10,   108,   739,   733,
     114,   115,   116,   117,   118,    10,    95,    96,   124,   805,
     111,   807,   129,   128,    10,   772,   129,   813,    10,   125,
     816,   127,   108,   128,   113,   821,   132,   116,    85,    85,
     129,    10,   129,   796,    10,    10,   311,   312,   313,   314,
     315,   316,   317,   318,   124,    55,   321,   124,   323,   324,
      51,   326,    53,    54,    55,    56,    10,    85,    39,    40,
      41,    42,    43,   859,    10,   861,   129,   126,   125,   125,
     127,   127,    72,     6,   877,   132,   132,   859,   874,   354,
     663,    53,    54,    82,    56,   881,   874,   883,   363,   364,
      62,    63,    79,     7,    95,    96,   876,   125,   209,   127,
     795,   897,   572,    63,   132,   380,   902,   382,   383,   699,
     699,    -1,   113,   873,    -1,   116,    -1,    -1,    -1,   394,
      -1,    -1,    -1,   398,    -1,    -1,    -1,   402,    -1,   404,
     131,    -1,    -1,    -1,    -1,    -1,     2,   412,     4,     5,
       6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,    15,
     425,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   270,
      -1,     2,    -1,     4,   439,    -1,    -1,    -1,    -1,    10,
      11,    12,    -1,   259,    -1,    16,    17,    -1,    -1,    20,
      -1,    -1,    -1,    49,   270,    -1,    27,    53,    -1,    -1,
      -1,    -1,   467,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     311,    -1,    -1,    -1,    -1,   316,    72,    -1,    49,    50,
      51,    -1,   323,   324,    -1,    -1,    -1,    -1,    -1,    60,
      -1,    -1,   875,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   323,    -1,    -1,
      -1,    -1,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   106,    -1,   108,    -1,    -1,
      -1,   382,    -1,    -1,    -1,    -1,    -1,    -1,   553,   554,
      -1,    -1,    -1,   394,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   402,    -1,   404,    -1,    -1,    -1,    -1,   573,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   394,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   402,    -1,   404,    -1,
      -1,    -1,    -1,    -1,    -1,     2,    -1,     4,   439,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,   614,
      -1,    -1,    -1,   209,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   439,    -1,    -1,    -1,    -1,    -1,   634,
      -1,    -1,    -1,    -1,    -1,   640,    -1,    -1,   209,   644,
      -1,    -1,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   225,   226,    -1,    -1,   663,    -1,
      -1,    -1,    -1,   259,    -1,    -1,   671,    -1,    -1,    -1,
      -1,   676,   677,    -1,   270,    -1,   247,    -1,   249,   250,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   270,
      -1,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   714,
      -1,    -1,   717,   554,    -1,   311,    -1,    -1,    -1,    -1,
     316,    -1,    -1,    -1,   729,   730,   731,   323,   324,    -1,
      -1,    -1,   573,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     311,   312,   313,   314,   315,   316,   317,   318,    -1,    -1,
     321,    -1,   323,    -1,    -1,   326,    -1,    -1,   763,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   772,    -1,     2,
      -1,     4,    -1,   778,   779,    -1,   781,    -1,    -1,    -1,
      -1,    -1,    -1,   354,    -1,    -1,   382,    -1,    -1,    -1,
      -1,   796,   363,   364,    -1,   800,    -1,    -1,   394,    -1,
      -1,    -1,   209,   644,    -1,    -1,   402,    -1,   404,   380,
      -1,   382,   383,    -1,    -1,    -1,    49,    -1,    -1,    -1,
      -1,    -1,   663,   394,    -1,    -1,    -1,   398,   644,   834,
      -1,   402,    -1,   404,    -1,    -1,   841,    -1,    -1,    -1,
      -1,   412,   847,   439,   849,    -1,    -1,    -1,    -1,    -1,
      -1,   856,   259,    -1,   425,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   270,    -1,    -1,    -1,    -1,   439,   874,
      -1,    -1,    -1,   714,    -1,   108,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   729,   730,
     731,    -1,    -1,    -1,    -1,    -1,   467,    -1,   714,    -1,
      -1,    -1,    -1,    -1,   311,    -1,    -1,    -1,    -1,   316,
      51,    52,    -1,   729,   730,   731,   323,    -1,    -1,    -1,
      -1,    62,   763,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   778,   779,    -1,
     781,    -1,    -1,    -1,    -1,    -1,    -1,   763,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   796,    -1,    -1,   554,   800,
      -1,    -1,   778,   779,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   382,   209,   573,    -1,    10,
      11,    12,   553,   554,   800,    -1,    -1,   394,    -1,    -1,
      -1,    -1,    -1,   834,    -1,   402,    27,   404,    -1,    -1,
     841,    -1,   573,    -1,    -1,    -1,   847,    -1,   849,    -1,
      -1,    -1,    -1,    -1,    -1,   856,    -1,    -1,   834,    50,
      -1,    -1,    -1,    -1,    -1,    -1,   259,    -1,    -1,    -1,
      -1,   847,   439,   849,    -1,    -1,    -1,   270,    -1,    -1,
     856,    -1,    -1,   614,    -1,    -1,    -1,    -1,   644,   645,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   634,    -1,    -1,    -1,   663,    -1,   640,
      -1,    -1,    -1,   644,    -1,   106,    -1,    -1,   311,    -1,
      -1,    -1,    -1,   316,    -1,    -1,    -1,    -1,    -1,    -1,
     323,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     671,   242,   243,    -1,    -1,   676,   677,    -1,    -1,   250,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   714,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   729,   730,   731,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   714,    -1,    -1,   717,   554,    -1,   382,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   729,   730,
     731,   394,    -1,    -1,    -1,    -1,   573,   763,    -1,   402,
      -1,   404,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   778,   779,    -1,   781,    -1,    -1,    -1,    -1,
      -1,    -1,   763,    -1,   225,   226,    -1,    -1,    -1,    -1,
     796,   772,    -1,    -1,   800,    -1,   439,   778,   779,    -1,
     781,    -1,    -1,   354,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   800,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   644,   834,   380,
      -1,    -1,    -1,    -1,    -1,   841,    -1,    -1,    -1,   390,
      -1,   847,    -1,   849,    -1,    -1,   663,   398,    -1,    -1,
     856,    -1,    -1,   834,    -1,    -1,    -1,    -1,    -1,    -1,
     841,   412,    -1,    -1,   415,    -1,   847,    -1,   849,    -1,
      -1,   312,   313,   314,   315,   856,   317,   318,    -1,   430,
      -1,   234,   235,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   874,    -1,    -1,    -1,   714,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   554,   729,   730,   731,    -1,   467,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     573,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     491,    -1,   383,    -1,    -1,    -1,   763,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   778,   779,    -1,   781,    -1,    -1,    -1,   321,    -1,
      -1,    -1,    -1,   326,    -1,    -1,    -1,    -1,    -1,   796,
      -1,    -1,    -1,   800,   425,    -1,    -1,    -1,   539,    -1,
     541,   542,   543,   544,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   644,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   560,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   834,    -1,    -1,
      -1,    -1,    -1,    -1,   841,   576,    -1,   578,    -1,   580,
     847,    -1,   849,    -1,    -1,   586,   587,    -1,   589,   856,
      -1,    -1,    -1,    -1,    -1,    -1,   597,    -1,    -1,    -1,
      -1,    -1,   405,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   417,   418,    -1,    -1,   421,    -1,
     621,   714,   425,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   632,    -1,    -1,    -1,    -1,   729,   730,   731,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   456,    -1,    -1,   459,    -1,   461,    -1,
      -1,    -1,   553,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     763,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   778,   779,   688,   781,   492,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   800,    -1,    -1,
      -1,   712,    -1,    -1,   715,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   614,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   734,    -1,    -1,   737,   738,    -1,    -1,
      -1,   834,    -1,    -1,    -1,    -1,   747,   748,   841,   640,
      -1,    -1,    -1,    -1,   847,    -1,   849,    -1,    -1,    -1,
      -1,    -1,    -1,   856,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   586,    -1,    -1,    -1,    -1,   789,   790,
      -1,    -1,    -1,   794,    -1,    -1,    -1,   600,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   808,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   618,    -1,   818,   819,   622,
      -1,    -1,    -1,   626,    -1,    -1,   717,    -1,    -1,    -1,
      -1,   634,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   857,   858,    -1,    -1,
      -1,    -1,   863,    -1,   667,    -1,    -1,    -1,    -1,   870,
      -1,    -1,    -1,   874,    -1,   876,   877,    -1,    -1,    43,
      -1,   882,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   695,    -1,    -1,   896,    -1,    -1,   899,    -1,
      -1,    -1,    -1,   904,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    -1,    -1,    83,
      84,    -1,    -1,    -1,    -1,   728,    -1,    -1,    -1,    -1,
      -1,    -1,   735,    -1,    -1,    -1,    -1,    -1,    -1,   742,
      -1,    -1,    -1,   107,    -1,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,    -1,    -1,    -1,    -1,    -1,
      -1,   764,    -1,    -1,    -1,   129,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     0,     1,    -1,     3,
       4,     5,     6,     7,    -1,    -1,    -1,    11,    12,    -1,
      -1,    -1,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,   807,    29,    30,    31,    32,    33,
      34,    35,    -1,   816,    38,    -1,    -1,    -1,   821,    -1,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    62,    63,
      -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   859,    -1,   861,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    -1,    97,    98,    99,   100,   101,   881,    -1,
     883,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     0,    -1,   897,   119,   120,   121,    -1,   902,
       8,     9,    10,    -1,    -1,    -1,    14,    15,    -1,    17,
     134,   135,    -1,    -1,    -1,    -1,    -1,    -1,    26,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,
      -1,    39,    40,    41,    42,    43,    -1,    -1,    -1,    -1,
      -1,    -1,    50,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    -1,    -1,    83,    84,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    85,    -1,    87,
      -1,    -1,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,    -1,    -1,    -1,    -1,   106,   107,
      -1,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,    -1,    -1,    -1,    -1,   123,   124,   125,    -1,   127,
     128,   129,     0,    -1,   132,    -1,   134,   135,    -1,    -1,
       8,     9,    10,    -1,    -1,    -1,    14,    15,    -1,    17,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,
      -1,    39,    40,    41,    42,    43,    -1,    -1,    -1,    -1,
      -1,    -1,    50,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    -1,    -1,    83,    84,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    85,    -1,    87,
      -1,    -1,   107,    -1,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,    -1,    -1,    -1,    -1,   106,   107,
      -1,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,    -1,    -1,    -1,    -1,    -1,   124,   125,    -1,   127,
     128,   129,    -1,    -1,   132,    -1,   134,   135,     1,    -1,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      -1,    -1,    15,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      -1,    44,    45,    46,    47,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    -1,    97,    98,    99,   100,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   119,   120,   121,    -1,
      -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,
       7,   134,   135,    10,    11,    12,    -1,    14,    15,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,    62,    63,    -1,    65,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,
      97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   119,   120,   121,     1,    -1,     3,     4,     5,
       6,     7,    -1,    -1,    10,    11,    12,   134,   135,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    -1,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,    63,    -1,    65,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      -1,    97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   119,   120,   121,     1,    -1,     3,     4,
       5,     6,     7,    -1,    -1,    10,    11,    12,   134,   135,
      15,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    -1,
      65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      95,    -1,    97,    98,    99,   100,   101,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   119,   120,   121,     1,    -1,     3,
       4,     5,     6,     7,    -1,     9,    10,    11,    12,   134,
     135,    -1,    16,    -1,    18,    19,    20,    21,    22,    23,
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
       3,     4,     5,     6,     7,    -1,    -1,    -1,    11,    12,
     134,   135,    -1,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      -1,    44,    45,    46,    47,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    -1,    97,    98,    99,   100,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   119,   120,   121,    -1,
      -1,    -1,    -1,    -1,     1,   128,     3,     4,     5,     6,
       7,   134,   135,    -1,    11,    12,    -1,    -1,    -1,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,    62,    63,    -1,    65,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,
      97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   119,   120,   121,    -1,    -1,    -1,    -1,    -1,
       1,   128,     3,     4,     5,     6,     7,   134,   135,    -1,
      11,    12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,
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
     121,    -1,    -1,   124,    -1,    -1,     1,    -1,     3,     4,
       5,     6,     7,   134,   135,    10,    11,    12,    -1,    -1,
      -1,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    -1,
      65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      95,    -1,    97,    98,    99,   100,   101,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   119,   120,   121,    -1,     3,     4,
       5,     6,     7,    -1,    -1,    -1,    11,    12,    -1,   134,
     135,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    -1,
      65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      95,    -1,    97,    98,    99,   100,   101,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   119,   120,   121,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,
     135,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    -1,
      65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      -1,    -1,    97,    98,    99,   100,   101,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   119,   120,   121,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,   134,
     135,    13,    14,    15,    16,    17,    -1,    -1,    20,    21,
      22,    23,    24,    25,    26,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    65,    66,    -1,    68,    69,    70,    71,
      -1,    73,    74,    -1,    -1,    77,    -1,    -1,    -1,    81,
      82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,    -1,    -1,   121,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,   130,   131,
      11,    12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    -1,
      -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      -1,    92,    93,    -1,    95,    96,    97,    98,    99,   100,
     101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   119,   120,
     121,    16,   123,    18,    19,    20,    21,    22,    23,    24,
     131,    26,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    -1,
      65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      95,    96,    97,    98,    99,   100,   101,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   119,   120,   121,    16,   123,    18,
      19,    20,    21,    22,    23,    24,   131,    26,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    -1,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    -1,    -1,    62,    63,    -1,    65,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    -1,    92,    93,    -1,    95,    96,    97,    98,
      99,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     119,   120,   121,    16,   123,    18,    19,    20,    21,    22,
      23,    24,   131,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    87,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    96,    97,    98,    99,   100,   101,    -1,
      -1,    -1,    -1,   106,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   119,   120,   121,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,   131,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,    62,    63,    -1,    65,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      87,    -1,    89,    90,    -1,    92,    93,    -1,    95,    96,
      97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,   106,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   119,   120,   121,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,   131,    -1,    -1,    -1,    29,    30,
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
     131,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    -1,
      65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,
      95,    96,    97,    98,    99,   100,   101,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   119,   120,   121,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,   131,    -1,    -1,    -1,
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
      23,    24,   131,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    96,    97,    98,    99,   100,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   119,   120,   121,     3,
       4,     5,     6,     7,     8,     9,    10,    -1,   131,    13,
      14,    15,    16,    17,    -1,    -1,    20,    21,    22,    23,
      24,    25,    26,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    65,    66,    -1,    68,    69,    70,    71,    -1,    73,
      74,    -1,    -1,    77,    -1,    -1,    -1,    81,    82,    83,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,    -1,    -1,   121,     3,     4,
       5,     6,     7,     8,     9,    10,   130,    -1,    13,    14,
      15,    16,    17,    -1,    -1,    20,    21,    22,    23,    24,
      25,    26,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    -1,
      55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    66,    -1,    68,    69,    70,    71,    -1,    73,    74,
      -1,    -1,    77,    -1,    -1,    -1,    81,    82,    83,    84,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      95,    -1,    97,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,    -1,    -1,   121,     3,     4,     5,
       6,     7,     8,     9,    10,   130,    -1,    13,    14,    15,
      16,    17,    -1,    -1,    20,    21,    22,    23,    24,    25,
      26,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    -1,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,
      66,    -1,    68,    69,    70,    71,    -1,    73,    74,    -1,
      -1,    77,    -1,    -1,    -1,    81,    82,    83,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      -1,    97,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,    -1,    -1,   121,     3,     4,     5,     6,
       7,     8,     9,    10,   130,    -1,    13,    14,    15,    16,
      17,    -1,    -1,    20,    21,    22,    23,    24,    25,    26,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    -1,    -1,    55,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,
      -1,    68,    69,    70,    71,    -1,    73,    74,    -1,    -1,
      77,    -1,    -1,    -1,    81,    82,    83,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,    -1,    -1,   121,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,   130,    11,    12,    -1,    -1,    -1,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,    62,    63,    -1,    65,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,    96,
      97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,   119,   120,   121,    11,    12,    -1,    -1,    -1,
      16,   128,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    -1,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,    63,    -1,    65,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      -1,    97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   119,   120,   121,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    96,    97,    98,    99,
     100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   119,
     120,   121,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    62,    63,
      -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    96,    97,    98,    99,   100,   101,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   119,   120,   121,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
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
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    96,    97,    98,    99,   100,   101,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   119,   120,   121,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
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
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    96,    97,    98,    99,
     100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   119,
     120,   121,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    62,    63,
      -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    96,    97,    98,    99,   100,   101,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   119,   120,   121,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
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
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    96,    97,    98,    99,   100,   101,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   119,   120,   121,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
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
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    96,    97,    98,    99,
     100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   119,
     120,   121,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    62,    63,
      -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    96,    97,    98,    99,   100,   101,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   119,   120,   121,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    -1,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    62,    63,    -1,    65,    66,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    -1,    92,    93,    -1,    -1,    -1,    97,
      98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   119,   120,   121,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    -1,    97,    98,    99,   100,   101,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   119,   120,   121,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    -1,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,    63,    -1,    65,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      -1,    97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   119,   120,   121,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    95,    -1,    97,    98,    99,
     100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   119,
     120,   121,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    62,    63,
      -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    -1,    97,    98,    99,   100,   101,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   119,   120,   121,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    -1,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    62,    63,    -1,    65,    66,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,    97,
      98,    99,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,   119,   120,   121,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    -1,    97,    98,    99,   100,   101,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   119,   120,   121,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    -1,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,    63,    -1,    65,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    95,
      -1,    97,    98,    99,   100,   101,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   119,   120,   121,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    -1,    62,    63,    -1,    65,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    -1,    -1,    97,    98,    99,
     100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   119,
     120,   121,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    62,    63,
      -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    -1,    -1,    97,    98,    99,   100,   101,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,   119,   120,   121,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    -1,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    62,    63,    -1,    -1,    -1,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    -1,    92,    93,    -1,    95,    -1,    97,
      98,    99,   100,   101,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,   111,    -1,    11,    12,    -1,    -1,    -1,
      16,   119,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    -1,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,    63,    -1,    -1,
      -1,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    -1,
      -1,    97,    98,    99,   100,   101,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,    -1,
      -1,    -1,    16,   119,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    -1,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    62,    63,
      -1,    -1,    -1,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    -1,    92,    93,
      -1,    95,    -1,    97,    98,    99,   100,   101,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
      12,    -1,    -1,    -1,    16,   119,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      62,    63,    -1,    -1,    -1,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    -1,
      92,    93,    -1,    95,    -1,    97,    98,    99,   100,   101,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,    -1,    -1,    -1,    16,   119,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    -1,    62,    63,    -1,    -1,    -1,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    -1,    92,    93,    -1,    -1,    -1,    97,    98,    99,
     100,   101,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,   119,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    -1,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    62,    63,    -1,    -1,    -1,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    -1,    92,    93,    -1,    -1,    -1,    97,
      98,    99,   100,   101,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,
      16,   119,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    -1,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,    63,    -1,    -1,
      -1,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    -1,    92,    93,    -1,    -1,
      -1,    97,    98,    99,   100,   101,    -1,    51,    52,    -1,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    65,    66,   119,    68,    69,    70,    71,    -1,    73,
      74,    -1,    -1,    77,    -1,    -1,    -1,    81,    82,    83,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,    51,    52,   121,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    -1,   130,    -1,    -1,    65,
      66,    -1,    68,    69,    70,    71,    -1,    73,    74,    -1,
      -1,    77,    -1,    -1,    -1,    81,    82,    83,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,    51,    52,   121,    -1,    55,    -1,    -1,
      -1,    -1,    -1,    -1,   130,    -1,    -1,    65,    66,    -1,
      68,    69,    70,    71,    -1,    73,    74,    -1,    -1,    77,
      -1,    -1,    -1,    81,    82,    83,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,    51,    52,   121,    -1,    55,    -1,    -1,    -1,    -1,
      -1,    -1,   130,    -1,    -1,    65,    66,    -1,    68,    69,
      70,    71,    -1,    73,    74,    -1,    -1,    77,    -1,    -1,
      -1,    81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,    51,
      52,   121,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,
     130,    -1,    -1,    65,    66,    -1,    68,    69,    70,    71,
      -1,    73,    74,    -1,    -1,    77,    -1,    -1,    -1,    81,
      82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,    51,    52,   121,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,   130,    -1,
      -1,    65,    66,    -1,    68,    69,    70,    71,    -1,    73,
      74,    -1,    -1,    77,    -1,    -1,    -1,    81,    82,    83,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,    51,    52,   121,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    -1,   130,    -1,    -1,    65,
      66,    -1,    68,    69,    70,    71,    -1,    73,    74,    -1,
      -1,    77,    -1,    -1,    -1,    81,    82,    83,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,    51,    52,   121,    -1,    55,    -1,    -1,
      -1,    -1,    -1,    -1,   130,    -1,    -1,    65,    66,    -1,
      68,    69,    70,    71,    -1,    73,    74,    -1,    -1,    77,
      -1,    -1,    -1,    81,    82,    83,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,    51,    52,   121,    -1,    55,    -1,    -1,    -1,    -1,
      -1,    -1,   130,    -1,    -1,    65,    66,    -1,    68,    69,
      70,    71,    -1,    73,    74,    -1,    -1,    77,    -1,    -1,
      -1,    81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,    51,
      52,   121,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,
     130,    -1,    -1,    65,    66,    -1,    68,    69,    70,    71,
      -1,    73,    74,    -1,    -1,    77,    -1,    -1,    -1,    81,
      82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,    51,    52,   121,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,   130,    -1,
      -1,    65,    66,    -1,    68,    69,    70,    71,    -1,    73,
      74,    -1,    -1,    77,    -1,    -1,    -1,    81,    82,    83,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,    51,    52,   121,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    -1,   130,    -1,    -1,    65,
      66,    -1,    68,    69,    70,    71,    -1,    73,    74,    -1,
      -1,    77,    -1,    -1,    -1,    81,    82,    83,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,    -1,    -1,   121,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   130,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   107,    -1,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,    -1,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      -1,   134,    83,    84,    -1,    -1,    -1,    88,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    -1,    -1,    83,    84,    -1,   107,    -1,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   129,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   129,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,   107,
      -1,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    68,    69,
      70,    71,    72,    73,    74,    75,    -1,    77,    78,    -1,
      -1,    -1,    -1,    83,    84,    68,    69,    70,    71,    72,
      73,    74,    -1,    -1,    77,    78,    -1,    -1,    -1,    -1,
      83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,    68,    69,    70,    71,
      72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,    -1,
      -1,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118
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
     156,   157,   158,   159,   160,   170,   188,   205,   215,   216,
     229,   230,   231,   232,   233,   234,   235,   238,   246,   248,
     249,   250,   251,   252,   253,   272,   281,   143,    21,    22,
      29,    30,    31,    45,    51,    55,    83,    86,    89,   119,
     161,   162,   188,   205,   250,   253,   272,   162,     3,     4,
       5,     6,     7,     8,     9,    10,    13,    14,    15,    16,
      17,    20,    21,    22,    23,    24,    25,    26,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    55,    65,    66,    68,    69,    70,    71,    73,
      74,    77,    81,    82,    83,    84,    95,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   121,   130,   131,
     163,   168,   169,   251,   267,    32,    33,    34,    35,    48,
      49,    51,    55,   163,   164,   165,   166,   246,   248,   189,
      86,   146,   147,   160,   205,   250,   252,   253,   147,   134,
     135,   147,   276,   279,   280,   192,   194,    86,   154,   160,
     205,   210,   250,   253,    95,    96,   120,   153,   170,   173,
     177,   184,   186,   270,   271,   177,   177,   131,   179,   180,
     131,   175,   179,   146,    53,   165,   134,   277,   145,   123,
     241,   170,   205,   170,    51,    52,    55,   168,   273,    55,
      89,   140,   155,   156,   146,    95,   153,   174,   186,   270,
     281,   186,   269,   270,   281,    86,   159,   205,   250,   253,
      53,    54,    56,   163,   241,   247,   240,   241,   133,   236,
     133,   239,    58,    59,   148,   170,   170,    50,   141,   281,
     276,   280,    39,    40,    41,    42,    43,    36,    37,    28,
     212,   106,   129,    89,    95,   157,   106,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      83,    84,   107,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,    85,   125,   127,   132,    85,   127,    26,
     123,   217,   232,    87,    87,   175,   179,   217,   277,   146,
      51,    55,   161,    58,    59,     1,   110,   254,   279,    85,
     125,   127,   201,   268,   202,    85,   127,   275,   129,   139,
     140,    55,    13,   108,   206,   279,   106,    85,   125,   127,
      87,    87,   206,   276,    15,    17,   222,   135,   147,   147,
      55,    85,   125,   127,    25,   173,   173,    88,   129,   185,
     281,   129,   185,   128,   177,    90,   177,   181,   153,   177,
     186,   215,   281,    53,    62,    63,   144,   131,   171,   123,
     140,    60,   102,   103,   104,   242,    85,   127,    87,   155,
     128,   128,   190,   170,   277,   126,   129,   134,   278,   129,
     278,   129,   278,   124,   278,    55,    85,   125,   127,   104,
     104,   242,    64,   104,   104,   237,   242,   104,    60,   104,
      68,    68,    61,   143,   147,   147,   147,   147,   143,   146,
     146,   213,    95,   148,   173,   186,   187,   155,   159,   129,
     148,   170,   173,   187,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   172,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
      51,    52,    55,   168,   273,   274,   174,    51,    55,   273,
     273,   273,   273,   220,   218,   148,   170,   148,   170,    94,
     150,   199,   279,   255,   198,    51,    55,   161,   273,   174,
     273,   139,   146,    51,    53,    54,    55,    56,    95,    96,
     113,   116,   131,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   203,   167,    10,     8,   225,   281,   140,
      13,   170,    51,    55,   174,    51,    55,   140,   222,   140,
      95,   186,   223,    10,    27,   108,   207,   279,   207,    51,
      55,   174,    51,    55,   196,   129,   185,   173,    95,   173,
     184,   270,    95,   173,   271,   128,    95,   170,   173,   178,
     182,   184,   270,   277,   129,    85,   127,   277,   128,   165,
     191,   170,   140,   124,   244,   243,   170,   128,   277,   277,
      95,   173,    51,    55,   174,    51,    55,   133,   242,   133,
     170,   170,    76,   111,   211,   281,   173,   129,   128,    43,
     170,    87,    87,   175,   179,   126,    87,    87,   175,   176,
     179,   281,   176,   179,   176,   179,   211,   211,   151,   279,
     147,   139,   126,    10,   277,   106,   257,   139,   279,   129,
     266,   281,   129,   266,    51,   129,   266,    51,   163,   165,
     173,   187,   226,   281,    15,   209,   281,    14,   208,   209,
      87,   126,    87,    87,   209,    10,    10,   173,   129,   206,
     193,   195,   126,   147,   173,   129,   185,   173,   173,   129,
     183,   128,   129,   185,   128,   153,   215,   273,   273,   128,
     146,   124,   140,    53,    54,    56,   245,   253,   128,   170,
     126,   111,   210,   214,    95,   173,   170,   108,   148,   170,
     170,   150,    87,   148,   170,   148,   170,   150,   221,   219,
     211,   200,   279,    10,   128,   173,   277,    10,   258,   261,
     263,   265,    51,   260,   263,   204,    88,   227,   281,   140,
       9,   228,   281,   147,    10,    87,    10,    95,   140,   140,
     140,   207,   185,    95,   185,   185,    95,   184,   186,   270,
     128,    95,   277,   128,   124,   277,   111,   140,   173,   170,
     148,   170,   140,   140,   152,   139,   128,   129,   266,   266,
     266,   256,    86,   160,   205,   250,   253,   206,   140,   206,
     173,   209,   222,   224,    10,    10,   197,   170,   173,   129,
     185,   129,   185,   173,   128,    10,    10,   124,   140,    10,
     263,   139,    55,    85,   125,   127,   140,   140,   140,   185,
     185,    95,   270,    95,   185,   124,   266,    10,    51,    55,
     174,    51,    55,   225,   208,    10,   173,   129,   185,   173,
     126,   185,    95,   185,   173,   185
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
#line 420 "grammar.y"
    {
                        vps->lex_state = EXPR_BEG;
                        vps->variables = new LocalState(0);
                        class_nest = 0;
                    ;}
    break;

  case 3:
#line 426 "grammar.y"
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
#line 449 "grammar.y"
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
#line 466 "grammar.y"
    {
                        void_stmts((yyvsp[(1) - (2)].node), vps);
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 7:
#line 474 "grammar.y"
    {
                      (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 9:
#line 480 "grammar.y"
    {
                        (yyval.node) = newline_node(vps, (yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 10:
#line 484 "grammar.y"
    {
                        (yyval.node) = block_append(vps, (yyvsp[(1) - (3)].node), newline_node(vps, (yyvsp[(3) - (3)].node)));
                    ;}
    break;

  case 11:
#line 488 "grammar.y"
    {
                        (yyval.node) = remove_begin((yyvsp[(2) - (2)].node), vps);
                    ;}
    break;

  case 12:
#line 493 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 13:
#line 494 "grammar.y"
    {
                        (yyval.node) = NEW_ALIAS((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 14:
#line 498 "grammar.y"
    {
                        (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 15:
#line 502 "grammar.y"
    {
                        char buf[3];

                        snprintf(buf, sizeof(buf), "$%c", (char)(yyvsp[(3) - (3)].node)->nd_nth);
                        (yyval.node) = NEW_VALIAS((yyvsp[(2) - (3)].id), rb_parser_sym(buf));
                    ;}
    break;

  case 16:
#line 509 "grammar.y"
    {
                        yyerror("can't make alias for the number variables");
                        (yyval.node) = 0;
                    ;}
    break;

  case 17:
#line 514 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 18:
#line 518 "grammar.y"
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
#line 527 "grammar.y"
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
#line 536 "grammar.y"
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
#line 548 "grammar.y"
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
#line 560 "grammar.y"
    {
                        NODE *resq = NEW_RESBODY(0, remove_begin((yyvsp[(3) - (3)].node), vps), 0);
                        (yyval.node) = NEW_RESCUE(remove_begin((yyvsp[(1) - (3)].node), vps), resq, 0);
                    ;}
    break;

  case 23:
#line 565 "grammar.y"
    {
                        if (in_def || in_single) {
                            yyerror("BEGIN in method");
                        }
                        local_push(0);
                    ;}
    break;

  case 24:
#line 572 "grammar.y"
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
#line 581 "grammar.y"
    {
                        if (in_def || in_single) {
                            rb_warn("END in method; use at_exit");
                        }

                        (yyval.node) = NEW_ITER(0, NEW_POSTEXE(), (yyvsp[(3) - (4)].node));
                    ;}
    break;

  case 26:
#line 589 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 27:
#line 593 "grammar.y"
    {
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyvsp[(1) - (3)].node)->nd_value = ((yyvsp[(1) - (3)].node)->nd_head) ? NEW_TO_ARY((yyvsp[(3) - (3)].node)) : NEW_ARRAY((yyvsp[(3) - (3)].node));
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 28:
#line 599 "grammar.y"
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
#line 624 "grammar.y"
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
#line 640 "grammar.y"
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
#line 652 "grammar.y"
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
#line 664 "grammar.y"
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
#line 676 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (3)].node), vps);
                        (yyval.node) = 0;
                    ;}
    break;

  case 34:
#line 681 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), NEW_SVALUE((yyvsp[(3) - (3)].node)), vps);
                    ;}
    break;

  case 35:
#line 685 "grammar.y"
    {
                        (yyvsp[(1) - (3)].node)->nd_value = ((yyvsp[(1) - (3)].node)->nd_head) ? NEW_TO_ARY((yyvsp[(3) - (3)].node)) : NEW_ARRAY((yyvsp[(3) - (3)].node));
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 36:
#line 690 "grammar.y"
    {
                        (yyvsp[(1) - (3)].node)->nd_value = (yyvsp[(3) - (3)].node);
                        (yyval.node) = (yyvsp[(1) - (3)].node);
                    ;}
    break;

  case 39:
#line 699 "grammar.y"
    {
                        (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 40:
#line 703 "grammar.y"
    {
                        (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 41:
#line 707 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), vps));
                    ;}
    break;

  case 42:
#line 711 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), vps));
                    ;}
    break;

  case 44:
#line 718 "grammar.y"
    {
                        value_expr((yyval.node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 47:
#line 727 "grammar.y"
    {
                        (yyval.node) = NEW_RETURN(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 48:
#line 731 "grammar.y"
    {
                        (yyval.node) = NEW_BREAK(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 49:
#line 735 "grammar.y"
    {
                        (yyval.node) = NEW_NEXT(ret_args(vps, (yyvsp[(2) - (2)].node)));
                    ;}
    break;

  case 51:
#line 742 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 52:
#line 746 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 53:
#line 752 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 54:
#line 756 "grammar.y"
    { (yyval.vars) = vps->variables->block_vars; ;}
    break;

  case 55:
#line 759 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 56:
#line 766 "grammar.y"
    {
                        (yyval.node) = new_fcall(vps, (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                   ;}
    break;

  case 57:
#line 771 "grammar.y"
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
#line 783 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 59:
#line 788 "grammar.y"
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
#line 800 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 61:
#line 805 "grammar.y"
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
#line 817 "grammar.y"
    {
                        (yyval.node) = new_super(vps, (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 63:
#line 822 "grammar.y"
    {
                        (yyval.node) = new_yield(vps, (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 65:
#line 830 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 67:
#line 837 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(NEW_LIST((yyvsp[(2) - (3)].node)), 0);
                    ;}
    break;

  case 68:
#line 843 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (1)].node), 0);
                    ;}
    break;

  case 69:
#line 847 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(list_append(vps, (yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node)), 0);
                    ;}
    break;

  case 70:
#line 851 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 71:
#line 855 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN((yyvsp[(1) - (2)].node), -1);
                    ;}
    break;

  case 72:
#line 859 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(0, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 73:
#line 863 "grammar.y"
    {
                        (yyval.node) = NEW_MASGN(0, -1);
                    ;}
    break;

  case 75:
#line 870 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 76:
#line 876 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 77:
#line 880 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 78:
#line 886 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, vps);
                    ;}
    break;

  case 79:
#line 890 "grammar.y"
    {
                        (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), vps);
                    ;}
    break;

  case 80:
#line 894 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 81:
#line 898 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 82:
#line 902 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 83:
#line 906 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                    ;}
    break;

  case 84:
#line 912 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                    ;}
    break;

  case 85:
#line 918 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (1)].node), vps);
                        (yyval.node) = 0;
                    ;}
    break;

  case 86:
#line 925 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, vps);
                    ;}
    break;

  case 87:
#line 929 "grammar.y"
    {
                        (yyval.node) = aryset((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), vps);
                    ;}
    break;

  case 88:
#line 933 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 89:
#line 937 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 90:
#line 941 "grammar.y"
    {
                        (yyval.node) = attrset((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), vps);
                    ;}
    break;

  case 91:
#line 945 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id)));
                    ;}
    break;

  case 92:
#line 951 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        (yyval.node) = NEW_CDECL(0, 0, NEW_COLON3((yyvsp[(2) - (2)].id)));
                    ;}
    break;

  case 93:
#line 957 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (1)].node), vps);
                        (yyval.node) = 0;
                    ;}
    break;

  case 94:
#line 964 "grammar.y"
    {
                        yyerror("class/module name must be CONSTANT");
                    ;}
    break;

  case 96:
#line 971 "grammar.y"
    {
                        (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 97:
#line 975 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2(0, (yyval.node));
                    ;}
    break;

  case 98:
#line 979 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 102:
#line 988 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = convert_op((yyvsp[(1) - (1)].id));
                    ;}
    break;

  case 103:
#line 993 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = (yyvsp[(1) - (1)].id);
                    ;}
    break;

  case 106:
#line 1004 "grammar.y"
    {
                        (yyval.node) = NEW_LIT(QUID2SYM((yyvsp[(1) - (1)].id)));
                    ;}
    break;

  case 108:
#line 1011 "grammar.y"
    {
                        (yyval.node) = NEW_UNDEF((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 109:
#line 1014 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 110:
#line 1015 "grammar.y"
    {
                        (yyval.node) = block_append(vps, (yyvsp[(1) - (4)].node), NEW_UNDEF((yyvsp[(4) - (4)].node)));
                    ;}
    break;

  case 111:
#line 1020 "grammar.y"
    { (yyval.id) = '|'; ;}
    break;

  case 112:
#line 1021 "grammar.y"
    { (yyval.id) = '^'; ;}
    break;

  case 113:
#line 1022 "grammar.y"
    { (yyval.id) = '&'; ;}
    break;

  case 114:
#line 1023 "grammar.y"
    { (yyval.id) = tCMP; ;}
    break;

  case 115:
#line 1024 "grammar.y"
    { (yyval.id) = tEQ; ;}
    break;

  case 116:
#line 1025 "grammar.y"
    { (yyval.id) = tEQQ; ;}
    break;

  case 117:
#line 1026 "grammar.y"
    { (yyval.id) = tMATCH; ;}
    break;

  case 118:
#line 1027 "grammar.y"
    { (yyval.id) = '>'; ;}
    break;

  case 119:
#line 1028 "grammar.y"
    { (yyval.id) = tGEQ; ;}
    break;

  case 120:
#line 1029 "grammar.y"
    { (yyval.id) = '<'; ;}
    break;

  case 121:
#line 1030 "grammar.y"
    { (yyval.id) = tLEQ; ;}
    break;

  case 122:
#line 1031 "grammar.y"
    { (yyval.id) = tLSHFT; ;}
    break;

  case 123:
#line 1032 "grammar.y"
    { (yyval.id) = tRSHFT; ;}
    break;

  case 124:
#line 1033 "grammar.y"
    { (yyval.id) = '+'; ;}
    break;

  case 125:
#line 1034 "grammar.y"
    { (yyval.id) = '-'; ;}
    break;

  case 126:
#line 1035 "grammar.y"
    { (yyval.id) = '*'; ;}
    break;

  case 127:
#line 1036 "grammar.y"
    { (yyval.id) = '*'; ;}
    break;

  case 128:
#line 1037 "grammar.y"
    { (yyval.id) = '/'; ;}
    break;

  case 129:
#line 1038 "grammar.y"
    { (yyval.id) = '%'; ;}
    break;

  case 130:
#line 1039 "grammar.y"
    { (yyval.id) = tPOW; ;}
    break;

  case 131:
#line 1040 "grammar.y"
    { (yyval.id) = '~'; ;}
    break;

  case 132:
#line 1041 "grammar.y"
    { (yyval.id) = tUPLUS; ;}
    break;

  case 133:
#line 1042 "grammar.y"
    { (yyval.id) = tUMINUS; ;}
    break;

  case 134:
#line 1043 "grammar.y"
    { (yyval.id) = tAREF; ;}
    break;

  case 135:
#line 1044 "grammar.y"
    { (yyval.id) = tASET; ;}
    break;

  case 136:
#line 1045 "grammar.y"
    { (yyval.id) = '`'; ;}
    break;

  case 179:
#line 1058 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 180:
#line 1062 "grammar.y"
    {
                        (yyval.node) = node_assign((yyvsp[(1) - (5)].node), NEW_RESCUE((yyvsp[(3) - (5)].node), NEW_RESBODY(0,(yyvsp[(5) - (5)].node),0), 0), vps);
                    ;}
    break;

  case 181:
#line 1066 "grammar.y"
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
#line 1091 "grammar.y"
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
#line 1107 "grammar.y"
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
#line 1119 "grammar.y"
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
#line 1131 "grammar.y"
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
#line 1143 "grammar.y"
    {
                        yyerror("constant re-assignment");
                        (yyval.node) = 0;
                    ;}
    break;

  case 187:
#line 1148 "grammar.y"
    {
                        yyerror("constant re-assignment");
                        (yyval.node) = 0;
                    ;}
    break;

  case 188:
#line 1153 "grammar.y"
    {
                        rb_backref_error((yyvsp[(1) - (3)].node), vps);
                        (yyval.node) = 0;
                    ;}
    break;

  case 189:
#line 1158 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (3)].node));
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyval.node) = NEW_DOT2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 190:
#line 1164 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (3)].node));
                        value_expr((yyvsp[(3) - (3)].node));
                        (yyval.node) = NEW_DOT3((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 191:
#line 1170 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '+', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 192:
#line 1174 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '-', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 193:
#line 1178 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '*', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 194:
#line 1182 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '/', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 195:
#line 1186 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '%', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 196:
#line 1190 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tPOW, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 197:
#line 1194 "grammar.y"
    {
                        (yyval.node) = call_op(call_op((yyvsp[(2) - (4)].node), tPOW, 1, (yyvsp[(4) - (4)].node), vps), tUMINUS, 0, 0, vps);
                    ;}
    break;

  case 198:
#line 1198 "grammar.y"
    {
                        (yyval.node) = call_op(call_op((yyvsp[(2) - (4)].node), tPOW, 1, (yyvsp[(4) - (4)].node), vps), tUMINUS, 0, 0, vps);
                    ;}
    break;

  case 199:
#line 1202 "grammar.y"
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
#line 1211 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(2) - (2)].node), tUMINUS, 0, 0, vps);
                    ;}
    break;

  case 201:
#line 1215 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '|', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 202:
#line 1219 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '^', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 203:
#line 1223 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '&', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 204:
#line 1227 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tCMP, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 205:
#line 1231 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '>', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 206:
#line 1235 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tGEQ, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 207:
#line 1239 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), '<', 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 208:
#line 1243 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tLEQ, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 209:
#line 1247 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tEQ, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 210:
#line 1251 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tEQQ, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 211:
#line 1255 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(call_op((yyvsp[(1) - (3)].node), tEQ, 1, (yyvsp[(3) - (3)].node), vps));
                    ;}
    break;

  case 212:
#line 1259 "grammar.y"
    {
                        (yyval.node) = match_gen((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 213:
#line 1263 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(match_gen((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps));
                    ;}
    break;

  case 214:
#line 1267 "grammar.y"
    {
                        (yyval.node) = NEW_NOT(cond((yyvsp[(2) - (2)].node), vps));
                    ;}
    break;

  case 215:
#line 1271 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(2) - (2)].node), '~', 0, 0, vps);
                    ;}
    break;

  case 216:
#line 1275 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tLSHFT, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 217:
#line 1279 "grammar.y"
    {
                        (yyval.node) = call_op((yyvsp[(1) - (3)].node), tRSHFT, 1, (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 218:
#line 1283 "grammar.y"
    {
                        (yyval.node) = logop(NODE_AND, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 219:
#line 1287 "grammar.y"
    {
                        (yyval.node) = logop(NODE_OR, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 220:
#line 1290 "grammar.y"
    {vps->in_defined = 1;;}
    break;

  case 221:
#line 1291 "grammar.y"
    {
                        vps->in_defined = 0;
                        (yyval.node) = NEW_DEFINED((yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 222:
#line 1295 "grammar.y"
    {vps->ternary_colon++;;}
    break;

  case 223:
#line 1296 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(1) - (6)].node), vps), (yyvsp[(4) - (6)].node), (yyvsp[(6) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (6)].node));
                        vps->ternary_colon--;
                    ;}
    break;

  case 224:
#line 1302 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 225:
#line 1308 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (1)].node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 227:
#line 1316 "grammar.y"
    {
                        rb_warn("parenthesize argument(s) for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 228:
#line 1321 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 229:
#line 1325 "grammar.y"
    {
                        value_expr((yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_concat(vps, (yyvsp[(1) - (5)].node), (yyvsp[(4) - (5)].node));
                    ;}
    break;

  case 230:
#line 1330 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                    ;}
    break;

  case 231:
#line 1334 "grammar.y"
    {
                        value_expr((yyvsp[(2) - (3)].node));
                        (yyval.node) = NEW_NEWLINE(NEW_SPLAT((yyvsp[(2) - (3)].node)));
                    ;}
    break;

  case 232:
#line 1341 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 233:
#line 1345 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                    ;}
    break;

  case 234:
#line 1349 "grammar.y"
    {
                        rb_warn("parenthesize argument for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(2) - (4)].node));
                    ;}
    break;

  case 235:
#line 1354 "grammar.y"
    {
                        rb_warn("parenthesize argument for future version");
                        (yyval.node) = list_append(vps, (yyvsp[(2) - (6)].node), (yyvsp[(4) - (6)].node));
                    ;}
    break;

  case 238:
#line 1365 "grammar.y"
    {
                        rb_warn("parenthesize argument(s) for future version");
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 239:
#line 1370 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 240:
#line 1374 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, (yyvsp[(1) - (5)].node), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 241:
#line 1379 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 242:
#line 1384 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, NEW_LIST(NEW_HASH((yyvsp[(1) - (5)].node))), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 243:
#line 1389 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (4)].node), NEW_HASH((yyvsp[(3) - (4)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 244:
#line 1394 "grammar.y"
    {
                        value_expr((yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_concat(vps, list_append(vps, (yyvsp[(1) - (7)].node), NEW_HASH((yyvsp[(3) - (7)].node))), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 245:
#line 1400 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(NEW_SPLAT((yyvsp[(2) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 247:
#line 1407 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(list_concat(NEW_LIST((yyvsp[(1) - (4)].node)),(yyvsp[(3) - (4)].node)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 248:
#line 1411 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 249:
#line 1415 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, NEW_LIST((yyvsp[(1) - (5)].node)), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 250:
#line 1420 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, list_concat(NEW_LIST((yyvsp[(1) - (7)].node)),(yyvsp[(3) - (7)].node)), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 251:
#line 1425 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_HASH((yyvsp[(1) - (2)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 252:
#line 1430 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, NEW_LIST(NEW_HASH((yyvsp[(1) - (5)].node))), (yyvsp[(4) - (5)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 253:
#line 1435 "grammar.y"
    {
                        (yyval.node) = list_append(vps, NEW_LIST((yyvsp[(1) - (4)].node)), NEW_HASH((yyvsp[(3) - (4)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 254:
#line 1440 "grammar.y"
    {
                        (yyval.node) = list_append(vps, list_concat(NEW_LIST((yyvsp[(1) - (6)].node)),(yyvsp[(3) - (6)].node)), NEW_HASH((yyvsp[(5) - (6)].node)));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(6) - (6)].node));
                    ;}
    break;

  case 255:
#line 1445 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, list_append(vps, NEW_LIST((yyvsp[(1) - (7)].node)), NEW_HASH((yyvsp[(3) - (7)].node))), (yyvsp[(6) - (7)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(7) - (7)].node));
                    ;}
    break;

  case 256:
#line 1450 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, list_append(vps,
                                        list_concat(NEW_LIST((yyvsp[(1) - (9)].node)), (yyvsp[(3) - (9)].node)), NEW_HASH((yyvsp[(5) - (9)].node))), (yyvsp[(8) - (9)].node));
                        (yyval.node) = arg_blk_pass((yyval.node), (yyvsp[(9) - (9)].node));
                    ;}
    break;

  case 257:
#line 1456 "grammar.y"
    {
                        (yyval.node) = arg_blk_pass(NEW_SPLAT((yyvsp[(2) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 259:
#line 1462 "grammar.y"
    {
                        (yyval.num) = vps->cmdarg_stack;
                        CMDARG_PUSH(1);
                    ;}
    break;

  case 260:
#line 1467 "grammar.y"
    {
                        /* CMDARG_POP() */
                        vps->cmdarg_stack = (yyvsp[(1) - (2)].num);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 262:
#line 1475 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 263:
#line 1476 "grammar.y"
    {
                        rb_warn("don't put space before argument parentheses");
                        (yyval.node) = 0;
                    ;}
    break;

  case 264:
#line 1480 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 265:
#line 1481 "grammar.y"
    {
                        rb_warn("don't put space before argument parentheses");
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                    ;}
    break;

  case 266:
#line 1488 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK_PASS((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 267:
#line 1494 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 269:
#line 1501 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 270:
#line 1505 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 271:
#line 1511 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 272:
#line 1515 "grammar.y"
    {
                        (yyval.node) = arg_concat(vps, (yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 273:
#line 1519 "grammar.y"
    {
                        (yyval.node) = NEW_SPLAT((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 282:
#line 1533 "grammar.y"
    {
                        (yyval.node) = NEW_FCALL((yyvsp[(1) - (1)].id), 0);
                    ;}
    break;

  case 283:
#line 1537 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                    ;}
    break;

  case 284:
#line 1542 "grammar.y"
    {
                        if ((yyvsp[(3) - (4)].node) == NULL)
                            (yyval.node) = NEW_NIL();
                        else
                            (yyval.node) = NEW_BEGIN((yyvsp[(3) - (4)].node));
                        nd_set_line((yyval.node), (yyvsp[(1) - (4)].num));
                    ;}
    break;

  case 285:
#line 1549 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;;}
    break;

  case 286:
#line 1550 "grammar.y"
    {
                        rb_warning("(...) interpreted as grouped expression");
                        (yyval.node) = (yyvsp[(2) - (5)].node);
                    ;}
    break;

  case 287:
#line 1555 "grammar.y"
    {
                        if (!(yyvsp[(2) - (3)].node)) (yyval.node) = NEW_NIL();
                        else (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 288:
#line 1560 "grammar.y"
    {
                        (yyval.node) = NEW_COLON2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id));
                    ;}
    break;

  case 289:
#line 1564 "grammar.y"
    {
                        (yyval.node) = NEW_COLON3((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 290:
#line 1568 "grammar.y"
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
#line 1577 "grammar.y"
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
#line 1586 "grammar.y"
    {
                        (yyval.node) = NEW_HASH((yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 293:
#line 1590 "grammar.y"
    {
                        (yyval.node) = NEW_RETURN(0);
                    ;}
    break;

  case 294:
#line 1594 "grammar.y"
    {
                        (yyval.node) = new_yield(vps, (yyvsp[(3) - (4)].node));
                    ;}
    break;

  case 295:
#line 1598 "grammar.y"
    {
                        (yyval.node) = NEW_YIELD(0, Qfalse);
                    ;}
    break;

  case 296:
#line 1602 "grammar.y"
    {
                        (yyval.node) = NEW_YIELD(0, Qfalse);
                    ;}
    break;

  case 297:
#line 1605 "grammar.y"
    {vps->in_defined = 1;;}
    break;

  case 298:
#line 1606 "grammar.y"
    {
                        vps->in_defined = 0;
                        (yyval.node) = NEW_DEFINED((yyvsp[(5) - (6)].node));
                    ;}
    break;

  case 299:
#line 1611 "grammar.y"
    {
                        (yyvsp[(2) - (2)].node)->nd_iter = NEW_FCALL((yyvsp[(1) - (2)].id), 0);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                        fixpos((yyvsp[(2) - (2)].node)->nd_iter, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 301:
#line 1618 "grammar.y"
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
#line 1630 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(2) - (6)].node), vps), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (6)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            NODE *tmp = (yyval.node)->nd_body;
                            (yyval.node)->nd_body = (yyval.node)->nd_else;
                            (yyval.node)->nd_else = tmp;
                        }
                    ;}
    break;

  case 303:
#line 1643 "grammar.y"
    {
                        (yyval.node) = NEW_UNLESS(cond((yyvsp[(2) - (6)].node), vps), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (6)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            NODE *tmp = (yyval.node)->nd_body;
                            (yyval.node)->nd_body = (yyval.node)->nd_else;
                            (yyval.node)->nd_else = tmp;
                        }
                    ;}
    break;

  case 304:
#line 1652 "grammar.y"
    {COND_PUSH(1);;}
    break;

  case 305:
#line 1652 "grammar.y"
    {COND_POP();;}
    break;

  case 306:
#line 1655 "grammar.y"
    {
                        (yyval.node) = NEW_WHILE(cond((yyvsp[(3) - (7)].node), vps), (yyvsp[(6) - (7)].node), 1);
                        fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            nd_set_type((yyval.node), NODE_UNTIL);
                        }
                    ;}
    break;

  case 307:
#line 1662 "grammar.y"
    {COND_PUSH(1);;}
    break;

  case 308:
#line 1662 "grammar.y"
    {COND_POP();;}
    break;

  case 309:
#line 1665 "grammar.y"
    {
                        (yyval.node) = NEW_UNTIL(cond((yyvsp[(3) - (7)].node), vps), (yyvsp[(6) - (7)].node), 1);
                        fixpos((yyval.node), (yyvsp[(3) - (7)].node));
                        if (cond_negative(&(yyval.node)->nd_cond)) {
                            nd_set_type((yyval.node), NODE_WHILE);
                        }
                    ;}
    break;

  case 310:
#line 1675 "grammar.y"
    {
                        (yyval.node) = NEW_CASE((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (5)].node));
                    ;}
    break;

  case 311:
#line 1680 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(3) - (4)].node);
                    ;}
    break;

  case 312:
#line 1684 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(4) - (5)].node);
                    ;}
    break;

  case 313:
#line 1687 "grammar.y"
    {COND_PUSH(1);;}
    break;

  case 314:
#line 1687 "grammar.y"
    {COND_POP();;}
    break;

  case 315:
#line 1690 "grammar.y"
    {
                        (yyval.node) = NEW_FOR((yyvsp[(2) - (9)].node), (yyvsp[(5) - (9)].node), (yyvsp[(8) - (9)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (9)].node));
                    ;}
    break;

  case 316:
#line 1695 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("class definition in method body");
                        class_nest++;
                        local_push(0);
                        (yyval.num) = ruby_sourceline;
                    ;}
    break;

  case 317:
#line 1704 "grammar.y"
    {
                        (yyval.node) = NEW_CLASS((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(3) - (6)].node));
                        nd_set_line((yyval.node), (yyvsp[(4) - (6)].num));
                        local_pop();
                        class_nest--;
                    ;}
    break;

  case 318:
#line 1711 "grammar.y"
    {
                        (yyval.num) = in_def;
                        in_def = 0;
                    ;}
    break;

  case 319:
#line 1716 "grammar.y"
    {
                        (yyval.num) = in_single;
                        in_single = 0;
                        class_nest++;
                        local_push(0);
                    ;}
    break;

  case 320:
#line 1724 "grammar.y"
    {
                        (yyval.node) = NEW_SCLASS((yyvsp[(3) - (8)].node), (yyvsp[(7) - (8)].node));
                        fixpos((yyval.node), (yyvsp[(3) - (8)].node));
                        local_pop();
                        class_nest--;
                        in_def = (yyvsp[(4) - (8)].num);
                        in_single = (yyvsp[(6) - (8)].num);
                    ;}
    break;

  case 321:
#line 1733 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("module definition in method body");
                        class_nest++;
                        local_push(0);
                        (yyval.num) = ruby_sourceline;
                    ;}
    break;

  case 322:
#line 1742 "grammar.y"
    {
                        (yyval.node) = NEW_MODULE((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node));
                        nd_set_line((yyval.node), (yyvsp[(3) - (5)].num));
                        local_pop();
                        class_nest--;
                    ;}
    break;

  case 323:
#line 1749 "grammar.y"
    {
                        (yyval.id) = cur_mid;
                        cur_mid = (yyvsp[(2) - (2)].id);
                        in_def++;
                        local_push(0);
                    ;}
    break;

  case 324:
#line 1758 "grammar.y"
    {
                        if (!(yyvsp[(5) - (6)].node)) (yyvsp[(5) - (6)].node) = NEW_NIL();
                        (yyval.node) = NEW_DEFN((yyvsp[(2) - (6)].id), (yyvsp[(4) - (6)].node), (yyvsp[(5) - (6)].node), NOEX_PRIVATE);
                        fixpos((yyval.node), (yyvsp[(4) - (6)].node));
                        local_pop();
                        in_def--;
                        cur_mid = (yyvsp[(3) - (6)].id);
                    ;}
    break;

  case 325:
#line 1766 "grammar.y"
    {vps->lex_state = EXPR_FNAME;;}
    break;

  case 326:
#line 1767 "grammar.y"
    {
                        in_single++;
                        local_push(0);
                        vps->lex_state = EXPR_END; /* force for args */
                    ;}
    break;

  case 327:
#line 1775 "grammar.y"
    {
                        (yyval.node) = NEW_DEFS((yyvsp[(2) - (9)].node), (yyvsp[(5) - (9)].id), (yyvsp[(7) - (9)].node), (yyvsp[(8) - (9)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (9)].node));
                        local_pop();
                        in_single--;
                    ;}
    break;

  case 328:
#line 1782 "grammar.y"
    {
                        (yyval.node) = NEW_BREAK(0);
                    ;}
    break;

  case 329:
#line 1786 "grammar.y"
    {
                        (yyval.node) = NEW_NEXT(0);
                    ;}
    break;

  case 330:
#line 1790 "grammar.y"
    {
                        (yyval.node) = NEW_REDO();
                    ;}
    break;

  case 331:
#line 1794 "grammar.y"
    {
                        (yyval.node) = NEW_RETRY();
                    ;}
    break;

  case 332:
#line 1800 "grammar.y"
    {
                        value_expr((yyvsp[(1) - (1)].node));
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                    ;}
    break;

  case 341:
#line 1821 "grammar.y"
    {
                        (yyval.node) = NEW_IF(cond((yyvsp[(2) - (5)].node), vps), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (5)].node));
                    ;}
    break;

  case 343:
#line 1829 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 347:
#line 1840 "grammar.y"
    {
                        (yyval.node) = (NODE*)1;
                    ;}
    break;

  case 348:
#line 1844 "grammar.y"
    {
                        (yyval.node) = (NODE*)1;
                    ;}
    break;

  case 349:
#line 1848 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 350:
#line 1854 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 351:
#line 1859 "grammar.y"
    {
                      (yyval.vars) = vps->variables->block_vars;
                    ;}
    break;

  case 352:
#line 1864 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 353:
#line 1871 "grammar.y"
    {
                        if ((yyvsp[(1) - (2)].node) && nd_type((yyvsp[(1) - (2)].node)) == NODE_BLOCK_PASS) {
                            rb_compile_error(vps, "both block arg and actual block given");
                        }
                        (yyvsp[(2) - (2)].node)->nd_iter = (yyvsp[(1) - (2)].node);
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                        fixpos((yyval.node), (yyvsp[(1) - (2)].node));
                    ;}
    break;

  case 354:
#line 1880 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 355:
#line 1884 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 356:
#line 1890 "grammar.y"
    {
                        (yyval.node) = new_fcall(vps, (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 357:
#line 1895 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 358:
#line 1900 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].node));
                        fixpos((yyval.node), (yyvsp[(1) - (4)].node));
                    ;}
    break;

  case 359:
#line 1905 "grammar.y"
    {
                        (yyval.node) = new_call(vps, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].id), 0);
                    ;}
    break;

  case 360:
#line 1909 "grammar.y"
    {
                        (yyval.node) = NEW_CALL((yyvsp[(1) - (3)].node), rb_parser_sym("get_reference"),
                        NEW_LIST(NEW_LIT(QUID2SYM((yyvsp[(3) - (3)].id)))));
                    ;}
    break;

  case 361:
#line 1914 "grammar.y"
    {
                        (yyval.node) = NEW_FCALL(rb_parser_sym("get_reference"),
                        NEW_LIST(NEW_LIT(QUID2SYM((yyvsp[(2) - (2)].id)))));
                    ;}
    break;

  case 362:
#line 1919 "grammar.y"
    {
                        (yyval.node) = new_super(vps, (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 363:
#line 1923 "grammar.y"
    {
                        (yyval.node) = NEW_ZSUPER();
                    ;}
    break;

  case 364:
#line 1929 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 365:
#line 1933 "grammar.y"
    { (yyval.vars) = vps->variables->block_vars; ;}
    break;

  case 366:
#line 1935 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 367:
#line 1940 "grammar.y"
    {
                        (yyvsp[(1) - (1)].num) = ruby_sourceline;
                        reset_block(vps);
                    ;}
    break;

  case 368:
#line 1944 "grammar.y"
    { (yyval.vars) = vps->variables->block_vars; ;}
    break;

  case 369:
#line 1946 "grammar.y"
    {
                        (yyval.node) = NEW_ITER((yyvsp[(3) - (6)].node), 0, extract_block_vars(vps, (yyvsp[(5) - (6)].node), (yyvsp[(4) - (6)].vars)));
                        nd_set_line((yyval.node), (yyvsp[(1) - (6)].num));
                    ;}
    break;

  case 370:
#line 1955 "grammar.y"
    {
                        (yyval.node) = NEW_WHEN((yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node), (yyvsp[(5) - (5)].node));
                    ;}
    break;

  case 372:
#line 1961 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (4)].node), NEW_WHEN((yyvsp[(4) - (4)].node), 0, 0));
                    ;}
    break;

  case 373:
#line 1965 "grammar.y"
    {
                        (yyval.node) = NEW_LIST(NEW_WHEN((yyvsp[(2) - (2)].node), 0, 0));
                    ;}
    break;

  case 376:
#line 1977 "grammar.y"
    {
                        if ((yyvsp[(3) - (6)].node)) {
                            (yyvsp[(3) - (6)].node) = node_assign((yyvsp[(3) - (6)].node), NEW_GVAR(rb_parser_sym("$!")), vps);
                            (yyvsp[(5) - (6)].node) = block_append(vps, (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].node));
                        }
                        (yyval.node) = NEW_RESBODY((yyvsp[(2) - (6)].node), (yyvsp[(5) - (6)].node), (yyvsp[(6) - (6)].node));
                        fixpos((yyval.node), (yyvsp[(2) - (6)].node)?(yyvsp[(2) - (6)].node):(yyvsp[(5) - (6)].node));
                    ;}
    break;

  case 378:
#line 1989 "grammar.y"
    {
                        (yyval.node) = NEW_LIST((yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 381:
#line 1997 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 383:
#line 2004 "grammar.y"
    {
                        if ((yyvsp[(2) - (2)].node))
                            (yyval.node) = (yyvsp[(2) - (2)].node);
                        else
                            /* place holder */
                            (yyval.node) = NEW_NIL();
                    ;}
    break;

  case 386:
#line 2016 "grammar.y"
    {
                        (yyval.node) = NEW_LIT(QUID2SYM((yyvsp[(1) - (1)].id)));
                    ;}
    break;

  case 388:
#line 2023 "grammar.y"
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

  case 390:
#line 2037 "grammar.y"
    {
                        (yyval.node) = literal_concat(vps, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 391:
#line 2043 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 392:
#line 2049 "grammar.y"
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

  case 393:
#line 2072 "grammar.y"
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

  case 394:
#line 2102 "grammar.y"
    {
                        (yyval.node) = NEW_ZARRAY();
                    ;}
    break;

  case 395:
#line 2106 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 396:
#line 2112 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 397:
#line 2116 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), evstr2dstr(vps, (yyvsp[(2) - (3)].node)));
                    ;}
    break;

  case 399:
#line 2123 "grammar.y"
    {
                        (yyval.node) = literal_concat(vps, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 400:
#line 2129 "grammar.y"
    {
                        (yyval.node) = NEW_ZARRAY();
                    ;}
    break;

  case 401:
#line 2133 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (3)].node);
                    ;}
    break;

  case 402:
#line 2139 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 403:
#line 2143 "grammar.y"
    {
                        (yyval.node) = list_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].node));
                    ;}
    break;

  case 404:
#line 2149 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 405:
#line 2153 "grammar.y"
    {
                        (yyval.node) = literal_concat(vps, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 406:
#line 2159 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 407:
#line 2163 "grammar.y"
    {
                        (yyval.node) = literal_concat(vps, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 409:
#line 2170 "grammar.y"
    {
                        (yyval.node) = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                    ;}
    break;

  case 410:
#line 2176 "grammar.y"
    {
                        lex_strterm = (yyvsp[(2) - (3)].node);
                        (yyval.node) = NEW_EVSTR((yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 411:
#line 2181 "grammar.y"
    {
                        (yyval.node) = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                        COND_PUSH(0);
                        CMDARG_PUSH(0);
                    ;}
    break;

  case 412:
#line 2189 "grammar.y"
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

  case 413:
#line 2200 "grammar.y"
    {(yyval.node) = NEW_GVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 414:
#line 2201 "grammar.y"
    {(yyval.node) = NEW_IVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 415:
#line 2202 "grammar.y"
    {(yyval.node) = NEW_CVAR((yyvsp[(1) - (1)].id));;}
    break;

  case 417:
#line 2207 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        (yyval.id) = (yyvsp[(2) - (2)].id);
                    ;}
    break;

  case 422:
#line 2220 "grammar.y"
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

  case 425:
#line 2253 "grammar.y"
    {
                        (yyval.node) = NEW_NEGATE((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 426:
#line 2257 "grammar.y"
    {
                        (yyval.node) = NEW_NEGATE((yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 432:
#line 2267 "grammar.y"
    {(yyval.id) = kNIL;;}
    break;

  case 433:
#line 2268 "grammar.y"
    {(yyval.id) = kSELF;;}
    break;

  case 434:
#line 2269 "grammar.y"
    {(yyval.id) = kTRUE;;}
    break;

  case 435:
#line 2270 "grammar.y"
    {(yyval.id) = kFALSE;;}
    break;

  case 436:
#line 2271 "grammar.y"
    {(yyval.id) = k__FILE__;;}
    break;

  case 437:
#line 2272 "grammar.y"
    {(yyval.id) = k__LINE__;;}
    break;

  case 438:
#line 2276 "grammar.y"
    {
                        (yyval.node) = gettable((yyvsp[(1) - (1)].id));
                    ;}
    break;

  case 439:
#line 2282 "grammar.y"
    {
                        (yyval.node) = assignable((yyvsp[(1) - (1)].id), 0, vps);
                    ;}
    break;

  case 442:
#line 2292 "grammar.y"
    {
                        (yyval.node) = 0;
                    ;}
    break;

  case 443:
#line 2296 "grammar.y"
    {
                        vps->lex_state = EXPR_BEG;
                    ;}
    break;

  case 444:
#line 2300 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(3) - (4)].node);
                    ;}
    break;

  case 445:
#line 2303 "grammar.y"
    {yyerrok; (yyval.node) = 0;;}
    break;

  case 446:
#line 2307 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (4)].node);
                        vps->lex_state = EXPR_BEG;
                        command_start = TRUE;
                    ;}
    break;

  case 447:
#line 2313 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 448:
#line 2319 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS((intptr_t)(yyvsp[(1) - (6)].num), (yyvsp[(3) - (6)].node), (yyvsp[(5) - (6)].id)), (yyvsp[(6) - (6)].node));
                    ;}
    break;

  case 449:
#line 2323 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS((intptr_t)(yyvsp[(1) - (4)].num), (yyvsp[(3) - (4)].node), 0), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 450:
#line 2327 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS((intptr_t)(yyvsp[(1) - (4)].num), 0, (yyvsp[(3) - (4)].id)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 451:
#line 2331 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS((intptr_t)(yyvsp[(1) - (2)].num), 0, 0), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 452:
#line 2335 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS(0, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].id)), (yyvsp[(4) - (4)].node));
                    ;}
    break;

  case 453:
#line 2339 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS(0, (yyvsp[(1) - (2)].node), 0), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 454:
#line 2343 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS(0, 0, (yyvsp[(1) - (2)].id)), (yyvsp[(2) - (2)].node));
                    ;}
    break;

  case 455:
#line 2347 "grammar.y"
    {
                        (yyval.node) = block_append(vps, NEW_ARGS(0, 0, 0), (yyvsp[(1) - (1)].node));
                    ;}
    break;

  case 456:
#line 2351 "grammar.y"
    {
                        (yyval.node) = NEW_ARGS(0, 0, 0);
                    ;}
    break;

  case 457:
#line 2357 "grammar.y"
    {
                        yyerror("formal argument cannot be a constant");
                    ;}
    break;

  case 458:
#line 2361 "grammar.y"
    {
                        yyerror("formal argument cannot be an instance variable");
                    ;}
    break;

  case 459:
#line 2365 "grammar.y"
    {
                        yyerror("formal argument cannot be a global variable");
                    ;}
    break;

  case 460:
#line 2369 "grammar.y"
    {
                        yyerror("formal argument cannot be a class variable");
                    ;}
    break;

  case 461:
#line 2373 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(1) - (1)].id)))
                            yyerror("formal argument must be local variable");
                        else if (local_id((yyvsp[(1) - (1)].id)))
                            yyerror("duplicate argument name");
                        local_cnt((yyvsp[(1) - (1)].id));
                        (yyval.num) = 1;
                    ;}
    break;

  case 463:
#line 2385 "grammar.y"
    {
                        (yyval.num) += 1;
                    ;}
    break;

  case 464:
#line 2391 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(1) - (3)].id)))
                            yyerror("formal argument must be local variable");
                        else if (local_id((yyvsp[(1) - (3)].id)))
                            yyerror("duplicate optional argument name");
                        (yyval.node) = assignable((yyvsp[(1) - (3)].id), (yyvsp[(3) - (3)].node), vps);
                    ;}
    break;

  case 465:
#line 2401 "grammar.y"
    {
                        (yyval.node) = NEW_BLOCK((yyvsp[(1) - (1)].node));
                        (yyval.node)->nd_end = (yyval.node);
                    ;}
    break;

  case 466:
#line 2406 "grammar.y"
    {
                        (yyval.node) = block_append(vps, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 469:
#line 2416 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(2) - (2)].id)))
                            yyerror("rest argument must be local variable");
                        else if (local_id((yyvsp[(2) - (2)].id)))
                            yyerror("duplicate rest argument name");
                        (yyval.id) = local_cnt((yyvsp[(2) - (2)].id)) + 1;
                    ;}
    break;

  case 470:
#line 2424 "grammar.y"
    {
                        (yyval.id) = -2;
                    ;}
    break;

  case 473:
#line 2434 "grammar.y"
    {
                        if (!is_local_id((yyvsp[(2) - (2)].id)))
                            yyerror("block argument must be local variable");
                        else if (local_id((yyvsp[(2) - (2)].id)))
                            yyerror("duplicate block argument name");
                        (yyval.node) = NEW_BLOCK_ARG((yyvsp[(2) - (2)].id));
                    ;}
    break;

  case 474:
#line 2444 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(2) - (2)].node);
                    ;}
    break;

  case 476:
#line 2451 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (1)].node);
                        value_expr((yyval.node));
                    ;}
    break;

  case 477:
#line 2455 "grammar.y"
    {vps->lex_state = EXPR_BEG;;}
    break;

  case 478:
#line 2456 "grammar.y"
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

  case 480:
#line 2482 "grammar.y"
    {
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 481:
#line 2486 "grammar.y"
    {
                        if ((yyvsp[(1) - (2)].node)->nd_alen%2 != 0) {
                            yyerror("odd number list for Hash");
                        }
                        (yyval.node) = (yyvsp[(1) - (2)].node);
                    ;}
    break;

  case 483:
#line 2496 "grammar.y"
    {
                        (yyval.node) = list_concat((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 484:
#line 2502 "grammar.y"
    {
                        (yyval.node) = list_append(vps, NEW_LIST((yyvsp[(1) - (3)].node)), (yyvsp[(3) - (3)].node));
                    ;}
    break;

  case 504:
#line 2540 "grammar.y"
    {yyerrok;;}
    break;

  case 507:
#line 2545 "grammar.y"
    {yyerrok;;}
    break;

  case 508:
#line 2548 "grammar.y"
    {(yyval.node) = 0;;}
    break;


/* Line 1267 of yacc.c.  */
#line 7445 "grammar.cpp"
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


#line 2550 "grammar.y"


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
  assert(vars == parse_state->variables->block_vars);
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


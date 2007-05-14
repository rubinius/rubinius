/* A Bison parser, made from grammar.y, by GNU bison 1.75.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON	1

/* Pure parsers.  */
#define YYPURE	1

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

#include "grammar_internal.h"
#include "grammar_runtime.h"
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

static ID rb_intern(char *name);
static ID rb_id_attrset(ID);

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
#define SIGN_EXTEND(x,n) (((1<<(n)-1)^((x)&~(~0<<(n))))-(1<<(n)-1))
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

#ifndef YYSTYPE
#line 273 "grammar.y"
typedef union {
    NODE *node;
    ID id;
    int num;
    var_table *vars;
} yystype;
/* Line 188 of /usr/local/share/bison/yacc.c.  */
#line 550 "grammar.c"
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif

#ifndef YYLTYPE
typedef struct yyltype
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} yyltype;
# define YYLTYPE yyltype
# define YYLTYPE_IS_TRIVIAL 1
#endif

/* Copy the second part of user declarations.  */


/* Line 213 of /usr/local/share/bison/yacc.c.  */
#line 571 "grammar.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];	\
	}					\
      while (0)
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
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  3
#define YYLAST   9919

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  135
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  146
/* YYNRULES -- Number of rules. */
#define YYNRULES  507
/* YYNRULES -- Number of states. */
#define YYNSTATES  905

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   361

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
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
static const unsigned short yyprhs[] =
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
    1505,  1507,  1509,  1513,  1516,  1518,  1520,  1522,  1524,  1526,
    1528,  1530,  1532,  1534,  1536,  1538,  1540,  1541,  1543,  1544,
    1546,  1547,  1549,  1551,  1553,  1555,  1557,  1560
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
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
     112,    -1,    68,    -1,    69,    -1,    71,    -1,    70,    -1,
      76,    -1,   108,    -1,    72,    -1,   109,    -1,    73,    -1,
      82,    -1,    83,    -1,   113,    -1,   114,    -1,   115,    -1,
      94,    -1,   116,    -1,   117,    -1,    67,    -1,   120,    -1,
      64,    -1,    65,    -1,    80,    -1,    81,    -1,   129,    -1,
      48,    -1,    49,    -1,    46,    -1,    47,    -1,    44,    -1,
      36,    -1,     7,    -1,    21,    -1,    16,    -1,     3,    -1,
       5,    -1,    45,    -1,    26,    -1,    15,    -1,    14,    -1,
      10,    -1,     9,    -1,    35,    -1,    20,    -1,    25,    -1,
       4,    -1,    22,    -1,    33,    -1,    38,    -1,    37,    -1,
      23,    -1,     8,    -1,    24,    -1,    29,    -1,    32,    -1,
      31,    -1,    13,    -1,    34,    -1,     6,    -1,    17,    -1,
      30,    -1,    39,    -1,    40,    -1,    41,    -1,    42,    -1,
      43,    -1,   158,   105,   167,    -1,   158,   105,   167,    43,
     167,    -1,   249,    86,   167,    -1,   202,   124,   171,   125,
      86,   167,    -1,   202,   126,    50,    86,   167,    -1,   202,
     126,    54,    86,   167,    -1,   202,    84,    50,    86,   167,
      -1,   202,    84,    54,    86,   167,    -1,    85,    54,    86,
     167,    -1,   250,    86,   167,    -1,   167,    78,   167,    -1,
     167,    79,   167,    -1,   167,   113,   167,    -1,   167,   114,
     167,    -1,   167,   115,   167,    -1,   167,   116,   167,    -1,
     167,   117,   167,    -1,   167,    67,   167,    -1,   118,    57,
      67,   167,    -1,   118,    58,    67,   167,    -1,    64,   167,
      -1,    65,   167,    -1,   167,   110,   167,    -1,   167,   111,
     167,    -1,   167,   112,   167,    -1,   167,    68,   167,    -1,
     167,   108,   167,    -1,   167,    72,   167,    -1,   167,   109,
     167,    -1,   167,    73,   167,    -1,   167,    69,   167,    -1,
     167,    70,   167,    -1,   167,    71,   167,    -1,   167,    76,
     167,    -1,   167,    77,   167,    -1,   119,   167,    -1,   120,
     167,    -1,   167,    82,   167,    -1,   167,    83,   167,    -1,
     167,    74,   167,    -1,   167,    75,   167,    -1,    -1,    45,
     276,   168,   167,    -1,    -1,   167,   106,   169,   167,   107,
     167,    -1,   185,    -1,   167,    -1,   280,    -1,   151,   276,
      -1,   183,   277,    -1,   183,   128,    94,   167,   276,    -1,
     267,   277,    -1,    94,   167,   276,    -1,   130,   280,   127,
      -1,   130,   174,   276,   127,    -1,   130,   212,   276,   127,
      -1,   130,   183,   128,   212,   276,   127,    -1,   280,    -1,
     172,    -1,   151,    -1,   183,   182,    -1,   183,   128,    94,
     170,   182,    -1,   267,   182,    -1,   267,   128,    94,   170,
     182,    -1,   183,   128,   267,   182,    -1,   183,   128,   267,
     128,    94,   167,   182,    -1,    94,   170,   182,    -1,   181,
      -1,   170,   128,   183,   182,    -1,   170,   128,   181,    -1,
     170,   128,    94,   170,   182,    -1,   170,   128,   183,   128,
      94,   170,   182,    -1,   267,   182,    -1,   267,   128,    94,
     170,   182,    -1,   170,   128,   267,   182,    -1,   170,   128,
     183,   128,   267,   182,    -1,   170,   128,   267,   128,    94,
     170,   182,    -1,   170,   128,   183,   128,   267,   128,    94,
     170,   182,    -1,    94,   170,   182,    -1,   181,    -1,    -1,
     177,   178,    -1,   174,    -1,    -1,    89,   179,   127,    -1,
      -1,    89,   175,   180,   127,    -1,    95,   170,    -1,   128,
     181,    -1,   280,    -1,   170,    -1,   183,   128,   170,    -1,
     183,   128,   170,    -1,   183,   128,    94,   170,    -1,    94,
     170,    -1,   226,    -1,   227,    -1,   230,    -1,   231,    -1,
     232,    -1,   235,    -1,   248,    -1,   250,    -1,    51,    -1,
      -1,     7,   186,   138,    10,    -1,    -1,    89,   144,   187,
     276,   127,    -1,    88,   139,   127,    -1,   202,    84,    54,
      -1,    85,    54,    -1,   202,   124,   171,   125,    -1,    91,
     171,   125,    -1,    92,   266,   123,    -1,    29,    -1,    30,
     130,   174,   127,    -1,    30,   130,   127,    -1,    30,    -1,
      -1,    45,   276,   130,   188,   144,   127,    -1,   271,   214,
      -1,   213,    -1,   213,   214,    -1,    11,   145,   203,   139,
     205,    10,    -1,    12,   145,   203,   139,   206,    10,    -1,
      -1,    -1,    18,   189,   145,   204,   190,   139,    10,    -1,
      -1,    -1,    19,   191,   145,   204,   192,   139,    10,    -1,
      16,   145,   275,   219,    10,    -1,    16,   275,   219,    10,
      -1,    16,   275,    15,   139,    10,    -1,    -1,    -1,    20,
     207,    25,   193,   145,   204,   194,   139,    10,    -1,    -1,
       3,   160,   251,   195,   138,    10,    -1,    -1,    -1,     3,
      82,   144,   196,   278,   197,   138,    10,    -1,    -1,     4,
     160,   198,   138,    10,    -1,    -1,     5,   161,   199,   253,
     138,    10,    -1,    -1,    -1,     5,   264,   274,   200,   161,
     201,   253,   138,    10,    -1,    21,    -1,    22,    -1,    23,
      -1,    24,    -1,   185,    -1,   278,    -1,   107,    -1,    13,
      -1,   278,    13,    -1,   278,    -1,   107,    -1,    27,    -1,
     206,    -1,    14,   145,   203,   139,   205,    -1,   280,    -1,
      15,   139,    -1,   158,    -1,   152,    -1,   280,    -1,   110,
     110,    -1,    75,    -1,   110,   207,   110,    -1,    -1,    -1,
      28,   210,   208,   211,   139,    10,    -1,   151,   209,    -1,
     212,   126,   272,   173,    -1,   212,    84,   272,   173,    -1,
     271,   172,    -1,   202,   126,   272,   173,    -1,   202,    84,
     272,   172,    -1,   202,    84,   273,    -1,   202,   131,   272,
      -1,    66,   272,    -1,    31,   172,    -1,    31,    -1,    -1,
      -1,   122,   215,   208,   216,   139,   123,    -1,    -1,    -1,
      26,   217,   208,   218,   139,    10,    -1,    17,   220,   203,
     139,   221,    -1,   183,    -1,   183,   128,    94,   170,    -1,
      94,   170,    -1,   206,    -1,   219,    -1,     8,   223,   224,
     203,   139,   222,    -1,   280,    -1,   170,    -1,   184,    -1,
     280,    -1,    87,   158,    -1,   280,    -1,     9,   139,    -1,
     280,    -1,   246,    -1,   243,    -1,   245,    -1,   228,    -1,
     229,    -1,   228,   229,    -1,    97,   237,   103,    -1,    56,
     238,   103,    -1,    98,   238,    62,    -1,    99,   132,   103,
      -1,    99,   233,   103,    -1,    -1,   233,   234,   132,    -1,
     239,    -1,   234,   239,    -1,   100,   132,   103,    -1,   100,
     236,   103,    -1,    -1,   236,    59,   132,    -1,    -1,   237,
     239,    -1,    -1,   238,   239,    -1,    59,    -1,    -1,   102,
     240,   242,    -1,    -1,   101,   241,   139,   123,    -1,    52,
      -1,    53,    -1,    55,    -1,   250,    -1,    96,   244,    -1,
     161,    -1,    53,    -1,    52,    -1,    55,    -1,    96,   238,
     103,    -1,    57,    -1,    58,    -1,   118,    57,    -1,   118,
      58,    -1,    50,    -1,    53,    -1,    52,    -1,    54,    -1,
      55,    -1,    33,    -1,    32,    -1,    34,    -1,    35,    -1,
      49,    -1,    48,    -1,   247,    -1,   247,    -1,    60,    -1,
      61,    -1,   278,    -1,    -1,   109,   252,   145,   278,    -1,
       1,   278,    -1,   130,   254,   276,   127,    -1,   254,   278,
      -1,   256,   128,   258,   128,   260,   263,    -1,   256,   128,
     258,   263,    -1,   256,   128,   260,   263,    -1,   256,   263,
      -1,   258,   128,   260,   263,    -1,   258,   263,    -1,   260,
     263,    -1,   262,    -1,    -1,    54,    -1,    53,    -1,    52,
      -1,    55,    -1,    50,    -1,   255,    -1,   256,   128,   255,
      -1,    50,   105,   170,    -1,   257,    -1,   258,   128,   257,
      -1,   115,    -1,    94,    -1,   259,    50,    -1,   259,    -1,
     112,    -1,    95,    -1,   261,    50,    -1,   128,   262,    -1,
     280,    -1,   248,    -1,    -1,   130,   265,   144,   276,   127,
      -1,   280,    -1,   267,   277,    -1,   183,   277,    -1,   268,
      -1,   267,   128,   268,    -1,   269,    -1,   270,    -1,   170,
      87,   170,    -1,    63,   170,    -1,    50,    -1,    54,    -1,
      51,    -1,    50,    -1,    54,    -1,    51,    -1,   165,    -1,
      50,    -1,    51,    -1,   165,    -1,   126,    -1,    84,    -1,
      -1,   279,    -1,    -1,   133,    -1,    -1,   133,    -1,   128,
      -1,   134,    -1,   133,    -1,   278,    -1,   279,   134,    -1,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
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
    1029,  1030,  1031,  1032,  1035,  1035,  1035,  1035,  1036,  1036,
    1036,  1036,  1036,  1036,  1036,  1037,  1037,  1037,  1037,  1037,
    1037,  1037,  1038,  1038,  1038,  1038,  1038,  1038,  1039,  1039,
    1039,  1039,  1039,  1039,  1039,  1040,  1040,  1040,  1040,  1040,
    1041,  1041,  1041,  1041,  1041,  1044,  1048,  1052,  1077,  1096,
    1108,  1120,  1132,  1137,  1142,  1147,  1153,  1159,  1163,  1167,
    1171,  1175,  1179,  1183,  1187,  1191,  1200,  1204,  1208,  1212,
    1216,  1220,  1224,  1228,  1232,  1236,  1240,  1244,  1248,  1252,
    1256,  1260,  1264,  1268,  1272,  1276,  1280,  1280,  1285,  1285,
    1291,  1297,  1304,  1305,  1310,  1314,  1319,  1323,  1330,  1334,
    1338,  1343,  1350,  1351,  1354,  1359,  1363,  1368,  1373,  1378,
    1383,  1389,  1393,  1396,  1400,  1404,  1409,  1414,  1419,  1424,
    1429,  1434,  1439,  1444,  1448,  1451,  1451,  1463,  1464,  1464,
    1469,  1469,  1476,  1482,  1486,  1489,  1493,  1499,  1503,  1507,
    1513,  1514,  1515,  1516,  1517,  1518,  1519,  1520,  1521,  1526,
    1525,  1538,  1538,  1543,  1547,  1551,  1555,  1564,  1573,  1577,
    1581,  1585,  1589,  1593,  1593,  1598,  1604,  1605,  1614,  1627,
    1640,  1640,  1640,  1650,  1650,  1650,  1660,  1667,  1671,  1675,
    1675,  1675,  1683,  1682,  1699,  1704,  1698,  1721,  1720,  1737,
    1736,  1754,  1755,  1754,  1769,  1773,  1777,  1781,  1787,  1794,
    1795,  1796,  1797,  1800,  1801,  1802,  1805,  1806,  1815,  1816,
    1822,  1823,  1826,  1827,  1831,  1835,  1842,  1847,  1841,  1858,
    1867,  1871,  1877,  1882,  1887,  1892,  1896,  1900,  1904,  1908,
    1915,  1919,  1914,  1926,  1930,  1925,  1938,  1945,  1946,  1950,
    1956,  1957,  1960,  1971,  1974,  1978,  1979,  1982,  1986,  1989,
    1997,  2000,  2001,  2005,  2008,  2021,  2022,  2028,  2034,  2063,
    2098,  2102,  2108,  2112,  2118,  2119,  2125,  2129,  2135,  2139,
    2145,  2149,  2155,  2159,  2165,  2167,  2166,  2178,  2177,  2198,
    2199,  2200,  2201,  2204,  2211,  2212,  2213,  2214,  2217,  2243,
    2244,  2245,  2249,  2255,  2256,  2257,  2258,  2259,  2260,  2261,
    2262,  2263,  2264,  2265,  2268,  2274,  2280,  2281,  2284,  2289,
    2288,  2296,  2299,  2304,  2310,  2315,  2319,  2324,  2328,  2333,
    2337,  2342,  2346,  2352,  2356,  2360,  2364,  2368,  2379,  2380,
    2386,  2396,  2401,  2407,  2408,  2411,  2419,  2426,  2427,  2430,
    2440,  2444,  2447,  2457,  2457,  2482,  2483,  2487,  2496,  2497,
    2503,  2505,  2509,  2515,  2521,  2522,  2523,  2526,  2527,  2528,
    2529,  2532,  2533,  2534,  2537,  2538,  2541,  2542,  2545,  2546,
    2549,  2550,  2551,  2554,  2555,  2558,  2559,  2562
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
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
  "tGEQ", "tLEQ", "tANDOP", "tOROP", "tMATCH", "tNMATCH", "tDOT2", 
  "tDOT3", "tAREF", "tASET", "tLSHFT", "tRSHFT", "tCOLON2", "tCOLON3", 
  "tOP_ASGN", "tASSOC", "tLPAREN", "tLPAREN_ARG", "tRPAREN", "tLBRACK", 
  "tLBRACE", "tLBRACE_ARG", "tSTAR", "tAMPER", "tSYMBEG", "tSTRING_BEG", 
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
static const unsigned short yytoknum[] =
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
static const unsigned short yyr1[] =
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
     165,   165,   165,   165,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   168,   167,   169,   167,
     167,   170,   171,   171,   171,   171,   171,   171,   172,   172,
     172,   172,   173,   173,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   177,   176,   178,   179,   178,
     180,   178,   181,   182,   182,   183,   183,   184,   184,   184,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   186,
     185,   187,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   188,   185,   185,   185,   185,   185,   185,
     189,   190,   185,   191,   192,   185,   185,   185,   185,   193,
     194,   185,   195,   185,   196,   197,   185,   198,   185,   199,
     185,   200,   201,   185,   185,   185,   185,   185,   202,   203,
     203,   203,   203,   204,   204,   204,   205,   205,   206,   206,
     207,   207,   208,   208,   208,   208,   210,   211,   209,   212,
     212,   212,   213,   213,   213,   213,   213,   213,   213,   213,
     215,   216,   214,   217,   218,   214,   219,   220,   220,   220,
     221,   221,   222,   222,   223,   223,   223,   224,   224,   225,
     225,   226,   226,   226,   227,   228,   228,   229,   230,   231,
     232,   232,   233,   233,   234,   234,   235,   235,   236,   236,
     237,   237,   238,   238,   239,   240,   239,   241,   239,   242,
     242,   242,   242,   243,   244,   244,   244,   244,   245,   246,
     246,   246,   246,   247,   247,   247,   247,   247,   247,   247,
     247,   247,   247,   247,   248,   249,   250,   250,   251,   252,
     251,   251,   253,   253,   254,   254,   254,   254,   254,   254,
     254,   254,   254,   255,   255,   255,   255,   255,   256,   256,
     257,   258,   258,   259,   259,   260,   260,   261,   261,   262,
     263,   263,   264,   265,   264,   266,   266,   266,   267,   267,
     268,   268,   269,   270,   271,   271,   271,   272,   272,   272,
     272,   273,   273,   273,   274,   274,   275,   275,   276,   276,
     277,   277,   277,   278,   278,   279,   279,   280
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
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
       1,     1,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     1,     0,     1,
       0,     1,     1,     1,     1,     1,     2,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
       2,     0,     0,     1,     0,     0,     0,     0,     0,   279,
       0,     0,   496,   300,   303,     0,   324,   325,   326,   327,
     289,   292,   359,   429,   428,   430,   431,     0,     0,   498,
      21,     0,   433,   432,   423,   486,   425,   424,   426,   427,
     402,   419,   420,   436,   437,     0,     0,     0,     0,     0,
       0,   507,   507,    71,   402,   400,   402,   392,   398,     0,
       0,     0,     3,   496,     7,    35,    36,    44,    43,     0,
      62,     0,    66,    72,     0,    41,   220,     0,    48,   296,
     270,   271,   384,   385,   272,   273,   274,   275,   382,   383,
     381,   434,   276,     0,   277,   255,     6,     9,   324,   325,
     289,   292,   359,   498,    92,    93,     0,     0,     0,     0,
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
      42,     0,     0,     0,   434,     0,   277,     0,   504,   503,
     496,     0,   505,   497,     0,     0,     0,   341,   340,     0,
       0,   434,   277,     0,     0,     0,     0,   234,   221,   265,
      46,   242,   507,   507,   478,   480,   481,    47,    45,     0,
      61,     0,   507,   358,    60,    39,     0,    10,   499,   216,
       0,     0,     0,   195,     0,   196,   487,   489,   488,   490,
     357,   285,     0,     0,     0,    62,   281,     0,   498,     0,
     500,   500,   222,   500,     0,   500,   475,     0,    70,     0,
      76,    83,   416,   415,   417,   414,     0,   413,     0,     0,
       0,     0,     0,     0,   421,   422,    40,   210,   211,     5,
     497,     0,     0,     0,     0,     0,     0,     0,   346,   349,
       0,    74,     0,    69,    67,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   218,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   507,     0,     0,     0,     0,   363,   360,
     297,   386,     0,     0,   352,    54,   295,     0,   314,    92,
      93,    94,   421,   422,     0,   439,   312,   438,     0,   507,
       0,     0,     0,   452,   495,   494,   321,   105,     0,   507,
     285,   331,   330,     0,   329,     0,     0,   507,     0,     0,
       0,     0,     0,     0,     0,     0,   506,     0,     0,   285,
       0,   507,     0,   309,   483,   507,   262,     0,     0,   235,
     264,     0,   237,   291,     0,   258,   257,   256,   234,   498,
     507,   498,     0,    12,    14,    13,     0,   293,     0,     0,
       0,   404,   407,   405,   388,   403,     0,     0,     0,     0,
     283,    63,   498,   498,   223,   287,   502,   501,   224,   502,
     226,   502,   477,   288,   476,    82,     0,   507,     0,   418,
     387,   401,   389,   390,   391,     0,   394,   396,     0,   397,
       0,     0,     8,    16,    17,    18,    19,    20,    37,    38,
     507,     0,    25,    33,     0,    34,     0,    68,    75,    24,
     175,   265,    32,   192,   200,   205,   206,   207,   202,   204,
     214,   215,   208,   209,   185,   186,   212,   213,     0,   201,
     203,   197,   198,   199,   187,   188,   189,   190,   191,   487,
     492,   488,   493,   255,   355,     0,   487,   488,   507,   356,
     255,   255,   507,   507,    26,   177,    31,   184,    51,    55,
       0,   441,     0,     0,    92,    93,    96,     0,     0,   507,
       0,   498,   457,   455,   454,   453,   456,   464,   468,   467,
     463,   452,     0,     0,   458,   507,   461,   507,   466,   507,
       0,   451,     0,     0,   280,   507,   507,   373,   507,   332,
     175,   491,   284,     0,   487,   488,   507,     0,     0,     0,
     367,     0,   307,   335,   334,   301,   333,   304,   491,   284,
       0,   487,   488,     0,     0,   241,   482,     0,   266,   263,
     507,     0,     0,   479,   290,     0,    41,     0,   260,     0,
     254,   507,     0,     0,     0,     0,     0,   228,    11,     0,
     217,     0,    23,     0,     0,   183,    63,     0,   227,     0,
     266,   491,    81,     0,   487,   488,   393,   395,   399,   193,
     194,   344,     0,   347,   342,   269,     0,    73,     0,     0,
       0,     0,   354,    58,   286,     0,     0,   233,   353,    56,
     232,   351,    50,   350,    49,   364,   361,   507,   315,     0,
       0,   286,   318,     0,     0,   498,     0,   443,     0,   447,
     471,     0,   449,   465,     0,   450,   469,   322,   106,   374,
     375,   507,   376,     0,   507,   338,     0,     0,   336,     0,
     286,     0,     0,     0,   306,   308,   369,     0,     0,     0,
       0,   286,     0,   507,     0,   239,   507,   507,     0,     0,
     259,     0,   247,   229,     0,   498,   507,   507,   230,     0,
      22,     0,   409,   410,   411,   406,   412,   282,   498,    77,
     343,     0,     0,     0,   267,   176,     0,    30,   181,   182,
      59,     0,    28,   179,    29,   180,    57,     0,     0,    52,
       0,   440,   313,   474,   460,     0,   320,   459,   507,   507,
     470,     0,   462,   507,   452,     0,     0,   378,   339,     0,
       4,   380,     0,   298,     0,   299,     0,   507,     0,     0,
     310,   236,     0,   238,   253,     0,   244,   507,   507,   261,
       0,     0,   294,   408,   225,   345,     0,   268,   219,    27,
     178,     0,     0,     0,     0,   442,     0,   445,   446,   448,
       0,     0,   377,     0,    84,    91,     0,   379,     0,   368,
     370,   371,   366,   302,   305,     0,   507,   507,     0,   243,
       0,   249,   507,   231,   348,   365,   362,     0,   316,   507,
       0,    90,     0,   507,     0,   507,   507,     0,   240,   245,
       0,   507,     0,   248,    53,   444,   323,   491,    89,     0,
     487,   488,   372,   337,   311,   507,     0,   250,   507,    85,
     246,     0,   251,   507,   252
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     2,   388,   389,    63,    64,   436,   260,    65,
     211,    66,    67,   549,   687,   833,    68,    69,   274,    70,
      71,    72,    73,   212,   110,   111,   204,   205,   206,   583,
     269,   192,    75,   438,   518,   239,   279,   677,   678,   240,
     628,   250,   251,   427,   629,   739,   619,   419,   280,   495,
      76,   208,   452,   639,   224,   729,   225,   730,   613,   855,
     553,   550,   780,   381,   383,   582,   794,   264,   393,   605,
     717,   718,   230,   663,   319,   490,   762,    78,    79,   366,
     543,   778,   542,   777,   405,   601,   852,   586,   711,   796,
     800,    80,    81,    82,    83,    84,    85,    86,   301,   475,
      87,   303,   298,   262,   445,   644,   643,   755,    88,   297,
      89,    90,   214,    92,   215,   216,   376,   552,   572,   573,
     574,   575,   576,   577,   578,   579,   580,   790,   699,   194,
     382,   284,   281,   244,   245,   246,   116,   557,   534,   386,
     221,   259,   458,   394,   223,    96
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -662
static const short yypact[] =
{
    -662,    89,  2212,  -662,  5615,  8236,  8539,  4059,  5268,  -662,
    7085,  7085,  3937,  -662,  -662,  8337,  5825,  5825,  -662,  -662,
    5825,  4599,  4704,  -662,  -662,  -662,  -662,  7085,  5150,   -41,
    -662,    10,  -662,  -662,  4179,  2327,  -662,  -662,  4284,  -662,
    -662,  -662,  -662,  -662,  -662,  7925,  7925,  8793,    52,  3324,
    7085,  6980,  7190,  8640,  5032,  -662,  -662,   -17,    58,   151,
    8030,  7925,  -662,   236,   892,   337,  -662,  -662,    97,    51,
    -662,    37,  8438,  -662,   110,  9683,   280,   420,    96,    47,
    -662,  -662,   142,  -662,  -662,  -662,  -662,  -662,  -662,  -662,
    -662,   326,  -662,    66,   339,    16,  -662,   892,  -662,  -662,
    -662,   101,   117,   -41,    46,   366,  7085,    99,  3449,   336,
    -662,    48,  -662,   513,  -662,  -662,    16,  -662,  -662,  -662,
    -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,
    -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,
    -662,   120,   129,   158,   216,  -662,  -662,  -662,  -662,  -662,
    -662,  -662,  -662,  -662,  -662,  -662,  -662,   274,   334,   365,
    -662,   403,  -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,
    -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,
    -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,
    -662,  -662,  -662,  -662,   453,  -662,  -662,  -662,  -662,  -662,
    -662,  -662,  -662,  5032,  -662,  -662,    50,  -662,  2719,   215,
     337,    92,   180,   583,     9,   220,    40,    92,  -662,  -662,
     236,   281,  -662,   174,  7085,  7085,   303,  -662,  -662,   595,
     335,    45,    93,  7925,  7925,  7925,  7925,  -662,  9683,   279,
    -662,  -662,   234,   254,  -662,  -662,  -662,  -662,  -662,  5505,
    -662,  5930,  5825,  -662,  -662,  -662,    98,  -662,  -662,   255,
     295,  3574,    32,  -662,   612,   356,  -662,  -662,  -662,  -662,
    -662,   367,  3324,   321,   328,   331,   337,  7925,   -41,   357,
     -12,   134,  -662,   164,   360,   134,  -662,   408,  -662,   658,
     667,   717,  -662,  -662,  -662,  -662,   517,  -662,   555,   306,
     381,   562,   386,    39,   427,   431,  -662,  -662,  -662,  -662,
    3818,  7085,  7085,  7085,  7085,  5615,  7085,  7085,  -662,  -662,
    7295,  -662,  3324,  8640,   375,  7295,  7925,  7925,  7925,  7925,
    7925,  7925,  7925,  7925,  7925,  7925,  7925,  7925,  7925,  7925,
    7925,  -662,  7925,  7925,  7925,  7925,  7925,  7925,  7925,  7925,
    7925,  7925,  8861,  6980,  8929,  8793,  8793,  8793,  -662,  -662,
    -662,  -662,  8030,  8030,  -662,   412,  -662,   255,   337,  -662,
     728,  -662,  -662,  -662,   236,  -662,  -662,  -662,  8997,  6980,
    8793,  2719,  7085,   574,  -662,  -662,  -662,  -662,   502,   515,
     185,  -662,  -662,  2844,   514,  7925,  9065,  6980,  9133,  7925,
    7925,  3084,   518,  3699,  7400,   520,  -662,    33,    33,   106,
    9201,  6980,  9269,  -662,  -662,   413,  -662,  7925,  6035,  -662,
    -662,  6140,  -662,  -662,   416,  5720,  -662,  -662,    97,   -41,
     429,    -6,   433,  -662,  -662,  -662,  5268,  -662,  7925,  3574,
     438,  -662,  -662,  -662,  -662,  -662,  9065,  9133,  7925,   436,
    -662,   439,   -41,  1489,  -662,  -662,  7505,  -662,  -662,  7190,
    -662,  7925,  -662,  -662,  -662,   728,  9337,  6980,  9405,  -662,
    -662,  -662,  -662,  -662,  -662,    22,  -662,  -662,   442,  -662,
    7925,  7925,   892,  -662,  -662,  -662,  -662,  -662,  -662,  -662,
      38,  7925,  -662,   440,   450,  -662,   443,  -662,  -662,  -662,
    2067,  -662,  -662,   356,  9751,  9751,  9751,  9751,  1016,  1016,
    9802,  9734,  9751,  9751,  1611,  1611,   693,   693,  7925,  1016,
    1016,  1041,  1041,   756,   143,   143,   356,   356,   356,  2456,
    4809,  2585,  4914,   117,  -662,   456,   371,   454,  4704,  -662,
    1988,  1988,    38,    38,  -662,  9683,  -662,  9683,  -662,  -662,
     236,  -662,  7085,  2719,   512,   469,  -662,   117,   462,   117,
     580,    74,   493,  -662,  -662,  -662,  -662,  -662,  -662,  -662,
    -662,    49,  2719,   236,  -662,   476,  -662,   478,   557,   480,
     560,  -662,  5386,  5268,  -662,  7610,   597,  -662,   382,  -662,
    2435,  4389,  4494,   497,   197,   262,   597,   621,   622,  7925,
     519,    92,  -662,  -662,  -662,  -662,  -662,  -662,    84,   138,
     516,   135,   146,  7085,   545,  -662,  -662,  7925,   279,  -662,
     533,  7925,   279,  -662,  -662,  7925,  2564,    13,  -662,   521,
    -662,   534,   539,  6245,  8793,  8793,   543,  -662,  -662,  7085,
    9683,   522,  -662,  3574,   726,  9683,   301,   544,  -662,  7925,
    -662,   122,   469,   547,    69,   314,  -662,  -662,  -662,   356,
     356,  -662,  8135,  -662,  -662,  -662,  7715,  -662,  7925,  9666,
    8030,  7925,  -662,   412,   461,  8030,  8030,  -662,  -662,   412,
    -662,  -662,  -662,  -662,  -662,  -662,  -662,    38,  -662,   236,
     663,  -662,  -662,   549,  7925,   -41,   671,  -662,    49,  -662,
    -662,   424,  -662,  -662,    17,  -662,  -662,  -662,  -662,   440,
    -662,   598,  -662,  3204,   675,  -662,  7085,   677,  -662,  7925,
     275,  7925,  7925,   685,  -662,  -662,  -662,  7820,  2964,  3699,
    3699,   167,    33,   413,  6350,  -662,   413,   413,  6455,   570,
    -662,  6560,  -662,  -662,    97,    -6,   117,   117,  -662,    60,
    -662,   575,  -662,  -662,  -662,  -662,  -662,  -662,  1489,   779,
    -662,   592,  3699,  7925,   582,  9683,  7925,  -662,  9683,  9683,
    -662,  8030,  -662,  9683,  -662,  9683,  -662,  3699,  3574,  -662,
    2719,  -662,  -662,  -662,  -662,   581,  -662,  -662,   584,   480,
    -662,   493,  -662,   480,   574,  8741,    92,  -662,  -662,  3699,
    -662,  -662,    92,  -662,  7925,  -662,  7925,   286,   710,   713,
    -662,  -662,  7925,  -662,  -662,  7925,  -662,   596,   599,  -662,
    7925,   601,  -662,  -662,  -662,  -662,   721,  -662,  9683,  -662,
    9683,   722,   610,  3574,   724,  -662,   424,  -662,  -662,  -662,
    2719,   683,  -662,   798,   667,   717,  2719,  -662,  2844,  -662,
    -662,  -662,  -662,  -662,  -662,  3699,  9604,   413,  6665,  -662,
    6770,  -662,   413,  -662,  -662,  -662,  -662,   618,  -662,   480,
     735,   728,  9473,  6980,  9541,   515,   382,   736,  -662,  -662,
    7925,   620,  7925,  -662,  -662,  -662,  -662,    55,   469,   624,
      95,   104,  -662,  -662,  -662,   413,  6875,  -662,   413,   779,
    -662,  7925,  -662,   413,  -662
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -662,  -662,  -662,  -367,   149,  -662,    28,  -662,  -662,   458,
      64,    24,  -662,  -618,  -662,  -662,    -8,   -13,  -185,    14,
     680,  -662,   -36,   191,   -87,   747,    -2,   -21,  -662,  -662,
      20,  -662,   -16,  -662,  -662,  1433,  -328,    -9,  -482,   102,
    -662,    -7,  -662,  -662,  -662,  -662,    21,   474,    30,  -291,
      18,  -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,  -662,
    -662,  -662,  -662,  -662,  -662,  -662,  -662,   505,  -211,  -392,
    -115,  -529,   103,  -463,  -662,  -662,  -662,  -242,  -662,   688,
    -662,  -662,  -662,  -662,  -381,  -662,  -662,  -111,  -662,  -662,
    -662,  -662,  -662,  -662,   691,  -662,  -662,  -662,  -662,  -662,
    -662,  -662,  -662,   251,  -245,  -662,  -662,  -662,     3,  -662,
    -662,  -662,   686,   769,  1193,  1098,  -662,  -662,     6,   214,
     105,  -662,  -661,   113,  -662,  -636,  -662,  -380,  -513,  -662,
    -662,  -662,     2,  -382,  -662,  -662,  1001,    81,  -662,  -662,
     -37,   222,   290,    73,   734,  1228
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -508
static const short yytable[] =
{
     238,   238,   227,   581,   238,   190,   401,   257,   237,   237,
     431,   207,   237,   253,   560,   254,   607,   288,   243,   243,
     371,   597,   243,   112,   112,   535,   309,   191,   191,   263,
     265,   207,    97,   112,   502,   238,   238,   241,   241,   623,
     792,   241,   358,   278,   307,   308,   242,   242,   191,   374,
     242,   558,   295,   471,   285,   770,   476,   714,   681,   683,
     603,   776,   789,   275,   702,   793,   705,   723,   -87,   593,
     -84,   112,  -484,   358,   191,   217,   220,   623,   634,   685,
     686,   441,   283,   610,   306,   222,   364,   449,   365,     3,
     112,   441,   258,   253,   -78,  -435,   316,   317,   478,   562,
     417,   563,   564,   565,   566,   391,   271,   364,   -86,   -87,
     316,   317,   568,   661,   -84,   300,   456,   -88,   -91,   247,
     635,   457,   248,   442,   443,   318,   400,   258,   270,   569,
    -423,   -90,   261,   442,   443,   444,   222,   496,   359,   653,
     604,   738,   479,   567,   568,   -91,   252,   -79,   662,   369,
     433,    62,   362,   370,   656,   -84,   320,   375,   434,   435,
     -86,   569,   -87,   -89,   570,   321,   218,   219,  -484,   359,
    -423,   -88,  -423,   -76,   -78,   792,  -484,  -423,   387,   -78,
     356,   218,   219,   402,   377,  -487,   690,   822,   -87,   -87,
     302,   581,   -85,    74,   -87,    74,   -78,   -78,   273,   392,
     869,   295,   -86,   -91,  -429,   696,   228,   258,   304,   305,
     326,   -88,   -79,  -428,  -487,   325,   -90,   238,   238,   238,
     307,   -83,   357,   191,   779,   218,   219,   -79,   -86,   -86,
     657,   249,   -79,   238,   -82,   238,   238,   -88,   -88,    55,
      74,   237,  -430,   237,   428,   -86,  -429,   252,   -89,   -79,
     -79,   243,  -487,   243,   243,  -428,   -88,   273,   349,   350,
     351,   453,   459,   -78,   681,   683,   -81,   457,  -488,   390,
     241,   448,   241,   241,   -80,   837,   838,   -85,   850,   242,
     839,   242,   430,   721,  -430,   395,   275,   497,   407,   408,
     -90,   556,   461,   222,   710,   -77,   403,   457,   404,    74,
    -431,   713,   -86,   404,   238,   296,   399,   299,   406,   500,
     503,   504,   505,   506,   507,   508,   509,   510,   511,   512,
     513,   514,   515,   516,   517,   367,   519,   520,   521,   522,
     523,   524,   525,   526,   527,   528,   275,   238,   482,   -80,
     810,   112,  -431,   487,   492,   278,   545,   547,   722,   499,
     494,   424,   623,   426,   429,   494,   885,   409,  -433,   623,
     413,   804,   418,   238,  -328,   441,   417,   -88,   472,   218,
     219,   278,   532,   316,   317,   483,   484,   485,   486,   590,
     -85,   238,   421,   545,   547,   437,   544,   546,   238,   278,
     728,   745,  -485,   372,   373,   238,   716,   713,   532,    74,
    -433,   238,   238,   278,  -328,   238,  -328,   442,   443,   626,
     440,  -328,  -435,   834,   581,   638,   532,   439,  -432,   -80,
     620,   273,   640,   326,   -80,   363,   851,   631,   -65,   -73,
     532,   -84,   645,   533,   600,   538,   539,   540,   541,   207,
     238,   -80,   -80,   238,   -91,   238,   630,   551,   450,  -423,
    -426,   238,    74,   448,   -76,   451,   191,   675,   -64,   278,
    -432,   559,   465,    74,   659,   660,   532,   -83,   210,   210,
     210,   273,   -90,   870,   791,   238,   -86,   533,   623,   538,
     606,   606,   455,   463,   473,   255,   532,  -426,  -485,   477,
    -426,  -423,  -426,   559,   480,   -82,  -485,  -426,   481,   -78,
     454,    74,   669,   498,   352,   548,    74,    77,   276,    77,
     113,   113,   584,    74,   623,   213,   213,   213,   567,   568,
     229,   213,   213,   585,   672,   213,   673,   589,   559,  -426,
     602,   679,   213,   682,   684,   404,   569,   384,  -491,   570,
     676,   614,   588,   624,   353,   889,   354,   771,   672,   559,
     596,   355,   598,  -284,    77,   213,   213,   633,   289,   -88,
     637,   642,   708,   646,   368,   213,   -85,   -73,  -265,   238,
     667,   460,    74,   462,   658,   464,   441,   289,   666,   385,
     707,   674,   -80,   238,    74,   846,   207,   691,   641,   -77,
     692,   848,    74,  -284,    74,  -284,  -491,   378,   694,  -488,
    -284,   238,   191,   191,   698,   238,   701,   703,   704,   238,
     706,   213,   713,    77,   441,   494,   689,   238,   442,   443,
     469,   441,   720,   688,   562,   744,   563,   564,   565,   566,
      74,   724,   725,   758,  -491,   620,  -491,   379,  -491,   380,
     235,   731,  -487,  -491,   355,   750,   697,   727,   740,   227,
     238,   632,   765,   636,   768,   769,   442,   443,   470,   773,
     775,   734,   741,   442,   443,   474,   743,   396,   567,   568,
     748,   757,   759,   782,   647,   648,   783,   732,   238,   410,
     112,   786,   210,   210,   799,   795,   569,   803,    91,   570,
      91,   114,   114,   114,   767,   805,   446,   819,   823,   772,
     774,   231,   825,   768,   571,   773,   775,   397,   835,   398,
    -266,   238,   836,    77,   355,   746,   747,   422,   238,   411,
     853,   412,   238,   854,   858,   238,   355,   860,   863,   213,
     213,   864,   865,   866,   868,    91,   397,   871,   447,   290,
     818,   884,   466,   355,    74,   886,   894,   238,   896,   899,
     828,  -434,   324,   117,   213,   830,   213,   213,   290,   816,
     326,   893,   781,    74,   892,   761,    77,   360,   817,   210,
     210,   210,   210,   361,   488,   489,   193,    77,   752,   753,
     802,   754,   467,   693,   468,   695,    43,    44,   830,   355,
     238,  -434,   751,  -434,    91,   829,   856,   310,  -434,   238,
     840,  -277,     0,   787,   238,   606,   347,   348,   349,   350,
     351,   788,  -285,   112,     0,    77,   213,   213,   213,   213,
      77,   213,   213,   326,     0,   213,     0,    77,   289,     0,
     213,     0,     0,     0,    74,     0,     0,     0,   339,   340,
     561,  -277,   238,  -277,   238,     0,     0,     0,  -277,     0,
       0,     0,  -285,   228,  -285,     0,     0,   238,   213,  -285,
     881,     0,   798,  -286,   238,   278,   238,   213,   213,   347,
     348,   349,   350,   351,     0,     0,     0,   807,   808,   809,
     238,     0,   872,   276,   213,   238,    77,   213,     0,   615,
       0,     0,   532,     0,    91,     0,     0,     0,    77,     0,
       0,     0,   213,  -286,    74,  -286,    77,     0,    77,     0,
    -286,   826,     0,     0,     0,     0,   213,   785,     0,    74,
      74,    74,   873,     0,   874,     0,   831,   832,     0,   355,
     213,   311,   312,   313,   314,   315,     0,     0,     0,     0,
       0,     0,     0,     0,    77,     0,     0,    91,   847,     0,
       0,     0,     0,    74,     0,   559,     0,     0,    91,     0,
       0,     0,     0,     0,     0,     0,     0,   821,    74,    74,
       0,    74,   213,     0,     0,     0,     0,     0,     0,     0,
     824,     0,   867,     0,     0,     0,   842,     0,     0,     0,
      74,     0,     0,     0,     0,   875,    91,   876,     0,     0,
       0,    91,     0,    95,   877,    95,     0,     0,    91,   290,
     210,    95,    95,    95,     0,     0,     0,    95,    95,     0,
       0,    95,     0,     0,    74,     0,     0,     0,    95,     0,
       0,    74,     0,     0,     0,     0,     0,    74,     0,    74,
       0,     0,     0,     0,     0,     0,    74,     0,     0,     0,
      95,    95,    95,     0,     0,     0,     0,   213,    77,     0,
       0,    95,     0,     0,     0,     0,     0,    91,     0,     0,
       0,   210,     0,     0,     0,     0,     0,    77,     0,    91,
       0,     0,     0,   326,     0,     0,     0,    91,     0,    91,
       0,     0,     0,     0,   735,     0,     0,   749,   339,   340,
      94,     0,    94,   115,   115,   742,     0,    95,   326,    95,
       0,     0,     0,   232,     0,     0,     0,     0,   213,     0,
       0,     0,     0,   339,   340,    91,   344,   345,   346,   347,
     348,   349,   350,   351,     0,     0,     0,     0,   213,     0,
       0,     0,     0,     0,   213,     0,     0,    94,    77,     0,
       0,   291,     0,   346,   347,   348,   349,   350,   351,     0,
       0,     0,     0,     0,     0,     0,     0,   229,     0,     0,
     291,     0,     0,     0,   210,   213,     0,     0,     0,     0,
     213,   213,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    93,     0,    93,     0,     0,
       0,     0,     0,     0,     0,     0,    94,   811,     0,    95,
     813,   814,     0,     0,     0,     0,     0,     0,    77,     0,
       0,   213,     0,     0,     0,    95,    95,     0,     0,     0,
       0,     0,     0,    77,    77,    77,     0,     0,     0,    91,
       0,     0,    93,     0,     0,     0,     0,     0,     0,     0,
      95,     0,    95,    95,     0,     0,     0,     0,    91,     0,
       0,     0,    95,     0,     0,     0,     0,    77,     0,     0,
       0,     0,     0,    95,     0,     0,   213,     0,     0,   282,
     286,     0,    77,    77,     0,    77,     0,     0,     0,     0,
       0,   859,   861,     0,     0,     0,     0,     0,     0,     0,
     843,    93,     0,     0,    77,     0,    94,     0,     0,     0,
       0,    95,    95,    95,    95,    95,    95,    95,    95,     0,
       0,    95,     0,    95,     0,     0,    95,     0,     0,    91,
     878,   879,     0,     0,     0,     0,   883,     0,    77,     0,
       0,     0,     0,     0,     0,    77,     0,     0,   231,     0,
       0,    77,     0,    77,    95,   897,     0,     0,     0,    94,
      77,     0,     0,    95,    95,     0,     0,     0,     0,   900,
      94,     0,   902,     0,     0,     0,     0,   904,   213,     0,
      95,     0,    95,    95,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    95,     0,     0,     0,    95,    91,
       0,    93,    95,     0,    95,     0,     0,     0,    94,     0,
       0,     0,    95,    94,    91,    91,    91,     0,     0,     0,
      94,   291,     0,     0,     0,     0,    95,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      95,     0,     0,     0,     0,     0,     0,     0,    91,     0,
       0,     0,     0,     0,    93,     0,     0,     0,     0,     0,
       0,     0,     0,    91,    91,    93,    91,     0,    95,     0,
     420,   420,     0,     0,     0,     0,     0,     0,     0,    94,
     432,   844,     0,     0,     0,    91,     0,     0,     0,     0,
       0,    94,     0,     0,     0,     0,     0,     0,     0,    94,
       0,    94,     0,    93,     0,     0,     0,     0,    93,     0,
       0,     0,     0,     0,     0,    93,     0,     0,     0,    91,
       0,     0,     0,     0,     0,     0,    91,     0,     0,     0,
       0,     0,    91,     0,    91,     0,     0,    94,     0,     0,
       0,    91,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    95,    95,     0,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,     0,
       0,   339,   340,    95,    93,     0,     0,     0,     0,     0,
       0,   282,     0,     0,     0,     0,    93,     0,     0,     0,
       0,     0,     0,     0,    93,   341,    93,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   282,     0,     0,
       0,     0,     0,     0,    95,     0,     0,   587,     0,     0,
       0,     0,   258,     0,     0,   282,     0,     0,     0,     0,
       0,     0,    93,     0,    95,     0,     0,     0,     0,   282,
      95,     0,     0,   420,    95,     0,     0,     0,     0,     0,
       0,    94,     0,     0,     0,     0,     0,     0,   420,     0,
       0,     0,     0,     0,     0,     0,   414,   415,   416,     0,
      94,    95,     0,     0,     0,     0,    95,    95,   326,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,  -508,
    -508,     0,     0,   339,   340,   282,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    95,     0,     0,    95,   664,   342,
     343,   344,   345,   346,   347,   348,   349,   350,   351,    95,
      95,    95,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    94,   756,     0,     0,     0,    93,     0,     0,     0,
       0,     0,     0,   493,     0,     0,     0,     0,   501,     0,
     232,     0,     0,    95,     0,    93,   680,     0,   680,   680,
     664,   664,    95,     0,     0,     0,     0,     0,    95,    95,
       0,    95,     0,     0,     0,     0,     0,   680,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      95,     0,     0,   700,     0,   700,     0,   700,     0,     0,
       0,    94,     0,   712,   715,     0,   715,     0,     0,     0,
       0,     0,     0,     0,   715,     0,    94,    94,    94,     0,
       0,     0,     0,     0,    95,     0,    93,   501,     0,     0,
       0,    95,     0,     0,     0,     0,     0,    95,   420,    95,
     616,   618,     0,     0,   622,     0,    95,     0,   627,   420,
      94,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    95,    94,    94,     0,    94,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   650,
       0,     0,   622,   845,   650,     0,     0,    94,     0,     0,
       0,     0,     0,     0,     0,     0,    93,     0,     0,     0,
       0,     0,     0,     0,     0,   664,     0,     0,     0,     0,
       0,    93,    93,    93,   665,     0,     0,     0,     0,     0,
       0,    94,     0,     0,     0,     0,     0,     0,    94,   797,
       0,     0,   801,     0,    94,     0,    94,     0,     0,     0,
       0,     0,     0,    94,     0,    93,     0,     0,     0,     0,
       0,   420,     0,     0,   420,   420,     0,     0,     0,     0,
      93,    93,     0,    93,   680,   680,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -507,     0,
       0,     0,    93,     0,     0,     0,  -507,  -507,  -507,     0,
       0,  -507,  -507,  -507,     0,  -507,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -507,   700,   700,   709,     0,
       0,   700,     0,     0,  -507,  -507,    93,  -507,  -507,  -507,
    -507,  -507,   726,    93,     0,   715,     0,     0,     0,    93,
       0,    93,     0,     0,     0,   420,   420,     0,    93,     0,
     733,     0,     0,     0,   736,     0,     0,     0,   737,     0,
       0,     0,     0,     0,     0,     0,   618,     0,     0,     0,
       0,     0,  -507,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   420,   420,     0,     0,     0,     0,
     420,     0,     0,     0,     0,  -507,     0,   700,     0,   764,
       0,   282,     0,   587,   715,     0,     0,     0,     0,   420,
     668,  -507,     0,     0,  -507,  -507,     0,     0,   252,     0,
       0,  -507,  -507,   420,     0,     0,   420,   784,     0,     0,
       0,   420,     0,     0,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,     0,     0,   339,
     340,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     650,     0,     0,     0,     0,     0,     0,   622,     0,     0,
       0,     0,     0,   341,   622,   342,   343,   344,   345,   346,
     347,   348,   349,   350,   351,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -221,   827,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -507,     4,     0,     5,     6,     7,     8,     9,
       0,     0,     0,    10,    11,     0,     0,     0,    12,     0,
      13,    14,    15,    16,    17,    18,    19,     0,     0,   849,
       0,    20,    21,    22,    23,    24,    25,    26,   857,     0,
      27,     0,     0,   862,     0,     0,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     0,    43,    44,     0,     0,    45,    46,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   618,     0,   622,     0,     0,     0,    48,     0,     0,
      49,    50,     0,    51,    52,     0,    53,     0,    54,    55,
      56,    57,    58,   895,     0,   898,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -278,     0,   622,
      59,    60,    61,     0,   903,  -278,  -278,  -278,     0,     0,
    -278,  -278,  -278,     0,  -278,  -507,  -507,     0,     0,     0,
       0,     0,     0,     0,  -278,  -278,     0,     0,     0,     0,
       0,     0,     0,  -278,  -278,     0,  -278,  -278,  -278,  -278,
    -278,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -278,  -278,  -278,  -278,  -278,  -278,
    -278,  -278,  -278,  -278,  -278,  -278,  -278,     0,     0,  -278,
    -278,  -278,     0,     0,  -278,     0,     0,     0,     0,     0,
    -278,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -278,  -278,  -278,  -278,  -278,  -278,  -278,
    -278,  -278,  -278,  -278,  -278,     0,     0,     0,     0,     0,
    -278,  -278,  -278,  -278,  -278,  -278,  -491,     0,  -278,     0,
    -278,  -278,     0,     0,  -491,  -491,  -491,     0,     0,     0,
    -491,  -491,     0,  -491,     0,     0,     0,     0,   668,     0,
       0,     0,  -491,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -491,  -491,     0,  -491,  -491,  -491,  -491,  -491,
       0,     0,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,     0,     0,   339,   340,     0,
       0,     0,     0,  -491,  -491,  -491,  -491,  -491,  -491,  -491,
    -491,  -491,  -491,  -491,  -491,  -491,     0,     0,  -491,  -491,
    -491,   341,   670,   342,   343,   344,   345,   346,   347,   348,
     349,   350,   351,     0,     0,     0,     0,     0,     0,     0,
       0,   -87,  -491,     0,  -491,  -491,  -491,  -491,  -491,  -491,
    -491,  -491,  -491,  -491,     0,     0,     0,     0,  -491,  -491,
    -491,     0,  -491,  -491,   -79,  -284,     0,  -491,     0,  -491,
    -491,     0,     0,  -284,  -284,  -284,     0,     0,     0,  -284,
    -284,     0,  -284,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -284,  -284,     0,  -284,  -284,  -284,  -284,  -284,     0,
       0,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,     0,     0,   339,   340,     0,     0,
       0,  -221,  -284,  -284,  -284,  -284,  -284,  -284,  -284,  -284,
    -284,  -284,  -284,  -284,  -284,     0,     0,  -284,  -284,  -284,
     341,   671,   342,   343,   344,   345,   346,   347,   348,   349,
     350,   351,     0,     0,     0,     0,     0,     0,     0,     0,
     -89,  -284,  -221,  -284,  -284,  -284,  -284,  -284,  -284,  -284,
    -284,  -284,  -284,     0,     0,     0,     0,     0,  -284,  -284,
       0,  -284,  -284,   -81,     0,     0,  -284,     0,  -284,  -284,
       4,     0,     5,     6,     7,     8,     9,  -507,  -507,  -507,
      10,    11,     0,     0,  -507,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,     0,    49,    50,     0,
      51,    52,     0,    53,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    59,    60,    61,
       0,     0,     0,     0,     0,     4,     0,     5,     6,     7,
       8,     9,  -507,  -507,  -507,    10,    11,     0,  -507,  -507,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,    49,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    59,    60,    61,     4,     0,     5,     6,     7,
       8,     9,     0,     0,  -507,    10,    11,  -507,  -507,  -507,
      12,  -507,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,    49,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    59,    60,    61,     4,     0,     5,     6,     7,
       8,     9,     0,     0,  -507,    10,    11,  -507,  -507,  -507,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,    49,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    59,    60,    61,     4,     0,     5,     6,     7,
       8,     9,     0,  -507,  -507,    10,    11,  -507,  -507,     0,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,    49,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    59,    60,    61,     4,     0,     5,     6,     7,
       8,     9,     0,     0,     0,    10,    11,  -507,  -507,     0,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,   272,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    59,    60,    61,     0,     0,     0,     0,     0,
       4,  -507,     5,     6,     7,     8,     9,  -507,  -507,     0,
      10,    11,     0,     0,     0,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,     0,    49,    50,     0,
      51,    52,     0,    53,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    59,    60,    61,
       0,     0,     0,     0,     0,     4,  -507,     5,     6,     7,
       8,     9,  -507,  -507,     0,    10,    11,     0,     0,     0,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    43,    44,     0,     0,    45,    46,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
       0,     0,    49,    50,     0,    51,    52,     0,    53,     0,
      54,    55,    56,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    59,    60,    61,     0,     0,  -507,     0,     0,
       4,     0,     5,     6,     7,     8,     9,  -507,  -507,  -507,
      10,    11,     0,     0,     0,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     0,    43,
      44,     0,     0,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,     0,    49,    50,     0,
      51,    52,     0,    53,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    59,    60,    61,
       0,     5,     6,     7,     8,     9,     0,     0,     0,    10,
      11,     0,  -507,  -507,    12,     0,    13,    14,    15,    16,
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
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
       0,     0,   406,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,     0,     0,    27,     0,     0,     0,     0,
       0,     0,    29,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,    43,    44,     0,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   209,     0,     0,   108,    50,     0,    51,    52,
       0,     0,     0,    54,    55,    56,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    59,    60,    61,     0,     0,
       0,     0,   118,   119,   120,   121,   122,   123,   124,   125,
     218,   219,   126,   127,   128,   129,   130,     0,     0,   131,
     132,   133,   134,   135,   136,   137,     0,     0,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,    36,    37,   161,    39,     0,     0,     0,     0,     0,
       0,     0,     0,   162,   163,     0,   164,   165,   166,   167,
     168,   169,   170,     0,     0,   171,     0,     0,     0,   172,
     173,   174,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,     0,     0,   187,
       0,     0,  -484,  -484,  -484,     0,  -484,     0,   188,   189,
    -484,  -484,     0,     0,     0,  -484,     0,  -484,  -484,  -484,
    -484,  -484,  -484,  -484,     0,  -484,     0,     0,  -484,  -484,
    -484,  -484,  -484,  -484,  -484,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -484,     0,     0,  -484,  -484,  -484,
    -484,  -484,  -484,  -484,  -484,  -484,  -484,  -484,     0,  -484,
    -484,     0,  -484,  -484,  -484,  -484,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -484,     0,     0,  -484,  -484,     0,
    -484,  -484,     0,  -484,  -484,  -484,  -484,  -484,  -484,  -484,
       0,     0,     0,     0,     0,     0,     0,  -485,  -485,  -485,
       0,  -485,     0,     0,     0,  -485,  -485,  -484,  -484,  -484,
    -485,  -484,  -485,  -485,  -485,  -485,  -485,  -485,  -485,  -484,
    -485,     0,     0,  -485,  -485,  -485,  -485,  -485,  -485,  -485,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -485,
       0,     0,  -485,  -485,  -485,  -485,  -485,  -485,  -485,  -485,
    -485,  -485,  -485,     0,  -485,  -485,     0,  -485,  -485,  -485,
    -485,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -485,
       0,     0,  -485,  -485,     0,  -485,  -485,     0,  -485,  -485,
    -485,  -485,  -485,  -485,  -485,     0,     0,     0,     0,     0,
       0,     0,  -487,  -487,  -487,     0,  -487,     0,     0,     0,
    -487,  -487,  -485,  -485,  -485,  -487,  -485,  -487,  -487,  -487,
    -487,  -487,  -487,  -487,  -485,     0,     0,     0,  -487,  -487,
    -487,  -487,  -487,  -487,  -487,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -487,     0,     0,  -487,  -487,  -487,
    -487,  -487,  -487,  -487,  -487,  -487,  -487,  -487,     0,  -487,
    -487,     0,  -487,  -487,  -487,  -487,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -487,   719,     0,  -487,  -487,     0,
    -487,  -487,     0,  -487,  -487,  -487,  -487,  -487,  -487,  -487,
       0,     0,     0,     0,   -87,     0,     0,  -488,  -488,  -488,
       0,  -488,     0,     0,     0,  -488,  -488,  -487,  -487,  -487,
    -488,     0,  -488,  -488,  -488,  -488,  -488,  -488,  -488,  -487,
       0,     0,     0,  -488,  -488,  -488,  -488,  -488,  -488,  -488,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -488,
       0,     0,  -488,  -488,  -488,  -488,  -488,  -488,  -488,  -488,
    -488,  -488,  -488,     0,  -488,  -488,     0,  -488,  -488,  -488,
    -488,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -488,
     671,     0,  -488,  -488,     0,  -488,  -488,     0,  -488,  -488,
    -488,  -488,  -488,  -488,  -488,     0,     0,     0,     0,   -89,
       0,     0,  -255,  -255,  -255,     0,  -255,     0,     0,     0,
    -255,  -255,  -488,  -488,  -488,  -255,     0,  -255,  -255,  -255,
    -255,  -255,  -255,  -255,  -488,     0,     0,     0,  -255,  -255,
    -255,  -255,  -255,  -255,  -255,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -255,     0,     0,  -255,  -255,  -255,
    -255,  -255,  -255,  -255,  -255,  -255,  -255,  -255,     0,  -255,
    -255,     0,  -255,  -255,  -255,  -255,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -255,     0,     0,  -255,  -255,     0,
    -255,  -255,     0,  -255,  -255,  -255,  -255,  -255,  -255,  -255,
       0,     0,     0,     0,     0,     0,     0,  -255,  -255,  -255,
       0,  -255,     0,     0,     0,  -255,  -255,  -255,  -255,  -255,
    -255,     0,  -255,  -255,  -255,  -255,  -255,  -255,  -255,   249,
       0,     0,     0,  -255,  -255,  -255,  -255,  -255,  -255,  -255,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -255,
       0,     0,  -255,  -255,  -255,  -255,  -255,  -255,  -255,  -255,
    -255,  -255,  -255,     0,  -255,  -255,     0,  -255,  -255,  -255,
    -255,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -255,
       0,     0,  -255,  -255,     0,  -255,  -255,     0,  -255,  -255,
    -255,  -255,  -255,  -255,  -255,     0,     0,     0,     0,     0,
       0,     0,  -489,  -489,  -489,     0,  -489,     0,     0,     0,
    -489,  -489,  -255,  -255,  -255,  -489,     0,  -489,  -489,  -489,
    -489,  -489,  -489,  -489,   252,     0,     0,     0,  -489,  -489,
    -489,  -489,  -489,  -489,  -489,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -489,     0,     0,  -489,  -489,  -489,
    -489,  -489,  -489,  -489,  -489,  -489,  -489,  -489,     0,  -489,
    -489,     0,  -489,  -489,  -489,  -489,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -489,     0,     0,  -489,  -489,     0,
    -489,  -489,     0,  -489,  -489,  -489,  -489,  -489,  -489,  -489,
       0,     0,     0,     0,     0,     0,     0,  -490,  -490,  -490,
       0,  -490,     0,     0,     0,  -490,  -490,  -489,  -489,  -489,
    -490,     0,  -490,  -490,  -490,  -490,  -490,  -490,  -490,  -489,
       0,     0,     0,  -490,  -490,  -490,  -490,  -490,  -490,  -490,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -490,
       0,     0,  -490,  -490,  -490,  -490,  -490,  -490,  -490,  -490,
    -490,  -490,  -490,     0,  -490,  -490,     0,  -490,  -490,  -490,
    -490,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -490,
       0,     0,  -490,  -490,     0,  -490,  -490,     0,  -490,  -490,
    -490,  -490,  -490,  -490,  -490,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -490,  -490,  -490,   118,   119,   120,   121,   122,
     123,   124,   125,     0,  -490,   126,   127,   128,   129,   130,
       0,     0,   131,   132,   133,   134,   135,   136,   137,     0,
       0,   138,   139,   140,   195,   196,   197,   198,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     199,   200,   201,   160,   292,   293,   202,   294,     0,     0,
       0,     0,     0,     0,     0,     0,   162,   163,     0,   164,
     165,   166,   167,   168,   169,   170,     0,     0,   171,     0,
       0,     0,   172,   173,   174,   175,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
       0,     0,   187,   118,   119,   120,   121,   122,   123,   124,
     125,   188,     0,   126,   127,   128,   129,   130,     0,     0,
     131,   132,   133,   134,   135,   136,   137,     0,     0,   138,
     139,   140,   195,   196,   197,   198,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   199,   200,
     201,   160,   256,     0,   202,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   162,   163,     0,   164,   165,   166,
     167,   168,   169,   170,     0,     0,   171,     0,     0,     0,
     172,   173,   174,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,     0,   203,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,     0,     0,
     187,   118,   119,   120,   121,   122,   123,   124,   125,   188,
       0,   126,   127,   128,   129,   130,     0,     0,   131,   132,
     133,   134,   135,   136,   137,     0,     0,   138,   139,   140,
     195,   196,   197,   198,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   199,   200,   201,   160,
       0,     0,   202,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   162,   163,     0,   164,   165,   166,   167,   168,
     169,   170,     0,     0,   171,     0,     0,     0,   172,   173,
     174,   175,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   176,     0,   203,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,     0,     0,   187,   118,
     119,   120,   121,   122,   123,   124,   125,   188,     0,   126,
     127,   128,   129,   130,     0,     0,   131,   132,   133,   134,
     135,   136,   137,     0,     0,   138,   139,   140,   195,   196,
     197,   198,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   199,   200,   201,   160,     0,     0,
     202,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     162,   163,     0,   164,   165,   166,   167,   168,   169,   170,
       0,     0,   171,     0,     0,     0,   172,   173,   174,   175,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     176,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,     0,     0,   187,     0,     5,     6,
       7,     0,     9,     0,     0,   188,    10,    11,     0,     0,
       0,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,    21,    22,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,   233,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     209,     0,     0,   108,    50,     0,    51,    52,     0,   234,
     235,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     8,     9,    59,   236,    61,    10,    11,     0,     0,
       0,    12,   423,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,    60,    61,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,   233,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,   625,   235,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,    60,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,    21,    22,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,   233,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     209,     0,     0,   108,    50,     0,    51,    52,     0,   234,
     235,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   236,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
      21,    22,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,   233,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,   425,
       0,    51,    52,     0,   234,   235,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   236,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,   233,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     209,     0,     0,   108,    50,     0,    51,    52,     0,   617,
     235,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   236,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,   233,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,   621,   235,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   236,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,    21,    22,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,   233,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     209,     0,     0,   108,    50,     0,    51,    52,     0,   617,
     235,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   236,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,   233,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,   812,   235,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   236,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,   233,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     209,     0,     0,   108,    50,     0,    51,    52,     0,   815,
     235,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   236,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,   233,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,   820,   235,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   236,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,   233,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     209,     0,     0,   108,    50,     0,    51,    52,     0,   880,
     235,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   236,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,   233,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,   882,   235,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   236,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,   233,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     209,     0,     0,   108,    50,     0,    51,    52,     0,   901,
     235,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   236,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
      21,    22,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,   233,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,   277,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   236,
      61,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     209,     0,     0,   108,    50,     0,    51,    52,     0,     0,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,    60,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,   233,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   236,
      61,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     209,     0,     0,   108,    50,     0,    51,    52,     0,   491,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   236,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,   599,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   236,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     209,     0,     0,   108,    50,     0,    51,    52,     0,   649,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   236,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,   491,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   236,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     209,     0,     0,   108,    50,     0,    51,    52,     0,   763,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   236,    61,    12,     0,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,   806,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   236,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,     0,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     209,     0,     0,   108,    50,     0,    51,    52,     0,     0,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,    59,   236,    61,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,     0,    45,    46,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,     0,     0,   108,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    59,   236,
      61,    12,     0,    13,    14,    15,    98,    99,    18,    19,
       0,     0,     0,     0,   100,   101,   102,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     103,     0,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,    43,    44,     0,     0,     0,
       0,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     226,     0,     0,    49,    50,     0,    51,    52,     0,    53,
       0,    54,    55,    56,    57,    58,     0,     0,     0,     5,
       6,     7,     0,     9,     0,   760,     0,    10,    11,     0,
       0,     0,    12,   109,    13,    14,    15,    98,    99,    18,
      19,     0,     0,     0,     0,   100,   101,   102,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   103,     0,     0,    32,    33,   104,    35,    36,    37,
     105,    39,    40,    41,    42,     0,    43,    44,     0,     0,
       0,     0,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   106,     0,
       0,   107,     0,     0,   108,    50,     0,    51,    52,     0,
       0,     0,    54,    55,    56,    57,    58,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
       0,     0,     0,    12,   109,    13,    14,    15,    98,    99,
      18,    19,     0,     0,     0,     0,   100,   101,   102,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   103,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     0,    43,    44,     0,
       0,     0,     0,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   226,     0,     0,    49,    50,     0,    51,    52,
       0,    53,     0,    54,    55,    56,    57,    58,     0,     0,
       0,     5,     6,     7,     0,     9,     0,     0,     0,    10,
      11,     0,     0,     0,    12,   109,    13,    14,    15,    98,
      99,    18,    19,     0,     0,     0,     0,   100,   101,   102,
      23,    24,    25,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   103,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,    43,    44,
       0,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   287,     0,     0,   322,    50,     0,    51,
      52,     0,   323,     0,    54,    55,    56,    57,    58,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   109,    13,    14,    15,
      98,    99,    18,    19,     0,     0,     0,     0,   100,   101,
     102,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,     0,     0,    32,    33,   104,
      35,    36,    37,   105,    39,    40,    41,    42,     0,    43,
      44,     0,     0,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   107,     0,     0,   108,    50,     0,
      51,    52,     0,     0,     0,    54,    55,    56,    57,    58,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,     0,     0,     0,    12,   109,    13,    14,
      15,    98,    99,    18,    19,     0,     0,     0,     0,   100,
     101,   102,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   103,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
      43,    44,     0,     0,     0,     0,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   287,     0,     0,   108,    50,
       0,    51,    52,     0,     0,     0,    54,    55,    56,    57,
      58,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,     0,     0,     0,    12,   109,    13,
      14,    15,    98,    99,    18,    19,     0,     0,     0,     0,
     100,   101,   102,    23,    24,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   103,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       0,    43,    44,     0,     0,     0,     0,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   841,     0,     0,   108,
      50,     0,    51,    52,     0,     0,     0,    54,    55,    56,
      57,    58,     0,   266,   267,     0,     0,   268,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   162,   163,   109,
     164,   165,   166,   167,   168,   169,   170,     0,     0,   171,
       0,     0,     0,   172,   173,   174,   175,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   176,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   529,   530,   187,     0,   531,     0,     0,     0,     0,
       0,     0,   188,     0,     0,   162,   163,     0,   164,   165,
     166,   167,   168,   169,   170,     0,     0,   171,     0,     0,
       0,   172,   173,   174,   175,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   176,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   536,
     267,   187,     0,   537,     0,     0,     0,     0,     0,     0,
     188,     0,     0,   162,   163,     0,   164,   165,   166,   167,
     168,   169,   170,     0,     0,   171,     0,     0,     0,   172,
     173,   174,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   554,   530,   187,
       0,   555,     0,     0,     0,     0,     0,     0,   188,     0,
       0,   162,   163,     0,   164,   165,   166,   167,   168,   169,
     170,     0,     0,   171,     0,     0,     0,   172,   173,   174,
     175,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   176,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   591,   530,   187,     0,   592,
       0,     0,     0,     0,     0,     0,   188,     0,     0,   162,
     163,     0,   164,   165,   166,   167,   168,   169,   170,     0,
       0,   171,     0,     0,     0,   172,   173,   174,   175,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   176,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   594,   267,   187,     0,   595,     0,     0,
       0,     0,     0,     0,   188,     0,     0,   162,   163,     0,
     164,   165,   166,   167,   168,   169,   170,     0,     0,   171,
       0,     0,     0,   172,   173,   174,   175,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   176,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   608,   530,   187,     0,   609,     0,     0,     0,     0,
       0,     0,   188,     0,     0,   162,   163,     0,   164,   165,
     166,   167,   168,   169,   170,     0,     0,   171,     0,     0,
       0,   172,   173,   174,   175,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   176,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   611,
     267,   187,     0,   612,     0,     0,     0,     0,     0,     0,
     188,     0,     0,   162,   163,     0,   164,   165,   166,   167,
     168,   169,   170,     0,     0,   171,     0,     0,     0,   172,
     173,   174,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   651,   530,   187,
       0,   652,     0,     0,     0,     0,     0,     0,   188,     0,
       0,   162,   163,     0,   164,   165,   166,   167,   168,   169,
     170,     0,     0,   171,     0,     0,     0,   172,   173,   174,
     175,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   176,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   654,   267,   187,     0,   655,
       0,     0,     0,     0,     0,     0,   188,     0,     0,   162,
     163,     0,   164,   165,   166,   167,   168,   169,   170,     0,
       0,   171,     0,     0,     0,   172,   173,   174,   175,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   176,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   887,   530,   187,     0,   888,     0,     0,
       0,     0,     0,     0,   188,     0,     0,   162,   163,     0,
     164,   165,   166,   167,   168,   169,   170,     0,     0,   171,
       0,     0,     0,   172,   173,   174,   175,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   176,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   890,   267,   187,     0,   891,     0,     0,     0,     0,
       0,     0,   188,     0,     0,   162,   163,     0,   164,   165,
     166,   167,   168,   169,   170,     0,     0,   171,     0,     0,
       0,   172,   173,   174,   175,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   176,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,     0,
       0,   187,     0,     0,     0,     0,     0,     0,     0,     0,
     188,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,     0,     0,   339,   340,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     341,     0,   342,   343,   344,   345,   346,   347,   348,   349,
     350,   351,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   614,   326,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,     0,     0,   339,   340,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,     0,     0,   339,   340,     0,     0,     0,
       0,     0,   341,   766,   342,   343,   344,   345,   346,   347,
     348,   349,   350,   351,     0,     0,     0,     0,     0,   341,
       0,   342,   343,   344,   345,   346,   347,   348,   349,   350,
     351,   326,   327,   328,   329,   330,   331,   332,   333,     0,
     335,   336,     0,     0,     0,     0,   339,   340,   326,  -508,
    -508,  -508,  -508,   331,   332,     0,     0,  -508,  -508,     0,
       0,     0,     0,   339,   340,     0,     0,     0,     0,     0,
       0,     0,   342,   343,   344,   345,   346,   347,   348,   349,
     350,   351,     0,     0,     0,     0,     0,     0,     0,   342,
     343,   344,   345,   346,   347,   348,   349,   350,   351,   326,
     327,   328,   329,   330,   331,   332,     0,     0,   335,   336,
       0,     0,     0,     0,   339,   340,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     342,   343,   344,   345,   346,   347,   348,   349,   350,   351
};

static const short yycheck[] =
{
      16,    17,    15,   383,    20,     7,   217,    28,    16,    17,
     252,     8,    20,    22,   381,    22,   408,    53,    16,    17,
     107,   402,    20,     5,     6,   353,    63,     7,     8,    45,
      46,    28,     4,    15,   325,    51,    52,    16,    17,   421,
     701,    20,    26,    51,    60,    61,    16,    17,    28,     1,
      20,   379,    54,   298,    52,   673,   301,   586,   540,   541,
      27,   679,   698,    49,   577,   701,   579,   596,    13,   397,
      25,    53,    26,    26,    54,    11,    12,   459,    84,   542,
     543,    59,    52,   411,    60,    12,    95,   272,    95,     0,
      72,    59,   133,   102,    25,    86,    36,    37,    59,    50,
      87,    52,    53,    54,    55,    13,    54,   116,    13,    25,
      36,    37,    95,    75,   105,   132,   128,    13,    25,    17,
     126,   133,    20,   101,   102,    28,    86,   133,    47,   112,
      84,    25,   122,   101,   102,   103,    63,   322,   122,   467,
     107,   128,   103,    94,    95,   105,   130,    25,   110,    50,
      52,     2,    86,    54,   132,   110,   105,   109,    60,    61,
      25,   112,   107,    25,   115,   128,   133,   134,   122,   122,
     124,    25,   126,   128,   105,   836,   130,   131,   128,   110,
      84,   133,   134,   220,   111,   130,   553,   127,   133,   134,
     132,   571,    25,     2,   110,     4,   127,   128,    49,   107,
     836,   203,   107,   110,    84,   572,    15,   133,    57,    58,
      67,   107,   128,    84,   130,   105,   110,   233,   234,   235,
     236,   128,   126,   203,   687,   133,   134,   105,   133,   134,
     475,   130,   110,   249,   128,   251,   252,   133,   134,    97,
      49,   249,    84,   251,   252,   110,   126,   130,   110,   127,
     128,   249,   130,   251,   252,   126,   110,   108,   115,   116,
     117,   277,   128,   128,   746,   747,   128,   133,   130,    54,
     249,    86,   251,   252,   128,   788,   789,   110,   807,   249,
     793,   251,   252,    86,   126,   105,   272,   323,   224,   225,
     105,   378,   128,   220,   585,   128,    15,   133,    17,   108,
      84,    15,   105,    17,   320,    54,    86,    56,   134,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   103,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   351,   322,   353,   310,    25,
     732,   323,   126,   315,   320,   353,   362,   363,    86,   325,
     320,   249,   734,   251,   252,   325,   869,    54,    84,   741,
      25,    86,   128,   379,    84,    59,    87,   105,    62,   133,
     134,   379,   352,    36,    37,   311,   312,   313,   314,   395,
     105,   397,   128,   399,   400,   130,   362,   363,   404,   397,
     601,   633,    26,    57,    58,   411,    14,    15,   378,   208,
     126,   417,   418,   411,   124,   421,   126,   101,   102,   425,
     261,   131,    86,   780,   794,   436,   396,   122,    84,   105,
     418,   272,   438,    67,   110,    86,   807,   425,   127,   128,
     410,   105,   448,   352,   404,   354,   355,   356,   357,   436,
     456,   127,   128,   459,   105,   461,   425,   374,   127,    84,
      84,   467,   261,    86,   128,   127,   436,    86,   127,   467,
     126,   380,    54,   272,   480,   481,   446,   128,    10,    11,
      12,   322,   105,   840,    50,   491,   105,   396,   860,   398,
     407,   408,   125,   123,   103,    27,   466,    84,   122,   103,
     124,   126,   126,   412,    67,   128,   130,   131,    67,   128,
     278,   310,   518,   128,    84,    93,   315,     2,    50,     4,
       5,     6,    10,   322,   896,    10,    11,    12,    94,    95,
      15,    16,    17,     8,   533,    20,   533,    13,   447,   126,
      10,   538,    27,   540,   541,    17,   112,    84,    26,   115,
      86,   128,   393,   127,   124,   873,   126,    86,   557,   468,
     401,   131,   403,    84,    49,    50,    51,   128,    53,   105,
     127,   123,   583,   127,   106,    60,   105,   128,   128,   585,
     127,   281,   381,   283,   132,   285,    59,    72,   128,   126,
     582,   125,   128,   599,   393,   796,   583,   125,   439,   128,
      10,   802,   401,   124,   403,   126,    84,    84,   105,   130,
     131,   617,   582,   583,   128,   621,   128,    50,   128,   625,
      50,   106,    15,   108,    59,   585,   552,   633,   101,   102,
     103,    59,   125,   550,    50,   633,    52,    53,    54,    55,
     439,    10,    10,   649,   122,   633,   124,   124,   126,   126,
      95,   125,   130,   131,   131,   123,   573,   128,   127,   662,
     666,   429,   668,   431,   670,   671,   101,   102,   103,   675,
     676,   128,   128,   101,   102,   103,   127,    84,    94,    95,
     127,   127,   125,    10,   452,   453,   127,   613,   694,    84,
     662,    10,   224,   225,     9,    87,   112,    10,     2,   115,
       4,     5,     6,     7,   670,    10,    84,   127,   123,   675,
     676,    15,   110,   719,   130,   721,   722,   124,   127,   126,
     128,   727,   128,   208,   131,   634,   635,   243,   734,   124,
      10,   126,   738,    10,   128,   741,   131,   128,   127,   224,
     225,    10,    10,   123,    10,    49,   124,    54,   126,    53,
     738,   123,    84,   131,   553,    10,    10,   763,   128,   125,
     766,    84,    72,     6,   249,   771,   251,   252,    72,   738,
      67,   876,   689,   572,   875,   662,   261,    79,   738,   311,
     312,   313,   314,    82,   316,   317,     7,   272,    52,    53,
     716,    55,   124,   561,   126,   571,    60,    61,   804,   131,
     806,   124,   643,   126,   108,   771,   812,    63,   131,   815,
     794,    84,    -1,   698,   820,   732,   113,   114,   115,   116,
     117,   698,    84,   795,    -1,   310,   311,   312,   313,   314,
     315,   316,   317,    67,    -1,   320,    -1,   322,   323,    -1,
     325,    -1,    -1,    -1,   643,    -1,    -1,    -1,    82,    83,
     382,   124,   858,   126,   860,    -1,    -1,    -1,   131,    -1,
      -1,    -1,   124,   662,   126,    -1,    -1,   873,   353,   131,
     858,    -1,   713,    84,   880,   873,   882,   362,   363,   113,
     114,   115,   116,   117,    -1,    -1,    -1,   728,   729,   730,
     896,    -1,    84,   425,   379,   901,   381,   382,    -1,   415,
      -1,    -1,   872,    -1,   208,    -1,    -1,    -1,   393,    -1,
      -1,    -1,   397,   124,   713,   126,   401,    -1,   403,    -1,
     131,   762,    -1,    -1,    -1,    -1,   411,   695,    -1,   728,
     729,   730,   124,    -1,   126,    -1,   777,   778,    -1,   131,
     425,    39,    40,    41,    42,    43,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   439,    -1,    -1,   261,   799,    -1,
      -1,    -1,    -1,   762,    -1,   874,    -1,    -1,   272,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   745,   777,   778,
      -1,   780,   467,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     758,    -1,   833,    -1,    -1,    -1,   795,    -1,    -1,    -1,
     799,    -1,    -1,    -1,    -1,   846,   310,   848,    -1,    -1,
      -1,   315,    -1,     2,   855,     4,    -1,    -1,   322,   323,
     552,    10,    11,    12,    -1,    -1,    -1,    16,    17,    -1,
      -1,    20,    -1,    -1,   833,    -1,    -1,    -1,    27,    -1,
      -1,   840,    -1,    -1,    -1,    -1,    -1,   846,    -1,   848,
      -1,    -1,    -1,    -1,    -1,    -1,   855,    -1,    -1,    -1,
      49,    50,    51,    -1,    -1,    -1,    -1,   552,   553,    -1,
      -1,    60,    -1,    -1,    -1,    -1,    -1,   381,    -1,    -1,
      -1,   613,    -1,    -1,    -1,    -1,    -1,   572,    -1,   393,
      -1,    -1,    -1,    67,    -1,    -1,    -1,   401,    -1,   403,
      -1,    -1,    -1,    -1,   620,    -1,    -1,   639,    82,    83,
       2,    -1,     4,     5,     6,   631,    -1,   106,    67,   108,
      -1,    -1,    -1,    15,    -1,    -1,    -1,    -1,   613,    -1,
      -1,    -1,    -1,    82,    83,   439,   110,   111,   112,   113,
     114,   115,   116,   117,    -1,    -1,    -1,    -1,   633,    -1,
      -1,    -1,    -1,    -1,   639,    -1,    -1,    49,   643,    -1,
      -1,    53,    -1,   112,   113,   114,   115,   116,   117,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   662,    -1,    -1,
      72,    -1,    -1,    -1,   716,   670,    -1,    -1,    -1,    -1,
     675,   676,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     2,    -1,     4,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   108,   733,    -1,   208,
     736,   737,    -1,    -1,    -1,    -1,    -1,    -1,   713,    -1,
      -1,   716,    -1,    -1,    -1,   224,   225,    -1,    -1,    -1,
      -1,    -1,    -1,   728,   729,   730,    -1,    -1,    -1,   553,
      -1,    -1,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     249,    -1,   251,   252,    -1,    -1,    -1,    -1,   572,    -1,
      -1,    -1,   261,    -1,    -1,    -1,    -1,   762,    -1,    -1,
      -1,    -1,    -1,   272,    -1,    -1,   771,    -1,    -1,    51,
      52,    -1,   777,   778,    -1,   780,    -1,    -1,    -1,    -1,
      -1,   817,   818,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     795,   108,    -1,    -1,   799,    -1,   208,    -1,    -1,    -1,
      -1,   310,   311,   312,   313,   314,   315,   316,   317,    -1,
      -1,   320,    -1,   322,    -1,    -1,   325,    -1,    -1,   643,
     856,   857,    -1,    -1,    -1,    -1,   862,    -1,   833,    -1,
      -1,    -1,    -1,    -1,    -1,   840,    -1,    -1,   662,    -1,
      -1,   846,    -1,   848,   353,   881,    -1,    -1,    -1,   261,
     855,    -1,    -1,   362,   363,    -1,    -1,    -1,    -1,   895,
     272,    -1,   898,    -1,    -1,    -1,    -1,   903,   873,    -1,
     379,    -1,   381,   382,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   393,    -1,    -1,    -1,   397,   713,
      -1,   208,   401,    -1,   403,    -1,    -1,    -1,   310,    -1,
      -1,    -1,   411,   315,   728,   729,   730,    -1,    -1,    -1,
     322,   323,    -1,    -1,    -1,    -1,   425,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     439,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   762,    -1,
      -1,    -1,    -1,    -1,   261,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   777,   778,   272,   780,    -1,   467,    -1,
     242,   243,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   381,
     252,   795,    -1,    -1,    -1,   799,    -1,    -1,    -1,    -1,
      -1,   393,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   401,
      -1,   403,    -1,   310,    -1,    -1,    -1,    -1,   315,    -1,
      -1,    -1,    -1,    -1,    -1,   322,    -1,    -1,    -1,   833,
      -1,    -1,    -1,    -1,    -1,    -1,   840,    -1,    -1,    -1,
      -1,    -1,   846,    -1,   848,    -1,    -1,   439,    -1,    -1,
      -1,   855,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   552,   553,    -1,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    -1,
      -1,    82,    83,   572,   381,    -1,    -1,    -1,    -1,    -1,
      -1,   353,    -1,    -1,    -1,    -1,   393,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   401,   106,   403,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   379,    -1,    -1,
      -1,    -1,    -1,    -1,   613,    -1,    -1,   389,    -1,    -1,
      -1,    -1,   133,    -1,    -1,   397,    -1,    -1,    -1,    -1,
      -1,    -1,   439,    -1,   633,    -1,    -1,    -1,    -1,   411,
     639,    -1,    -1,   415,   643,    -1,    -1,    -1,    -1,    -1,
      -1,   553,    -1,    -1,    -1,    -1,    -1,    -1,   430,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   233,   234,   235,    -1,
     572,   670,    -1,    -1,    -1,    -1,   675,   676,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    -1,    -1,    82,    83,   467,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   713,    -1,    -1,   716,   490,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   728,
     729,   730,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   643,   644,    -1,    -1,    -1,   553,    -1,    -1,    -1,
      -1,    -1,    -1,   320,    -1,    -1,    -1,    -1,   325,    -1,
     662,    -1,    -1,   762,    -1,   572,   538,    -1,   540,   541,
     542,   543,   771,    -1,    -1,    -1,    -1,    -1,   777,   778,
      -1,   780,    -1,    -1,    -1,    -1,    -1,   559,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     799,    -1,    -1,   575,    -1,   577,    -1,   579,    -1,    -1,
      -1,   713,    -1,   585,   586,    -1,   588,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   596,    -1,   728,   729,   730,    -1,
      -1,    -1,    -1,    -1,   833,    -1,   643,   404,    -1,    -1,
      -1,   840,    -1,    -1,    -1,    -1,    -1,   846,   620,   848,
     417,   418,    -1,    -1,   421,    -1,   855,    -1,   425,   631,
     762,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   873,   777,   778,    -1,   780,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   456,
      -1,    -1,   459,   795,   461,    -1,    -1,   799,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   713,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   687,    -1,    -1,    -1,    -1,
      -1,   728,   729,   730,   491,    -1,    -1,    -1,    -1,    -1,
      -1,   833,    -1,    -1,    -1,    -1,    -1,    -1,   840,   711,
      -1,    -1,   714,    -1,   846,    -1,   848,    -1,    -1,    -1,
      -1,    -1,    -1,   855,    -1,   762,    -1,    -1,    -1,    -1,
      -1,   733,    -1,    -1,   736,   737,    -1,    -1,    -1,    -1,
     777,   778,    -1,   780,   746,   747,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,    -1,
      -1,    -1,   799,    -1,    -1,    -1,     8,     9,    10,    -1,
      -1,    13,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,   788,   789,   585,    -1,
      -1,   793,    -1,    -1,    36,    37,   833,    39,    40,    41,
      42,    43,   599,   840,    -1,   807,    -1,    -1,    -1,   846,
      -1,   848,    -1,    -1,    -1,   817,   818,    -1,   855,    -1,
     617,    -1,    -1,    -1,   621,    -1,    -1,    -1,   625,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   633,    -1,    -1,    -1,
      -1,    -1,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   856,   857,    -1,    -1,    -1,    -1,
     862,    -1,    -1,    -1,    -1,   107,    -1,   869,    -1,   666,
      -1,   873,    -1,   875,   876,    -1,    -1,    -1,    -1,   881,
      43,   123,    -1,    -1,   126,   127,    -1,    -1,   130,    -1,
      -1,   133,   134,   895,    -1,    -1,   898,   694,    -1,    -1,
      -1,   903,    -1,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    -1,    -1,    82,
      83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     727,    -1,    -1,    -1,    -1,    -1,    -1,   734,    -1,    -1,
      -1,    -1,    -1,   106,   741,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   128,   763,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     0,     1,    -1,     3,     4,     5,     6,     7,
      -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,   806,
      -1,    29,    30,    31,    32,    33,    34,    35,   815,    -1,
      38,    -1,    -1,   820,    -1,    -1,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    -1,    60,    61,    -1,    -1,    64,    65,    66,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   858,    -1,   860,    -1,    -1,    -1,    85,    -1,    -1,
      88,    89,    -1,    91,    92,    -1,    94,    -1,    96,    97,
      98,    99,   100,   880,    -1,   882,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,    -1,   896,
     118,   119,   120,    -1,   901,     8,     9,    10,    -1,    -1,
      13,    14,    15,    -1,    17,   133,   134,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    28,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    36,    37,    -1,    39,    40,    41,    42,
      43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    -1,    -1,    82,
      83,    84,    -1,    -1,    87,    -1,    -1,    -1,    -1,    -1,
      93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,    -1,    -1,    -1,    -1,    -1,
     123,   124,   125,   126,   127,   128,     0,    -1,   131,    -1,
     133,   134,    -1,    -1,     8,     9,    10,    -1,    -1,    -1,
      14,    15,    -1,    17,    -1,    -1,    -1,    -1,    43,    -1,
      -1,    -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    36,    37,    -1,    39,    40,    41,    42,    43,
      -1,    -1,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    -1,    -1,    82,    83,    -1,
      -1,    -1,    -1,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    -1,    -1,    82,    83,
      84,   106,    86,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   105,   106,    -1,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,    -1,    -1,    -1,    -1,   122,   123,
     124,    -1,   126,   127,   128,     0,    -1,   131,    -1,   133,
     134,    -1,    -1,     8,     9,    10,    -1,    -1,    -1,    14,
      15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    36,    37,    -1,    39,    40,    41,    42,    43,    -1,
      -1,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    -1,    -1,    82,    83,    -1,    -1,
      -1,    87,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    -1,    -1,    82,    83,    84,
     106,    86,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     105,   106,   128,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    -1,    -1,    -1,    -1,    -1,   123,   124,
      -1,   126,   127,   128,    -1,    -1,   131,    -1,   133,   134,
       1,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    -1,    -1,    15,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    -1,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   118,   119,   120,
      -1,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,
       6,     7,   133,   134,    10,    11,    12,    -1,    14,    15,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   118,   119,   120,     1,    -1,     3,     4,     5,
       6,     7,    -1,    -1,    10,    11,    12,   133,   134,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   118,   119,   120,     1,    -1,     3,     4,     5,
       6,     7,    -1,    -1,    10,    11,    12,   133,   134,    15,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   118,   119,   120,     1,    -1,     3,     4,     5,
       6,     7,    -1,     9,    10,    11,    12,   133,   134,    -1,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   118,   119,   120,     1,    -1,     3,     4,     5,
       6,     7,    -1,    -1,    -1,    11,    12,   133,   134,    -1,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   118,   119,   120,    -1,    -1,    -1,    -1,    -1,
       1,   127,     3,     4,     5,     6,     7,   133,   134,    -1,
      11,    12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    -1,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   118,   119,   120,
      -1,    -1,    -1,    -1,    -1,     1,   127,     3,     4,     5,
       6,     7,   133,   134,    -1,    11,    12,    -1,    -1,    -1,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   118,   119,   120,    -1,    -1,   123,    -1,    -1,
       1,    -1,     3,     4,     5,     6,     7,   133,   134,    10,
      11,    12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    -1,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   118,   119,   120,
      -1,     3,     4,     5,     6,     7,    -1,    -1,    -1,    11,
      12,    -1,   133,   134,    16,    -1,    18,    19,    20,    21,
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
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
      -1,    -1,   134,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
      -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,
      -1,    -1,    -1,    96,    97,    98,    99,   100,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   118,   119,   120,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,     9,    10,
     133,   134,    13,    14,    15,    16,    17,    -1,    -1,    20,
      21,    22,    23,    24,    25,    26,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    64,    65,    -1,    67,    68,    69,    70,
      71,    72,    73,    -1,    -1,    76,    -1,    -1,    -1,    80,
      81,    82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,    -1,    -1,   120,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,   129,   130,
      11,    12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    95,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,   122,    18,    19,    20,    21,    22,    23,    24,   130,
      26,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    95,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   118,   119,   120,    16,   122,    18,    19,    20,
      21,    22,    23,    24,   130,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    86,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    95,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,   105,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,   130,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      86,    -1,    88,    89,    -1,    91,    92,    -1,    94,    95,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,   105,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   118,   119,   120,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,   130,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    95,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,   130,
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
      21,    22,    23,    24,   130,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
      91,    92,    -1,    94,    95,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,   130,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    61,    -1,    63,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,    95,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   118,   119,   120,     3,     4,     5,     6,     7,
       8,     9,    10,    -1,   130,    13,    14,    15,    16,    17,
      -1,    -1,    20,    21,    22,    23,    24,    25,    26,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    64,    65,    -1,    67,
      68,    69,    70,    71,    72,    73,    -1,    -1,    76,    -1,
      -1,    -1,    80,    81,    82,    83,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
      -1,    -1,   120,     3,     4,     5,     6,     7,     8,     9,
      10,   129,    -1,    13,    14,    15,    16,    17,    -1,    -1,
      20,    21,    22,    23,    24,    25,    26,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    -1,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    64,    65,    -1,    67,    68,    69,
      70,    71,    72,    73,    -1,    -1,    76,    -1,    -1,    -1,
      80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    -1,    96,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,    -1,    -1,
     120,     3,     4,     5,     6,     7,     8,     9,    10,   129,
      -1,    13,    14,    15,    16,    17,    -1,    -1,    20,    21,
      22,    23,    24,    25,    26,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      -1,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    65,    -1,    67,    68,    69,    70,    71,
      72,    73,    -1,    -1,    76,    -1,    -1,    -1,    80,    81,
      82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    94,    -1,    96,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,    -1,    -1,   120,     3,
       4,     5,     6,     7,     8,     9,    10,   129,    -1,    13,
      14,    15,    16,    17,    -1,    -1,    20,    21,    22,    23,
      24,    25,    26,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    -1,    -1,
      54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      64,    65,    -1,    67,    68,    69,    70,    71,    72,    73,
      -1,    -1,    76,    -1,    -1,    -1,    80,    81,    82,    83,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,    -1,    -1,   120,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,   129,    11,    12,    -1,    -1,
      -1,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,
      95,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,     7,   118,   119,   120,    11,    12,    -1,    -1,
      -1,    16,   127,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    -1,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,
      -1,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   118,   119,   120,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      -1,    91,    92,    -1,    94,    95,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,
     120,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,
      95,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   118,   119,   120,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      -1,    91,    92,    -1,    94,    95,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,
     120,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,
      95,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   118,   119,   120,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      -1,    91,    92,    -1,    94,    95,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,
     120,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,
      95,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   118,   119,   120,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      -1,    91,    92,    -1,    94,    95,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,
     120,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,
      95,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   118,   119,   120,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      -1,    91,    92,    -1,    94,    95,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,
     120,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,
      95,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   118,   119,   120,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      -1,    91,    92,    -1,    94,    95,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,
     120,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    63,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,
      95,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   118,   119,   120,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      -1,    91,    92,    -1,    94,    -1,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,
     120,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    -1,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    -1,
      -1,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   118,   119,   120,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    63,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      -1,    91,    92,    -1,    -1,    -1,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,
     120,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    -1,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,
      -1,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   118,   119,   120,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      -1,    91,    92,    -1,    94,    -1,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,
     120,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    -1,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,
      -1,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   118,   119,   120,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      -1,    91,    92,    -1,    94,    -1,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,
     120,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    -1,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,
      -1,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   118,   119,   120,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      -1,    91,    92,    -1,    94,    -1,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,
     120,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    -1,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    -1,
      -1,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,   118,   119,   120,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      60,    61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      -1,    91,    92,    -1,    -1,    -1,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   118,   119,
     120,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    -1,    -1,
      -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,    94,
      -1,    96,    97,    98,    99,   100,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,   110,    -1,    11,    12,    -1,
      -1,    -1,    16,   118,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    60,    61,    -1,    -1,
      -1,    -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,
      -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,    -1,
      -1,    -1,    96,    97,    98,    99,   100,    -1,    -1,    -1,
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
      92,    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,
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
     100,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,    -1,    -1,    -1,    16,   118,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      -1,    60,    61,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,
      89,    -1,    91,    92,    -1,    -1,    -1,    96,    97,    98,
      99,   100,    -1,    50,    51,    -1,    -1,    54,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    65,   118,
      67,    68,    69,    70,    71,    72,    73,    -1,    -1,    76,
      -1,    -1,    -1,    80,    81,    82,    83,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,    50,    51,   120,    -1,    54,    -1,    -1,    -1,    -1,
      -1,    -1,   129,    -1,    -1,    64,    65,    -1,    67,    68,
      69,    70,    71,    72,    73,    -1,    -1,    76,    -1,    -1,
      -1,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,    50,
      51,   120,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,
     129,    -1,    -1,    64,    65,    -1,    67,    68,    69,    70,
      71,    72,    73,    -1,    -1,    76,    -1,    -1,    -1,    80,
      81,    82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,    50,    51,   120,
      -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,   129,    -1,
      -1,    64,    65,    -1,    67,    68,    69,    70,    71,    72,
      73,    -1,    -1,    76,    -1,    -1,    -1,    80,    81,    82,
      83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,    50,    51,   120,    -1,    54,
      -1,    -1,    -1,    -1,    -1,    -1,   129,    -1,    -1,    64,
      65,    -1,    67,    68,    69,    70,    71,    72,    73,    -1,
      -1,    76,    -1,    -1,    -1,    80,    81,    82,    83,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    50,    51,   120,    -1,    54,    -1,    -1,
      -1,    -1,    -1,    -1,   129,    -1,    -1,    64,    65,    -1,
      67,    68,    69,    70,    71,    72,    73,    -1,    -1,    76,
      -1,    -1,    -1,    80,    81,    82,    83,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,    50,    51,   120,    -1,    54,    -1,    -1,    -1,    -1,
      -1,    -1,   129,    -1,    -1,    64,    65,    -1,    67,    68,
      69,    70,    71,    72,    73,    -1,    -1,    76,    -1,    -1,
      -1,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,    50,
      51,   120,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,
     129,    -1,    -1,    64,    65,    -1,    67,    68,    69,    70,
      71,    72,    73,    -1,    -1,    76,    -1,    -1,    -1,    80,
      81,    82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,    50,    51,   120,
      -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,   129,    -1,
      -1,    64,    65,    -1,    67,    68,    69,    70,    71,    72,
      73,    -1,    -1,    76,    -1,    -1,    -1,    80,    81,    82,
      83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,    50,    51,   120,    -1,    54,
      -1,    -1,    -1,    -1,    -1,    -1,   129,    -1,    -1,    64,
      65,    -1,    67,    68,    69,    70,    71,    72,    73,    -1,
      -1,    76,    -1,    -1,    -1,    80,    81,    82,    83,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    50,    51,   120,    -1,    54,    -1,    -1,
      -1,    -1,    -1,    -1,   129,    -1,    -1,    64,    65,    -1,
      67,    68,    69,    70,    71,    72,    73,    -1,    -1,    76,
      -1,    -1,    -1,    80,    81,    82,    83,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,    50,    51,   120,    -1,    54,    -1,    -1,    -1,    -1,
      -1,    -1,   129,    -1,    -1,    64,    65,    -1,    67,    68,
      69,    70,    71,    72,    73,    -1,    -1,    76,    -1,    -1,
      -1,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,    -1,
      -1,   120,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     129,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    -1,    -1,    82,    83,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     106,    -1,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   128,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    -1,    -1,    82,    83,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    -1,    -1,    82,    83,    -1,    -1,    -1,
      -1,    -1,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,    -1,    -1,    -1,    -1,    -1,   106,
      -1,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,    67,    68,    69,    70,    71,    72,    73,    74,    -1,
      76,    77,    -1,    -1,    -1,    -1,    82,    83,    67,    68,
      69,    70,    71,    72,    73,    -1,    -1,    76,    77,    -1,
      -1,    -1,    -1,    82,    83,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,    67,
      68,    69,    70,    71,    72,    73,    -1,    -1,    76,    77,
      -1,    -1,    -1,    -1,    82,    83,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
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
      51,    54,    64,    65,    67,    68,    69,    70,    71,    72,
      73,    76,    80,    81,    82,    83,    94,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   120,   129,   130,
     161,   165,   166,   248,   264,    32,    33,    34,    35,    48,
      49,    50,    54,    96,   161,   162,   163,   243,   186,    85,
     144,   145,   158,   202,   247,   249,   250,   145,   133,   134,
     145,   275,   278,   279,   189,   191,    85,   152,   158,   202,
     207,   247,   250,    63,    94,    95,   119,   151,   167,   170,
     174,   181,   183,   267,   268,   269,   270,   174,   174,   130,
     176,   177,   130,   172,   176,   144,    52,   162,   133,   276,
     143,   122,   238,   167,   202,   167,    50,    51,    54,   165,
     272,    54,    88,   139,   153,   154,   144,    94,   151,   171,
     183,   267,   280,   183,   266,   267,   280,    85,   157,   202,
     247,   250,    52,    53,    55,   161,   238,   244,   237,   238,
     132,   233,   132,   236,    57,    58,   146,   167,   167,   275,
     279,    39,    40,    41,    42,    43,    36,    37,    28,   209,
     105,   128,    88,    94,   155,   105,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    82,
      83,   106,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,    84,   124,   126,   131,    84,   126,    26,   122,
     214,   229,    86,    86,   172,   176,   214,   276,   144,    50,
      54,   159,    57,    58,     1,   109,   251,   278,    84,   124,
     126,   198,   265,   199,    84,   126,   274,   128,   138,   139,
      54,    13,   107,   203,   278,   105,    84,   124,   126,    86,
      86,   203,   275,    15,    17,   219,   134,   145,   145,    54,
      84,   124,   126,    25,   170,   170,   170,    87,   128,   182,
     280,   128,   182,   127,   174,    89,   174,   178,   151,   174,
     183,   212,   280,    52,    60,    61,   142,   130,   168,   122,
     139,    59,   101,   102,   103,   239,    84,   126,    86,   153,
     127,   127,   187,   167,   276,   125,   128,   133,   277,   128,
     277,   128,   277,   123,   277,    54,    84,   124,   126,   103,
     103,   239,    62,   103,   103,   234,   239,   103,    59,   103,
      67,    67,   141,   145,   145,   145,   145,   141,   144,   144,
     210,    94,   146,   170,   183,   184,   153,   157,   128,   146,
     167,   170,   184,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   169,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,    50,
      51,    54,   165,   272,   273,   171,    50,    54,   272,   272,
     272,   272,   217,   215,   146,   167,   146,   167,    93,   148,
     196,   278,   252,   195,    50,    54,   159,   272,   171,   272,
     138,   144,    50,    52,    53,    54,    55,    94,    95,   112,
     115,   130,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   200,   164,    10,     8,   222,   280,   139,    13,
     167,    50,    54,   171,    50,    54,   139,   219,   139,    94,
     183,   220,    10,    27,   107,   204,   278,   204,    50,    54,
     171,    50,    54,   193,   128,   182,   170,    94,   170,   181,
     267,    94,   170,   268,   127,    94,   167,   170,   175,   179,
     181,   267,   276,   128,    84,   126,   276,   127,   162,   188,
     167,   139,   123,   241,   240,   167,   127,   276,   276,    94,
     170,    50,    54,   171,    50,    54,   132,   239,   132,   167,
     167,    75,   110,   208,   280,   170,   128,   127,    43,   167,
      86,    86,   172,   176,   125,    86,    86,   172,   173,   176,
     280,   173,   176,   173,   176,   208,   208,   149,   278,   145,
     138,   125,    10,   276,   105,   254,   138,   278,   128,   263,
     280,   128,   263,    50,   128,   263,    50,   161,   162,   170,
     184,   223,   280,    15,   206,   280,    14,   205,   206,    86,
     125,    86,    86,   206,    10,    10,   170,   128,   203,   190,
     192,   125,   145,   170,   128,   182,   170,   170,   128,   180,
     127,   128,   182,   127,   151,   212,   272,   272,   127,   144,
     123,   139,    52,    53,    55,   242,   250,   127,   167,   125,
     110,   207,   211,    94,   170,   167,   107,   146,   167,   167,
     148,    86,   146,   167,   146,   167,   148,   218,   216,   208,
     197,   278,    10,   127,   170,   276,    10,   255,   258,   260,
     262,    50,   257,   260,   201,    87,   224,   280,   139,     9,
     225,   280,   145,    10,    86,    10,    94,   139,   139,   139,
     204,   182,    94,   182,   182,    94,   181,   183,   267,   127,
      94,   276,   127,   123,   276,   110,   139,   170,   167,   146,
     167,   139,   139,   150,   138,   127,   128,   263,   263,   263,
     253,    85,   158,   202,   247,   250,   203,   139,   203,   170,
     206,   219,   221,    10,    10,   194,   167,   170,   128,   182,
     128,   182,   170,   127,    10,    10,   123,   139,    10,   260,
     138,    54,    84,   124,   126,   139,   139,   139,   182,   182,
      94,   267,    94,   182,   123,   263,    10,    50,    54,   171,
      50,    54,   222,   205,    10,   170,   128,   182,   170,   125,
     182,    94,   182,   170,   182
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

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
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)           \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX	yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX	yylex (&yylval)
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
} while (0)
# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*-----------------------------.
| Print this symbol on YYOUT.  |
`-----------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yysymprint (FILE* yyout, int yytype, YYSTYPE yyvalue)
#else
yysymprint (yyout, yytype, yyvalue)
    FILE* yyout;
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyout, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyout, yytoknum[yytype], yyvalue);
# endif
    }
  else
    YYFPRINTF (yyout, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyout, ")");
}
#endif /* YYDEBUG. */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yydestruct (int yytype, YYSTYPE yyvalue)
#else
yydestruct (yytype, yyvalue)
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  switch (yytype)
    {
      default:
        break;
    }
}



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif




int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of parse errors so far.  */
int yynerrs;

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

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
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
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

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with.  */

  if (yychar <= 0)		/* This means end of input.  */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more.  */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

      /* We have to keep this `#if YYDEBUG', since we use variables
	 which are defined only if `YYDEBUG' is set.  */
      YYDPRINTF ((stderr, "Next token is "));
      YYDSYMPRINT ((stderr, yychar1, yylval));
      YYDPRINTF ((stderr, "\n"));
    }

  /* If the proper action on seeing token YYCHAR1 is to reduce or to
     detect an error, take that action.  */
  yyn += yychar1;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yychar1)
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

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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



#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn - 1, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] >= 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif
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
                    }
    break;

  case 3:
#line 421 "grammar.y"
    {
                        if (yyvsp[0].node && !compile_for_eval) {
                            /* last expression should not be void */
                            if (nd_type(yyvsp[0].node) != NODE_BLOCK) void_expr(yyvsp[0].node);
                            else {
                                NODE *node = yyvsp[0].node;
                                while (node->nd_next) {
                                    node = node->nd_next;
                                }
                                void_expr(node->nd_head);
                            }
                        }
                        vps->top = block_append(parse_state, vps->top, yyvsp[0].node); 
                        //vps->top = NEW_SCOPE(block_append(vps->top, )); 
                        // rb_funcall(vps->self, rb_intern("local_finish"), 0);
                        
                        // top_local_setup();
                        class_nest = 0;
                    }
    break;

  case 4:
#line 446 "grammar.y"
    {
                        yyval.node = yyvsp[-3].node;
                        if (yyvsp[-2].node) {
                            yyval.node = NEW_RESCUE(yyvsp[-3].node, yyvsp[-2].node, yyvsp[-1].node);
                        }
                        else if (yyvsp[-1].node) {
                            rb_warn("else without rescue is useless");
                            yyval.node = block_append(parse_state, yyval.node, yyvsp[-1].node);
                        }
                        if (yyvsp[0].node) {
                            yyval.node = NEW_ENSURE(yyval.node, yyvsp[0].node);
                        }
                        fixpos(yyval.node, yyvsp[-3].node);
                    }
    break;

  case 5:
#line 463 "grammar.y"
    {
                        void_stmts(yyvsp[-1].node, parse_state);
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 7:
#line 471 "grammar.y"
    {
                        yyval.node = newline_node(parse_state, yyvsp[0].node);
                    }
    break;

  case 8:
#line 475 "grammar.y"
    {
                        yyval.node = block_append(parse_state, yyvsp[-2].node, newline_node(parse_state, yyvsp[0].node));
                    }
    break;

  case 9:
#line 479 "grammar.y"
    {
                        yyval.node = yyvsp[0].node;
                    }
    break;

  case 10:
#line 484 "grammar.y"
    {vps->lex_state = EXPR_FNAME;}
    break;

  case 11:
#line 485 "grammar.y"
    {
                        yyval.node = NEW_ALIAS(yyvsp[-2].id, yyvsp[0].id);
                    }
    break;

  case 12:
#line 489 "grammar.y"
    {
                        yyval.node = NEW_VALIAS(yyvsp[-1].id, yyvsp[0].id);
                    }
    break;

  case 13:
#line 493 "grammar.y"
    {
                        char buf[3];

                        sprintf(buf, "$%c", (char)yyvsp[0].node->nd_nth);
                        yyval.node = NEW_VALIAS(yyvsp[-1].id, rb_intern(buf));
                    }
    break;

  case 14:
#line 500 "grammar.y"
    {
                        yyerror("can't make alias for the number variables");
                        yyval.node = 0;
                    }
    break;

  case 15:
#line 505 "grammar.y"
    {
                        yyval.node = yyvsp[0].node;
                    }
    break;

  case 16:
#line 509 "grammar.y"
    {
                        yyval.node = NEW_IF(cond(yyvsp[0].node, parse_state), yyvsp[-2].node, 0);
                        fixpos(yyval.node, yyvsp[0].node);
                        if (cond_negative(&yyval.node->nd_cond)) {
                            yyval.node->nd_else = yyval.node->nd_body;
                            yyval.node->nd_body = 0;
                        }
                    }
    break;

  case 17:
#line 518 "grammar.y"
    {
                        yyval.node = NEW_UNLESS(cond(yyvsp[0].node, parse_state), yyvsp[-2].node, 0);
                        fixpos(yyval.node, yyvsp[0].node);
                        if (cond_negative(&yyval.node->nd_cond)) {
                            yyval.node->nd_body = yyval.node->nd_else;
                            yyval.node->nd_else = 0;
                        }
                    }
    break;

  case 18:
#line 527 "grammar.y"
    {
                        if (yyvsp[-2].node && nd_type(yyvsp[-2].node) == NODE_BEGIN) {
                            yyval.node = NEW_WHILE(cond(yyvsp[0].node, parse_state), yyvsp[-2].node->nd_body, 0);
                        }
                        else {
                            yyval.node = NEW_WHILE(cond(yyvsp[0].node, parse_state), yyvsp[-2].node, 1);
                        }
                        if (cond_negative(&yyval.node->nd_cond)) {
                            nd_set_type(yyval.node, NODE_UNTIL);
                        }
                    }
    break;

  case 19:
#line 539 "grammar.y"
    {
                        if (yyvsp[-2].node && nd_type(yyvsp[-2].node) == NODE_BEGIN) {
                            yyval.node = NEW_UNTIL(cond(yyvsp[0].node, parse_state), yyvsp[-2].node->nd_body, 0);
                        }
                        else {
                            yyval.node = NEW_UNTIL(cond(yyvsp[0].node, parse_state), yyvsp[-2].node, 1);
                        }
                        if (cond_negative(&yyval.node->nd_cond)) {
                            nd_set_type(yyval.node, NODE_WHILE);
                        }
                    }
    break;

  case 20:
#line 551 "grammar.y"
    {
                        yyval.node = NEW_RESCUE(yyvsp[-2].node, NEW_RESBODY(0,yyvsp[0].node,0), 0);
                    }
    break;

  case 21:
#line 555 "grammar.y"
    {
                        if (in_def || in_single) {
                            yyerror("BEGIN in method");
                        }
                        local_push(0);
                    }
    break;

  case 22:
#line 562 "grammar.y"
    {
                        /*
                        ruby_eval_tree_begin = block_append(ruby_eval_tree_begin,
                                                            NEW_PREEXE());
                        */
                        local_pop();
                        yyval.node = 0;
                    }
    break;

  case 23:
#line 571 "grammar.y"
    {
                        if (in_def || in_single) {
                            rb_warn("END in method; use at_exit");
                        }

                        yyval.node = NEW_ITER(0, NEW_POSTEXE(), yyvsp[-1].node);
                    }
    break;

  case 24:
#line 579 "grammar.y"
    {
                        yyval.node = node_assign(yyvsp[-2].node, yyvsp[0].node, parse_state);
                    }
    break;

  case 25:
#line 583 "grammar.y"
    {
                        value_expr(yyvsp[0].node);
                        yyvsp[-2].node->nd_value = (yyvsp[-2].node->nd_head) ? NEW_TO_ARY(yyvsp[0].node) : NEW_ARRAY(yyvsp[0].node);
                        yyval.node = yyvsp[-2].node;
                    }
    break;

  case 26:
#line 589 "grammar.y"
    {
                        value_expr(yyvsp[0].node);
                        if (yyvsp[-2].node) {
                            ID vid = yyvsp[-2].node->nd_vid;
                            if (yyvsp[-1].id == tOROP) {
                                yyvsp[-2].node->nd_value = yyvsp[0].node;
                                yyval.node = NEW_OP_ASGN_OR(gettable(vid), yyvsp[-2].node);
                                if (is_asgn_or_id(vid)) {
                                    yyval.node->nd_aid = vid;
                                }
                            }
                            else if (yyvsp[-1].id == tANDOP) {
                                yyvsp[-2].node->nd_value = yyvsp[0].node;
                                yyval.node = NEW_OP_ASGN_AND(gettable(vid), yyvsp[-2].node);
                            }
                            else {
                                yyval.node = yyvsp[-2].node;
                                yyval.node->nd_value = call_op(gettable(vid),yyvsp[-1].id,1,yyvsp[0].node, parse_state);
                            }
                        }
                        else {
                            yyval.node = 0;
                        }
                    }
    break;

  case 27:
#line 614 "grammar.y"
    {
                        NODE *args;

                        value_expr(yyvsp[0].node);
                        args = NEW_LIST(yyvsp[0].node);
                        if (yyvsp[-3].node && nd_type(yyvsp[-3].node) != NODE_ARRAY)
                            yyvsp[-3].node = NEW_LIST(yyvsp[-3].node);
                        yyvsp[-3].node = list_append(parse_state, yyvsp[-3].node, NEW_NIL());
                        list_concat(args, yyvsp[-3].node);
                        if (yyvsp[-1].id == tOROP) {
                            yyvsp[-1].id = 0;
                        }
                        else if (yyvsp[-1].id == tANDOP) {
                            yyvsp[-1].id = 1;
                        }
                        yyval.node = NEW_OP_ASGN1(yyvsp[-5].node, yyvsp[-1].id, args);
                        fixpos(yyval.node, yyvsp[-5].node);
                    }
    break;

  case 28:
#line 633 "grammar.y"
    {
                        value_expr(yyvsp[0].node);
                        if (yyvsp[-1].id == tOROP) {
                            yyvsp[-1].id = 0;
                        }
                        else if (yyvsp[-1].id == tANDOP) {
                            yyvsp[-1].id = 1;
                        }
                        yyval.node = NEW_OP_ASGN2(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-4].node);
                    }
    break;

  case 29:
#line 645 "grammar.y"
    {
                        value_expr(yyvsp[0].node);
                        if (yyvsp[-1].id == tOROP) {
                            yyvsp[-1].id = 0;
                        }
                        else if (yyvsp[-1].id == tANDOP) {
                            yyvsp[-1].id = 1;
                        }
                        yyval.node = NEW_OP_ASGN2(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-4].node);
                    }
    break;

  case 30:
#line 657 "grammar.y"
    {
                        value_expr(yyvsp[0].node);
                        if (yyvsp[-1].id == tOROP) {
                            yyvsp[-1].id = 0;
                        }
                        else if (yyvsp[-1].id == tANDOP) {
                            yyvsp[-1].id = 1;
                        }
                        yyval.node = NEW_OP_ASGN2(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-4].node);
                    }
    break;

  case 31:
#line 669 "grammar.y"
    {
                        rb_backref_error(yyvsp[-2].node);
                        yyval.node = 0;
                    }
    break;

  case 32:
#line 674 "grammar.y"
    {
                        yyval.node = node_assign(yyvsp[-2].node, NEW_SVALUE(yyvsp[0].node), parse_state);
                    }
    break;

  case 33:
#line 678 "grammar.y"
    {
                        yyvsp[-2].node->nd_value = (yyvsp[-2].node->nd_head) ? NEW_TO_ARY(yyvsp[0].node) : NEW_ARRAY(yyvsp[0].node);
                        yyval.node = yyvsp[-2].node;
                    }
    break;

  case 34:
#line 683 "grammar.y"
    {
                        yyvsp[-2].node->nd_value = yyvsp[0].node;
                        yyval.node = yyvsp[-2].node;
                    }
    break;

  case 37:
#line 692 "grammar.y"
    {
                        yyval.node = logop(NODE_AND, yyvsp[-2].node, yyvsp[0].node, parse_state);
                    }
    break;

  case 38:
#line 696 "grammar.y"
    {
                        yyval.node = logop(NODE_OR, yyvsp[-2].node, yyvsp[0].node, parse_state);
                    }
    break;

  case 39:
#line 700 "grammar.y"
    {
                        yyval.node = NEW_NOT(cond(yyvsp[0].node, parse_state));
                    }
    break;

  case 40:
#line 704 "grammar.y"
    {
                        yyval.node = NEW_NOT(cond(yyvsp[0].node, parse_state));
                    }
    break;

  case 42:
#line 711 "grammar.y"
    {
                        value_expr(yyval.node);
                        yyval.node = yyvsp[0].node;
                    }
    break;

  case 45:
#line 720 "grammar.y"
    {
                        yyval.node = NEW_RETURN(ret_args(vps, yyvsp[0].node));
                    }
    break;

  case 46:
#line 724 "grammar.y"
    {
                        yyval.node = NEW_BREAK(ret_args(vps, yyvsp[0].node));
                    }
    break;

  case 47:
#line 728 "grammar.y"
    {
                        yyval.node = NEW_NEXT(ret_args(vps, yyvsp[0].node));
                    }
    break;

  case 49:
#line 735 "grammar.y"
    {
                        yyval.node = new_call(parse_state, yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                    }
    break;

  case 50:
#line 739 "grammar.y"
    {
                        yyval.node = new_call(parse_state, yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                    }
    break;

  case 51:
#line 745 "grammar.y"
    {
                        yyvsp[0].num = ruby_sourceline;
                        reset_block(vps);
                    }
    break;

  case 52:
#line 749 "grammar.y"
    { yyval.vars = vps->block_vars; }
    break;

  case 53:
#line 752 "grammar.y"
    {
                        yyval.node = NEW_ITER(yyvsp[-3].node, 0, extract_block_vars(vps, yyvsp[-1].node, yyvsp[-2].vars));
                        nd_set_line(yyval.node, yyvsp[-5].num);
                    }
    break;

  case 54:
#line 759 "grammar.y"
    {
                        yyval.node = new_fcall(parse_state, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[0].node);
                   }
    break;

  case 55:
#line 764 "grammar.y"
    {
                        yyval.node = new_fcall(parse_state, yyvsp[-2].id, yyvsp[-1].node);
                        if (yyvsp[0].node) {
                            if (nd_type(yyval.node) == NODE_BLOCK_PASS) {
                                rb_compile_error("both block arg and actual block given");
                            }
                            yyvsp[0].node->nd_iter = yyval.node;
                            yyval.node = yyvsp[0].node;
                        }
                        fixpos(yyval.node, yyvsp[-1].node);
                   }
    break;

  case 56:
#line 776 "grammar.y"
    {
                        yyval.node = new_call(parse_state, yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-3].node);
                    }
    break;

  case 57:
#line 781 "grammar.y"
    {
                        yyval.node = new_call(parse_state, yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].node);
                        if (yyvsp[0].node) {
                            if (nd_type(yyval.node) == NODE_BLOCK_PASS) {
                                rb_compile_error("both block arg and actual block given");
                            }
                            yyvsp[0].node->nd_iter = yyval.node;
                            yyval.node = yyvsp[0].node;
                        }
                        fixpos(yyval.node, yyvsp[-4].node);
                   }
    break;

  case 58:
#line 793 "grammar.y"
    {
                        yyval.node = new_call(parse_state, yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-3].node);
                    }
    break;

  case 59:
#line 798 "grammar.y"
    {
                        yyval.node = new_call(parse_state, yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].node);
                        if (yyvsp[0].node) {
                            if (nd_type(yyval.node) == NODE_BLOCK_PASS) {
                                rb_compile_error("both block arg and actual block given");
                            }
                            yyvsp[0].node->nd_iter = yyval.node;
                            yyval.node = yyvsp[0].node;
                        }
                        fixpos(yyval.node, yyvsp[-4].node);
                   }
    break;

  case 60:
#line 810 "grammar.y"
    {
                        yyval.node = new_super(parse_state, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[0].node);
                    }
    break;

  case 61:
#line 815 "grammar.y"
    {
                        yyval.node = new_yield(parse_state, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[0].node);
                    }
    break;

  case 63:
#line 823 "grammar.y"
    {
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 65:
#line 830 "grammar.y"
    {
                        yyval.node = NEW_MASGN(NEW_LIST(yyvsp[-1].node), 0);
                    }
    break;

  case 66:
#line 836 "grammar.y"
    {
                        yyval.node = NEW_MASGN(yyvsp[0].node, 0);
                    }
    break;

  case 67:
#line 840 "grammar.y"
    {
                        yyval.node = NEW_MASGN(list_append(parse_state, yyvsp[-1].node,yyvsp[0].node), 0);
                    }
    break;

  case 68:
#line 844 "grammar.y"
    {
                        yyval.node = NEW_MASGN(yyvsp[-2].node, yyvsp[0].node);
                    }
    break;

  case 69:
#line 848 "grammar.y"
    {
                        yyval.node = NEW_MASGN(yyvsp[-1].node, -1);
                    }
    break;

  case 70:
#line 852 "grammar.y"
    {
                        yyval.node = NEW_MASGN(0, yyvsp[0].node);
                    }
    break;

  case 71:
#line 856 "grammar.y"
    {
                        yyval.node = NEW_MASGN(0, -1);
                    }
    break;

  case 73:
#line 863 "grammar.y"
    {
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 74:
#line 869 "grammar.y"
    {
                        yyval.node = NEW_LIST(yyvsp[-1].node);
                    }
    break;

  case 75:
#line 873 "grammar.y"
    {
                        yyval.node = list_append(parse_state, yyvsp[-2].node, yyvsp[-1].node);
                    }
    break;

  case 76:
#line 879 "grammar.y"
    {
                        yyval.node = assignable(yyvsp[0].id, 0, parse_state);
                    }
    break;

  case 77:
#line 883 "grammar.y"
    {
                        yyval.node = aryset(yyvsp[-3].node, yyvsp[-1].node, parse_state);
                    }
    break;

  case 78:
#line 887 "grammar.y"
    {
                        yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id, parse_state);
                    }
    break;

  case 79:
#line 891 "grammar.y"
    {
                        yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id, parse_state);
                    }
    break;

  case 80:
#line 895 "grammar.y"
    {
                        yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id, parse_state);
                    }
    break;

  case 81:
#line 899 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        yyval.node = NEW_CDECL(0, 0, NEW_COLON2(yyvsp[-2].node, yyvsp[0].id));
                    }
    break;

  case 82:
#line 905 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        yyval.node = NEW_CDECL(0, 0, NEW_COLON3(yyvsp[0].id));
                    }
    break;

  case 83:
#line 911 "grammar.y"
    {
                        rb_backref_error(yyvsp[0].node);
                        yyval.node = 0;
                    }
    break;

  case 84:
#line 918 "grammar.y"
    {
                        yyval.node = assignable(yyvsp[0].id, 0, parse_state);
                    }
    break;

  case 85:
#line 922 "grammar.y"
    {
                        yyval.node = aryset(yyvsp[-3].node, yyvsp[-1].node, parse_state);
                    }
    break;

  case 86:
#line 926 "grammar.y"
    {
                        yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id, parse_state);
                    }
    break;

  case 87:
#line 930 "grammar.y"
    {
                        yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id, parse_state);
                    }
    break;

  case 88:
#line 934 "grammar.y"
    {
                        yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id, parse_state);
                    }
    break;

  case 89:
#line 938 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        yyval.node = NEW_CDECL(0, 0, NEW_COLON2(yyvsp[-2].node, yyvsp[0].id));
                    }
    break;

  case 90:
#line 944 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        yyval.node = NEW_CDECL(0, 0, NEW_COLON3(yyvsp[0].id));
                    }
    break;

  case 91:
#line 950 "grammar.y"
    {
                        rb_backref_error(yyvsp[0].node);
                        yyval.node = 0;
                    }
    break;

  case 92:
#line 957 "grammar.y"
    {
                        yyerror("class/module name must be CONSTANT");
                    }
    break;

  case 94:
#line 964 "grammar.y"
    {
                        yyval.node = NEW_COLON3(yyvsp[0].id);
                    }
    break;

  case 95:
#line 968 "grammar.y"
    {
                        yyval.node = NEW_COLON2(0, yyval.node);
                    }
    break;

  case 96:
#line 972 "grammar.y"
    {
                        yyval.node = NEW_COLON2(yyvsp[-2].node, yyvsp[0].id);
                    }
    break;

  case 100:
#line 981 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        yyval.id = convert_op(yyvsp[0].id);
                    }
    break;

  case 101:
#line 986 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        yyval.id = yyvsp[0].id;
                    }
    break;

  case 104:
#line 997 "grammar.y"
    {
                        yyval.node = NEW_UNDEF(yyvsp[0].id);
                    }
    break;

  case 105:
#line 1000 "grammar.y"
    {vps->lex_state = EXPR_FNAME;}
    break;

  case 106:
#line 1001 "grammar.y"
    {
                        yyval.node = block_append(parse_state, yyvsp[-3].node, NEW_UNDEF(yyvsp[0].id));
                    }
    break;

  case 107:
#line 1006 "grammar.y"
    { yyval.id = '|'; }
    break;

  case 108:
#line 1007 "grammar.y"
    { yyval.id = '^'; }
    break;

  case 109:
#line 1008 "grammar.y"
    { yyval.id = '&'; }
    break;

  case 110:
#line 1009 "grammar.y"
    { yyval.id = tCMP; }
    break;

  case 111:
#line 1010 "grammar.y"
    { yyval.id = tEQ; }
    break;

  case 112:
#line 1011 "grammar.y"
    { yyval.id = tNEQ; }
    break;

  case 113:
#line 1012 "grammar.y"
    { yyval.id = tEQQ; }
    break;

  case 114:
#line 1013 "grammar.y"
    { yyval.id = tMATCH; }
    break;

  case 115:
#line 1014 "grammar.y"
    { yyval.id = '>'; }
    break;

  case 116:
#line 1015 "grammar.y"
    { yyval.id = tGEQ; }
    break;

  case 117:
#line 1016 "grammar.y"
    { yyval.id = '<'; }
    break;

  case 118:
#line 1017 "grammar.y"
    { yyval.id = tLEQ; }
    break;

  case 119:
#line 1018 "grammar.y"
    { yyval.id = tLSHFT; }
    break;

  case 120:
#line 1019 "grammar.y"
    { yyval.id = tRSHFT; }
    break;

  case 121:
#line 1020 "grammar.y"
    { yyval.id = '+'; }
    break;

  case 122:
#line 1021 "grammar.y"
    { yyval.id = '-'; }
    break;

  case 123:
#line 1022 "grammar.y"
    { yyval.id = '*'; }
    break;

  case 124:
#line 1023 "grammar.y"
    { yyval.id = '*'; }
    break;

  case 125:
#line 1024 "grammar.y"
    { yyval.id = '/'; }
    break;

  case 126:
#line 1025 "grammar.y"
    { yyval.id = '%'; }
    break;

  case 127:
#line 1026 "grammar.y"
    { yyval.id = tPOW; }
    break;

  case 128:
#line 1027 "grammar.y"
    { yyval.id = '~'; }
    break;

  case 129:
#line 1028 "grammar.y"
    { yyval.id = tUPLUS; }
    break;

  case 130:
#line 1029 "grammar.y"
    { yyval.id = tUMINUS; }
    break;

  case 131:
#line 1030 "grammar.y"
    { yyval.id = tAREF; }
    break;

  case 132:
#line 1031 "grammar.y"
    { yyval.id = tASET; }
    break;

  case 133:
#line 1032 "grammar.y"
    { yyval.id = '`'; }
    break;

  case 175:
#line 1045 "grammar.y"
    {
                        yyval.node = node_assign(yyvsp[-2].node, yyvsp[0].node, parse_state);
                    }
    break;

  case 176:
#line 1049 "grammar.y"
    {
                        yyval.node = node_assign(yyvsp[-4].node, NEW_RESCUE(yyvsp[-2].node, NEW_RESBODY(0,yyvsp[0].node,0), 0), parse_state);
                    }
    break;

  case 177:
#line 1053 "grammar.y"
    {
                        value_expr(yyvsp[0].node);
                        if (yyvsp[-2].node) {
                            ID vid = yyvsp[-2].node->nd_vid;
                            if (yyvsp[-1].id == tOROP) {
                                yyvsp[-2].node->nd_value = yyvsp[0].node;
                                yyval.node = NEW_OP_ASGN_OR(gettable(vid), yyvsp[-2].node);
                                if (is_asgn_or_id(vid)) {
                                    yyval.node->nd_aid = vid;
                                }
                            }
                            else if (yyvsp[-1].id == tANDOP) {
                                yyvsp[-2].node->nd_value = yyvsp[0].node;
                                yyval.node = NEW_OP_ASGN_AND(gettable(vid), yyvsp[-2].node);
                            }
                            else {
                                yyval.node = yyvsp[-2].node;
                                yyval.node->nd_value = call_op(gettable(vid),yyvsp[-1].id,1,yyvsp[0].node, parse_state);
                            }
                        }
                        else {
                            yyval.node = 0;
                        }
                    }
    break;

  case 178:
#line 1078 "grammar.y"
    {
                        NODE *args;

                        value_expr(yyvsp[0].node);
                        args = NEW_LIST(yyvsp[0].node);
                        if (yyvsp[-3].node && nd_type(yyvsp[-3].node) != NODE_ARRAY)
                            yyvsp[-3].node = NEW_LIST(yyvsp[-3].node);
                        yyvsp[-3].node = list_append(parse_state, yyvsp[-3].node, NEW_NIL());
                        list_concat(args, yyvsp[-3].node);
                        if (yyvsp[-1].id == tOROP) {
                            yyvsp[-1].id = 0;
                        }
                        else if (yyvsp[-1].id == tANDOP) {
                            yyvsp[-1].id = 1;
                        }
                        yyval.node = NEW_OP_ASGN1(yyvsp[-5].node, yyvsp[-1].id, args);
                        fixpos(yyval.node, yyvsp[-5].node);
                    }
    break;

  case 179:
#line 1097 "grammar.y"
    {
                        value_expr(yyvsp[0].node);
                        if (yyvsp[-1].id == tOROP) {
                            yyvsp[-1].id = 0;
                        }
                        else if (yyvsp[-1].id == tANDOP) {
                            yyvsp[-1].id = 1;
                        }
                        yyval.node = NEW_OP_ASGN2(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-4].node);
                    }
    break;

  case 180:
#line 1109 "grammar.y"
    {
                        value_expr(yyvsp[0].node);
                        if (yyvsp[-1].id == tOROP) {
                            yyvsp[-1].id = 0;
                        }
                        else if (yyvsp[-1].id == tANDOP) {
                            yyvsp[-1].id = 1;
                        }
                        yyval.node = NEW_OP_ASGN2(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-4].node);
                    }
    break;

  case 181:
#line 1121 "grammar.y"
    {
                        value_expr(yyvsp[0].node);
                        if (yyvsp[-1].id == tOROP) {
                            yyvsp[-1].id = 0;
                        }
                        else if (yyvsp[-1].id == tANDOP) {
                            yyvsp[-1].id = 1;
                        }
                        yyval.node = NEW_OP_ASGN2(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-4].node);
                    }
    break;

  case 182:
#line 1133 "grammar.y"
    {
                        yyerror("constant re-assignment");
                        yyval.node = 0;
                    }
    break;

  case 183:
#line 1138 "grammar.y"
    {
                        yyerror("constant re-assignment");
                        yyval.node = 0;
                    }
    break;

  case 184:
#line 1143 "grammar.y"
    {
                        rb_backref_error(yyvsp[-2].node);
                        yyval.node = 0;
                    }
    break;

  case 185:
#line 1148 "grammar.y"
    {
                        value_expr(yyvsp[-2].node);
                        value_expr(yyvsp[0].node);
                        yyval.node = NEW_DOT2(yyvsp[-2].node, yyvsp[0].node);
                    }
    break;

  case 186:
#line 1154 "grammar.y"
    {
                        value_expr(yyvsp[-2].node);
                        value_expr(yyvsp[0].node);
                        yyval.node = NEW_DOT3(yyvsp[-2].node, yyvsp[0].node);
                    }
    break;

  case 187:
#line 1160 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, '+', 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 188:
#line 1164 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, '-', 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 189:
#line 1168 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, '*', 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 190:
#line 1172 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, '/', 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 191:
#line 1176 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, '%', 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 192:
#line 1180 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, tPOW, 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 193:
#line 1184 "grammar.y"
    {
                        yyval.node = call_op(call_op(yyvsp[-2].node, tPOW, 1, yyvsp[0].node, parse_state), tUMINUS, 0, 0, parse_state);
                    }
    break;

  case 194:
#line 1188 "grammar.y"
    {
                        yyval.node = call_op(call_op(yyvsp[-2].node, tPOW, 1, yyvsp[0].node, parse_state), tUMINUS, 0, 0, parse_state);
                    }
    break;

  case 195:
#line 1192 "grammar.y"
    {
                        if (yyvsp[0].node && nd_type(yyvsp[0].node) == NODE_LIT) {
                            yyval.node = yyvsp[0].node;
                        }
                        else {
                            yyval.node = call_op(yyvsp[0].node, tUPLUS, 0, 0, parse_state);
                        }
                    }
    break;

  case 196:
#line 1201 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[0].node, tUMINUS, 0, 0, parse_state);
                    }
    break;

  case 197:
#line 1205 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, '|', 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 198:
#line 1209 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, '^', 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 199:
#line 1213 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, '&', 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 200:
#line 1217 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, tCMP, 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 201:
#line 1221 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, '>', 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 202:
#line 1225 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, tGEQ, 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 203:
#line 1229 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, '<', 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 204:
#line 1233 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, tLEQ, 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 205:
#line 1237 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, tEQ, 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 206:
#line 1241 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, tEQQ, 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 207:
#line 1245 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, tNEQ, 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 208:
#line 1249 "grammar.y"
    {
                        yyval.node = match_gen(yyvsp[-2].node, yyvsp[0].node, parse_state);
                    }
    break;

  case 209:
#line 1253 "grammar.y"
    {
                        yyval.node = NEW_NOT(match_gen(yyvsp[-2].node, yyvsp[0].node, parse_state));
                    }
    break;

  case 210:
#line 1257 "grammar.y"
    {
                        yyval.node = NEW_NOT(cond(yyvsp[0].node, parse_state));
                    }
    break;

  case 211:
#line 1261 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[0].node, '~', 0, 0, parse_state);
                    }
    break;

  case 212:
#line 1265 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, tLSHFT, 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 213:
#line 1269 "grammar.y"
    {
                        yyval.node = call_op(yyvsp[-2].node, tRSHFT, 1, yyvsp[0].node, parse_state);
                    }
    break;

  case 214:
#line 1273 "grammar.y"
    {
                        yyval.node = logop(NODE_AND, yyvsp[-2].node, yyvsp[0].node, parse_state);
                    }
    break;

  case 215:
#line 1277 "grammar.y"
    {
                        yyval.node = logop(NODE_OR, yyvsp[-2].node, yyvsp[0].node, parse_state);
                    }
    break;

  case 216:
#line 1280 "grammar.y"
    {vps->in_defined = 1;}
    break;

  case 217:
#line 1281 "grammar.y"
    {
                        vps->in_defined = 0;
                        yyval.node = NEW_DEFINED(yyvsp[0].node);
                    }
    break;

  case 218:
#line 1285 "grammar.y"
    {vps->ternary_colon++;}
    break;

  case 219:
#line 1286 "grammar.y"
    {
                        yyval.node = NEW_IF(cond(yyvsp[-5].node, parse_state), yyvsp[-2].node, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-5].node);
                        vps->ternary_colon--;
                    }
    break;

  case 220:
#line 1292 "grammar.y"
    {
                        yyval.node = yyvsp[0].node;
                    }
    break;

  case 221:
#line 1298 "grammar.y"
    {
                        value_expr(yyvsp[0].node);
                        yyval.node = yyvsp[0].node;
                    }
    break;

  case 223:
#line 1306 "grammar.y"
    {
                        rb_warn("parenthesize argument(s) for future version");
                        yyval.node = NEW_LIST(yyvsp[-1].node);
                    }
    break;

  case 224:
#line 1311 "grammar.y"
    {
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 225:
#line 1315 "grammar.y"
    {
                        value_expr(yyvsp[-1].node);
                        yyval.node = arg_concat(parse_state, yyvsp[-4].node, yyvsp[-1].node);
                    }
    break;

  case 226:
#line 1320 "grammar.y"
    {
                        yyval.node = NEW_LIST(NEW_HASH(yyvsp[-1].node));
                    }
    break;

  case 227:
#line 1324 "grammar.y"
    {
                        value_expr(yyvsp[-1].node);
                        yyval.node = NEW_NEWLINE(NEW_SPLAT(yyvsp[-1].node));
                    }
    break;

  case 228:
#line 1331 "grammar.y"
    {
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 229:
#line 1335 "grammar.y"
    {
                        yyval.node = yyvsp[-2].node;
                    }
    break;

  case 230:
#line 1339 "grammar.y"
    {
                        rb_warn("parenthesize argument for future version");
                        yyval.node = NEW_LIST(yyvsp[-2].node);
                    }
    break;

  case 231:
#line 1344 "grammar.y"
    {
                        rb_warn("parenthesize argument for future version");
                        yyval.node = list_append(parse_state, yyvsp[-4].node, yyvsp[-2].node);
                    }
    break;

  case 234:
#line 1355 "grammar.y"
    {
                        rb_warn("parenthesize argument(s) for future version");
                        yyval.node = NEW_LIST(yyvsp[0].node);
                    }
    break;

  case 235:
#line 1360 "grammar.y"
    {
                        yyval.node = arg_blk_pass(yyvsp[-1].node, yyvsp[0].node);
                    }
    break;

  case 236:
#line 1364 "grammar.y"
    {
                        yyval.node = arg_concat(parse_state, yyvsp[-4].node, yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    }
    break;

  case 237:
#line 1369 "grammar.y"
    {
                        yyval.node = NEW_LIST(NEW_POSITIONAL(yyvsp[-1].node));
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    }
    break;

  case 238:
#line 1374 "grammar.y"
    {
                        yyval.node = arg_concat(parse_state, NEW_LIST(NEW_POSITIONAL(yyvsp[-4].node)), yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    }
    break;

  case 239:
#line 1379 "grammar.y"
    {
                        yyval.node = list_append(parse_state, yyvsp[-3].node, NEW_POSITIONAL(yyvsp[-1].node));
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    }
    break;

  case 240:
#line 1384 "grammar.y"
    {
                        value_expr(yyvsp[-1].node);
                        yyval.node = arg_concat(parse_state, list_append(parse_state, yyvsp[-6].node, NEW_POSITIONAL(yyvsp[-4].node)), yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    }
    break;

  case 241:
#line 1390 "grammar.y"
    {
                        yyval.node = arg_blk_pass(NEW_SPLAT(yyvsp[-1].node), yyvsp[0].node);
                    }
    break;

  case 243:
#line 1397 "grammar.y"
    {
                        yyval.node = arg_blk_pass(list_concat(NEW_LIST(yyvsp[-3].node),yyvsp[-1].node), yyvsp[0].node);
                    }
    break;

  case 244:
#line 1401 "grammar.y"
    {
                        yyval.node = arg_blk_pass(yyvsp[-2].node, yyvsp[0].node);
                    }
    break;

  case 245:
#line 1405 "grammar.y"
    {
                        yyval.node = arg_concat(parse_state, NEW_LIST(yyvsp[-4].node), yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    }
    break;

  case 246:
#line 1410 "grammar.y"
    {
            yyval.node = arg_concat(parse_state, list_concat(NEW_LIST(yyvsp[-6].node),yyvsp[-4].node), yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    }
    break;

  case 247:
#line 1415 "grammar.y"
    {
                        yyval.node = NEW_LIST(NEW_POSITIONAL(yyvsp[-1].node));
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    }
    break;

  case 248:
#line 1420 "grammar.y"
    {
                        yyval.node = arg_concat(parse_state, NEW_LIST(NEW_POSITIONAL(yyvsp[-4].node)), yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    }
    break;

  case 249:
#line 1425 "grammar.y"
    {
                        yyval.node = list_append(parse_state, NEW_LIST(yyvsp[-3].node), NEW_POSITIONAL(yyvsp[-1].node));
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    }
    break;

  case 250:
#line 1430 "grammar.y"
    {
                        yyval.node = list_append(parse_state, list_concat(NEW_LIST(yyvsp[-5].node),yyvsp[-3].node), NEW_POSITIONAL(yyvsp[-1].node));
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    }
    break;

  case 251:
#line 1435 "grammar.y"
    {
                        yyval.node = arg_concat(parse_state, list_append(parse_state, NEW_LIST(yyvsp[-6].node), NEW_POSITIONAL(yyvsp[-4].node)), yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    }
    break;

  case 252:
#line 1440 "grammar.y"
    {
                        yyval.node = arg_concat(parse_state, list_append(parse_state, list_concat(NEW_LIST(yyvsp[-8].node), yyvsp[-6].node), NEW_POSITIONAL(yyvsp[-4].node)), yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    }
    break;

  case 253:
#line 1445 "grammar.y"
    {
                        yyval.node = arg_blk_pass(NEW_SPLAT(yyvsp[-1].node), yyvsp[0].node);
                    }
    break;

  case 255:
#line 1451 "grammar.y"
    {
                        yyval.num = vps->cmdarg_stack;
                        CMDARG_PUSH(1);
                    }
    break;

  case 256:
#line 1456 "grammar.y"
    {
                        /* CMDARG_POP() */
                        vps->cmdarg_stack = yyvsp[-1].num;
                        yyval.node = yyvsp[0].node;
                    }
    break;

  case 258:
#line 1464 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;}
    break;

  case 259:
#line 1465 "grammar.y"
    {
                        rb_warn("don't put space before argument parentheses");
                        yyval.node = 0;
                    }
    break;

  case 260:
#line 1469 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;}
    break;

  case 261:
#line 1470 "grammar.y"
    {
                        rb_warn("don't put space before argument parentheses");
                        yyval.node = yyvsp[-2].node;
                    }
    break;

  case 262:
#line 1477 "grammar.y"
    {
                        yyval.node = NEW_BLOCK_PASS(yyvsp[0].node);
                    }
    break;

  case 263:
#line 1483 "grammar.y"
    {
                        yyval.node = yyvsp[0].node;
                    }
    break;

  case 265:
#line 1490 "grammar.y"
    {
                        yyval.node = NEW_LIST(yyvsp[0].node);
                    }
    break;

  case 266:
#line 1494 "grammar.y"
    {
                        yyval.node = list_append(parse_state, yyvsp[-2].node, yyvsp[0].node);
                    }
    break;

  case 267:
#line 1500 "grammar.y"
    {
                        yyval.node = list_append(parse_state, yyvsp[-2].node, yyvsp[0].node);
                    }
    break;

  case 268:
#line 1504 "grammar.y"
    {
                        yyval.node = arg_concat(parse_state, yyvsp[-3].node, yyvsp[0].node);
                    }
    break;

  case 269:
#line 1508 "grammar.y"
    {
                        yyval.node = NEW_SPLAT(yyvsp[0].node);
                    }
    break;

  case 278:
#line 1522 "grammar.y"
    {
                        yyval.node = NEW_FCALL(yyvsp[0].id, 0);
                    }
    break;

  case 279:
#line 1526 "grammar.y"
    {
                        yyvsp[0].num = ruby_sourceline;
                    }
    break;

  case 280:
#line 1531 "grammar.y"
    {
                        if (yyvsp[-1].node == NULL)
                            yyval.node = NEW_NIL();
                        else
                            yyval.node = NEW_BEGIN(yyvsp[-1].node);
                        nd_set_line(yyval.node, yyvsp[-3].num);
                    }
    break;

  case 281:
#line 1538 "grammar.y"
    {vps->lex_state = EXPR_ENDARG;}
    break;

  case 282:
#line 1539 "grammar.y"
    {
                        rb_warning("(...) interpreted as grouped expression");
                        yyval.node = yyvsp[-3].node;
                    }
    break;

  case 283:
#line 1544 "grammar.y"
    {
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 284:
#line 1548 "grammar.y"
    {
                        yyval.node = NEW_COLON2(yyvsp[-2].node, yyvsp[0].id);
                    }
    break;

  case 285:
#line 1552 "grammar.y"
    {
                        yyval.node = NEW_COLON3(yyvsp[0].id);
                    }
    break;

  case 286:
#line 1556 "grammar.y"
    {
                        if (yyvsp[-3].node && nd_type(yyvsp[-3].node) == NODE_SELF) {
                            yyval.node = NEW_FCALL(convert_op(tAREF), yyvsp[-1].node);
                        } else {
                            yyval.node = NEW_CALL(yyvsp[-3].node, convert_op(tAREF), yyvsp[-1].node);
                        }
                        fixpos(yyval.node, yyvsp[-3].node);
                    }
    break;

  case 287:
#line 1565 "grammar.y"
    {
                        if (yyvsp[-1].node == 0) {
                            yyval.node = NEW_ZARRAY(); /* zero length array*/
                        }
                        else {
                            yyval.node = yyvsp[-1].node;
                        }
                    }
    break;

  case 288:
#line 1574 "grammar.y"
    {
                        yyval.node = NEW_HASH(yyvsp[-1].node);
                    }
    break;

  case 289:
#line 1578 "grammar.y"
    {
                        yyval.node = NEW_RETURN(0);
                    }
    break;

  case 290:
#line 1582 "grammar.y"
    {
                        yyval.node = new_yield(parse_state, yyvsp[-1].node);
                    }
    break;

  case 291:
#line 1586 "grammar.y"
    {
                        yyval.node = NEW_YIELD(0, Qfalse);
                    }
    break;

  case 292:
#line 1590 "grammar.y"
    {
                        yyval.node = NEW_YIELD(0, Qfalse);
                    }
    break;

  case 293:
#line 1593 "grammar.y"
    {vps->in_defined = 1;}
    break;

  case 294:
#line 1594 "grammar.y"
    {
                        vps->in_defined = 0;
                        yyval.node = NEW_DEFINED(yyvsp[-1].node);
                    }
    break;

  case 295:
#line 1599 "grammar.y"
    {
                        yyvsp[0].node->nd_iter = NEW_FCALL(yyvsp[-1].id, 0);
                        yyval.node = yyvsp[0].node;
                        fixpos(yyvsp[0].node->nd_iter, yyvsp[0].node);
                    }
    break;

  case 297:
#line 1606 "grammar.y"
    {
                        if (yyvsp[-1].node && nd_type(yyvsp[-1].node) == NODE_BLOCK_PASS) {
                            rb_compile_error("both block arg and actual block given");
                        }
                        yyvsp[0].node->nd_iter = yyvsp[-1].node;
                        yyval.node = yyvsp[0].node;
                        fixpos(yyval.node, yyvsp[-1].node);
                    }
    break;

  case 298:
#line 1618 "grammar.y"
    {
                        yyval.node = NEW_IF(cond(yyvsp[-4].node, parse_state), yyvsp[-2].node, yyvsp[-1].node);
                        fixpos(yyval.node, yyvsp[-4].node);
                        if (cond_negative(&yyval.node->nd_cond)) {
                            NODE *tmp = yyval.node->nd_body;
                            yyval.node->nd_body = yyval.node->nd_else;
                            yyval.node->nd_else = tmp;
                        }
                    }
    break;

  case 299:
#line 1631 "grammar.y"
    {
                        yyval.node = NEW_UNLESS(cond(yyvsp[-4].node, parse_state), yyvsp[-2].node, yyvsp[-1].node);
                        fixpos(yyval.node, yyvsp[-4].node);
                        if (cond_negative(&yyval.node->nd_cond)) {
                            NODE *tmp = yyval.node->nd_body;
                            yyval.node->nd_body = yyval.node->nd_else;
                            yyval.node->nd_else = tmp;
                        }
                    }
    break;

  case 300:
#line 1640 "grammar.y"
    {COND_PUSH(1);}
    break;

  case 301:
#line 1640 "grammar.y"
    {COND_POP();}
    break;

  case 302:
#line 1643 "grammar.y"
    {
                        yyval.node = NEW_WHILE(cond(yyvsp[-4].node, parse_state), yyvsp[-1].node, 1);
                        fixpos(yyval.node, yyvsp[-4].node);
                        if (cond_negative(&yyval.node->nd_cond)) {
                            nd_set_type(yyval.node, NODE_UNTIL);
                        }
                    }
    break;

  case 303:
#line 1650 "grammar.y"
    {COND_PUSH(1);}
    break;

  case 304:
#line 1650 "grammar.y"
    {COND_POP();}
    break;

  case 305:
#line 1653 "grammar.y"
    {
                        yyval.node = NEW_UNTIL(cond(yyvsp[-4].node, parse_state), yyvsp[-1].node, 1);
                        fixpos(yyval.node, yyvsp[-4].node);
                        if (cond_negative(&yyval.node->nd_cond)) {
                            nd_set_type(yyval.node, NODE_WHILE);
                        }
                    }
    break;

  case 306:
#line 1663 "grammar.y"
    {
                        yyval.node = NEW_CASE(yyvsp[-3].node, yyvsp[-1].node);
                        fixpos(yyval.node, yyvsp[-3].node);
                    }
    break;

  case 307:
#line 1668 "grammar.y"
    {
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 308:
#line 1672 "grammar.y"
    {
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 309:
#line 1675 "grammar.y"
    {COND_PUSH(1);}
    break;

  case 310:
#line 1675 "grammar.y"
    {COND_POP();}
    break;

  case 311:
#line 1678 "grammar.y"
    {
                        yyval.node = NEW_FOR(yyvsp[-7].node, yyvsp[-4].node, yyvsp[-1].node);
                        fixpos(yyval.node, yyvsp[-7].node);
                    }
    break;

  case 312:
#line 1683 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("class definition in method body");
                        class_nest++;
                        local_push(0);
                        yyval.num = ruby_sourceline;
                    }
    break;

  case 313:
#line 1692 "grammar.y"
    {
                        yyval.node = NEW_CLASS(yyvsp[-4].node, yyvsp[-1].node, yyvsp[-3].node);
                        nd_set_line(yyval.node, yyvsp[-2].num);
                        local_pop();
                        class_nest--;
                    }
    break;

  case 314:
#line 1699 "grammar.y"
    {
                        yyval.num = in_def;
                        in_def = 0;
                    }
    break;

  case 315:
#line 1704 "grammar.y"
    {
                        yyval.num = in_single;
                        in_single = 0;
                        class_nest++;
                        local_push(0);
                    }
    break;

  case 316:
#line 1712 "grammar.y"
    {
                        yyval.node = NEW_SCLASS(yyvsp[-5].node, yyvsp[-1].node);
                        fixpos(yyval.node, yyvsp[-5].node);
                        local_pop();
                        class_nest--;
                        in_def = yyvsp[-4].num;
                        in_single = yyvsp[-2].num;
                    }
    break;

  case 317:
#line 1721 "grammar.y"
    {
                        if (in_def || in_single)
                            yyerror("module definition in method body");
                        class_nest++;
                        local_push(0);
                        yyval.num = ruby_sourceline;
                    }
    break;

  case 318:
#line 1730 "grammar.y"
    {
                        yyval.node = NEW_MODULE(yyvsp[-3].node, yyvsp[-1].node);
                        nd_set_line(yyval.node, yyvsp[-2].num);
                        local_pop();
                        class_nest--;
                    }
    break;

  case 319:
#line 1737 "grammar.y"
    {
                        yyval.id = cur_mid;
                        cur_mid = yyvsp[0].id;
                        in_def++;
                        local_push(0);
                    }
    break;

  case 320:
#line 1746 "grammar.y"
    {
                        if (!yyvsp[-1].node) yyvsp[-1].node = NEW_NIL();
                        yyval.node = NEW_DEFN(yyvsp[-4].id, yyvsp[-2].node, yyvsp[-1].node, NOEX_PRIVATE);
                        fixpos(yyval.node, yyvsp[-2].node);
                        local_pop();
                        in_def--;
                        cur_mid = yyvsp[-3].id;
                    }
    break;

  case 321:
#line 1754 "grammar.y"
    {vps->lex_state = EXPR_FNAME;}
    break;

  case 322:
#line 1755 "grammar.y"
    {
                        in_single++;
                        local_push(0);
                        vps->lex_state = EXPR_END; /* force for args */
                    }
    break;

  case 323:
#line 1763 "grammar.y"
    {
                        yyval.node = NEW_DEFS(yyvsp[-7].node, yyvsp[-4].id, yyvsp[-2].node, yyvsp[-1].node);
                        fixpos(yyval.node, yyvsp[-7].node);
                        local_pop();
                        in_single--;
                    }
    break;

  case 324:
#line 1770 "grammar.y"
    {
                        yyval.node = NEW_BREAK(0);
                    }
    break;

  case 325:
#line 1774 "grammar.y"
    {
                        yyval.node = NEW_NEXT(0);
                    }
    break;

  case 326:
#line 1778 "grammar.y"
    {
                        yyval.node = NEW_REDO();
                    }
    break;

  case 327:
#line 1782 "grammar.y"
    {
                        yyval.node = NEW_RETRY();
                    }
    break;

  case 328:
#line 1788 "grammar.y"
    {
                        value_expr(yyvsp[0].node);
                        yyval.node = yyvsp[0].node;
                    }
    break;

  case 337:
#line 1809 "grammar.y"
    {
                        yyval.node = NEW_IF(cond(yyvsp[-3].node, parse_state), yyvsp[-1].node, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-3].node);
                    }
    break;

  case 339:
#line 1817 "grammar.y"
    {
                        yyval.node = yyvsp[0].node;
                    }
    break;

  case 343:
#line 1828 "grammar.y"
    {
                        yyval.node = (NODE*)1;
                    }
    break;

  case 344:
#line 1832 "grammar.y"
    {
                        yyval.node = (NODE*)1;
                    }
    break;

  case 345:
#line 1836 "grammar.y"
    {
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 346:
#line 1842 "grammar.y"
    {
                        yyvsp[0].num = ruby_sourceline;
                        reset_block(vps);
                    }
    break;

  case 347:
#line 1847 "grammar.y"
    {
                      yyval.vars = vps->block_vars;
                    }
    break;

  case 348:
#line 1852 "grammar.y"
    {
                        yyval.node = NEW_ITER(yyvsp[-3].node, 0, extract_block_vars(vps, yyvsp[-1].node, yyvsp[-2].vars));
                        nd_set_line(yyval.node, yyvsp[-5].num);
                    }
    break;

  case 349:
#line 1859 "grammar.y"
    {
                        if (yyvsp[-1].node && nd_type(yyvsp[-1].node) == NODE_BLOCK_PASS) {
                            rb_compile_error("both block arg and actual block given");
                        }
                        yyvsp[0].node->nd_iter = yyvsp[-1].node;
                        yyval.node = yyvsp[0].node;
                        fixpos(yyval.node, yyvsp[-1].node);
                    }
    break;

  case 350:
#line 1868 "grammar.y"
    {
                        yyval.node = new_call(parse_state, yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                    }
    break;

  case 351:
#line 1872 "grammar.y"
    {
                        yyval.node = new_call(parse_state, yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                    }
    break;

  case 352:
#line 1878 "grammar.y"
    {
                        yyval.node = new_fcall(parse_state, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[0].node);
                    }
    break;

  case 353:
#line 1883 "grammar.y"
    {
                        yyval.node = new_call(parse_state, yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-3].node);
                    }
    break;

  case 354:
#line 1888 "grammar.y"
    {
                        yyval.node = new_call(parse_state, yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-3].node);
                    }
    break;

  case 355:
#line 1893 "grammar.y"
    {
                        yyval.node = new_call(parse_state, yyvsp[-2].node, yyvsp[0].id, 0);
                    }
    break;

  case 356:
#line 1897 "grammar.y"
    {
                        yyval.node = NEW_CALL(yyvsp[-2].node, rb_intern("get_reference"), NEW_LIST(NEW_LIT(ID2SYM(yyvsp[0].id))));
                    }
    break;

  case 357:
#line 1901 "grammar.y"
    {
                        yyval.node = NEW_FCALL(rb_intern("get_reference"), NEW_LIST(NEW_LIT(ID2SYM(yyvsp[0].id))));
                    }
    break;

  case 358:
#line 1905 "grammar.y"
    {
                        yyval.node = new_super(parse_state, yyvsp[0].node);
                    }
    break;

  case 359:
#line 1909 "grammar.y"
    {
                        yyval.node = NEW_ZSUPER();
                    }
    break;

  case 360:
#line 1915 "grammar.y"
    {
                        yyvsp[0].num = ruby_sourceline;
                        reset_block(vps);
                    }
    break;

  case 361:
#line 1919 "grammar.y"
    { yyval.vars = vps->block_vars; }
    break;

  case 362:
#line 1921 "grammar.y"
    {
                        yyval.node = NEW_ITER(yyvsp[-3].node, 0, extract_block_vars(vps, yyvsp[-1].node, yyvsp[-2].vars));
                        nd_set_line(yyval.node, yyvsp[-5].num);
                    }
    break;

  case 363:
#line 1926 "grammar.y"
    {
                        yyvsp[0].num = ruby_sourceline;
                        reset_block(vps);
                    }
    break;

  case 364:
#line 1930 "grammar.y"
    { yyval.vars = vps->block_vars; }
    break;

  case 365:
#line 1932 "grammar.y"
    {
                        yyval.node = NEW_ITER(yyvsp[-3].node, 0, extract_block_vars(vps, yyvsp[-1].node, yyvsp[-2].vars));
                        nd_set_line(yyval.node, yyvsp[-5].num);
                    }
    break;

  case 366:
#line 1941 "grammar.y"
    {
                        yyval.node = NEW_WHEN(yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node);
                    }
    break;

  case 368:
#line 1947 "grammar.y"
    {
                        yyval.node = list_append(parse_state, yyvsp[-3].node, NEW_WHEN(yyvsp[0].node, 0, 0));
                    }
    break;

  case 369:
#line 1951 "grammar.y"
    {
                        yyval.node = NEW_LIST(NEW_WHEN(yyvsp[0].node, 0, 0));
                    }
    break;

  case 372:
#line 1963 "grammar.y"
    {
                        if (yyvsp[-3].node) {
                            yyvsp[-3].node = node_assign(yyvsp[-3].node, NEW_GVAR(rb_intern("$!")), parse_state);
                            yyvsp[-1].node = block_append(parse_state, yyvsp[-3].node, yyvsp[-1].node);
                        }
                        yyval.node = NEW_RESBODY(yyvsp[-4].node, yyvsp[-1].node, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-4].node?yyvsp[-4].node:yyvsp[-1].node);
                    }
    break;

  case 374:
#line 1975 "grammar.y"
    {
                        yyval.node = NEW_LIST(yyvsp[0].node);
                    }
    break;

  case 377:
#line 1983 "grammar.y"
    {
                        yyval.node = yyvsp[0].node;
                    }
    break;

  case 379:
#line 1990 "grammar.y"
    {
                        if (yyvsp[0].node)
                            yyval.node = yyvsp[0].node;
                        else
                            /* place holder */
                            yyval.node = NEW_NIL();
                    }
    break;

  case 382:
#line 2002 "grammar.y"
    {
                        yyval.node = NEW_LIT(ID2SYM(yyvsp[0].id));
                    }
    break;

  case 384:
#line 2009 "grammar.y"
    {
                        NODE *node = yyvsp[0].node;
                        if (!node) {
                            node = NEW_STR(string_new(0, 0));
                        }
                        else {
                            node = evstr2dstr(parse_state, node);
                        }
                        yyval.node = node;
                    }
    break;

  case 386:
#line 2023 "grammar.y"
    {
                        yyval.node = literal_concat(parse_state, yyvsp[-1].node, yyvsp[0].node);
                    }
    break;

  case 387:
#line 2029 "grammar.y"
    {
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 388:
#line 2035 "grammar.y"
    {
                        ID code = yyvsp[-2].id;
                        NODE *node = yyvsp[-1].node;
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
                        yyval.node = node;
                    }
    break;

  case 389:
#line 2064 "grammar.y"
    {
                        int options = yyvsp[0].num;
                        NODE *node = yyvsp[-1].node;
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
                        yyval.node = node;
                    }
    break;

  case 390:
#line 2099 "grammar.y"
    {
                        yyval.node = NEW_ZARRAY();
                    }
    break;

  case 391:
#line 2103 "grammar.y"
    {
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 392:
#line 2109 "grammar.y"
    {
                        yyval.node = 0;
                    }
    break;

  case 393:
#line 2113 "grammar.y"
    {
                        yyval.node = list_append(parse_state, yyvsp[-2].node, evstr2dstr(parse_state, yyvsp[-1].node));
                    }
    break;

  case 395:
#line 2120 "grammar.y"
    {
                        yyval.node = literal_concat(parse_state, yyvsp[-1].node, yyvsp[0].node);
                    }
    break;

  case 396:
#line 2126 "grammar.y"
    {
                        yyval.node = NEW_ZARRAY();
                    }
    break;

  case 397:
#line 2130 "grammar.y"
    {
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 398:
#line 2136 "grammar.y"
    {
                        yyval.node = 0;
                    }
    break;

  case 399:
#line 2140 "grammar.y"
    {
                        yyval.node = list_append(parse_state, yyvsp[-2].node, yyvsp[-1].node);
                    }
    break;

  case 400:
#line 2146 "grammar.y"
    {
                        yyval.node = 0;
                    }
    break;

  case 401:
#line 2150 "grammar.y"
    {
                        yyval.node = literal_concat(parse_state, yyvsp[-1].node, yyvsp[0].node);
                    }
    break;

  case 402:
#line 2156 "grammar.y"
    {
                        yyval.node = 0;
                    }
    break;

  case 403:
#line 2160 "grammar.y"
    {
                        yyval.node = literal_concat(parse_state, yyvsp[-1].node, yyvsp[0].node);
                    }
    break;

  case 405:
#line 2167 "grammar.y"
    {
                        yyval.node = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                    }
    break;

  case 406:
#line 2173 "grammar.y"
    {
                        lex_strterm = yyvsp[-1].node;
                        yyval.node = NEW_EVSTR(yyvsp[0].node);
                    }
    break;

  case 407:
#line 2178 "grammar.y"
    {
                        yyval.node = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                        COND_PUSH(0);
                        CMDARG_PUSH(0);
                    }
    break;

  case 408:
#line 2186 "grammar.y"
    {
                        lex_strterm = yyvsp[-2].node;
                        COND_LEXPOP();
                        CMDARG_LEXPOP();
                        if ((yyval.node = yyvsp[-1].node) && nd_type(yyval.node) == NODE_NEWLINE) {
                            yyval.node = yyval.node->nd_next;
                            // rb_gc_force_recycle((VALUE));
                        }
                        yyval.node = new_evstr(parse_state, yyval.node);
                    }
    break;

  case 409:
#line 2198 "grammar.y"
    {yyval.node = NEW_GVAR(yyvsp[0].id);}
    break;

  case 410:
#line 2199 "grammar.y"
    {yyval.node = NEW_IVAR(yyvsp[0].id);}
    break;

  case 411:
#line 2200 "grammar.y"
    {yyval.node = NEW_CVAR(yyvsp[0].id);}
    break;

  case 413:
#line 2205 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        yyval.id = yyvsp[0].id;
                    }
    break;

  case 418:
#line 2218 "grammar.y"
    {
                        vps->lex_state = EXPR_END;
                        if (!(yyval.node = yyvsp[-1].node)) {
                            yyerror("empty symbol literal");
                        }
                        else {
                            switch (nd_type(yyval.node)) {
                              case NODE_DSTR:
                                nd_set_type(yyval.node, NODE_DSYM);
                                break;
                              case NODE_STR:
                                if (strlen(yyval.node->nd_str->str) == yyval.node->nd_str->len) {
                                    yyval.node->nd_lit = rb_intern(yyval.node->nd_str->str);
                                    nd_set_type(yyval.node, NODE_LIT);
                                    break;
                                }
                                /* fall through */
                              default:
                                yyval.node = NEW_NODE(NODE_DSYM, string_new(0, 0), 1, NEW_LIST(yyval.node));
                                break;
                            }
                        }
                    }
    break;

  case 421:
#line 2246 "grammar.y"
    {
                        yyval.node = NEW_NEGATE(yyvsp[0].node);
                    }
    break;

  case 422:
#line 2250 "grammar.y"
    {
                        yyval.node = NEW_NEGATE(yyvsp[0].node);
                    }
    break;

  case 428:
#line 2260 "grammar.y"
    {yyval.id = kNIL;}
    break;

  case 429:
#line 2261 "grammar.y"
    {yyval.id = kSELF;}
    break;

  case 430:
#line 2262 "grammar.y"
    {yyval.id = kTRUE;}
    break;

  case 431:
#line 2263 "grammar.y"
    {yyval.id = kFALSE;}
    break;

  case 432:
#line 2264 "grammar.y"
    {yyval.id = k__FILE__;}
    break;

  case 433:
#line 2265 "grammar.y"
    {yyval.id = k__LINE__;}
    break;

  case 434:
#line 2269 "grammar.y"
    {
                        yyval.node = gettable(yyvsp[0].id);
                    }
    break;

  case 435:
#line 2275 "grammar.y"
    {
                        yyval.node = assignable(yyvsp[0].id, 0, parse_state);
                    }
    break;

  case 438:
#line 2285 "grammar.y"
    {
                        yyval.node = 0;
                    }
    break;

  case 439:
#line 2289 "grammar.y"
    {
                        vps->lex_state = EXPR_BEG;
                    }
    break;

  case 440:
#line 2293 "grammar.y"
    {
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 441:
#line 2296 "grammar.y"
    {yyerrok; yyval.node = 0;}
    break;

  case 442:
#line 2300 "grammar.y"
    {
                        yyval.node = yyvsp[-2].node;
                        vps->lex_state = EXPR_BEG;
                    }
    break;

  case 443:
#line 2305 "grammar.y"
    {
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 444:
#line 2311 "grammar.y"
    {
                        // printf("rest + all = %d\n", );
                        yyval.node = block_append(parse_state, NEW_ARGS(yyvsp[-5].num, yyvsp[-3].node, yyvsp[-1].id), yyvsp[0].node);
                    }
    break;

  case 445:
#line 2316 "grammar.y"
    {
                        yyval.node = block_append(parse_state, NEW_ARGS(yyvsp[-3].num, yyvsp[-1].node, -1), yyvsp[0].node);
                    }
    break;

  case 446:
#line 2320 "grammar.y"
    {
                        // printf("arg + rest = %d\n", );
                        yyval.node = block_append(parse_state, NEW_ARGS(yyvsp[-3].num, 0, yyvsp[-1].id), yyvsp[0].node);
                    }
    break;

  case 447:
#line 2325 "grammar.y"
    {
                        yyval.node = block_append(parse_state, NEW_ARGS(yyvsp[-1].num, 0, -1), yyvsp[0].node);
                    }
    break;

  case 448:
#line 2329 "grammar.y"
    {
                        // printf("rest + opt = %d\n", );
                        yyval.node = block_append(parse_state, NEW_ARGS(0, yyvsp[-3].node, yyvsp[-1].id), yyvsp[0].node);
                    }
    break;

  case 449:
#line 2334 "grammar.y"
    {
                        yyval.node = block_append(parse_state, NEW_ARGS(0, yyvsp[-1].node, -1), yyvsp[0].node);
                    }
    break;

  case 450:
#line 2338 "grammar.y"
    {
                        // printf("rest only = %d\n", );
                        yyval.node = block_append(parse_state, NEW_ARGS(0, 0, yyvsp[-1].id), yyvsp[0].node);
                    }
    break;

  case 451:
#line 2343 "grammar.y"
    {
                        yyval.node = block_append(parse_state, NEW_ARGS(0, 0, -1), yyvsp[0].node);
                    }
    break;

  case 452:
#line 2347 "grammar.y"
    {
                        yyval.node = NEW_ARGS(0, 0, -1);
                    }
    break;

  case 453:
#line 2353 "grammar.y"
    {
                        yyerror("formal argument cannot be a constant");
                    }
    break;

  case 454:
#line 2357 "grammar.y"
    {
                        yyerror("formal argument cannot be an instance variable");
                    }
    break;

  case 455:
#line 2361 "grammar.y"
    {
                        yyerror("formal argument cannot be a global variable");
                    }
    break;

  case 456:
#line 2365 "grammar.y"
    {
                        yyerror("formal argument cannot be a class variable");
                    }
    break;

  case 457:
#line 2369 "grammar.y"
    {
                        if (!is_local_id(yyvsp[0].id))
                            yyerror("formal argument must be local variable");
                        else if (local_id(yyvsp[0].id))
                            yyerror("duplicate argument name");
                        local_cnt(yyvsp[0].id);
                        yyval.num = 1;
                    }
    break;

  case 459:
#line 2381 "grammar.y"
    {
                        yyval.num += 1;
                    }
    break;

  case 460:
#line 2387 "grammar.y"
    {
                        if (!is_local_id(yyvsp[-2].id))
                            yyerror("formal argument must be local variable");
                        else if (local_id(yyvsp[-2].id))
                            yyerror("duplicate optional argument name");
                        yyval.node = assignable(yyvsp[-2].id, yyvsp[0].node, parse_state);
                    }
    break;

  case 461:
#line 2397 "grammar.y"
    {
                        yyval.node = NEW_BLOCK(yyvsp[0].node);
                        yyval.node->nd_end = yyval.node;
                    }
    break;

  case 462:
#line 2402 "grammar.y"
    {
                        yyval.node = block_append(parse_state, yyvsp[-2].node, yyvsp[0].node);
                    }
    break;

  case 465:
#line 2412 "grammar.y"
    {
                        if (!is_local_id(yyvsp[0].id))
                            yyerror("rest argument must be local variable");
                        else if (local_id(yyvsp[0].id))
                            yyerror("duplicate rest argument name");
                        yyval.id = local_cnt(yyvsp[0].id) + 1;
                    }
    break;

  case 466:
#line 2420 "grammar.y"
    {
                        //$$ = -2;
                        yyval.id = 0;
                    }
    break;

  case 469:
#line 2431 "grammar.y"
    {
                        if (!is_local_id(yyvsp[0].id))
                            yyerror("block argument must be local variable");
                        else if (local_id(yyvsp[0].id))
                            yyerror("duplicate block argument name");
                        yyval.node = NEW_BLOCK_ARG(yyvsp[0].id);
                    }
    break;

  case 470:
#line 2441 "grammar.y"
    {
                        yyval.node = yyvsp[0].node;
                    }
    break;

  case 472:
#line 2448 "grammar.y"
    {
                        if (nd_type(yyvsp[0].node) == NODE_SELF) {
                            yyval.node = NEW_SELF();
                        }
                        else {
                            yyval.node = yyvsp[0].node;
                            value_expr(yyval.node);
                        }
                    }
    break;

  case 473:
#line 2457 "grammar.y"
    {vps->lex_state = EXPR_BEG;}
    break;

  case 474:
#line 2458 "grammar.y"
    {
                        if (yyvsp[-2].node == 0) {
                            yyerror("can't define singleton method for ().");
                        }
                        else {
                            switch (nd_type(yyvsp[-2].node)) {
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
                                value_expr(yyvsp[-2].node);
                                break;
                            }
                        }
                        yyval.node = yyvsp[-2].node;
                    }
    break;

  case 476:
#line 2484 "grammar.y"
    {
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 477:
#line 2488 "grammar.y"
    {
                        if (yyvsp[-1].node->nd_alen%2 != 0) {
                            yyerror("odd number list for Hash");
                        }
                        yyval.node = yyvsp[-1].node;
                    }
    break;

  case 479:
#line 2498 "grammar.y"
    {
                        yyval.node = list_concat(yyvsp[-2].node, yyvsp[0].node);
                    }
    break;

  case 480:
#line 2504 "grammar.y"
    { yyval.node = yyvsp[0].node; }
    break;

  case 481:
#line 2506 "grammar.y"
    { yyval.node  = yyvsp[0].node; }
    break;

  case 482:
#line 2510 "grammar.y"
    {
                        yyval.node = list_append(parse_state, NEW_LIST(yyvsp[-2].node), yyvsp[0].node);
                    }
    break;

  case 483:
#line 2516 "grammar.y"
    {
                        yyval.node = list_append(parse_state, NEW_LIST(NEW_LIT(ID2SYM(yyvsp[-1].id))), yyvsp[0].node);
                    }
    break;

  case 503:
#line 2554 "grammar.y"
    {yyerrok;}
    break;

  case 506:
#line 2559 "grammar.y"
    {yyerrok;}
    break;

  case 507:
#line 2562 "grammar.y"
    {yyval.node = 0;}
    break;


    }

/* Line 1016 of /usr/local/share/bison/yacc.c.  */
#line 7048 "grammar.c"

  yyvsp -= yylen;
  yyssp -= yylen;


#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

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
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyssp > yyss)
	    {
	      YYDPRINTF ((stderr, "Error: popping "));
	      YYDSYMPRINT ((stderr,
			    yystos[*yyssp],
			    *yyvsp));
	      YYDPRINTF ((stderr, "\n"));
	      yydestruct (yystos[*yyssp], *yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yydestruct (yychar1, yylval);
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

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

      YYDPRINTF ((stderr, "Error: popping "));
      YYDSYMPRINT ((stderr,
		    yystos[*yyssp], *yyvsp));
      YYDPRINTF ((stderr, "\n"));

      yydestruct (yystos[yystate], *yyvsp);
      yyvsp--;
      yystate = *--yyssp;


#if YYDEBUG
      if (yydebug)
	{
	  short *yyssp1 = yyss - 1;
	  YYFPRINTF (stderr, "Error: state stack now");
	  while (yyssp1 != yyssp)
	    YYFPRINTF (stderr, " %d", *++yyssp1);
	  YYFPRINTF (stderr, "\n");
	}
#endif
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
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
    
    if (parse_state->lex_str_used) {
        if (parse_state->lex_string->len == parse_state->lex_str_used) return NULL;
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
    parse_state->state = state;
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
                if(i == cur_line->len) {
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
          case '*':             /* : argv */
          case '$':             /* $$: pid */
          case '?':             /* $?: last status */
          case '!':             /* $!: error string */
          case '@':             /* : error position */
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

static ID
rb_intern(char *name)
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
  id_regist:  
    // printf("Registered '%s' as %d (%d, %d, %d).\n", name, id, qrk, tLAST_TOKEN, bef);
    return id;
}

GQuark id_to_quark(ID id) {
  GQuark qrk;
  
  qrk = (GQuark)((id >> ID_SCOPE_SHIFT) - tLAST_TOKEN);
  // printf("ID %d == %d\n", id, qrk);
  return qrk;
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



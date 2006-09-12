
/*  A Bison parser, made from sydparse.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	kCLASS	257
#define	kMODULE	258
#define	kDEF	259
#define	kUNDEF	260
#define	kBEGIN	261
#define	kRESCUE	262
#define	kENSURE	263
#define	kEND	264
#define	kIF	265
#define	kUNLESS	266
#define	kTHEN	267
#define	kELSIF	268
#define	kELSE	269
#define	kCASE	270
#define	kWHEN	271
#define	kWHILE	272
#define	kUNTIL	273
#define	kFOR	274
#define	kBREAK	275
#define	kNEXT	276
#define	kREDO	277
#define	kRETRY	278
#define	kIN	279
#define	kDO	280
#define	kDO_COND	281
#define	kDO_BLOCK	282
#define	kRETURN	283
#define	kYIELD	284
#define	kSUPER	285
#define	kSELF	286
#define	kNIL	287
#define	kTRUE	288
#define	kFALSE	289
#define	kAND	290
#define	kOR	291
#define	kNOT	292
#define	kIF_MOD	293
#define	kUNLESS_MOD	294
#define	kWHILE_MOD	295
#define	kUNTIL_MOD	296
#define	kRESCUE_MOD	297
#define	kALIAS	298
#define	kDEFINED	299
#define	klBEGIN	300
#define	klEND	301
#define	k__LINE__	302
#define	k__FILE__	303
#define	tIDENTIFIER	304
#define	tFID	305
#define	tGVAR	306
#define	tIVAR	307
#define	tCONSTANT	308
#define	tCVAR	309
#define	tXSTRING_BEG	310
#define	tINTEGER	311
#define	tFLOAT	312
#define	tSTRING_CONTENT	313
#define	tNTH_REF	314
#define	tBACK_REF	315
#define	tREGEXP_END	316
#define	tKEYSYM	317
#define	tUPLUS	318
#define	tUMINUS	319
#define	tUBS	320
#define	tPOW	321
#define	tCMP	322
#define	tEQ	323
#define	tEQQ	324
#define	tNEQ	325
#define	tGEQ	326
#define	tLEQ	327
#define	tANDOP	328
#define	tOROP	329
#define	tMATCH	330
#define	tNMATCH	331
#define	tDOT2	332
#define	tDOT3	333
#define	tAREF	334
#define	tASET	335
#define	tLSHFT	336
#define	tRSHFT	337
#define	tCOLON2	338
#define	tCOLON3	339
#define	tOP_ASGN	340
#define	tASSOC	341
#define	tLPAREN	342
#define	tLPAREN_ARG	343
#define	tRPAREN	344
#define	tLBRACK	345
#define	tLBRACE	346
#define	tLBRACE_ARG	347
#define	tSTAR	348
#define	tAMPER	349
#define	tSYMBEG	350
#define	tSTRING_BEG	351
#define	tREGEXP_BEG	352
#define	tWORDS_BEG	353
#define	tQWORDS_BEG	354
#define	tSTRING_DBEG	355
#define	tSTRING_DVAR	356
#define	tSTRING_END	357
#define	tLOWEST	358
#define	tUMINUS_NUM	359
#define	tLAST_TOKEN	360

#line 13 "sydparse.y"


#define YYDEBUG 1

#include "ruby.h"
#include "env.h"
#include "intern.h"
#include "node.h"
#include "st.h"
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#include "internal.h"
#include "runtime.h"
#include "parse_override.h"

static int syd_sourceline;
static char *syd_sourcefile;

static struct local_vars *syd_lvtbl = 0;
#define lvtbl syd_lvtbl

#define ruby_sourceline syd_sourceline
#define ruby_sourcefile syd_sourcefile

// static struct RVarmap *syd_dyna_vars = 0;
// #define ruby_dyna_vars syd_dyna_vars

static struct SCOPE *syd_scope = 0;
#define ruby_scope syd_scope

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

static NODE *cond();
static NODE *logop();
static int cond_negative();

static NODE *newline_node();
static void fixpos();

static int value_expr0();
static void void_expr0();
static void void_stmts();
static NODE *remove_begin();
#define value_expr(node) value_expr0((node) = remove_begin(node), parse_state)
#define void_expr(node) void_expr0((node) = remove_begin(node), parse_state)

static NODE *block_append();
static NODE *list_append();
static NODE *list_concat();
static NODE *arg_concat();
static NODE *arg_prepend();
static NODE *literal_concat();
static NODE *new_evstr();
static NODE *evstr2dstr();
static NODE *call_op();

static NODE *negate_lit();
static NODE *ret_args();
static NODE *arg_blk_pass();
static NODE *new_call();
static NODE *new_fcall();
static NODE *new_super();
static NODE *new_yield();

static NODE *syd_gettable();
#define gettable(i) syd_gettable(parse_state, i)
static NODE *assignable();
static NODE *aryset();
static NODE *attrset();
static void rb_backref_error();
static NODE *node_assign();

static NODE *match_gen();
static void syd_local_push(rb_parse_state*, int cnt);
#define local_push(cnt) syd_local_push(vps, cnt)
static void syd_local_pop(rb_parse_state*);
#define local_pop() syd_local_pop(vps)
static int  syd_local_append();
#define local_append(i) syd_local_append(vps, i)
static int  syd_local_cnt();
#define local_cnt(i) syd_local_cnt(vps, i)
static int  syd_local_id();
#define local_id(i) syd_local_id(vps, i)
static ID  *syd_local_tbl();
static ID   internal_id();
static ID   convert_op();
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

// static void top_local_init();
// static void top_local_setup();

#define RE_OPTION_ONCE 0x80

#define NODE_STRTERM NODE_ZARRAY        /* nothing to gc */
#define NODE_HEREDOC NODE_ARRAY         /* 1, 3 to gc */
#define SIGN_EXTEND(x,n) (((1<<(n)-1)^((x)&~(~0<<(n))))-(1<<(n)-1))
#define nd_func u1.id
#if SIZEOF_SHORT == 2
#define nd_term(node) ((signed short)(node)->u2.id)
#else
#define nd_term(node) SIGN_EXTEND((node)->u2.id, CHAR_BIT*2)
#endif
#define nd_paren(node) (char)((node)->u2.id >> CHAR_BIT*2)
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


#line 221 "sydparse.y"
typedef union {
    NODE *node;
    ID id;
    int num;
    VALUE vars;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		904
#define	YYFLAG		-32768
#define	YYNTBASE	135

#define YYTRANSLATE(x) ((unsigned)(x) <= 360 ? yytranslate[x] : 280)

static const short yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,   133,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,   132,   119,     2,     2,     2,   117,   112,     2,   130,
   127,   115,   113,   128,   114,   126,   116,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,   107,   134,   109,
   105,   108,   106,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
   124,   131,   125,   111,     2,   129,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,   122,   110,   123,   120,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
    87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
    97,    98,    99,   100,   101,   102,   103,   104,   118,   121
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     9,    12,    14,    16,    20,    23,    24,
    29,    33,    37,    41,    44,    48,    52,    56,    60,    64,
    65,    71,    76,    80,    84,    88,    95,   101,   107,   113,
   117,   121,   125,   129,   131,   133,   137,   141,   144,   147,
   149,   151,   153,   155,   158,   161,   164,   166,   171,   176,
   177,   178,   185,   188,   192,   197,   203,   208,   214,   217,
   220,   222,   226,   228,   232,   234,   237,   241,   244,   247,
   249,   251,   255,   258,   262,   264,   269,   273,   277,   281,
   285,   288,   290,   292,   297,   301,   305,   309,   313,   316,
   318,   320,   322,   325,   327,   331,   333,   335,   337,   339,
   341,   343,   345,   347,   348,   353,   355,   357,   359,   361,
   363,   365,   367,   369,   371,   373,   375,   377,   379,   381,
   383,   385,   387,   389,   391,   393,   395,   397,   399,   401,
   403,   405,   407,   409,   411,   413,   415,   417,   419,   421,
   423,   425,   427,   429,   431,   433,   435,   437,   439,   441,
   443,   445,   447,   449,   451,   453,   455,   457,   459,   461,
   463,   465,   467,   469,   471,   473,   475,   477,   479,   481,
   483,   485,   487,   491,   497,   501,   508,   514,   520,   526,
   532,   537,   541,   545,   549,   553,   557,   561,   565,   569,
   573,   578,   583,   586,   589,   593,   597,   601,   605,   609,
   613,   617,   621,   625,   629,   633,   637,   641,   644,   647,
   651,   655,   659,   663,   664,   669,   670,   677,   679,   681,
   683,   686,   689,   695,   698,   702,   706,   711,   716,   723,
   725,   727,   729,   732,   738,   741,   747,   752,   760,   764,
   766,   771,   775,   781,   789,   792,   798,   803,   810,   818,
   828,   832,   834,   835,   838,   840,   841,   845,   846,   851,
   854,   857,   859,   861,   865,   869,   874,   877,   879,   881,
   883,   885,   887,   889,   891,   893,   895,   896,   901,   902,
   908,   912,   916,   919,   924,   928,   932,   934,   939,   943,
   945,   946,   953,   956,   958,   961,   968,   975,   976,   977,
   985,   986,   987,   995,  1001,  1006,  1012,  1013,  1014,  1024,
  1025,  1032,  1033,  1034,  1043,  1044,  1050,  1051,  1058,  1059,
  1060,  1070,  1072,  1074,  1076,  1078,  1080,  1082,  1084,  1086,
  1089,  1091,  1093,  1095,  1097,  1103,  1105,  1108,  1110,  1112,
  1114,  1117,  1119,  1123,  1124,  1125,  1132,  1135,  1140,  1145,
  1148,  1153,  1158,  1162,  1166,  1169,  1172,  1174,  1175,  1176,
  1183,  1184,  1185,  1192,  1198,  1200,  1205,  1208,  1210,  1212,
  1219,  1221,  1223,  1225,  1227,  1230,  1232,  1235,  1237,  1239,
  1241,  1243,  1245,  1247,  1250,  1254,  1258,  1262,  1266,  1270,
  1271,  1275,  1277,  1280,  1284,  1288,  1289,  1293,  1294,  1297,
  1298,  1301,  1303,  1304,  1308,  1309,  1314,  1316,  1318,  1320,
  1322,  1325,  1327,  1329,  1331,  1333,  1337,  1339,  1341,  1344,
  1347,  1349,  1351,  1353,  1355,  1357,  1359,  1361,  1363,  1365,
  1367,  1369,  1371,  1373,  1375,  1377,  1379,  1380,  1385,  1388,
  1393,  1396,  1403,  1408,  1413,  1416,  1421,  1424,  1427,  1429,
  1430,  1432,  1434,  1436,  1438,  1440,  1442,  1446,  1450,  1452,
  1456,  1458,  1460,  1463,  1465,  1467,  1469,  1472,  1475,  1477,
  1479,  1480,  1486,  1488,  1491,  1494,  1496,  1500,  1502,  1504,
  1508,  1511,  1513,  1515,  1517,  1519,  1521,  1523,  1525,  1527,
  1529,  1531,  1533,  1535,  1536,  1538,  1539,  1541,  1542,  1544,
  1546,  1548,  1550,  1552,  1555
};

static const short yyrhs[] = {    -1,
   136,   138,     0,   138,   221,   205,   224,     0,   139,   274,
     0,   279,     0,   140,     0,   139,   278,   140,     0,     1,
   140,     0,     0,    44,   161,   141,   161,     0,    44,    52,
    52,     0,    44,    52,    61,     0,    44,    52,    60,     0,
     6,   162,     0,   140,    39,   144,     0,   140,    40,   144,
     0,   140,    41,   144,     0,   140,    42,   144,     0,   140,
    43,   140,     0,     0,    46,   142,   122,   138,   123,     0,
    47,   122,   138,   123,     0,   157,   105,   145,     0,   151,
   105,   145,     0,   248,    86,   145,     0,   201,   124,   170,
   125,    86,   145,     0,   201,   126,    50,    86,   145,     0,
   201,   126,    54,    86,   145,     0,   201,    84,    50,    86,
   145,     0,   249,    86,   145,     0,   157,   105,   183,     0,
   151,   105,   169,     0,   151,   105,   183,     0,   143,     0,
   145,     0,   143,    36,   143,     0,   143,    37,   143,     0,
    38,   143,     0,   119,   145,     0,   166,     0,   143,     0,
   150,     0,   146,     0,    29,   173,     0,    21,   173,     0,
    22,   173,     0,   211,     0,   211,   126,   271,   175,     0,
   211,    84,   271,   175,     0,     0,     0,    93,   148,   207,
   149,   138,   123,     0,   270,   175,     0,   270,   175,   147,
     0,   201,   126,   271,   175,     0,   201,   126,   271,   175,
   147,     0,   201,    84,   271,   175,     0,   201,    84,   271,
   175,   147,     0,    31,   175,     0,    30,   175,     0,   153,
     0,    88,   152,   127,     0,   153,     0,    88,   152,   127,
     0,   155,     0,   155,   154,     0,   155,    94,   156,     0,
   155,    94,     0,    94,   156,     0,    94,     0,   156,     0,
    88,   152,   127,     0,   154,   128,     0,   155,   154,   128,
     0,   246,     0,   201,   124,   170,   125,     0,   201,   126,
    50,     0,   201,    84,    50,     0,   201,   126,    54,     0,
   201,    84,    54,     0,    85,    54,     0,   249,     0,   246,
     0,   201,   124,   170,   125,     0,   201,   126,    50,     0,
   201,    84,    50,     0,   201,   126,    54,     0,   201,    84,
    54,     0,    85,    54,     0,   249,     0,    50,     0,    54,
     0,    85,   158,     0,   158,     0,   201,    84,   158,     0,
    50,     0,    54,     0,    51,     0,   164,     0,   165,     0,
   160,     0,   242,     0,   161,     0,     0,   162,   128,   163,
   161,     0,   110,     0,   111,     0,   112,     0,    68,     0,
    69,     0,    70,     0,    76,     0,   108,     0,    72,     0,
   109,     0,    73,     0,    82,     0,    83,     0,   113,     0,
   114,     0,   115,     0,    94,     0,   116,     0,   117,     0,
    67,     0,   120,     0,    64,     0,    65,     0,    80,     0,
    81,     0,   129,     0,    48,     0,    49,     0,    46,     0,
    47,     0,    44,     0,    36,     0,     7,     0,    21,     0,
    16,     0,     3,     0,     5,     0,    45,     0,    26,     0,
    15,     0,    14,     0,    10,     0,     9,     0,    35,     0,
    20,     0,    25,     0,     4,     0,    22,     0,    33,     0,
    38,     0,    37,     0,    23,     0,     8,     0,    24,     0,
    29,     0,    32,     0,    31,     0,    13,     0,    34,     0,
     6,     0,    17,     0,    30,     0,    39,     0,    40,     0,
    41,     0,    42,     0,    43,     0,   157,   105,   166,     0,
   157,   105,   166,    43,   166,     0,   248,    86,   166,     0,
   201,   124,   170,   125,    86,   166,     0,   201,   126,    50,
    86,   166,     0,   201,   126,    54,    86,   166,     0,   201,
    84,    50,    86,   166,     0,   201,    84,    54,    86,   166,
     0,    85,    54,    86,   166,     0,   249,    86,   166,     0,
   166,    78,   166,     0,   166,    79,   166,     0,   166,   113,
   166,     0,   166,   114,   166,     0,   166,   115,   166,     0,
   166,   116,   166,     0,   166,   117,   166,     0,   166,    67,
   166,     0,   118,    57,    67,   166,     0,   118,    58,    67,
   166,     0,    64,   166,     0,    65,   166,     0,   166,   110,
   166,     0,   166,   111,   166,     0,   166,   112,   166,     0,
   166,    68,   166,     0,   166,   108,   166,     0,   166,    72,
   166,     0,   166,   109,   166,     0,   166,    73,   166,     0,
   166,    69,   166,     0,   166,    70,   166,     0,   166,    71,
   166,     0,   166,    76,   166,     0,   166,    77,   166,     0,
   119,   166,     0,   120,   166,     0,   166,    82,   166,     0,
   166,    83,   166,     0,   166,    74,   166,     0,   166,    75,
   166,     0,     0,    45,   275,   167,   166,     0,     0,   166,
   106,   168,   166,   107,   166,     0,   184,     0,   166,     0,
   279,     0,   150,   275,     0,   182,   276,     0,   182,   128,
    94,   166,   275,     0,   266,   276,     0,    94,   166,   275,
     0,   130,   279,   127,     0,   130,   173,   275,   127,     0,
   130,   211,   275,   127,     0,   130,   182,   128,   211,   275,
   127,     0,   279,     0,   171,     0,   150,     0,   182,   181,
     0,   182,   128,    94,   169,   181,     0,   266,   181,     0,
   266,   128,    94,   169,   181,     0,   182,   128,   266,   181,
     0,   182,   128,   266,   128,    94,   166,   181,     0,    94,
   169,   181,     0,   180,     0,   169,   128,   182,   181,     0,
   169,   128,   180,     0,   169,   128,    94,   169,   181,     0,
   169,   128,   182,   128,    94,   169,   181,     0,   266,   181,
     0,   266,   128,    94,   169,   181,     0,   169,   128,   266,
   181,     0,   169,   128,   182,   128,   266,   181,     0,   169,
   128,   266,   128,    94,   169,   181,     0,   169,   128,   182,
   128,   266,   128,    94,   169,   181,     0,    94,   169,   181,
     0,   180,     0,     0,   176,   177,     0,   173,     0,     0,
    89,   178,   127,     0,     0,    89,   174,   179,   127,     0,
    95,   169,     0,   128,   180,     0,   279,     0,   169,     0,
   182,   128,   169,     0,   182,   128,   169,     0,   182,   128,
    94,   169,     0,    94,   169,     0,   225,     0,   226,     0,
   229,     0,   230,     0,   231,     0,   234,     0,   247,     0,
   249,     0,    51,     0,     0,     7,   185,   137,    10,     0,
     0,    89,   143,   186,   275,   127,     0,    88,   138,   127,
     0,   201,    84,    54,     0,    85,    54,     0,   201,   124,
   170,   125,     0,    91,   170,   125,     0,    92,   265,   123,
     0,    29,     0,    30,   130,   173,   127,     0,    30,   130,
   127,     0,    30,     0,     0,    45,   275,   130,   187,   143,
   127,     0,   270,   213,     0,   212,     0,   212,   213,     0,
    11,   144,   202,   138,   204,    10,     0,    12,   144,   202,
   138,   205,    10,     0,     0,     0,    18,   188,   144,   203,
   189,   138,    10,     0,     0,     0,    19,   190,   144,   203,
   191,   138,    10,     0,    16,   144,   274,   218,    10,     0,
    16,   274,   218,    10,     0,    16,   274,    15,   138,    10,
     0,     0,     0,    20,   206,    25,   192,   144,   203,   193,
   138,    10,     0,     0,     3,   159,   250,   194,   137,    10,
     0,     0,     0,     3,    82,   143,   195,   277,   196,   137,
    10,     0,     0,     4,   159,   197,   137,    10,     0,     0,
     5,   160,   198,   252,   137,    10,     0,     0,     0,     5,
   263,   273,   199,   160,   200,   252,   137,    10,     0,    21,
     0,    22,     0,    23,     0,    24,     0,   184,     0,   277,
     0,   107,     0,    13,     0,   277,    13,     0,   277,     0,
   107,     0,    27,     0,   205,     0,    14,   144,   202,   138,
   204,     0,   279,     0,    15,   138,     0,   157,     0,   151,
     0,   279,     0,   110,   110,     0,    75,     0,   110,   206,
   110,     0,     0,     0,    28,   209,   207,   210,   138,    10,
     0,   150,   208,     0,   211,   126,   271,   172,     0,   211,
    84,   271,   172,     0,   270,   171,     0,   201,   126,   271,
   172,     0,   201,    84,   271,   171,     0,   201,    84,   272,
     0,   201,   131,   271,     0,    66,   271,     0,    31,   171,
     0,    31,     0,     0,     0,   122,   214,   207,   215,   138,
   123,     0,     0,     0,    26,   216,   207,   217,   138,    10,
     0,    17,   219,   202,   138,   220,     0,   182,     0,   182,
   128,    94,   169,     0,    94,   169,     0,   205,     0,   218,
     0,     8,   222,   223,   202,   138,   221,     0,   279,     0,
   169,     0,   183,     0,   279,     0,    87,   157,     0,   279,
     0,     9,   138,     0,   279,     0,   245,     0,   242,     0,
   244,     0,   227,     0,   228,     0,   227,   228,     0,    97,
   236,   103,     0,    56,   237,   103,     0,    98,   237,    62,
     0,    99,   132,   103,     0,    99,   232,   103,     0,     0,
   232,   233,   132,     0,   238,     0,   233,   238,     0,   100,
   132,   103,     0,   100,   235,   103,     0,     0,   235,    59,
   132,     0,     0,   236,   238,     0,     0,   237,   238,     0,
    59,     0,     0,   102,   239,   241,     0,     0,   101,   240,
   138,   123,     0,    52,     0,    53,     0,    55,     0,   249,
     0,    96,   243,     0,   160,     0,    53,     0,    52,     0,
    55,     0,    96,   237,   103,     0,    57,     0,    58,     0,
   118,    57,     0,   118,    58,     0,    50,     0,    53,     0,
    52,     0,    54,     0,    55,     0,    33,     0,    32,     0,
    34,     0,    35,     0,    49,     0,    48,     0,   246,     0,
   246,     0,    60,     0,    61,     0,   277,     0,     0,   109,
   251,   144,   277,     0,     1,   277,     0,   130,   253,   275,
   127,     0,   253,   277,     0,   255,   128,   257,   128,   259,
   262,     0,   255,   128,   257,   262,     0,   255,   128,   259,
   262,     0,   255,   262,     0,   257,   128,   259,   262,     0,
   257,   262,     0,   259,   262,     0,   261,     0,     0,    54,
     0,    53,     0,    52,     0,    55,     0,    50,     0,   254,
     0,   255,   128,   254,     0,    50,   105,   169,     0,   256,
     0,   257,   128,   256,     0,   115,     0,    94,     0,   258,
    50,     0,   258,     0,   112,     0,    95,     0,   260,    50,
     0,   128,   261,     0,   279,     0,   247,     0,     0,   130,
   264,   143,   275,   127,     0,   279,     0,   266,   276,     0,
   182,   276,     0,   267,     0,   266,   128,   267,     0,   268,
     0,   269,     0,   169,    87,   169,     0,    63,   169,     0,
    50,     0,    54,     0,    51,     0,    50,     0,    54,     0,
    51,     0,   164,     0,    50,     0,    51,     0,   164,     0,
   126,     0,    84,     0,     0,   278,     0,     0,   133,     0,
     0,   133,     0,   128,     0,   134,     0,   133,     0,   277,
     0,   278,   134,     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   359,   368,   389,   409,   416,   417,   421,   425,   431,   432,
   435,   439,   446,   451,   455,   464,   473,   485,   497,   501,
   508,   517,   525,   529,   535,   560,   579,   591,   603,   615,
   620,   624,   629,   634,   637,   638,   642,   646,   650,   654,
   657,   664,   665,   666,   670,   674,   680,   681,   685,   691,
   696,   698,   706,   711,   723,   728,   740,   745,   757,   762,
   769,   770,   776,   777,   783,   787,   791,   795,   799,   803,
   809,   810,   816,   820,   826,   830,   834,   838,   842,   846,
   852,   858,   865,   869,   873,   877,   881,   885,   891,   897,
   904,   908,   911,   915,   919,   925,   926,   927,   928,   933,
   940,   941,   944,   948,   949,   954,   955,   956,   957,   958,
   959,   960,   961,   962,   963,   964,   965,   966,   967,   968,
   969,   970,   971,   972,   973,   974,   975,   976,   977,   978,
   979,   982,   982,   982,   982,   983,   983,   983,   983,   983,
   983,   983,   984,   984,   984,   984,   984,   984,   984,   985,
   985,   985,   985,   985,   985,   986,   986,   986,   986,   986,
   986,   986,   987,   987,   987,   987,   987,   988,   988,   988,
   988,   988,   991,   995,   999,  1024,  1043,  1055,  1067,  1079,
  1084,  1089,  1094,  1100,  1106,  1110,  1114,  1118,  1122,  1126,
  1130,  1134,  1138,  1147,  1151,  1155,  1159,  1163,  1167,  1171,
  1175,  1179,  1183,  1187,  1191,  1195,  1199,  1203,  1207,  1211,
  1215,  1219,  1223,  1227,  1228,  1232,  1232,  1238,  1244,  1251,
  1252,  1257,  1261,  1266,  1270,  1277,  1281,  1285,  1290,  1297,
  1298,  1301,  1306,  1310,  1315,  1320,  1325,  1330,  1336,  1340,
  1343,  1347,  1351,  1356,  1361,  1366,  1371,  1376,  1381,  1386,
  1391,  1395,  1398,  1403,  1410,  1411,  1412,  1416,  1417,  1423,
  1429,  1433,  1436,  1440,  1446,  1450,  1454,  1460,  1461,  1462,
  1463,  1464,  1465,  1466,  1467,  1468,  1472,  1477,  1485,  1485,
  1490,  1494,  1498,  1502,  1511,  1520,  1524,  1528,  1532,  1536,
  1540,  1540,  1545,  1551,  1552,  1561,  1574,  1587,  1587,  1589,
  1597,  1597,  1599,  1607,  1614,  1618,  1622,  1622,  1624,  1629,
  1638,  1645,  1651,  1658,  1667,  1676,  1683,  1691,  1701,  1702,
  1708,  1716,  1720,  1724,  1728,  1734,  1741,  1742,  1743,  1744,
  1747,  1748,  1749,  1752,  1753,  1762,  1763,  1769,  1770,  1773,
  1774,  1778,  1782,  1788,  1793,  1795,  1803,  1812,  1816,  1822,
  1827,  1832,  1837,  1841,  1845,  1849,  1853,  1859,  1864,  1865,
  1871,  1876,  1877,  1885,  1892,  1893,  1897,  1903,  1904,  1907,
  1918,  1921,  1925,  1926,  1929,  1933,  1936,  1944,  1947,  1948,
  1952,  1955,  1968,  1969,  1975,  1981,  2010,  2043,  2047,  2053,
  2057,  2063,  2064,  2070,  2074,  2080,  2084,  2090,  2094,  2100,
  2104,  2110,  2111,  2118,  2122,  2130,  2143,  2144,  2145,  2146,
  2149,  2156,  2157,  2158,  2159,  2162,  2188,  2189,  2190,  2194,
  2200,  2201,  2202,  2203,  2204,  2205,  2206,  2207,  2208,  2209,
  2210,  2213,  2219,  2225,  2226,  2229,  2233,  2237,  2241,  2244,
  2249,  2255,  2260,  2264,  2269,  2273,  2278,  2282,  2287,  2291,
  2297,  2301,  2305,  2309,  2313,  2324,  2325,  2331,  2341,  2346,
  2352,  2353,  2356,  2364,  2370,  2371,  2374,  2384,  2388,  2391,
  2401,  2401,  2426,  2427,  2431,  2440,  2441,  2447,  2449,  2452,
  2457,  2462,  2463,  2464,  2467,  2468,  2469,  2470,  2473,  2474,
  2475,  2478,  2479,  2482,  2483,  2486,  2487,  2490,  2491,  2492,
  2495,  2496,  2499,  2500,  2503
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","kCLASS",
"kMODULE","kDEF","kUNDEF","kBEGIN","kRESCUE","kENSURE","kEND","kIF","kUNLESS",
"kTHEN","kELSIF","kELSE","kCASE","kWHEN","kWHILE","kUNTIL","kFOR","kBREAK","kNEXT",
"kREDO","kRETRY","kIN","kDO","kDO_COND","kDO_BLOCK","kRETURN","kYIELD","kSUPER",
"kSELF","kNIL","kTRUE","kFALSE","kAND","kOR","kNOT","kIF_MOD","kUNLESS_MOD",
"kWHILE_MOD","kUNTIL_MOD","kRESCUE_MOD","kALIAS","kDEFINED","klBEGIN","klEND",
"k__LINE__","k__FILE__","tIDENTIFIER","tFID","tGVAR","tIVAR","tCONSTANT","tCVAR",
"tXSTRING_BEG","tINTEGER","tFLOAT","tSTRING_CONTENT","tNTH_REF","tBACK_REF",
"tREGEXP_END","tKEYSYM","tUPLUS","tUMINUS","tUBS","tPOW","tCMP","tEQ","tEQQ",
"tNEQ","tGEQ","tLEQ","tANDOP","tOROP","tMATCH","tNMATCH","tDOT2","tDOT3","tAREF",
"tASET","tLSHFT","tRSHFT","tCOLON2","tCOLON3","tOP_ASGN","tASSOC","tLPAREN",
"tLPAREN_ARG","tRPAREN","tLBRACK","tLBRACE","tLBRACE_ARG","tSTAR","tAMPER","tSYMBEG",
"tSTRING_BEG","tREGEXP_BEG","tWORDS_BEG","tQWORDS_BEG","tSTRING_DBEG","tSTRING_DVAR",
"tSTRING_END","tLOWEST","'='","'?'","':'","'>'","'<'","'|'","'^'","'&'","'+'",
"'-'","'*'","'/'","'%'","tUMINUS_NUM","'!'","'~'","tLAST_TOKEN","'{'","'}'",
"'['","']'","'.'","')'","','","'`'","'('","'\\\\'","' '","'\\n'","';'","program",
"@1","bodystmt","compstmt","stmts","stmt","@2","@3","expr","expr_value","command_call",
"block_command","cmd_brace_block","@4","@5","command","mlhs","mlhs_entry","mlhs_basic",
"mlhs_item","mlhs_head","mlhs_node","lhs","cname","cpath","fname","fitem","undef_list",
"@6","op","reswords","arg","@7","@8","arg_value","aref_args","paren_args","opt_paren_args",
"call_args","call_args2","command_args","@9","open_args","@10","@11","block_arg",
"opt_block_arg","args","mrhs","primary","@12","@13","@14","@15","@16","@17",
"@18","@19","@20","@21","@22","@23","@24","@25","@26","@27","primary_value",
"then","do","if_tail","opt_else","block_var","opt_block_var","do_block","@28",
"@29","block_call","method_call","brace_block","@30","@31","@32","@33","case_body",
"when_args","cases","opt_rescue","exc_list","exc_var","opt_ensure","literal",
"strings","string","string1","xstring","regexp","words","word_list","word","qwords",
"qword_list","string_contents","xstring_contents","string_content","@34","@35",
"string_dvar","symbol","sym","dsym","numeric","variable","var_ref","var_lhs",
"backref","superclass","@36","f_arglist","f_args","f_norm_arg","f_arg","f_opt",
"f_optarg","restarg_mark","f_rest_arg","blkarg_mark","f_block_arg","opt_f_block_arg",
"singleton","@37","assoc_list","assocs","assoc","assoc1","assoc2","operation",
"operation2","operation3","dot_or_colon","opt_terms","opt_nl","trailer","term",
"terms","none", NULL
};
#endif

static const short yyr1[] = {     0,
   136,   135,   137,   138,   139,   139,   139,   139,   141,   140,
   140,   140,   140,   140,   140,   140,   140,   140,   140,   142,
   140,   140,   140,   140,   140,   140,   140,   140,   140,   140,
   140,   140,   140,   140,   143,   143,   143,   143,   143,   143,
   144,   145,   145,   145,   145,   145,   146,   146,   146,   148,
   149,   147,   150,   150,   150,   150,   150,   150,   150,   150,
   151,   151,   152,   152,   153,   153,   153,   153,   153,   153,
   154,   154,   155,   155,   156,   156,   156,   156,   156,   156,
   156,   156,   157,   157,   157,   157,   157,   157,   157,   157,
   158,   158,   159,   159,   159,   160,   160,   160,   160,   160,
   161,   161,   162,   163,   162,   164,   164,   164,   164,   164,
   164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
   164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
   164,   165,   165,   165,   165,   165,   165,   165,   165,   165,
   165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
   165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
   165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
   165,   165,   166,   166,   166,   166,   166,   166,   166,   166,
   166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
   166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
   166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
   166,   166,   166,   167,   166,   168,   166,   166,   169,   170,
   170,   170,   170,   170,   170,   171,   171,   171,   171,   172,
   172,   173,   173,   173,   173,   173,   173,   173,   173,   173,
   174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
   174,   174,   176,   175,   177,   178,   177,   179,   177,   180,
   181,   181,   182,   182,   183,   183,   183,   184,   184,   184,
   184,   184,   184,   184,   184,   184,   185,   184,   186,   184,
   184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
   187,   184,   184,   184,   184,   184,   184,   188,   189,   184,
   190,   191,   184,   184,   184,   184,   192,   193,   184,   194,
   184,   195,   196,   184,   197,   184,   198,   184,   199,   200,
   184,   184,   184,   184,   184,   201,   202,   202,   202,   202,
   203,   203,   203,   204,   204,   205,   205,   206,   206,   207,
   207,   207,   207,   209,   210,   208,   211,   211,   211,   212,
   212,   212,   212,   212,   212,   212,   212,   214,   215,   213,
   216,   217,   213,   218,   219,   219,   219,   220,   220,   221,
   221,   222,   222,   222,   223,   223,   224,   224,   225,   225,
   225,   226,   227,   227,   228,   229,   230,   231,   231,   232,
   232,   233,   233,   234,   234,   235,   235,   236,   236,   237,
   237,   238,   239,   238,   240,   238,   241,   241,   241,   241,
   242,   243,   243,   243,   243,   244,   245,   245,   245,   245,
   246,   246,   246,   246,   246,   246,   246,   246,   246,   246,
   246,   247,   248,   249,   249,   250,   251,   250,   250,   252,
   252,   253,   253,   253,   253,   253,   253,   253,   253,   253,
   254,   254,   254,   254,   254,   255,   255,   256,   257,   257,
   258,   258,   259,   259,   260,   260,   261,   262,   262,   263,
   264,   263,   265,   265,   265,   266,   266,   267,   267,   268,
   269,   270,   270,   270,   271,   271,   271,   271,   272,   272,
   272,   273,   273,   274,   274,   275,   275,   276,   276,   276,
   277,   277,   278,   278,   279
};

static const short yyr2[] = {     0,
     0,     2,     4,     2,     1,     1,     3,     2,     0,     4,
     3,     3,     3,     2,     3,     3,     3,     3,     3,     0,
     5,     4,     3,     3,     3,     6,     5,     5,     5,     3,
     3,     3,     3,     1,     1,     3,     3,     2,     2,     1,
     1,     1,     1,     2,     2,     2,     1,     4,     4,     0,
     0,     6,     2,     3,     4,     5,     4,     5,     2,     2,
     1,     3,     1,     3,     1,     2,     3,     2,     2,     1,
     1,     3,     2,     3,     1,     4,     3,     3,     3,     3,
     2,     1,     1,     4,     3,     3,     3,     3,     2,     1,
     1,     1,     2,     1,     3,     1,     1,     1,     1,     1,
     1,     1,     1,     0,     4,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     3,     5,     3,     6,     5,     5,     5,     5,
     4,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     4,     4,     2,     2,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     2,     2,     3,
     3,     3,     3,     0,     4,     0,     6,     1,     1,     1,
     2,     2,     5,     2,     3,     3,     4,     4,     6,     1,
     1,     1,     2,     5,     2,     5,     4,     7,     3,     1,
     4,     3,     5,     7,     2,     5,     4,     6,     7,     9,
     3,     1,     0,     2,     1,     0,     3,     0,     4,     2,
     2,     1,     1,     3,     3,     4,     2,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     0,     4,     0,     5,
     3,     3,     2,     4,     3,     3,     1,     4,     3,     1,
     0,     6,     2,     1,     2,     6,     6,     0,     0,     7,
     0,     0,     7,     5,     4,     5,     0,     0,     9,     0,
     6,     0,     0,     8,     0,     5,     0,     6,     0,     0,
     9,     1,     1,     1,     1,     1,     1,     1,     1,     2,
     1,     1,     1,     1,     5,     1,     2,     1,     1,     1,
     2,     1,     3,     0,     0,     6,     2,     4,     4,     2,
     4,     4,     3,     3,     2,     2,     1,     0,     0,     6,
     0,     0,     6,     5,     1,     4,     2,     1,     1,     6,
     1,     1,     1,     1,     2,     1,     2,     1,     1,     1,
     1,     1,     1,     2,     3,     3,     3,     3,     3,     0,
     3,     1,     2,     3,     3,     0,     3,     0,     2,     0,
     2,     1,     0,     3,     0,     4,     1,     1,     1,     1,
     2,     1,     1,     1,     1,     3,     1,     1,     2,     2,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     0,     4,     2,     4,
     2,     6,     4,     4,     2,     4,     2,     2,     1,     0,
     1,     1,     1,     1,     1,     1,     3,     3,     1,     3,
     1,     1,     2,     1,     1,     1,     2,     2,     1,     1,
     0,     5,     1,     2,     2,     1,     3,     1,     1,     3,
     2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     0,     1,     0,     1,     0,     1,     1,
     1,     1,     1,     2,     0
};

static const short yydefact[] = {     1,
     0,     0,     0,     0,     0,     0,   277,     0,     0,   494,
   298,   301,     0,   322,   323,   324,   325,   287,   290,   357,
   427,   426,   428,   429,     0,     0,   496,    20,     0,   431,
   430,   421,   484,   423,   422,   424,   425,   400,   417,   418,
   434,   435,     0,     0,     0,     0,     0,     0,   505,   505,
    70,   400,   398,   400,   390,   396,     0,     0,     0,     2,
   494,     6,    34,    35,    43,    42,     0,    61,     0,    65,
    71,     0,    40,   218,     0,    47,   294,   268,   269,   382,
   383,   270,   271,   272,   273,   380,   381,   379,   432,   274,
     0,   275,   253,     5,     8,   322,   323,   287,   290,   357,
   496,    91,    92,     0,     0,     0,     0,    94,     0,   326,
     0,   432,   275,     0,   315,   141,   152,   142,   165,   138,
   158,   148,   147,   163,   146,   145,   140,   166,   150,   139,
   153,   157,   159,   151,   144,   160,   167,   162,   161,   154,
   164,   149,   137,   156,   155,   168,   169,   170,   171,   172,
   136,   143,   134,   135,   132,   133,    96,    98,    97,   127,
   128,   125,   109,   110,   111,   114,   116,   112,   129,   130,
   117,   118,   122,   113,   115,   106,   107,   108,   119,   120,
   121,   123,   124,   126,   131,   471,   317,    99,   100,   470,
     0,   161,   154,   164,   149,   132,   133,    96,    97,     0,
   101,   103,    14,   102,     0,     0,    41,     0,     0,     0,
   432,     0,   275,     0,   502,   501,   494,     0,   503,   495,
     0,     0,     0,   339,   338,     0,     0,   432,   275,     0,
     0,     0,     0,   232,   219,   263,    45,   240,   505,   505,
   476,   478,   479,    46,    44,     0,    60,     0,   505,   356,
    59,    38,     0,     9,   497,   214,     0,     0,     0,   193,
     0,   194,   485,   487,   486,   488,   355,   283,     0,     0,
     0,    61,   279,     0,   496,     0,   498,   498,   220,   498,
     0,   498,   473,     0,    69,     0,    75,    82,   414,   413,
   415,   412,     0,   411,     0,     0,     0,     0,     0,     0,
   419,   420,    39,   208,   209,     4,   495,     0,     0,     0,
     0,     0,     0,     0,   344,   347,     0,    73,     0,    68,
    66,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   216,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   505,
     0,     0,     0,     0,   361,   358,   295,   384,     0,     0,
   350,    53,   293,     0,   312,    91,    92,    93,   419,   420,
     0,   437,   310,   436,     0,   505,     0,     0,     0,   450,
   493,   492,   319,   104,     0,   505,   283,   329,   328,     0,
   327,     0,     0,   505,     0,     0,     0,     0,     0,     0,
     0,     0,   504,     0,     0,   283,     0,   505,     0,   307,
   481,   505,   260,     0,     0,   233,   262,     0,   235,   289,
     0,   256,   255,   254,   232,   496,   505,   496,     0,    11,
    13,    12,     0,   291,     0,     0,     0,   402,   405,   403,
   386,   401,     0,     0,     0,     0,   281,    62,   496,   496,
   221,   285,   500,   499,   222,   500,   224,   500,   475,   286,
   474,    81,     0,   505,     0,   416,   385,   399,   387,   388,
   389,     0,   392,   394,     0,   395,     0,     0,     7,    15,
    16,    17,    18,    19,    36,    37,   505,     0,    24,    32,
     0,    33,     0,    67,    74,    23,   173,   263,    31,   190,
   198,   203,   204,   205,   200,   202,   212,   213,   206,   207,
   183,   184,   210,   211,     0,   199,   201,   195,   196,   197,
   185,   186,   187,   188,   189,   485,   490,   486,   491,   253,
   353,     0,   485,   486,   505,   354,   253,   253,   505,   505,
    25,   175,    30,   182,    50,    54,     0,   439,     0,     0,
    91,    92,    95,     0,     0,   505,     0,   496,   455,   453,
   452,   451,   454,   462,   466,   465,   461,   450,     0,     0,
   456,   505,   459,   505,   464,   505,     0,   449,     0,     0,
   278,   505,   505,   371,   505,   330,   173,   489,   282,     0,
   485,   486,   505,     0,     0,     0,   365,     0,   305,   333,
   332,   299,   331,   302,   489,   282,     0,   485,   486,     0,
     0,   239,   480,     0,   264,   261,   505,     0,     0,   477,
   288,     0,    40,     0,   258,     0,   252,   505,     0,     0,
     0,     0,     0,   226,    10,     0,   215,     0,    22,     0,
     0,   181,    62,     0,   225,     0,   264,   489,    80,     0,
   485,   486,   391,   393,   397,   191,   192,   342,     0,   345,
   340,   267,     0,    72,     0,     0,     0,     0,   352,    57,
   284,     0,     0,   231,   351,    55,   230,   349,    49,   348,
    48,   362,   359,   505,   313,     0,     0,   284,   316,     0,
     0,   496,     0,   441,     0,   445,   469,     0,   447,   463,
     0,   448,   467,   320,   105,   372,   373,   505,   374,     0,
   505,   336,     0,     0,   334,     0,   284,     0,     0,     0,
   304,   306,   367,     0,     0,     0,     0,   284,     0,   505,
     0,   237,   505,   505,     0,     0,   257,     0,   245,   227,
     0,   496,   505,   505,   228,     0,    21,     0,   407,   408,
   409,   404,   410,   280,   496,    76,   341,     0,     0,     0,
   265,   174,     0,    29,   179,   180,    58,     0,    27,   177,
    28,   178,    56,     0,     0,    51,     0,   438,   311,   472,
   458,     0,   318,   457,   505,   505,   468,     0,   460,   505,
   450,     0,     0,   376,   337,     0,     3,   378,     0,   296,
     0,   297,     0,   505,     0,     0,   308,   234,     0,   236,
   251,     0,   242,   505,   505,   259,     0,     0,   292,   406,
   223,   343,     0,   266,   217,    26,   176,     0,     0,     0,
     0,   440,     0,   443,   444,   446,     0,     0,   375,     0,
    83,    90,     0,   377,     0,   366,   368,   369,   364,   300,
   303,     0,   505,   505,     0,   241,     0,   247,   505,   229,
   346,   363,   360,     0,   314,   505,     0,    89,     0,   505,
     0,   505,   505,     0,   238,   243,     0,   505,     0,   246,
    52,   442,   321,   489,    88,     0,   485,   486,   370,   335,
   309,   505,     0,   248,   505,    84,   244,     0,   249,   505,
   250,     0,     0,     0
};

static const short yydefgoto[] = {   902,
     1,   385,   386,    61,    62,   433,   257,    63,   208,    64,
    65,   546,   684,   830,    66,    67,   271,    68,    69,    70,
    71,   209,   108,   109,   201,   202,   203,   580,   266,   189,
    73,   435,   515,   236,   276,   674,   675,   237,   625,   247,
   248,   424,   626,   736,   616,   416,   277,   492,    74,   205,
   449,   636,   221,   726,   222,   727,   610,   852,   550,   547,
   777,   378,   380,   579,   791,   261,   390,   602,   714,   715,
   227,   660,   316,   487,   759,    76,    77,   363,   540,   775,
   539,   774,   402,   598,   849,   583,   708,   793,   797,    78,
    79,    80,    81,    82,    83,    84,   298,   472,    85,   300,
   295,   259,   442,   641,   640,   752,    86,   294,    87,    88,
   211,    90,   212,   213,   373,   549,   569,   570,   571,   572,
   573,   574,   575,   576,   577,   787,   696,   191,   379,   281,
   278,   241,   242,   243,   114,   554,   531,   383,   218,   256,
   455,   391,   220,    94
};

static const short yypact[] = {-32768,
  2010,  5719,  8340,  8643,  4163,  5372,-32768,  7189,  7189,  4041,
-32768,-32768,  8441,  5929,  5929,-32768,-32768,  5929,  4703,  4808,
-32768,-32768,-32768,-32768,  7189,  5254,   -27,-32768,     3,-32768,
-32768,  4283,  2431,-32768,-32768,  4388,-32768,-32768,-32768,-32768,
-32768,-32768,  8029,  8029,  8897,    62,  3428,  7189,  7084,  7294,
  8744,  5136,-32768,-32768,    23,    31,   219,  8134,  8029,-32768,
   173,   630,   348,-32768,-32768,   110,    44,-32768,    70,  8542,
-32768,    65,  9839,   267,   418,    27,    89,-32768,-32768,   169,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    26,-32768,
   128,    41,    51,-32768,   630,-32768,-32768,-32768,   153,   217,
   -27,   107,   364,  7189,    98,  3553,   367,-32768,    55,-32768,
   519,-32768,-32768,    51,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    94,   129,
   266,   269,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   273,   283,   302,-32768,   346,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   357,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  5136,
-32768,-32768,   100,-32768,  2823,   239,   348,    53,   247,   558,
    -7,   278,    19,    53,-32768,-32768,   173,   160,-32768,   262,
  7189,  7189,   327,-32768,-32768,   613,   388,    56,   111,  8029,
  8029,  8029,  8029,-32768,  9839,   332,-32768,-32768,   325,   331,
-32768,-32768,-32768,-32768,-32768,  5609,-32768,  6034,  5929,-32768,
-32768,-32768,   163,-32768,-32768,   345,   349,  3678,   193,-32768,
   647,   410,-32768,-32768,-32768,-32768,-32768,   196,  3428,   352,
   355,   360,   348,  8029,   -27,   359,    97,   151,-32768,   158,
   370,   151,-32768,   437,-32768,   668,   676,   680,-32768,-32768,
-32768,-32768,   197,-32768,   471,    33,   394,   560,   409,   -10,
   452,   457,-32768,-32768,-32768,-32768,  3922,  7189,  7189,  7189,
  7189,  5719,  7189,  7189,-32768,-32768,  7399,-32768,  3428,  8744,
   403,  7399,  8029,  8029,  8029,  8029,  8029,  8029,  8029,  8029,
  8029,  8029,  8029,  8029,  8029,  8029,  8029,-32768,  8029,  8029,
  8029,  8029,  8029,  8029,  8029,  8029,  8029,  8029,  8965,  7084,
  9033,  8897,  8897,  8897,-32768,-32768,-32768,-32768,  8134,  8134,
-32768,   440,-32768,   345,   348,-32768,   713,-32768,-32768,-32768,
   173,-32768,-32768,-32768,  9101,  7084,  8897,  2823,  7189,   573,
-32768,-32768,-32768,-32768,   543,   549,    99,-32768,-32768,  2948,
   553,  8029,  9169,  7084,  9237,  8029,  8029,  3188,   550,  3803,
  7504,   566,-32768,    75,    75,   116,  9305,  7084,  9373,-32768,
-32768,   450,-32768,  8029,  6139,-32768,-32768,  6244,-32768,-32768,
   458,  5824,-32768,-32768,   110,   -27,   461,     6,   464,-32768,
-32768,-32768,  5372,-32768,  8029,  3678,   469,-32768,-32768,-32768,
-32768,-32768,  9169,  9237,  8029,   467,-32768,   470,   -27,  9708,
-32768,-32768,  7609,-32768,-32768,  7294,-32768,  8029,-32768,-32768,
-32768,   713,  9441,  7084,  9509,-32768,-32768,-32768,-32768,-32768,
-32768,    42,-32768,-32768,   465,-32768,  8029,  8029,   630,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,    13,  8029,-32768,   477,
   478,-32768,   480,-32768,-32768,-32768,  2105,-32768,-32768,   410,
  2264,  2264,  2264,  2264,   810,   810,  9890,  1610,  2264,  2264,
  1862,  1862,   422,   422,  8029,   810,   810,   924,   924,   446,
     4,     4,   410,   410,   410,  2560,  4913,  2689,  5018,   217,
-32768,   495,   368,   375,  4808,-32768,  1055,  1055,    13,    13,
-32768,  9839,-32768,  9839,-32768,-32768,   173,-32768,  7189,  2823,
   421,   281,-32768,   217,   496,   217,   600,    67,   524,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   584,  2823,   173,
-32768,   502,-32768,   503,   583,   512,   591,-32768,  5490,  5372,
-32768,  7714,   629,-32768,   485,-32768,  2539,  4493,  4598,   527,
   185,   251,   629,   650,   654,  8029,   529,    53,-32768,-32768,
-32768,-32768,-32768,-32768,    69,    82,   530,   117,   137,  7189,
   571,-32768,-32768,  8029,   332,-32768,   546,  8029,   332,-32768,
-32768,  8029,  2668,    30,-32768,   548,-32768,   555,   554,  6349,
  8897,  8897,   559,-32768,-32768,  7189,  9839,   564,-32768,  3678,
   770,  9839,   379,   563,-32768,  8029,-32768,   131,   281,   568,
   341,   413,-32768,-32768,-32768,   410,   410,-32768,  8239,-32768,
-32768,-32768,  7819,-32768,  8029,  9822,  8134,  8029,-32768,   440,
   376,  8134,  8134,-32768,-32768,   440,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,    13,-32768,   173,   681,-32768,-32768,   567,
  8029,   -27,   685,-32768,   584,-32768,-32768,    35,-32768,-32768,
    14,-32768,-32768,-32768,-32768,   477,-32768,   611,-32768,  3308,
   691,-32768,  7189,   696,-32768,  8029,   254,  8029,  8029,   697,
-32768,-32768,-32768,  7924,  3068,  3803,  3803,   140,    75,   450,
  6454,-32768,   450,   450,  6559,   574,-32768,  6664,-32768,-32768,
   110,     6,   217,   217,-32768,    63,-32768,   585,-32768,-32768,
-32768,-32768,-32768,-32768,  9708,   721,-32768,   601,  3803,  8029,
   588,  9839,  8029,-32768,  9839,  9839,-32768,  8134,-32768,  9839,
-32768,  9839,-32768,  3803,  3678,-32768,  2823,-32768,-32768,-32768,
-32768,   593,-32768,-32768,   590,   512,-32768,   524,-32768,   512,
   573,  8845,    53,-32768,-32768,  3803,-32768,-32768,    53,-32768,
  8029,-32768,  8029,   343,   712,   715,-32768,-32768,  8029,-32768,
-32768,  8029,-32768,   595,   599,-32768,  8029,   605,-32768,-32768,
-32768,-32768,   723,-32768,  9839,-32768,  9839,   724,   622,  3678,
   726,-32768,    35,-32768,-32768,-32768,  2823,   693,-32768,   756,
   676,   680,  2823,-32768,  2948,-32768,-32768,-32768,-32768,-32768,
-32768,  3803,  9760,   450,  6769,-32768,  6874,-32768,   450,-32768,
-32768,-32768,-32768,   625,-32768,   512,   740,   713,  9577,  7084,
  9645,   549,   485,   747,-32768,-32768,  8029,   638,  8029,-32768,
-32768,-32768,-32768,    38,   281,   633,    60,    73,-32768,-32768,
-32768,   450,  6979,-32768,   450,   721,-32768,  8029,-32768,   450,
-32768,   768,   769,-32768
};

static const short yypgoto[] = {-32768,
-32768,  -367,    16,-32768,    36,-32768,-32768,   561,    66,   -55,
-32768,  -562,-32768,-32768,    -6,   -11,  -182,    20,   700,-32768,
    18,  1568,   -73,   773,     1,   -16,-32768,-32768,    22,-32768,
   -14,-32768,-32768,   531,  -324,    -4,  -480,    24,-32768,    58,
-32768,-32768,-32768,-32768,     0,   102,    32,  -285,    21,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   507,  -209,  -374,   -99,  -529,
   121,  -479,-32768,-32768,-32768,  -227,-32768,   707,-32768,-32768,
-32768,-32768,  -378,-32768,-32768,   -87,-32768,-32768,-32768,-32768,
-32768,-32768,   706,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   309,  -215,-32768,-32768,-32768,     7,-32768,-32768,-32768,
  1432,   783,   645,  1591,-32768,-32768,     2,   228,   106,-32768,
  -657,   113,-32768,  -630,-32768,  -373,  -417,-32768,-32768,-32768,
     5,  -359,-32768,-32768,  1003,    83,-32768,-32768,   -21,    17,
   301,    52,   748,  1004
};


#define	YYLAST		10007


static const short yytable[] = {   235,
   235,   224,   303,   235,   398,   187,   578,   234,   234,   254,
   557,   234,   204,   238,   238,   250,    60,   238,   240,   240,
   594,   428,   240,   110,   110,   532,   188,   188,   260,   262,
   604,   368,   204,   110,   235,   235,   499,    95,   244,   306,
   789,   245,   275,   304,   305,   239,   239,   188,   475,   239,
   -86,   555,   292,   711,   282,   371,   678,   680,   620,   682,
   683,   219,   270,   720,   786,   388,   272,   790,   285,   590,
   323,   110,   -85,   188,   214,   217,   355,   251,  -433,   468,
   -83,   280,   473,   607,   788,   -87,   446,   658,   361,   631,
   110,   438,   476,   -86,   469,   250,   620,   -83,   313,   314,
   438,   600,   313,   314,   397,   255,   -88,   767,   565,   361,
   353,  -433,   219,   773,   355,   268,   414,   364,   346,   347,
   348,   270,   659,   -90,   258,   566,   360,   267,   564,   565,
   -83,   632,  -482,   439,   440,   -90,   493,   315,   255,   650,
   -89,   -85,   439,   440,   -86,   -90,   566,   366,   317,   567,
   362,   367,   354,   -75,   297,   -78,   699,   735,   702,   389,
   374,   -87,   299,   372,   -84,   -83,   -85,  -485,   -82,   322,
   -86,   -86,   356,   653,   400,   789,   401,  -427,   -86,   -87,
   249,   601,   687,   -75,   445,   215,   216,   215,   216,   819,
  -421,   -88,   -85,   -85,   578,   399,   -78,   318,  -485,   255,
   292,   693,   866,   -89,   776,   -87,   -87,   215,   216,   -80,
   356,  -486,  -426,   359,   430,   235,   235,   235,   304,  -427,
   -90,   188,   431,   432,   453,   -89,   -85,   384,  -482,   454,
  -421,   235,  -421,   235,   235,   -78,  -482,  -421,   -82,   234,
   -78,   234,   425,   -81,   -77,   238,   -87,   238,   238,   -84,
   240,   438,   240,   240,  -426,   438,   654,   -78,   -78,   450,
  -485,   489,   678,   680,   -79,    53,   496,   -76,   219,   421,
   718,   423,   426,   437,   847,   301,   302,   239,   456,   239,
   427,   445,   246,   454,   270,   458,   404,   405,   272,   -85,
   454,   451,   387,   439,   440,   441,   707,   439,   440,   466,
   -89,   553,   235,   541,   543,   215,   216,   497,   500,   501,
   502,   503,   504,   505,   506,   507,   508,   509,   510,   511,
   512,   513,   514,   -81,   516,   517,   518,   519,   520,   521,
   522,   523,   524,   525,   270,   235,   719,   494,   272,   801,
   110,   419,   479,   275,   542,   544,   249,   484,   491,  -428,
  -326,   392,  -429,   491,   807,   -87,  -431,   710,   -84,   401,
   293,   235,   296,   396,  -282,   -77,  -430,   834,   835,   275,
   529,   620,   836,   480,   481,   482,   483,   587,   620,   235,
   406,   542,   544,   313,   314,  -421,   235,   275,   725,  -483,
  -326,  -428,  -326,   235,  -429,   403,   529,  -326,  -431,   235,
   235,   275,   742,   235,  -282,   585,  -282,   623,  -430,   831,
  -486,  -282,   410,   593,   529,   595,   635,   578,   414,   617,
   637,   627,   548,   369,   370,   848,   628,  -421,   529,  -424,
   642,   530,   597,   535,   536,   537,   538,   -79,   235,   204,
   381,   235,   629,   235,   633,   -77,  -489,  -424,   882,   235,
   -77,   638,   415,   672,   188,   603,   603,   275,   418,   556,
   673,   768,   656,   657,   529,   644,   645,   -77,   -77,   867,
   436,  -424,   -85,   235,   434,   530,   323,   535,   447,   -87,
   -84,   448,   382,   452,   529,  -483,   -63,  -424,   323,  -424,
   462,   556,   460,  -483,  -424,   -77,   470,   620,   713,   710,
   666,   349,   -79,   -76,  -489,   -64,   -72,    75,    75,   111,
   111,   474,   323,   612,   210,   210,   210,   -79,   477,   226,
   210,   210,   -79,   478,   210,   669,   556,   336,   337,   438,
   495,   210,   545,   620,   344,   345,   346,   347,   348,   -79,
   -79,   350,  -489,   351,  -489,   886,  -489,   556,   352,   669,
  -485,  -489,   581,    75,   210,   210,   582,   286,   344,   345,
   346,   347,   348,   705,   210,   586,   401,   235,   207,   207,
   207,   439,   440,   467,   690,   599,   286,   611,   457,   704,
   459,   235,   461,   843,   621,   252,   204,   670,   630,   845,
   634,   639,   676,   643,   679,   681,   655,   -72,   685,   235,
   188,   188,   375,   235,  -263,   663,   664,   235,   273,   689,
   210,   764,    75,   491,   686,   235,   769,   771,   438,   671,
   688,   694,   559,   741,   560,   561,   562,   563,   691,   695,
   698,   755,   700,   559,   617,   560,   561,   562,   563,   701,
   703,   393,   376,   710,   377,    91,    91,   224,   235,   352,
   762,   717,   765,   766,   728,   748,   724,   770,   772,   721,
   439,   440,   471,   722,   365,   232,   564,   565,   308,   309,
   310,   311,   312,   731,   737,   729,   235,   564,   565,   110,
   740,   394,   738,   395,   566,   745,   747,   567,   352,   754,
   779,    91,   756,   780,   783,   566,   407,   792,   567,   796,
   816,   765,   568,   770,   772,   800,   802,   820,   782,   235,
   822,    75,   826,   743,   744,  -264,   235,   833,   732,   832,
   235,   850,   855,   235,   851,   795,   857,   210,   210,   739,
   443,   860,   861,   862,   813,   865,   408,   778,   409,   815,
   804,   805,   806,   352,   863,   235,   868,   881,   825,   883,
    91,   463,   210,   827,   210,   210,   891,   896,   818,  -432,
   411,   412,   413,  -275,    75,   893,   814,   903,   904,   321,
   394,   821,   444,   890,   823,    75,   115,   352,   799,   758,
   603,   207,   207,   357,   889,   358,   827,   190,   235,   828,
   829,   464,   837,   465,   853,   692,  -283,   235,   352,  -432,
   784,  -432,   235,  -275,  -284,  -275,  -432,   785,   307,     0,
  -275,   844,   110,    75,   210,   210,   210,   210,    75,   210,
   210,   749,   750,   210,   751,    75,   286,     0,   210,    41,
    42,   808,     0,     0,   810,   811,  -283,     0,  -283,   869,
   235,     0,   235,  -283,  -284,   864,  -284,   490,     0,    91,
     0,  -284,   498,     0,     0,   235,   210,     0,   872,   878,
   873,     0,   235,   275,   235,   210,   210,   874,   207,   207,
   207,   207,     0,   485,   486,     0,   323,     0,   235,   870,
     0,   871,   210,   235,    75,   210,   352,     0,     0,     0,
   529,   336,   337,     0,     0,     0,    75,     0,     0,     0,
   210,     0,    91,     0,    75,     0,    75,     0,     0,     0,
     0,     0,     0,    91,   210,   856,   858,     0,     0,   341,
   342,   343,   344,   345,   346,   347,   348,     0,   210,     0,
     0,   498,     0,     0,     0,     0,     0,     0,     0,   558,
     0,     0,    75,     0,   613,   615,     0,     0,   619,     0,
     0,    91,   624,   556,   875,   876,    91,     0,     0,     0,
   880,     0,     0,    91,     0,     0,     0,     0,     0,     0,
   210,     0,     0,     0,     0,     0,     0,     0,     0,   894,
     0,     0,   273,   647,     0,     0,   619,     0,   647,     0,
   323,     0,     0,   897,     0,     0,   899,     0,     0,     0,
     0,   901,     0,    93,    93,   336,   337,     0,     0,     0,
    93,    93,    93,     0,     0,     0,    93,    93,   662,     0,
    93,     0,    91,     0,     0,     0,     0,    93,     0,     0,
     0,     0,     0,     0,    91,   343,   344,   345,   346,   347,
   348,     0,    91,     0,    91,     0,     0,     0,     0,    93,
    93,    93,   279,   283,  -505,   210,    75,     0,     0,     0,
    93,     0,  -505,  -505,  -505,     0,     0,  -505,  -505,  -505,
     0,  -505,     0,     0,     0,    75,     0,     0,     0,     0,
    91,  -505,     0,     0,     0,     0,     0,     0,     0,     0,
  -505,  -505,     0,  -505,  -505,  -505,  -505,  -505,     0,     0,
     0,     0,     0,     0,     0,     0,    93,     0,    93,   207,
     0,     0,   706,     0,     0,     0,   210,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   723,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   210,     0,  -505,     0,
     0,     0,   210,     0,   730,     0,    75,     0,   733,     0,
     0,     0,   734,     0,     0,     0,     0,     0,     0,     0,
   615,  -505,     0,     0,     0,   226,     0,     0,     0,     0,
   207,     0,     0,   210,     0,     0,     0,  -505,   210,   210,
  -505,  -505,     0,     0,   249,     0,     0,  -505,  -505,     0,
     0,     0,     0,   761,    91,     0,   746,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    93,     0,     0,
     0,     0,     0,    91,     0,     0,    75,     0,     0,   210,
     0,   781,     0,    93,    93,     0,     0,     0,     0,     0,
     0,    75,    75,    75,     0,     0,     0,     0,     0,     0,
     0,     0,   417,   417,     0,     0,     0,     0,    93,     0,
    93,    93,   429,     0,   647,     0,     0,     0,     0,     0,
    93,   619,     0,     0,     0,    75,     0,     0,   619,     0,
     0,    93,     0,   207,   210,     0,     0,     0,     0,     0,
    75,    75,     0,    75,    91,     0,     0,     0,     0,     0,
   824,     0,     0,     0,     0,     0,     0,     0,   840,     0,
     0,     0,    75,     0,     0,     0,     0,     0,     0,    93,
    93,    93,    93,    93,    93,    93,    93,     0,     0,    93,
     0,    93,     0,     0,    93,     0,     0,     0,     0,     0,
     0,     0,     0,   846,     0,     0,    75,     0,     0,     0,
     0,     0,   854,    75,     0,     0,     0,   859,     0,    75,
     0,    75,    93,   279,    91,     0,     0,     0,    75,     0,
     0,    93,    93,     0,     0,     0,     0,     0,     0,    91,
    91,    91,     0,     0,     0,     0,   210,     0,    93,   279,
    93,    93,     0,     0,     0,   615,     0,   619,     0,   584,
     0,     0,    93,     0,     0,     0,    93,   279,     0,     0,
    93,     0,    93,    91,     0,     0,     0,   892,     0,   895,
    93,   279,     0,     0,     0,   417,     0,     0,    91,    91,
     0,    91,     0,   619,    93,     0,     0,     0,   900,     0,
   417,     0,    89,    89,   112,   112,   112,     0,    93,     0,
    91,     0,     0,     0,   228,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    93,   279,     0,     0,
     0,     0,     0,     0,    91,     0,     0,     0,    89,     0,
     0,    91,   287,     0,     0,     0,     0,    91,     0,    91,
   661,     0,     0,     0,     0,     0,    91,     0,     0,     0,
     0,   287,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    89,   677,     0,
   677,   677,   661,   661,     0,     0,     0,     0,     0,     0,
     0,    93,    93,     0,     0,     0,     0,     0,     0,   677,
     0,     0,     0,     0,     0,     0,     0,     0,    72,    72,
     0,    93,     0,     0,     0,   697,     0,   697,     0,   697,
   225,     0,     0,     0,     0,   709,   712,     0,   712,     0,
     0,    92,    92,   113,   113,     0,   712,     0,     0,     0,
     0,     0,     0,   229,     0,     0,     0,     0,     0,     0,
     0,     0,    93,     0,    72,     0,     0,     0,     0,     0,
   417,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   417,    93,     0,     0,     0,    89,    92,    93,     0,
     0,   288,    93,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   288,     0,     0,     0,     0,     0,     0,     0,     0,    93,
     0,     0,     0,    72,    93,    93,   323,   324,   325,   326,
   327,   328,   329,   330,     0,   332,   333,   661,     0,    89,
     0,   336,   337,     0,     0,     0,    92,     0,     0,     0,
    89,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   794,    93,     0,   798,    93,     0,   339,   340,   341,
   342,   343,   344,   345,   346,   347,   348,    93,    93,    93,
     0,     0,     0,   417,     0,     0,   417,   417,    89,     0,
     0,     0,     0,    89,     0,     0,   677,   677,     0,     0,
    89,   287,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    93,     0,     0,     0,     0,     0,     0,     0,     0,
    93,     0,    72,     0,     0,     0,    93,    93,     0,    93,
     0,     0,     0,     0,     0,     0,     0,     0,   697,   697,
     0,     0,     0,   697,     0,    92,     0,     0,    93,     0,
     0,     0,     0,     0,     0,     0,     0,   712,     0,    89,
     0,     0,     0,     0,     0,     0,     0,   417,   417,     0,
     0,    89,     0,     0,     0,    72,     0,     0,     0,    89,
     0,    89,    93,     0,     0,     0,    72,     0,     0,    93,
     0,     0,     0,     0,     0,    93,     0,    93,    92,     0,
     0,     0,     0,     0,    93,     0,   417,   417,     0,    92,
     0,     0,   417,     0,     0,     0,     0,    89,     0,   697,
     0,     0,    93,   279,    72,   584,   712,     0,     0,    72,
     0,   417,     0,     0,     0,     0,    72,     0,     0,     0,
     0,     0,     0,     0,     0,   417,     0,    92,   417,     0,
     0,     0,    92,   417,     0,     0,     0,     0,     0,    92,
   288,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   323,   324,
   325,   326,   327,   328,   329,   330,   331,   332,   333,-32768,
-32768,     0,     0,   336,   337,    72,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    72,     0,     0,
     0,     0,     0,     0,     0,    72,     0,    72,    92,   339,
   340,   341,   342,   343,   344,   345,   346,   347,   348,     0,
    92,    89,     0,     0,     0,     0,     0,     0,    92,     0,
    92,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    89,     0,     0,    72,     0,     0,     0,     0,     0,  -505,
     2,     0,     3,     4,     5,     6,     7,     0,     0,     0,
     8,     9,     0,     0,     0,    10,    92,    11,    12,    13,
    14,    15,    16,    17,     0,     0,     0,     0,    18,    19,
    20,    21,    22,    23,    24,     0,     0,    25,     0,     0,
     0,     0,     0,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,     0,    41,
    42,    89,     0,    43,    44,    45,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   228,     0,     0,     0,    46,     0,     0,    47,    48,     0,
    49,    50,     0,    51,     0,    52,    53,    54,    55,    56,
     0,     0,     0,     0,     0,     0,     0,    72,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    57,    58,    59,
     0,     0,     0,     0,     0,     0,    72,     0,     0,     0,
    92,    89,  -505,  -505,     0,     0,     0,   665,     0,     0,
     0,     0,     0,     0,     0,     0,    89,    89,    89,    92,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   323,   324,   325,   326,   327,   328,   329,   330,   331,
   332,   333,   334,   335,     0,     0,   336,   337,     0,     0,
    89,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    89,    89,    72,    89,     0,
   338,     0,   339,   340,   341,   342,   343,   344,   345,   346,
   347,   348,     0,   841,     0,     0,   225,    89,     0,     0,
    92,   753,  -219,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   229,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    89,     0,     0,     0,     0,     0,     0,    89,     0,
     0,     0,     0,     0,    89,     0,    89,    72,     0,     0,
     0,     0,     0,    89,     0,     0,     0,     0,     0,     0,
     0,     0,    72,    72,    72,     0,     0,     0,     0,     0,
    92,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    92,    92,    92,     0,     0,
     0,     0,     0,     0,     0,     0,    72,     0,     0,     0,
   323,-32768,-32768,-32768,-32768,   328,   329,     0,     0,-32768,
-32768,    72,    72,     0,    72,   336,   337,     0,     0,    92,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   839,
     0,     0,     0,    72,    92,    92,     0,    92,     0,     0,
     0,   339,   340,   341,   342,   343,   344,   345,   346,   347,
   348,     0,   842,     0,     0,     0,    92,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    72,     0,     0,
     0,     0,     0,     0,    72,     0,     0,     0,     0,     0,
    72,     0,    72,     0,     0,     0,     0,     0,     0,    72,
    92,     0,     0,     0,     0,     0,     0,    92,     0,     0,
  -276,     0,     0,    92,     0,    92,     0,     0,  -276,  -276,
  -276,     0,    92,  -276,  -276,  -276,     0,  -276,     0,     0,
     0,     0,     0,     0,     0,     0,     0,  -276,  -276,     0,
     0,     0,     0,     0,     0,     0,  -276,  -276,     0,  -276,
  -276,  -276,  -276,  -276,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,  -276,  -276,  -276,
  -276,  -276,  -276,  -276,  -276,  -276,  -276,  -276,  -276,  -276,
     0,     0,  -276,  -276,  -276,     0,     0,  -276,     0,     0,
     0,     0,     0,  -276,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,  -276,  -276,  -276,  -276,
  -276,  -276,  -276,  -276,  -276,  -276,  -276,  -276,     0,     0,
     0,     0,     0,  -276,  -276,  -276,  -276,  -276,  -276,  -489,
     0,  -276,     0,  -276,  -276,     0,     0,  -489,  -489,  -489,
     0,     0,     0,  -489,  -489,     0,  -489,     0,     0,     0,
     0,   665,     0,     0,     0,  -489,     0,     0,     0,     0,
     0,     0,     0,     0,     0,  -489,  -489,     0,  -489,  -489,
  -489,  -489,  -489,     0,     0,   323,   324,   325,   326,   327,
   328,   329,   330,   331,   332,   333,   334,   335,     0,     0,
   336,   337,     0,     0,     0,     0,  -489,  -489,  -489,  -489,
  -489,  -489,  -489,  -489,  -489,  -489,  -489,  -489,  -489,     0,
     0,  -489,  -489,  -489,   338,   667,   339,   340,   341,   342,
   343,   344,   345,   346,   347,   348,     0,     0,     0,     0,
     0,     0,     0,     0,   -86,  -489,     0,  -489,  -489,  -489,
  -489,  -489,  -489,  -489,  -489,  -489,  -489,     0,     0,     0,
     0,  -489,  -489,  -489,     0,  -489,  -489,   -78,  -282,     0,
  -489,     0,  -489,  -489,     0,     0,  -282,  -282,  -282,     0,
     0,     0,  -282,  -282,     0,  -282,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,  -282,  -282,     0,  -282,  -282,  -282,
  -282,  -282,     0,     0,   323,   324,   325,   326,   327,   328,
   329,   330,   331,   332,   333,   334,   335,     0,     0,   336,
   337,     0,     0,     0,  -219,  -282,  -282,  -282,  -282,  -282,
  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,     0,     0,
  -282,  -282,  -282,   338,   668,   339,   340,   341,   342,   343,
   344,   345,   346,   347,   348,     0,     0,     0,     0,     0,
     0,     0,     0,   -88,  -282,  -219,  -282,  -282,  -282,  -282,
  -282,  -282,  -282,  -282,  -282,  -282,     0,     0,     0,     0,
     0,  -282,  -282,     0,  -282,  -282,   -80,     0,     0,  -282,
     0,  -282,  -282,     2,     0,     3,     4,     5,     6,     7,
  -505,  -505,  -505,     8,     9,     0,     0,  -505,    10,     0,
    11,    12,    13,    14,    15,    16,    17,     0,     0,     0,
     0,    18,    19,    20,    21,    22,    23,    24,     0,     0,
    25,     0,     0,     0,     0,     0,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,     0,    41,    42,     0,     0,    43,    44,    45,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    46,     0,     0,
    47,    48,     0,    49,    50,     0,    51,     0,    52,    53,
    54,    55,    56,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    57,    58,    59,     0,     0,     0,     0,     0,     2,     0,
     3,     4,     5,     6,     7,  -505,  -505,  -505,     8,     9,
     0,  -505,  -505,    10,     0,    11,    12,    13,    14,    15,
    16,    17,     0,     0,     0,     0,    18,    19,    20,    21,
    22,    23,    24,     0,     0,    25,     0,     0,     0,     0,
     0,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
     0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    46,     0,     0,    47,    48,     0,    49,    50,
     0,    51,     0,    52,    53,    54,    55,    56,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    57,    58,    59,     2,     0,
     3,     4,     5,     6,     7,     0,     0,  -505,     8,     9,
  -505,  -505,  -505,    10,  -505,    11,    12,    13,    14,    15,
    16,    17,     0,     0,     0,     0,    18,    19,    20,    21,
    22,    23,    24,     0,     0,    25,     0,     0,     0,     0,
     0,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
     0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    46,     0,     0,    47,    48,     0,    49,    50,
     0,    51,     0,    52,    53,    54,    55,    56,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    57,    58,    59,     2,     0,
     3,     4,     5,     6,     7,     0,     0,  -505,     8,     9,
  -505,  -505,  -505,    10,     0,    11,    12,    13,    14,    15,
    16,    17,     0,     0,     0,     0,    18,    19,    20,    21,
    22,    23,    24,     0,     0,    25,     0,     0,     0,     0,
     0,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
     0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    46,     0,     0,    47,    48,     0,    49,    50,
     0,    51,     0,    52,    53,    54,    55,    56,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    57,    58,    59,     2,     0,
     3,     4,     5,     6,     7,     0,  -505,  -505,     8,     9,
  -505,  -505,     0,    10,     0,    11,    12,    13,    14,    15,
    16,    17,     0,     0,     0,     0,    18,    19,    20,    21,
    22,    23,    24,     0,     0,    25,     0,     0,     0,     0,
     0,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
     0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    46,     0,     0,    47,    48,     0,    49,    50,
     0,    51,     0,    52,    53,    54,    55,    56,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    57,    58,    59,     2,     0,
     3,     4,     5,     6,     7,     0,     0,     0,     8,     9,
  -505,  -505,     0,    10,     0,    11,    12,    13,    14,    15,
    16,    17,     0,     0,     0,     0,    18,    19,    20,    21,
    22,    23,    24,     0,     0,    25,     0,     0,     0,     0,
     0,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
     0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    46,     0,     0,   269,    48,     0,    49,    50,
     0,    51,     0,    52,    53,    54,    55,    56,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    57,    58,    59,     0,     0,
     0,     0,     0,     2,  -505,     3,     4,     5,     6,     7,
  -505,  -505,     0,     8,     9,     0,     0,     0,    10,     0,
    11,    12,    13,    14,    15,    16,    17,     0,     0,     0,
     0,    18,    19,    20,    21,    22,    23,    24,     0,     0,
    25,     0,     0,     0,     0,     0,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,     0,    41,    42,     0,     0,    43,    44,    45,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    46,     0,     0,
    47,    48,     0,    49,    50,     0,    51,     0,    52,    53,
    54,    55,    56,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    57,    58,    59,     0,     0,     0,     0,     0,     2,  -505,
     3,     4,     5,     6,     7,  -505,  -505,     0,     8,     9,
     0,     0,     0,    10,     0,    11,    12,    13,    14,    15,
    16,    17,     0,     0,     0,     0,    18,    19,    20,    21,
    22,    23,    24,     0,     0,    25,     0,     0,     0,     0,
     0,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,     0,    41,    42,     0,
     0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    46,     0,     0,    47,    48,     0,    49,    50,
     0,    51,     0,    52,    53,    54,    55,    56,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    57,    58,    59,     0,     0,
  -505,     0,     0,     2,     0,     3,     4,     5,     6,     7,
  -505,  -505,  -505,     8,     9,     0,     0,     0,    10,     0,
    11,    12,    13,    14,    15,    16,    17,     0,     0,     0,
     0,    18,    19,    20,    21,    22,    23,    24,     0,     0,
    25,     0,     0,     0,     0,     0,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,     0,    41,    42,     0,     0,    43,    44,    45,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    46,     0,     0,
    47,    48,     0,    49,    50,     0,    51,     0,    52,    53,
    54,    55,    56,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    57,    58,    59,     0,     3,     4,     5,     6,     7,     0,
     0,     0,     8,     9,     0,  -505,  -505,    10,     0,    11,
    12,    13,    14,    15,    16,    17,     0,     0,     0,     0,
    18,    19,    20,    21,    22,    23,    24,     0,     0,    25,
     0,     0,     0,     0,     0,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
     0,    41,    42,     0,     0,    43,    44,    45,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    46,     0,     0,    47,
    48,     0,    49,    50,     0,    51,     0,    52,    53,    54,
    55,    56,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    57,
    58,    59,     0,     3,     4,     5,     0,     7,     0,     0,
     0,     8,     9,     0,     0,   403,    10,     0,    11,    12,
    13,    14,    15,    16,    17,     0,     0,     0,     0,    18,
    19,    20,    21,    22,    23,    24,     0,     0,    25,     0,
     0,     0,     0,     0,     0,    27,     0,     0,    30,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
    41,    42,     0,     0,    43,    44,    45,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   206,     0,     0,   106,    48,
     0,    49,    50,     0,     0,     0,    52,    53,    54,    55,
    56,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    57,    58,
    59,     0,     0,     0,     0,   116,   117,   118,   119,   120,
   121,   122,   123,   215,   216,   124,   125,   126,   127,   128,
     0,     0,   129,   130,   131,   132,   133,   134,   135,     0,
     0,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
   155,   156,   157,   158,    34,    35,   159,    37,     0,     0,
     0,     0,     0,     0,     0,     0,   160,   161,     0,   162,
   163,   164,   165,     0,   166,   167,     0,     0,   168,     0,
     0,     0,   169,   170,   171,   172,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   173,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     0,     0,   184,     0,     0,  -482,  -482,  -482,     0,  -482,
     0,   185,   186,  -482,  -482,     0,     0,     0,  -482,     0,
  -482,  -482,  -482,  -482,  -482,  -482,  -482,     0,  -482,     0,
     0,  -482,  -482,  -482,  -482,  -482,  -482,  -482,     0,     0,
     0,     0,     0,     0,     0,     0,     0,  -482,     0,     0,
  -482,  -482,  -482,  -482,  -482,  -482,  -482,  -482,  -482,  -482,
  -482,     0,  -482,  -482,     0,  -482,  -482,  -482,  -482,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,  -482,     0,     0,
  -482,  -482,     0,  -482,  -482,     0,  -482,  -482,  -482,  -482,
  -482,  -482,  -482,     0,     0,     0,     0,     0,     0,     0,
  -483,  -483,  -483,     0,  -483,     0,     0,     0,  -483,  -483,
  -482,  -482,  -482,  -483,  -482,  -483,  -483,  -483,  -483,  -483,
  -483,  -483,  -482,  -483,     0,     0,  -483,  -483,  -483,  -483,
  -483,  -483,  -483,     0,     0,     0,     0,     0,     0,     0,
     0,     0,  -483,     0,     0,  -483,  -483,  -483,  -483,  -483,
  -483,  -483,  -483,  -483,  -483,  -483,     0,  -483,  -483,     0,
  -483,  -483,  -483,  -483,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,  -483,     0,     0,  -483,  -483,     0,  -483,  -483,
     0,  -483,  -483,  -483,  -483,  -483,  -483,  -483,     0,     0,
     0,     0,     0,     0,     0,  -485,  -485,  -485,     0,  -485,
     0,     0,     0,  -485,  -485,  -483,  -483,  -483,  -485,  -483,
  -485,  -485,  -485,  -485,  -485,  -485,  -485,  -483,     0,     0,
     0,  -485,  -485,  -485,  -485,  -485,  -485,  -485,     0,     0,
     0,     0,     0,     0,     0,     0,     0,  -485,     0,     0,
  -485,  -485,  -485,  -485,  -485,  -485,  -485,  -485,  -485,  -485,
  -485,     0,  -485,  -485,     0,  -485,  -485,  -485,  -485,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,  -485,   716,     0,
  -485,  -485,     0,  -485,  -485,     0,  -485,  -485,  -485,  -485,
  -485,  -485,  -485,     0,     0,     0,     0,   -86,     0,     0,
  -486,  -486,  -486,     0,  -486,     0,     0,     0,  -486,  -486,
  -485,  -485,  -485,  -486,     0,  -486,  -486,  -486,  -486,  -486,
  -486,  -486,  -485,     0,     0,     0,  -486,  -486,  -486,  -486,
  -486,  -486,  -486,     0,     0,     0,     0,     0,     0,     0,
     0,     0,  -486,     0,     0,  -486,  -486,  -486,  -486,  -486,
  -486,  -486,  -486,  -486,  -486,  -486,     0,  -486,  -486,     0,
  -486,  -486,  -486,  -486,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,  -486,   668,     0,  -486,  -486,     0,  -486,  -486,
     0,  -486,  -486,  -486,  -486,  -486,  -486,  -486,     0,     0,
     0,     0,   -88,     0,     0,  -253,  -253,  -253,     0,  -253,
     0,     0,     0,  -253,  -253,  -486,  -486,  -486,  -253,     0,
  -253,  -253,  -253,  -253,  -253,  -253,  -253,  -486,     0,     0,
     0,  -253,  -253,  -253,  -253,  -253,  -253,  -253,     0,     0,
     0,     0,     0,     0,     0,     0,     0,  -253,     0,     0,
  -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,
  -253,     0,  -253,  -253,     0,  -253,  -253,  -253,  -253,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,  -253,     0,     0,
  -253,  -253,     0,  -253,  -253,     0,  -253,  -253,  -253,  -253,
  -253,  -253,  -253,     0,     0,     0,     0,     0,     0,     0,
  -253,  -253,  -253,     0,  -253,     0,     0,     0,  -253,  -253,
  -253,  -253,  -253,  -253,     0,  -253,  -253,  -253,  -253,  -253,
  -253,  -253,   246,     0,     0,     0,  -253,  -253,  -253,  -253,
  -253,  -253,  -253,     0,     0,     0,     0,     0,     0,     0,
     0,     0,  -253,     0,     0,  -253,  -253,  -253,  -253,  -253,
  -253,  -253,  -253,  -253,  -253,  -253,     0,  -253,  -253,     0,
  -253,  -253,  -253,  -253,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,  -253,     0,     0,  -253,  -253,     0,  -253,  -253,
     0,  -253,  -253,  -253,  -253,  -253,  -253,  -253,     0,     0,
     0,     0,     0,     0,     0,  -487,  -487,  -487,     0,  -487,
     0,     0,     0,  -487,  -487,  -253,  -253,  -253,  -487,     0,
  -487,  -487,  -487,  -487,  -487,  -487,  -487,   249,     0,     0,
     0,  -487,  -487,  -487,  -487,  -487,  -487,  -487,     0,     0,
     0,     0,     0,     0,     0,     0,     0,  -487,     0,     0,
  -487,  -487,  -487,  -487,  -487,  -487,  -487,  -487,  -487,  -487,
  -487,     0,  -487,  -487,     0,  -487,  -487,  -487,  -487,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,  -487,     0,     0,
  -487,  -487,     0,  -487,  -487,     0,  -487,  -487,  -487,  -487,
  -487,  -487,  -487,     0,     0,     0,     0,     0,     0,     0,
  -488,  -488,  -488,     0,  -488,     0,     0,     0,  -488,  -488,
  -487,  -487,  -487,  -488,     0,  -488,  -488,  -488,  -488,  -488,
  -488,  -488,  -487,     0,     0,     0,  -488,  -488,  -488,  -488,
  -488,  -488,  -488,     0,     0,     0,     0,     0,     0,     0,
     0,     0,  -488,     0,     0,  -488,  -488,  -488,  -488,  -488,
  -488,  -488,  -488,  -488,  -488,  -488,     0,  -488,  -488,     0,
  -488,  -488,  -488,  -488,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,  -488,     0,     0,  -488,  -488,     0,  -488,  -488,
     0,  -488,  -488,  -488,  -488,  -488,  -488,  -488,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,  -488,  -488,  -488,   116,   117,
   118,   119,   120,   121,   122,   123,     0,  -488,   124,   125,
   126,   127,   128,     0,     0,   129,   130,   131,   132,   133,
   134,   135,     0,     0,   136,   137,   138,   192,   193,   194,
   195,   143,   144,   145,   146,   147,   148,   149,   150,   151,
   152,   153,   154,   196,   197,   198,   158,   289,   290,   199,
   291,     0,     0,     0,     0,     0,     0,     0,     0,   160,
   161,     0,   162,   163,   164,   165,     0,   166,   167,     0,
     0,   168,     0,     0,     0,   169,   170,   171,   172,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   173,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   174,   175,   176,   177,   178,   179,   180,
   181,   182,   183,     0,     0,   184,   116,   117,   118,   119,
   120,   121,   122,   123,   185,     0,   124,   125,   126,   127,
   128,     0,     0,   129,   130,   131,   132,   133,   134,   135,
     0,     0,   136,   137,   138,   192,   193,   194,   195,   143,
   144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
   154,   196,   197,   198,   158,   253,     0,   199,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   160,   161,     0,
   162,   163,   164,   165,     0,   166,   167,     0,     0,   168,
     0,     0,     0,   169,   170,   171,   172,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   173,     0,   200,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   174,   175,   176,   177,   178,   179,   180,   181,   182,
   183,     0,     0,   184,   116,   117,   118,   119,   120,   121,
   122,   123,   185,     0,   124,   125,   126,   127,   128,     0,
     0,   129,   130,   131,   132,   133,   134,   135,     0,     0,
   136,   137,   138,   192,   193,   194,   195,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   196,
   197,   198,   158,     0,     0,   199,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   160,   161,     0,   162,   163,
   164,   165,     0,   166,   167,     0,     0,   168,     0,     0,
     0,   169,   170,   171,   172,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   173,     0,   200,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   174,
   175,   176,   177,   178,   179,   180,   181,   182,   183,     0,
     0,   184,   116,   117,   118,   119,   120,   121,   122,   123,
   185,     0,   124,   125,   126,   127,   128,     0,     0,   129,
   130,   131,   132,   133,   134,   135,     0,     0,   136,   137,
   138,   192,   193,   194,   195,   143,   144,   145,   146,   147,
   148,   149,   150,   151,   152,   153,   154,   196,   197,   198,
   158,     0,     0,   199,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   160,   161,     0,   162,   163,   164,   165,
     0,   166,   167,     0,     0,   168,     0,     0,     0,   169,
   170,   171,   172,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   173,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   174,   175,   176,
   177,   178,   179,   180,   181,   182,   183,     0,     0,   184,
     0,     3,     4,     5,     0,     7,     0,     0,   185,     8,
     9,     0,     0,     0,    10,     0,    11,    12,    13,    96,
    97,    16,    17,     0,     0,     0,     0,    98,    19,    20,
    21,    22,    23,    24,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    27,     0,     0,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
     0,   230,    43,    44,    45,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   206,     0,     0,   106,    48,     0,    49,
    50,     0,   231,   232,    52,    53,    54,    55,    56,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     5,     6,     7,    57,   233,    59,     8,
     9,     0,     0,     0,    10,   420,    11,    12,    13,    14,
    15,    16,    17,     0,     0,     0,     0,    18,    19,    20,
    21,    22,    23,    24,     0,     0,    25,     0,     0,     0,
     0,     0,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
     0,     0,    43,    44,    45,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    46,     0,     0,    47,    48,     0,    49,
    50,     0,    51,     0,    52,    53,    54,    55,    56,     0,
     0,     0,     0,     0,     0,     0,     3,     4,     5,     0,
     7,     0,     0,     0,     8,     9,    57,    58,    59,    10,
     0,    11,    12,    13,    14,    15,    16,    17,     0,     0,
     0,     0,    18,    19,    20,    21,    22,    23,    24,     0,
     0,    25,     0,     0,     0,     0,     0,     0,    27,     0,
     0,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,     0,    41,    42,     0,   230,    43,    44,    45,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   206,     0,
     0,   106,    48,     0,    49,    50,     0,   622,   232,    52,
    53,    54,    55,    56,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     5,     0,     7,     0,     0,     0,     8,
     9,    57,    58,    59,    10,     0,    11,    12,    13,    96,
    97,    16,    17,     0,     0,     0,     0,    98,    19,    20,
    21,    22,    23,    24,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    27,     0,     0,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
     0,   230,    43,    44,    45,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   206,     0,     0,   106,    48,     0,    49,
    50,     0,   231,   232,    52,    53,    54,    55,    56,     0,
     0,     0,     0,     0,     0,     0,     3,     4,     5,     0,
     7,     0,     0,     0,     8,     9,    57,   233,    59,    10,
     0,    11,    12,    13,    96,    97,    16,    17,     0,     0,
     0,     0,    98,    19,    20,    21,    22,    23,    24,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
     0,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,     0,    41,    42,     0,   230,    43,    44,    45,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   206,     0,
     0,   106,   422,     0,    49,    50,     0,   231,   232,    52,
    53,    54,    55,    56,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     5,     0,     7,     0,     0,     0,     8,
     9,    57,   233,    59,    10,     0,    11,    12,    13,    96,
    97,    16,    17,     0,     0,     0,     0,    98,    99,   100,
    21,    22,    23,    24,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    27,     0,     0,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
     0,   230,    43,    44,    45,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   206,     0,     0,   106,    48,     0,    49,
    50,     0,   614,   232,    52,    53,    54,    55,    56,     0,
     0,     0,     0,     0,     0,     0,     3,     4,     5,     0,
     7,     0,     0,     0,     8,     9,    57,   233,    59,    10,
     0,    11,    12,    13,    96,    97,    16,    17,     0,     0,
     0,     0,    98,    99,   100,    21,    22,    23,    24,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
     0,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,     0,    41,    42,     0,   230,    43,    44,    45,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   206,     0,
     0,   106,    48,     0,    49,    50,     0,   618,   232,    52,
    53,    54,    55,    56,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     5,     0,     7,     0,     0,     0,     8,
     9,    57,   233,    59,    10,     0,    11,    12,    13,    96,
    97,    16,    17,     0,     0,     0,     0,    98,    19,    20,
    21,    22,    23,    24,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    27,     0,     0,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
     0,   230,    43,    44,    45,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   206,     0,     0,   106,    48,     0,    49,
    50,     0,   614,   232,    52,    53,    54,    55,    56,     0,
     0,     0,     0,     0,     0,     0,     3,     4,     5,     0,
     7,     0,     0,     0,     8,     9,    57,   233,    59,    10,
     0,    11,    12,    13,    96,    97,    16,    17,     0,     0,
     0,     0,    98,    99,   100,    21,    22,    23,    24,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
     0,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,     0,    41,    42,     0,   230,    43,    44,    45,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   206,     0,
     0,   106,    48,     0,    49,    50,     0,   809,   232,    52,
    53,    54,    55,    56,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     5,     0,     7,     0,     0,     0,     8,
     9,    57,   233,    59,    10,     0,    11,    12,    13,    96,
    97,    16,    17,     0,     0,     0,     0,    98,    99,   100,
    21,    22,    23,    24,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    27,     0,     0,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
     0,   230,    43,    44,    45,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   206,     0,     0,   106,    48,     0,    49,
    50,     0,   812,   232,    52,    53,    54,    55,    56,     0,
     0,     0,     0,     0,     0,     0,     3,     4,     5,     0,
     7,     0,     0,     0,     8,     9,    57,   233,    59,    10,
     0,    11,    12,    13,    96,    97,    16,    17,     0,     0,
     0,     0,    98,    99,   100,    21,    22,    23,    24,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
     0,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,     0,    41,    42,     0,   230,    43,    44,    45,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   206,     0,
     0,   106,    48,     0,    49,    50,     0,   817,   232,    52,
    53,    54,    55,    56,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     5,     0,     7,     0,     0,     0,     8,
     9,    57,   233,    59,    10,     0,    11,    12,    13,    96,
    97,    16,    17,     0,     0,     0,     0,    98,    99,   100,
    21,    22,    23,    24,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    27,     0,     0,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
     0,   230,    43,    44,    45,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   206,     0,     0,   106,    48,     0,    49,
    50,     0,   877,   232,    52,    53,    54,    55,    56,     0,
     0,     0,     0,     0,     0,     0,     3,     4,     5,     0,
     7,     0,     0,     0,     8,     9,    57,   233,    59,    10,
     0,    11,    12,    13,    96,    97,    16,    17,     0,     0,
     0,     0,    98,    99,   100,    21,    22,    23,    24,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
     0,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,     0,    41,    42,     0,   230,    43,    44,    45,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   206,     0,
     0,   106,    48,     0,    49,    50,     0,   879,   232,    52,
    53,    54,    55,    56,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     5,     0,     7,     0,     0,     0,     8,
     9,    57,   233,    59,    10,     0,    11,    12,    13,    96,
    97,    16,    17,     0,     0,     0,     0,    98,    99,   100,
    21,    22,    23,    24,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    27,     0,     0,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
     0,   230,    43,    44,    45,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   206,     0,     0,   106,    48,     0,    49,
    50,     0,   898,   232,    52,    53,    54,    55,    56,     0,
     0,     0,     0,     0,     0,     0,     3,     4,     5,     0,
     7,     0,     0,     0,     8,     9,    57,   233,    59,    10,
     0,    11,    12,    13,    96,    97,    16,    17,     0,     0,
     0,     0,    98,    19,    20,    21,    22,    23,    24,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
     0,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,     0,    41,    42,     0,   230,    43,    44,    45,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   206,     0,
     0,   106,    48,     0,    49,    50,     0,   274,     0,    52,
    53,    54,    55,    56,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     5,     0,     7,     0,     0,     0,     8,
     9,    57,   233,    59,    10,     0,    11,    12,    13,    14,
    15,    16,    17,     0,     0,     0,     0,    18,    19,    20,
    21,    22,    23,    24,     0,     0,    25,     0,     0,     0,
     0,     0,     0,    27,     0,     0,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
     0,     0,    43,    44,    45,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   206,     0,     0,   106,    48,     0,    49,
    50,     0,     0,     0,    52,    53,    54,    55,    56,     0,
     0,     0,     0,     0,     0,     0,     3,     4,     5,     0,
     7,     0,     0,     0,     8,     9,    57,    58,    59,    10,
     0,    11,    12,    13,    96,    97,    16,    17,     0,     0,
     0,     0,    98,    99,   100,    21,    22,    23,    24,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
     0,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,     0,    41,    42,     0,   230,    43,    44,    45,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   206,     0,
     0,   106,    48,     0,    49,    50,     0,     0,     0,    52,
    53,    54,    55,    56,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     5,     0,     7,     0,     0,     0,     8,
     9,    57,   233,    59,    10,     0,    11,    12,    13,    14,
    15,    16,    17,     0,     0,     0,     0,    18,    19,    20,
    21,    22,    23,    24,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    27,     0,     0,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
     0,     0,    43,    44,    45,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   206,     0,     0,   106,    48,     0,    49,
    50,     0,   488,     0,    52,    53,    54,    55,    56,     0,
     0,     0,     0,     0,     0,     0,     3,     4,     5,     0,
     7,     0,     0,     0,     8,     9,    57,   233,    59,    10,
     0,    11,    12,    13,    96,    97,    16,    17,     0,     0,
     0,     0,    98,    99,   100,    21,    22,    23,    24,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
     0,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,     0,    41,    42,     0,     0,    43,    44,    45,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   206,     0,
     0,   106,    48,     0,    49,    50,     0,   596,     0,    52,
    53,    54,    55,    56,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     5,     0,     7,     0,     0,     0,     8,
     9,    57,   233,    59,    10,     0,    11,    12,    13,    96,
    97,    16,    17,     0,     0,     0,     0,    98,    99,   100,
    21,    22,    23,    24,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    27,     0,     0,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
     0,     0,    43,    44,    45,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   206,     0,     0,   106,    48,     0,    49,
    50,     0,   646,     0,    52,    53,    54,    55,    56,     0,
     0,     0,     0,     0,     0,     0,     3,     4,     5,     0,
     7,     0,     0,     0,     8,     9,    57,   233,    59,    10,
     0,    11,    12,    13,    96,    97,    16,    17,     0,     0,
     0,     0,    98,    99,   100,    21,    22,    23,    24,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
     0,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,     0,    41,    42,     0,     0,    43,    44,    45,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   206,     0,
     0,   106,    48,     0,    49,    50,     0,   488,     0,    52,
    53,    54,    55,    56,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     5,     0,     7,     0,     0,     0,     8,
     9,    57,   233,    59,    10,     0,    11,    12,    13,    96,
    97,    16,    17,     0,     0,     0,     0,    98,    99,   100,
    21,    22,    23,    24,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    27,     0,     0,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
     0,     0,    43,    44,    45,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   206,     0,     0,   106,    48,     0,    49,
    50,     0,   760,     0,    52,    53,    54,    55,    56,     0,
     0,     0,     0,     0,     0,     0,     3,     4,     5,     0,
     7,     0,     0,     0,     8,     9,    57,   233,    59,    10,
     0,    11,    12,    13,    96,    97,    16,    17,     0,     0,
     0,     0,    98,    99,   100,    21,    22,    23,    24,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
     0,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,     0,    41,    42,     0,     0,    43,    44,    45,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   206,     0,
     0,   106,    48,     0,    49,    50,     0,   803,     0,    52,
    53,    54,    55,    56,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     5,     0,     7,     0,     0,     0,     8,
     9,    57,   233,    59,    10,     0,    11,    12,    13,    96,
    97,    16,    17,     0,     0,     0,     0,    98,    99,   100,
    21,    22,    23,    24,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    27,     0,     0,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
     0,     0,    43,    44,    45,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   206,     0,     0,   106,    48,     0,    49,
    50,     0,     0,     0,    52,    53,    54,    55,    56,     0,
     0,     0,     0,     0,     0,     0,     3,     4,     5,     0,
     7,     0,     0,     0,     8,     9,    57,   233,    59,    10,
     0,    11,    12,    13,    14,    15,    16,    17,     0,     0,
     0,     0,    18,    19,    20,    21,    22,    23,    24,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
     0,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,     0,    41,    42,     0,     0,    43,    44,    45,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   206,     0,
     0,   106,    48,     0,    49,    50,     0,     0,     0,    52,
    53,    54,    55,    56,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     5,     0,     7,     0,     0,     0,     8,
     9,    57,   233,    59,    10,     0,    11,    12,    13,    96,
    97,    16,    17,     0,     0,     0,     0,    98,    99,   100,
    21,    22,    23,    24,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   101,     0,     0,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     0,    41,    42,
     0,     0,     0,     0,    45,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   223,     0,     0,    47,    48,     0,    49,
    50,     0,    51,     0,    52,    53,    54,    55,    56,     0,
     0,     0,     3,     4,     5,     0,     7,     0,   757,     0,
     8,     9,     0,     0,     0,    10,   107,    11,    12,    13,
    96,    97,    16,    17,     0,     0,     0,     0,    98,    99,
   100,    21,    22,    23,    24,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   101,     0,     0,    30,    31,   102,
    33,    34,    35,   103,    37,    38,    39,    40,     0,    41,
    42,     0,     0,     0,     0,    45,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   104,     0,     0,   105,     0,     0,   106,    48,     0,
    49,    50,     0,     0,     0,    52,    53,    54,    55,    56,
     0,     0,     0,     3,     4,     5,     0,     7,     0,     0,
     0,     8,     9,     0,     0,     0,    10,   107,    11,    12,
    13,    96,    97,    16,    17,     0,     0,     0,     0,    98,
    99,   100,    21,    22,    23,    24,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   101,     0,     0,    30,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
    41,    42,     0,     0,     0,     0,    45,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   223,     0,     0,    47,    48,
     0,    49,    50,     0,    51,     0,    52,    53,    54,    55,
    56,     0,     0,     0,     3,     4,     5,     0,     7,     0,
     0,     0,     8,     9,     0,     0,     0,    10,   107,    11,
    12,    13,    96,    97,    16,    17,     0,     0,     0,     0,
    98,    99,   100,    21,    22,    23,    24,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   101,     0,     0,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
     0,    41,    42,     0,     0,     0,     0,    45,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   284,     0,     0,   319,
    48,     0,    49,    50,     0,   320,     0,    52,    53,    54,
    55,    56,     0,     0,     0,     3,     4,     5,     0,     7,
     0,     0,     0,     8,     9,     0,     0,     0,    10,   107,
    11,    12,    13,    96,    97,    16,    17,     0,     0,     0,
     0,    98,    99,   100,    21,    22,    23,    24,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   101,     0,     0,
    30,    31,   102,    33,    34,    35,   103,    37,    38,    39,
    40,     0,    41,    42,     0,     0,     0,     0,    45,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   105,     0,     0,
   106,    48,     0,    49,    50,     0,     0,     0,    52,    53,
    54,    55,    56,     0,     0,     0,     3,     4,     5,     0,
     7,     0,     0,     0,     8,     9,     0,     0,     0,    10,
   107,    11,    12,    13,    96,    97,    16,    17,     0,     0,
     0,     0,    98,    99,   100,    21,    22,    23,    24,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   101,     0,
     0,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,     0,    41,    42,     0,     0,     0,     0,    45,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   284,     0,
     0,   106,    48,     0,    49,    50,     0,     0,     0,    52,
    53,    54,    55,    56,     0,     0,     0,     3,     4,     5,
     0,     7,     0,     0,     0,     8,     9,     0,     0,     0,
    10,   107,    11,    12,    13,    96,    97,    16,    17,     0,
     0,     0,     0,    98,    99,   100,    21,    22,    23,    24,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   101,
     0,     0,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,     0,    41,    42,     0,     0,     0,     0,
    45,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   838,
     0,     0,   106,    48,     0,    49,    50,     0,     0,     0,
    52,    53,    54,    55,    56,     0,   263,   264,     0,     0,
   265,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   160,   161,   107,   162,   163,   164,   165,     0,   166,   167,
     0,     0,   168,     0,     0,     0,   169,   170,   171,   172,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   173,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   174,   175,   176,   177,   178,   179,
   180,   181,   182,   183,   526,   527,   184,     0,   528,     0,
     0,     0,     0,     0,     0,   185,     0,     0,   160,   161,
     0,   162,   163,   164,   165,     0,   166,   167,     0,     0,
   168,     0,     0,     0,   169,   170,   171,   172,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   173,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   174,   175,   176,   177,   178,   179,   180,   181,
   182,   183,   533,   264,   184,     0,   534,     0,     0,     0,
     0,     0,     0,   185,     0,     0,   160,   161,     0,   162,
   163,   164,   165,     0,   166,   167,     0,     0,   168,     0,
     0,     0,   169,   170,   171,   172,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   173,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
   551,   527,   184,     0,   552,     0,     0,     0,     0,     0,
     0,   185,     0,     0,   160,   161,     0,   162,   163,   164,
   165,     0,   166,   167,     0,     0,   168,     0,     0,     0,
   169,   170,   171,   172,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   173,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   174,   175,
   176,   177,   178,   179,   180,   181,   182,   183,   588,   527,
   184,     0,   589,     0,     0,     0,     0,     0,     0,   185,
     0,     0,   160,   161,     0,   162,   163,   164,   165,     0,
   166,   167,     0,     0,   168,     0,     0,     0,   169,   170,
   171,   172,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   173,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   174,   175,   176,   177,
   178,   179,   180,   181,   182,   183,   591,   264,   184,     0,
   592,     0,     0,     0,     0,     0,     0,   185,     0,     0,
   160,   161,     0,   162,   163,   164,   165,     0,   166,   167,
     0,     0,   168,     0,     0,     0,   169,   170,   171,   172,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   173,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   174,   175,   176,   177,   178,   179,
   180,   181,   182,   183,   605,   527,   184,     0,   606,     0,
     0,     0,     0,     0,     0,   185,     0,     0,   160,   161,
     0,   162,   163,   164,   165,     0,   166,   167,     0,     0,
   168,     0,     0,     0,   169,   170,   171,   172,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   173,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   174,   175,   176,   177,   178,   179,   180,   181,
   182,   183,   608,   264,   184,     0,   609,     0,     0,     0,
     0,     0,     0,   185,     0,     0,   160,   161,     0,   162,
   163,   164,   165,     0,   166,   167,     0,     0,   168,     0,
     0,     0,   169,   170,   171,   172,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   173,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
   648,   527,   184,     0,   649,     0,     0,     0,     0,     0,
     0,   185,     0,     0,   160,   161,     0,   162,   163,   164,
   165,     0,   166,   167,     0,     0,   168,     0,     0,     0,
   169,   170,   171,   172,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   173,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   174,   175,
   176,   177,   178,   179,   180,   181,   182,   183,   651,   264,
   184,     0,   652,     0,     0,     0,     0,     0,     0,   185,
     0,     0,   160,   161,     0,   162,   163,   164,   165,     0,
   166,   167,     0,     0,   168,     0,     0,     0,   169,   170,
   171,   172,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   173,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   174,   175,   176,   177,
   178,   179,   180,   181,   182,   183,   884,   527,   184,     0,
   885,     0,     0,     0,     0,     0,     0,   185,     0,     0,
   160,   161,     0,   162,   163,   164,   165,     0,   166,   167,
     0,     0,   168,     0,     0,     0,   169,   170,   171,   172,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   173,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   174,   175,   176,   177,   178,   179,
   180,   181,   182,   183,   887,   264,   184,     0,   888,     0,
     0,     0,     0,     0,     0,   185,     0,     0,   160,   161,
     0,   162,   163,   164,   165,     0,   166,   167,     0,     0,
   168,     0,     0,     0,   169,   170,   171,   172,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   173,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   174,   175,   176,   177,   178,   179,   180,   181,
   182,   183,     0,     0,   184,     0,     0,     0,     0,     0,
     0,     0,     0,   185,   323,   324,   325,   326,   327,   328,
   329,   330,   331,   332,   333,   334,   335,     0,     0,   336,
   337,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   338,     0,   339,   340,   341,   342,   343,
   344,   345,   346,   347,   348,     0,   323,   324,   325,   326,
   327,   328,   329,   330,   331,   332,   333,   334,   335,     0,
   255,   336,   337,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   338,     0,   339,   340,   341,
   342,   343,   344,   345,   346,   347,   348,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   611,   323,   324,
   325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
   335,     0,     0,   336,   337,   323,   324,   325,   326,   327,
   328,   329,   330,   331,   332,   333,   334,   335,     0,     0,
   336,   337,     0,     0,     0,     0,     0,   338,   763,   339,
   340,   341,   342,   343,   344,   345,   346,   347,   348,     0,
     0,     0,     0,     0,   338,     0,   339,   340,   341,   342,
   343,   344,   345,   346,   347,   348,   323,   324,   325,   326,
   327,   328,   329,     0,     0,   332,   333,     0,     0,     0,
     0,   336,   337,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   339,   340,   341,
   342,   343,   344,   345,   346,   347,   348
};

static const short yycheck[] = {    14,
    15,    13,    58,    18,   214,     5,   380,    14,    15,    26,
   378,    18,     6,    14,    15,    20,     1,    18,    14,    15,
   399,   249,    18,     3,     4,   350,     5,     6,    43,    44,
   405,   105,    26,    13,    49,    50,   322,     2,    15,    61,
   698,    18,    49,    58,    59,    14,    15,    26,    59,    18,
    13,   376,    52,   583,    50,     1,   537,   538,   418,   539,
   540,    10,    47,   593,   695,    13,    47,   698,    51,   394,
    67,    51,    13,    52,     9,    10,    26,    20,    86,   295,
    25,    50,   298,   408,    50,    13,   269,    75,    93,    84,
    70,    59,   103,    25,    62,   100,   456,   105,    36,    37,
    59,    27,    36,    37,    86,   133,    25,   670,    95,   114,
    84,    86,    61,   676,    26,    54,    87,   101,   115,   116,
   117,   106,   110,   105,   122,   112,    86,    45,    94,    95,
   105,   126,    26,   101,   102,    25,   319,    28,   133,   464,
    25,    25,   101,   102,   107,   105,   112,    50,   105,   115,
    93,    54,   126,   128,   132,    25,   574,   128,   576,   107,
   109,    25,   132,   109,    25,   110,   107,   130,   128,   105,
   133,   134,   122,   132,    15,   833,    17,    84,   110,   107,
   130,   107,   550,   128,    86,   133,   134,   133,   134,   127,
    84,   110,   133,   134,   568,   217,   128,   128,   130,   133,
   200,   569,   833,   105,   684,   133,   134,   133,   134,   128,
   122,   130,    84,    86,    52,   230,   231,   232,   233,   126,
   110,   200,    60,    61,   128,   110,   110,   128,   122,   133,
   124,   246,   126,   248,   249,   105,   130,   131,   128,   246,
   110,   248,   249,   128,   128,   246,   110,   248,   249,   110,
   246,    59,   248,   249,   126,    59,   472,   127,   128,   274,
   130,   317,   743,   744,   128,    97,   322,   128,   217,   246,
    86,   248,   249,   258,   804,    57,    58,   246,   128,   248,
   249,    86,   130,   133,   269,   128,   221,   222,   269,   105,
   133,   275,    54,   101,   102,   103,   582,   101,   102,   103,
   105,   375,   317,   359,   360,   133,   134,   322,   323,   324,
   325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
   335,   336,   337,   128,   339,   340,   341,   342,   343,   344,
   345,   346,   347,   348,   319,   350,    86,   320,   319,    86,
   320,   240,   307,   350,   359,   360,   130,   312,   317,    84,
    84,   105,    84,   322,   729,   105,    84,    15,   105,    17,
    52,   376,    54,    86,    84,    25,    84,   785,   786,   376,
   349,   731,   790,   308,   309,   310,   311,   392,   738,   394,
    54,   396,   397,    36,    37,    84,   401,   394,   598,    26,
   124,   126,   126,   408,   126,   134,   375,   131,   126,   414,
   415,   408,   630,   418,   124,   390,   126,   422,   126,   777,
   130,   131,    25,   398,   393,   400,   433,   791,    87,   415,
   435,   422,   371,    57,    58,   804,   422,   126,   407,    84,
   445,   349,   401,   351,   352,   353,   354,    25,   453,   433,
    84,   456,   426,   458,   428,   105,    26,    84,   866,   464,
   110,   436,   128,    86,   433,   404,   405,   464,   128,   377,
    86,    86,   477,   478,   443,   449,   450,   127,   128,   837,
   122,   126,   105,   488,   130,   393,    67,   395,   127,   105,
   105,   127,   126,   125,   463,   122,   127,   124,    67,   126,
    54,   409,   123,   130,   131,   128,   103,   857,    14,    15,
   515,    84,   128,   128,    84,   127,   128,     1,     2,     3,
     4,   103,    67,   412,     8,     9,    10,   105,    67,    13,
    14,    15,   110,    67,    18,   530,   444,    82,    83,    59,
   128,    25,    93,   893,   113,   114,   115,   116,   117,   127,
   128,   124,   122,   126,   124,   870,   126,   465,   131,   554,
   130,   131,    10,    47,    48,    49,     8,    51,   113,   114,
   115,   116,   117,   580,    58,    13,    17,   582,     8,     9,
    10,   101,   102,   103,   558,    10,    70,   128,   278,   579,
   280,   596,   282,   793,   127,    25,   580,   530,   128,   799,
   127,   123,   535,   127,   537,   538,   132,   128,   547,   614,
   579,   580,    84,   618,   128,   128,   127,   622,    48,    10,
   104,   667,   106,   582,   549,   630,   672,   673,    59,   125,
   125,   570,    50,   630,    52,    53,    54,    55,   105,   128,
   128,   646,    50,    50,   630,    52,    53,    54,    55,   128,
    50,    84,   124,    15,   126,     1,     2,   659,   663,   131,
   665,   125,   667,   668,   125,   640,   128,   672,   673,    10,
   101,   102,   103,    10,   104,    95,    94,    95,    39,    40,
    41,    42,    43,   128,   127,   610,   691,    94,    95,   659,
   127,   124,   128,   126,   112,   127,   123,   115,   131,   127,
    10,    47,   125,   127,    10,   112,    84,    87,   115,     9,
   127,   716,   130,   718,   719,    10,    10,   123,   692,   724,
   110,   205,   768,   631,   632,   128,   731,   128,   617,   127,
   735,    10,   128,   738,    10,   710,   128,   221,   222,   628,
    84,   127,    10,    10,   735,    10,   124,   686,   126,   735,
   725,   726,   727,   131,   123,   760,    54,   123,   763,    10,
   106,    84,   246,   768,   248,   249,    10,   125,   742,    84,
   230,   231,   232,    84,   258,   128,   735,     0,     0,    70,
   124,   755,   126,   873,   759,   269,     4,   131,   713,   659,
   729,   221,   222,    77,   872,    80,   801,     5,   803,   774,
   775,   124,   791,   126,   809,   568,    84,   812,   131,   124,
   695,   126,   817,   124,    84,   126,   131,   695,    61,    -1,
   131,   796,   792,   307,   308,   309,   310,   311,   312,   313,
   314,    52,    53,   317,    55,   319,   320,    -1,   322,    60,
    61,   730,    -1,    -1,   733,   734,   124,    -1,   126,    84,
   855,    -1,   857,   131,   124,   830,   126,   317,    -1,   205,
    -1,   131,   322,    -1,    -1,   870,   350,    -1,   843,   855,
   845,    -1,   877,   870,   879,   359,   360,   852,   308,   309,
   310,   311,    -1,   313,   314,    -1,    67,    -1,   893,   124,
    -1,   126,   376,   898,   378,   379,   131,    -1,    -1,    -1,
   869,    82,    83,    -1,    -1,    -1,   390,    -1,    -1,    -1,
   394,    -1,   258,    -1,   398,    -1,   400,    -1,    -1,    -1,
    -1,    -1,    -1,   269,   408,   814,   815,    -1,    -1,   110,
   111,   112,   113,   114,   115,   116,   117,    -1,   422,    -1,
    -1,   401,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   379,
    -1,    -1,   436,    -1,   414,   415,    -1,    -1,   418,    -1,
    -1,   307,   422,   871,   853,   854,   312,    -1,    -1,    -1,
   859,    -1,    -1,   319,    -1,    -1,    -1,    -1,    -1,    -1,
   464,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   878,
    -1,    -1,   422,   453,    -1,    -1,   456,    -1,   458,    -1,
    67,    -1,    -1,   892,    -1,    -1,   895,    -1,    -1,    -1,
    -1,   900,    -1,     1,     2,    82,    83,    -1,    -1,    -1,
     8,     9,    10,    -1,    -1,    -1,    14,    15,   488,    -1,
    18,    -1,   378,    -1,    -1,    -1,    -1,    25,    -1,    -1,
    -1,    -1,    -1,    -1,   390,   112,   113,   114,   115,   116,
   117,    -1,   398,    -1,   400,    -1,    -1,    -1,    -1,    47,
    48,    49,    49,    50,     0,   549,   550,    -1,    -1,    -1,
    58,    -1,     8,     9,    10,    -1,    -1,    13,    14,    15,
    -1,    17,    -1,    -1,    -1,   569,    -1,    -1,    -1,    -1,
   436,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    36,    37,    -1,    39,    40,    41,    42,    43,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   104,    -1,   106,   549,
    -1,    -1,   582,    -1,    -1,    -1,   610,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   596,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   630,    -1,    84,    -1,
    -1,    -1,   636,    -1,   614,    -1,   640,    -1,   618,    -1,
    -1,    -1,   622,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   630,   107,    -1,    -1,    -1,   659,    -1,    -1,    -1,    -1,
   610,    -1,    -1,   667,    -1,    -1,    -1,   123,   672,   673,
   126,   127,    -1,    -1,   130,    -1,    -1,   133,   134,    -1,
    -1,    -1,    -1,   663,   550,    -1,   636,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   205,    -1,    -1,
    -1,    -1,    -1,   569,    -1,    -1,   710,    -1,    -1,   713,
    -1,   691,    -1,   221,   222,    -1,    -1,    -1,    -1,    -1,
    -1,   725,   726,   727,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   239,   240,    -1,    -1,    -1,    -1,   246,    -1,
   248,   249,   249,    -1,   724,    -1,    -1,    -1,    -1,    -1,
   258,   731,    -1,    -1,    -1,   759,    -1,    -1,   738,    -1,
    -1,   269,    -1,   713,   768,    -1,    -1,    -1,    -1,    -1,
   774,   775,    -1,   777,   640,    -1,    -1,    -1,    -1,    -1,
   760,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   792,    -1,
    -1,    -1,   796,    -1,    -1,    -1,    -1,    -1,    -1,   307,
   308,   309,   310,   311,   312,   313,   314,    -1,    -1,   317,
    -1,   319,    -1,    -1,   322,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   803,    -1,    -1,   830,    -1,    -1,    -1,
    -1,    -1,   812,   837,    -1,    -1,    -1,   817,    -1,   843,
    -1,   845,   350,   350,   710,    -1,    -1,    -1,   852,    -1,
    -1,   359,   360,    -1,    -1,    -1,    -1,    -1,    -1,   725,
   726,   727,    -1,    -1,    -1,    -1,   870,    -1,   376,   376,
   378,   379,    -1,    -1,    -1,   855,    -1,   857,    -1,   386,
    -1,    -1,   390,    -1,    -1,    -1,   394,   394,    -1,    -1,
   398,    -1,   400,   759,    -1,    -1,    -1,   877,    -1,   879,
   408,   408,    -1,    -1,    -1,   412,    -1,    -1,   774,   775,
    -1,   777,    -1,   893,   422,    -1,    -1,    -1,   898,    -1,
   427,    -1,     1,     2,     3,     4,     5,    -1,   436,    -1,
   796,    -1,    -1,    -1,    13,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   464,   464,    -1,    -1,
    -1,    -1,    -1,    -1,   830,    -1,    -1,    -1,    47,    -1,
    -1,   837,    51,    -1,    -1,    -1,    -1,   843,    -1,   845,
   487,    -1,    -1,    -1,    -1,    -1,   852,    -1,    -1,    -1,
    -1,    70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   106,   535,    -1,
   537,   538,   539,   540,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   549,   550,    -1,    -1,    -1,    -1,    -1,    -1,   556,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,     2,
    -1,   569,    -1,    -1,    -1,   572,    -1,   574,    -1,   576,
    13,    -1,    -1,    -1,    -1,   582,   583,    -1,   585,    -1,
    -1,     1,     2,     3,     4,    -1,   593,    -1,    -1,    -1,
    -1,    -1,    -1,    13,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   610,    -1,    47,    -1,    -1,    -1,    -1,    -1,
   617,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   628,   630,    -1,    -1,    -1,   205,    47,   636,    -1,
    -1,    51,   640,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   667,
    -1,    -1,    -1,   106,   672,   673,    67,    68,    69,    70,
    71,    72,    73,    74,    -1,    76,    77,   684,    -1,   258,
    -1,    82,    83,    -1,    -1,    -1,   106,    -1,    -1,    -1,
   269,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   708,   710,    -1,   711,   713,    -1,   108,   109,   110,
   111,   112,   113,   114,   115,   116,   117,   725,   726,   727,
    -1,    -1,    -1,   730,    -1,    -1,   733,   734,   307,    -1,
    -1,    -1,    -1,   312,    -1,    -1,   743,   744,    -1,    -1,
   319,   320,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   759,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   768,    -1,   205,    -1,    -1,    -1,   774,   775,    -1,   777,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   785,   786,
    -1,    -1,    -1,   790,    -1,   205,    -1,    -1,   796,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   804,    -1,   378,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   814,   815,    -1,
    -1,   390,    -1,    -1,    -1,   258,    -1,    -1,    -1,   398,
    -1,   400,   830,    -1,    -1,    -1,   269,    -1,    -1,   837,
    -1,    -1,    -1,    -1,    -1,   843,    -1,   845,   258,    -1,
    -1,    -1,    -1,    -1,   852,    -1,   853,   854,    -1,   269,
    -1,    -1,   859,    -1,    -1,    -1,    -1,   436,    -1,   866,
    -1,    -1,   870,   870,   307,   872,   873,    -1,    -1,   312,
    -1,   878,    -1,    -1,    -1,    -1,   319,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   892,    -1,   307,   895,    -1,
    -1,    -1,   312,   900,    -1,    -1,    -1,    -1,    -1,   319,
   320,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,
    69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
    79,    -1,    -1,    82,    83,   378,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   390,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   398,    -1,   400,   378,   108,
   109,   110,   111,   112,   113,   114,   115,   116,   117,    -1,
   390,   550,    -1,    -1,    -1,    -1,    -1,    -1,   398,    -1,
   400,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   569,    -1,    -1,   436,    -1,    -1,    -1,    -1,    -1,     0,
     1,    -1,     3,     4,     5,     6,     7,    -1,    -1,    -1,
    11,    12,    -1,    -1,    -1,    16,   436,    18,    19,    20,
    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
    31,    32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,
    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,    50,
    51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
    61,   640,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   659,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
    91,    92,    -1,    94,    -1,    96,    97,    98,    99,   100,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   550,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   118,   119,   120,
    -1,    -1,    -1,    -1,    -1,    -1,   569,    -1,    -1,    -1,
   550,   710,   133,   134,    -1,    -1,    -1,    43,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   725,   726,   727,   569,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    -1,    -1,    82,    83,    -1,    -1,
   759,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   774,   775,   640,   777,    -1,
   106,    -1,   108,   109,   110,   111,   112,   113,   114,   115,
   116,   117,    -1,   792,    -1,    -1,   659,   796,    -1,    -1,
   640,   641,   128,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   659,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   830,    -1,    -1,    -1,    -1,    -1,    -1,   837,    -1,
    -1,    -1,    -1,    -1,   843,    -1,   845,   710,    -1,    -1,
    -1,    -1,    -1,   852,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   725,   726,   727,    -1,    -1,    -1,    -1,    -1,
   710,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   725,   726,   727,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   759,    -1,    -1,    -1,
    67,    68,    69,    70,    71,    72,    73,    -1,    -1,    76,
    77,   774,   775,    -1,   777,    82,    83,    -1,    -1,   759,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   792,
    -1,    -1,    -1,   796,   774,   775,    -1,   777,    -1,    -1,
    -1,   108,   109,   110,   111,   112,   113,   114,   115,   116,
   117,    -1,   792,    -1,    -1,    -1,   796,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   830,    -1,    -1,
    -1,    -1,    -1,    -1,   837,    -1,    -1,    -1,    -1,    -1,
   843,    -1,   845,    -1,    -1,    -1,    -1,    -1,    -1,   852,
   830,    -1,    -1,    -1,    -1,    -1,    -1,   837,    -1,    -1,
     0,    -1,    -1,   843,    -1,   845,    -1,    -1,     8,     9,
    10,    -1,   852,    13,    14,    15,    -1,    17,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    28,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,    -1,    39,
    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,
    70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
    -1,    -1,    82,    83,    84,    -1,    -1,    87,    -1,    -1,
    -1,    -1,    -1,    93,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   106,   107,   108,   109,
   110,   111,   112,   113,   114,   115,   116,   117,    -1,    -1,
    -1,    -1,    -1,   123,   124,   125,   126,   127,   128,     0,
    -1,   131,    -1,   133,   134,    -1,    -1,     8,     9,    10,
    -1,    -1,    -1,    14,    15,    -1,    17,    -1,    -1,    -1,
    -1,    43,    -1,    -1,    -1,    26,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    36,    37,    -1,    39,    40,
    41,    42,    43,    -1,    -1,    67,    68,    69,    70,    71,
    72,    73,    74,    75,    76,    77,    78,    79,    -1,    -1,
    82,    83,    -1,    -1,    -1,    -1,    67,    68,    69,    70,
    71,    72,    73,    74,    75,    76,    77,    78,    79,    -1,
    -1,    82,    83,    84,   106,    86,   108,   109,   110,   111,
   112,   113,   114,   115,   116,   117,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   105,   106,    -1,   108,   109,   110,
   111,   112,   113,   114,   115,   116,   117,    -1,    -1,    -1,
    -1,   122,   123,   124,    -1,   126,   127,   128,     0,    -1,
   131,    -1,   133,   134,    -1,    -1,     8,     9,    10,    -1,
    -1,    -1,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    36,    37,    -1,    39,    40,    41,
    42,    43,    -1,    -1,    67,    68,    69,    70,    71,    72,
    73,    74,    75,    76,    77,    78,    79,    -1,    -1,    82,
    83,    -1,    -1,    -1,    87,    67,    68,    69,    70,    71,
    72,    73,    74,    75,    76,    77,    78,    79,    -1,    -1,
    82,    83,    84,   106,    86,   108,   109,   110,   111,   112,
   113,   114,   115,   116,   117,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   105,   106,   128,   108,   109,   110,   111,
   112,   113,   114,   115,   116,   117,    -1,    -1,    -1,    -1,
    -1,   123,   124,    -1,   126,   127,   128,    -1,    -1,   131,
    -1,   133,   134,     1,    -1,     3,     4,     5,     6,     7,
     8,     9,    10,    11,    12,    -1,    -1,    15,    16,    -1,
    18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
    -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    -1,    60,    61,    -1,    -1,    64,    65,    66,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,
    88,    89,    -1,    91,    92,    -1,    94,    -1,    96,    97,
    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   118,   119,   120,    -1,    -1,    -1,    -1,    -1,     1,    -1,
     3,     4,     5,     6,     7,   133,   134,    10,    11,    12,
    -1,    14,    15,    16,    -1,    18,    19,    20,    21,    22,
    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
    -1,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,
    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   118,   119,   120,     1,    -1,
     3,     4,     5,     6,     7,    -1,    -1,    10,    11,    12,
   133,   134,    15,    16,    17,    18,    19,    20,    21,    22,
    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
    -1,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,
    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   118,   119,   120,     1,    -1,
     3,     4,     5,     6,     7,    -1,    -1,    10,    11,    12,
   133,   134,    15,    16,    -1,    18,    19,    20,    21,    22,
    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
    -1,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,
    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   118,   119,   120,     1,    -1,
     3,     4,     5,     6,     7,    -1,     9,    10,    11,    12,
   133,   134,    -1,    16,    -1,    18,    19,    20,    21,    22,
    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
    -1,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,
    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   118,   119,   120,     1,    -1,
     3,     4,     5,     6,     7,    -1,    -1,    -1,    11,    12,
   133,   134,    -1,    16,    -1,    18,    19,    20,    21,    22,
    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
    -1,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,
    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   118,   119,   120,    -1,    -1,
    -1,    -1,    -1,     1,   127,     3,     4,     5,     6,     7,
   133,   134,    -1,    11,    12,    -1,    -1,    -1,    16,    -1,
    18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
    -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    -1,    60,    61,    -1,    -1,    64,    65,    66,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,
    88,    89,    -1,    91,    92,    -1,    94,    -1,    96,    97,
    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   118,   119,   120,    -1,    -1,    -1,    -1,    -1,     1,   127,
     3,     4,     5,     6,     7,   133,   134,    -1,    11,    12,
    -1,    -1,    -1,    16,    -1,    18,    19,    20,    21,    22,
    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
    -1,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,
    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   118,   119,   120,    -1,    -1,
   123,    -1,    -1,     1,    -1,     3,     4,     5,     6,     7,
   133,   134,    10,    11,    12,    -1,    -1,    -1,    16,    -1,
    18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
    -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    -1,    60,    61,    -1,    -1,    64,    65,    66,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,
    88,    89,    -1,    91,    92,    -1,    94,    -1,    96,    97,
    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   118,   119,   120,    -1,     3,     4,     5,     6,     7,    -1,
    -1,    -1,    11,    12,    -1,   133,   134,    16,    -1,    18,
    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
    29,    30,    31,    32,    33,    34,    35,    -1,    -1,    38,
    -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,
    49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
    -1,    60,    61,    -1,    -1,    64,    65,    66,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,
    89,    -1,    91,    92,    -1,    94,    -1,    96,    97,    98,
    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   118,
   119,   120,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
    -1,    11,    12,    -1,    -1,   134,    16,    -1,    18,    19,
    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
    30,    31,    32,    33,    34,    35,    -1,    -1,    38,    -1,
    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
    50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
    60,    61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
    -1,    91,    92,    -1,    -1,    -1,    96,    97,    98,    99,
   100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   118,   119,
   120,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
     8,     9,    10,   133,   134,    13,    14,    15,    16,    17,
    -1,    -1,    20,    21,    22,    23,    24,    25,    26,    -1,
    -1,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    64,    65,    -1,    67,
    68,    69,    70,    -1,    72,    73,    -1,    -1,    76,    -1,
    -1,    -1,    80,    81,    82,    83,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
    -1,    -1,   120,    -1,    -1,     3,     4,     5,    -1,     7,
    -1,   129,   130,    11,    12,    -1,    -1,    -1,    16,    -1,
    18,    19,    20,    21,    22,    23,    24,    -1,    26,    -1,
    -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    -1,    60,    61,    -1,    63,    64,    65,    66,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,
    88,    89,    -1,    91,    92,    -1,    94,    95,    96,    97,
    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
   118,   119,   120,    16,   122,    18,    19,    20,    21,    22,
    23,    24,   130,    26,    -1,    -1,    29,    30,    31,    32,
    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,
    -1,    94,    95,    96,    97,    98,    99,   100,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,   122,
    18,    19,    20,    21,    22,    23,    24,   130,    -1,    -1,
    -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    -1,    60,    61,    -1,    63,    64,    65,    66,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    86,    -1,
    88,    89,    -1,    91,    92,    -1,    94,    95,    96,    97,
    98,    99,   100,    -1,    -1,    -1,    -1,   105,    -1,    -1,
     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
   118,   119,   120,    16,    -1,    18,    19,    20,    21,    22,
    23,    24,   130,    -1,    -1,    -1,    29,    30,    31,    32,
    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    85,    86,    -1,    88,    89,    -1,    91,    92,
    -1,    94,    95,    96,    97,    98,    99,   100,    -1,    -1,
    -1,    -1,   105,    -1,    -1,     3,     4,     5,    -1,     7,
    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,    -1,
    18,    19,    20,    21,    22,    23,    24,   130,    -1,    -1,
    -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    -1,    60,    61,    -1,    63,    64,    65,    66,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,
    88,    89,    -1,    91,    92,    -1,    94,    95,    96,    97,
    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
   118,   119,   120,    16,    -1,    18,    19,    20,    21,    22,
    23,    24,   130,    -1,    -1,    -1,    29,    30,    31,    32,
    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,
    -1,    94,    95,    96,    97,    98,    99,   100,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,    -1,
    18,    19,    20,    21,    22,    23,    24,   130,    -1,    -1,
    -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    -1,    60,    61,    -1,    63,    64,    65,    66,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,
    88,    89,    -1,    91,    92,    -1,    94,    95,    96,    97,
    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
   118,   119,   120,    16,    -1,    18,    19,    20,    21,    22,
    23,    24,   130,    -1,    -1,    -1,    29,    30,    31,    32,
    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    58,    -1,    60,    61,    -1,
    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,    92,
    -1,    94,    95,    96,    97,    98,    99,   100,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   118,   119,   120,     3,     4,
     5,     6,     7,     8,     9,    10,    -1,   130,    13,    14,
    15,    16,    17,    -1,    -1,    20,    21,    22,    23,    24,
    25,    26,    -1,    -1,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,
    65,    -1,    67,    68,    69,    70,    -1,    72,    73,    -1,
    -1,    76,    -1,    -1,    -1,    80,    81,    82,    83,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   108,   109,   110,   111,   112,   113,   114,
   115,   116,   117,    -1,    -1,   120,     3,     4,     5,     6,
     7,     8,     9,    10,   129,    -1,    13,    14,    15,    16,
    17,    -1,    -1,    20,    21,    22,    23,    24,    25,    26,
    -1,    -1,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    -1,    54,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    65,    -1,
    67,    68,    69,    70,    -1,    72,    73,    -1,    -1,    76,
    -1,    -1,    -1,    80,    81,    82,    83,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    96,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   108,   109,   110,   111,   112,   113,   114,   115,   116,
   117,    -1,    -1,   120,     3,     4,     5,     6,     7,     8,
     9,    10,   129,    -1,    13,    14,    15,    16,    17,    -1,
    -1,    20,    21,    22,    23,    24,    25,    26,    -1,    -1,
    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
    49,    50,    51,    -1,    -1,    54,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    64,    65,    -1,    67,    68,
    69,    70,    -1,    72,    73,    -1,    -1,    76,    -1,    -1,
    -1,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    94,    -1,    96,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
   109,   110,   111,   112,   113,   114,   115,   116,   117,    -1,
    -1,   120,     3,     4,     5,     6,     7,     8,     9,    10,
   129,    -1,    13,    14,    15,    16,    17,    -1,    -1,    20,
    21,    22,    23,    24,    25,    26,    -1,    -1,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
    51,    -1,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    64,    65,    -1,    67,    68,    69,    70,
    -1,    72,    73,    -1,    -1,    76,    -1,    -1,    -1,    80,
    81,    82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,   110,
   111,   112,   113,   114,   115,   116,   117,    -1,    -1,   120,
    -1,     3,     4,     5,    -1,     7,    -1,    -1,   129,    11,
    12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,    21,
    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
    92,    -1,    94,    95,    96,    97,    98,    99,   100,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,     3,     4,     5,     6,     7,   118,   119,   120,    11,
    12,    -1,    -1,    -1,    16,   127,    18,    19,    20,    21,
    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
    32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,
    -1,    -1,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
    92,    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
    -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
    -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
    -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    -1,    60,    61,    -1,    63,    64,    65,    66,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
    -1,    88,    89,    -1,    91,    92,    -1,    94,    95,    96,
    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
    12,   118,   119,   120,    16,    -1,    18,    19,    20,    21,
    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
    92,    -1,    94,    95,    96,    97,    98,    99,   100,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
    -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
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
    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
    92,    -1,    94,    95,    96,    97,    98,    99,   100,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
    -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
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
    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
    92,    -1,    94,    95,    96,    97,    98,    99,   100,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
    -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
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
    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
    92,    -1,    94,    95,    96,    97,    98,    99,   100,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
    -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
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
    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
    92,    -1,    94,    95,    96,    97,    98,    99,   100,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
    -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
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
    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
    -1,    63,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
    92,    -1,    94,    95,    96,    97,    98,    99,   100,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
    -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
    -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
    -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    -1,    60,    61,    -1,    63,    64,    65,    66,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,    96,
    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
    12,   118,   119,   120,    16,    -1,    18,    19,    20,    21,
    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
    32,    33,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,
    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
    92,    -1,    -1,    -1,    96,    97,    98,    99,   100,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
    -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
    -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
    -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    -1,    60,    61,    -1,    63,    64,    65,    66,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
    -1,    88,    89,    -1,    91,    92,    -1,    -1,    -1,    96,
    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
    12,   118,   119,   120,    16,    -1,    18,    19,    20,    21,
    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
    92,    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
    -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
    -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
    -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,    66,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,    96,
    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
    12,   118,   119,   120,    16,    -1,    18,    19,    20,    21,
    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
    92,    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
    -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
    -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
    -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,    66,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,    96,
    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
    12,   118,   119,   120,    16,    -1,    18,    19,    20,    21,
    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
    92,    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
    -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
    -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
    -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,    66,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
    -1,    88,    89,    -1,    91,    92,    -1,    94,    -1,    96,
    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
    12,   118,   119,   120,    16,    -1,    18,    19,    20,    21,
    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
    92,    -1,    -1,    -1,    96,    97,    98,    99,   100,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
     7,    -1,    -1,    -1,    11,    12,   118,   119,   120,    16,
    -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
    -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
    -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    -1,    60,    61,    -1,    -1,    64,    65,    66,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
    -1,    88,    89,    -1,    91,    92,    -1,    -1,    -1,    96,
    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,
    12,   118,   119,   120,    16,    -1,    18,    19,    20,    21,
    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    31,
    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
    -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,    91,
    92,    -1,    94,    -1,    96,    97,    98,    99,   100,    -1,
    -1,    -1,     3,     4,     5,    -1,     7,    -1,   110,    -1,
    11,    12,    -1,    -1,    -1,    16,   118,    18,    19,    20,
    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
    31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
    51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
    61,    -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    82,    -1,    -1,    85,    -1,    -1,    88,    89,    -1,
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
    -1,    91,    92,    -1,    94,    -1,    96,    97,    98,    99,
   100,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
    -1,    -1,    11,    12,    -1,    -1,    -1,    16,   118,    18,
    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
    29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
    49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
    -1,    60,    61,    -1,    -1,    -1,    -1,    66,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,
    89,    -1,    91,    92,    -1,    94,    -1,    96,    97,    98,
    99,   100,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,   118,
    18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
    -1,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    -1,    60,    61,    -1,    -1,    -1,    -1,    66,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,
    88,    89,    -1,    91,    92,    -1,    -1,    -1,    96,    97,
    98,    99,   100,    -1,    -1,    -1,     3,     4,     5,    -1,
     7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,
   118,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
    -1,    -1,    29,    30,    31,    32,    33,    34,    35,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
    -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    -1,    60,    61,    -1,    -1,    -1,    -1,    66,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
    -1,    88,    89,    -1,    91,    92,    -1,    -1,    -1,    96,
    97,    98,    99,   100,    -1,    -1,    -1,     3,     4,     5,
    -1,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,
    16,   118,    18,    19,    20,    21,    22,    23,    24,    -1,
    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
    -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    -1,    60,    61,    -1,    -1,    -1,    -1,
    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
    -1,    -1,    88,    89,    -1,    91,    92,    -1,    -1,    -1,
    96,    97,    98,    99,   100,    -1,    50,    51,    -1,    -1,
    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    64,    65,   118,    67,    68,    69,    70,    -1,    72,    73,
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
   114,   115,   116,   117,    50,    51,   120,    -1,    54,    -1,
    -1,    -1,    -1,    -1,    -1,   129,    -1,    -1,    64,    65,
    -1,    67,    68,    69,    70,    -1,    72,    73,    -1,    -1,
    76,    -1,    -1,    -1,    80,    81,    82,    83,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   108,   109,   110,   111,   112,   113,   114,   115,
   116,   117,    -1,    -1,   120,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   129,    67,    68,    69,    70,    71,    72,
    73,    74,    75,    76,    77,    78,    79,    -1,    -1,    82,
    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   106,    -1,   108,   109,   110,   111,   112,
   113,   114,   115,   116,   117,    -1,    67,    68,    69,    70,
    71,    72,    73,    74,    75,    76,    77,    78,    79,    -1,
   133,    82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   106,    -1,   108,   109,   110,
   111,   112,   113,   114,   115,   116,   117,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,    67,    68,
    69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
    79,    -1,    -1,    82,    83,    67,    68,    69,    70,    71,
    72,    73,    74,    75,    76,    77,    78,    79,    -1,    -1,
    82,    83,    -1,    -1,    -1,    -1,    -1,   106,   107,   108,
   109,   110,   111,   112,   113,   114,   115,   116,   117,    -1,
    -1,    -1,    -1,    -1,   106,    -1,   108,   109,   110,   111,
   112,   113,   114,   115,   116,   117,    67,    68,    69,    70,
    71,    72,    73,    -1,    -1,    76,    77,    -1,    -1,    -1,
    -1,    82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,   110,
   111,   112,   113,   114,   115,   116,   117
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

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

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 359 "sydparse.y"
{
                        vps->lex_state = EXPR_BEG;
                        rb_funcall(vps->self, rb_intern("local_init"), 0);
                        // top_local_init();
                        class_nest = 0;
                        // if (ruby_class == rb_cObject) class_nest = 0;
                        // else class_nest = 1;
                    ;
    break;}
case 2:
#line 368 "sydparse.y"
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
                        vps->top = block_append(vps->top, yyvsp[0].node); 
                        //vps->top = NEW_SCOPE(block_append(vps->top, $2)); 
                        rb_funcall(vps->self, rb_intern("local_finish"), 0);
                        
                        // top_local_setup();
                        class_nest = 0;
                    ;
    break;}
case 3:
#line 393 "sydparse.y"
{
                        yyval.node = yyvsp[-3].node;
                        if (yyvsp[-2].node) {
                            yyval.node = NEW_RESCUE(yyvsp[-3].node, yyvsp[-2].node, yyvsp[-1].node);
                        }
                        else if (yyvsp[-1].node) {
                            rb_warn("else without rescue is useless");
                            yyval.node = block_append(yyval.node, yyvsp[-1].node);
                        }
                        if (yyvsp[0].node) {
                            yyval.node = NEW_ENSURE(yyval.node, yyvsp[0].node);
                        }
                        fixpos(yyval.node, yyvsp[-3].node);
                    ;
    break;}
case 4:
#line 410 "sydparse.y"
{
                        void_stmts(yyvsp[-1].node, parse_state);
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 6:
#line 418 "sydparse.y"
{
                        yyval.node = newline_node(yyvsp[0].node);
                    ;
    break;}
case 7:
#line 422 "sydparse.y"
{
                        yyval.node = block_append(yyvsp[-2].node, newline_node(yyvsp[0].node));
                    ;
    break;}
case 8:
#line 426 "sydparse.y"
{
                        yyval.node = yyvsp[0].node;
                    ;
    break;}
case 9:
#line 431 "sydparse.y"
{vps->lex_state = EXPR_FNAME;;
    break;}
case 10:
#line 432 "sydparse.y"
{
                        yyval.node = NEW_ALIAS(yyvsp[-2].id, yyvsp[0].id);
                    ;
    break;}
case 11:
#line 436 "sydparse.y"
{
                        yyval.node = NEW_VALIAS(yyvsp[-1].id, yyvsp[0].id);
                    ;
    break;}
case 12:
#line 440 "sydparse.y"
{
                        char buf[3];

                        sprintf(buf, "$%c", (char)yyvsp[0].node->nd_nth);
                        yyval.node = NEW_VALIAS(yyvsp[-1].id, rb_intern(buf));
                    ;
    break;}
case 13:
#line 447 "sydparse.y"
{
                        yyerror("can't make alias for the number variables");
                        yyval.node = 0;
                    ;
    break;}
case 14:
#line 452 "sydparse.y"
{
                        yyval.node = yyvsp[0].node;
                    ;
    break;}
case 15:
#line 456 "sydparse.y"
{
                        yyval.node = NEW_IF(cond(yyvsp[0].node, parse_state), yyvsp[-2].node, 0);
                        fixpos(yyval.node, yyvsp[0].node);
                        if (cond_negative(&yyval.node->nd_cond)) {
                            yyval.node->nd_else = yyval.node->nd_body;
                            yyval.node->nd_body = 0;
                        }
                    ;
    break;}
case 16:
#line 465 "sydparse.y"
{
                        yyval.node = NEW_UNLESS(cond(yyvsp[0].node, parse_state), yyvsp[-2].node, 0);
                        fixpos(yyval.node, yyvsp[0].node);
                        if (cond_negative(&yyval.node->nd_cond)) {
                            yyval.node->nd_body = yyval.node->nd_else;
                            yyval.node->nd_else = 0;
                        }
                    ;
    break;}
case 17:
#line 474 "sydparse.y"
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
                    ;
    break;}
case 18:
#line 486 "sydparse.y"
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
                    ;
    break;}
case 19:
#line 498 "sydparse.y"
{
                        yyval.node = NEW_RESCUE(yyvsp[-2].node, NEW_RESBODY(0,yyvsp[0].node,0), 0);
                    ;
    break;}
case 20:
#line 502 "sydparse.y"
{
                        if (in_def || in_single) {
                            yyerror("BEGIN in method");
                        }
                        local_push(0);
                    ;
    break;}
case 21:
#line 509 "sydparse.y"
{
                        /*
                        ruby_eval_tree_begin = block_append(ruby_eval_tree_begin,
                                                            NEW_PREEXE($4));
                        */
                        local_pop();
                        yyval.node = 0;
                    ;
    break;}
case 22:
#line 518 "sydparse.y"
{
                        if (in_def || in_single) {
                            rb_warn("END in method; use at_exit");
                        }

                        yyval.node = NEW_ITER(0, NEW_POSTEXE(), yyvsp[-1].node);
                    ;
    break;}
case 23:
#line 526 "sydparse.y"
{
                        yyval.node = node_assign(yyvsp[-2].node, yyvsp[0].node, parse_state);
                    ;
    break;}
case 24:
#line 530 "sydparse.y"
{
                        value_expr(yyvsp[0].node);
                        yyvsp[-2].node->nd_value = (yyvsp[-2].node->nd_head) ? NEW_TO_ARY(yyvsp[0].node) : NEW_ARRAY(yyvsp[0].node);
                        yyval.node = yyvsp[-2].node;
                    ;
    break;}
case 25:
#line 536 "sydparse.y"
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
                    ;
    break;}
case 26:
#line 561 "sydparse.y"
{
                        NODE *args;

                        value_expr(yyvsp[0].node);
                        args = NEW_LIST(yyvsp[0].node);
                        if (yyvsp[-3].node && nd_type(yyvsp[-3].node) != NODE_ARRAY)
                            yyvsp[-3].node = NEW_LIST(yyvsp[-3].node);
                        yyvsp[-3].node = list_append(yyvsp[-3].node, NEW_NIL());
                        list_concat(args, yyvsp[-3].node);
                        if (yyvsp[-1].id == tOROP) {
                            yyvsp[-1].id = 0;
                        }
                        else if (yyvsp[-1].id == tANDOP) {
                            yyvsp[-1].id = 1;
                        }
                        yyval.node = NEW_OP_ASGN1(yyvsp[-5].node, yyvsp[-1].id, args);
                        fixpos(yyval.node, yyvsp[-5].node);
                    ;
    break;}
case 27:
#line 580 "sydparse.y"
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
                    ;
    break;}
case 28:
#line 592 "sydparse.y"
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
                    ;
    break;}
case 29:
#line 604 "sydparse.y"
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
                    ;
    break;}
case 30:
#line 616 "sydparse.y"
{
                        rb_backref_error(yyvsp[-2].node);
                        yyval.node = 0;
                    ;
    break;}
case 31:
#line 621 "sydparse.y"
{
                        yyval.node = node_assign(yyvsp[-2].node, NEW_SVALUE(yyvsp[0].node), parse_state);
                    ;
    break;}
case 32:
#line 625 "sydparse.y"
{
                        yyvsp[-2].node->nd_value = (yyvsp[-2].node->nd_head) ? NEW_TO_ARY(yyvsp[0].node) : NEW_ARRAY(yyvsp[0].node);
                        yyval.node = yyvsp[-2].node;
                    ;
    break;}
case 33:
#line 630 "sydparse.y"
{
                        yyvsp[-2].node->nd_value = yyvsp[0].node;
                        yyval.node = yyvsp[-2].node;
                    ;
    break;}
case 36:
#line 639 "sydparse.y"
{
                        yyval.node = logop(NODE_AND, yyvsp[-2].node, yyvsp[0].node, parse_state);
                    ;
    break;}
case 37:
#line 643 "sydparse.y"
{
                        yyval.node = logop(NODE_OR, yyvsp[-2].node, yyvsp[0].node, parse_state);
                    ;
    break;}
case 38:
#line 647 "sydparse.y"
{
                        yyval.node = NEW_NOT(cond(yyvsp[0].node, parse_state));
                    ;
    break;}
case 39:
#line 651 "sydparse.y"
{
                        yyval.node = NEW_NOT(cond(yyvsp[0].node, parse_state));
                    ;
    break;}
case 41:
#line 658 "sydparse.y"
{
                        value_expr(yyval.node);
                        yyval.node = yyvsp[0].node;
                    ;
    break;}
case 44:
#line 667 "sydparse.y"
{
                        yyval.node = NEW_RETURN(ret_args(yyvsp[0].node));
                    ;
    break;}
case 45:
#line 671 "sydparse.y"
{
                        yyval.node = NEW_BREAK(ret_args(yyvsp[0].node));
                    ;
    break;}
case 46:
#line 675 "sydparse.y"
{
                        yyval.node = NEW_NEXT(ret_args(yyvsp[0].node));
                    ;
    break;}
case 48:
#line 682 "sydparse.y"
{
                        yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                    ;
    break;}
case 49:
#line 686 "sydparse.y"
{
                        yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                    ;
    break;}
case 50:
#line 692 "sydparse.y"
{
                        yyval.vars = dyna_push();
                        yyvsp[0].num = ruby_sourceline;
                    ;
    break;}
case 51:
#line 696 "sydparse.y"
{yyval.vars = ruby_dyna_vars;;
    break;}
case 52:
#line 699 "sydparse.y"
{
                        yyval.node = NEW_ITER(yyvsp[-3].node, 0, dyna_init(yyvsp[-1].node, yyvsp[-2].vars));
                        nd_set_line(yyval.node, yyvsp[-5].num);
                        dyna_pop(yyvsp[-4].vars);
                    ;
    break;}
case 53:
#line 707 "sydparse.y"
{
                        yyval.node = new_fcall(yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[0].node);
                   ;
    break;}
case 54:
#line 712 "sydparse.y"
{
                        yyval.node = new_fcall(yyvsp[-2].id, yyvsp[-1].node);
                        if (yyvsp[0].node) {
                            if (nd_type(yyval.node) == NODE_BLOCK_PASS) {
                                rb_compile_error("both block arg and actual block given");
                            }
                            yyvsp[0].node->nd_iter = yyval.node;
                            yyval.node = yyvsp[0].node;
                        }
                        fixpos(yyval.node, yyvsp[-1].node);
                   ;
    break;}
case 55:
#line 724 "sydparse.y"
{
                        yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-3].node);
                    ;
    break;}
case 56:
#line 729 "sydparse.y"
{
                        yyval.node = new_call(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].node);
                        if (yyvsp[0].node) {
                            if (nd_type(yyval.node) == NODE_BLOCK_PASS) {
                                rb_compile_error("both block arg and actual block given");
                            }
                            yyvsp[0].node->nd_iter = yyval.node;
                            yyval.node = yyvsp[0].node;
                        }
                        fixpos(yyval.node, yyvsp[-4].node);
                   ;
    break;}
case 57:
#line 741 "sydparse.y"
{
                        yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-3].node);
                    ;
    break;}
case 58:
#line 746 "sydparse.y"
{
                        yyval.node = new_call(yyvsp[-4].node, yyvsp[-2].id, yyvsp[-1].node);
                        if (yyvsp[0].node) {
                            if (nd_type(yyval.node) == NODE_BLOCK_PASS) {
                                rb_compile_error("both block arg and actual block given");
                            }
                            yyvsp[0].node->nd_iter = yyval.node;
                            yyval.node = yyvsp[0].node;
                        }
                        fixpos(yyval.node, yyvsp[-4].node);
                   ;
    break;}
case 59:
#line 758 "sydparse.y"
{
                        yyval.node = new_super(yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 60:
#line 763 "sydparse.y"
{
                        yyval.node = new_yield(yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 62:
#line 771 "sydparse.y"
{
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 64:
#line 778 "sydparse.y"
{
                        yyval.node = NEW_MASGN(NEW_LIST(yyvsp[-1].node), 0);
                    ;
    break;}
case 65:
#line 784 "sydparse.y"
{
                        yyval.node = NEW_MASGN(yyvsp[0].node, 0);
                    ;
    break;}
case 66:
#line 788 "sydparse.y"
{
                        yyval.node = NEW_MASGN(list_append(yyvsp[-1].node,yyvsp[0].node), 0);
                    ;
    break;}
case 67:
#line 792 "sydparse.y"
{
                        yyval.node = NEW_MASGN(yyvsp[-2].node, yyvsp[0].node);
                    ;
    break;}
case 68:
#line 796 "sydparse.y"
{
                        yyval.node = NEW_MASGN(yyvsp[-1].node, -1);
                    ;
    break;}
case 69:
#line 800 "sydparse.y"
{
                        yyval.node = NEW_MASGN(0, yyvsp[0].node);
                    ;
    break;}
case 70:
#line 804 "sydparse.y"
{
                        yyval.node = NEW_MASGN(0, -1);
                    ;
    break;}
case 72:
#line 811 "sydparse.y"
{
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 73:
#line 817 "sydparse.y"
{
                        yyval.node = NEW_LIST(yyvsp[-1].node);
                    ;
    break;}
case 74:
#line 821 "sydparse.y"
{
                        yyval.node = list_append(yyvsp[-2].node, yyvsp[-1].node);
                    ;
    break;}
case 75:
#line 827 "sydparse.y"
{
                        yyval.node = assignable(yyvsp[0].id, 0, parse_state);
                    ;
    break;}
case 76:
#line 831 "sydparse.y"
{
                        yyval.node = aryset(yyvsp[-3].node, yyvsp[-1].node, parse_state);
                    ;
    break;}
case 77:
#line 835 "sydparse.y"
{
                        yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id, parse_state);
                    ;
    break;}
case 78:
#line 839 "sydparse.y"
{
                        yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id, parse_state);
                    ;
    break;}
case 79:
#line 843 "sydparse.y"
{
                        yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id, parse_state);
                    ;
    break;}
case 80:
#line 847 "sydparse.y"
{
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        yyval.node = NEW_CDECL(0, 0, NEW_COLON2(yyvsp[-2].node, yyvsp[0].id));
                    ;
    break;}
case 81:
#line 853 "sydparse.y"
{
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        yyval.node = NEW_CDECL(0, 0, NEW_COLON3(yyvsp[0].id));
                    ;
    break;}
case 82:
#line 859 "sydparse.y"
{
                        rb_backref_error(yyvsp[0].node);
                        yyval.node = 0;
                    ;
    break;}
case 83:
#line 866 "sydparse.y"
{
                        yyval.node = assignable(yyvsp[0].id, 0, parse_state);
                    ;
    break;}
case 84:
#line 870 "sydparse.y"
{
                        yyval.node = aryset(yyvsp[-3].node, yyvsp[-1].node, parse_state);
                    ;
    break;}
case 85:
#line 874 "sydparse.y"
{
                        yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id, parse_state);
                    ;
    break;}
case 86:
#line 878 "sydparse.y"
{
                        yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id, parse_state);
                    ;
    break;}
case 87:
#line 882 "sydparse.y"
{
                        yyval.node = attrset(yyvsp[-2].node, yyvsp[0].id, parse_state);
                    ;
    break;}
case 88:
#line 886 "sydparse.y"
{
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        yyval.node = NEW_CDECL(0, 0, NEW_COLON2(yyvsp[-2].node, yyvsp[0].id));
                    ;
    break;}
case 89:
#line 892 "sydparse.y"
{
                        if (in_def || in_single)
                            yyerror("dynamic constant assignment");
                        yyval.node = NEW_CDECL(0, 0, NEW_COLON3(yyvsp[0].id));
                    ;
    break;}
case 90:
#line 898 "sydparse.y"
{
                        rb_backref_error(yyvsp[0].node);
                        yyval.node = 0;
                    ;
    break;}
case 91:
#line 905 "sydparse.y"
{
                        yyerror("class/module name must be CONSTANT");
                    ;
    break;}
case 93:
#line 912 "sydparse.y"
{
                        yyval.node = NEW_COLON3(yyvsp[0].id);
                    ;
    break;}
case 94:
#line 916 "sydparse.y"
{
                        yyval.node = NEW_COLON2(0, yyval.node);
                    ;
    break;}
case 95:
#line 920 "sydparse.y"
{
                        yyval.node = NEW_COLON2(yyvsp[-2].node, yyvsp[0].id);
                    ;
    break;}
case 99:
#line 929 "sydparse.y"
{
                        vps->lex_state = EXPR_END;
                        yyval.id = convert_op(yyvsp[0].id);
                    ;
    break;}
case 100:
#line 934 "sydparse.y"
{
                        vps->lex_state = EXPR_END;
                        yyval.id = yyvsp[0].id;
                    ;
    break;}
case 103:
#line 945 "sydparse.y"
{
                        yyval.node = NEW_UNDEF(yyvsp[0].id);
                    ;
    break;}
case 104:
#line 948 "sydparse.y"
{vps->lex_state = EXPR_FNAME;;
    break;}
case 105:
#line 949 "sydparse.y"
{
                        yyval.node = block_append(yyvsp[-3].node, NEW_UNDEF(yyvsp[0].id));
                    ;
    break;}
case 106:
#line 954 "sydparse.y"
{ yyval.id = '|'; ;
    break;}
case 107:
#line 955 "sydparse.y"
{ yyval.id = '^'; ;
    break;}
case 108:
#line 956 "sydparse.y"
{ yyval.id = '&'; ;
    break;}
case 109:
#line 957 "sydparse.y"
{ yyval.id = tCMP; ;
    break;}
case 110:
#line 958 "sydparse.y"
{ yyval.id = tEQ; ;
    break;}
case 111:
#line 959 "sydparse.y"
{ yyval.id = tEQQ; ;
    break;}
case 112:
#line 960 "sydparse.y"
{ yyval.id = tMATCH; ;
    break;}
case 113:
#line 961 "sydparse.y"
{ yyval.id = '>'; ;
    break;}
case 114:
#line 962 "sydparse.y"
{ yyval.id = tGEQ; ;
    break;}
case 115:
#line 963 "sydparse.y"
{ yyval.id = '<'; ;
    break;}
case 116:
#line 964 "sydparse.y"
{ yyval.id = tLEQ; ;
    break;}
case 117:
#line 965 "sydparse.y"
{ yyval.id = tLSHFT; ;
    break;}
case 118:
#line 966 "sydparse.y"
{ yyval.id = tRSHFT; ;
    break;}
case 119:
#line 967 "sydparse.y"
{ yyval.id = '+'; ;
    break;}
case 120:
#line 968 "sydparse.y"
{ yyval.id = '-'; ;
    break;}
case 121:
#line 969 "sydparse.y"
{ yyval.id = '*'; ;
    break;}
case 122:
#line 970 "sydparse.y"
{ yyval.id = '*'; ;
    break;}
case 123:
#line 971 "sydparse.y"
{ yyval.id = '/'; ;
    break;}
case 124:
#line 972 "sydparse.y"
{ yyval.id = '%'; ;
    break;}
case 125:
#line 973 "sydparse.y"
{ yyval.id = tPOW; ;
    break;}
case 126:
#line 974 "sydparse.y"
{ yyval.id = '~'; ;
    break;}
case 127:
#line 975 "sydparse.y"
{ yyval.id = tUPLUS; ;
    break;}
case 128:
#line 976 "sydparse.y"
{ yyval.id = tUMINUS; ;
    break;}
case 129:
#line 977 "sydparse.y"
{ yyval.id = tAREF; ;
    break;}
case 130:
#line 978 "sydparse.y"
{ yyval.id = tASET; ;
    break;}
case 131:
#line 979 "sydparse.y"
{ yyval.id = '`'; ;
    break;}
case 173:
#line 992 "sydparse.y"
{
                        yyval.node = node_assign(yyvsp[-2].node, yyvsp[0].node, parse_state);
                    ;
    break;}
case 174:
#line 996 "sydparse.y"
{
                        yyval.node = node_assign(yyvsp[-4].node, NEW_RESCUE(yyvsp[-2].node, NEW_RESBODY(0,yyvsp[0].node,0), 0), parse_state);
                    ;
    break;}
case 175:
#line 1000 "sydparse.y"
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
                    ;
    break;}
case 176:
#line 1025 "sydparse.y"
{
                        NODE *args;

                        value_expr(yyvsp[0].node);
                        args = NEW_LIST(yyvsp[0].node);
                        if (yyvsp[-3].node && nd_type(yyvsp[-3].node) != NODE_ARRAY)
                            yyvsp[-3].node = NEW_LIST(yyvsp[-3].node);
                        yyvsp[-3].node = list_append(yyvsp[-3].node, NEW_NIL());
                        list_concat(args, yyvsp[-3].node);
                        if (yyvsp[-1].id == tOROP) {
                            yyvsp[-1].id = 0;
                        }
                        else if (yyvsp[-1].id == tANDOP) {
                            yyvsp[-1].id = 1;
                        }
                        yyval.node = NEW_OP_ASGN1(yyvsp[-5].node, yyvsp[-1].id, args);
                        fixpos(yyval.node, yyvsp[-5].node);
                    ;
    break;}
case 177:
#line 1044 "sydparse.y"
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
                    ;
    break;}
case 178:
#line 1056 "sydparse.y"
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
                    ;
    break;}
case 179:
#line 1068 "sydparse.y"
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
                    ;
    break;}
case 180:
#line 1080 "sydparse.y"
{
                        yyerror("constant re-assignment");
                        yyval.node = 0;
                    ;
    break;}
case 181:
#line 1085 "sydparse.y"
{
                        yyerror("constant re-assignment");
                        yyval.node = 0;
                    ;
    break;}
case 182:
#line 1090 "sydparse.y"
{
                        rb_backref_error(yyvsp[-2].node);
                        yyval.node = 0;
                    ;
    break;}
case 183:
#line 1095 "sydparse.y"
{
                        value_expr(yyvsp[-2].node);
                        value_expr(yyvsp[0].node);
                        yyval.node = NEW_DOT2(yyvsp[-2].node, yyvsp[0].node);
                    ;
    break;}
case 184:
#line 1101 "sydparse.y"
{
                        value_expr(yyvsp[-2].node);
                        value_expr(yyvsp[0].node);
                        yyval.node = NEW_DOT3(yyvsp[-2].node, yyvsp[0].node);
                    ;
    break;}
case 185:
#line 1107 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, '+', 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 186:
#line 1111 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, '-', 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 187:
#line 1115 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, '*', 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 188:
#line 1119 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, '/', 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 189:
#line 1123 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, '%', 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 190:
#line 1127 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, tPOW, 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 191:
#line 1131 "sydparse.y"
{
                        yyval.node = call_op(call_op(yyvsp[-2].node, tPOW, 1, yyvsp[0].node, parse_state), tUMINUS, 0, 0, parse_state);
                    ;
    break;}
case 192:
#line 1135 "sydparse.y"
{
                        yyval.node = call_op(call_op(yyvsp[-2].node, tPOW, 1, yyvsp[0].node, parse_state), tUMINUS, 0, 0, parse_state);
                    ;
    break;}
case 193:
#line 1139 "sydparse.y"
{
                        if (yyvsp[0].node && nd_type(yyvsp[0].node) == NODE_LIT) {
                            yyval.node = yyvsp[0].node;
                        }
                        else {
                            yyval.node = call_op(yyvsp[0].node, tUPLUS, 0, 0, parse_state);
                        }
                    ;
    break;}
case 194:
#line 1148 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[0].node, tUMINUS, 0, 0, parse_state);
                    ;
    break;}
case 195:
#line 1152 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, '|', 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 196:
#line 1156 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, '^', 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 197:
#line 1160 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, '&', 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 198:
#line 1164 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, tCMP, 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 199:
#line 1168 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, '>', 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 200:
#line 1172 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, tGEQ, 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 201:
#line 1176 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, '<', 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 202:
#line 1180 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, tLEQ, 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 203:
#line 1184 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, tEQ, 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 204:
#line 1188 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, tEQQ, 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 205:
#line 1192 "sydparse.y"
{
                        yyval.node = NEW_NOT(call_op(yyvsp[-2].node, tEQ, 1, yyvsp[0].node, parse_state));
                    ;
    break;}
case 206:
#line 1196 "sydparse.y"
{
                        yyval.node = match_gen(yyvsp[-2].node, yyvsp[0].node, parse_state);
                    ;
    break;}
case 207:
#line 1200 "sydparse.y"
{
                        yyval.node = NEW_NOT(match_gen(yyvsp[-2].node, yyvsp[0].node, parse_state));
                    ;
    break;}
case 208:
#line 1204 "sydparse.y"
{
                        yyval.node = NEW_NOT(cond(yyvsp[0].node, parse_state));
                    ;
    break;}
case 209:
#line 1208 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[0].node, '~', 0, 0, parse_state);
                    ;
    break;}
case 210:
#line 1212 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, tLSHFT, 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 211:
#line 1216 "sydparse.y"
{
                        yyval.node = call_op(yyvsp[-2].node, tRSHFT, 1, yyvsp[0].node, parse_state);
                    ;
    break;}
case 212:
#line 1220 "sydparse.y"
{
                        yyval.node = logop(NODE_AND, yyvsp[-2].node, yyvsp[0].node, parse_state);
                    ;
    break;}
case 213:
#line 1224 "sydparse.y"
{
                        yyval.node = logop(NODE_OR, yyvsp[-2].node, yyvsp[0].node, parse_state);
                    ;
    break;}
case 214:
#line 1227 "sydparse.y"
{vps->in_defined = 1;;
    break;}
case 215:
#line 1228 "sydparse.y"
{
                        vps->in_defined = 0;
                        yyval.node = NEW_DEFINED(yyvsp[0].node);
                    ;
    break;}
case 216:
#line 1232 "sydparse.y"
{vps->ternary_colon++;;
    break;}
case 217:
#line 1233 "sydparse.y"
{
                        yyval.node = NEW_IF(cond(yyvsp[-5].node, parse_state), yyvsp[-2].node, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-5].node);
                        vps->ternary_colon--;
                    ;
    break;}
case 218:
#line 1239 "sydparse.y"
{
                        yyval.node = yyvsp[0].node;
                    ;
    break;}
case 219:
#line 1245 "sydparse.y"
{
                        value_expr(yyvsp[0].node);
                        yyval.node = yyvsp[0].node;
                    ;
    break;}
case 221:
#line 1253 "sydparse.y"
{
                        rb_warn("parenthesize argument(s) for future version");
                        yyval.node = NEW_LIST(yyvsp[-1].node);
                    ;
    break;}
case 222:
#line 1258 "sydparse.y"
{
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 223:
#line 1262 "sydparse.y"
{
                        value_expr(yyvsp[-1].node);
                        yyval.node = arg_concat(yyvsp[-4].node, yyvsp[-1].node);
                    ;
    break;}
case 224:
#line 1267 "sydparse.y"
{
                        yyval.node = NEW_LIST(NEW_HASH(yyvsp[-1].node));
                    ;
    break;}
case 225:
#line 1271 "sydparse.y"
{
                        value_expr(yyvsp[-1].node);
                        yyval.node = NEW_NEWLINE(NEW_SPLAT(yyvsp[-1].node));
                    ;
    break;}
case 226:
#line 1278 "sydparse.y"
{
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 227:
#line 1282 "sydparse.y"
{
                        yyval.node = yyvsp[-2].node;
                    ;
    break;}
case 228:
#line 1286 "sydparse.y"
{
                        rb_warn("parenthesize argument for future version");
                        yyval.node = NEW_LIST(yyvsp[-2].node);
                    ;
    break;}
case 229:
#line 1291 "sydparse.y"
{
                        rb_warn("parenthesize argument for future version");
                        yyval.node = list_append(yyvsp[-4].node, yyvsp[-2].node);
                    ;
    break;}
case 232:
#line 1302 "sydparse.y"
{
                        rb_warn("parenthesize argument(s) for future version");
                        yyval.node = NEW_LIST(yyvsp[0].node);
                    ;
    break;}
case 233:
#line 1307 "sydparse.y"
{
                        yyval.node = arg_blk_pass(yyvsp[-1].node, yyvsp[0].node);
                    ;
    break;}
case 234:
#line 1311 "sydparse.y"
{
                        yyval.node = arg_concat(yyvsp[-4].node, yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 235:
#line 1316 "sydparse.y"
{
                        yyval.node = NEW_LIST(NEW_POSITIONAL(yyvsp[-1].node));
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 236:
#line 1321 "sydparse.y"
{
                        yyval.node = arg_concat(NEW_LIST(NEW_POSITIONAL(yyvsp[-4].node)), yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 237:
#line 1326 "sydparse.y"
{
                        yyval.node = list_append(yyvsp[-3].node, NEW_POSITIONAL(yyvsp[-1].node));
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 238:
#line 1331 "sydparse.y"
{
                        value_expr(yyvsp[-1].node);
                        yyval.node = arg_concat(list_append(yyvsp[-6].node, NEW_POSITIONAL(yyvsp[-4].node)), yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 239:
#line 1337 "sydparse.y"
{
                        yyval.node = arg_blk_pass(NEW_SPLAT(yyvsp[-1].node), yyvsp[0].node);
                    ;
    break;}
case 241:
#line 1344 "sydparse.y"
{
                        yyval.node = arg_blk_pass(list_concat(NEW_LIST(yyvsp[-3].node),yyvsp[-1].node), yyvsp[0].node);
                    ;
    break;}
case 242:
#line 1348 "sydparse.y"
{
                        yyval.node = arg_blk_pass(yyvsp[-2].node, yyvsp[0].node);
                    ;
    break;}
case 243:
#line 1352 "sydparse.y"
{
                        yyval.node = arg_concat(NEW_LIST(yyvsp[-4].node), yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 244:
#line 1357 "sydparse.y"
{
            yyval.node = arg_concat(list_concat(NEW_LIST(yyvsp[-6].node),yyvsp[-4].node), yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 245:
#line 1362 "sydparse.y"
{
                        yyval.node = NEW_LIST(NEW_POSITIONAL(yyvsp[-1].node));
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 246:
#line 1367 "sydparse.y"
{
                        yyval.node = arg_concat(NEW_LIST(NEW_POSITIONAL(yyvsp[-4].node)), yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 247:
#line 1372 "sydparse.y"
{
                        yyval.node = list_append(NEW_LIST(yyvsp[-3].node), NEW_POSITIONAL(yyvsp[-1].node));
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 248:
#line 1377 "sydparse.y"
{
                        yyval.node = list_append(list_concat(NEW_LIST(yyvsp[-5].node),yyvsp[-3].node), NEW_POSITIONAL(yyvsp[-1].node));
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 249:
#line 1382 "sydparse.y"
{
                        yyval.node = arg_concat(list_append(NEW_LIST(yyvsp[-6].node), NEW_POSITIONAL(yyvsp[-4].node)), yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 250:
#line 1387 "sydparse.y"
{
                        yyval.node = arg_concat(list_append(list_concat(NEW_LIST(yyvsp[-8].node), yyvsp[-6].node), NEW_POSITIONAL(yyvsp[-4].node)), yyvsp[-1].node);
                        yyval.node = arg_blk_pass(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 251:
#line 1392 "sydparse.y"
{
                        yyval.node = arg_blk_pass(NEW_SPLAT(yyvsp[-1].node), yyvsp[0].node);
                    ;
    break;}
case 253:
#line 1398 "sydparse.y"
{
                        yyval.num = vps->cmdarg_stack;
                        CMDARG_PUSH(1);
                    ;
    break;}
case 254:
#line 1403 "sydparse.y"
{
                        /* CMDARG_POP() */
                        vps->cmdarg_stack = yyvsp[-1].num;
                        yyval.node = yyvsp[0].node;
                    ;
    break;}
case 256:
#line 1411 "sydparse.y"
{vps->lex_state = EXPR_ENDARG;;
    break;}
case 257:
#line 1412 "sydparse.y"
{
                        rb_warn("don't put space before argument parentheses");
                        yyval.node = 0;
                    ;
    break;}
case 258:
#line 1416 "sydparse.y"
{vps->lex_state = EXPR_ENDARG;;
    break;}
case 259:
#line 1417 "sydparse.y"
{
                        rb_warn("don't put space before argument parentheses");
                        yyval.node = yyvsp[-2].node;
                    ;
    break;}
case 260:
#line 1424 "sydparse.y"
{
                        yyval.node = NEW_BLOCK_PASS(yyvsp[0].node);
                    ;
    break;}
case 261:
#line 1430 "sydparse.y"
{
                        yyval.node = yyvsp[0].node;
                    ;
    break;}
case 263:
#line 1437 "sydparse.y"
{
                        yyval.node = NEW_LIST(yyvsp[0].node);
                    ;
    break;}
case 264:
#line 1441 "sydparse.y"
{
                        yyval.node = list_append(yyvsp[-2].node, yyvsp[0].node);
                    ;
    break;}
case 265:
#line 1447 "sydparse.y"
{
                        yyval.node = list_append(yyvsp[-2].node, yyvsp[0].node);
                    ;
    break;}
case 266:
#line 1451 "sydparse.y"
{
                        yyval.node = arg_concat(yyvsp[-3].node, yyvsp[0].node);
                    ;
    break;}
case 267:
#line 1455 "sydparse.y"
{
                        yyval.node = NEW_SPLAT(yyvsp[0].node);
                    ;
    break;}
case 276:
#line 1469 "sydparse.y"
{
                        yyval.node = NEW_FCALL(yyvsp[0].id, 0);
                    ;
    break;}
case 277:
#line 1473 "sydparse.y"
{
                        yyvsp[0].num = ruby_sourceline;
                    ;
    break;}
case 278:
#line 1478 "sydparse.y"
{
                        if (yyvsp[-1].node == NULL)
                            yyval.node = NEW_NIL();
                        else
                            yyval.node = NEW_BEGIN(yyvsp[-1].node);
                        nd_set_line(yyval.node, yyvsp[-3].num);
                    ;
    break;}
case 279:
#line 1485 "sydparse.y"
{vps->lex_state = EXPR_ENDARG;;
    break;}
case 280:
#line 1486 "sydparse.y"
{
                        rb_warning("(...) interpreted as grouped expression");
                        yyval.node = yyvsp[-3].node;
                    ;
    break;}
case 281:
#line 1491 "sydparse.y"
{
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 282:
#line 1495 "sydparse.y"
{
                        yyval.node = NEW_COLON2(yyvsp[-2].node, yyvsp[0].id);
                    ;
    break;}
case 283:
#line 1499 "sydparse.y"
{
                        yyval.node = NEW_COLON3(yyvsp[0].id);
                    ;
    break;}
case 284:
#line 1503 "sydparse.y"
{
                        if (yyvsp[-3].node && nd_type(yyvsp[-3].node) == NODE_SELF) {
                            yyval.node = NEW_FCALL(convert_op(tAREF), yyvsp[-1].node);
                        } else {
                            yyval.node = NEW_CALL(yyvsp[-3].node, convert_op(tAREF), yyvsp[-1].node);
                        }
                        fixpos(yyval.node, yyvsp[-3].node);
                    ;
    break;}
case 285:
#line 1512 "sydparse.y"
{
                        if (yyvsp[-1].node == 0) {
                            yyval.node = NEW_ZARRAY(); /* zero length array*/
                        }
                        else {
                            yyval.node = yyvsp[-1].node;
                        }
                    ;
    break;}
case 286:
#line 1521 "sydparse.y"
{
                        yyval.node = NEW_HASH(yyvsp[-1].node);
                    ;
    break;}
case 287:
#line 1525 "sydparse.y"
{
                        yyval.node = NEW_RETURN(0);
                    ;
    break;}
case 288:
#line 1529 "sydparse.y"
{
                        yyval.node = new_yield(yyvsp[-1].node);
                    ;
    break;}
case 289:
#line 1533 "sydparse.y"
{
                        yyval.node = NEW_YIELD(0, Qfalse);
                    ;
    break;}
case 290:
#line 1537 "sydparse.y"
{
                        yyval.node = NEW_YIELD(0, Qfalse);
                    ;
    break;}
case 291:
#line 1540 "sydparse.y"
{vps->in_defined = 1;;
    break;}
case 292:
#line 1541 "sydparse.y"
{
                        vps->in_defined = 0;
                        yyval.node = NEW_DEFINED(yyvsp[-1].node);
                    ;
    break;}
case 293:
#line 1546 "sydparse.y"
{
                        yyvsp[0].node->nd_iter = NEW_FCALL(yyvsp[-1].id, 0);
                        yyval.node = yyvsp[0].node;
                        fixpos(yyvsp[0].node->nd_iter, yyvsp[0].node);
                    ;
    break;}
case 295:
#line 1553 "sydparse.y"
{
                        if (yyvsp[-1].node && nd_type(yyvsp[-1].node) == NODE_BLOCK_PASS) {
                            rb_compile_error("both block arg and actual block given");
                        }
                        yyvsp[0].node->nd_iter = yyvsp[-1].node;
                        yyval.node = yyvsp[0].node;
                        fixpos(yyval.node, yyvsp[-1].node);
                    ;
    break;}
case 296:
#line 1565 "sydparse.y"
{
                        yyval.node = NEW_IF(cond(yyvsp[-4].node, parse_state), yyvsp[-2].node, yyvsp[-1].node);
                        fixpos(yyval.node, yyvsp[-4].node);
                        if (cond_negative(&yyval.node->nd_cond)) {
                            NODE *tmp = yyval.node->nd_body;
                            yyval.node->nd_body = yyval.node->nd_else;
                            yyval.node->nd_else = tmp;
                        }
                    ;
    break;}
case 297:
#line 1578 "sydparse.y"
{
                        yyval.node = NEW_UNLESS(cond(yyvsp[-4].node, parse_state), yyvsp[-2].node, yyvsp[-1].node);
                        fixpos(yyval.node, yyvsp[-4].node);
                        if (cond_negative(&yyval.node->nd_cond)) {
                            NODE *tmp = yyval.node->nd_body;
                            yyval.node->nd_body = yyval.node->nd_else;
                            yyval.node->nd_else = tmp;
                        }
                    ;
    break;}
case 298:
#line 1587 "sydparse.y"
{COND_PUSH(1);;
    break;}
case 299:
#line 1587 "sydparse.y"
{COND_POP();;
    break;}
case 300:
#line 1590 "sydparse.y"
{
                        yyval.node = NEW_WHILE(cond(yyvsp[-4].node, parse_state), yyvsp[-1].node, 1);
                        fixpos(yyval.node, yyvsp[-4].node);
                        if (cond_negative(&yyval.node->nd_cond)) {
                            nd_set_type(yyval.node, NODE_UNTIL);
                        }
                    ;
    break;}
case 301:
#line 1597 "sydparse.y"
{COND_PUSH(1);;
    break;}
case 302:
#line 1597 "sydparse.y"
{COND_POP();;
    break;}
case 303:
#line 1600 "sydparse.y"
{
                        yyval.node = NEW_UNTIL(cond(yyvsp[-4].node, parse_state), yyvsp[-1].node, 1);
                        fixpos(yyval.node, yyvsp[-4].node);
                        if (cond_negative(&yyval.node->nd_cond)) {
                            nd_set_type(yyval.node, NODE_WHILE);
                        }
                    ;
    break;}
case 304:
#line 1610 "sydparse.y"
{
                        yyval.node = NEW_CASE(yyvsp[-3].node, yyvsp[-1].node);
                        fixpos(yyval.node, yyvsp[-3].node);
                    ;
    break;}
case 305:
#line 1615 "sydparse.y"
{
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 306:
#line 1619 "sydparse.y"
{
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 307:
#line 1622 "sydparse.y"
{COND_PUSH(1);;
    break;}
case 308:
#line 1622 "sydparse.y"
{COND_POP();;
    break;}
case 309:
#line 1625 "sydparse.y"
{
                        yyval.node = NEW_FOR(yyvsp[-7].node, yyvsp[-4].node, yyvsp[-1].node);
                        fixpos(yyval.node, yyvsp[-7].node);
                    ;
    break;}
case 310:
#line 1630 "sydparse.y"
{
                        if (in_def || in_single)
                            yyerror("class definition in method body");
                        class_nest++;
                        local_push(0);
                        yyval.num = ruby_sourceline;
                    ;
    break;}
case 311:
#line 1639 "sydparse.y"
{
                        yyval.node = NEW_CLASS(yyvsp[-4].node, yyvsp[-1].node, yyvsp[-3].node);
                        nd_set_line(yyval.node, yyvsp[-2].num);
                        local_pop();
                        class_nest--;
                    ;
    break;}
case 312:
#line 1646 "sydparse.y"
{
                        yyval.num = in_def;
                        in_def = 0;
                    ;
    break;}
case 313:
#line 1651 "sydparse.y"
{
                        yyval.num = in_single;
                        in_single = 0;
                        class_nest++;
                        local_push(0);
                    ;
    break;}
case 314:
#line 1659 "sydparse.y"
{
                        yyval.node = NEW_SCLASS(yyvsp[-5].node, yyvsp[-1].node);
                        fixpos(yyval.node, yyvsp[-5].node);
                        local_pop();
                        class_nest--;
                        in_def = yyvsp[-4].num;
                        in_single = yyvsp[-2].num;
                    ;
    break;}
case 315:
#line 1668 "sydparse.y"
{
                        if (in_def || in_single)
                            yyerror("module definition in method body");
                        class_nest++;
                        local_push(0);
                        yyval.num = ruby_sourceline;
                    ;
    break;}
case 316:
#line 1677 "sydparse.y"
{
                        yyval.node = NEW_MODULE(yyvsp[-3].node, yyvsp[-1].node);
                        nd_set_line(yyval.node, yyvsp[-2].num);
                        local_pop();
                        class_nest--;
                    ;
    break;}
case 317:
#line 1684 "sydparse.y"
{
                        yyval.id = cur_mid;
                        cur_mid = yyvsp[0].id;
                        in_def++;
                        local_push(0);
                    ;
    break;}
case 318:
#line 1693 "sydparse.y"
{
                        if (!yyvsp[-1].node) yyvsp[-1].node = NEW_NIL();
                        yyval.node = NEW_DEFN(yyvsp[-4].id, yyvsp[-2].node, yyvsp[-1].node, NOEX_PRIVATE);
                        fixpos(yyval.node, yyvsp[-2].node);
                        local_pop();
                        in_def--;
                        cur_mid = yyvsp[-3].id;
                    ;
    break;}
case 319:
#line 1701 "sydparse.y"
{vps->lex_state = EXPR_FNAME;;
    break;}
case 320:
#line 1702 "sydparse.y"
{
                        in_single++;
                        local_push(0);
                        vps->lex_state = EXPR_END; /* force for args */
                    ;
    break;}
case 321:
#line 1710 "sydparse.y"
{
                        yyval.node = NEW_DEFS(yyvsp[-7].node, yyvsp[-4].id, yyvsp[-2].node, yyvsp[-1].node);
                        fixpos(yyval.node, yyvsp[-7].node);
                        local_pop();
                        in_single--;
                    ;
    break;}
case 322:
#line 1717 "sydparse.y"
{
                        yyval.node = NEW_BREAK(0);
                    ;
    break;}
case 323:
#line 1721 "sydparse.y"
{
                        yyval.node = NEW_NEXT(0);
                    ;
    break;}
case 324:
#line 1725 "sydparse.y"
{
                        yyval.node = NEW_REDO();
                    ;
    break;}
case 325:
#line 1729 "sydparse.y"
{
                        yyval.node = NEW_RETRY();
                    ;
    break;}
case 326:
#line 1735 "sydparse.y"
{
                        value_expr(yyvsp[0].node);
                        yyval.node = yyvsp[0].node;
                    ;
    break;}
case 335:
#line 1756 "sydparse.y"
{
                        yyval.node = NEW_IF(cond(yyvsp[-3].node, parse_state), yyvsp[-1].node, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-3].node);
                    ;
    break;}
case 337:
#line 1764 "sydparse.y"
{
                        yyval.node = yyvsp[0].node;
                    ;
    break;}
case 341:
#line 1775 "sydparse.y"
{
                        yyval.node = (NODE*)1;
                    ;
    break;}
case 342:
#line 1779 "sydparse.y"
{
                        yyval.node = (NODE*)1;
                    ;
    break;}
case 343:
#line 1783 "sydparse.y"
{
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 344:
#line 1789 "sydparse.y"
{
                        yyval.vars = dyna_push();
                        yyvsp[0].num = ruby_sourceline;
                    ;
    break;}
case 345:
#line 1793 "sydparse.y"
{yyval.vars = ruby_dyna_vars;;
    break;}
case 346:
#line 1796 "sydparse.y"
{
                        yyval.node = NEW_ITER(yyvsp[-3].node, 0, dyna_init(yyvsp[-1].node, yyvsp[-2].vars));
                        nd_set_line(yyval.node, yyvsp[-5].num);
                        dyna_pop(yyvsp[-4].vars);
                    ;
    break;}
case 347:
#line 1804 "sydparse.y"
{
                        if (yyvsp[-1].node && nd_type(yyvsp[-1].node) == NODE_BLOCK_PASS) {
                            rb_compile_error("both block arg and actual block given");
                        }
                        yyvsp[0].node->nd_iter = yyvsp[-1].node;
                        yyval.node = yyvsp[0].node;
                        fixpos(yyval.node, yyvsp[-1].node);
                    ;
    break;}
case 348:
#line 1813 "sydparse.y"
{
                        yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                    ;
    break;}
case 349:
#line 1817 "sydparse.y"
{
                        yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                    ;
    break;}
case 350:
#line 1823 "sydparse.y"
{
                        yyval.node = new_fcall(yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[0].node);
                    ;
    break;}
case 351:
#line 1828 "sydparse.y"
{
                        yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-3].node);
                    ;
    break;}
case 352:
#line 1833 "sydparse.y"
{
                        yyval.node = new_call(yyvsp[-3].node, yyvsp[-1].id, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-3].node);
                    ;
    break;}
case 353:
#line 1838 "sydparse.y"
{
                        yyval.node = new_call(yyvsp[-2].node, yyvsp[0].id, 0);
                    ;
    break;}
case 354:
#line 1842 "sydparse.y"
{
                        yyval.node = NEW_CALL(yyvsp[-2].node, rb_intern("get_reference"), NEW_LIST(NEW_LIT(ID2SYM(yyvsp[0].id))));
                    ;
    break;}
case 355:
#line 1846 "sydparse.y"
{
                        yyval.node = NEW_FCALL(rb_intern("get_reference"), NEW_LIST(NEW_LIT(ID2SYM(yyvsp[0].id))));
                    ;
    break;}
case 356:
#line 1850 "sydparse.y"
{
                        yyval.node = new_super(yyvsp[0].node);
                    ;
    break;}
case 357:
#line 1854 "sydparse.y"
{
                        yyval.node = NEW_ZSUPER();
                    ;
    break;}
case 358:
#line 1860 "sydparse.y"
{
                        yyval.vars = dyna_push();
                        yyvsp[0].num = ruby_sourceline;
                    ;
    break;}
case 359:
#line 1864 "sydparse.y"
{yyval.vars = ruby_dyna_vars;;
    break;}
case 360:
#line 1866 "sydparse.y"
{
                        yyval.node = NEW_ITER(yyvsp[-3].node, 0, dyna_init(yyvsp[-1].node, yyvsp[-2].vars));
                        nd_set_line(yyval.node, yyvsp[-5].num);
                        dyna_pop(yyvsp[-4].vars);
                    ;
    break;}
case 361:
#line 1872 "sydparse.y"
{
                        yyval.vars = dyna_push();
                        yyvsp[0].num = ruby_sourceline;
                    ;
    break;}
case 362:
#line 1876 "sydparse.y"
{yyval.vars = ruby_dyna_vars;;
    break;}
case 363:
#line 1878 "sydparse.y"
{
                        yyval.node = NEW_ITER(yyvsp[-3].node, 0, dyna_init(yyvsp[-1].node, yyvsp[-2].vars));
                        nd_set_line(yyval.node, yyvsp[-5].num);
                        dyna_pop(yyvsp[-4].vars);
                    ;
    break;}
case 364:
#line 1888 "sydparse.y"
{
                        yyval.node = NEW_WHEN(yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node);
                    ;
    break;}
case 366:
#line 1894 "sydparse.y"
{
                        yyval.node = list_append(yyvsp[-3].node, NEW_WHEN(yyvsp[0].node, 0, 0));
                    ;
    break;}
case 367:
#line 1898 "sydparse.y"
{
                        yyval.node = NEW_LIST(NEW_WHEN(yyvsp[0].node, 0, 0));
                    ;
    break;}
case 370:
#line 1910 "sydparse.y"
{
                        if (yyvsp[-3].node) {
                            yyvsp[-3].node = node_assign(yyvsp[-3].node, NEW_GVAR(rb_intern("$!")), parse_state);
                            yyvsp[-1].node = block_append(yyvsp[-3].node, yyvsp[-1].node);
                        }
                        yyval.node = NEW_RESBODY(yyvsp[-4].node, yyvsp[-1].node, yyvsp[0].node);
                        fixpos(yyval.node, yyvsp[-4].node?yyvsp[-4].node:yyvsp[-1].node);
                    ;
    break;}
case 372:
#line 1922 "sydparse.y"
{
                        yyval.node = NEW_LIST(yyvsp[0].node);
                    ;
    break;}
case 375:
#line 1930 "sydparse.y"
{
                        yyval.node = yyvsp[0].node;
                    ;
    break;}
case 377:
#line 1937 "sydparse.y"
{
                        if (yyvsp[0].node)
                            yyval.node = yyvsp[0].node;
                        else
                            /* place holder */
                            yyval.node = NEW_NIL();
                    ;
    break;}
case 380:
#line 1949 "sydparse.y"
{
                        yyval.node = NEW_LIT(ID2SYM(yyvsp[0].id));
                    ;
    break;}
case 382:
#line 1956 "sydparse.y"
{
                        NODE *node = yyvsp[0].node;
                        if (!node) {
                            node = NEW_STR(rb_str_new(0, 0));
                        }
                        else {
                            node = evstr2dstr(node);
                        }
                        yyval.node = node;
                    ;
    break;}
case 384:
#line 1970 "sydparse.y"
{
                        yyval.node = literal_concat(yyvsp[-1].node, yyvsp[0].node);
                    ;
    break;}
case 385:
#line 1976 "sydparse.y"
{
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 386:
#line 1982 "sydparse.y"
{
                        ID code = yyvsp[-2].id;
                        NODE *node = yyvsp[-1].node;
                        if (!node) {
                            node = NEW_XSTR(rb_str_new(0, 0));
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
                                node = NEW_NODE(NODE_DXSTR, rb_str_new(0, 0), 1, NEW_LIST(node));
                                break;
                            }
                        }
                        if(code) {
                            node->u2.id = code;
                        } else {
                            node->u2.id = 0;
                        }
                        yyval.node = node;
                    ;
    break;}
case 387:
#line 2011 "sydparse.y"
{
                        int options = yyvsp[0].num;
                        NODE *node = yyvsp[-1].node;
                        if (!node) {
                            node = NEW_LIT(rb_reg_new("", 0, options & ~RE_OPTION_ONCE));
                        }
                        else switch (nd_type(node)) {
                          case NODE_STR:
                            {
                                VALUE src = node->nd_lit;
                                nd_set_type(node, NODE_LIT);
                                node->nd_lit = rb_reg_new(RSTRING(src)->ptr,
                                                          RSTRING(src)->len,
                                                          options & ~RE_OPTION_ONCE);
                            }
                            break;
                          default:
                            node = NEW_NODE(NODE_DSTR, rb_str_new(0, 0), 1, NEW_LIST(node));
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
                    ;
    break;}
case 388:
#line 2044 "sydparse.y"
{
                        yyval.node = NEW_ZARRAY();
                    ;
    break;}
case 389:
#line 2048 "sydparse.y"
{
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 390:
#line 2054 "sydparse.y"
{
                        yyval.node = 0;
                    ;
    break;}
case 391:
#line 2058 "sydparse.y"
{
                        yyval.node = list_append(yyvsp[-2].node, evstr2dstr(yyvsp[-1].node));
                    ;
    break;}
case 393:
#line 2065 "sydparse.y"
{
                        yyval.node = literal_concat(yyvsp[-1].node, yyvsp[0].node);
                    ;
    break;}
case 394:
#line 2071 "sydparse.y"
{
                        yyval.node = NEW_ZARRAY();
                    ;
    break;}
case 395:
#line 2075 "sydparse.y"
{
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 396:
#line 2081 "sydparse.y"
{
                        yyval.node = 0;
                    ;
    break;}
case 397:
#line 2085 "sydparse.y"
{
                        yyval.node = list_append(yyvsp[-2].node, yyvsp[-1].node);
                    ;
    break;}
case 398:
#line 2091 "sydparse.y"
{
                        yyval.node = 0;
                    ;
    break;}
case 399:
#line 2095 "sydparse.y"
{
                        yyval.node = literal_concat(yyvsp[-1].node, yyvsp[0].node);
                    ;
    break;}
case 400:
#line 2101 "sydparse.y"
{
                        yyval.node = 0;
                    ;
    break;}
case 401:
#line 2105 "sydparse.y"
{
                        yyval.node = literal_concat(yyvsp[-1].node, yyvsp[0].node);
                    ;
    break;}
case 403:
#line 2112 "sydparse.y"
{
                        yyval.node = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                    ;
    break;}
case 404:
#line 2118 "sydparse.y"
{
                        lex_strterm = yyvsp[-1].node;
                        yyval.node = NEW_EVSTR(yyvsp[0].node);
                    ;
    break;}
case 405:
#line 2123 "sydparse.y"
{
                        yyval.node = lex_strterm;
                        lex_strterm = 0;
                        vps->lex_state = EXPR_BEG;
                        COND_PUSH(0);
                        CMDARG_PUSH(0);
                    ;
    break;}
case 406:
#line 2131 "sydparse.y"
{
                        lex_strterm = yyvsp[-2].node;
                        COND_LEXPOP();
                        CMDARG_LEXPOP();
                        if ((yyval.node = yyvsp[-1].node) && nd_type(yyval.node) == NODE_NEWLINE) {
                            yyval.node = yyval.node->nd_next;
                            rb_gc_force_recycle((VALUE)yyvsp[-1].node);
                        }
                        yyval.node = new_evstr(yyval.node);
                    ;
    break;}
case 407:
#line 2143 "sydparse.y"
{yyval.node = NEW_GVAR(yyvsp[0].id);;
    break;}
case 408:
#line 2144 "sydparse.y"
{yyval.node = NEW_IVAR(yyvsp[0].id);;
    break;}
case 409:
#line 2145 "sydparse.y"
{yyval.node = NEW_CVAR(yyvsp[0].id);;
    break;}
case 411:
#line 2150 "sydparse.y"
{
                        vps->lex_state = EXPR_END;
                        yyval.id = yyvsp[0].id;
                    ;
    break;}
case 416:
#line 2163 "sydparse.y"
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
                                if (strlen(RSTRING(yyval.node->nd_lit)->ptr) == RSTRING(yyval.node->nd_lit)->len) {
                                    yyval.node->nd_lit = ID2SYM(rb_intern(RSTRING(yyval.node->nd_lit)->ptr));
                                    nd_set_type(yyval.node, NODE_LIT);
                                    break;
                                }
                                /* fall through */
                              default:
                                yyval.node = NEW_NODE(NODE_DSYM, rb_str_new(0, 0), 1, NEW_LIST(yyval.node));
                                break;
                            }
                        }
                    ;
    break;}
case 419:
#line 2191 "sydparse.y"
{
                        yyval.node = negate_lit(yyvsp[0].node);
                    ;
    break;}
case 420:
#line 2195 "sydparse.y"
{
                        yyval.node = negate_lit(yyvsp[0].node);
                    ;
    break;}
case 426:
#line 2205 "sydparse.y"
{yyval.id = kNIL;;
    break;}
case 427:
#line 2206 "sydparse.y"
{yyval.id = kSELF;;
    break;}
case 428:
#line 2207 "sydparse.y"
{yyval.id = kTRUE;;
    break;}
case 429:
#line 2208 "sydparse.y"
{yyval.id = kFALSE;;
    break;}
case 430:
#line 2209 "sydparse.y"
{yyval.id = k__FILE__;;
    break;}
case 431:
#line 2210 "sydparse.y"
{yyval.id = k__LINE__;;
    break;}
case 432:
#line 2214 "sydparse.y"
{
                        yyval.node = gettable(yyvsp[0].id);
                    ;
    break;}
case 433:
#line 2220 "sydparse.y"
{
                        yyval.node = assignable(yyvsp[0].id, 0, parse_state);
                    ;
    break;}
case 436:
#line 2230 "sydparse.y"
{
                        yyval.node = 0;
                    ;
    break;}
case 437:
#line 2234 "sydparse.y"
{
                        vps->lex_state = EXPR_BEG;
                    ;
    break;}
case 438:
#line 2238 "sydparse.y"
{
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 439:
#line 2241 "sydparse.y"
{yyerrok; yyval.node = 0;;
    break;}
case 440:
#line 2245 "sydparse.y"
{
                        yyval.node = yyvsp[-2].node;
                        vps->lex_state = EXPR_BEG;
                    ;
    break;}
case 441:
#line 2250 "sydparse.y"
{
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 442:
#line 2256 "sydparse.y"
{
                        // printf("rest + all = %d\n", $5);
                        yyval.node = block_append(NEW_ARGS(yyvsp[-5].num, yyvsp[-3].node, yyvsp[-1].id), yyvsp[0].node);
                    ;
    break;}
case 443:
#line 2261 "sydparse.y"
{
                        yyval.node = block_append(NEW_ARGS(yyvsp[-3].num, yyvsp[-1].node, -1), yyvsp[0].node);
                    ;
    break;}
case 444:
#line 2265 "sydparse.y"
{
                        // printf("arg + rest = %d\n", $3);
                        yyval.node = block_append(NEW_ARGS(yyvsp[-3].num, 0, yyvsp[-1].id), yyvsp[0].node);
                    ;
    break;}
case 445:
#line 2270 "sydparse.y"
{
                        yyval.node = block_append(NEW_ARGS(yyvsp[-1].num, 0, -1), yyvsp[0].node);
                    ;
    break;}
case 446:
#line 2274 "sydparse.y"
{
                        // printf("rest + opt = %d\n", $3);
                        yyval.node = block_append(NEW_ARGS(0, yyvsp[-3].node, yyvsp[-1].id), yyvsp[0].node);
                    ;
    break;}
case 447:
#line 2279 "sydparse.y"
{
                        yyval.node = block_append(NEW_ARGS(0, yyvsp[-1].node, -1), yyvsp[0].node);
                    ;
    break;}
case 448:
#line 2283 "sydparse.y"
{
                        // printf("rest only = %d\n", $1);
                        yyval.node = block_append(NEW_ARGS(0, 0, yyvsp[-1].id), yyvsp[0].node);
                    ;
    break;}
case 449:
#line 2288 "sydparse.y"
{
                        yyval.node = block_append(NEW_ARGS(0, 0, -1), yyvsp[0].node);
                    ;
    break;}
case 450:
#line 2292 "sydparse.y"
{
                        yyval.node = NEW_ARGS(0, 0, -1);
                    ;
    break;}
case 451:
#line 2298 "sydparse.y"
{
                        yyerror("formal argument cannot be a constant");
                    ;
    break;}
case 452:
#line 2302 "sydparse.y"
{
                        yyerror("formal argument cannot be an instance variable");
                    ;
    break;}
case 453:
#line 2306 "sydparse.y"
{
                        yyerror("formal argument cannot be a global variable");
                    ;
    break;}
case 454:
#line 2310 "sydparse.y"
{
                        yyerror("formal argument cannot be a class variable");
                    ;
    break;}
case 455:
#line 2314 "sydparse.y"
{
                        if (!is_local_id(yyvsp[0].id))
                            yyerror("formal argument must be local variable");
                        else if (local_id(yyvsp[0].id))
                            yyerror("duplicate argument name");
                        local_cnt(yyvsp[0].id);
                        yyval.num = 1;
                    ;
    break;}
case 457:
#line 2326 "sydparse.y"
{
                        yyval.num += 1;
                    ;
    break;}
case 458:
#line 2332 "sydparse.y"
{
                        if (!is_local_id(yyvsp[-2].id))
                            yyerror("formal argument must be local variable");
                        else if (local_id(yyvsp[-2].id))
                            yyerror("duplicate optional argument name");
                        yyval.node = assignable(yyvsp[-2].id, yyvsp[0].node, parse_state);
                    ;
    break;}
case 459:
#line 2342 "sydparse.y"
{
                        yyval.node = NEW_BLOCK(yyvsp[0].node);
                        yyval.node->nd_end = yyval.node;
                    ;
    break;}
case 460:
#line 2347 "sydparse.y"
{
                        yyval.node = block_append(yyvsp[-2].node, yyvsp[0].node);
                    ;
    break;}
case 463:
#line 2357 "sydparse.y"
{
                        if (!is_local_id(yyvsp[0].id))
                            yyerror("rest argument must be local variable");
                        else if (local_id(yyvsp[0].id))
                            yyerror("duplicate rest argument name");
                        yyval.id = local_cnt(yyvsp[0].id);
                    ;
    break;}
case 464:
#line 2365 "sydparse.y"
{
                        yyval.id = -2;
                    ;
    break;}
case 467:
#line 2375 "sydparse.y"
{
                        if (!is_local_id(yyvsp[0].id))
                            yyerror("block argument must be local variable");
                        else if (local_id(yyvsp[0].id))
                            yyerror("duplicate block argument name");
                        yyval.node = NEW_BLOCK_ARG(yyvsp[0].id);
                    ;
    break;}
case 468:
#line 2385 "sydparse.y"
{
                        yyval.node = yyvsp[0].node;
                    ;
    break;}
case 470:
#line 2392 "sydparse.y"
{
                        if (nd_type(yyvsp[0].node) == NODE_SELF) {
                            yyval.node = NEW_SELF();
                        }
                        else {
                            yyval.node = yyvsp[0].node;
                            value_expr(yyval.node);
                        }
                    ;
    break;}
case 471:
#line 2401 "sydparse.y"
{vps->lex_state = EXPR_BEG;;
    break;}
case 472:
#line 2402 "sydparse.y"
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
                    ;
    break;}
case 474:
#line 2428 "sydparse.y"
{
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 475:
#line 2432 "sydparse.y"
{
                        if (yyvsp[-1].node->nd_alen%2 != 0) {
                            yyerror("odd number list for Hash");
                        }
                        yyval.node = yyvsp[-1].node;
                    ;
    break;}
case 477:
#line 2442 "sydparse.y"
{
                        yyval.node = list_concat(yyvsp[-2].node, yyvsp[0].node);
                    ;
    break;}
case 478:
#line 2448 "sydparse.y"
{ yyval.node = yyvsp[0].node; ;
    break;}
case 479:
#line 2450 "sydparse.y"
{ yyval.node  = yyvsp[0].node; ;
    break;}
case 480:
#line 2453 "sydparse.y"
{
                        yyval.node = list_append(NEW_LIST(yyvsp[-2].node), yyvsp[0].node);
                    ;
    break;}
case 481:
#line 2458 "sydparse.y"
{
                        yyval.node = list_append(NEW_LIST(NEW_LIT(ID2SYM(yyvsp[-1].id))), yyvsp[0].node);
                    ;
    break;}
case 501:
#line 2495 "sydparse.y"
{yyerrok;;
    break;}
case 504:
#line 2500 "sydparse.y"
{yyerrok;;
    break;}
case 505:
#line 2503 "sydparse.y"
{yyval.node = 0;;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 2505 "sydparse.y"

#include "regex.h"
#include "util.h"

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
    char *p, *pe, *buf;
    int len, i;
    
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

    return 0;
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
            fname = rb_str_new2(f);
            ruby_debug_lines = rb_hash_aref(hash, fname);
            if (NIL_P(ruby_debug_lines)) {
                ruby_debug_lines = rb_ary_new();
                rb_hash_aset(hash, fname, ruby_debug_lines);
            }
        }
        if (line > 1) {
            VALUE str = rb_str_new(0,0);
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
    ruby_current_node = 0;
    ruby_sourcefile = rb_source_filename(f);
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

static VALUE
lex_get_str(s, parse_state)
    VALUE s;
    rb_parse_state *parse_state;
{
    char *beg, *end, *pend;

    beg = RSTRING(s)->ptr;
    if (parse_state->lex_gets_ptr) {
        if (RSTRING(s)->len == parse_state->lex_gets_ptr) return Qnil;
        beg += parse_state->lex_gets_ptr;
    }
    pend = RSTRING(s)->ptr + RSTRING(s)->len;
    end = beg;
    while (end < pend) {
        if (*end++ == '\n') break;
    }
    parse_state->lex_gets_ptr = end - RSTRING(s)->ptr;
    return rb_str_new(beg, end - beg);
}

static VALUE
lex_getline(rb_parse_state *parse_state)
{
    VALUE line = (*(parse_state->lex_gets))(parse_state->lex_input, parse_state);
    if (ruby_debug_lines && !NIL_P(line)) {
        rb_ary_push(ruby_debug_lines, line);
    }
    return line;
}

NODE*
syd_compile_string(f, s, line, parse_state)
    const char *f;
    VALUE s;
    int line;
    rb_parse_state *parse_state;
{
    int n;
    
    /* Data_Get_Struct(CPATH(Ruby::PARSER), rb_parse_state, parse_state); */
    if(parse_state == NULL) {
        parse_state = alloc_parse_state();
    }
    parse_state->lex_gets = lex_get_str;
    parse_state->lex_gets_ptr = 0;
    parse_state->lex_input = s;
    parse_state->lex_pbeg = 0;
    parse_state->lex_p = 0;
    parse_state->lex_pend = 0;
    ruby_sourceline = line - 1;
    compile_for_eval = 1;
    
    rb_gc_register_address(&(parse_state->lex_input));
    rb_gc_register_address(&(parse_state->lex_lastline));

    n = yycompile(parse_state, f, line);
    // ruby_eval_tree = parse_state->top;
    // if (n != 0) ruby_eval_tree_begin = 0;
    rb_gc_unregister_address(&(parse_state->lex_input));
    rb_gc_unregister_address(&(parse_state->lex_lastline));

    if(!n) {
        return parse_state->top;
    } else {
        return 0;
    }
}

NODE*
syd_compile_cstr(f, s, len, line)
    const char *f, *s;
    int len, line;
{
    return syd_compile_string(f, rb_str_new(s, len), line, NULL);
}

static VALUE parse_io_gets(VALUE input, rb_parse_state *parse_state) {
    return rb_io_gets(input);
}

int
syd_compile_file_from_state(f, file, start, parse_state)
    const char *f;
    VALUE file;
    int start;
    rb_parse_state *parse_state;
{
    int n;

    parse_state->lex_gets = parse_io_gets;
    parse_state->lex_input = file;
    parse_state->lex_pbeg = 0;
    parse_state->lex_p = 0;
    parse_state->lex_pend = 0;
    ruby_sourceline = start - 1;

    rb_gc_register_address(&(parse_state->lex_input));
    rb_gc_register_address(&(parse_state->lex_lastline));

    n = yycompile(parse_state, f, start);
    
    rb_gc_unregister_address(&(parse_state->lex_input));
    rb_gc_unregister_address(&(parse_state->lex_lastline));

    return n;
}

NODE *syd_compile_file(f, file, start)
    const char *f;
    VALUE file;
    int start;
{
    int n;
    rb_parse_state *parse_state;
    parse_state = alloc_parse_state();
    n = syd_compile_file_from_state(f, file, start, parse_state);
    if(!n) {
        // ruby_eval_tree = parse_state->top;
        return parse_state->top;
    } else {
        // ruby_eval_tree_begin = 0;
        return 0;
    }
}

#define nextc() ps_nextc(parse_state)

static inline int
ps_nextc(rb_parse_state *parse_state)
{
    int c;

    if (parse_state->lex_p == parse_state->lex_pend) {
        if (parse_state->lex_input) {
            VALUE v = lex_getline(parse_state);

            if (NIL_P(v)) return -1;
            if (heredoc_end > 0) {
                ruby_sourceline = heredoc_end;
                heredoc_end = 0;
            }
            ruby_sourceline++;
            parse_state->lex_pbeg = parse_state->lex_p = RSTRING(v)->ptr;
            parse_state->lex_pend = parse_state->lex_p + RSTRING(v)->len;
            parse_state->lex_lastline = v;
        }
        else {
            parse_state->lex_lastline = 0;
            return -1;
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

#define was_bol() (parse_state->lex_p == parse_state->lex_pbeg + 1)
#define peek(c) (parse_state->lex_p != parse_state->lex_pend && (c) == *(parse_state->lex_p))


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

static void
tokadd(c, parse_state)
    char c;
    rb_parse_state *parse_state;
{
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

            tokadd('\\', parse_state);
            tokadd(c, parse_state);
            for (i=0; i<2; i++) {
                c = nextc();
                if (c == -1) goto eof;
                if (c < '0' || '7' < c) {
                    pushback(c, parse_state);
                    break;
                }
                tokadd(c, parse_state);
            }
        }
        return 0;

      case 'x': /* hex constant */
        {
            int numlen;

            tokadd('\\', parse_state);
            tokadd(c, parse_state);
            scan_hex(parse_state->lex_p, 2, &numlen);
            if (numlen == 0) {
                yyerror("Invalid escape character syntax");
                return -1;
            }
            while (numlen--)
                tokadd(nextc(), parse_state);
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
        tokadd(c, parse_state);
        return 0;

      eof:
      case -1:
        yyerror("Invalid escape character syntax");
        return -1;

      default:
        if (c != '\\' || c != term)
            tokadd('\\', parse_state);
        tokadd(c, parse_state);
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
            tokadd(c, parse_state);
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
    VALUE str;
{
    free(RSTRING(str)->ptr);
    rb_gc_force_recycle(str);
}

static int
tokadd_string(func, term, paren, nest, parse_state)
    int func, term, paren, *nest;
    rb_parse_state *parse_state;
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
                if (func & STR_FUNC_ESCAPE) tokadd(c, parse_state);
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
                tokadd(c, parse_state);
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
        tokadd(c, parse_state);
    }
    return c;
}

#define NEW_STRTERM(func, term, paren) \
        syd_node_newnode(NODE_STRTERM, (func), (term) | ((paren) << (CHAR_BIT * 2)), 0)
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
    if (tokadd_string(func, term, paren, &quote->nd_nest, parse_state) == -1) {
        ruby_sourceline = nd_line(quote);
        rb_compile_error("unterminated string meets end of file");
        return tSTRING_END;
    }

    tokfix();
    pslval->node = NEW_STR(rb_str_new(tok(), toklen()));
    return tSTRING_CONTENT;
}

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
        newtok(parse_state);
        tokadd(func, parse_state);
        term = c;
        while ((c = nextc()) != -1 && c != term) {
            len = mbclen(c);
            do {tokadd(c, parse_state);} while (--len > 0 && (c = nextc()) != -1);
        }
        if (c == -1) {
            rb_compile_error("unterminated here document identifier");
            return 0;
        }
        break;

      default:
        if (!is_identchar(c)) {
            pushback(c, parse_state);
            if (func & STR_FUNC_INDENT) {
                pushback('-', parse_state);
            }
            return 0;
        }
        newtok(parse_state);
        term = '"';
        tokadd(func |= str_dquote, parse_state);
        do {
            len = mbclen(c);
            do {tokadd(c, parse_state);} while (--len > 0 && (c = nextc()) != -1);
        } while ((c = nextc()) != -1 && is_identchar(c));
        pushback(c, parse_state);
        break;
    }

    tokfix();
    len = parse_state->lex_p - parse_state->lex_pbeg;
    parse_state->lex_p = parse_state->lex_pend;
    pslval->id = 0;
    lex_strterm = syd_node_newnode(NODE_HEREDOC,
                                  rb_str_new(tok(), toklen()),  /* nd_lit */
                                  len,                          /* nd_nth */
                                  parse_state->lex_lastline);           /* nd_orig */
    return term == '`' ? tXSTRING_BEG : tSTRING_BEG;
}

static void
heredoc_restore(here, parse_state)
    NODE *here;
    rb_parse_state *parse_state;
{
    VALUE line = here->nd_orig;
    parse_state->lex_lastline = line;
    parse_state->lex_pbeg = RSTRING(line)->ptr;
    parse_state->lex_pend = parse_state->lex_pbeg + RSTRING(line)->len;
    parse_state->lex_p = parse_state->lex_pbeg + here->nd_nth;
    heredoc_end = ruby_sourceline;
    ruby_sourceline = nd_line(here);
    dispose_string(here->nd_lit);
    rb_gc_force_recycle((VALUE)here);
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
    n= parse_state->lex_pend - (p + len);
    if (n < 0 || (n > 0 && p[len] != '\n' && p[len] != '\r')) return Qfalse;
    if (strncmp(eos, p, len) == 0) return Qtrue;
    return Qfalse;
}

static int
here_document(here, parse_state)
    NODE *here;
    rb_parse_state *parse_state;
{
    int c, func, indent = 0;
    char *eos, *p, *pend;
    long len;
    VALUE str = 0;

    eos = RSTRING(here->nd_lit)->ptr;
    len = RSTRING(here->nd_lit)->len - 1;
    indent = (func = *eos++) & STR_FUNC_INDENT;

    if ((c = nextc()) == -1) {
      error:
        rb_compile_error("can't find string \"%s\" anywhere before EOF", eos);
        heredoc_restore(lex_strterm, parse_state);
        lex_strterm = 0;
        return 0;
    }
    if (was_bol() && whole_match_p(eos, len, indent, parse_state)) {
        heredoc_restore(lex_strterm, parse_state);
        return tSTRING_END;
    }

    if (!(func & STR_FUNC_EXPAND)) {
        do {
            p = RSTRING(parse_state->lex_lastline)->ptr;
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
                rb_str_cat(str, p, pend - p);
            else
                str = rb_str_new(p, pend - p);
            if (pend < parse_state->lex_pend) rb_str_cat(str, "\n", 1);
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
        do {
            pushback(c, parse_state);
            if ((c = tokadd_string(func, '\n', 0, NULL, parse_state)) == -1) goto error;
            if (c != '\n') {
                pslval->node = NEW_STR(rb_str_new(tok(), toklen()));
                return tSTRING_CONTENT;
            }
            tokadd(nextc(), parse_state);
            if ((c = nextc()) == -1) goto error;
        } while (!whole_match_p(eos, len, indent, parse_state));
        str = rb_str_new(tok(), toklen());
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

static int
yylex(YYSTYPE *yylval, void *vstate)
{
    register int c;
    int space_seen = 0;
    int cmd_state, comment_column;
    struct rb_parse_state *parse_state;
    VALUE cur_line;
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
                rb_gc_force_recycle((VALUE)lex_strterm);
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
            cur_line = rb_str_new(0, 50);
            
            while((c = nextc()) != '\n' && c != -1) {
                RSTRING(cur_line)->ptr[i++] = c;
                if(i == RSTRING(cur_line)->len) {
                    rb_str_resize(cur_line, i+50);
                }
            }
            
            rb_str_resize(cur_line, i);
            rb_ary_push(parse_state->comments, 
                rb_ary_new3(3, INT2NUM(ruby_sourceline), INT2NUM(comment_column),
                    cur_line));
            
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
        pslval->node = NEW_LIT(INT2FIX(c));
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
                tokadd(c,parse_state);
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
                            tokadd(c,parse_state);
                        } while ((c = nextc()) != -1);
                    }
                    pushback(c, parse_state);
                    tokfix();
                    if (toklen() == start) {
                        yyerror("numeric literal without digits");
                    }
                    else if (nondigit) goto trailing_uc;
                    pslval->node = NEW_LIT(rb_cstr_to_inum(tok(), 16, Qfalse));
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
                            tokadd(c, parse_state);
                        } while ((c = nextc()) != -1);
                    }
                    pushback(c, parse_state);
                    tokfix();
                    if (toklen() == start) {
                        yyerror("numeric literal without digits");
                    }
                    else if (nondigit) goto trailing_uc;
                    pslval->node = NEW_LIT(rb_cstr_to_inum(tok(), 2, Qfalse));
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
                            tokadd(c, parse_state);
                        } while ((c = nextc()) != -1);
                    }
                    pushback(c, parse_state);
                    tokfix();
                    if (toklen() == start) {
                        yyerror("numeric literal without digits");
                    }
                    else if (nondigit) goto trailing_uc;
                    pslval->node = NEW_LIT(rb_cstr_to_inum(tok(), 10, Qfalse));
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
                        tokadd(c, parse_state);
                    } while ((c = nextc()) != -1);
                    if (toklen() > start) {
                        pushback(c, parse_state);
                        tokfix();
                        if (nondigit) goto trailing_uc;
                        pslval->node = NEW_LIT(rb_cstr_to_inum(tok(), 8, Qfalse));
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
                    pslval->node = NEW_LIT(INT2FIX(0));
                    return tINTEGER;
                }
            }

            for (;;) {
                switch (c) {
                  case '0': case '1': case '2': case '3': case '4':
                  case '5': case '6': case '7': case '8': case '9':
                    nondigit = 0;
                    tokadd(c, parse_state);
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
                    tokadd(c, parse_state);
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
                    tokadd(c, parse_state);
                    seen_e++;
                    is_float++;
                    nondigit = c;
                    c = nextc();
                    if (c != '-' && c != '+') continue;
                    tokadd(c, parse_state);
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
                double d = strtod(tok(), 0);
                if (errno == ERANGE) {
                    rb_warn("Float %s out of range", tok());
                    errno = 0;
                }
                pslval->node = NEW_LIT(rb_float_new(d));
                return tFLOAT;
            }
            pslval->node = NEW_LIT(rb_cstr_to_inum(tok(), 10, Qfalse));
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
            tokadd(c, parse_state);
            tokfix();
            pslval->id = rb_intern(tok());
            return tGVAR;

          case '-':
            tokadd('$', parse_state);
            tokadd(c, parse_state);
            c = nextc();
            tokadd(c, parse_state);
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
                tokadd(c, parse_state);
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
        tokadd(c, parse_state);
        if (ismbchar(c)) {
            int i, len = mbclen(c)-1;

            for (i = 0; i < len; i++) {
                c = nextc();
                tokadd(c, parse_state);
            }
        }
        c = nextc();
    } while (is_identchar(c));
    if ((c == '!' || c == '?') && is_identchar(tok()[0]) && !peek('=')) {
        tokadd(c, parse_state);
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
                        tokadd(c, parse_state);
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
        if (is_local_id(pslval->id) &&
            ((syd_dyna_in_block(parse_state) && 
                      rb_dvar_defined(pslval->id)) || local_id(pslval->id))) {
            parse_state->lex_state = EXPR_END;
        }
        return result;
    }
}

NODE*
syd_node_newnode(type, a0, a1, a2)
    enum node_type type;
    VALUE a0, a1, a2;
{
    NODE *n = (NODE*)rb_newobj();

    n->flags |= T_NODE;
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
newline_node(node)
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
    rb_warning(mesg);
    ruby_sourceline = line;
}

static void
parser_warn(node, mesg)
    NODE *node;
    const char *mesg;
{
    int line = ruby_sourceline;
    ruby_sourceline = nd_line(node);
    rb_warn(mesg);
    ruby_sourceline = line;
}

static NODE*
block_append(head, tail)
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
list_append(list, item)
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
literal_concat(head, tail)
    NODE *head, *tail;
{
    enum node_type htype;

    if (!head) return tail;
    if (!tail) return head;

    htype = nd_type(head);
    if (htype == NODE_EVSTR) {
        NODE *node = NEW_DSTR(rb_str_new(0, 0));
        head = list_append(node, head);
    }
    switch (nd_type(tail)) {
      case NODE_STR:
        if (htype == NODE_STR) {
            rb_str_concat(head->nd_lit, tail->nd_lit);
            rb_gc_force_recycle((VALUE)tail);
        }
        else {
            list_append(head, tail);
        }
        break;

      case NODE_DSTR:
        if (htype == NODE_STR) {
            rb_str_concat(head->nd_lit, tail->nd_lit);
            tail->nd_lit = head->nd_lit;
            rb_gc_force_recycle((VALUE)head);
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
        list_append(head, tail);
        break;
    }
    return head;
}

static NODE *
evstr2dstr(node)
    NODE *node;
{
    if (nd_type(node) == NODE_EVSTR) {
        node = list_append(NEW_DSTR(rb_str_new(0, 0)), node);
    }
    return node;
}

static NODE *
new_evstr(node)
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
    int i;
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

          case NODE_LIT:
            if (TYPE(node1->nd_lit) == T_REGEXP) {
                return NEW_MATCH2(node1, node2);
            }
        }
    }

    if (node2) {
        switch (nd_type(node2)) {
          case NODE_DREGX:
          case NODE_DREGX_ONCE:
            return NEW_MATCH3(node2, node1);

          case NODE_LIT:
            if (TYPE(node2->nd_lit) == T_REGEXP) {
                return NEW_MATCH3(node2, node1);
            }
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
        return NEW_STR(rb_str_new2(ruby_sourcefile));
    }
    else if (id == k__LINE__) {
        return NEW_LIT(INT2FIX(ruby_sourceline));
    }
    else if (is_local_id(id)) {
        if (syd_dyna_in_block(parse_state) && rb_dvar_defined(id)) return NEW_DVAR(id);
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
    rb_compile_error("identifier %s is not valid", rb_id2name(id));
    return 0;
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
        if (rb_dvar_curr(id)) {
            return NEW_DASGN_CURR(id, val);
        }
        else if (rb_dvar_defined(id)) {
            return NEW_DASGN(id, val);
        }
        else if (local_id(id) || !syd_dyna_in_block(parse_state)) {
            return NEW_LASGN(id, val);
        }
        else{
            rb_dvar_push(id, Qnil);
            return NEW_DASGN_CURR(id, val);
        }
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
        rb_compile_error("identifier %s is not valid", rb_id2name(id));
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

/*
ID
rb_id_attrset(id)
    ID id;
{
    id &= ~ID_SCOPE_MASK;
    id |= ID_ATTRSET;
    return id;
}
*/

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
        rb_compile_error("Can't set variable $%d", node->nd_nth);
        break;
      case NODE_BACK_REF:
        rb_compile_error("Can't set variable $%c", (int)node->nd_nth);
        break;
    }
}

static NODE *
arg_concat(node1, node2)
    NODE *node1;
    NODE *node2;
{
    if (!node2) return node1;
    return NEW_ARGSCAT(node1, node2);
}

static NODE *
arg_add(node1, node2)
    NODE *node1;
    NODE *node2;
{
    if (!node1) return NEW_LIST(node2);
    if (nd_type(node1) == NODE_ARRAY) {
        return list_append(node1, node2);
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
        lhs->nd_args = arg_add(lhs->nd_args, rhs);
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
            useless = rb_id2name(node->nd_mid);
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

      case NODE_LIT:
        if (TYPE(node->nd_lit) == T_REGEXP) {
            nd_set_type(node, NODE_MATCH);
            local_cnt('_');
            local_cnt('~');
        }
        else {
        }
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
ret_args(node)
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
new_yield(node)
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
arg_prepend(node1, node2)
    NODE *node1, *node2;
{
    switch (nd_type(node2)) {
      case NODE_ARRAY:
        return list_concat(NEW_LIST(node1), node2);

      case NODE_SPLAT:
        return arg_concat(node1, node2->nd_head);

      case NODE_BLOCK_PASS:
        node2->nd_body = arg_prepend(node1, node2->nd_body);
        return node2;

      default:
        rb_bug("unknown nodetype(%d) for arg_prepend", nd_type(node2));
    }
    return 0;                   /* not reached */
}

static NODE*
new_call(r,m,a)
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
new_fcall(m,a)
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
new_super(a)
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
    rb_funcall(st->self, rb_intern("local_push"), 1, top ? Qtrue : Qfalse);
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
    rb_funcall(st->self, rb_intern("local_pop"), 0);
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
    ID tmp;
    VALUE tbl;
    int i, len;
    tbl = rb_funcall(st->self, rb_intern("local_table"), 0);
    len = RARRAY(tbl)->len;
    // printf("Converting local table with %d entries.\n", len);
    lcl_tbl = ALLOC_N(ID, len + 3);
    lcl_tbl[0] = len;
    lcl_tbl[1] = '_';
    lcl_tbl[2] = '~';
    for(i = 0; i < len; i++) {
        // printf("Adding entry at %d\n", i);
        tmp = SYM2ID(RARRAY(tbl)->ptr[i]);
        lcl_tbl[i + 3] = SYM2ID(RARRAY(tbl)->ptr[i]);
    }
    // printf("Created table %x\n", lcl_tbl);
    return lcl_tbl;
}

static int
syd_local_append(rb_parse_state *st, ID id)
{
    VALUE out;
    out = rb_funcall(st->self, rb_intern("local_append"), 1, ID2SYM(id));
    return NUM2INT(out);
    /*
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
    VALUE out;
    /* Leave these hardcoded here because they arne't REALLY ids at all. */
    if(id == '_') {
        return 0;
    } else if(id == '~') {
        return 1;
    }
    out = rb_funcall(st->self, rb_intern("local_cnt"), 1, ID2SYM(id));
    
    return NUM2INT(out);
    /*
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
    VALUE out;
    out = rb_funcall(st->self, rb_intern("local_defined?"), 1, ID2SYM(id));
    return out;
    /*
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
 */
static VALUE
syd_dyna_push(rb_parse_state *st)
{
    return rb_funcall(st->self, rb_intern("dyna_push"), 0);
}

static void
syd_dyna_pop(rb_parse_state *st, VALUE vars)
{
    rb_funcall(st->self, rb_intern("dyna_pop"), 1, vars);
    /*
    lvtbl->dlev--;
    ruby_dyna_vars = vars;
    */
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

static NODE *
syd_dyna_init(st, node, pre)
    rb_parse_state *st;
    NODE *node;
    VALUE pre;
{
    VALUE vars;
    int i;
    // struct RVarmap *post = ruby_dyna_vars;
    NODE *var, *out;
    
    if (!node) return node;
    
    vars = rb_funcall(st->self, rb_intern("dyna_init"), 1, pre);
    if(RARRAY(vars)->len == 0) return node;
    
    var = NULL;
    // if (!node || !post || pre == post) return node;
    for(i = 0; i < RARRAY(vars)->len; i++) {
        var = NEW_DASGN_CURR(SYM2ID(RARRAY(vars)->ptr[i]), var);
    }
    /*
    for (var = 0; post != pre && post->id; post = post->next) {
        var = NEW_DASGN_CURR(post->id, var);
    }
    */
    out = block_append(var, node);
    return out;
}

static VALUE
syd_dyna_vars(rb_parse_state *st) {
    return rb_funcall(st->self, rb_intern("dyna_vars"), 0);
}

int
syd_parser_stack_on_heap()
{
#if defined(YYBISON) && !defined(C_ALLOCA)
    return Qfalse;
#else
    return Qtrue;
#endif
}

void
syd_gc_mark_parser(rb_parse_state *parse_state)
{
    // if (!ruby_in_compile) return;

    rb_gc_mark_maybe((VALUE)pslval->node);
    /* rb_gc_mark(ruby_debug_lines); */
    rb_gc_mark(parse_state->lex_lastline);
    rb_gc_mark(parse_state->lex_input);
    /* rb_gc_mark((VALUE)lex_strterm); */
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
/*
ID
rb_intern(name)
    const char *name;
{
    const char *m = name;
    ID id;
    int last;

    if (st_lookup(sym_tbl, (st_data_t)name, (st_data_t *)&id)) {
        return id;
    } 
    
    last = strlen(name)-1;
    id = 0;
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
                    goto id_regist;
                }
            }
        }

        if (name[last] == '=') {
            char *buf = ALLOCA_N(char,last+1);

            strncpy(buf, name, last);
            buf[last] = '\0';
            id = rb_intern(buf);
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
    id |= ++STATE_VAR(last_id) << ID_SCOPE_SHIFT;
  id_regist:
    name = strdup(name);
    st_add_direct(sym_tbl, (st_data_t)name, id);
    st_add_direct(sym_rev_tbl, id, (st_data_t)name);
    return id;
}

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

    if (st_lookup(sym_rev_tbl, id, (st_data_t *)&name))
        return name;

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
#ifndef MEL_SYMBOLS_HPP
#define MEL_SYMBOLS_HPP

#ifdef __cplusplus
extern "C" {
#endif

namespace melbourne {
  extern ID rb_sAlias;
  extern ID rb_sAnd;
  extern ID rb_sArgs;
  extern ID rb_sArgsCat;
  extern ID rb_sArgsPush;
  extern ID rb_sArray;
  extern ID rb_sAttrAsgn;
  extern ID rb_sBackRef;
  extern ID rb_sBegin;
  extern ID rb_sBlock;
  extern ID rb_sBlockArg;
  extern ID rb_sBlockPass;
  extern ID rb_sBreak;
  extern ID rb_sCall;
  extern ID rb_sCase;
  extern ID rb_sCDecl;
  extern ID rb_sClass;
  extern ID rb_sColon2;
  extern ID rb_sColon3;
  extern ID rb_sConst;
  extern ID rb_sCVar;
  extern ID rb_sCVAsgn;
  extern ID rb_sCVDecl;
  extern ID rb_sData;
  extern ID rb_sDefined;
  extern ID rb_sDefn;
  extern ID rb_sDefs;
  extern ID rb_sDot2;
  extern ID rb_sDot3;
  extern ID rb_sDRegx;
  extern ID rb_sDRegxOnce;
  extern ID rb_sDStr;
  extern ID rb_sDSym;
  extern ID rb_sDXStr;
  extern ID rb_sEncoding;
  extern ID rb_sEnsure;
  extern ID rb_sEvStr;
  extern ID rb_sFalse;
  extern ID rb_sFCall;
  extern ID rb_sFile;
  extern ID rb_sFixnum;
  extern ID rb_sFlip2;
  extern ID rb_sFlip3;
  extern ID rb_sFloat;
  extern ID rb_sFor;
  extern ID rb_sGAsgn;
  extern ID rb_sGVar;
  extern ID rb_sHash;
  extern ID rb_sIAsgn;
  extern ID rb_sIf;
  extern ID rb_sIter;
  extern ID rb_sIVar;
  extern ID rb_sLAsgn;
  extern ID rb_sLambda;
  extern ID rb_sLit;
  extern ID rb_sLVar;
  extern ID rb_sMAsgn;
  extern ID rb_sMatch;
  extern ID rb_sMatch2;
  extern ID rb_sMatch3;
  extern ID rb_sModule;
  extern ID rb_sNegate;
  extern ID rb_sNext;
  extern ID rb_sNil;
  extern ID rb_sNot;
  extern ID rb_sNthRef;
  extern ID rb_sNumber;
  extern ID rb_sOpAsgn1;
  extern ID rb_sOpAsgn2;
  extern ID rb_sOpAsgnAnd;
  extern ID rb_sOpAsgnOr;
  extern ID rb_sOptArg;
  extern ID rb_sOr;
  extern ID rb_sPostExe;
  extern ID rb_sPostArg;
  extern ID rb_sPreExe;
  extern ID rb_sRedo;
  extern ID rb_sRegex;
  extern ID rb_sResbody;
  extern ID rb_sRescue;
  extern ID rb_sRetry;
  extern ID rb_sReturn;
  extern ID rb_sSClass;
  extern ID rb_sScope;
  extern ID rb_sSelf;
  extern ID rb_sSplat;
  extern ID rb_sStr;
  extern ID rb_sSuper;
  extern ID rb_sSValue;
  extern ID rb_sToAry;
  extern ID rb_sTrue;
  extern ID rb_sUndef;
  extern ID rb_sUntil;
  extern ID rb_sVAlias;
  extern ID rb_sValues;
  extern ID rb_sVCall;
  extern ID rb_sWhen;
  extern ID rb_sWhile;
  extern ID rb_sXStr;
  extern ID rb_sYield;
  extern ID rb_sZArray;
  extern ID rb_sZSuper;

  void init_symbols20();
};

#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif

#ifndef RBX_CAPI_CONSTANTS_H
#define RBX_CAPI_CONSTANTS_H

/**
 *  Global object abstraction.
 *
 *  @internal.
 *
 *  ALWAYS add new entries at the end right before cCApiMaxConstant, otherwise
 *  the ABI for C extensions is changed and it breaks all currently compiled ones.
 */
typedef enum {
  cCApiArray = 0,
  cCApiBignum,
  cCApiClass,
  cCApiComparable,
  cCApiData,
  cCApiEnumerable,
  cCApiFalse,
  cCApiFile,
  cCApiFixnum,
  cCApiFloat,
  cCApiHash,
  cCApiInteger,
  cCApiIO,
  cCApiKernel,
  cCApiMatch,
  cCApiModule,
  cCApiNil,
  cCApiNumeric,
  cCApiObject,
  cCApiRange,
  cCApiRegexp,
  cCApiRubinius,
  cCApiString,
  cCApiStruct,
  cCApiSymbol,
  cCApiThread,
  cCApiTime,
  cCApiTrue,
  cCApiProc,
  cCApiGC,
  cCApiCAPI,

  cCApiArgumentError,
  cCApiEOFError,
  cCApiErrno,
  cCApiException,
  cCApiFatal,
  cCApiFloatDomainError,
  cCApiIndexError,
  cCApiInterrupt,
  cCApiIOError,
  cCApiLoadError,
  cCApiLocalJumpError,
  cCApiNameError,
  cCApiNoMemoryError,
  cCApiNoMethodError,
  cCApiNotImplementedError,
  cCApiRangeError,
  cCApiRegexpError,
  cCApiRuntimeError,
  cCApiScriptError,
  cCApiSecurityError,
  cCApiSignalException,
  cCApiStandardError,
  cCApiSyntaxError,
  cCApiSystemCallError,
  cCApiSystemExit,
  cCApiSystemStackError,
  cCApiTypeError,
  cCApiThreadError,
  cCApiZeroDivisionError,
  cCApiMethod,
  cCApiRational,
  cCApiComplex,
  cCApiMathDomainError,
  cCApiEncoding,
  cCApiEncCompatError,
  cCApiWaitReadable,
  cCApiWaitWritable,
  cCApiEnumerator,
  cCApiMutex,

  // MUST be last
  cCApiMaxConstant
} CApiConstant;

/**
 *  Method variants that can be defined.
 */
typedef enum {
  cCApiPublicMethod,
  cCApiProtectedMethod,
  cCApiPrivateMethod,
  cCApiSingletonMethod

} CApiMethodKind;

#endif

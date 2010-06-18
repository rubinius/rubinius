#ifndef RUBYSPEC_H
#define RUBYSPEC_H
// First define all function flags

// Array
#define HAVE_RARRAY                        1
#define HAVE_RARRAY_LEN                    1
#define HAVE_RARRAY_PTR                    1
#define HAVE_RB_ARY_AREF                   1
#define HAVE_RB_ARY_CLEAR                  1
#define HAVE_RB_ARY_DELETE                 1
#define HAVE_RB_ARY_DUP                    1
#define HAVE_RB_ARY_ENTRY                  1
#define HAVE_RB_ARY_INCLUDES               1
#define HAVE_RB_ARY_JOIN                   1
#define HAVE_RB_ARY_NEW                    1
#define HAVE_RB_ARY_NEW2                   1
#define HAVE_RB_ARY_POP                    1
#define HAVE_RB_ARY_PUSH                   1
#define HAVE_RB_ARY_REVERSE                1
#define HAVE_RB_ARY_SHIFT                  1
#define HAVE_RB_ARY_STORE                  1
#define HAVE_RB_ARY_UNSHIFT                1
#define HAVE_RB_ASSOC_NEW                  1

#define HAVE_RB_EACH                       1
#define HAVE_RB_ITERATE                    1

// Bignum
#define HAVE_RB_BIG2DBL                    1
#define HAVE_RB_BIG2LL                     1
#define HAVE_RB_BIG2LONG                   1
#define HAVE_RB_BIG2STR                    1
#define HAVE_RB_BIG2ULONG                  1

// Class
#define HAVE_RB_CALL_SUPER                 1
#define HAVE_RB_CLASS2NAME                 1
#define HAVE_RB_CLASS_INHERITED            1
#define HAVE_RB_CLASS_NEW                  1
#define HAVE_RB_CLASS_NEW_INSTANCE         1
#define HAVE_RB_CVAR_DEFINED               1
#define HAVE_RB_CVAR_GET                   1
#define HAVE_RB_CVAR_SET                   1
#define HAVE_RB_CV_GET                     1
#define HAVE_RB_CV_SET                     1
#define HAVE_RB_DEFINE_ATTR                1
#define HAVE_RB_DEFINE_CLASS_VARIABLE      1
#define HAVE_RB_INCLUDE_MODULE             1

// Constants
#define HAVE_RB_CARRAY                     1
#define HAVE_RB_CBIGNUM                    1
#define HAVE_RB_CCLASS                     1
#define HAVE_RB_CDATA                      1
#define HAVE_RB_CFALSECLASS                1
#define HAVE_RB_CFILE                      1
#define HAVE_RB_CFIXNUM                    1
#define HAVE_RB_CFLOAT                     1
#define HAVE_RB_CHASH                      1
#define HAVE_RB_CINTEGER                   1
#define HAVE_RB_CIO                        1
#define HAVE_RB_CMODULE                    1
#define HAVE_RB_CMATCH                     1
#define HAVE_RB_CNILCLASS                  1
#define HAVE_RB_CNUMERIC                   1
#define HAVE_RB_COBJECT                    1
#define HAVE_RB_CRANGE                     1
#define HAVE_RB_CREGEXP                    1
#define HAVE_RB_CSTRING                    1
#define HAVE_RB_CSTRUCT                    1
#define HAVE_RB_CSYMBOL                    1
#define HAVE_RB_CTHREAD                    1
#define HAVE_RB_CTRUECLASS                 1
#define HAVE_RB_CPROC                      1
#define HAVE_RB_MCOMPARABLE                1
#define HAVE_RB_MENUMERABLE                1
#define HAVE_RB_MKERNEL                    1
#define HAVE_RB_EARGERROR                  1
#define HAVE_RB_EEOFERROR                  1
#define HAVE_RB_MERRNO                     1
#define HAVE_RB_EEXCEPTION                 1
#define HAVE_RB_EFLOATDOMAINERROR          1
#define HAVE_RB_EINDEXERROR                1
#define HAVE_RB_EINTERRUPT                 1
#define HAVE_RB_EIOERROR                   1
#define HAVE_RB_ELOADERROR                 1
#define HAVE_RB_ELOCALJUMPERROR            1
#define HAVE_RB_ENAMEERROR                 1
#define HAVE_RB_ENOMEMERROR                1
#define HAVE_RB_ENOMETHODERROR             1
#define HAVE_RB_ENOTIMPERROR               1
#define HAVE_RB_ERANGEERROR                1
#define HAVE_RB_EREGEXPERROR               1
#define HAVE_RB_ERUNTIMEERROR              1
#define HAVE_RB_ESCRIPTERROR               1
#define HAVE_RB_ESECURITYERROR             1
#define HAVE_RB_ESIGNAL                    1
#define HAVE_RB_ESTANDARDERROR             1
#define HAVE_RB_ESYNTAXERROR               1
#define HAVE_RB_ESYSTEMCALLERROR           1
#define HAVE_RB_ESYSTEMEXIT                1
#define HAVE_RB_ESYSSTACKERROR             1
#define HAVE_RB_ETYPEERROR                 1
#define HAVE_RB_ETHREADERROR               1
#define HAVE_RB_EZERODIVERROR              1

// Data
#define HAVE_DATA_WRAP_STRUCT              1
#define HAVE_RDATA                         1

// Exception
#define HAVE_RB_EXC_NEW                    1
#define HAVE_RB_EXC_NEW2                   1
#define HAVE_RB_EXC_NEW3                   1
#define HAVE_RB_EXC_RAISE                  1
#define HAVE_RB_SET_ERRINFO                1

// Float
#define HAVE_RB_FLOAT_NEW                  1
#define HAVE_RB_RFLOAT                     1
#define HAVE_RFLOAT                        1

// Globals
#define HAVE_RB_DEFINE_HOOKED_VARIABLE     1
#define HAVE_RB_DEFINE_READONLY_VARIABLE   1
#define HAVE_RB_DEFINE_VARIABLE            1
#define HAVE_RB_F_GLOBAL_VARIABLES         1
#define HAVE_RB_GV_GET                     1
#define HAVE_RB_GV_SET                     1
#define HAVE_RB_SET_KCODE                  1

// Hash
#define HAVE_RB_HASH_AREF                  1
#define HAVE_RB_HASH_ASET                  1
#define HAVE_RB_HASH_DELETE                1
#define HAVE_RB_HASH_FOREACH               1
#define HAVE_RB_HASH_LOOKUP                1
#define HAVE_RB_HASH_NEW                   1
#define HAVE_RB_HASH_SIZE                  1

// IO
#define HAVE_RB_IO_WRITE                   1

// Kernel
#define HAVE_RB_BLOCK_GIVEN_P              1
#define HAVE_RB_BLOCK_PROC                 1
#define HAVE_RB_ENSURE                     1
#define HAVE_RB_EVAL_STRING                1
#define HAVE_RB_RAISE                      1
#define HAVE_RB_RESCUE                     1
#define HAVE_RB_RESCUE2                    1
#define HAVE_RB_SYS_FAIL                   1
#define HAVE_RB_WARN                       1
#define HAVE_RB_YIELD                      1

// GC
#define HAVE_RB_GC_REGISTER_ADDRESS        1

// Module
#define HAVE_RB_CONST_DEFINED              1
#define HAVE_RB_CONST_DEFINED_AT           1
#define HAVE_RB_CONST_GET                  1
#define HAVE_RB_CONST_GET_AT               1
#define HAVE_RB_CONST_GET_FROM             1
#define HAVE_RB_CONST_SET                  1
#define HAVE_RB_DEFINE_ALIAS               1
#define HAVE_RB_DEFINE_CLASS_UNDER         1
#define HAVE_RB_DEFINE_MODULE_UNDER        1
#define HAVE_RB_DEFINE_CONST               1
#define HAVE_RB_DEFINE_GLOBAL_CONST        1
#define HAVE_RB_DEFINE_GLOBAL_FUNCTION     1
#define HAVE_RB_DEFINE_METHOD              1
#define HAVE_RB_DEFINE_MODULE_FUNCTION     1
#define HAVE_RB_DEFINE_PRIVATE_METHOD      1
#define HAVE_RB_DEFINE_PROTECTED_METHOD    1
#define HAVE_RB_DEFINE_SINGLETON_METHOD    1
#define HAVE_RB_UNDEF_METHOD               1

// Numeric
#define HAVE_NUM2CHR                       1
#define HAVE_RB_INT2INUM                   1
#define HAVE_RB_INTEGER                    1
#define HAVE_RB_LL2INUM                    1
#define HAVE_RB_NUM2DBL                    1
#define HAVE_RB_NUM2LONG                   1
#define HAVE_RB_NUM2ULONG                  1

// Object
#define HAVE_OBJ_TAINT                     1
#define HAVE_OBJ_TAINTED                   1
#define HAVE_RB_ANY_TO_S                   1
#define HAVE_RB_ATTR_GET                   1
#define HAVE_RB_CHECK_ARRAY_TYPE           1
#define HAVE_RB_CHECK_CONVERT_TYPE         1
#define HAVE_RB_CHECK_FROZEN               1
#define HAVE_RB_CHECK_STRING_TYPE          1
#define HAVE_RB_CLASS_OF                   1
#define HAVE_RB_CONVERT_TYPE               1
#define HAVE_RB_EXTEND_OBJECT              1
#define HAVE_RB_INSPECT                    1
#define HAVE_RB_OBJ_ALLOC                  1
#define HAVE_RB_OBJ_CALL_INIT              1
#define HAVE_RB_OBJ_CLASSNAME              1
#define HAVE_RB_OBJ_FREEZE                 1
#define HAVE_RB_OBJ_FROZEN_P               1
#define HAVE_RB_OBJ_ID                     1
#define HAVE_RB_OBJ_IS_INSTANCE_OF         1
#define HAVE_RB_OBJ_IS_KIND_OF             1
#define HAVE_RB_OBJ_TAINT                  1
#define HAVE_RB_REQUIRE                    1
#define HAVE_RB_RESPOND_TO                 1
#define HAVE_RB_SPECIAL_CONST_P            1
#define HAVE_RB_TO_ID                      1
#define HAVE_RB_TO_INT                     1
#define HAVE_RTEST                         1
#define HAVE_TYPE                          1

// Proc
#define HAVE_RB_PROC_NEW                   1

// Range
#define HAVE_RB_RANGE_NEW                  1

// Safe
#define HAVE_RB_SAFE_LEVEL                 1
#define HAVE_RB_SECURE                     1

// String
#define HAVE_RB_CSTR2INUM                  1
#define HAVE_RB_STR2CSTR                   1
#define HAVE_RB_STR2INUM                   1
#define HAVE_RB_STR_APPEND                 1
#define HAVE_RB_STR_BUF_CAT                1
#define HAVE_RB_STR_CAT                    1
#define HAVE_RB_STR_CAT2                   1
#define HAVE_RB_STR_CMP                    1
#define HAVE_RB_STR_DUP                    1
#define HAVE_RB_STR_FLUSH                  1
#define HAVE_RB_STR_FREEZE                 1
#define HAVE_RB_STR_INTERN                 1
#define HAVE_RB_STR_LEN                    1
#define HAVE_RB_STR_NEW                    1
#define HAVE_RB_STR_NEW2                   1
#define HAVE_RB_STR_NEW3                   1
#define HAVE_RB_STR_PLUS                   1
#define HAVE_RB_STR_PTR                    1
#define HAVE_RB_STR_PTR_READONLY           1
#define HAVE_RB_STR_RESIZE                 1
#define HAVE_RB_STR_SPLIT                  1
#define HAVE_RB_STR_SUBSTR                 1
#define HAVE_RB_STR_TO_STR                 1
#define HAVE_RSTRING                       1
#define HAVE_RSTRING_LEN                   1
#define HAVE_RSTRING_PTR                   1
#define HAVE_STR2CSTR                      1
#define HAVE_STRINGVALUE                   1

// Struct
#define HAVE_RB_STRUCT_DEFINE              1

// Symbol
#define HAVE_RB_IS_CLASS_ID                1
#define HAVE_RB_IS_CONST_ID                1
#define HAVE_RB_IS_INSTANCE_ID             1

// Thread
#define HAVE_RB_THREAD_ALONE               1
#define HAVE_RB_THREAD_BLOCKING_REGION     1
#define HAVE_RB_THREAD_CURRENT             1
#define HAVE_RB_THREAD_LOCAL_AREF          1
#define HAVE_RB_THREAD_LOCAL_ASET          1
#define HAVE_RB_THREAD_SELECT              1

// Time
#define HAVE_RB_TIME_NEW                   1

/* Now, create the differential set. The format of the preprocessor directives
 * is significant. The alternative implementations should define RUBY because
 * some extensions depend on that. But only one alternative implementation
 * macro should be defined at a time. The conditional is structured so that if
 * no alternative implementation is defined then MRI is assumed and "mri.h"
 * will be included.
 */

#if defined(RUBINIUS)
#include "rubinius.h"
#elif defined(JRUBY)
#include "jruby.h"
#else // MRI
#include "mri.h"
#endif

#endif

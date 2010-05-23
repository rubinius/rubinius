#include "vm/test/test.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"
#include "builtin/nativemethod.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

#include "capi_reset.h"

using namespace rubinius;
using namespace rubinius::capi;

class TestCApiConstants : public CxxTest::TestSuite, public VMTest {
  public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void get_and_compare_constant_name(CApiConstant type, std::string name) {
    TS_ASSERT(name == capi_get_constant_name(type));
  }

  void test_get_Array_name() {
    get_and_compare_constant_name(cCApiArray, "Array");
  }

  void test_get_Bignum_name() {
    get_and_compare_constant_name(cCApiBignum, "Bignum");
  }

  void test_get_Class_name() {
    get_and_compare_constant_name(cCApiClass, "Class");
  }

  void test_get_Comparable_name() {
    get_and_compare_constant_name(cCApiComparable, "Comparable");
  }

  void test_get_Data_name() {
    get_and_compare_constant_name(cCApiData, "Data");
  }

  void test_get_Enumerable_name() {
    get_and_compare_constant_name(cCApiEnumerable, "Enumerable");
  }

  void test_get_FalseClass_name() {
    get_and_compare_constant_name(cCApiFalse, "FalseClass");
  }

  void test_get_Fixnum_name() {
    get_and_compare_constant_name(cCApiFixnum, "Fixnum");
  }

  void test_get_Float_name() {
    get_and_compare_constant_name(cCApiFloat, "Float");
  }

  void test_get_Hash_name() {
    get_and_compare_constant_name(cCApiHash, "Hash");
  }

  void test_get_Integer_name() {
    get_and_compare_constant_name(cCApiInteger, "Integer");
  }

  void test_get_IO_name() {
    get_and_compare_constant_name(cCApiIO, "IO");
  }

  void test_get_Kernel_name() {
    get_and_compare_constant_name(cCApiKernel, "Kernel");
  }

  void test_get_Module_name() {
    get_and_compare_constant_name(cCApiModule, "Module");
  }

  void test_get_NilClass_name() {
    get_and_compare_constant_name(cCApiNil, "NilClass");
  }

  void test_get_Object_name() {
    get_and_compare_constant_name(cCApiObject, "Object");
  }

  void test_get_Regexp_name() {
    get_and_compare_constant_name(cCApiRegexp, "Regexp");
  }

  void test_get_String_name() {
    get_and_compare_constant_name(cCApiString, "String");
  }

  void test_get_Symbol_name() {
    get_and_compare_constant_name(cCApiSymbol, "Symbol");
  }

  void test_get_Thread_name() {
    get_and_compare_constant_name(cCApiThread, "Thread");
  }

  void test_get_TrueClass_name() {
    get_and_compare_constant_name(cCApiTrue, "TrueClass");
  }

  void test_get_ArgumentError_name() {
    get_and_compare_constant_name(cCApiArgumentError, "ArgumentError");
  }

  void test_get_EOFError_name() {
    get_and_compare_constant_name(cCApiEOFError, "EOFError");
  }

  void test_get_Errno_name() {
    get_and_compare_constant_name(cCApiErrno, "Errno");
  }

  void test_get_Exception_name() {
    get_and_compare_constant_name(cCApiException, "Exception");
  }

  void test_get_Fatal_name() {
    get_and_compare_constant_name(cCApiFatal, "FatalError");
  }

  void test_get_FloatDomainError_name() {
    get_and_compare_constant_name(cCApiFloatDomainError, "FloatDomainError");
  }

  void test_get_IndexError_name() {
    get_and_compare_constant_name(cCApiIndexError, "IndexError");
  }

  void test_get_Interrupt_name() {
    get_and_compare_constant_name(cCApiInterrupt, "Interrupt");
  }

  void test_get_IOError_name() {
    get_and_compare_constant_name(cCApiIOError, "IOError");
  }

  void test_get_LoadError_name() {
    get_and_compare_constant_name(cCApiLoadError, "LoadError");
  }

  void test_get_LocalJumpError_name() {
    get_and_compare_constant_name(cCApiLocalJumpError, "LocalJumpError");
  }

  void test_get_NameError_name() {
    get_and_compare_constant_name(cCApiNameError, "NameError");
  }

  void test_get_NoMemoryError_name() {
    get_and_compare_constant_name(cCApiNoMemoryError, "NoMemoryError");
  }

  void test_get_NoMethodError_name() {
    get_and_compare_constant_name(cCApiNoMethodError, "NoMethodError");
  }

  void test_get_NotImplementedError_name() {
    get_and_compare_constant_name(cCApiNotImplementedError, "NotImplementedError");
  }

  void test_get_RangeError_name() {
    get_and_compare_constant_name(cCApiRangeError, "RangeError");
  }

  void test_get_RegexpError_name() {
    get_and_compare_constant_name(cCApiRegexpError, "RegexpError");
  }

  void test_get_RuntimeError_name() {
    get_and_compare_constant_name(cCApiRuntimeError, "RuntimeError");
  }

  void test_get_ScriptError_name() {
    get_and_compare_constant_name(cCApiScriptError, "ScriptError");
  }

  void test_get_SecurityError_name() {
    get_and_compare_constant_name(cCApiSecurityError, "SecurityError");
  }

  void test_get_SignalException_name() {
    get_and_compare_constant_name(cCApiSignalException, "SignalException");
  }

  void test_get_StandardError_name() {
    get_and_compare_constant_name(cCApiStandardError, "StandardError");
  }

  void test_get_SyntaxError_name() {
    get_and_compare_constant_name(cCApiSyntaxError, "SyntaxError");
  }

  void test_get_SystemCallError_name() {
    get_and_compare_constant_name(cCApiSystemCallError, "SystemCallError");
  }

  void test_get_SystemExit_name() {
    get_and_compare_constant_name(cCApiSystemExit, "SystemExit");
  }

  void test_get_SystemStackError_name() {
    get_and_compare_constant_name(cCApiSystemStackError, "SystemStackError");
  }

  void test_get_TypeError_name() {
    get_and_compare_constant_name(cCApiTypeError, "TypeError");
  }

  void test_get_ThreadError_name() {
    get_and_compare_constant_name(cCApiThreadError, "ThreadError");
  }

  void test_get_ZeroDivisionError_name() {
    get_and_compare_constant_name(cCApiZeroDivisionError, "ZeroDivisionError");
  }

  // @todo test out-of-bounds index
};

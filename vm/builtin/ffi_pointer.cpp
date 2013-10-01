#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/ffi_pointer.hpp"
#include "builtin/native_function.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "ffi_util.hpp"
#include "object_utils.hpp"
#include "object_memory.hpp"
#include "ontology.hpp"
#include "windows_compat.h"

#ifdef RBX_WINDOWS
#include <winsock2.h>
#else
#include <dlfcn.h>
#include <arpa/inet.h>
#endif

namespace rubinius {

  void Pointer::init(STATE) {
    GO(ffi_pointer).set(ontology::new_class_under(state, "Pointer", G(ffi)));
    G(ffi_pointer)->set_object_type(state, PointerType);

    G(ffi_pointer)->set_const(state, "CURRENT_PROCESS",
      Pointer::create(state, dlopen(NULL, RTLD_NOW | RTLD_GLOBAL)));

    G(ffi_pointer)->set_const(state, "DLSYM",
      Pointer::create(state, (void*)dlsym));

    G(ffi_pointer)->set_const(state, "DLOPEN",
      Pointer::create(state, (void*)dlopen));

    G(ffi_pointer)->set_const(state, "DLERROR",
      Pointer::create(state, (void*)dlerror));

    // Not exactly the right place, but FFI needs this and this is the main
    // FFI file atm.
    Array* suffix = Array::create(state, 2);
#ifdef _WIN32
    suffix->set(state, 0, String::create(state, "dll"));
#else
  #ifdef __APPLE_CC__
    suffix->set(state, 0, String::create(state, "bundle"));
    suffix->set(state, 1, String::create(state, "dylib"));
  #else
    suffix->set(state, 0, String::create(state, "so"));
  #endif
#endif

    G(ffi)->set_const(state, "LIB_SUFFIXES", suffix);

    // Legacy. Fix the kernel to not need this.
    String* main_suffix;
#ifdef _WIN32
    main_suffix = String::create(state, ".dll");
#else
  #ifdef __APPLE_CC__
    main_suffix = String::create(state, ".bundle");
  #else
    main_suffix = String::create(state, ".so");
  #endif
#endif

    G(rubinius)->set_const(state, "LIBSUFFIX", main_suffix);
  }

  Pointer* Pointer::create(STATE, void* ptr) {
    Pointer* obj = state->new_object<Pointer>(G(ffi_pointer));
    obj->pointer = ptr;
    obj->autorelease = false;
    obj->set_finalizer = false;
    return obj;
  }

  Pointer* Pointer::allocate(STATE, Object* self) {
    Pointer* obj = state->new_object<Pointer>(as<Class>(self));
    obj->pointer = 0;
    obj->autorelease = false;
    obj->set_finalizer = false;
    return obj;
  }

  Pointer* Pointer::allocate_memory(STATE, Object* self, Fixnum* size) {
    Pointer* obj = state->vm()->new_object<Pointer>(as<Class>(self));
    obj->pointer = malloc(size->to_native());;
    obj->autorelease = false;
    obj->set_finalizer = false;
    return obj;
  }

  Integer* Pointer::get_address(STATE) {
    return Integer::from(state, (uintptr_t)pointer);
  }

  Integer* Pointer::set_address(STATE, Integer* ptr) {
    pointer = (void*)ptr->to_native();
    return ptr;
  }

  Pointer* Pointer::add(STATE, Integer* amount) {
    return Pointer::create(state, (char*)pointer + amount->to_native());
  }

  Object* Pointer::set_autorelease(STATE, Object* val) {
    autorelease = val->true_p() ? true : false;

    if(autorelease && !set_finalizer) {
      state->memory()->needs_finalization(this,
          (FinalizerFunction)&Pointer::finalize);
      set_finalizer = true;
    }

    return val;
  }

  Object* Pointer::free(STATE) {
    if(pointer) ::free(pointer);
    autorelease = false;
    pointer = NULL;
    return cNil;
  }

  void Pointer::finalize(STATE, Pointer* ptr) {
    if(ptr->autorelease && ptr->pointer) {
      ::free(ptr->pointer);
    }
    ptr->pointer = NULL;
  }

  String* Pointer::read_string(STATE, Fixnum* len) {
    return String::create(state, (char*)pointer, len->to_native());
  }

  String* Pointer::read_string_to_null(STATE) {
    return String::create(state, (char*)pointer);
  }

  Pointer* Pointer::write_string(STATE, String* str, Fixnum* len) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to write string");

    native_int n = len->to_native();
    if(str->byte_size() < n) n = str->byte_size();
    memcpy(pointer, (void*)str->byte_address(), n);
    return this;
  }

  Integer* Pointer::write_char(STATE, Integer* val) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to write char");

    unsigned char s = val->to_native();
    *(unsigned char*)pointer = s;
    return val;
  }

  Integer* Pointer::read_char(STATE, Object* sign) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to read char");

    if(CBOOL(sign)) {
      return Integer::from(state, *(char*)pointer);
    } else {
      return Integer::from(state, *(unsigned char*)pointer);
    }
  }

  Integer* Pointer::write_short(STATE, Integer* val) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to write short");

    unsigned short s = val->to_native();
    *(unsigned short*)pointer = s;
    return val;
  }

  Integer* Pointer::read_short(STATE, Object* sign) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to read short");

    if(CBOOL(sign)) {
      return Integer::from(state, *(short*)pointer);
    } else {
      return Integer::from(state, *(unsigned short*)pointer);
    }
  }

  Integer* Pointer::write_int(STATE, Integer* val) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to write int");

    *(int*)pointer = val->to_native();
    return val;
  }

  Integer* Pointer::read_int(STATE, Object* sign) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to read int");

    if(CBOOL(sign)) {
      return Integer::from(state, *(int*)pointer);
    } else {
      return Integer::from(state, *(unsigned int*)pointer);
    }
  }

  Integer* Pointer::write_long(STATE, Integer* val) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to write long");

    *(long*)pointer = val->to_native();
    return val;
  }

  Integer* Pointer::read_long(STATE, Object* sign) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to read long");

    if(CBOOL(sign)) {
      return Integer::from(state, *(long*)pointer);
    } else {
      return Integer::from(state, *(unsigned long*)pointer);
    }
  }

  Integer* Pointer::write_long_long(STATE, Integer* val) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to write long long");

    *(long long*)pointer = val->to_long_long();
    return val;
  }

  Integer* Pointer::read_long_long(STATE, Object* sign) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to read long long");

    if(CBOOL(sign)) {
      return Integer::from(state, *(long long*)pointer);
    } else {
      return Integer::from(state, *(unsigned long long*)pointer);
    }
  }

  Float* Pointer::write_float(STATE, Float* flt) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to write float");

    *(float*)pointer = (float)flt->val;
    return flt;
  }

  Float* Pointer::read_float(STATE) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to read float");

    return Float::create(state, (double)(*(float*)pointer));
  }

  Float* Pointer::write_double(STATE, Float* flt) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to write double");

    *(double*)pointer = flt->val;
    return flt;
  }

  Float* Pointer::read_double(STATE) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to read double");

    return Float::create(state, *(double*)pointer);
  }

  Pointer* Pointer::write_pointer(STATE, Pointer* ptr) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to write pointer");

    *(void**)pointer = ptr->pointer;
    return ptr;
  }

  Pointer* Pointer::read_pointer(STATE) {
    if(!pointer) Exception::argument_error(state, "invalid pointer to read pointer");

    return Pointer::create(state, *(void**)pointer);
  }

  Object* Pointer::network_order(STATE, Fixnum* offset, Fixnum* intsize) {

    if(!pointer) Exception::argument_error(state, "invalid pointer to network order value");

    native_int size = intsize->to_native();

    char* pos = ((char*)pointer) + offset->to_native();

#define ptr_to(type) (*(type*)pos)

    switch(size) {
    case 2:
      ptr_to(uint16_t) = htons(ptr_to(uint16_t));
      return Integer::from(state, ptr_to(uint16_t));
    case 4:
      ptr_to(uint32_t) = htonl(ptr_to(uint32_t));
      return Integer::from(state, ptr_to(uint32_t));
    case 8:
      ptr_to(uint32_t) = htonl(ptr_to(uint32_t));
      pos += 4; // 32 bits
      ptr_to(uint32_t) = htonl(ptr_to(uint32_t));
      return Integer::from(state, *(uint64_t*)(pos - 4));
    }

    return Primitives::failure();
  }

  Object* Pointer::get_at_offset(STATE, Fixnum* offset, Fixnum* type) {
    return get_field(state, offset->to_native(), type->to_native());
  }

  Object* Pointer::get_field(STATE, int offset, int type) {
    Object* ret;
    if(!pointer) Exception::argument_error(state, "invalid pointer to get field from");

    char* ptr = (char*)pointer;

    ptr += offset;

#define READ(type) (*((type*)(ptr)))

    switch(type) {
    case RBX_FFI_TYPE_CHAR:
      ret = Fixnum::from((int)(READ(char)));
      break;
    case RBX_FFI_TYPE_UCHAR:
      ret = Fixnum::from((unsigned int)(READ(unsigned char)));
      break;
    case RBX_FFI_TYPE_BOOL:
      ret = RBOOL(READ(unsigned char));
      break;
    case RBX_FFI_TYPE_SHORT:
      ret = Fixnum::from((int)(READ(short)));
      break;
    case RBX_FFI_TYPE_USHORT:
      ret = Fixnum::from((unsigned int)(READ(unsigned short)));
      break;
    case RBX_FFI_TYPE_INT:
      ret = Integer::from(state, READ(int));
      break;
    case RBX_FFI_TYPE_UINT:
      ret = Integer::from(state, READ(unsigned int));
      break;
    case RBX_FFI_TYPE_LONG:
      ret = Integer::from(state, READ(long));
      break;
    case RBX_FFI_TYPE_ULONG:
      ret = Integer::from(state, READ(unsigned long));
      break;
    case RBX_FFI_TYPE_FLOAT:
      ret = Float::create(state, (double)READ(float));
      break;
    case RBX_FFI_TYPE_DOUBLE:
      ret = Float::create(state, READ(double));
      break;
    case RBX_FFI_TYPE_LONG_LONG:
      ret = Integer::from(state, READ(long long));
      break;
    case RBX_FFI_TYPE_ULONG_LONG:
      ret = Integer::from(state, READ(unsigned long long));
      break;
    case RBX_FFI_TYPE_PTR: {
      void *lptr = READ(void*);
      if(!lptr) {
        ret = cNil;
      } else {
        ret = Pointer::create(state, lptr);
      }
      break;
    }
    case RBX_FFI_TYPE_STRING: {
      char* result = READ(char*);
      if(result == NULL) {
        ret = cNil;
      } else {
        ret = String::create(state, result);
      }
      break;
    }
    case RBX_FFI_TYPE_STRPTR: {
      char* result;
      Object* s;
      Object* p;

      result = READ(char*);

      if(result == NULL) {
        s = p = cNil;
      } else {
        s = String::create(state, result);
        p = Pointer::create(state, result);
      }

      Array* ary = Array::create(state, 2);
      ary->set(state, 0, s);
      ary->set(state, 1, p);
      ret = ary;
      break;
    }
    default:
    case RBX_FFI_TYPE_VOID:
      ret = cNil;
      break;
    }

    return ret;
  }

  Object* Pointer::set_at_offset(STATE, Fixnum* offset, Fixnum* type, Object* val) {
    set_field(state, offset->to_native(), type->to_native(), val);
    return val;
  }

  void Pointer::set_field(STATE, int offset, int type, Object* val) {

    if(!pointer) Exception::argument_error(state, "invalid pointer to set field of");

    char* ptr = (char*)pointer;

    ptr += offset;

#define WRITE(type, val) *(reinterpret_cast<type*>(ptr)) = (type)val

    switch(type) {
    case RBX_FFI_TYPE_CHAR:
      type_assert(state, val, FixnumType, "converting to char");
      WRITE(char, as<Fixnum>(val)->to_native());
      break;
    case RBX_FFI_TYPE_UCHAR:
      type_assert(state, val, FixnumType, "converting to unsigned char");
      WRITE(unsigned char, as<Fixnum>(val)->to_native());
      break;
    case RBX_FFI_TYPE_BOOL:
      WRITE(unsigned char, CBOOL(val) ? 1 : 0);
      break;
    case RBX_FFI_TYPE_SHORT:
      type_assert(state, val, FixnumType, "converting to short");
      WRITE(short, as<Fixnum>(val)->to_native());
      break;
    case RBX_FFI_TYPE_USHORT:
      type_assert(state, val, FixnumType, "converting to unsigned short");
      WRITE(unsigned short, as<Fixnum>(val)->to_native());
      break;
    case RBX_FFI_TYPE_INT:
      if(val->fixnum_p()) {
        WRITE(int, as<Fixnum>(val)->to_int());
      } else {
        type_assert(state, val, BignumType, "converting to int");
        WRITE(int, as<Bignum>(val)->to_int());
      }
      break;
    case RBX_FFI_TYPE_UINT:
      if(val->fixnum_p()) {
        WRITE(unsigned int, as<Fixnum>(val)->to_uint());
      } else {
        type_assert(state, val, BignumType, "converting to unsigned int");
        WRITE(unsigned int, as<Bignum>(val)->to_uint());
      }
      break;
    case RBX_FFI_TYPE_LONG:
      if(val->fixnum_p()) {
        WRITE(long, as<Fixnum>(val)->to_long());
      } else {
        type_assert(state, val, BignumType, "converting to long");
        WRITE(long, as<Bignum>(val)->to_long());
      }
      break;
    case RBX_FFI_TYPE_ULONG:
      if(val->fixnum_p()) {
        WRITE(unsigned long, as<Fixnum>(val)->to_ulong());
      } else {
        type_assert(state, val, BignumType, "converting to unsigned long");
        WRITE(unsigned long, as<Bignum>(val)->to_ulong());
      }
      break;
    case RBX_FFI_TYPE_FLOAT: {
      Float* flt = as<Float>(val);
      type_assert(state, val, FloatType, "converting to float");
      WRITE(float, flt->to_double(state));
      break;
    }
    case RBX_FFI_TYPE_DOUBLE: {
      Float* flt = as<Float>(val);
      type_assert(state, val, FloatType, "converting to double");
      WRITE(double, flt->to_double(state));
      break;
    }
    case RBX_FFI_TYPE_LONG_LONG:
      if(val->fixnum_p()) {
        WRITE(long long, as<Fixnum>(val)->to_long_long());
      } else {
        type_assert(state, val, BignumType, "converting to long long");
        WRITE(long long, as<Bignum>(val)->to_long_long());
      }
      break;
    case RBX_FFI_TYPE_ULONG_LONG:
      if(val->fixnum_p()) {
        WRITE(unsigned long long, as<Fixnum>(val)->to_ulong_long());
      } else {
        type_assert(state, val, BignumType, "converting to unsigned long long");
        WRITE(unsigned long long, as<Bignum>(val)->to_ulong_long());
      }
      break;
    case RBX_FFI_TYPE_PTR:
      if(val->nil_p()) {
        WRITE(void*, NULL);
      } else {
        Pointer *mp = as<Pointer>(val);
        type_assert(state, val, PointerType, "converting to pointer");
        WRITE(void*, mp->pointer);
      }
      break;
    case RBX_FFI_TYPE_STRING: {
      const char* result;
      if(val->nil_p()) {
        result = NULL;
      } else {
        String* str = as<String>(val);
        /* TODO this is probably not correct. Saving away an
         * internal pointer to the string means that when the string
         * moves, the data will point at the wrong place. Probably need to
         * copy the string data instead */
        result = str->c_str(state);
      }
      WRITE(const char*, result);
      break;
    }
    default:
      rubinius::bug("Unknown FFI type");
    }
  }

  void Pointer::Info::mark(Object* obj, ObjectMark& mark) {
  }
}


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <arpa/inet.h>

#include "vm.hpp"
#include "oop.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/memorypointer.hpp"
#include "builtin/nativefunction.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

#include "ffi_util.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"

namespace rubinius {

  void MemoryPointer::init(STATE) {
    Module* ffi = as<Module>(G(object)->get_const(state, "FFI"));
    GO(memory_pointer).set(state->new_class_under("MemoryPointer", ffi));
    G(memory_pointer)->set_object_type(state, MemoryPointerType);
  }

  MemoryPointer* MemoryPointer::create(STATE, void* ptr) {
    MemoryPointer* obj = state->new_struct<MemoryPointer>(G(memory_pointer));
    obj->pointer = ptr;
    obj->autorelease = false;
    obj->set_finalizer = false;
    return obj;
  }

  Integer* MemoryPointer::get_address(STATE) {
    return Integer::from(state, (intptr_t)pointer);
  }

  Integer* MemoryPointer::set_address(STATE, Integer* ptr) {
    pointer = (void*)ptr->to_native();
    return ptr;
  }

  MemoryPointer* MemoryPointer::add(STATE, Integer* amount) {
    return MemoryPointer::create(state, (char*)pointer + amount->to_native());
  }

  Object* MemoryPointer::set_autorelease(STATE, Object* val) {
    autorelease = val->true_p() ? true : false;

    if(autorelease && !set_finalizer) {
      state->om->needs_finalization(this,
          (FinalizerFunction)&MemoryPointer::finalize);
      set_finalizer = true;
    }

    return val;
  }

  void MemoryPointer::finalize(STATE, MemoryPointer* ptr) {
    if(ptr->autorelease && ptr->pointer) {
      ::free(ptr->pointer);
    }
  }

  String* MemoryPointer::read_string(STATE, Fixnum* len) {
    // HM. This is pretty dangerous. Should we figure out how to
    // protect this?
    return String::create(state, (char*)pointer, len->to_native());
  }

  String* MemoryPointer::read_string_to_null(STATE) {
    // Danger!
    // This operation might be too dangerous! You can read into any
    // memory using it!
    return String::create(state, (char*)pointer);
  }

  MemoryPointer* MemoryPointer::write_string(STATE, String* str, Fixnum* len) {
    memcpy(pointer, (void*)str->byte_address(), len->to_native());
    return this;
  }

  Integer* MemoryPointer::write_short(STATE, Integer* val) {
    unsigned short s = val->to_native();
    *(unsigned short*)pointer = s;
    return val;
  }

  Integer* MemoryPointer::read_short(STATE) {
    return Integer::from(state, *(short*)pointer);
  }

  Integer* MemoryPointer::write_int(STATE, Integer* val) {
    *(int*)pointer = val->to_native();
    return val;
  }

  Integer* MemoryPointer::read_int(STATE, Object* sign) {
    if(RTEST(sign)) {
      return Integer::from(state, *(int*)pointer);
    } else {
      return Integer::from(state, *(unsigned int*)pointer);
    }
  }

  Integer* MemoryPointer::write_long(STATE, Integer* val) {
    *(long*)pointer = val->to_native();
    return val;
  }

  Integer* MemoryPointer::read_long(STATE) {
    return Integer::from(state, *(long*)pointer);
  }

  Integer* MemoryPointer::write_long_long(STATE, Integer* val) {
    *(long long*)pointer = val->to_long_long();
    return val;
  }

  Integer* MemoryPointer::read_long_long(STATE) {
    return Integer::from(state, *(long long*)pointer);
  }

  Float* MemoryPointer::write_float(STATE, Float* flt) {
    *(float*)pointer = (float)flt->val;
    return flt;
  }

  Float* MemoryPointer::read_float(STATE) {
    return Float::create(state, (double)(*(float*)pointer));
  }
  
  Float* MemoryPointer::write_double(STATE, Float* flt) {
    *(double*)pointer = flt->val;
    return flt;
  }
  
  Float* MemoryPointer::read_double(STATE) {
    return Float::create(state, *(double*)pointer);
  }

  MemoryPointer* MemoryPointer::read_pointer(STATE) {
    return MemoryPointer::create(state, *(void**)pointer);
  }

  Object* MemoryPointer::network_order(STATE, Fixnum* offset, Fixnum* intsize) {
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

  Object* MemoryPointer::get_at_offset(STATE, Fixnum* offset, Fixnum* type) {
    return get_field(state, offset->to_native(), type->to_native());
  }

  Object* MemoryPointer::get_field(STATE, int offset, int type) {
    Object* ret;
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
    case RBX_FFI_TYPE_OBJECT:
      ret = READ(Object*);
      break;
    case RBX_FFI_TYPE_PTR: {
      void *lptr = READ(void*);
      if(!lptr) {
        ret = Qnil;
      } else {
        ret = MemoryPointer::create(state, lptr);
      }
      break;
    }
    case RBX_FFI_TYPE_STRING: {
      char* result = READ(char*);
      if(result == NULL) {
        ret = Qnil;
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
        s = p = Qnil;
      } else {
        s = String::create(state, result);
        p = MemoryPointer::create(state, result);
      }

      Array* ary = Array::create(state, 2);
      ary->set(state, 0, s);
      ary->set(state, 1, p);
      ret = ary;
      break;
    }
    default:
    case RBX_FFI_TYPE_VOID:
      ret = Qnil;
      break;
    }

    return ret;
  }

  Object* MemoryPointer::set_at_offset(STATE, Fixnum* offset, Fixnum* type, Object* val) {
    set_field(state, offset->to_native(), type->to_native(), val);
    return val;
  }

  void MemoryPointer::set_field(STATE, int offset, int type, Object* val) {
    char* ptr = (char*)pointer;

    ptr += offset;

#define WRITE(type, val) *((type*)ptr) = (type)val

    switch(type) {
    case RBX_FFI_TYPE_CHAR:
      type_assert(state, val, FixnumType, "converting to char");
      WRITE(char, as<Fixnum>(val)->to_native());
      break;
    case RBX_FFI_TYPE_UCHAR:
      type_assert(state, val, FixnumType, "converting to unsigned char");
      WRITE(unsigned char, as<Fixnum>(val)->to_native());
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
      if(FIXNUM_P(val)) {
        WRITE(int, as<Fixnum>(val)->to_int());
      } else {
        type_assert(state, val, BignumType, "converting to int");
        WRITE(int, as<Bignum>(val)->to_int());
      }
      break;
    case RBX_FFI_TYPE_UINT:
      if(FIXNUM_P(val)) {
        WRITE(unsigned int, as<Fixnum>(val)->to_uint());
      } else {
        type_assert(state, val, BignumType, "converting to unsigned int");
        WRITE(unsigned int, as<Bignum>(val)->to_uint());
      }
      break;
    case RBX_FFI_TYPE_LONG:
      if(FIXNUM_P(val)) {
        WRITE(long, as<Fixnum>(val)->to_long());
      } else {
        type_assert(state, val, BignumType, "converting to long");
        WRITE(long, as<Bignum>(val)->to_long());
      }
      break;
    case RBX_FFI_TYPE_ULONG:
      if(FIXNUM_P(val)) {
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
      if(FIXNUM_P(val)) {
        WRITE(long long, as<Fixnum>(val)->to_long_long());
      } else {
        type_assert(state, val, BignumType, "converting to long long");
        WRITE(long long, as<Bignum>(val)->to_long_long());
      }
      break;
    case RBX_FFI_TYPE_ULONG_LONG:
      if(FIXNUM_P(val)) {
        WRITE(unsigned long long, as<Fixnum>(val)->to_ulong_long());
      } else {
        type_assert(state, val, BignumType, "converting to unsigned long long");
        WRITE(unsigned long long, as<Bignum>(val)->to_ulong_long());
      }
      break;
    case RBX_FFI_TYPE_OBJECT:
      WRITE(Object*, val);
      break;
    case RBX_FFI_TYPE_PTR:
      if(NIL_P(val)) {
        WRITE(void*, NULL);
      } else {
        MemoryPointer *mp = as<MemoryPointer>(val);
        type_assert(state, val, MemoryPointerType, "converting to pointer");
        WRITE(void*, mp->pointer);
      }
      break;
    case RBX_FFI_TYPE_STRING: {
      const char* result;
      if(NIL_P(val)) {
        result = NULL;
      } else {
        String* str = as<String>(val);
        /* TODO this is probably not correct. Saving away an 
         * internal pointer to the string means that when the string
         * moves, the data will point at the wrong place. Probably need to
         * copy the string data instead */
        result = str->c_str();
      }
      WRITE(const char*, result);
      break;
    }
    default:
      sassert(0);
    }
  }

  void MemoryPointer::Info::mark(Object* obj, ObjectMark& mark) {
    // @todo implement
  }
}

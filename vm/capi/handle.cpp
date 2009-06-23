#include "builtin/nativemethod.hpp"
#include "capi/handle.hpp"

#include "ruby.h"

namespace rubinius {
  namespace capi {
    void Handle::free_data() {
      switch(type_) {
      case cRArray:
        if(as_.rarray) {
          delete[] as_.rarray->dmwmb;
          delete as_.rarray;
        }

        as_.rarray = 0;
        break;
      case cRData:
        if(as_.rdata) delete as_.rdata;
        as_.rdata = 0;
      case cRString:
        if(as_.rstring) {
          delete[] as_.rstring->dmwmb;
          delete as_.rstring;
        }

        as_.rstring = 0;
      case cRFloat:
        if(as_.rfloat) delete as_.rfloat;
        as_.rfloat = 0;
      default:
        break;
      }

      type_ = cUnknown;
    }

    void Handle::debug_print() {
      std::cerr << std::endl << "Invalid handle usage detected!" << std::endl;
      std::cerr << "  handle:     " << this << std::endl;
      std::cerr << "  checksum:   0x" << std::hex << checksum_ << std::endl;
      std::cerr << "  references: " << references_ << std::endl;
      std::cerr << "  type:       " << type_ << std::endl;
      std::cerr << "  object:     " << object_ << std::endl;
    }

    Handle::~Handle() {
      invalidate();
    }
  }
}

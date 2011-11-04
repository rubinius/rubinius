#include "vm.hpp"
#include "vm/object_utils.hpp"

#include "capi/capi.hpp"
#include "capi/19/include/ruby.h"

#include <stdio.h>

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {
  }
}

extern "C" {
  FILE *  rb_io_stdio_file(rb_io_t *fptr) {
    return fptr->f;
  }
}

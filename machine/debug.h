#ifndef RBX_DEBUG_H
#define RBX_DEBUG_H

#include "config.h"

#include "memory/mark_stack.hpp"

#include <iostream>

using namespace rubinius;

namespace rubinius {
  using namespace memory;
}

extern "C" {
  void mark_stack_find(MarkStack& mark_stack, void* obj) {
#ifdef RBX_GC_STACK_CHECK
    mark_stack.find(obj);
#else
    std::cerr << "Configure with --gc-stack-check to enable finding an object on the mark stack" << std::endl;
#endif
  }
}

#endif

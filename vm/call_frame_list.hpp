#ifndef RBX_CALL_FRAME_LIST_HPP
#define RBX_CALL_FRAME_LIST_HPP

#include "util/pointer_list.hpp"

namespace rubinius {
  class CallFrame;

  typedef PointerList<CallFrame> CallFrameList;
}

#endif


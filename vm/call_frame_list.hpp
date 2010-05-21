#ifndef RBX_CALL_FRAME_LIST_HPP
#define RBX_CALL_FRAME_LIST_HPP

#include "util/pointer_list.hpp"
#include <list>

namespace rubinius {
  struct CallFrame;

  typedef PointerList<CallFrame> CallFrameList;
  typedef std::list<CallFrame**> CallFrameLocationList;
}

#endif


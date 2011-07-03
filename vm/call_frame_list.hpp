#ifndef RBX_CALL_FRAME_LIST_HPP
#define RBX_CALL_FRAME_LIST_HPP

#include <list>

namespace rubinius {
  struct CallFrame;

  typedef std::list<CallFrame**> CallFrameLocationList;
}

#endif


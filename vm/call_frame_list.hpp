#ifndef RBX_CALL_FRAME_LIST_HPP
#define RBX_CALL_FRAME_LIST_HPP

#include <list>
namespace rubinius {
  class CallFrame;
  typedef std::list<CallFrame*> CallFrameList;
}

#endif


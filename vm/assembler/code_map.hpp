#ifndef RBX_ASM_CODE_MAP
#define RBX_ASM_CODE_MAP

#include <map>

namespace rubinius {
  typedef std::map<rubinius::opcode, void*> CodeMap;
}

#endif

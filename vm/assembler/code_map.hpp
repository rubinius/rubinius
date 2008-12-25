#ifndef RBX_ASM_CODE_MAP
#define RBX_ASM_CODE_MAP

#include <map>
#include <string>

namespace rubinius {
  typedef std::map<int, void*> CodeMap;
  typedef std::map<void*, const char*> AddressComments;
}

#endif

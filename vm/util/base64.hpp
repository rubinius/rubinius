#include <stdint.h>
#include <string>

namespace base64 {
  std::string encode(uint8_t* in, size_t len);
  std::string decode(uint8_t* in, size_t len);
}

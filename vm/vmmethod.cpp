#include "objects.hpp"

namespace rubinius {
  static inline uint32_t read_int_from_be(uint8_t *str) {
    return (uint32_t)((str[0] << 24)
                    | (str[1] << 16)
                    | (str[2] << 8 )
                    |  str[3]      );
  }

  VMMethod::VMMethod(CompiledMethod* meth) {
    uint8_t *buf;

    buf = (uint8_t*)meth->iseq->bytes;

    size_t total = meth->iseq->field_count;

    opcodes = new opcode[total];

    for(size_t index = 0; index < total; index++, buf += 4) {
      opcodes[index] = (opcode)read_int_from_be(buf);
    }
  }

  VMMethod::~VMMethod() {
    delete[] opcodes;
  }
}

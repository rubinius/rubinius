#include "objects.hpp"

namespace rubinius {
  VMMethod::VMMethod(CompiledMethod* meth) {
    size_t total = meth->iseq->instructions->field_count;

    opcodes = new opcode[total];

    for(size_t index = 0; index < total; index++) {
      OBJECT val = meth->iseq->instructions->at(index);
      if(val->nil_p()) {
        opcodes[index] = 0;
      } else {
        opcodes[index] = as<Fixnum>(val)->n2i();
      }
    }
  }

  VMMethod::VMMethod(size_t fields) {
    opcodes = new opcode[fields];
  }

  VMMethod::~VMMethod() {
    delete[] opcodes;
  }
}

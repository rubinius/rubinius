#ifndef RBX_BUILTIN_ISEQ_HPP
#define RBX_BUILTIN_ISEQ_HPP

#include "objects.hpp"

namespace rubinius {
  class ISeq : public BuiltinType {
  public:
    const static size_t fields = 2;
    const static object_type type = ISeqType;

    OBJECT instance_variables;
    Tuple* instructions;

    void post_marshal(STATE);
    static ISeq* create(STATE, size_t instructions);

#include "gen/iseq_instruction_names.hpp"
  };
}

#endif

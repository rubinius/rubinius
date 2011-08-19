#ifndef RBX_LLVM_TYPES_HPP
#define RBX_LLVM_TYPES_HPP

namespace llvm {
  class Instruction;
  class Value;
}

namespace rubinius {
  class LLVMState;

  namespace type {
    enum Kind {
      eUnknown = 0,
      eTrue = 1,
      eFalse = 2,
      eNil = 3,
      eStaticFixnum = 4,
      eInstance = 5
    };

    class KnownType {
      Kind kind_;
      int value_;

    public:
      KnownType()
        : kind_(eUnknown)
        , value_(0)
      {}

      KnownType(Kind kind, int value=0)
        : kind_(kind)
        , value_(value)
      {}

      static KnownType unknown() {
        return KnownType(eUnknown);
      }

      static KnownType instance(int class_id) {
        return KnownType(eInstance, class_id);
      }

      static KnownType nil() {
        return KnownType(eNil);
      }

      static KnownType true_() {
        return KnownType(eTrue);
      }

      static KnownType false_() {
        return KnownType(eFalse);
      }

      static KnownType fixnum(int val) {
        return KnownType(eStaticFixnum, val);
      }

      bool known_p() {
        return kind_ != eUnknown;
      }

      Kind kind() {
        return kind_;
      }

      int value() {
        return value_;
      }

      bool instance_p() {
        return kind_ == eInstance;
      }

      int class_id() {
        if(kind_ != eInstance) return -1;
        return value_;
      }

      bool fixnum_p() {
        return kind_ == eStaticFixnum;
      }

      bool static_fixnum_p() {
        return kind_ == eStaticFixnum;
      }

      const char* describe();

      void associate(LLVMState* ls, llvm::Instruction* I);
      void associate(LLVMState* ls, llvm::Value* V);

      static KnownType extract(LLVMState* ls, llvm::Instruction* I);
      static KnownType extract(LLVMState* ls, llvm::Value* I);

      static bool has_hint(LLVMState* ls, llvm::Value* V);
    };
  }
}

#endif

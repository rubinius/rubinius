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
      eTrue,
      eFalse,
      eNil,
      eFixnum,
      eStaticFixnum,
      eInstance,
      eSingletonInstance,
      eSymbol,
      eType,
      eClassObject
    };

    enum Source {
      eUnknownSource = 0,
      eLocal = 1
    };

    class KnownType {
      Kind kind_;
      int value_;

      Source source_;
      int source_id_;

    public:
      KnownType()
        : kind_(eUnknown)
        , value_(0)
        , source_(eUnknownSource)
        , source_id_(0)
      {}

      KnownType(Kind kind, int value=0, Source s=eUnknownSource, int sid=0)
        : kind_(kind)
        , value_(value)
        , source_(s)
        , source_id_(sid)
      {}

      static KnownType unknown() {
        return KnownType(eUnknown);
      }

      static KnownType instance(int class_id) {
        return KnownType(eInstance, class_id);
      }

      static KnownType singleton_instance(int class_id) {
        return KnownType(eSingletonInstance, class_id);
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

      static KnownType fixnum() {
        return KnownType(eFixnum);
      }

      static KnownType symbol() {
        return KnownType(eSymbol);
      }

      static KnownType type() {
        return KnownType(eType);
      }

      static KnownType class_object(int class_id) {
        return KnownType(eClassObject, class_id);
      }

      bool known_p() {
        return kind_ != eUnknown;
      }

      bool unknown_p() {
        return kind_ == eUnknown;
      }

      Kind kind() {
        return kind_;
      }

      int value() {
        return value_;
      }

      bool instance_p() {
        return kind_ == eInstance || kind_ == eSingletonInstance;
      }

      bool singleton_instance_p() {
        return kind_ == eSingletonInstance;
      }

      bool class_p() {
        return kind_ == eClassObject;
      }

      int class_id() {
        switch(kind_) {
        case eInstance:
        case eSingletonInstance:
        case eClassObject:
          return value_;
        default:
          return -1;
        }
      }

      bool fixnum_p() {
        return kind_ == eStaticFixnum || kind_ == eFixnum;
      }

      bool static_fixnum_p() {
        return kind_ == eStaticFixnum;
      }

      bool symbol_p() {
        return kind_ == eSymbol;
      }

      bool type_p() {
        return kind_ == eType;
      }

      void set_local_source(int id) {
        source_ = eLocal;
        source_id_ = id;
      }

      Source source() {
        return source_;
      }

      bool local_source_p() {
        return source_ == eLocal;
      }

      int local_id() {
        if(source_ != eLocal) return -1;
        return source_id_;
      }

      int source_id() {
        return source_id_;
      }

      const char* describe();

      void associate(LLVMState* ls, llvm::Instruction* I);
      void associate(LLVMState* ls, llvm::Value* V);

      static KnownType extract(LLVMState* ls, llvm::Instruction* I);
      static KnownType extract(LLVMState* ls, llvm::Value* I);

      static bool has_hint(LLVMState* ls, llvm::Value* V);

      void inherit_source(LLVMState* ls, llvm::Value* V);
      void inherit_source(type::KnownType kt);
    };
  }
}

#endif

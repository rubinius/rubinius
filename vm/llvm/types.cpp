#include "llvm/jit.hpp"
#include "llvm/jit_operations.hpp"
#include "llvm/types.hpp"

namespace rubinius {
namespace type {
  void KnownType::associate(LLVMState* ls, llvm::Instruction* I) {
    if(kind_ == eUnknown) return;

    llvm::Value *impMD[] = {
      llvm::ConstantInt::get(ls->Int32Ty, kind_),
      llvm::ConstantInt::get(ls->Int32Ty, value_)
    };

    llvm::MDNode *node = llvm::MDNode::get(ls->ctx(), impMD, 2);

    I->setMetadata(ls->metadata_id(), node);
  }

  void KnownType::associate(LLVMState* ls, llvm::Value* V) {
    if(Instruction* I = dyn_cast<Instruction>(V)) {
      associate(ls, I);
    }
  }


  KnownType KnownType::extract(LLVMState* ls, llvm::Value* V) {
    if(Instruction* I = dyn_cast<Instruction>(V)) {
      return extract(ls, I);
    }

    return KnownType::unknown();
  }

  KnownType KnownType::extract(LLVMState* ls, llvm::Instruction* I) {
    if(llvm::MDNode* md = I->getMetadata(ls->metadata_id())) {
      if(md->getNumOperands() == 2) {
        ConstantInt* kind = dyn_cast<ConstantInt>(md->getOperand(0));
        ConstantInt* value = dyn_cast<ConstantInt>(md->getOperand(1));

        if(kind && value) {
          return KnownType((Kind)kind->getValue().getSExtValue(),
                           value->getValue().getSExtValue());
        }
      }
    }

    return KnownType::unknown();
  }

  bool KnownType::has_hint(LLVMState* ls, llvm::Value* V) {
    if(Instruction* I = dyn_cast<Instruction>(V)) {
      if(I->getMetadata(ls->metadata_id())) {
        return true;
      }
    }

    return false;
  }

  const char* KnownType::describe() {
    switch(kind_) {
    case eUnknown:
      return "<type: unknown>";
    case eTrue:
      return "<type: true>";
    case eFalse:
      return "<type: false>";
    case eNil:
      return "<type: nil>";
    case eStaticFixnum:
      return "<type: static fixnum>";
    case eInstance:
      return "<type: instance>";
    }

    return "Confused type info!";
  }
}
}

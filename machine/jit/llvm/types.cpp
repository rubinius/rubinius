#include "jit/llvm/operations.hpp"
#include "jit/llvm/types.hpp"

#include "builtin/constant_cache.hpp"

namespace rubinius {
namespace type {
  void KnownType::associate(Context* ctx, llvm::Instruction* I) {
    if(kind_ == eUnknown && source_ == eUnknownSource) return;

    llvm::MDNode* node;
    if(source_ != eUnknownSource) {
      llvm::Metadata* impMD[] = {
        llvm::ConstantAsMetadata::get(ctx->cint(kind_)),
        llvm::ConstantAsMetadata::get(ctx->clong(value_)),
        llvm::ConstantAsMetadata::get(ctx->cint(source_)),
        llvm::ConstantAsMetadata::get(ctx->cint(source_id_))
      };
      llvm::ArrayRef<llvm::Metadata*> md(impMD, 4);

      node = llvm::MDNode::get(ctx->llvm_context(), md);
    } else {
      llvm::Metadata* impMD[] = {
        llvm::ConstantAsMetadata::get(ctx->cint(kind_)),
        llvm::ConstantAsMetadata::get(ctx->clong(value_))
      };
      llvm::ArrayRef<llvm::Metadata*> md(impMD, 2);

      node = llvm::MDNode::get(ctx->llvm_context(), md);
    }

    I->setMetadata(ctx->metadata_id(), node);
  }

  void KnownType::associate(Context* ctx, llvm::Value* V) {
    if(Instruction* I = dyn_cast<Instruction>(V)) {
      associate(ctx, I);
    }
  }

  KnownType KnownType::extract(Context* ctx, llvm::Value* V) {
    if(Instruction* I = dyn_cast<Instruction>(V)) {
      return extract(ctx, I);
    }

    return KnownType::unknown();
  }

  KnownType KnownType::extract(Context* ctx, llvm::Instruction* I) {
    if(llvm::MDNode* md = I->getMetadata(ctx->metadata_id())) {
      if(md->getNumOperands() == 2) {
        ConstantInt* kind = dyn_cast<ConstantInt>(
            llvm::MetadataAsValue::get(ctx->llvm_context(), md->getOperand(0)));
        ConstantInt* value = dyn_cast<ConstantInt>(
            llvm::MetadataAsValue::get(ctx->llvm_context(), md->getOperand(1)));

        if(kind && value) {
          return KnownType((Kind)kind->getValue().getSExtValue(),
                           value->getValue().getSExtValue());
        }
      } else if(md->getNumOperands() == 4) {
        ConstantInt* kind = dyn_cast<ConstantInt>(
            llvm::MetadataAsValue::get(ctx->llvm_context(), md->getOperand(0)));
        ConstantInt* value = dyn_cast<ConstantInt>(
            llvm::MetadataAsValue::get(ctx->llvm_context(), md->getOperand(1)));
        ConstantInt* source = dyn_cast<ConstantInt>(
            llvm::MetadataAsValue::get(ctx->llvm_context(), md->getOperand(2)));
        ConstantInt* source_id = dyn_cast<ConstantInt>(
            llvm::MetadataAsValue::get(ctx->llvm_context(), md->getOperand(3)));

        if(kind && value && source && source_id) {
          return KnownType((Kind)kind->getValue().getSExtValue(),
                           value->getValue().getSExtValue(),
                           (Source)source->getValue().getSExtValue(),
                           source_id->getValue().getSExtValue());
        }
      }
    }

    return KnownType::unknown();
  }

  bool KnownType::has_hint(Context* ctx, llvm::Value* V) {
    if(Instruction* I = dyn_cast<Instruction>(V)) {
      if(I->getMetadata(ctx->metadata_id())) {
        return true;
      }
    }

    return false;
  }

  void KnownType::inherit_source(Context* ctx, llvm::Value* V) {
    inherit_source(extract(ctx, V));
  }

  void KnownType::inherit_source(KnownType kt) {
    if(kt.source() == eUnknownSource) return;

    source_ = kt.source();
    source_id_ = kt.source_id();
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
    case eFixnum:
      return "<type: fixnum>";
    case eStaticFixnum:
      return "<type: static fixnum>";
    case eInstance:
      return "<type: instance>";
    case eSingletonInstance:
      return "<type: singleton instance>";
    case eSymbol:
      return "<type: symbol>";
    case eStaticSymbol:
      return "<type: static symbol>";
    case eType:
      return "<obj: Rubinius::Type>";
    case eClassObject:
      return "<obj: class>";
    case eConstantCache:
      return "<obj: global cache entry>";
    }

    return "Confused type info!";
  }
}
}

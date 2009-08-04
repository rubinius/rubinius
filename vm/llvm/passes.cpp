#ifdef ENABLE_LLVM

#include "llvm/passes.hpp"

#include <llvm/Attributes.h>
#include <llvm/BasicBlock.h>
#include <llvm/Function.h>
#include <llvm/Instructions.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/Support/CallSite.h>
#include <llvm/ADT/APInt.h>
#include <llvm/Constants.h>
#include <llvm/Module.h>
#include <llvm/Intrinsics.h>
#include <llvm/Analysis/SparsePropagation.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <iostream>

namespace {

  using namespace llvm;

  class OverflowConstantFolder : public FunctionPass {
  public:
    static char ID;
    OverflowConstantFolder()
      : FunctionPass(&ID)
    {}

    bool try_to_fold_addition(CallInst* call) {
      CallSite cs(call);

      Value* lhs = cs.getArgument(0);
      Value* rhs = cs.getArgument(1);

      bool changed = false;

      if(ConstantInt* lc = dyn_cast<ConstantInt>(lhs)) {
        if(ConstantInt* rc = dyn_cast<ConstantInt>(rhs)) {
          const APInt& lval = lc->getValue();
          const APInt& rval = rc->getValue();
          APInt zero(lval.getBitWidth(), 0, true);
          APInt res = lval + rval;

          ConstantInt* overflow = ConstantInt::getFalse();
          if(lval.sgt(zero) && res.slt(rval)) {
            overflow = ConstantInt::getTrue();
          } else if(rval.sgt(zero) && res.slt(lval)) {
            overflow = ConstantInt::getTrue();
          }

          // Now, update the extracts
          for(Value::use_iterator i = call->use_begin();
              i != call->use_end();
              i++) {
            if(ExtractValueInst* extract = dyn_cast<ExtractValueInst>(*i)) {
              Value* result = 0;

              ExtractValueInst::idx_iterator idx = extract->idx_begin();
              if(*idx == 0) {
                result = ConstantInt::get(res);
              } else if(*idx == 1) {
                result = overflow;
              } else {
                std::cout << "unknown index on sadd.overflow extract\n";
              }

              if(result) {
                extract->replaceAllUsesWith(result);
                result->takeName(extract);
                extract->eraseFromParent();
              }
            }
          }

          changed = true;
        }
      }

      return changed;
    }

    virtual bool runOnFunction(Function& f) {
      bool changed = false;

      std::vector<CallInst*> to_remove;

      for(Function::iterator bb = f.begin(), e = f.end();
          bb != e;
          bb++) {
        for(BasicBlock::iterator inst = bb->begin();
            inst != bb->end();
            inst++) {
          CallInst *call = dyn_cast<CallInst>(inst);
          if(call == NULL) continue;

          // This may miss inlining indirect calls that become
          // direct after inlining something else.
          Function *called_function = call->getCalledFunction();
          if(!called_function) continue;

          if(called_function->getIntrinsicID() == Intrinsic::sadd_with_overflow) {
            if(try_to_fold_addition(call)) {
              if(call->use_empty()) {
                to_remove.push_back(call);
                changed = true;
              }
            }
          }
        }
      }

      for(std::vector<CallInst*>::iterator i = to_remove.begin();
          i != to_remove.end();
          i++) {
        (*i)->eraseFromParent();
      }

      return changed;
    }
  };

  // The address of this variable identifies the pass.  See
  // http://llvm.org/docs/WritingAnLLVMPass.html#basiccode.
  char OverflowConstantFolder::ID = 0;

  class RubiniusAliasAnalysis : public FunctionPass, public AliasAnalysis {
    const Type* class_type_;
    const Type* object_type_;
    const Type* args_type_;

  public:
    static char ID;
    RubiniusAliasAnalysis()
      : FunctionPass(&ID)
      , class_type_(0)
    {}

    virtual void getAnalysisUsage(llvm::AnalysisUsage& usage) const {
      AliasAnalysis::getAnalysisUsage(usage);
      usage.setPreservesAll();
    }

    virtual bool doInitialization(Module& mod) {
      class_type_ = PointerType::getUnqual(
          mod.getTypeByName("struct.rubinius::Class"));

      object_type_ = PointerType::getUnqual(
          mod.getTypeByName("struct.rubinius::Object"));

      args_type_ = PointerType::getUnqual(
          mod.getTypeByName("struct.rubinius::Arguments"));

      return false;
    }

    virtual bool runOnFunction(Function& func) {
      AliasAnalysis::InitializeAliasAnalysis(this);
      return false;
    }

    virtual
    AliasAnalysis::AliasResult alias(const Value *V1, unsigned V1Size,
                                     const Value *V2, unsigned V2Size)
    {
      // Indicate that tagged fixnums can't alias anything.
      if(const IntToPtrInst* ip = dyn_cast<IntToPtrInst>(V1)) {
        if(ip->getType() == object_type_) {
          if(const ConstantInt* ci = dyn_cast<ConstantInt>(ip->getOperand(0))) {
            const APInt& cv = ci->getValue();
            APInt one(cv.getBitWidth(), 1);

            if(cv.And(one) == one) return NoAlias;
          }
        }
      }

      return AliasAnalysis::alias(V1, V1Size, V2, V2Size);
    }


    virtual bool pointsToConstantMemory(const Value* val) {
      if(const GetElementPtrInst* gep = dyn_cast<GetElementPtrInst>(val)) {
        if(gep->getPointerOperand()->getType() == class_type_) {
          // Indicate that the class_id field is constant
          if(gep->getNumIndices() == 2
              && gep->getOperand(1) == ConstantInt::get(Type::Int32Ty, 0)
              && gep->getOperand(2) == ConstantInt::get(Type::Int32Ty, 3)) {
            return true;
          }

        // Indicate that pointers to classes are constant
        } else if(gep->getType() == PointerType::getUnqual(class_type_)) {
          return true;

        // Indicate that all fields within Arguments are constant
        } else if(gep->getPointerOperand()->getType() == args_type_) {
          return true;
        }
      }
      return AliasAnalysis::pointsToConstantMemory(val);
    }
  };

  char RubiniusAliasAnalysis::ID = 0;

  static llvm::RegisterPass<RubiniusAliasAnalysis>
  U("rbx-aa", "Rubinius-specific Alias Analysis", false, true);

  static llvm::RegisterAnalysisGroup<AliasAnalysis> V(U);

  /*
  class TypeGuardRemoval : public FunctionPass {
  public:
    static char ID;
    TypeGuardRemoval()
      : FunctionPass(&ID)
    {}

    class Driver : public AbstractLatticeFunction {
      virtual LatticeVal ComputeInstructionState(
                           Instructions &I, SparseSolver& SS)
      {
        if(ICmpInst* ic = dyn_cast<ICmpInst>(&I)) {
          if(LoadInst* lc = dyn_cast<LoadInst>(ic->getOperand(0))) {
            if(GetElementPtrInst* gepc =
                dyn_cast<GetElementPtrInst>(lc->getPointerOperand())) {
          }
        }
      }
    };

    virtual bool runOnFunction(Function& f) { return true; }

  };

  char TypeGuardRemoval::ID = 0;
  */

}  // anonymous namespace

namespace rubinius {
  llvm::FunctionPass* create_overflow_folding_pass() {
    return new OverflowConstantFolder();
  }

  llvm::FunctionPass* create_rubinius_alias_analysis() {
    return new RubiniusAliasAnalysis();
  }
}

#endif

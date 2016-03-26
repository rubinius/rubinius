#include "jit/llvm/passes.hpp"
#include "config.h"

#include <llvm/IR/CallSite.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/ADT/APInt.h>
#include <llvm/Analysis/SparsePropagation.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>

namespace {

  using namespace llvm;

  class GuardEliminator : public FunctionPass {
    Type* float_type_;

  public:
    static char ID;
    GuardEliminator()
      : FunctionPass(ID)
      , float_type_(0)
    {}

    virtual bool doInitialization(Module& mod) {
      float_type_ = PointerType::getUnqual(
          mod.getTypeByName("struct.rubinius::Float"));

      return false;
    }

    virtual bool runOnFunction(Function& f) {
      bool changed = false;

      for(Function::iterator bb = f.begin(), e = f.end();
          bb != e;
          ++bb) {
        for(BasicBlock::iterator inst = bb->begin();
            inst != bb->end();
            ++inst) {
          ICmpInst* icmp = dyn_cast<ICmpInst>(inst);
          if(icmp == NULL) continue;

          if(BinaryOperator* bin = dyn_cast<BinaryOperator>(icmp->getOperand(0))) {
            if(bin->getOpcode() != Instruction::And) continue;
            if(PtrToIntInst* p2i = dyn_cast<PtrToIntInst>(bin->getOperand(0))) {
              if(p2i->getOperand(0)->getType() == float_type_) {
                icmp->replaceAllUsesWith(ConstantInt::getTrue(f.getContext()));
                changed = true;
                continue;
              }
            }
          } else if(LoadInst* load = dyn_cast<LoadInst>(icmp->getOperand(0))) {
            if(GetElementPtrInst* gep1 =
                dyn_cast<GetElementPtrInst>(load->getOperand(0))) {
              if(LoadInst* load2 = dyn_cast<LoadInst>(gep1->getOperand(0))) {
                if(GetElementPtrInst* gep2 =
                    dyn_cast<GetElementPtrInst>(load2->getOperand(0))) {
                  if(gep2->getOperand(0)->getType() == float_type_) {
                    icmp->replaceAllUsesWith(ConstantInt::getTrue(f.getContext()));
                    changed = true;
                    continue;
                  }
                }
              }
            }
          }
        }
      }

      return changed;
    }
  };

  char GuardEliminator::ID = 0;

  class AllocationEliminator : public FunctionPass {
    Function* float_alloc_;

  public:
    static char ID;
    AllocationEliminator()
      : FunctionPass(ID)
      , float_alloc_(0)
    {}

    virtual bool doInitialization(Module& mod) {
      float_alloc_ = mod.getFunction("rbx_float_allocate");

      return false;
    }

    bool gep_used(GetElementPtrInst* gep) {
      for(Value::use_iterator u = gep->use_begin();
          u != gep->use_end();
          u++) {
        llvm::outs() << "gep user: " << *(*u) << "\n";
        if(!dyn_cast<StoreInst>(*u)) return true;
      }

      return false;
    }

    void erase_gep_users(GetElementPtrInst* gep) {
      for(Value::use_iterator u = gep->use_begin();
          u != gep->use_end(); /* nothing */)
      {
        if(Instruction* inst = dyn_cast<Instruction>(*u)) {
          u++;
          inst->eraseFromParent();
        } else {
          u++;
          assert(0);
        }
      }
    }

    virtual bool runOnFunction(Function& f) {
      std::vector<CallInst*> to_remove;

      for(Function::iterator bb = f.begin(), e = f.end();
          bb != e;
          ++bb) {
        for(BasicBlock::iterator inst = bb->begin();
            inst != bb->end();
            ++inst) {
          CallInst* call = dyn_cast<CallInst>(inst);
          if(!call) continue;

          Function* func = call->getCalledFunction();
          if(func && func->getName() == "rbx_float_allocate") {
            llvm::outs() << "here2!\n";

            bool use = false;
            for(Value::use_iterator u = call->use_begin();
                u != call->use_end();
                u++) {

              llvm::outs() << "float user: " << *(*u) << "\n";

              if(GetElementPtrInst* gep = dyn_cast<GetElementPtrInst>(*u)) {
                if(gep_used(gep)) {
                  use = true;
                  break;
                }
              } else {
                use = true;
                break;
              }
            }

            if(!use) {
              to_remove.push_back(call);
            }
          }
        }
      }

      for(std::vector<CallInst*>::iterator i = to_remove.begin();
          i != to_remove.end();
          ++i) {
        CallInst* call = *i;
        for(Value::use_iterator u = call->use_begin();
            u != call->use_end(); /* nothing */)
        {
          if(GetElementPtrInst* gep = dyn_cast<GetElementPtrInst>(*u)) {
            llvm::outs() << "erasing: " << *gep << "\n";
            erase_gep_users(gep);
            u++;
            gep->eraseFromParent();
          } else {
            assert(0);
          }
        }

        call->eraseFromParent();
      }

      return !to_remove.empty();
    }
  };

  char AllocationEliminator::ID = 0;

  class OverflowConstantFolder : public FunctionPass {
  public:
    static char ID;
    OverflowConstantFolder()
      : FunctionPass(ID)
    {}

    bool try_to_fold_addition(LLVMContext& ctx, CallInst* call) {
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

          ConstantInt* overflow = ConstantInt::getFalse(ctx);
          if(lval.sgt(zero) && res.slt(rval)) {
            overflow = ConstantInt::getTrue(ctx);
          } else if(rval.sgt(zero) && res.slt(lval)) {
            overflow = ConstantInt::getTrue(ctx);
          }

          // Now, update the extracts
          for(Value::use_iterator i = call->use_begin();
              i != call->use_end(); /* nothing */)
          {
            if(ExtractValueInst* extract = dyn_cast<ExtractValueInst>(*i)) {
              Value* result = 0;

              ExtractValueInst::idx_iterator idx = extract->idx_begin();
              if(*idx == 0) {
                result = ConstantInt::get(ctx, res);
              } else if(*idx == 1) {
                result = overflow;
              } else {
                llvm::outs() << "unknown index on sadd.overflow extract\n";
              }

              if(result) {
                extract->replaceAllUsesWith(result);
                result->takeName(extract);
                // I uses the element itself to find the next, so we have
                // to increment before we remove it.
                i++;
                extract->eraseFromParent();
                continue;
              }
            }
            i++;
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
          ++bb) {
        for(BasicBlock::iterator inst = bb->begin();
            inst != bb->end();
            ++inst) {
          CallInst *call = dyn_cast<CallInst>(inst);
          if(call == NULL) continue;

          // This may miss inlining indirect calls that become
          // direct after inlining something else.
          Function *called_function = call->getCalledFunction();
          if(!called_function) continue;

          if(called_function->getIntrinsicID() == Intrinsic::sadd_with_overflow) {
            if(try_to_fold_addition(f.getContext(), call)) {
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
          ++i) {
        (*i)->eraseFromParent();
      }

      return changed;
    }
  };

  // The address of this variable identifies the pass.  See
  // http://llvm.org/docs/WritingAnLLVMPass.html#basiccode.
  char OverflowConstantFolder::ID = 0;


  class RubiniusAliasAnalysis : public FunctionPass, public AliasAnalysis {
    Type* class_type_;
    Type* object_type_;
    Type* args_type_;
    Type* float_type_;

  public:
    static char ID;
    RubiniusAliasAnalysis()
      : FunctionPass(ID)
      , class_type_(0)
      , object_type_(0)
      , args_type_(0)
      , float_type_(0)
    {}

    virtual void getAnalysisUsage(llvm::AnalysisUsage& usage) const {
      usage.setPreservesAll();
      AliasAnalysis::getAnalysisUsage(usage);
    }

    virtual bool doInitialization(Module& mod) {
      class_type_ = PointerType::getUnqual(
          mod.getTypeByName("struct.rubinius::Class"));

      object_type_ = PointerType::getUnqual(
          mod.getTypeByName("struct.rubinius::Object"));

      args_type_ = PointerType::getUnqual(
          mod.getTypeByName("struct.rubinius::Arguments"));

      float_type_ = PointerType::getUnqual(
          mod.getTypeByName("struct.rubinius::Float"));

      return false;
    }

    virtual bool runOnFunction(Function& func) {
      InitializeAliasAnalysis(this);
      return false;
    }

    virtual void* getAdjustedAnalysisPointer(const void* PI) {
      if(PI == &AliasAnalysis::ID) {
        return (AliasAnalysis*)this;
      }

      return this;
    }

    virtual
    AliasAnalysis::AliasResult alias(const Location &LocA, const Location &LocB)
    {
      // Indicate that tagged fixnums can't alias anything.
      if(const IntToPtrInst* ip = dyn_cast<IntToPtrInst>(LocA.Ptr)) {
        if(ip->getType() == object_type_) {
          if(const ConstantInt* ci = dyn_cast<ConstantInt>(ip->getOperand(0))) {
            const APInt& cv = ci->getValue();
            APInt one(cv.getBitWidth(), 1);

            if(cv.And(one) == one) return NoAlias;
          }
        }
      }

      return AliasAnalysis::alias(LocA, LocB);
    }


    virtual
    AliasAnalysis::ModRefResult getModRefInfo(ImmutableCallSite cs, const Location &Loc) {
      if(const Function* func = cs.getCalledFunction()) {
        if(func->getName() == "rbx_float_allocate") {
          return NoModRef;
        }
      }

      return AliasAnalysis::getModRefInfo(cs, Loc);
    }

    // This method only exists to appease -Woverloaded-virtual. It's dumb
    // that it won't allow us to overload only one of the signatures for
    // getModRefInfo.
    virtual
    AliasAnalysis::ModRefResult getModRefInfo(ImmutableCallSite CS1,
                                              ImmutableCallSite CS2) {

      return AliasAnalysis::getModRefInfo(CS1, CS2);
    }

    virtual bool pointsToConstantMemory(const Location &Loc, bool OrLocal = false) {
      LLVMContext& ctx = Loc.Ptr->getContext();

      if(const GetElementPtrInst* gep = dyn_cast<GetElementPtrInst>(Loc.Ptr)) {
        if(gep->getPointerOperand()->getType() == class_type_) {
          // Indicate that the class_id field is constant
          if(gep->getNumIndices() == 2
              && gep->getOperand(1) == ConstantInt::get(Type::getInt32Ty(ctx), 0)
              && gep->getOperand(2) == ConstantInt::get(Type::getInt32Ty(ctx), 3)) {
            return true;
          }

        // Indicate that pointers to classes are constant
        } else if(gep->getType() == PointerType::getUnqual(class_type_)) {
          return true;

        // Indicate that all fields within Arguments are constant
        } else if(gep->getPointerOperand()->getType() == args_type_) {
          return true;
        } else if(gep->getPointerOperand()->getType() == float_type_) {
          return true;
        }
      } else if(const BitCastInst* bc = dyn_cast<BitCastInst>(Loc.Ptr)) {
        if(bc->getType() == PointerType::getUnqual(Type::getDoubleTy(ctx))) {
          return true;
        }
      }
      return AliasAnalysis::pointsToConstantMemory(Loc, OrLocal);
    }
  };

  char RubiniusAliasAnalysis::ID = 0;
  static RegisterPass<RubiniusAliasAnalysis> RubiniusAliasAnalysis_info("rbx-aa", "Rubinius-specific Alias Analysis", false, true);
  static RegisterAnalysisGroup<AliasAnalysis, false> RubiniusAliasAnalysis_ag(RubiniusAliasAnalysis_info);

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

  llvm::FunctionPass* create_guard_eliminator_pass() {
    return new GuardEliminator();
  }

  llvm::FunctionPass* create_allocation_eliminator_pass() {
    return new AllocationEliminator();
  }
}

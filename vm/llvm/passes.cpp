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

#include <iostream>

namespace {

  using namespace llvm;

  namespace Attribute = llvm::Attribute;
  using llvm::BasicBlock;
  using llvm::CallInst;
  using llvm::Function;
  using llvm::FunctionPass;
  using llvm::InvokeInst;
  using llvm::dyn_cast;
  using llvm::isa;

  class OverflowConstantFolder : public FunctionPass {
    Function* sadd_;

  public:
    static char ID;
    OverflowConstantFolder()
      : FunctionPass(&ID)
      , sadd_(0)
    {}

    bool try_to_fold_addition(CallInst* call) {
      CallSite cs(call);

      Value* lhs = cs.getArgument(0);
      Value* rhs = cs.getArgument(1);

      bool changed = false;

      if(ConstantInt* lc = dyn_cast<ConstantInt>(lhs)) {
        if(ConstantInt* rc = dyn_cast<ConstantInt>(rhs)) {
          APInt zero(31, 0, true);
          const APInt& lval = lc->getValue();
          const APInt& rval = rc->getValue();
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

    virtual bool doInitialization(Module& m) {
      std::vector<const Type*> types;
      types.push_back(IntegerType::get(31));
      types.push_back(IntegerType::get(31));

      std::vector<const Type*> struct_types;
      struct_types.push_back(IntegerType::get(31));
      struct_types.push_back(Type::Int1Ty);

      StructType* st = StructType::get(struct_types);

      FunctionType* ft = FunctionType::get(st, types, false);
      sadd_ = cast<Function>(
          m.getOrInsertFunction("llvm.sadd.with.overflow.i31", ft));

      return true;
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
          if(called_function == sadd_) {
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

}  // anonymous namespace

namespace rubinius {
  llvm::FunctionPass* create_overflow_folding_pass() {
    return new OverflowConstantFolder();
  }
}

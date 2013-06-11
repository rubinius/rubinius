#ifndef VM_LLVM_METHOD_INFO_HPP
#define VM_LLVM_METHOD_INFO_HPP

#include "llvm/state.hpp"

#include "llvm/inline_block.hpp"

#include "llvm/jit_context.hpp"

namespace rubinius {
  class JITStackArgs;
  class InlinePolicy;

  namespace jit {
    class RuntimeData;
  }

  class JITMethodInfo {
    Context* context_;
    llvm::BasicBlock* entry_;
    llvm::Value* call_frame_;
    llvm::Value* stack_;
    llvm::Value* args_;
    llvm::Value* variables_;
    llvm::Value* previous_;
    llvm::Value* profiling_entry_;

    JITMethodInfo* parent_info_;
    JITMethodInfo* creator_info_;

    bool use_full_scope_;

    JITInlineBlock* inline_block_;
    JITInlineBlock* block_info_;

    TypedRoot<CompiledCode*> method_;

    llvm::BasicBlock* return_pad_;
    llvm::PHINode* return_phi_;

    TypedRoot<Class*> self_class_;
    LocalMap local_info_;

  public:
    MachineCode* machine_code;
    bool is_block;
    llvm::BasicBlock* inline_return;
    llvm::Value* return_value;
    InlinePolicy* inline_policy;
    llvm::BasicBlock* fin_block;
    int called_args;
    int hits;
    JITStackArgs* stack_args;

    JITMethodInfo* root;
    type::KnownType self_type;

  public:
    JITMethodInfo(Context* ctx, CompiledCode* code, MachineCode* mcode,
                  JITMethodInfo* parent = 0);

    Context* context() {
      return context_;
    }

    llvm::Function* function() {
      return context_->function();
    }

    void set_state(llvm::Value* state) {
      context_->set_state(state);
    }

    llvm::Value* state() {
      return context_->state();
    }

    void set_args(llvm::Value* args) {
      args_ = args;
    }

    llvm::Value* args() {
      return args_;
    }

    void set_previous(llvm::Value* prev) {
      previous_ = prev;
    }

    llvm::Value* previous() {
      return previous_;
    }

    void set_profiling_entry(llvm::Value* val) {
      profiling_entry_ = val;
    }

    llvm::Value* profiling_entry() {
      return profiling_entry_;
    }

    void set_entry(llvm::BasicBlock* entry) {
      entry_ = entry;
    }

    llvm::BasicBlock* entry() {
      return entry_;
    }

    void set_call_frame(llvm::Value* val) {
      call_frame_ = val;
    }

    llvm::Value* call_frame() {
      return call_frame_;
    }

    void set_stack(llvm::Value* val) {
      stack_ = val;
    }

    llvm::Value* stack() {
      return stack_;
    }

    void set_variables(llvm::Value* vars) {
      variables_ = vars;
    }

    llvm::Value* variables() {
      return variables_;
    }

    CompiledCode* method() {
      return method_.get();
    }

    llvm::BasicBlock* return_pad() {
      return return_pad_;
    }

    llvm::PHINode* return_phi() {
      return return_phi_;
    }

    void add_return_value(llvm::Value* val, llvm::BasicBlock* block) {
      return_phi_->addIncoming(val, block);
    }

    void set_parent_info(JITMethodInfo& info) {
      parent_info_ = &info;

      setup_return();
    }

    llvm::Value* parent_call_frame() {
      if(parent_info_) {
        return parent_info_->call_frame();
      }

      return 0;
    }

    JITMethodInfo* parent_info() {
      return parent_info_;
    }

    llvm::Value* top_parent_call_frame() {
      if(!parent_info_) return call_frame();

      JITMethodInfo* info = parent_info_;
      while(info->parent_info()) {
        info = info->parent_info();
      }

      return info->call_frame();
    }

    llvm::Value* creator_call_frame() {
      if(!creator_info_) return llvm::Constant::getNullValue(context_->ptr_type("CallFrame"));
      return creator_info_->call_frame();
    }

    bool for_inlined_method() {
      return parent_info_ != 0;
    }

    JITMethodInfo* creator_info() {
      return creator_info_;
    }

    void set_creator_info(JITMethodInfo* creator_info) {
      creator_info_ = creator_info;
    }

    JITMethodInfo* home_info() {
      JITMethodInfo* nfo = creator_info_;
      if(!nfo) return 0;

      while(JITMethodInfo* nxt = nfo->creator_info()) {
        nfo = nxt;
      }

      return nfo;
    }

    JITInlineBlock* inline_block() {
      return inline_block_;
    }

    void set_inline_block(JITInlineBlock* bi) {
      inline_block_ = bi;
    }

    void clear_inline_block() {
      inline_block_ = 0;
    }

    void set_block_info(JITInlineBlock* block) {
      block_info_ = block;
    }

    JITInlineBlock* block_info() {
      return block_info_;
    }

    llvm::BasicBlock* block_break_loc() {
      return block_info_->block_break_loc();
    }

    llvm::PHINode* block_break_result() {
      return block_info_->block_break_result();
    }

    llvm::Value* out_args() {
      return context_->out_args();
    }

    bool use_full_scope() {
      return use_full_scope_;
    }

    void set_use_full_scope() {
      use_full_scope_ = true;
    }

    llvm::Value* counter() {
      return context_->counter();
    }

    Class* self_class() {
      return self_class_.get();
    }

    void set_self_class(Class* cls) {
      self_class_.set((Object*)cls);
    }

    LocalInfo* get_local(int which) {
      std::map<int, LocalInfo>::iterator i = local_info_.find(which);
      if(i == local_info_.end()) {
        LocalInfo li(which);
        local_info_[which] = li;
        return &local_info_[which];
      }

      return &i->second;
    }

    void setup_return();

    llvm::AllocaInst* create_alloca(llvm::Type* type, llvm::Value* size = 0,
                                    const llvm::Twine& name = "");

    llvm::BasicBlock* new_block(const char* name);

  };


}

#endif

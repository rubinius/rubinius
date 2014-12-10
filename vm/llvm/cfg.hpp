#include <list>
#include "machine_code.hpp"

namespace rubinius {
namespace jit {
  class CFGBlock {
  public:
    typedef std::list<CFGBlock*> List;

  private:
    int start_ip_;
    int end_ip_;
    List children_;
    bool loop_;
    bool detached_;
    CFGBlock* exception_handler_;
    int exception_type_;

  public:
    CFGBlock(int start, bool loop=false)
      : start_ip_(start)
      , end_ip_(0)
      , loop_(loop)
      , detached_(true)
      , exception_handler_(0)
      , exception_type_(-1)
    {}

    int start_ip() {
      return start_ip_;
    }

    void set_end_ip(int ip) {
      end_ip_ = ip;
    }

    void add_child(CFGBlock* block) {
      if(block->detached_p()) {
        block->attached();
        children_.push_back(block);
      }
    }

    void attached() {
      detached_ = false;
    }

    bool detached_p() {
      return detached_;
    }

    bool loop_p() {
      return loop_;
    }

    CFGBlock* exception_handler() {
      return exception_handler_;
    }

    void set_exception_handler(CFGBlock* blk) {
      exception_handler_ = blk;
    }

    int exception_type() {
      return exception_type_;
    }

    void set_exception_type(int type) {
      exception_type_ = type;
    }
  };

  class CFGCalculator {
  public:
    typedef std::map<opcode, CFGBlock*> Blocks;

  private:
    Blocks blocks_;
    CFGBlock* root_;
    CFGBlock* current_;
    opcode* stream_;
    size_t stream_size_;

  public:
    CFGCalculator(opcode* stream, size_t size)
      : root_(0)
      , current_(0)
      , stream_(stream)
      , stream_size_(size)
    {}

    CFGCalculator(MachineCode* mcode)
      : root_(0)
      , current_(0)
      , stream_(mcode->opcodes)
      , stream_size_(mcode->total)
    {}

    ~CFGCalculator() {
      for(Blocks::iterator i = blocks_.begin();
          i != blocks_.end();
          ++i) {
        delete i->second;
      }
    }

    CFGBlock* root() {
      return root_;
    }

    CFGBlock* find_block(int ip) {
      Blocks::iterator i = blocks_.find(ip);
      if(i == blocks_.end()) return 0;
      return i->second;
    }

    void set_block(int ip, CFGBlock* blk) {
      assert(!blocks_[ip]);
      blocks_[ip] = blk;
    }

    CFGBlock* add_block(int ip, bool loop=false) {
      CFGBlock* blk = find_block(ip);
      if(blk) {
        // If we hit a block that is the start of a loop header,
        // be sure to set it's exception handler. These blocks are created
        // during the first pass.
        if(blk->loop_p()) {
          // Inherit the current exception handler
          blk->set_exception_handler(current_->exception_handler());
        }
        return blk;
      }

      blk = new CFGBlock(ip, loop);

      // Inherit the current exception handler
      blk->set_exception_handler(current_->exception_handler());

      set_block(ip, blk);
      return blk;
    }

    void find_backward_gotos() {
      MachineCode::Iterator iter(stream_, stream_size_);

      while(!iter.end()) {
        switch(iter.op()) {
        case InstructionSequence::insn_goto:
        case InstructionSequence::insn_goto_if_true:
        case InstructionSequence::insn_goto_if_false:
        case InstructionSequence::insn_goto_if_nil:
        case InstructionSequence::insn_goto_if_not_nil:
        case InstructionSequence::insn_goto_if_undefined:
        case InstructionSequence::insn_goto_if_not_undefined:
        case InstructionSequence::insn_goto_if_equal:
        case InstructionSequence::insn_goto_if_not_equal:
          if(iter.operand1() < iter.position()) {
            if(!find_block(iter.operand1())) {
              CFGBlock* blk = new CFGBlock(iter.operand1(), true);
              set_block(iter.operand1(), blk);
            }
          }
          break;
        }

        iter.inc();
      }
    }

    void close_current(MachineCode::Iterator& iter, CFGBlock* next) {
      current_->set_end_ip(iter.position());
      current_->add_child(next);
      current_ = next;
    }

    CFGBlock* start_new_block(MachineCode::Iterator& iter) {
      if(!iter.last_instruction()) {
        CFGBlock* blk = add_block(iter.next_position());
        close_current(iter, blk);
        return blk;
      }

      return 0;
    }

    void build() {
      find_backward_gotos();

      // Construct the root block specially.
      if(blocks_[0]) {
        root_ = blocks_[0];
      } else {
        root_ = new CFGBlock(0);
        blocks_[0] = root_;
      }

      current_ = root_;

      MachineCode::Iterator iter(stream_, stream_size_);
      for(;;) {
        if(CFGBlock* next_block = find_block(iter.position())) {
          if(next_block->loop_p() && current_ != next_block) {
            // The handler wasn't setup originally, so we have to set it now.
            next_block->set_exception_handler(current_->exception_handler());

            close_current(iter, next_block);
          } else {
            current_ = next_block;
          }
        }

        switch(iter.op()) {
        case InstructionSequence::insn_goto:
        case InstructionSequence::insn_goto_if_true:
        case InstructionSequence::insn_goto_if_false:
        case InstructionSequence::insn_goto_if_nil:
        case InstructionSequence::insn_goto_if_not_nil:
        case InstructionSequence::insn_goto_if_undefined:
        case InstructionSequence::insn_goto_if_not_undefined:
        case InstructionSequence::insn_goto_if_equal:
        case InstructionSequence::insn_goto_if_not_equal:
          if(iter.operand1() > iter.position()) {
            current_->add_child(add_block(iter.operand1()));
          } else {
#ifndef NDEBUG
            CFGBlock* loop_header = find_block(iter.operand1());
            assert(loop_header);
            assert(loop_header->exception_handler() == current_->exception_handler());
#endif
          }
          start_new_block(iter);
          break;

        case InstructionSequence::insn_setup_unwind: {
          assert(iter.operand1() > iter.position());
          CFGBlock* handler = add_block(iter.operand1());
          handler->set_exception_type(iter.operand2());

          current_->add_child(handler);

          CFGBlock* body = start_new_block(iter);
          assert(body); // make sure it's not at the end.

          body->set_exception_handler(handler);
          break;
        }
        case InstructionSequence::insn_pop_unwind: {
          assert(current_->exception_handler());
          CFGBlock* cont = start_new_block(iter);
          CFGBlock* current_handler = cont->exception_handler();
          assert(current_handler);

          // Effectively pop the current handler by setting the
          // blocks handler (and thus all blocks after it) to the current
          // handlers handler.
          cont->set_exception_handler(current_handler->exception_handler());
          break;
        }

        case InstructionSequence::insn_ensure_return:
        case InstructionSequence::insn_raise_exc:
        case InstructionSequence::insn_raise_return:
        case InstructionSequence::insn_raise_break:
        case InstructionSequence::insn_reraise:
        case InstructionSequence::insn_ret:
          start_new_block(iter);
          break;
        }

        if(!iter.advance()) break;
      }

      current_->set_end_ip(iter.position());
    }
  };
}}

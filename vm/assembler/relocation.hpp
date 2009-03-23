#ifndef RBX_ASM_RELOCATION
#define RBX_ASM_RELOCATION

#include <string>
#include <stdint.h>
#include <map>
#include <dlfcn.h>

namespace assembler {

  class Relocation {
  public: // Types
    enum Kind {
      Relative,
      LocalAbsolute
    };

    enum TargetKind {
      Absolute,
      Symbol
    };

  private:
    Kind  kind_;
    void* instruction_location_;
    void* address_;
    int   offset_;
    TargetKind target_kind_;
    std::string* symbol_;

  public:

    Relocation(Kind kind, void* insn, void* address, int offset = 4)
      : kind_(kind)
      , instruction_location_(insn)
      , address_(address)
      , offset_(offset)
      , target_kind_(Absolute)
      , symbol_(0)
    { }

    Kind kind() {
      return kind_;
    }

    TargetKind target_kind() {
      return target_kind_;
    }

    void* instruction_location() {
      return instruction_location_;
    }

    void* address() {
      return address_;
    }

    intptr_t value() {
      intptr_t insn = reinterpret_cast<intptr_t>(instruction_location_);
      intptr_t val = reinterpret_cast<intptr_t>(address_);
      if(kind_ == Relative) {
        val -= (insn + offset_);
      }

      return val;
    }

    // Write the relocation out to memory
    void write() {
      intptr_t* write_to = reinterpret_cast<intptr_t*>(instruction_location_);
      *write_to = value();
    }

    void references_symbol(const char* name) {
      if(symbol_) delete symbol_;
      symbol_ = new std::string(name);
      target_kind_ = Symbol;
    }

    std::string& symbol() {
      return *symbol_;
    }

    static void* resolve_symbol(std::string& str) {
      return dlsym(RTLD_DEFAULT, str.c_str());
    }

    void resolve_and_write() {
      if(target_kind_ == Symbol) {
        if(!symbol_) {
          std::cout << "Symbol relocation setup, but no symbol present!\n";
          return;
        }

        void* new_addr = Relocation::resolve_symbol(*symbol_);
        if(new_addr) {
          address_ = new_addr;
        } else {
          std::cout << "Unable to resolve symbol '" << *symbol_ << "': "
                    << dlerror() << "\n";
        }
      }

      write();
    }

    void adjust_base(void* old_base, void* new_base) {
      intptr_t ob = reinterpret_cast<intptr_t>(old_base);
      intptr_t nb = reinterpret_cast<intptr_t>(new_base);

      instruction_location_ = reinterpret_cast<void*>(
        nb + (reinterpret_cast<intptr_t>(instruction_location_) - ob));

      if(kind_ == Relocation::LocalAbsolute) {
        intptr_t diff = reinterpret_cast<intptr_t>(address_) - ob;
        address_ = reinterpret_cast<void*>(nb + diff);
      }
    }
  };

  typedef std::map<void*, Relocation*> Relocations;
}

#endif

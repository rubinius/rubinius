#include "oniguruma.h" // Must be first.
#include "regenc.h"
#include "transcoder.h"

#include "symbol_table.hpp"
#include "exception.hpp"
#include "configuration.hpp"

#include "class/array.hpp"
#include "class/encoding.hpp"
#include "class/exception.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"

#include "diagnostics/memory.hpp"

#include "logger.hpp"

#include <sstream>
#include <iomanip>
#include <mutex>

namespace rubinius {
  SymbolTable::SymbolTable()
    : diagnostic_(new diagnostics::SymbolTable())
    , lock_()
  {
  }

  SymbolTable::~SymbolTable() {
    if(diagnostic_) {
      delete diagnostic_;
      diagnostic_ = nullptr;
    }
  }

  void SymbolTable::sweep(STATE) {
    if(state->configuration()->diagnostics_memory_enabled) {
      diagnostic_->update();
      state->machine()->report_diagnostics(diagnostic_);
    }
  }

  SymbolTable::Kind SymbolTable::detect_kind(STATE, const Symbol* sym) {
    std::string str = strings[sym->index()];
    size_t size = str.size();
    uint8_t* p = reinterpret_cast<uint8_t*>(const_cast<char*>(str.c_str()));

    Encoding* e = Encoding::from_index(state, encodings[sym->index()]);
    OnigEncodingType* enc = e->encoding();

    // Constants start with A-Z, followed by alphanumeric characters or '_' or
    // non-ascii character.
    if(isupper(*p)) {
      uint8_t* e = p + size;
      int n = 0, code = 0;

      for(++p; p < e; p += n) {
        n = Encoding::precise_mbclen(p, e, enc);
        if(!ONIGENC_MBCLEN_CHARFOUND_P(n)) {
          return SymbolTable::eNormal;
        }

        n = ONIGENC_MBCLEN_CHARFOUND_LEN(n);
        code = ONIGENC_MBC_TO_CODE(enc, p, p + n);
        if(!(ONIGENC_IS_CODE_ALNUM(enc, code) || *p == '_' || !ISASCII(*p))) {
          return SymbolTable::eNormal;
        }
      }

      return SymbolTable::eConstant;
    }

    if(p[0] == '@') {
      // A class variable begins with @@
      if(size > 1 && p[1] == '@') {
        return SymbolTable::eCVar;
      }

      // An instance variable can't start with a digit and can't be just @.
      if((size == 1) || (size > 1 && ISDIGIT(p[1]))) {
        return SymbolTable::eNormal;
      }

      // An instance variable begins with @
      return SymbolTable::eIVar;
    }

    // A system variable begins with __
    if(size > 2 && p[0] == '_' && p[1] == '_') {
      return SymbolTable::eSystem;
    }

    // Everything else is normal
    return SymbolTable::eNormal;
  }

  SymbolTable::Kind SymbolTable::kind(STATE, const Symbol* sym) {
    std::lock_guard<locks::spinlock_mutex> guard(lock_);

    Kind k = kinds[sym->index()];

    if(k == eUnknown) {
      k = kinds[sym->index()] = detect_kind(state, sym);
    }

    return k;
  }

  size_t SymbolTable::add(STATE, std::string str, int enc) {
    size_t bytes = (str.size() + sizeof(std::string) + sizeof(int) + sizeof(Kind));
    diagnostic()->objects++;
    diagnostic()->bytes += bytes;

    strings.push_back(str);
    encodings.push_back(enc);
    kinds.push_back(eUnknown);

    state->diagnostics()->memory_metrics()->symbols++;
    state->diagnostics()->memory_metrics()->symbols_bytes += bytes;

    return strings.size() - 1;
  }

  Symbol* SymbolTable::lookup(STATE, const char* str, size_t length) {
    return lookup(state, str, length, Encoding::eAscii, state->hash_seed());
  }

  Symbol* SymbolTable::lookup(STATE, const std::string& str) {
    return lookup(state, str.data(), str.size(), Encoding::eAscii, state->hash_seed());
  }

  Symbol* SymbolTable::lookup(STATE, const char* str, size_t length,
      int enc, uint32_t seed)
  {
    size_t sym;

    hashval hash = String::hash_str((unsigned char*)str, length, seed);

    // Symbols can be looked up by multiple threads at the same time.
    // This is fast operation, so we protect this with a spinlock.
    {
      std::lock_guard<locks::spinlock_mutex> guard(lock_);

      SymbolMap::iterator entry = symbols.find(hash);
      if(entry == symbols.end()) {
        sym = add(state, std::string(str, length), enc);
        SymbolIds v(1, sym);
        symbols[hash] = v;
      } else {
        SymbolIds& v = entry->second;
        for(SymbolIds::const_iterator i = v.begin(); i != v.end(); ++i) {
          std::string& s = strings[*i];
          int e = encodings[*i];

          if(!strncmp(s.data(), str, length) && e == enc) return Symbol::from_index(*i);
        }
        sym = add(state, std::string(str, length), enc);
        v.push_back(sym);
      }
    }

    return Symbol::from_index(sym);
  }

  Symbol* SymbolTable::lookup(STATE, String* str) {
    if(str->nil_p()) {
      Exception::raise_argument_error(state, "Cannot look up Symbol from nil");
      return NULL;
    }

    // Since we also explicitly use the size, we can safely
    // use byte_address() here.
    const char* bytes = (const char*) str->byte_address();
    size_t size = str->byte_size();

    int enc = str->encoding(state)->index();

    if(CBOOL(str->ascii_only_p(state))) {
      enc = Encoding::eAscii;
    }
    return lookup(state, bytes, size, enc, state->hash_seed());
  }

  String* SymbolTable::lookup_string(STATE, const Symbol* sym) {
    std::lock_guard<locks::spinlock_mutex> guard(lock_);

    if(sym->nil_p()) {
      Exception::raise_argument_error(state, "Cannot look up Symbol from nil");
      return NULL;
    }

    size_t sym_index = sym->index();
    if(sym_index >= strings.size()) {
      return NULL;
    }
    std::string& str = strings[sym_index];
    int enc = encodings[sym_index];
    String* s = String::create(state, str.data(), str.size());
    s->encoding(state, Encoding::from_index(state, enc));
    return s;
  }

  std::string& SymbolTable::lookup_cppstring(const Symbol* sym) {
    std::lock_guard<locks::spinlock_mutex> guard(lock_);

    return strings[sym->index()];
  }

  int SymbolTable::lookup_encoding(const Symbol* sym) {
    std::lock_guard<locks::spinlock_mutex> guard(lock_);

    return encodings[sym->index()];
  }

  std::string SymbolTable::lookup_debug_string(const Symbol* sym) {
    std::lock_guard<locks::spinlock_mutex> guard(lock_);

    std::string str = strings[sym->index()];
    std::ostringstream os;
    unsigned char* cstr = (unsigned char*) str.data();
    size_t size = str.size();
    for(size_t i = 0; i < size; ++i) {
      if(isprint(cstr[i]) && isascii(cstr[i])) {
        os << cstr[i];
      } else {
        os << "\\x" << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)cstr[i];
      }
    }
    return os.str();
  }

  Array* SymbolTable::all_as_array(STATE) {
    std::lock_guard<locks::spinlock_mutex> guard(lock_);

    size_t idx = 0;
    Array* ary = Array::create(state, strings.size());

    for(SymbolMap::iterator s = symbols.begin(); s != symbols.end(); ++s) {
      for(SymbolIds::iterator i = s->second.begin(); i != s->second.end(); ++i) {
        ary->set(state, idx++, Symbol::from_index(state, *i));
      }
    }

    return ary;
  }
}

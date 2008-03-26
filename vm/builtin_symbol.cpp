#include "prelude.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#define StartSize 16
#define Increments 32

namespace rubinius {
  SymbolTable* SymbolTable::create(STATE) {
    SymbolTable *tbl = (SymbolTable*)state->om->new_object(state->globals.symtbl,
                                                           SymbolTable::fields);
    SET(tbl, symbols, Tuple::create(state, StartSize));
    SET(tbl, strings, Hash::create(state, StartSize));

    return tbl;
  }

  OBJECT SymbolTable::lookup(STATE, const unsigned char* str, size_t size) {
    hashval hash = String::hash_str(str, size);
    OBJECT ent = strings->get(state, hash);

    /* If it wasn't present, use the longer, more correct version. */
    if(ent->nil_p() || ent == Qundef) {
      return lookup(state, String::create(state, (char*)str, size));
    }

    String* key = (String*)ent->at(1);
    /* TODO: validate that key is a String */
    char* cur =  key->byte_address(state);

    /* Check that this is actually the right string. */
    if(size != (size_t)key->bytes->n2i() || strncmp(cur, (const char*)str, size)) {
      return lookup(state, String::create(state, (char*)str, size));
    }

    return Symbol::from_index(state, ent->at(2)->n2i());
  }

  OBJECT SymbolTable::lookup(STATE, String* str) {
    hashval hash = str->hash_string(state);

    OBJECT idx = Qnil;

    if(!strings->lookup2(state, String::string_equal_p, str, hash, &idx)) {
      idx = strings->entries;
      size_t sz = symbols->field_count;

      if((size_t)idx->n2i() == sz) {
        Tuple* ns = (Tuple*)Tuple::create(state, sz + Increments);
        for(size_t i = 0; i < sz; i++) {
          ns->put(state, i, symbols->at(i));
        }
        SET(this, symbols, ns);
      }

      symbols->put(state, idx->n2i(), str);
      strings->assign(state, String::string_equal_p, str, hash, idx);
    }

    return Symbol::from_index(state, idx->n2i());
  }

  OBJECT SymbolTable::find_string(STATE, Symbol* sym) {
    return symbols->at(sym->index());
  }

  OBJECT Symbol::to_str(STATE) {
    return state->globals.symbols->find_string(state, this);
  }
}

#include "prelude.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#define StartSize 16
#define Increments 32

namespace rubinius {
  SymbolTable* SymbolTable::create(STATE) {
    SymbolTable *tbl = (SymbolTable*)state->om->new_object(G(symtbl),
                                                           SymbolTable::fields);
    SET(tbl, symbols, Tuple::create(state, StartSize));
    SET(tbl, strings, Hash::create(state, StartSize));

    return tbl;
  }

  SYMBOL SymbolTable::lookup(STATE, const char* str, size_t size) {
    if(!size) size = std::strlen(str);

    hashval hash = String::hash_str((unsigned char*)str, size);
    OBJECT ent = strings->find_entry(state, hash);

    /* If it wasn't present, use the longer, more correct version. */
    if(ent->nil_p() || ent == Qundef) {
      return lookup(state, String::create(state, str, size));
    }

    String* key = as<String>(ent->at(1));
    /* TODO: validate that key is a String */
    char* cur =  key->byte_address(state);

    /* Check that this is actually the right string. */
    if(size != (size_t)key->size(state) || strncmp(cur, str, size)) {
      return lookup(state, String::create(state, str, size));
    }

    return Symbol::from_index(state, as<Fixnum>(ent->at(2))->n2i());
  }

  SYMBOL SymbolTable::lookup(STATE, String* str) {
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

  String* SymbolTable::find_string(STATE, SYMBOL sym) {
    return as<String>(symbols->at(sym->index()));
  }

  String* Symbol::to_str(STATE) {
    return G(symbols)->find_string(state, this);
  }
}

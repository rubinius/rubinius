// This is in a seperate file, even though it would normally be in system.cpp,
// because it uses a whole bunch of local classes and it's cleaner to have
// all that be in it's own file.

#include "vm/vm.hpp"

#include "gc/gc.hpp"
#include "gc/walker.hpp"
#include "objectmemory.hpp"

#include "builtin/object.hpp"
#include "builtin/array.hpp"
#include "builtin/symbol.hpp"
#include "builtin/module.hpp"
#include "builtin/string.hpp"
#include "builtin/class.hpp"
#include "builtin/tuple.hpp"

#include "builtin/system.hpp"

#include "object_utils.hpp"

#include <fcntl.h>
#include <arpa/inet.h>

#include <map>
#include <vector>
#include <algorithm>
#include <iostream>

namespace rubinius {
  typedef std::map<Object*, int> Identities;
  typedef std::map<Symbol*, int> Symbols;

  const static int cSymbolCode = 's';
  const static int cObjectCode = 'o';
  const static int cLayoutCode = 'l';
  const static int cRefCode    = 'r';
  const static int cSymRefCode = 'x';
  const static int cFixnumCode = 'f';
  const static int cImmCode =    'i';
  const static int cTupleCode =  't';
  const static int cBytesCode =  'b';
  const static int cFooterCode = '-';

  class Encoder {
    int fd_;
    Symbols symbols_;
    int sym_ids_;

  public:

    Encoder(int fd)
      : fd_(fd)
      , sym_ids_(0)
    {}

    void write1(uint8_t num) {
      if(write(fd_, &num, 1) == -1) perror("write(2)");
    }

    void write2(uint16_t num) {
      num = htons(num);
      if(write(fd_, &num, 2) == -1) perror("write(2)");
    }

    void write4(uint32_t num) {
      num = htonl(num);
      if(write(fd_, &num, 4) == -1) perror("write(2)");
    }

    void write_raw(const char* str, int count) {
      if(write(fd_, str, count) == -1) perror("write(2)");
    }

    void start_object(uint32_t id) {
      write1(cObjectCode);
      write4(id);
    }

    void start_layout(uint32_t id) {
      write1(cLayoutCode);
      write4(id);
    }

    int initialize_symbol(STATE, Symbol* sym) {
      Symbols::iterator i = symbols_.find(sym);
      if(i != symbols_.end()) {
        return i->second;
      }

      int id = sym_ids_++;

      symbols_[sym] = id;

      const char* str = sym->c_str(state);
      int sz = strlen(str);

      write1(cSymbolCode);
      write4(id);
      write4(sz);
      write_raw(str, sz);

      return id;
    }

    void cached_symbol(STATE, Symbol* sym) {
      Symbols::iterator i = symbols_.find(sym);
      if(i != symbols_.end()) {
        write1(cSymRefCode);
        write4(i->second);
      }

      initialize_symbol(state, sym);
    }

    void write_fixnum(Fixnum* fix) {
      write1(cFixnumCode);
      write4(fix->to_native());
    }

    void write_immediate(Object* obj) {
      write1(cImmCode);
      if(obj->false_p()) {
        write1(0);
      } else if(obj->true_p()) {
        write1(1);
      } else {
        write1(2);
      }
    }
  };

  class Layout;

  class LayoutManager {
    int id_;

  public:
    LayoutManager()
      : id_(0)
    {}

    Layout* new_layout(Symbol* sym);
  };

  typedef std::vector<Symbol*> SymbolList;
  typedef std::map<Symbol*, Layout*> LayoutMapping;

  class LayoutManager;

  class Layout {
    LayoutManager& manager_;
    int id_;
    Symbol* name_;
    LayoutMapping subtrees_;
    bool written_;

  public:
    Layout(LayoutManager& manager, int id, Symbol* name)
      : manager_(manager)
      , id_(id)
      , name_(name)
      , written_(false)
    {}

    ~Layout() {
      for(LayoutMapping::iterator i = subtrees_.begin();
          i != subtrees_.end();
          i++) {
        delete i->second;
      }
    }

    int id() {
      return id_;
    }

    Symbol* name() {
      return name_;
    }

    Layout* find_subtree(Symbol* sym) {
      LayoutMapping::iterator i = subtrees_.find(sym);
      if(i != subtrees_.end()) {
        return i->second;
      }

      Layout* layout = manager_.new_layout(sym);

      subtrees_[sym] = layout;

      return layout;
    }

    void encode(STATE, Encoder& enc, SymbolList& syms) {
      if(written_) return;
      written_ = true;

      std::vector<int> sym_ids;
      for(SymbolList::iterator i = syms.begin();
          i != syms.end();
          i++) {
        sym_ids.push_back(enc.initialize_symbol(state, *i));
      }

      enc.start_layout(id_);
      enc.write4(syms.size());

      for(std::vector<int>::iterator i = sym_ids.begin();
          i != sym_ids.end();
          i++) {
        enc.write4(*i);
      }
    }
  };

  Layout* LayoutManager::new_layout(Symbol* sym) {
    return new Layout(*this, id_++, sym);
  }

  static Layout* find_layout(STATE, Object* obj, Layout* root, Array* ary, SymbolList& syms) {
    // Collect all the ivar names that we want to pull out
    TypeInfo* ti = state->om->type_info[obj->type_id()];
    for(TypeInfo::Slots::iterator i = ti->slots.begin();
        i != ti->slots.end();
        i++) {
      syms.push_back(Symbol::from_index(i->first));
    }

    // Reset the array.
    ary->total(state, Fixnum::from(0));

    obj->ivar_names(state, ary);
    for(size_t i = 0; i < ary->size(); i++) {
      if(Symbol* sym = try_as<Symbol>(ary->get(state, i))) {
        syms.push_back(sym);
      }
    }

    // Ok, now sort them..
    std::sort(syms.begin(), syms.end());

    Layout* current = root;

    // Look down the Layout tree to find the one we want.
    for(SymbolList::iterator i = syms.begin();
        i != syms.end();
        i++) {
      current = current->find_subtree(*i);
    }

    return current;
  }

  class HeapDump {
    Identities ids;
    int total_ids;
    LayoutManager mgr;
    Layout* root;
    Encoder enc;
    Array* tmp;

  public:
    HeapDump(int fd)
      : total_ids(0)
      , enc(fd)
      , tmp(0)
    {
      root = mgr.new_layout(0);
    }

    int object_id(Object* obj) {
      Identities::iterator i = ids.find(obj);
      if(i != ids.end()) {
        return i->second;
      }

      int id = total_ids++;
      ids[obj] = id;
      return id;
    }

    bool seen_object_p(Object* obj) {
      Identities::iterator i = ids.find(obj);
      return i != ids.end();
    }

    void dump_reference(STATE, Object* sub) {
      if(sub->reference_p()) {
        int sub_id = object_id(sub);
        enc.write1(cRefCode);
        enc.write4(sub_id);
      } else if(Symbol* subsym = try_as<Symbol>(sub)) {
        enc.cached_symbol(state, subsym);
      } else if(Fixnum* subfix = try_as<Fixnum>(sub)) {
        enc.write_fixnum(subfix);
      } else {
        enc.write_immediate(sub);
      }
    }

    void dump_object(STATE, Object* obj) {
      if(!tmp) tmp = Array::create(state, 20);
      SymbolList syms;
      Layout* layout = find_layout(state, obj, root, tmp, syms);
      layout->encode(state, enc, syms);

      int id = object_id(obj);

      enc.start_object(id);

      enc.write4(obj->size_in_bytes(state));

      enc.write4(layout->id());

      enc.write4(object_id(obj->reference_class()));
      dump_reference(state, obj->ivars());

      if(Tuple* tup = try_as<Tuple>(obj)) {
        enc.write2(syms.size() + 1);
        dump_ivars(state, obj, syms);

        enc.write1(cTupleCode);
        enc.write4(tup->num_fields());

        for(size_t i = 0; i < tup->num_fields(); i++) {
          dump_reference(state, tup->at(i));
        }
      } else if(ByteArray* ba = try_as<ByteArray>(obj)) {
        enc.write2(syms.size() + 1);
        dump_ivars(state, obj, syms);

        enc.write1(cBytesCode);
        enc.write4(ba->size());
        enc.write_raw((const char*)ba->raw_bytes(), ba->size());
      } else {
        enc.write2(syms.size());
        dump_ivars(state, obj, syms);
      }
    }

    void dump_ivars(STATE, Object* obj, SymbolList& syms) {
      for(SymbolList::iterator i = syms.begin();
          i != syms.end();
          i++) {
        dump_reference(state, obj->get_ivar(state, *i));
      }
    }

    void header(STATE) {
      enc.write_raw("RBXHEAPDUMP\0", 12);
      enc.write4(1);
    }

    void footer(STATE) {
      enc.write1(cFooterCode);
      enc.write4(object_id(G(object)));
      enc.write4(object_id(G(klass)));
      enc.write4(object_id(G(module)));
    }
  };

  Object* System::vm_dump_heap(STATE, String* path) {
    ObjectWalker walker(state->om);
    GCData gc_data(state);

    // Seed it with the root objects.
    walker.seed(gc_data);

    int fd = open(path->c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd < 0) return Qnil;

    HeapDump dump(fd);

    dump.header(state);

    Object* obj = walker.next();

    while(obj) {
      dump.dump_object(state, obj);
      obj = walker.next();
    }

    dump.footer(state);

    std::cout << "Heap dumped to " << path->c_str() << "\n";
    close(fd);

    return Qnil;
  }
}

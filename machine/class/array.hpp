#ifndef RBX_ARRAY_HPP
#define RBX_ARRAY_HPP

#include "object_utils.hpp"

#include "class/fixnum.hpp"
#include "class/object.hpp"
#include "class/tuple.hpp"

namespace rubinius {
  class Tuple;
  class Fixnum;

  class Array : public Object {
  public:
    const static object_type type = ArrayType;

    enum HeaderFlags {
      eRArray = 1,
    };

    Fixnum* _total_;    // slot
    Fixnum* _start_;    // slot
    Tuple* _tuple_;     // slot

    void read_rarray();
    void write_rarray(STATE);

    Fixnum* total() {
      if(type_specific() == eRArray) {
        read_rarray();
      }

      return _total_;
    }

    Object** p_total() {
      return reinterpret_cast<Object**>(&this->_total_);
    }

    void total(Fixnum* obj) {
      _total_ = obj;
    }

    template<typename T>
      void total(T state, Fixnum* obj) {
        total(obj);

        if(type_specific() == eRArray) {
          write_rarray(state);
        }
      }

    Fixnum* start() const {
      return _start_;
    }

    void start(Fixnum* obj) {
      _start_ = obj;
    }

    Object** p_start() {
      return reinterpret_cast<Object**>(&this->_start_);
    }

    template<typename T>
      void start(T state, Fixnum* obj) {
        start(obj);

        if(type_specific() == eRArray) {
          write_rarray(state);
        }
      }

    Tuple* tuple() const {
      return _tuple_;
    }

    void tuple(Tuple* obj) {
      _tuple_ = obj;
    }

    Object** p_tuple() {
      return reinterpret_cast<Object**>(&this->_tuple_);
    }

    template<typename T>
    void tuple(T* state, Tuple* obj) {
      tuple(obj);
      memory::write_barrier(state, this, obj);

      if(type_specific() == eRArray) {
        write_rarray(state);
      }
    }

    intptr_t size();
    intptr_t offset();

    static void bootstrap(STATE);
    static void initialize(STATE, Array* array) {
      array->total(Fixnum::from(0));
      array->tuple(nil<Tuple>());
      array->start(Fixnum::from(0));
    }

    static Array* create(STATE, intptr_t size);
    static Array* from_tuple(STATE, Tuple* tup);
    static Array* to_ary(STATE, Object* obj);

    // Rubinius.primitive :array_allocate
    static Array* allocate(STATE, Object* self);

    // Rubinius.primitive :array_dup_as_array
    static Array* dup_as_array(STATE, Object* obj);

    // Rubinius.primitive :array_new_range
    Array* new_range(STATE, Fixnum* index, Fixnum* count);

    // Rubinius.primitive :array_new_reserved
    Array* new_reserved(STATE, Fixnum* count);

    // Rubinius.primitive+ :array_aref
    Object* aref(STATE, Fixnum* idx);

    // Rubinius.primitive :array_aset
    Object* aset(STATE, Fixnum* idx, Object* val);

    // Rubinius.primitive :array_concat
    Array* concat(STATE, Array* other);

    // Rubinius.primitive :array_pack
    String* pack(STATE, String* directives);

    Object* get(STATE, intptr_t idx);
    Object* set(STATE, intptr_t idx, Object* val);
    void   unshift(STATE, Object* val);
    Object* shift(STATE);
    Object* append(STATE, Object* val);
    Object* pop(STATE);
    bool   includes_p(STATE, Object* val);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
    };
  };
}

#endif

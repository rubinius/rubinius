#ifndef RBX_BUILTIN_CONSTANT_TABLE_HPP
#define RBX_BUILTIN_CONSTANT_TABLE_HPP

#include "builtin/object.hpp"
#include "builtin/tuple.hpp"
#include "object_utils.hpp"

namespace rubinius {
  class Tuple;
  class Array;
  class Object;
  class Symbol;

  class ConstantTableBucket : public Object {
  public:
    const static object_type type = ConstantTableBucketType;

  private:
    Symbol* name_;   // slot
    Object* constant_; // slot
    Symbol* visibility_; // slot
    ConstantTableBucket* next_;  // slot

  public:
    attr_accessor(name, Symbol);
    attr_accessor(constant, Object);
    attr_accessor(visibility, Symbol);
    attr_accessor(next, ConstantTableBucket);

    static ConstantTableBucket* create(STATE, Symbol* name,
        Object* constant, Symbol* visibility);

    Object* append(STATE, ConstantTableBucket *nxt);

    bool private_p(STATE);
    bool public_p(STATE);
    bool protected_p(STATE);
    bool undef_p(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  #define CONSTANT_TABLE_MIN_SIZE 16
  class ConstantTable : public Object {
  public:
    const static object_type type = ConstantTableType;

  private:
    Tuple* values_;   // slot
    Integer* bins_;    // slot
    Integer* entries_; // slot
    utilities::thread::SpinLock lock_;

    void   redistribute(STATE, native_int size);

  public:
    /* accessors */

    attr_accessor(values, Tuple);
    attr_accessor(bins, Integer);
    attr_accessor(entries, Integer);

    /* interface */

    static ConstantTable* create(STATE, native_int sz = CONSTANT_TABLE_MIN_SIZE);
    void setup(STATE, native_int sz);

    // Rubinius.primitive :constant_table_allocate
    static ConstantTable* allocate(STATE, Object* self);

    // Rubinius.primitive :constant_table_store
    Object* store(STATE, Symbol* name, Object* constant, Symbol* vis);

    // Rubinius.primitive :constant_table_duplicate
    ConstantTable* duplicate(STATE);

    ConstantTableBucket* find_entry(STATE, Symbol* name);

    // Rubinius.primitive+ :constant_table_lookup
    ConstantTableBucket* lookup(STATE, Symbol* name);

    // Rubinius.primitive :constant_table_delete
    Object* remove(STATE, Symbol* name);

    // Rubinius.primitive+ :constant_table_has_name
    Object* has_name(STATE, Symbol* name);

    // Rubinius.primitive :constant_table_keys
    Array* all_keys(STATE);

    class iterator {
      ConstantTable* tbl_;
      ConstantTableBucket* bucket_;
      int index_;
      int total_;

    public:
      iterator(ConstantTable* tbl)
        : tbl_(tbl)
        , bucket_(0)
        , index_(-1)
        , total_(tbl->bins()->to_native())
      {}

      bool advance() {
        if(index_ >= total_) return false;

        if(bucket_) {
          bucket_ = try_as<ConstantTableBucket>(bucket_->next());
        }

        while(!bucket_) {
          index_++;
          if(index_ >= total_) return false;
          bucket_ = try_as<ConstantTableBucket>(tbl_->values()->at(index_));
        }

        return true;
      }

      ConstantTableBucket* bucket() {
        return bucket_;
      }

      Symbol* name() {
        return bucket_->name();
      }

      Symbol* visibility() {
        return bucket_->visibility();
      }

      Object* constant() {
        return bucket_->constant();
      }

    };


    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
    };
  };
}

#endif


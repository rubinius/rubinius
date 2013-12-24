#ifndef RBX_BUILTIN_LOOKUPTABLE_HPP
#define RBX_BUILTIN_LOOKUPTABLE_HPP

#include "builtin/object.hpp"
#include "builtin/tuple.hpp"
#include "builtin/integer.hpp"
#include "object_utils.hpp"

namespace rubinius {

  class Tuple;
  class Array;

  class LookupTableBucket : public Object {
  public:
    const static object_type type = LookupTableBucketType;

  private:
    Object *key_;   // slot
    Object *value_; // slot
    LookupTableBucket *next_;  // slot

  public:
    attr_accessor(key, Object);
    attr_accessor(value, Object);
    attr_accessor(next, LookupTableBucket);

    static LookupTableBucket* create(STATE, Object* key, Object* value);

    Object* append(STATE, LookupTableBucket *nxt);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  #define LOOKUPTABLE_MIN_SIZE 16
  class LookupTable : public Object {
  public:
    const static object_type type = LookupTableType;

  private:
    Tuple* values_;   // slot
    Integer* bins_;    // slot
    Integer* entries_; // slot

  public:
    /* accessors */

    attr_accessor(values, Tuple);
    attr_accessor(bins, Integer);
    attr_accessor(entries, Integer);

    /* interface */

    static LookupTable* create(STATE, size_t sz = LOOKUPTABLE_MIN_SIZE);
    void setup(STATE, size_t sz);

    // Rubinius.primitive :lookuptable_allocate
    static LookupTable* allocate(STATE, Object* self);

    // Rubinius.primitive :lookuptable_store
    Object* store(STATE, Object* key, Object* val);

    // Rubinius.primitive+ :lookuptable_aref
    Object* aref(STATE, Object* key);
    /** Compatibility, this is the same as aref(). */
    Object* fetch(STATE, Object* key);

    // Rubinius.primitive+ :lookuptable_fetch
    Object* fetch(STATE, Object* key, Object* return_on_failure);

    Object* fetch(STATE, Object* key, bool* found);

    // Rubinius.primitive :lookuptable_duplicate
    LookupTable* duplicate(STATE);

    void   redistribute(STATE, size_t size);

    LookupTableBucket* find_entry(STATE, Object* key);
    Object* find(STATE, Object* key);

    Object* remove(STATE, Object* key, bool* removed = 0);

    // Rubinius.primitive :lookuptable_delete
    Object* delete_prim(STATE, Object* key) {
      return remove(state, key, 0);
    }

    // Rubinius.primitive+ :lookuptable_has_key
    Object* has_key(STATE, Object* key);

    static Array* collect(STATE, LookupTable* tbl,
                          Object* (*action)(STATE, LookupTableBucket*));
    static Object* get_key(STATE, LookupTableBucket* entry);

    // Rubinius.primitive :lookuptable_keys
    Array* all_keys(STATE);

    Array* filtered_keys(STATE, ObjectMatcher& match, Array* ary=0);

    static Object* get_value(STATE, LookupTableBucket* entry);

    // Rubinius.primitive :lookuptable_values
    Array* all_values(STATE);

    static Object* get_entry(STATE, LookupTableBucket* entry);

    // Rubinius.primitive :lookuptable_entries
    Array* all_entries(STATE);

    // This must be below all primitive declarations due to parsing bug
    // in field_extract
    class CollectAction {
    public:
      virtual Object* call(STATE, LookupTableBucket* bucket) = 0;
      virtual ~CollectAction() {}
    };

    Array* collect(STATE, LookupTable* tbl, CollectAction& action, Array* ary=0);

    class iterator {
      LookupTable* tbl_;
      LookupTableBucket* bucket_;
      int index_;
      int total_;

    public:
      iterator(LookupTable* tbl)
        : tbl_(tbl)
        , bucket_(0)
        , index_(-1)
        , total_(tbl->bins()->to_native())
      {}

      bool advance() {
        if(index_ >= total_) return false;

        if(bucket_) {
          bucket_ = try_as<LookupTableBucket>(bucket_->next());
        }

        while(!bucket_) {
          index_++;
          if(index_ >= total_) return false;
          bucket_ = try_as<LookupTableBucket>(tbl_->values()->at(index_));
        }

        return true;
      }

      LookupTableBucket* bucket() {
        return bucket_;
      }

      Object* key() {
        return bucket_->key();
      }

      Object* value() {
        return bucket_->value();
      }

    };

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
    };
  };
};

#endif

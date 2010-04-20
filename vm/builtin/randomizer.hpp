#ifndef RBX_BUILTIN_RANDOMIZER_HPP
#define RBX_BUILTIN_RANDOMIZER_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class ByteArray;
  class Float;
  class Integer;

  class Randomizer : public Object {
  public:
    const static object_type type = RandomizerType;

  private:
    int next;
    int left;
    ByteArray *rng_state_; // slot

    uint32_t* rng_data();

    void init_genrand(uint32_t s);
    void init_by_array(uint32_t init_key[], int key_length);
    void next_state();
    uint32_t rb_genrand_int32();
    native_uint limited_rand(native_uint limit);
    double rb_genrand_real();

  public:

    attr_accessor(rng_state, ByteArray);

    /* interface */

    static void init(STATE);


    static Randomizer* create(STATE);

    // Ruby.primitive :randomizer_allocate
    static Randomizer* allocate(STATE, Object* self);

    // Ruby.primitive :randomizer_gen_seed
    Integer* gen_seed(STATE);

    // Ruby.primitive :randomizer_seed
    Object* seed(STATE, Integer* seed);

    // Ruby.primitive :randomizer_rand_int
    Integer* rand_int(STATE, Integer* max);

    // Ruby.primitive :randomizer_rand_float
    Float* rand_float(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif

#ifndef RBX_BUILTIN_RANDOMIZER_HPP
#define RBX_BUILTIN_RANDOMIZER_HPP

#include "builtin/object.hpp"
#include "util/random.h"

namespace rubinius {
  class ByteArray;
  class Float;
  class Integer;

  class Randomizer : public Object {
  public:
    const static object_type type = RandomizerType;

  private:
    int lock_;
    ByteArray *rng_data_; // slot

    struct random_state* rng_state();

    void init_by_single(uint32_t s);
    void init_by_array(uint32_t init_key[], int key_length);
    uint32_t rb_genrand_int32();
    double rb_genrand_real();
    native_uint limited_rand(native_uint limit);

  public:

    attr_accessor(rng_data, ByteArray);

    /* interface */

    static void init(STATE);


    static Randomizer* create(STATE);

    // Rubinius.primitive :randomizer_allocate
    static Randomizer* allocate(STATE, Object* self);

    // Rubinius.primitive :randomizer_gen_seed
    Integer* gen_seed(STATE);

    // Rubinius.primitive :randomizer_seed
    Object* seed(STATE, Integer* seed);

    // Rubinius.primitive :randomizer_rand_int
    Integer* rand_int(STATE, Integer* max);

    // Rubinius.primitive :randomizer_rand_float
    Float* rand_float(STATE);

    static uint32_t random_uint32();

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif

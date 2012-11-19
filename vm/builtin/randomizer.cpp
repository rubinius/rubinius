#include "vm/config.h"

#include "builtin/randomizer.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/bignum.hpp"
#include "builtin/class.hpp"
#include "builtin/float.hpp"
#include "builtin/integer.hpp"

#include "util/spinlock.hpp"

#include "object_utils.hpp"

#include "ontology.hpp"

#ifdef RBX_WINDOWS
#include <malloc.h>
#include <process.h>
#endif

template <typename T>
static T make_mask(T x) {
  for(unsigned int i = 1; i < sizeof(T) * 8; i *= 2)
    x = x | x >> i;
  return x;
}

namespace rubinius {
  struct random_state* Randomizer::rng_state() {
    return (struct random_state*)rng_data()->raw_bytes();
  }

  void Randomizer::init_by_single(uint32_t s) {
    struct random_state* rng = rng_state();
    rbx_spinlock_lock(&lock_);
    random_init_single(rng, s);
    rbx_spinlock_unlock(&lock_);
  }

  void Randomizer::init_by_array(uint32_t init_key[], int key_length) {
    struct random_state* rng = rng_state();
    rbx_spinlock_lock(&lock_);
    random_init_array(rng, init_key, key_length);
    rbx_spinlock_unlock(&lock_);
  }

  /* generates a random number on [0,0xffffffff]-interval */
  uint32_t Randomizer::rb_genrand_int32() {
    uint32_t y;
    rbx_spinlock_lock(&lock_);
    y = random_gen_uint32(rng_state());
    rbx_spinlock_unlock(&lock_);
    return y;
  }

  native_uint Randomizer::limited_rand(native_uint limit) {
    native_uint mask = make_mask(limit);

  retry:
    native_uint val = 0;
    for(int i = sizeof(native_uint)/4-1; 0 <= i; i--) {
      if(mask >> (i * 32)) {
        val |= rb_genrand_int32() << (i * 32);
        val &= mask;
        if(limit < val) goto retry;
      }
    }
    return val;
  }

  double Randomizer::rb_genrand_real() {
    double y;
    rbx_spinlock_lock(&lock_);
    y = random_gen_double(rng_state());
    rbx_spinlock_unlock(&lock_);
    return y;
  }

  void Randomizer::init(STATE) {
    GO(randomizer).set(ontology::new_class(state,
          "Randomizer", G(object), G(rubinius)));
    G(randomizer)->set_object_type(state, RandomizerType);
  }

  Randomizer* Randomizer::create(STATE) {
    Randomizer* r = state->new_object<Randomizer>(G(randomizer));
    r->lock_ = RBX_SPINLOCK_INIT;
    r->rng_data(state, ByteArray::create(state, sizeof(struct random_state)));
    random_init_single(r->rng_state(), 5489UL);
    return r;
  }

  Randomizer* Randomizer::allocate(STATE, Object* self) {
    Randomizer* randomizer = create(state);

    if(Class* cls = try_as<Class>(self)) {
      randomizer->klass(state, cls);
    }

    return randomizer;
  }

  Integer* Randomizer::gen_seed(STATE) {

    uint32_t seed[] = { 0, 0, 0, 0 };
    random_seed(seed, 4);
    return Bignum::from_array(state, seed, 4);
  }

  Object* Randomizer::seed(STATE, Integer* seed) {
    Bignum* big;

    size_t longs;

    if(!seed->fixnum_p())
      seed = as<Bignum>(seed)->abs(state);

    if(seed->fixnum_p()) {
      native_uint s = seed->to_native();

      // Remove the sign bit, for no good reason... but that's what MRI
      // does.
      s &= ~((native_uint)1 << (sizeof(native_uint) * 8 - 1));

      if(s <= 0xffffffffUL) {
        init_by_single((uint32_t)s);
        return cNil;
      }

      longs = sizeof(native_uint) / 4;
      uint32_t data[longs];
      for(unsigned int i = 0; i < longs; i++) {
        data[i] = (uint32_t)(s >> (i * 32));
      }

      init_by_array(data, longs);
    } else {
      big = as<Bignum>(seed);

      longs = big->into_array(state, NULL, 0);

      uint32_t data[longs];
      big->into_array(state, data, longs);

      init_by_array(data, longs);
    }

    return cNil;
  }

  Integer* Randomizer::rand_int(STATE, Integer* max) {
    if(max->fixnum_p()) {
      native_uint max_i = max->to_native();
      native_uint result = limited_rand(max_i);
      return Integer::from(state, result);
    }

    Bignum* max_big = as<Bignum>(max);
    size_t longs = max_big->into_array(state, NULL, 0);

    uint32_t max_data[longs];
    max_big->into_array(state, max_data, longs);

    uint32_t result_data[longs];

  retry:
    uint32_t mask = 0;
    int boundary = 1;

    for(int i = longs - 1; i >= 0; i--) {
      uint32_t rnd;
      uint32_t lim = max_data[i];
      mask = mask ? 0xffffffff : make_mask(lim);
      if(mask) {
        rnd = rb_genrand_int32() & mask;
        if(boundary) {
          if(lim < rnd) goto retry;
          if(rnd < lim) boundary = 0;
        }
      } else {
        rnd = 0;
      }
      result_data[i] = rnd;
    }

    return Bignum::from_array(state, result_data, longs);
  }

  Float* Randomizer::rand_float(STATE) {
    return Float::create(state, rb_genrand_real());
  }

  /*
   * Return a random value without depending
   * on anything externally so it can be used
   * also outside the context of the VM
   */
  uint32_t Randomizer::random_uint32() {
    uint32_t seed[] = { 0, 0, 0, 0 };
    random_seed(seed, 4);
    struct random_state rng;
    random_init_array(&rng, seed, 4);
    return random_gen_uint32(&rng);
  }

}


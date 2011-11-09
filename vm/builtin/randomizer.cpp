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

/*
 * For gen_seed:
 */
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef RBX_WINDOWS
#include <malloc.h>
#include <process.h>
#endif


/*

This is based on random.c from Ruby 1.8.7, which carries the following notice:

This is based on trimmed version of MT19937.  To get the original version,
contact <http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html>.

The original copyright notice follows.

   A C-program for MT19937, with initialization improved 2002/2/10.
   Coded by Takuji Nishimura and Makoto Matsumoto.
   This is a faster version by taking Shawn Cokus's optimization,
   Matthe Bellew's simplification, Isaku Wada's real version.

   Before using, initialize the rng_state by using init_genrand(seed)
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

   3. The names of its contributors may not be used to endorse or promote
    products derived from this software without specific prior written
    permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.keio.ac.jp/matumoto/emt.html
   email: matumoto@math.keio.ac.jp
*/

/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UMASK 0x80000000UL /* most significant w-r bits */
#define LMASK 0x7fffffffUL /* least significant r bits */
#define MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
#define TWIST(u,v) ((MIXBITS(u,v) >> 1) ^ ((v)&1UL ? MATRIX_A : 0UL))


template <typename T>
static T make_mask(T x) {
  for(unsigned int i = 1; i < sizeof(T) * 8; i *= 2)
    x = x | x >> i;
  return x;
}

namespace rubinius {
  uint32_t* Randomizer::rng_data() {
    return (uint32_t*)rng_state()->raw_bytes();
  }

  /* initializes rng_state[N] with a seed */
  void Randomizer::init_genrand(uint32_t s) {
    rbx_spinlock_lock(&lock_);

    uint32_t* rng_state = rng_data();

    rng_state[0] = s;
    for(int j = 1; j < N; j++) {
      rng_state[j] = (1812433253UL * (rng_state[j-1] ^ (rng_state[j-1] >> 30)) + j);
      /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
      /* In the previous versions, MSBs of the seed affect   */
      /* only MSBs of the array rng_state[].                        */
      /* 2002/01/09 modified by Makoto Matsumoto             */
    }
    left_ = 1;

    rbx_spinlock_unlock(&lock_);
  }

  /* initialize by an array with array-length */
  /* init_key is the array for initializing keys */
  /* key_length is its length */
  /* slight change for C++, 2004/2/26 */
  void Randomizer::init_by_array(uint32_t init_key[], int key_length) {
    init_genrand(19650218UL);

    rbx_spinlock_lock(&lock_);
    uint32_t* rng_state = rng_data();

    int i = 1;
    int j = 0;
    int k = (N > key_length ? N : key_length);

    for(; k; k--) {
      rng_state[i] = (rng_state[i] ^ ((rng_state[i-1] ^ (rng_state[i-1] >> 30)) * 1664525UL))
                       + init_key[j] + j; /* non linear */
      i++;
      j++;

      if(i >= N) {
        rng_state[0] = rng_state[N-1];
        i = 1;
      }

      if(j >= key_length) j = 0;
    }

    for(k = N-1; k; k--) {
      rng_state[i] = (rng_state[i] ^ ((rng_state[i-1] ^ (rng_state[i-1] >> 30)) * 1566083941UL))
                       - i; /* non linear */
      i++;
      if(i >= N) {
        rng_state[0] = rng_state[N-1];
        i = 1;
      }
    }

    rng_state[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
    left_ = 1;
    rbx_spinlock_unlock(&lock_);
  }

  void Randomizer::next_state() {
    uint32_t* rng_state = rng_data();

    left_ = N;
    next_ = 0;

    uint32_t* p = rng_state;

    int j;

    for(j = N-M+1; --j; p++)
      *p = p[M] ^ TWIST(p[0], p[1]);

    for(j = M; --j; p++)
      *p = p[M-N] ^ TWIST(p[0], p[1]);

    *p = p[M-N] ^ TWIST(p[0], rng_state[0]);
  }

  /* generates a random number on [0,0xffffffff]-interval */
  uint32_t Randomizer::rb_genrand_int32() {
    rbx_spinlock_lock(&lock_);

    if(--left_ == 0) next_state();
    uint32_t* rng_state = rng_data();

    uint32_t y;
    y = rng_state[next_++];

    rbx_spinlock_unlock(&lock_);

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

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

  /* generates a random number on [0,1] with 53-bit resolution*/
  double Randomizer::rb_genrand_real() {
    uint32_t a = rb_genrand_int32()>>5;
    uint32_t b = rb_genrand_int32()>>6;

    return(a * 67108864.0 + b) * (1.0 / 9007199254740992.0);
  }
  /* These real versions are due to Isaku Wada, 2002/01/09 added */




  void Randomizer::init(STATE) {
    GO(randomizer).set(ontology::new_class(state,
          "Randomizer", G(object), G(rubinius)));
    G(randomizer)->set_object_type(state, RandomizerType);
  }

  Randomizer* Randomizer::create(STATE) {
    Randomizer* r = state->new_object<Randomizer>(G(randomizer));
    r->lock_ = RBX_SPINLOCK_INIT;
    r->rng_state(state, ByteArray::create(state, N * sizeof(uint32_t)));

    return r;
  }

  Randomizer* Randomizer::allocate(STATE, Object* self) {
    Randomizer* randomizer = create(state);

    if(Class* cls = try_as<Class>(self)) {
      randomizer->klass(state, cls);
    }

    randomizer->init_genrand(5489UL);

    return randomizer;
  }

  Integer* Randomizer::gen_seed(STATE) {
    static int n = 0;

    uint32_t seed[] = { 0, 0, 0, 0 };

#ifdef S_ISCHR
    int fd;
    if((fd = open("/dev/urandom", O_RDONLY
#ifdef O_NONBLOCK
        |O_NONBLOCK
#endif
#ifdef O_NOCTTY
        |O_NOCTTY
#endif
#ifdef O_NOFOLLOW
        |O_NOFOLLOW
#endif
        )) >= 0) {
      struct stat statbuf;
      if(fstat(fd, &statbuf) == 0 && S_ISCHR(statbuf.st_mode)) {
        /* If this fails, we're no worse off than if it hadn't opened to
         * begin with */
        if(read(fd, seed, 4 * sizeof(uint32_t)) < 0) {
          seed[0] = 57;
          seed[1] = 0x83221ab;
          seed[2] = 0;
          seed[3] = 0;
        }
      }
      close(fd);
    }
#endif

    struct timeval tv;
    gettimeofday(&tv, 0);
    seed[0] ^= tv.tv_usec;
    seed[1] ^= tv.tv_sec;
    seed[2] ^= getpid() ^ (n++ << 16);
    seed[3] ^= (uint32_t)(uintptr_t)&seed;

    return Bignum::from_array(state, seed, 4);
  }

  Object* Randomizer::seed(STATE, Integer* seed) {
    Bignum* big;

    size_t longs;
    uint32_t* data;

    if(!seed->fixnum_p())
      seed = as<Bignum>(seed)->abs(state);

    if(seed->fixnum_p()) {
      native_uint s = seed->to_native();

      // Remove the sign bit, for no good reason... but that's what MRI
      // does.
      s &= ~((native_uint)1 << (sizeof(native_uint) * 8 - 1));

      if(s <= 0xffffffffUL) {
        init_genrand((uint32_t)s);
        return Qnil;
      }

      longs = sizeof(native_uint) / 4;
      data = (uint32_t*)alloca(longs * 4);
      for(unsigned int i = 0; i < longs; i++)
        data[i] = (uint32_t)(s >> (i * 32));

      init_by_array(data, longs);
    } else {
      big = as<Bignum>(seed);

      longs = big->into_array(state, NULL, 0);

      data = (uint32_t*)alloca(longs * 4);
      big->into_array(state, data, longs);

      init_by_array(data, longs);
    }

    return Qnil;
  }

  Integer* Randomizer::rand_int(STATE, Integer* max) {
    if(max->fixnum_p()) {
      native_uint max_i = max->to_native();
      native_uint result = limited_rand(max_i);
      return Integer::from(state, result);
    }

    Bignum* max_big = as<Bignum>(max);
    size_t longs = max_big->into_array(state, NULL, 0);

    uint32_t* max_data = (uint32_t*)alloca(longs * 4);
    max_big->into_array(state, max_data, longs);

    uint32_t* result_data = (uint32_t*)alloca(longs * 4);

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
}


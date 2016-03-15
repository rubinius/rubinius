#include "random.h"
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

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

#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UMASK 0x80000000UL /* most significant w-r bits */
#define LMASK 0x7fffffffUL /* least significant r bits */
#define MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
#define TWIST(u,v) ((MIXBITS(u,v) >> 1) ^ ((v)&1UL ? MATRIX_A : 0UL))

void random_init_single(struct random_state* rng, uint32_t s) {
  int j;
  uint32_t* rng_state = rng->state;

  rng_state[0] = s;
  for(j = 1; j < RANDOM_N; j++) {
    rng_state[j] = (1812433253UL * (rng_state[j-1] ^ (rng_state[j-1] >> 30)) + j);
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    /* In the previous versions, MSBs of the seed affect   */
    /* only MSBs of the array rng_state[].                        */
    /* 2002/01/09 modified by Makoto Matsumoto             */
  }
  rng->left = 1;
}

void random_init_array(struct random_state* rng, uint32_t* init_key, int key_length) {
  uint32_t* rng_state = rng->state;

  random_init_single(rng, 19650218UL);

  int i = 1;
  int j = 0;
  int k = (RANDOM_N > key_length ? RANDOM_N : key_length);

  for(; k; k--) {
    rng_state[i] = (rng_state[i] ^ ((rng_state[i-1] ^ (rng_state[i-1] >> 30)) * 1664525UL))
                     + init_key[j] + j; /* non linear */
    i++;
    j++;

    if(i >= RANDOM_N) {
      rng_state[0] = rng_state[RANDOM_N-1];
      i = 1;
    }

    if(j >= key_length) j = 0;
  }

  for(k = RANDOM_N-1; k; k--) {
    rng_state[i] = (rng_state[i] ^ ((rng_state[i-1] ^ (rng_state[i-1] >> 30)) * 1566083941UL))
                     - i; /* non linear */
    i++;
    if(i >= RANDOM_N) {
      rng_state[0] = rng_state[RANDOM_N-1];
      i = 1;
    }
  }

  rng_state[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
  rng->left = 1;
}

uint32_t random_gen_uint32(struct random_state* rng) {
  uint32_t* rng_state = rng->state;

  if(--rng->left == 0) random_next_state(rng);

  uint32_t y;
  y = rng_state[rng->next++];

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);

  return y;
}

double random_gen_double(struct random_state* rng) {
  uint32_t a = random_gen_uint32(rng)>>5;
  uint32_t b = random_gen_uint32(rng)>>6;

  return(a * 67108864.0 + b) * (1.0 / 9007199254740992.0);
}

void random_next_state(struct random_state* rng) {
  uint32_t* rng_state = rng->state;

  rng->left = RANDOM_N;
  rng->next = 0;

  uint32_t* p = rng_state;

  int j;

  for(j = RANDOM_N-M+1; --j; p++)
    *p = p[M] ^ TWIST(p[0], p[1]);

  for(j = M; --j; p++)
    *p = p[M-RANDOM_N] ^ TWIST(p[0], p[1]);

  *p = p[M-RANDOM_N] ^ TWIST(p[0], rng_state[0]);
}

void random_seed(uint32_t* seed, int len) {
  static int n;
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
      if(read(fd, seed, len * sizeof(uint32_t)) < 0) {
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
}

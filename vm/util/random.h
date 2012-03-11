#ifndef RBX_UTIL_RANDOM_H
#define RBX_UTIL_RANDOM_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

#define RANDOM_N 624

struct random_state {
  uint32_t state[RANDOM_N];
  int next;
  int left;
};

void random_init_single(struct random_state* rng, uint32_t seed);
void random_init_array(struct random_state* rng, uint32_t* seed_data, int seed_size);
void random_next_state(struct random_state* rng);
void random_seed(uint32_t* seed, int len);

uint32_t random_gen_uint32(struct random_state* rng);
double   random_gen_double(struct random_state* rng);

#if defined(__cplusplus)
}
#endif
#endif



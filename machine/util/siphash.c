#include "siphash.h"


static inline uint64_t rotl64(uint64_t u, int s)
{
        return (u << s) | (u >> (64 - s));
}


#define sipround() do {         \
        v0 += v1;               \
        v1 = rotl64(v1, 13);    \
        v1 ^= v0;               \
        v0 = rotl64(v0, 32);    \
                                \
        v2 += v3;               \
        v3 = rotl64(v3, 16);    \
        v3 ^= v2;               \
                                \
        v2 += v1;               \
        v1 = rotl64(v1, 17);    \
        v1 ^= v2;               \
        v2 = rotl64(v2, 32);    \
                                \
        v0 += v3;               \
        v3 = rotl64(v3, 21);    \
        v3 ^= v0;               \
    } while (0)


#define sipcompress2(m) do {    \
        v3 ^= m;                \
                                \
        sipround();             \
        sipround();             \
                                \
        v0 ^= m;                \
    } while (0)


static inline uint64_t get64le(void const* data, size_t ix)
{
        uint8_t const* p = (uint8_t const*)data + ix * 8;

        return (uint64_t)p[0] << (0 * 8) |
               (uint64_t)p[1] << (1 * 8) |
               (uint64_t)p[2] << (2 * 8) |
               (uint64_t)p[3] << (3 * 8) |
               (uint64_t)p[4] << (4 * 8) |
               (uint64_t)p[5] << (5 * 8) |
               (uint64_t)p[6] << (6 * 8) |
               (uint64_t)p[7] << (7 * 8);
}


static inline uint8_t get8(void const* data, size_t ix)
{
        return *((uint8_t const*)data + ix);
}


static inline uint64_t siplast(void const* data, size_t size)
{
        uint64_t last = 0;
        size_t i;

        for(i = 0; i < size % 8; ++i) {
                last |= (uint64_t)get8(data, size / 8 * 8 + i) << (i * 8);
        }
        last |= ((uint64_t)(size % 0xff)) << (7 * 8);

        return last;
}


uint64_t siphash24(uint64_t key0, uint64_t key1, void const* data, size_t size)
{
        uint64_t v0 = key0 ^ 0x736f6d6570736575ull;
        uint64_t v1 = key1 ^ 0x646f72616e646f6dull;
        uint64_t v2 = key0 ^ 0x6c7967656e657261ull;
        uint64_t v3 = key1 ^ 0x7465646279746573ull;
        size_t i = 0;

        for(i = 0; i < size / 8; ++i) {
                uint64_t m = get64le(data, i);
                sipcompress2(m);
        }
        uint64_t m = siplast(data, size);
        sipcompress2(m);

        v2 ^= 0xff;

        sipround();
        sipround();
        sipround();
        sipround();

        return v0 ^ v1 ^ v2 ^ v3;
}


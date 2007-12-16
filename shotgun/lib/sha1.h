#ifndef RBS_SHA1_H
#define RBS_SHA1_H

#include <sys/types.h>
#include <stdint.h>
#include <bstrlib.h>

typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    unsigned char buffer[64];
} SHA1_CTX;

void SHA1Transform(uint32_t state[5], unsigned char buffer[64]);
void SHA1Init(SHA1_CTX* context);
void SHA1Update(SHA1_CTX* context, unsigned char* data, unsigned int len);
void SHA1Final(unsigned char digest[20], SHA1_CTX* context);

void sha1_hash_bstring(bstring input, unsigned char *digest);
void sha1_hash_string(unsigned char *input, int len, unsigned char *digest);

#endif

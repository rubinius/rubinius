#ifndef __SIPHASH_H__
#define __SIPHASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>


uint64_t siphash24(uint64_t key0, uint64_t key1, void const* data, size_t size);

#ifdef __cplusplus
}
#endif


#endif


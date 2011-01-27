/*
 * SHA-1 in C
 * By Steve Reid <steve@edmweb.com>
 * 100% Public Domain
 */

#ifndef _SYS_XSHA1_H_
#define	_SYS_XSHA1_H_

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint32_t state[5];
	uint32_t count[2];
	uint8_t buffer[64];
} XSHA1_CTX;

void	XSHA1_Transform(uint32_t state[5], const uint8_t buffer[64]);
void	XSHA1_Init(XSHA1_CTX *context);
void	XSHA1_Update(XSHA1_CTX *context, const uint8_t *data, size_t len);
void	XSHA1_Finish(XSHA1_CTX *context, uint8_t digest[20]);

#define XSHA1_BLOCK_LENGTH		64
#define XSHA1_DIGEST_LENGTH		20
#define XSHA1_DIGEST_STRING_LENGTH	(XSHA1_DIGEST_LENGTH * 2 + 1)


#ifdef __cplusplus
}
#endif

#endif /* _SYS_XSHA1_H_ */

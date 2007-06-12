#ifndef RBS_COMPRESS
#define RBS_COMPRESS 1

OBJECT compress_deflate(STATE, unsigned char* in);
OBJECT compress_inflate(STATE, unsigned char* in);

#endif

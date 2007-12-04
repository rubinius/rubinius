#ifndef RBS_COMPRESS_H
#define RBS_COMPRESS_H

OBJECT compress_deflate(STATE, unsigned char* in);
OBJECT compress_inflate(STATE, unsigned char* in);

#endif

#include "shotgun.h"
#include "string.h"
#include "compress.h"

#include <bstrlib.h>
#include <zlib.h>

#define ZLIB_CHUNK_SIZE 512

OBJECT compress_inflate(STATE, unsigned char* in) {
  unsigned char out_buffer[ZLIB_CHUNK_SIZE];
  bstring output = NULL;
  int status, size;
  OBJECT out;

  z_stream zs;
  zs.zfree = Z_NULL;
  zs.zalloc = Z_NULL;
  zs.opaque = Z_NULL;
  zs.avail_in = strlen((char*)in) + 1; // termination is added by bstring.
  zs.next_in = in;

  status = inflateInit(&zs); // Returns zlib error code
  if(status != Z_OK) {
    inflateEnd(&zs);
    return Qnil;
  }

  output = bfromcstralloc(zs.avail_in, ""); 
  do {
    zs.avail_out = ZLIB_CHUNK_SIZE;
    zs.next_out = out_buffer;
    status = inflate(&zs, Z_SYNC_FLUSH);
    size = ZLIB_CHUNK_SIZE - zs.avail_out; // How much we got.
    switch(status) {
      case Z_OK:
        // Fall through
      case Z_STREAM_END:
        bcatblk(output, out_buffer, size);
        break;
      default: // Punt on any other return value.
        inflateEnd(&zs);
		bdestroy(output);
        return Qnil;
      }
  } while(zs.avail_out == 0);

  inflateEnd(&zs);

  if(status != Z_STREAM_END && status != Z_OK) {
    bdestroy(output);
    return Qnil;
  }
  out = string_newfrombstr(state, output);
  bdestroy(output);
  return out;
}

OBJECT compress_deflate(STATE, unsigned char* in) {
  unsigned char out_buffer[ZLIB_CHUNK_SIZE];
  bstring output = NULL;
  int size, status;
  OBJECT out;
  
  z_stream zs;
  zs.zfree = Z_NULL;
  zs.zalloc = Z_NULL;
  zs.opaque = Z_NULL;

  status = deflateInit(&zs, Z_DEFAULT_COMPRESSION); // Returns zlib error code
  if (status != Z_OK) {
    deflateEnd(&zs);
    return Qnil;
  }

  zs.avail_in = strlen((char *)in); // Lower than for zlib_inflate, so that we don't consume the zero-terminator.  [and appease emacs' syntax parser]
  zs.next_in = in;
  output = bfromcstralloc(zs.avail_in, "");
  do {
    zs.avail_out = ZLIB_CHUNK_SIZE;
    zs.next_out = out_buffer;
    status = deflate(&zs, Z_FINISH);
    size = ZLIB_CHUNK_SIZE - zs.avail_out; // How much we got.
    switch (status) {
      case Z_OK:
        // Fall through
      case Z_STREAM_END:
        bcatblk(output, out_buffer, size);
        break;
      default: // Punt on any other return value.
        deflateEnd(&zs);
        bdestroy(output);
        return Qnil;
      }
  } while(zs.avail_out == 0);

  deflateEnd(&zs);

  if(status != Z_STREAM_END && status != Z_OK) {
    bdestroy(output);
    return 0;
  }
  out = string_newfrombstr(state, output);
  bdestroy(output);
  return out;
}

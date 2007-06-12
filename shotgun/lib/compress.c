#include "shotgun.h"
#include "string.h"
#include "compress.h"

#include <glib.h>
#include <zlib.h>

#define ZLIB_CHUNK_SIZE 512

OBJECT compress_inflate(STATE, unsigned char* in) {
  unsigned char out_buffer[ZLIB_CHUNK_SIZE];
  GString *output = g_string_new(NULL);
  int status, size;
  OBJECT out;

  z_stream zs;
  zs.zfree = Z_NULL;
  zs.zalloc = Z_NULL;
  zs.opaque = Z_NULL;
  zs.avail_in = strlen((char*)in) + 1; // termination is added by GString.
  zs.next_in = in;

  status = inflateInit(&zs); // Returns zlib error code
  if(status != Z_OK) {
    inflateEnd(&zs);
    g_string_free(output, TRUE);
    return Qnil;
  }

  do {
    zs.avail_out = ZLIB_CHUNK_SIZE;
    zs.next_out = out_buffer;
    status = inflate(&zs, Z_SYNC_FLUSH);
    size = ZLIB_CHUNK_SIZE - zs.avail_out; // How much we got.
    switch(status) {
      case Z_OK:
        // Fall through
      case Z_STREAM_END:
        g_string_append_len(output, (char*)out_buffer, size);
        break;
      default: // Punt on any other return value.
        inflateEnd(&zs);
        g_string_free(output, TRUE);
        return Qnil;
      }
  } while(zs.avail_out == 0);

  inflateEnd(&zs);

  if(status != Z_STREAM_END && status != Z_OK) {
    g_string_free(output, TRUE);
    return Qnil;
  }
  out = string_new2(state, output->str, output->len);
  g_string_free(output, TRUE);
  return out;
}

OBJECT compress_deflate(STATE, unsigned char* in) {
  unsigned char out_buffer[ZLIB_CHUNK_SIZE];
  GString *output = g_string_new(NULL);
  int size, status;
  OBJECT out;
  
  z_stream zs;
  zs.zfree = Z_NULL;
  zs.zalloc = Z_NULL;
  zs.opaque = Z_NULL;

  status = deflateInit(&zs, Z_DEFAULT_COMPRESSION); // Returns zlib error code
  if (status != Z_OK) {
    deflateEnd(&zs);
    g_string_free(output, TRUE);
    return Qnil;
  }

  zs.avail_in = strlen((char *)in); // Lower than for zlib_inflate, so that we don't consume the zero-terminator.  [and appease emacs' syntax parser]
  zs.next_in = in;
  do {
    zs.avail_out = ZLIB_CHUNK_SIZE;
    zs.next_out = out_buffer;
    status = deflate(&zs, Z_FINISH);
    size = ZLIB_CHUNK_SIZE - zs.avail_out; // How much we got.
    switch (status) {
      case Z_OK:
        // Fall through
      case Z_STREAM_END:
        g_string_append_len(output, (char *)out_buffer, size);
        break;
      default: // Punt on any other return value.
        deflateEnd(&zs);
        g_string_free(output, TRUE);
        return Qnil;
      }
  } while(zs.avail_out == 0);

  deflateEnd(&zs);

  if(status != Z_STREAM_END && status != Z_OK) {
    g_string_free(output, TRUE);
    return 0;
  }
  out = string_new2(state, output->str, output->len);
  g_string_free(output, TRUE);
  return out;
}

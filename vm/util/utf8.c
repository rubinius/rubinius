#include <sys/types.h>
#include <stdint.h>

/**
 * Based on the implementation discussed here:
 *
 * http://www.daemonology.net/blog/2008-06-05-faster-utf8-strlen.html
 *
 * This code has been placed in the public domain by Colin Percival,
 * the author of the blog post.
 *
 * This is adapted to the usage in Rubinius where strings are not
 * null terminated, but have a known length. This also uses a slightly
 * easier check for the non aligned characters at the beginning
 * and end.
 *
 * The index transformation functions are used by Rubinius for cases
 * such as substringing. This works because the start can point at any
 * point in the underlying storage.
 *
 * These could be improved by using SIMD instructions to work on larger
 * blocks than the current word sized blocks. This could be used to
 * process 128 bits or with AVX even 256 bits at once. This should give
 * a fairly linear performance improvement for large UTF-8 strings.
 *
 * An example of using SIMD instructions is discussed here:
 *
 * http://woboq.com/blog/utf-8-processing-using-simd.html
 */

#define UTF8_MASK 0xC0
#define UTF8_NON_START 0x80
#define UTF8_START_OF_CHAR(p) ((p & UTF8_MASK) != UTF8_NON_START)
#define UTF8_SKIP_NON_START_CHARACTERS(p, e) { while(!UTF8_START_OF_CHAR(*p) && p < e) ++p; }
#define UTF8_SKIP_NON_START_CHARACTERS_WITH_INDEX(p, e, i) { while(!UTF8_START_OF_CHAR(*p) && p < e) { ++p; --i; } }

#define UTF8_ONES_MASK ((uintptr_t)(-1) / 0xFF)
#define UTF8_WORD_NON_START UTF8_ONES_MASK * 0x80

static inline ssize_t utf8_count_non_start_bytes_in_word(uintptr_t w) {
  w = ((w & (UTF8_WORD_NON_START)) >> 7) & ((~w) >> 6);
  return (w * UTF8_ONES_MASK) >> ((sizeof(uintptr_t) - 1) * 8);
}

ssize_t utf8_find_byte_character_index(const uint8_t* start, const uint8_t* end, ssize_t index) {
  uint8_t* p = (uint8_t*) start;
  ssize_t char_index = 0;

  while((uintptr_t)(p) & (sizeof(uintptr_t) - 1) && p < end && index > 0) {
    ++p;
    --index;
    ++char_index;
    UTF8_SKIP_NON_START_CHARACTERS_WITH_INDEX(p, end, index);
  }

  UTF8_SKIP_NON_START_CHARACTERS_WITH_INDEX(p, end, index);
  while(p < (end - sizeof(uintptr_t)) && index > (ssize_t)sizeof(uintptr_t)) {
    uintptr_t w = *(uintptr_t*)(p);
    p += sizeof(uintptr_t);
    index -= sizeof(uintptr_t);
    char_index += sizeof(uintptr_t) - utf8_count_non_start_bytes_in_word(w);
  }

  UTF8_SKIP_NON_START_CHARACTERS_WITH_INDEX(p, end, index);
  while(p < end && index > 0) {
    ++p;
    --index;
    ++char_index;
    UTF8_SKIP_NON_START_CHARACTERS_WITH_INDEX(p, end, index);
  }

  return char_index;
}

ssize_t utf8_find_character_byte_index(const uint8_t* start, const uint8_t* end, ssize_t index) {
  uint8_t* p = (uint8_t*) start;

  while((uintptr_t)(p) & (sizeof(uintptr_t) - 1) && p < end && index > 0) {
    ++p;
    --index;
    UTF8_SKIP_NON_START_CHARACTERS(p, end);
  }

  UTF8_SKIP_NON_START_CHARACTERS(p, end);
  while(p < (end - sizeof(uintptr_t)) && index > (ssize_t)sizeof(uintptr_t)) {
    uintptr_t w = *(uintptr_t*)(p);
    p += sizeof(uintptr_t);
    index -= sizeof(uintptr_t) - utf8_count_non_start_bytes_in_word(w);
  }

  UTF8_SKIP_NON_START_CHARACTERS(p, end);
  while(p < end && index > 0) {
    ++p;
    --index;
    UTF8_SKIP_NON_START_CHARACTERS(p, end);
  }

  if(p > end) p = (uint8_t*) end;
  return p - start;
}

ssize_t utf8_string_length(const uint8_t* start, const uint8_t* end) {

  ssize_t non_start_count = 0;
  const uint8_t* p = start;

  while((uintptr_t)(p) & (sizeof(uintptr_t) - 1) && p < end) {
    non_start_count += !UTF8_START_OF_CHAR(*p);
    ++p;
  }

  while(p < (end - sizeof(uintptr_t))) {
    uintptr_t w = *(uintptr_t*)(p);
    p += sizeof(uintptr_t);
    non_start_count += utf8_count_non_start_bytes_in_word(w);
  }

  while(p < end) {
    non_start_count += !UTF8_START_OF_CHAR(*p);
    ++p;
  }

  return end - start - non_start_count;
}

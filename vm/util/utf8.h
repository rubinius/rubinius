#ifndef UTF8_H
#define UTF8_H

#if defined(__cplusplus)
extern "C" {
#endif

ssize_t utf8_find_byte_character_index(const uint8_t* start, const uint8_t* end, ssize_t index);
ssize_t utf8_find_character_byte_index(const uint8_t* start, const uint8_t* end, ssize_t index);
ssize_t utf8_string_length(const uint8_t* start, const uint8_t* end);

#if defined(__cplusplus)
}
#endif

#endif

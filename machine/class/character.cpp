#include "oniguruma.h" // Must be first.
#include "regenc.h"
#include "transcoder.h"

#include "object_utils.hpp"
#include "memory.hpp"

#include "class/byte_array.hpp"
#include "class/character.hpp"
#include "class/class.hpp"
#include "class/encoding.hpp"
#include "class/object.hpp"

namespace rubinius {
  void Character::bootstrap(STATE) {
    GO(character).set(state->memory()->new_class<Class, Character>(
          state, G(string), G(rubinius), "Character"));
  }

  Character* Character::allocate(STATE, Object* self) {
    return state->memory()->new_object<Character>(state, as<Class>(self));
  }

  Character* Character::create(STATE, intptr_t size) {
    Character* chr = Character::allocate(state, G(character));

    chr->num_bytes(state, Fixnum::from(size));
    chr->num_chars(state, nil<Fixnum>());
    chr->hash_value(state, nil<Fixnum>());
    chr->shared(state, cFalse);
    chr->encoding(state, nil<Encoding>());
    chr->ascii_only(state, cNil);
    chr->valid_encoding(state, cNil);

    ByteArray* ba = ByteArray::create(state, size + 1);
    ba->raw_bytes()[size] = 0;

    chr->data(state, ba);

    return chr;
  }

  Character* Character::create(STATE, const char* bytes, intptr_t size) {
    Character* chr = Character::create(state, size);

    if(bytes) memcpy(chr->byte_address(), bytes, size);

    return chr;
  }

  Character* Character::create_from(STATE, String* str, Fixnum* byte) {
    intptr_t i = byte->to_native();
    intptr_t size = str->byte_size();

    if(i < 0 || i >= size) return nil<Character>();

    OnigEncodingType* enc = str->encoding(state)->encoding();
    uint8_t* p = str->byte_address() + i;
    uint8_t* e = str->byte_address() + str->byte_size();

    int c = Encoding::precise_mbclen(p, e, enc);

    if(!ONIGENC_MBCLEN_CHARFOUND_P(c)) return nil<Character>();

    int n = ONIGENC_MBCLEN_CHARFOUND_LEN(c);
    if(i + n > size) return nil<Character>();

    Character* chr = Character::create(state, (const char*)p, n);
    str->infect(state, chr);
    chr->encoding_from(state, str);

    return chr;
  }

  Object* Character::alphabetical_p(STATE) {
    bool found;
    int c = codepoint(state, &found);

    OnigEncodingType* enc = encoding()->encoding();
    return RBOOL(found && ONIGENC_IS_CODE_ALPHA(enc, c));
  }

  Object* Character::ascii_p(STATE) {
    bool found;
    int c = codepoint(state, &found);

    return RBOOL(found && ONIGENC_IS_CODE_ASCII(c));
  }

  Object* Character::digit_p(STATE) {
    bool found;
    int c = codepoint(state, &found);

    OnigEncodingType* enc = encoding()->encoding();
    return RBOOL(found && ONIGENC_IS_CODE_DIGIT(enc, c));
  }

  Object* Character::lower_p(STATE) {
    bool found;
    int c = codepoint(state, &found);

    OnigEncodingType* enc = encoding()->encoding();
    return RBOOL(found && ONIGENC_IS_CODE_LOWER(enc, c));
  }

  Object* Character::printable_p(STATE) {
    bool found;
    int c = codepoint(state, &found);

    OnigEncodingType* enc = encoding()->encoding();
    return RBOOL(found && ONIGENC_IS_CODE_PRINT(enc, c));
  }

  Object* Character::punctuation_p(STATE) {
    bool found;
    int c = codepoint(state, &found);

    OnigEncodingType* enc = encoding()->encoding();
    return RBOOL(found && ONIGENC_IS_CODE_PUNCT(enc, c));
  }

  Object* Character::space_p(STATE) {
    bool found;
    int c = codepoint(state, &found);

    OnigEncodingType* enc = encoding()->encoding();
    return RBOOL(found && ONIGENC_IS_CODE_SPACE(enc, c));
  }

  Object* Character::upper_p(STATE) {
    bool found;
    int c = codepoint(state, &found);

    OnigEncodingType* enc = encoding()->encoding();
    return RBOOL(found && ONIGENC_IS_CODE_UPPER(enc, c));
  }
}

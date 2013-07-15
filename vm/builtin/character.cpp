#include "oniguruma.h" // Must be first.
#include "regenc.h"
#include "transcoder.h"

#include "builtin/bytearray.hpp"
#include "builtin/character.hpp"
#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/object.hpp"
#include "object_utils.hpp"
#include "ontology.hpp"

namespace rubinius {
  void Character::init(STATE) {
    GO(character).set(ontology::new_class(state, "Character", G(string), G(rubinius)));
    G(character)->set_object_type(state, CharacterType);
  }

  Character* Character::allocate(STATE, Object* self) {
    Character* chr = state->new_object<Character>(G(character));
    chr->klass(state, as<Class>(self));
    return chr;
  }

  Character* Character::create(STATE, native_int size) {
    Character* chr = state->new_object_dirty<Character>(G(character));

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

  Character* Character::create(STATE, const char* bytes, native_int size) {
    Character* chr = Character::create(state, size);

    if(bytes) memcpy(chr->byte_address(), bytes, size);

    return chr;
  }

  Character* Character::create_from(STATE, String* str, Fixnum* byte) {
    native_int i = byte->to_native();
    native_int size = str->byte_size();

    if(i < 0 || i >= size) return nil<Character>();

    OnigEncodingType* enc = str->encoding(state)->get_encoding();
    uint8_t* p = str->byte_address() + i;
    uint8_t* e = str->byte_address() + str->byte_size();

    int c = Encoding::precise_mbclen(p, e, enc);

    if(!ONIGENC_MBCLEN_CHARFOUND_P(c)) return nil<Character>();

    int n = ONIGENC_MBCLEN_CHARFOUND_LEN(c);
    if(i + n > size) return nil<Character>();

    Character* chr = Character::create(state, (const char*)p, n);
    chr->encoding(state, str->encoding());
    chr->ascii_only(state, str->ascii_only());
    chr->valid_encoding(state, str->valid_encoding());

    return chr;
  }

  Object* Character::alphabetical_p(STATE) {
    bool found;
    int c = codepoint(state, &found);

    OnigEncodingType* enc = encoding()->get_encoding();
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

    OnigEncodingType* enc = encoding()->get_encoding();
    return RBOOL(found && ONIGENC_IS_CODE_DIGIT(enc, c));
  }

  Object* Character::lower_p(STATE) {
    bool found;
    int c = codepoint(state, &found);

    OnigEncodingType* enc = encoding()->get_encoding();
    return RBOOL(found && ONIGENC_IS_CODE_LOWER(enc, c));
  }

  Object* Character::printable_p(STATE) {
    bool found;
    int c = codepoint(state, &found);

    OnigEncodingType* enc = encoding()->get_encoding();
    return RBOOL(found && ONIGENC_IS_CODE_PRINT(enc, c));
  }

  Object* Character::punctuation_p(STATE) {
    bool found;
    int c = codepoint(state, &found);

    OnigEncodingType* enc = encoding()->get_encoding();
    return RBOOL(found && ONIGENC_IS_CODE_PUNCT(enc, c));
  }

  Object* Character::space_p(STATE) {
    bool found;
    int c = codepoint(state, &found);

    OnigEncodingType* enc = encoding()->get_encoding();
    return RBOOL(found && ONIGENC_IS_CODE_SPACE(enc, c));
  }

  Object* Character::upper_p(STATE) {
    bool found;
    int c = codepoint(state, &found);

    OnigEncodingType* enc = encoding()->get_encoding();
    return RBOOL(found && ONIGENC_IS_CODE_UPPER(enc, c));
  }
}

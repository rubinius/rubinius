#ifndef RBX_CHARACTER_HPP
#define RBX_CHARACTER_HPP

#include "builtin/string.hpp"

namespace rubinius {
  class Character : public String {
  public:
    const static object_type type = CharacterType;

    static void init(STATE);

    // Rubinius.primitive :character_allocate
    static Character* allocate(STATE, Object* self);

    static Character* create(STATE, native_int size);
    static Character* create(STATE, const char* bytes, native_int size);
    static Character* create_from(STATE, String* str, Fixnum* byte);

    // Rubinius.primitive+ :character_alphabetical_p
    Object* alphabetical_p(STATE);

    // Rubinius.primitive+ :character_ascii_p
    Object* ascii_p(STATE);

    // Rubinius.primitive+ :character_digit_p
    Object* digit_p(STATE);

    // Rubinius.primitive+ :character_lower_p
    Object* lower_p(STATE);

    // Rubinius.primitive+ :character_printable_p
    Object* printable_p(STATE);

    // Rubinius.primitive+ :character_punctuation_p
    Object* punctuation_p(STATE);

    // Rubinius.primitive+ :character_space_p
    Object* space_p(STATE);

    // Rubinius.primitive+ :character_upper_p
    Object* upper_p(STATE);

    class Info : public String::Info {
    public:
      BASIC_TYPEINFO(String::Info)
    };
  };
};

#endif

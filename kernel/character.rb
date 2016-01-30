module Rubinius
  class Character < ::String

    def self.__allocate__
      Rubinius.primitive :character_allocate
      raise PrimitiveFailure, "Rubinius::Character.allocate primitive failed"
    end

    def self.allocate
      chr = __allocate__
      chr.__data__ = Rubinius::ByteArray.allocate_sized(1)
      chr.num_bytes = 0
      chr
    end

    def alphabetical?
      Rubinius.primitive :character_alphabetical_p
      raise PrimitiveFailure, "Rubinius::Character#alphabetical? primitive failed"
    end

    def ascii?
      Rubinius.primitive :character_ascii_p
      raise PrimitiveFailure, "Rubinius::Character#ascii? primitive failed"
    end

    def digit?
      Rubinius.primitive :character_digit_p
      raise PrimitiveFailure, "Rubinius::Character#digit? primitive failed"
    end

    def lower?
      Rubinius.primitive :character_lower_p
      raise PrimitiveFailure, "Rubinius::Character#lower? primitive failed"
    end

    def printable?
      Rubinius.primitive :character_printable_p
      raise PrimitiveFailure, "Rubinius::Character#printable? primitive failed"
    end

    def punctuation?
      Rubinius.primitive :character_punctuation_p
      raise PrimitiveFailure, "Rubinius::Character#punctuation? primitive failed"
    end

    def space?
      Rubinius.primitive :character_space_p
      raise PrimitiveFailure, "Rubinius::Character#space? primitive failed"
    end

    def upper?
      Rubinius.primitive :character_upper_p
      raise PrimitiveFailure, "Rubinius::Character#upper? primitive failed"
    end
  end
end

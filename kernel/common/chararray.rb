##
# An encoding-aware, fixed-size vector of bytes used to implement String.

module Rubinius
  class CharArray
    alias_method :[], :get_byte
    alias_method :[]=, :set_byte

    def each
      i = 0
      s = size
      while i < s
        yield get_byte(i)
        i += 1
      end
    end

    def inspect
      "#<#{self.class}:0x#{object_id.to_s(16)} #{size} bytes>"
    end

    def <=>(other)
      compare_bytes(other, size, other.size)
    end

    # Sets the first character to be an ASCII capitalize letter
    # if it's an ASCII lower case letter
    def first_capitalize!
      self[0] = self[0].toupper
    end
  end
end

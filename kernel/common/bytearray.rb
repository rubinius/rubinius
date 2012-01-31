# -*- encoding: us-ascii -*-

##
# An array of bytes, used as a low-level data store for implementing various
# other classes.

module Rubinius
  class ByteArray
    alias_method :[], :get_byte
    alias_method :[]=, :set_byte

    def each
      i = 0
      max = size()

      while i < max
        yield get_byte(i)
        i += 1
      end
    end

    def inspect
      "#<#{self.class}:0x#{object_id.to_s(16)} #{size} bytes>"
    end

    def <=>(other)
      compare_bytes other, size, other.size
    end

    # Sets the first character to be an ASCII capitalize letter
    # if it's an ASCII lower case letter
    def first_capitalize!
      self[0] = self[0].toupper
    end
  end
end

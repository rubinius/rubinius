module Rubinius
  class CharArray
    def self.allocate
      raise TypeError, "CharArray cannot be created via allocate()"
    end

    def self.allocate_sized(cnt)
      Rubinius.primitive :chararray_allocate
      raise PrimitiveFailure, "CharArray#allocate primitive failed"
    end

    def self.new(cnt)
      obj = allocate_sized cnt
      Rubinius.asm(obj) do |obj|
        push_block
        run obj
        send_with_block :initialize, 0, true
      end

      return obj
    end

    def fetch_bytes(start, count)
      Rubinius.primitive :chararray_fetch_bytes
      raise PrimitiveFailure, "CharArray#fetch_bytes primitive failed"
    end

    def move_bytes(start, count, dest)
      Rubinius.primitive :chararray_move_bytes
      raise ArgumentError, "CharArray#move_bytes primitive failed"
    end

    def get_byte(index)
      Rubinius.primitive :chararray_get_byte
      raise PrimitiveFailure, "CharArray#get_byte primitive failed"
    end

    def set_byte(index, value)
      Rubinius.primitive :chararray_set_byte
      raise PrimitiveFailure, "CharArray#set_byte primitive failed"
    end

    def compare_bytes(other, a, b)
      Rubinius.primitive :chararray_compare_bytes
      raise PrimitiveFailure, "CharArray#compare_bytes primitive failed"
    end

    def size
      Rubinius.primitive :chararray_size
      raise PrimitiveFailure, "CharArray#size primitive failed"
    end

    def dup(cls=nil)
      cls ||= self.class
      obj = cls.new(self.size)

      Rubinius.invoke_primitive :object_copy_object, obj, self

      Rubinius.privately do
        obj.initialize_copy self
      end

      return obj
    end

    ##
    # Searches for +pattern+ in the CharArray. Returns the number
    # of characters from the front of the CharArray to the end
    # of the pattern if a match is found. Returns Qnil if a match
    # is not found. Starts searching at index +start+.
    def locate(pattern, start, max)
      Rubinius.primitive :chararray_locate
      raise PrimitiveFailure, "CharArray#locate primitive failed"
    end

    # Return a new CharArray by taking the bytes from +string+ and +self+
    # together.
    def prepend(string)
      Rubinius.primitive :chararray_prepend

      if string.kind_of? String
        raise PrimitiveFailure, "CharArray#prepend failed"
      else
        prepend(StringValue(string))
      end
    end

    def utf8_char(offset)
      Rubinius.primitive :chararray_get_utf8_char
      raise ArgumentError, "unable to extract utf8 character"
    end

    def reverse(start, total)
      Rubinius.primitive :chararray_reverse
      raise PrimitiveFailure, "CharArray#reverse primitive failed"
    end
  end
end

module Rubinius
  class ByteArray
    def self.allocate
      raise TypeError, "ByteArray cannot be created via allocate()"
    end

    def self.allocate_sized(cnt)
      Ruby.primitive :bytearray_allocate
      raise PrimitiveFailure, "ByteArray#allocate primitive failed"
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
      Ruby.primitive :bytearray_fetch_bytes
      raise PrimitiveFailure, "ByteArray#fetch_bytes primitive failed"
    end

    def move_bytes(start, count, dest)
      Ruby.primitive :bytearray_move_bytes
      raise ArgumentError, "ByteArray#move_bytes primitive failed"
    end

    def get_byte(index)
      Ruby.primitive :bytearray_get_byte
      raise PrimitiveFailure, "ByteArray#get_byte primitive failed"
    end

    def set_byte(index, value)
      Ruby.primitive :bytearray_set_byte
      raise PrimitiveFailure, "ByteArray#set_byte primitive failed"
    end

    def compare_bytes(other, a, b)
      Ruby.primitive :bytearray_compare_bytes
      raise PrimitiveFailure, "ByteArray#compare_bytes primitive failed"
    end

    def size
      Ruby.primitive :bytearray_size
      raise PrimitiveFailure, "ByteArray#size primitive failed"
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
    # Searches for +pattern+ in the ByteArray. Returns the number
    # of characters from the front of the ByteArray to the end
    # of the pattern if a match is found. Returns Qnil if a match
    # is not found. Starts searching at index +start+.
    def locate(pattern, start, max)
      Ruby.primitive :bytearray_locate
      raise PrimitiveFailure, "ByteArray#locate primitive failed"
    end

    # Return a new ByteArray by taking the bytes from +string+ and +self+
    # together.
    def prepend(string)
      Ruby.primitive :bytearray_prepend

      if string.kind_of? String
        raise PrimitiveFailure, "ByteArray#prepend failed"
      else
        prepend(StringValue(string))
      end
    end

    def utf8_char(offset)
      Ruby.primitive :bytearray_get_utf8_char
      raise ArgumentError, "unable to extract utf8 character"
    end

    def reverse(start, total)
      Ruby.primitive :bytearray_reverse
      raise PrimitiveFailure, "ByteArray#reverse primitive failed"
    end
  end
end

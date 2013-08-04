# -*- encoding: us-ascii -*-

module Rubinius
  class ByteArray
    def self.allocate
      raise TypeError, "ByteArray cannot be created via allocate()"
    end

    def self.allocate_sized(cnt)
      Rubinius.primitive :bytearray_allocate
      raise PrimitiveFailure, "ByteArray#allocate primitive failed"
    end

    def self.new(cnt)
      obj = allocate_sized cnt
      Rubinius.asm(obj) do |obj|
        run obj
        send :initialize, 0, true
      end

      obj
    end

    def fetch_bytes(start, count)
      Rubinius.primitive :bytearray_fetch_bytes
      raise PrimitiveFailure, "ByteArray#fetch_bytes primitive failed"
    end

    def move_bytes(start, count, dest)
      Rubinius.primitive :bytearray_move_bytes
      raise ArgumentError, "ByteArray#move_bytes primitive failed"
    end

    def get_byte(index)
      Rubinius.primitive :bytearray_get_byte
      raise PrimitiveFailure, "ByteArray#get_byte primitive failed"
    end

    def set_byte(index, value)
      Rubinius.primitive :bytearray_set_byte
      raise PrimitiveFailure, "ByteArray#set_byte primitive failed"
    end

    def compare_bytes(other, a, b)
      Rubinius.primitive :bytearray_compare_bytes
      raise PrimitiveFailure, "ByteArray#compare_bytes primitive failed"
    end

    def size
      Rubinius.primitive :bytearray_size
      raise PrimitiveFailure, "ByteArray#size primitive failed"
    end

    def dup
      obj = Rubinius::Type.object_class(self).allocate_sized(self.size)

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
      Rubinius.primitive :bytearray_locate
      raise PrimitiveFailure, "ByteArray#locate primitive failed"
    end

    # Return a new ByteArray by taking the bytes from +string+ and +self+
    # together.
    def prepend(string)
      Rubinius.primitive :bytearray_prepend

      if string.kind_of? String
        raise PrimitiveFailure, "ByteArray#prepend failed"
      else
        prepend(StringValue(string))
      end
    end

    def reverse(start, total)
      Rubinius.primitive :bytearray_reverse
      raise PrimitiveFailure, "ByteArray#reverse primitive failed"
    end
  end
end

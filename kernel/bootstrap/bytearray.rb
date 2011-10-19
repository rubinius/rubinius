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
        push_block
        run obj
        send_with_block :initialize, 0, true
      end

      return obj
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

    def dup(cls=nil)
      cls ||= self.class
      obj = cls.new(self.size)

      Rubinius.invoke_primitive :object_copy_object, obj, self

      Rubinius.privately do
        obj.initialize_copy self
      end

      return obj
    end
  end
end

module Rubinius
  class Mirror
    class String < Mirror
      subject = ::String

      def character_to_byte_index(idx, start=0)
        Rubinius.invoke_primitive :string_character_byte_index, @object, idx, start
      end

      def byte_to_character_index(idx, start=0)
        Rubinius.invoke_primitive :string_byte_character_index, @object, idx, start
      end

      def character_index(str, start)
        Rubinius.invoke_primitive :string_character_index, @object, str, start
      end

      def byte_index(value, start=0)
        Rubinius.invoke_primitive :string_byte_index, @object, value, start
      end

      def previous_byte_index(index)
        Rubinius.invoke_primitive :string_previous_byte_index, @object, index
      end

      def copy_from(other, start, size, dest)
        Rubinius.invoke_primitive :string_copy_from, @object, other, start, size, dest
      end

      def resize_capacity(count)
        Rubinius.invoke_primitive :string_resize_capacity, @object, count
      end

      def splice(start, count, replacement)
        str = @object

        # TODO: copy_from unshares
        str.modify!

        bs = str.bytesize
        rbs = replacement.bytesize

        bytes = bs - count + rbs

        s = start + count
        b = bs - s
        d = start + rbs

        # Always resize if the resulting size is different to prevent "leaking"
        # bytes in large ByteArray instances when splicing out chunks.

        if rbs < count
          copy_from str, s, b, d
          resize_capacity bytes
        elsif rbs > count
          resize_capacity bytes
          copy_from str, s, b, d
        end

        copy_from replacement, 0, rbs, start if rbs > 0

        str.num_bytes = bytes

        self
      end
    end
  end
end

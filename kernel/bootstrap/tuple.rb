module Rubinius
  class Tuple

    def self.allocate
      raise TypeError, "Tuple cannot be created via allocate()"
    end

    def self.new(cnt)
      Rubinius.primitive :tuple_allocate
      raise PrimitiveFailure, "Tuple.new primitive failed"
    end

    def self.pattern(size, obj)
      Rubinius.primitive :tuple_pattern
      raise PrimitiveFailure, "Tuple.pattern primitive failed"
    end

    def [](idx)
      Rubinius.primitive :tuple_at

      unless idx.kind_of? Fixnum
        raise TypeError, "Only Fixnums valid for Tuple indices"
      end

      raise PrimitiveFailure, "Tuple#[] primitive failed"
    end

    alias_method :at, :[]

    def []=(idx, val)
      Rubinius.primitive :tuple_put

      unless idx.kind_of? Fixnum
        raise TypeError, "Only Fixnums valid for Tuple indices"
      end
      raise PrimitiveFailure, "Tuple#[]= primitive failed"
    end

    alias_method :put, :[]=

    def fields
      Rubinius.primitive :tuple_fields
      raise PrimitiveFailure, "Tuple#fields primitive failed"
    end

    def copy_from(other, start, length, dest)
      Rubinius.primitive :tuple_copy_from

      unless other.kind_of? Rubinius::Tuple
        raise TypeError, "Tuple#copy_from was expecting a Tuple, not a #{other.class}"
      end
      start = Rubinius::Type.coerce_to start, Fixnum, :to_i
      length = Rubinius::Type.coerce_to length, Fixnum, :to_i
      dest = Rubinius::Type.coerce_to dest, Fixnum, :to_i

      if start < 0 || start > other.fields
        raise IndexError, "Start %d is out of bounds %d" % [start, other.fields]
      end

      if dest < 0 || dest > self.fields
        raise IndexError, "Destination %d is out of bounds %d" % [dest, self.fields]
      end

      if length < 0
        raise IndexError, "length %d must be positive" % [length]
      end

      if (start + length) > other.fields
        raise IndexError, "end index %d can not exceed size of source %d" % [start+length, other.fields]
      end

      if length > (self.fields - dest)
        raise IndexError, "length %d can not exceed space %d in destination" % [length, self.fields - dest]
      end

      src = start
      dst = dest
      while src < (start + length)
        put dst, other.at(src)
        src += 1
        dst += 1
      end

      self
    end

    def dup
      Rubinius.primitive :tuple_dup

      obj = Rubinius::Type.object_class(self).new(self.size)

      Rubinius.invoke_primitive :object_copy_object, obj, self

      Rubinius.privately do
        obj.initialize_copy self
      end

      obj
    end

    def delete(start, length, object)
      Rubinius.primitive :tuple_delete_inplace

      start = Rubinius::Type.coerce_to start, Fixnum, :to_i
      length = Rubinius::Type.coerce_to length, Fixnum, :to_i

      lend = start
      rend = lend + length

      return 0 if self.fields == 0
      if lend < 0 || lend >= self.fields
        raise IndexError, "Index %d is not within Tuple of length %d" % [lend, self.fields]
      end

      if rend < 0 || rend > self.fields
        raise IndexError, "Index %d is not within Tuple of length %d" % [rend, self.fields]
      end

      i = lend
      while i < rend
        if at(i) == object
          j = i
          i += 1
          while i < rend
            val = at(i)
            if val != object
              put j, val
              j += 1
            end
            i += 1
          end

          # cleanup all the remaining bins in the tuple
          i = j
          while i < rend
            put i, nil
            i += 1
          end

          return rend - j
        end
        i += 1
      end

      return 0
    end

    def reverse!(start, total)
      Rubinius.primitive :tuple_reverse

      reverse(
        Rubinius::Type.coerce_to(start, Fixnum, :to_i),
        Rubinius::Type.coerce_to(total, Fixnum, :to_i))
    end

    def delete_at_index(index)
      return if size == 1

      s = size - 1
      t = self.class.new s

      if index != 0
        t.copy_from self, 0, index, 0
      end

      if index < s
        t.copy_from self, index+1, s-index, index
      end

      t
    end

    def insert_at_index(index, value)
      t = self.class.new size + 1

      if index != 0
        t.copy_from self, 0, index, 0
      end

      if index < size
        t.copy_from self, index, size-index, index+1
      end

      t[index] = value

      t
    end
  end
end

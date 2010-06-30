module Rubinius
  class Tuple

    def self.allocate
      raise TypeError, "Tuple cannot be created via allocate()"
    end

    def self.new(cnt)
      Ruby.primitive :tuple_allocate
      raise PrimitiveFailure, "Tuple.new primitive failed"
    end

    def self.pattern(size, obj)
      Ruby.primitive :tuple_pattern
      raise PrimitiveFailure, "Tuple.pattern primitive failed"
    end

    def [](idx)
      Ruby.primitive :tuple_at

      unless idx.kind_of? Fixnum
        raise TypeError, "Only Fixnums valid for Tuple indices"
      end

      raise PrimitiveFailure, "Tuple#[] primitive failed"
    end

    def []=(idx, val)
      Ruby.primitive :tuple_put

      unless idx.kind_of? Fixnum
        raise TypeError, "Only Fixnums valid for Tuple indices"
      end
      raise PrimitiveFailure, "Tuple#[]= primitive failed"
    end

    def at(idx)
      Ruby.primitive :tuple_at
      raise PrimitiveFailure, "Tuple#[] primitive failed"
    end

    def put(idx, val)
      Ruby.primitive :tuple_put
      raise PrimitiveFailure, "Tuple#[]= primitive failed"
    end

    def fields
      Ruby.primitive :tuple_fields
      raise PrimitiveFailure, "Tuple#fields primitive failed"
    end

    def copy_from(other, start, length, dest)
      Ruby.primitive :tuple_copy_from

      unless other.kind_of? Rubinius::Tuple
        raise TypeError, "Tuple#copy_from was expecting a Tuple, not a #{other.class}"
      end
      start = Type.coerce_to start, Fixnum, :to_i
      length = Type.coerce_to length, Fixnum, :to_i
      dest = Type.coerce_to dest, Fixnum, :to_i

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
      obj = self.class.new(self.size)

      Rubinius.invoke_primitive :object_copy_object, obj, self

      Rubinius.privately do
        obj.initialize_copy self
      end

      obj
    end

    def delete(start,length,object)
      Ruby.primitive :tuple_delete_inplace

      start = Type.coerce_to start, Fixnum, :to_i
      length = Type.coerce_to length, Fixnum, :to_i

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
      Ruby.primitive :tuple_reverse

      reverse(
        Type.coerce_to(start, Fixnum, :to_i),
        Type.coerce_to(total, Fixnum, :to_i))
    end

    def self.create_weakref(object)
      Ruby.primitive :tuple_create_weakref
      raise PrimitiveFailure, "Tuple.create_weakref failed, unable to create a weak reference"
    end

  end
end

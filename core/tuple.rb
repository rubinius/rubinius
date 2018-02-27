module Rubinius
  class Tuple
    include Enumerable

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

    def self.[](*args)
      args.to_tuple
    end

    def to_s
      "#<#{self.class}:0x#{object_id.to_s(16)} #{fields} elements>"
    end

    def each
      i = 0
      t = fields
      while i < t
        yield at(i)
        i += 1
      end
      self
    end

    def ==(tup)
      return super unless tup.kind_of?(Tuple)

      t = fields()

      return false unless t == tup.size

      i = 0
      while i < t
        return false unless at(i) == tup.at(i)
        i += 1
      end

      return true
    end

    def +(o)
      t = Tuple.new(size + o.size)
      t.copy_from(self, 0, size, 0)
      t.copy_from(o, 0, o.size, size)
      t
    end

    def inspect
      str = "#<#{self.class}"
      if fields == 0
        str << " empty>"
      else
        str << ": #{join(", ", :inspect)}>"
      end
      return str
    end

    def join(sep, meth=:to_s)
      join_upto(sep, fields, meth)
    end

    def join_upto(sep, count, meth=:to_s)
      str = ""
      return str if count == 0 or empty?

      count = fields if count >= fields
      count -= 1
      i = 0
      while i < count
        str.append at(i).__send__(meth)
        str.append sep.dup
        i += 1
      end

      str.append at(count).__send__(meth)
      return str
    end

    def ===(other)
      return false unless Tuple === other and fields == other.fields
      i = 0
      while i < fields
        return false unless at(i) === other.at(i)
        i += 1
      end
      true
    end

    def to_a
      array = Array.allocate

      m = Rubinius::Mirror.reflect array
      m.tuple = dup
      m.total = fields

      array
    end

    alias_method :to_ary, :to_a

    def shift
      return self unless fields > 0
      t = Tuple.new(fields-1)
      t.copy_from self, 1, fields-1, 0
      return t
    end

    # Swap elements of the two indexes.
    def swap(a, b)
      temp = at(a)
      put a, at(b)
      put b, temp
    end

    alias_method :size, :fields
    alias_method :length, :fields

    def empty?
      size == 0
    end

    def first
      at(0)
    end

    def last
      at(fields - 1)
    end

    # Marshal support - _dump / _load are deprecated so eventually we should figure
    # out a better way.
    def _dump(depth)
      # TODO use depth
      Marshal.dump to_a
    end

    def self._load(str)
      ary = Marshal.load(str)
      t = new(ary.size)
      ary.each_with_index { |obj, idx| t[idx] = obj }
      return t
    end
  end

  class RTuple < Tuple
    include Enumerable

    def self.new(cnt)
      Rubinius.primitive :rtuple_allocate
      raise PrimitiveFailure, "RTuple.new primitive failed"
    end

    def [](idx)
      Rubinius.primitive :rtuple_at

      unless idx.kind_of? Fixnum
        raise TypeError, "Only Fixnums valid for RTuple indices"
      end

      raise PrimitiveFailure, "RTuple#[] primitive failed"
    end

    alias_method :at, :[]

    def []=(idx, val)
      Rubinius.primitive :rtuple_put

      unless idx.kind_of? Fixnum
        raise TypeError, "Only Fixnums valid for RTuple indices"
      end
      raise PrimitiveFailure, "RTuple#[]= primitive failed"
    end

    alias_method :put, :[]=
  end
end

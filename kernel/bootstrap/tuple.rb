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
      raise PrimitiveFailure, "Tuple#[] primitive failed"
    end

    def []=(idx, val)
      Ruby.primitive :tuple_put
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
      raise PrimitiveFailure, "Tuple#copy_from primitive failed"
    end

    def dup
      obj = self.class.new(self.size)
      obj.copy_object self
      obj.send :initialize_copy, self
      obj
    end

    def delete(start,length,object)
      Ruby.primitive :tuple_delete_inplace
      raise PrimitiveFailure, "Tuple#delete primitive failed"
    end

    def self.create_weakref(object)
      Ruby.primitive :tuple_create_weakref
      raise PrimitiveFailure, "Tuple.create_weakref failed, unable to create a weak reference"
    end
  end
end

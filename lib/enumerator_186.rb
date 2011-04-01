# A class which provides a method `each' to be used as an Enumerable
# object.

module Enumerable
  class Enumerator
    include Enumerable

    def initialize(obj, iter = :each, *args, &block)
      @object = obj
      @iter = iter.to_sym
      @args = args
    end

    def each(&block)
      @object.send(@iter, *@args, &block)
    end
  end
end

module Enumerable
  def each_cons(num)
    n = Rubinius::Type.coerce_to(num, Fixnum, :to_int)
    raise ArgumentError, "invalid size: #{n}" if n <= 0
    array = []
    each do |element|
      array << element
      array.shift if array.size > n
      yield array.dup if array.size == n
    end
    nil
  end

  def each_slice(slice_size)
    n = Rubinius::Type.coerce_to(slice_size, Fixnum, :to_int)
    raise ArgumentError, "invalid slice size: #{n}" if n <= 0
    a = []
    each do |element|
      a << element
      if a.length == n
        yield a
        a = []
      end
    end
    yield a unless a.empty?
    nil
  end

  def enum_cons(n)
    Enumerable::Enumerator.new(self, :each_cons, n)
  end

  def enum_slice(n)
    Enumerable::Enumerator.new(self, :each_slice, n)
  end

  def enum_with_index
    Enumerable::Enumerator.new(self, :each_with_index)
  end
end

module Kernel
  def enum_for(method = :each, *args)
    Enumerable::Enumerator.new(self,method,*args)
  end
  
  alias_method :to_enum, :enum_for
end

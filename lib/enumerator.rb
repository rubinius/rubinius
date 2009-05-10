# A class which provides a method `each' to be used as an Enumerable
# object.

MODULE_FOR_ENUMERATOR_CLASS = RUBY_VERSION < "1.9" ? Enumerable : Object

module MODULE_FOR_ENUMERATOR_CLASS
  class Enumerator
    include Enumerable

    # Actually defined in Ruby 1.9 only, we define it here because it can be useful to implementers
    class Yielder
      def initialize(&block)
        @main_block = block
      end

      def each(&block)
        @final_block = block
        @main_block.call(self)
      end

      def yield(*arg)
        @final_block.call(*arg)
      end
    end
    
    def self.new_with_block(&block)
      new(Yielder.new(&block))
    end

    def initialize(obj = Undefined, iter = :each, *args, &block)
      if obj.equal? Undefined
        raise ArgumentError, "method 'initialize': wrong number of argument (0 for 1+)" unless Rubinius::TARGET_IS_19 && block_given?
        obj = Yielder.new(&block)
      end
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
    n = Type.coerce_to(num, Fixnum, :to_int)
    raise ArgumentError, "invalid size: #{n}" if n <= 0
    return to_enum :each_cons, num unless block_given? || Rubinius::TARGET_IS_186
    array = []
    each do |element|
      array << element
      array.shift if array.size > n
      yield array.dup if array.size == n
    end
    nil
  end

  def enum_cons(n)
    MODULE_FOR_ENUMERATOR_CLASS::Enumerator.new(self, :each_cons, n)
  end

  def each_slice(slice_size)
    n = Type.coerce_to(slice_size, Fixnum, :to_int)
    raise ArgumentError, "invalid slice size: #{n}" if n <= 0
    return to_enum :each_slice, slice_size unless block_given? || Rubinius::TARGET_IS_186
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

  def enum_slice(n)
    MODULE_FOR_ENUMERATOR_CLASS::Enumerator.new(self, :each_slice, n)
  end

  def enum_with_index
    MODULE_FOR_ENUMERATOR_CLASS::Enumerator.new(self, :each_with_index)
  end
end

module Kernel
  def enum_for(method = :each, *args)
    MODULE_FOR_ENUMERATOR_CLASS::Enumerator.new(self,method,*args)
  end
  
  alias_method :to_enum, :enum_for
end

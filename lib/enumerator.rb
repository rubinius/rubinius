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
  def each_cons(n, &block)
    array = []
    elements = self.to_a
    while elements.size > 0 do
      array << elements[0,n] if elements[0,n].size == n
      elements.shift
    end
    array.each { |set| yield set }
    nil
  end

  def enum_cons(n)
    MODULE_FOR_ENUMERATOR_CLASS::Enumerator.new(self, :each_cons, n)
  end

  def each_slice(slice_size, &block)
    a = []
    each { |element|
      a << element
      if a.length == slice_size
        yield a
        a = []
      end
    }
    yield a if a.length > 0
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

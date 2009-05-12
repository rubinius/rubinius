class Enumerator
  def initialize(obj = Undefined, iter = :each, *args, &block)
    if obj.equal? Undefined
      raise ArgumentError, "method 'initialize': wrong number of argument (0 for 1+)" unless block_given?
      obj = Yielder.new(&block)
    end
    @object = obj
    @iter = iter.to_sym
    @args = args
  end
end

module Kernel
  def enum_for(method = :each, *args)
    Enumerator.new(self,method,*args)
  end
  
  alias_method :to_enum, :enum_for
end

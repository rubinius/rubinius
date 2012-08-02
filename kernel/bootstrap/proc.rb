# -*- encoding: us-ascii -*-

class Proc
  def self.allocate
    raise TypeError, "allocator undefined for Proc"
  end

  def self.__allocate__
    Rubinius.primitive :proc_allocate
    raise PrimitiveFailure, "Proc#allocate primitive failed"
  end

  def call(*args)
    Rubinius.primitive :proc_call
    raise PrimitiveFailure, "Proc#call primitive failed"
  end

  def call_on_object(*args)
    Rubinius.primitive :proc_call_on_object
    raise PrimitiveFailure, "Proc#call_on_object primitive failed"
  end

  def lambda_style!
    @lambda = true
  end

  def lambda?
    !!@lambda
  end
end

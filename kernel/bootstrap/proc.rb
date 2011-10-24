class Proc
  def self.allocate
    Rubinius.primitive :proc_allocate
    raise PrimitiveFailure, "Proc#allocate failed"
  end

  def call(*args)
    Rubinius.primitive :proc_call
    raise PrimitiveFailure, "Proc#call failed"
  end

  def call_on_object(*args)
    Rubinius.primitive :proc_call_on_object
    raise PrimitiveFailure, "Proc#call_on_object failed"
  end

  def lambda_style!
    @lambda = true
  end

  def lambda?
    !!@lambda
  end
end

class Proc
  def self.allocate
    Ruby.primitive :proc_allocate
    raise PrimitiveFailure, "Proc#allocate failed"
  end

  def call(*args)
    Ruby.primitive :proc_call
    raise PrimitiveFailure, "Proc#call failed"
  end

  def call_on_object(*args)
    Ruby.primitive :proc_call_on_object
    raise PrimitiveFailure, "Proc#call_on_object failed"
  end

  def lambda_style!
    @lambda = true
  end
end

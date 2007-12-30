
class Primitives

  def add(x, y)
    x.fixnum!
    y.fixnum!
    stack_push fixnum_add(@state, x, y)
  end
  
  def bignum_add(x, y)
    x.bignum!
    y.bignum!
    
    
  end

end


class Object
  def equal(other)
    Ruby.primitive do
      if self == other
        stack_push true
      else
        stack_push false
      end
    end
  end
end
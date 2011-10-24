class Proc
  alias_method :===, :call

  def curry(curried_arity = nil)
    verify_arity(curried_arity)

    args = []

    m = lambda? ? :lambda : :proc
    f = send(m) {|*x|
      args += x
      if args.length >= arity
        self[*args]
      else
        f
      end
    }
  end

  def to_s
    l = " (lambda)" if lambda?
    "#<#{self.class}:0x#{self.object_id.to_s(16)}@#{@block.file}:#{@block.line}#{l}>"
  end

  # private
  # Marking the following as private causes a VM exception

  def raise_arity_error(actual, expected)
    raise ArgumentError, "Wrong number of arguments (%i for %i)" % [
      actual,
      expected
    ]
  end

  def verify_arity(curried_arity)
    if lambda? && curried_arity
      if arity > 0 && curried_arity != arity
        raise_arity_error(curried_arity, arity)
      end

      if arity < 0 && curried_arity < (-arity - 1)
        raise_arity_error(curried_arity, -arity - 1)
      end
    end
  end
end

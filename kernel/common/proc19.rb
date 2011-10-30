class Proc

  alias_method :===, :call

  def curry(curried_arity = nil)
    if lambda? && curried_arity
      if arity > 0 && curried_arity != arity
        raise ArgumentError, "Wrong number of arguments (%i for %i)" % [
          curried_arity,
          arity
        ]
      end

      if arity < 0 && curried_arity < (-arity - 1)
        raise ArgumentError, "Wrong number of arguments (%i for %i)" % [
          curried_arity,
          -arity - 1
        ]
      end
    end

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
    f.singleton_class.send(:define_method, :binding) {
      raise ArgumentError, "cannot create binding from f proc"
    }
    f.singleton_class.send(:define_method, :parameters) {
      [[:rest]]
    }
    f.singleton_class.send(:define_method, :source_location) {
      nil
    }
    f
  end

  def source_location
    [@block.file.to_s, @block.line]
  end

  def to_s
    l = " (lambda)" if lambda?
    "#<#{self.class}:0x#{self.object_id.to_s(16)}@#{@block.file}:#{@block.line}#{l}>"
  end
end

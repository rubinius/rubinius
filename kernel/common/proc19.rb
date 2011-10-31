class Proc

  alias_method :===, :call

  #
  #  call-seq:
  #     prc.curry         -> a_proc
  #     prc.curry(arity)  -> a_proc
  #
  #  Returns a curried proc. If the optional +arity+ argument is given,
  #  it determines the number of arguments.
  #
  #  A curried proc receives some arguments. If a sufficient number of
  #  arguments are supplied, it passes the supplied arguments to the original
  #  proc and returns the result. Otherwise, returns another curried proc that
  #  takes the rest of arguments.
  #
  #     b = proc {|x, y, z| (x||0) + (y||0) + (z||0) }
  #     p b.curry[1][2][3]           #=> 6
  #     p b.curry[1, 2][3, 4]        #=> 6
  #     p b.curry(5)[1][2][3][4][5]  #=> 6
  #     p b.curry(5)[1, 2][3, 4][5]  #=> 6
  #     p b.curry(1)[1]              #=> 1
  #
  #     b = proc {|x, y, z, *w| (x||0) + (y||0) + (z||0) + w.inject(0, &:+) }
  #     p b.curry[1][2][3]           #=> 6
  #     p b.curry[1, 2][3, 4]        #=> 10
  #     p b.curry(5)[1][2][3][4][5]  #=> 15
  #     p b.curry(5)[1, 2][3, 4][5]  #=> 15
  #     p b.curry(1)[1]              #=> 1
  #
  #     b = lambda {|x, y, z| (x||0) + (y||0) + (z||0) }
  #     p b.curry[1][2][3]           #=> 6
  #     p b.curry[1, 2][3, 4]        #=> wrong number of arguments (4 for 3)
  #     p b.curry(5)                 #=> wrong number of arguments (5 for 3)
  #     p b.curry(1)                 #=> wrong number of arguments (1 for 3)
  #
  #     b = lambda {|x, y, z, *w| (x||0) + (y||0) + (z||0) + w.inject(0, &:+) }
  #     p b.curry[1][2][3]           #=> 6
  #     p b.curry[1, 2][3, 4]        #=> 10
  #     p b.curry(5)[1][2][3][4][5]  #=> 15
  #     p b.curry(5)[1, 2][3, 4][5]  #=> 15
  #     p b.curry(1)                 #=> wrong number of arguments (1 for 3)
  #
  #     b = proc { :foo }
  #     p b.curry[]                  #=> :foo
  #
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

  #
  # call-seq:
  #    prc.source_location  -> [String, Fixnum]
  #
  # Returns the ruby source filename and line number containing
  # this proc, or nil if this proc was not defined in ruby (i.e.
  # native)
  #
  def source_location
    [@block.file.to_s, @block.line]
  end


  #
  # call-seq:
  #   prc.to_s   -> string
  #
  # Shows the unique identifier for this proc, along with
  # an indication of where the proc was defined.
  #
  def to_s
    l = " (lambda)" if lambda?
    "#<#{self.class}:0x#{self.object_id.to_s(16)}@#{@block.file}:#{@block.line}#{l}>"
  end
end

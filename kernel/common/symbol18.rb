# -*- encoding: us-ascii -*-

class Symbol
  def inspect
    str = to_s

    case str
    when /\A(\$|@@?)[a-z_][a-z_\d]*\z/i,                      # Variable names
         /\A[a-z_][a-z_\d]*[=?!]?\z/i,                        # Method names
         /\A\$(-[a-z_\d]|[+~:?<_\/'"$.,`!;\\=*>&@]|\d+)\z/i,  # Special global variables
         /\A([|^&\/%~`]|<<|>>|<=>|===?|=~|[<>]=?|[+-]@?|\*\*?|\[\]=?)\z/ # Operators
      ":#{str}"
    else
      ":#{str.inspect}"
    end
  end

  alias_method :intern, :to_sym
  alias_method :id2name, :to_s

  # Returns a Proc object which respond to the given method by sym.
  def to_proc
    # Put sym in the outer enclosure so that this proc can be instance_eval'd.
    # If we used self in the block and the block is passed to instance_eval, then
    # self becomes the object instance_eval was called on. So to get around this,
    # we leave the symbol in sym and use it in the block.
    #
    sym = self
    Proc.new do |*args|
      raise ArgumentError, "no receiver given" if args.empty?
      args.shift.__send__(sym, *args)
    end
  end
end

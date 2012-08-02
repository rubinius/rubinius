# -*- encoding: us-ascii -*-

class Symbol

  # Use equal? for ===
  alias_method :===, :equal?

  def self.all_symbols
    Rubinius.primitive :symbol_all_symbols
    raise PrimitiveFailure, "Symbol.all_symbols primitive failed."
  end

  # Returns a Proc object which respond to the given method by sym.
  def to_proc
    # Put sym in the outer enclosure so that this proc can be instance_eval'd.
    # If we used self in the block and the block is passed to instance_eval, then
    # self becomes the object instance_eval was called on. So to get around this,
    # we leave the symbol in sym and use it in the block.
    #
    sym = self
    Proc.new { |*args| args.shift.__send__(sym, *args) }
  end

end

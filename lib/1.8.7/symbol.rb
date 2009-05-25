class Symbol
  # Returns a Proc object which respond to the given method by sym.
  def to_proc
    Proc.new { |*args| args.shift.__send__(self, *args) }
  end
end
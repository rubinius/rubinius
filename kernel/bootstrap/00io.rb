class IO
  ivar_as_index :__ivars__ => 0, :descriptor => 1
  def descriptor; @descriptor ; end
  def __ivars__ ; @__ivars__  ; end

  def write(str)
    Ruby.primitive :io_write
    exc = IOError.new("Unable to write '#{str}' via #{self}")
    raise exc
  end
end


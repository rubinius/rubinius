class IO
  ivar_as_index :descriptor => 0
  def descriptor; @descriptor ; end
  def __ivars__ ; nil         ; end

  def write(str)
    Ruby.primitive :io_write
    exc = IOError.new("Unable to write '#{str}' via #{self}")
    raise exc
  end
end


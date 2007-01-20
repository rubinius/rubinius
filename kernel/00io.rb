class IO
  def write(str)
    Ruby.primitive :io_write
    exc = IOError.new("Unable to write '#{str}' via #{self}")
    raise exc
  end
  
  def <<(obj)
    write(obj.to_s)
  end
end

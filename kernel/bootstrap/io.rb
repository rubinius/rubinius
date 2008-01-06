class IO
  def write(str)
    Ruby.primitive :io_write
    raise PrimitiveFailure, "IO#write failed. Might not have passed a string."
  end

  def prim_seek(amount, whence)
    Ruby.primitive :io_seek
  end

  def prim_operation(which)
    Ruby.primitive :io_operation
  end

  def tty?
    prim_operation(0)
  end

  def ttyname
    prim_operation(1)
  end
end

class IO
  def self.open_with_mode(path, mode, perm)
    Ruby.primitive :io_open
    raise PrimitiveFailure, "primitive failed"
  end

  def write(str)
    Ruby.primitive :io_write
    raise PrimitiveFailure, "IO#write failed. Might not have passed a string."
  end

  def prim_seek(amount, whence)
    Ruby.primitive :io_seek
    raise PrimitiveFailure, "primitive failed"
  end

  def prim_operation(which)
    Ruby.primitive :io_operation
    raise PrimitiveFailure, "primitive failed"
  end

  def tty?
    prim_operation(0)
  end

  def ttyname
    prim_operation(1)
  end
end

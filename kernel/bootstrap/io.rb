class IO
  def self.open_with_mode(path, mode, perm)
    Ruby.primitive :io_open
    raise PrimitiveFailure, "primitive failed"
  end

  def self.create_pipe(lhs, rhs)
    Ruby.primitive :create_pipe
    raise PrimitiveFailure, "primitive failed"
  end

  def write(str)
    Ruby.primitive :io_write
    raise PrimitiveFailure, "IO#write failed. Might not have passed a string."
  end

  def prim_read(size, buffer)
    Ruby.primitive :io_read
    raise PrimitiveFailure, "primitive failed"
  end

  def prim_reopen(other)
    Ruby.primitive :io_reopen
    raise ArgumentError, "only accepts an IO object"
  end

  def prim_seek(amount, whence)
    Ruby.primitive :io_seek
    raise PrimitiveFailure, "primitive failed"
  end

  def prim_operation(which)
    Ruby.primitive :io_operation
    raise PrimitiveFailure, "primitive failed"
  end

  def reopen(other)
    prim_reopen other
  end

  def tty?
    prim_operation(0)
  end

  def ttyname
    prim_operation(1)
  end

  def close
    Ruby.primitive :io_close_ng
    raise PrimitiveFailure, "IO#close primitive failed"
  end
end

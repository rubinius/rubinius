class IO
  def write(str)
    Ruby.primitive :io_write
    raise PrimitiveFailure, "IO#write failed. Might not have passed a string."
  end

  def prim_seek(amount, whence)
    Ruby.primitive :io_seek
  end
end

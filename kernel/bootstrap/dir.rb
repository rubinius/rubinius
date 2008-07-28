class Dir
  def __open__(path)
    Ruby.primitive :dir_open
    raise PrimitiveFailure, "Dir#__open__ primitive failed"
  end

  def close
    Ruby.primitive :dir_close
    raise PrimitiveFailure, "Dir#close primitive failed"
  end

  def closed?
    Ruby.primitive :dir_closed_p
    raise PrimitiveFailure, "Dir#closed? primitive failed"
  end

  def read
    Ruby.primitive :dir_read
    raise PrimitiveFailure, "Dir#read primitive failed"
  end

  def __control__(kind, pos)
    Ruby.primitive :dir_control
    raise PrimitiveFailure, "Dir#__control__ primitive failed"
  end
end

class Dir
  def self.allocate
    Rubinius.primitive :dir_allocate
    raise PrimitiveFailure, "Dir.allocate primitive failed"
  end

  def initialize(path)
    path = Rubinius::Type.coerce_to_path path
    Rubinius.invoke_primitive :dir_open, self, path
  end

  private :initialize

  def close
    Rubinius.primitive :dir_close
    raise PrimitiveFailure, "Dir#close primitive failed"
  end

  def closed?
    Rubinius.primitive :dir_closed_p
    raise PrimitiveFailure, "Dir#closed? primitive failed"
  end

  def read
    Rubinius.primitive :dir_read
    raise PrimitiveFailure, "Dir#read primitive failed"
  end

  def control(kind, pos)
    Rubinius.primitive :dir_control
    raise PrimitiveFailure, "Dir#__control__ primitive failed"
  end

  private :control
end

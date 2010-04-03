class Dir
  def self.allocate
    Ruby.primitive :dir_allocate
    raise PrimitiveFailure, "Dir.allocate primitive failed"
  end

  def initialize(path)
    Ruby.primitive :dir_open
    raise PrimitiveFailure, "Dir#open primitive failed"
  end

  private :initialize

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

  def control(kind, pos)
    Ruby.primitive :dir_control
    raise PrimitiveFailure, "Dir#__control__ primitive failed"
  end

  private :control
end

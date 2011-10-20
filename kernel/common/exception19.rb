class Exception
  def ==(other)
    (self.class == other.class || other.kind_of?(Exception)) &&
      message == other.message &&
      backtrace == other.backtrace
  end
end

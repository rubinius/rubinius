class Exception
  def ==(other)
    other.is_a?(self.class) &&
      message == other.message &&
      backtrace == other.backtrace
  end
end

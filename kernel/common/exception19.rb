class Exception
  def ==(other)
    (self.class == other.class || other.class.ancestors.include?(Exception)) &&
      message == other.message &&
      backtrace == other.backtrace
  end
end

class Exception
  def ==(other)
    other.kind_of?(Exception) &&
      message == other.message &&
      backtrace == other.backtrace
  end
end

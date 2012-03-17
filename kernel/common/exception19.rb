# -*- encoding: us-ascii -*-

class Exception
  def ==(other)
    other.kind_of?(Exception) &&
      message == other.message &&
      backtrace == other.backtrace
  end

  def to_s
    if @reason_message
      @reason_message.to_s
    else
      self.class.to_s
    end
  end
end

class KeyError < IndexError
end

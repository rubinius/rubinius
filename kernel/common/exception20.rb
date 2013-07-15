# -*- encoding: us-ascii -*-

class Exception
  def ==(other)
    other.instance_of?(__class__) &&
      message == other.message &&
      backtrace == other.backtrace
  end
end

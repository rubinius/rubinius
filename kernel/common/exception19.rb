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

class SignalException < Exception

  attr_reader :signo
  attr_reader :signm

  def initialize(signo = nil, signm = nil)
    # MRI overrides this behavior just for SignalException itself
    # but not for anything that inherits from it, therefore we
    # need this ugly check to make sure it works as intented.
    return super(signo) unless self.class == SignalException
    if signo.is_a? Integer
      unless @signm = Signal::Numbers[signo]
        raise ArgumentError, "invalid signal number #{signo}"
      end
      @signo = signo
      @signm = signm || "SIG#{@signm}"
    elsif signo
      if signm
        raise ArgumentError, "wrong number of arguments (2 for 1)"
      end
      signm = signo
      if signo.kind_of?(Symbol)
        signm = signm.to_s
      else
        signm = StringValue(signm)
      end
      signm = signm[3..-1] if signm.prefix? "SIG"
      unless @signo = Signal::Names[signm]
        raise ArgumentError, "invalid signal name #{signm}"
      end
      @signm = "SIG#{signm}"
    end
    super(@signm)
  end
end

class StopIteration
  attr_accessor :result
  private :result=
end

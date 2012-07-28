# -*- encoding: us-ascii -*-

class Exception
  alias_method :to_str, :message

  def to_s
    @reason_message || self.class.to_s
  end
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
      if sym_sig = signo.kind_of?(Symbol)
        signm = signm.to_s
      else
        signm = StringValue(signm)
      end
      signm = signm[3..-1] if signm.prefix? "SIG"
      unless @signo = Signal::Names[signm]
        raise ArgumentError, "invalid signal name #{signm}"
      end
      @signm = sym_sig ? signm : signo
    end
    super(@signm)
  end
end

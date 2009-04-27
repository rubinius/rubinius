module Signal
  Names = {"EXIT" => 0}

  @threads = {}
  @handlers = {}

  def self.trap(sig, prc=nil, &block)
    sig = sig.to_s if sig.kind_of?(Symbol)

    if sig.kind_of?(String)
      osig = sig

      if sig.prefix? "SIG"
        sig = sig[3..-1]
      end

      if sig == "EXIT"
        at_exit { block.call }
        return
      end

      unless number = Names[sig]
        raise ArgumentError, "Unknown signal '#{osig}'"
      end
    else
      number = sig.to_i
    end

    if prc and block
      raise ArgumentError, "Either a Proc or a block, not both."
    elsif block
      prc = block
    end

    old = @handlers[number]
    @handlers[number] = prc

    Rubinius.watch_signal number

    return old
  end

  def self.run_handler(sig)
    @handlers[sig].call(sig)
  end

  def self.list
    Names.dup
  end

end

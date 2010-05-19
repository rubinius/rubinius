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
        at_exit(&block)
        return
      end

      unless number = Names[sig]
        raise ArgumentError, "Unknown signal '#{osig}'"
      end
    else
      number = sig.to_i

      if number == 0
        at_exit(&block)
        return
      end
    end

    if prc and block
      raise ArgumentError, "Either a Proc/String or a block, not both."
    elsif block
      prc = block
    end

    case prc
    when "DEFAULT", "_DFL"
      old = @handlers.delete(number)
      Rubinius.watch_signal -number
      return old
    when "IGNORE", "_IGN"
      prc = nil
    when "EXIT"
      prc = proc { exit }
    when String
      raise ArgumentError, "Unsupported command '#{prc}'"
    else
      unless prc.respond_to? :call
        raise ArgumentError, "Handler must respond to #call"
      end
    end

    old = @handlers[number]
    @handlers[number] = prc

    Rubinius.watch_signal number

    return old
  end

  def self.run_handler(sig)
    # Ignore nil handlers
    if handler = @handlers[sig]
      handler.call(sig)
    end
  end

  def self.list
    Names.dup
  end

end

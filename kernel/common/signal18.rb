# -*- encoding: us-ascii -*-

module Signal

  def self.trap(sig, prc=nil, &block)
    sig = sig.to_s if sig.kind_of?(Symbol)

    if sig.kind_of?(String)
      osig = sig

      if sig.prefix? "SIG"
        sig = sig[3..-1]
      end

      unless number = Names[sig]
        raise ArgumentError, "Unknown signal '#{osig}'"
      end
    else
      number = sig.to_i
    end

    # If no command, use the block.
    prc ||= block

    case prc
    when "DEFAULT", "SIG_DFL"
      had_old = @handlers.key?(number)
      old = @handlers.delete(number)

      if number != Names["EXIT"]
        Rubinius.watch_signal(-number, prc.nil?)
      end

      return "DEFAULT" unless had_old
      return old ? old : "IGNORE"
    when "IGNORE", "SIG_IGN", nil
      prc = nil
    when "EXIT"
      prc = proc { exit }
    when String
      raise ArgumentError, "Unsupported command '#{prc}'"
    else
      unless prc.respond_to? :call
        raise ArgumentError, "Handler must respond to #call (was #{prc.class})"
      end
    end

    had_old = @handlers.key?(number)

    old = @handlers[number]
    @handlers[number] = prc

    if number != Names["EXIT"]
      Rubinius.watch_signal(number, prc.nil?)
    end

    return "DEFAULT" unless had_old
    return old ? old : "IGNORE"
  end

end

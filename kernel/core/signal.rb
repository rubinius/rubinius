# depends on: module.rb

module Signal
  Names = {"EXIT" => 0}

  @threads = {}
  @handlers = {}
  
  def self.trap(sig, prc=nil, pass_ctx=false, &block)
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

    # If there is already at thread for this sig, give up.
    return old if @threads[number]

    chan = Channel.new

    thr = Thread.new do
      while true
        ctx = chan.receive

        # Run the handler in a new thread so chan.receive doesn't
        # block signals during handler execution, e.g., a SIGINT
        # during a sleep() in a SIGINT handler.

        Thread.new do
          if pass_ctx
            obj = ctx
          else
            obj = number
          end

          begin
            @handlers[number].call(obj)
          rescue Object => e
            Thread.main.raise e
          end
        end
      end
    end

    @threads[number] = thr

    Scheduler.send_on_signal chan, number
    return old
  end
  
  def self.action(sig, prc=nil, &block)
    trap(sig, prc, true, &block)
  end

  def self.list
    Names.dup
  end

  def self.after_loaded
    Rubinius::RUBY_CONFIG.keys.each do |key|
      if key[0, 20] == 'rbx.platform.signal.'
        Names[ key[23, 100] ] = Rubinius::RUBY_CONFIG[key]
      end
    end
    # special case of signal.c
    if Names["CHLD"]
      Names["CLD"] = Names["CHLD"]
    end
  end

end

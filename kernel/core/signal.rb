# depends on: module.rb

module Signal
  Names = {"EXIT" => 0}

  @handlers = {}
  
  def self.trap(sig, prc=nil, pass_ctx=false, &block)
    if sig.kind_of?(String)
      number = Names[sig]
    else
      number = sig.to_i
    end

    if prc and block
      raise ArgumentError, "Either a Proc or a block, not both."
    elsif block
      prc = block
    end

    chan = Channel.new

    thr = Thread.new do
      while true
        break unless ctx = chan.receive

        if pass_ctx
          obj = ctx
        else
          obj = number
        end

        begin
          prc.call(obj)
        rescue Object
        end
      end
    end

    Scheduler.send_on_signal chan, number

    old = @handlers[number]
    @handlers[number] = thr
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

module Signal

  Names = {
    "HUP" =>  1,
    "INT" =>  2,
    "QUIT" => 3,
    "ABRT" => 6,
    "POLL" => 7,
    "EMT" =>  7,
    "FPE" =>  8,
    "SYS" =>  12,
    "PIPE" => 13,
    "ALRM" => 14,
    "TERM" => 15,
    "URG" =>  16,
    "STOP" => 17,
    "TSTP" => 18,
    "CONT" => 19,
    "CHLD" => 20,
    "IO" =>   23,
    "PROF" => 27,
    "INFO" => 29,
    "USR1" => 30,
    "USR2" => 31
  }

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
    
end

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
  def self.trap(sig, prc=nil, &block)
    if String === sig
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
      loop do
        chan.receive
        prc.call
      end
    end

    chan.send_on_signal number

    old = @handlers[number]
    @handlers[number] = thr
    return old
  end
end

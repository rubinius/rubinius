# -*- encoding: us-ascii -*-

module Signal
  Names = {
    "EXIT" => 0
  }

  Numbers = {
    0 => "EXIT"
  }

  @threads = {}
  @handlers = {}

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

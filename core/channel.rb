# Channel is a FIFO. If #receive is called and no value is available, the
# Thread will sleep until a value is available. If #receive_timeout is called
# and no value is availale, the Thread will sleep until a value is available
# or duration time has elapsed. If #try_receive is called, it returns
# immediately with a value or nil. It's impossible to know whether nil was in
# the channel in the case that the return value is nil.
module Rubinius
  class Channel
    def self.allocate
      Rubinius.primitive :channel_new
      raise PrimitiveFailure, "Channel.allocate primitive failed"
    end

    def send(obj)
      Rubinius.primitive :channel_send
      raise PrimitiveFailure, "Channel#send primitive failed"
    end

    alias_method :<<, :send

    def receive
      Rubinius.primitive :channel_receive
      raise PrimitiveFailure, "Channel#receive primitive failed"
    end

    def receive_timeout(duration)
      Rubinius.primitive :channel_receive_timeout
      raise PrimitiveFailure, "Channel#receive_timeout primitive failed"
    end

    def try_receive
      Rubinius.primitive :channel_try_receive
      raise PrimitiveFailure, "Channel#try_receive primitive failed"
    end

    def inspect
      "#<Rubinius::Channel:0x#{object_id.to_s(16)}>"
    end
  end
end

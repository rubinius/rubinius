class Channel
  ivar_as_index :waiting => 1, :value => 2
  def waiting; @waiting ; end
  def value  ; @value   ; end
  
  def self.new
    Ruby.primitive :channel_new
    raise PrimitiveFailure, "primitive failed"
  end

  def send(obj)
    Ruby.primitive :channel_send
    raise PrimitiveFailure, "primitive failed"
  end

  def receive
    Ruby.primitive :channel_receive
    raise PrimitiveFailure, "primitive failed"
  end

  def self.convert_to_channel(obj)
    return obj if Channel === obj
    begin
      o2 = obj.to_channel
      unless Channel === o2
        raise ArgumentError, "to_channel on #{obj.inspect} did not return a Channel"
      end
      return o2
    rescue NoMethodError
       raise ArgumentError, "Unable to convert #{obj.inspect} into a channel"
    end
  end

  def self.receive(*ary)
    if ary.size == 1
      return convert_to_channel(ary.shift).receive
    else
      na = ary.map { |c| convert_to_channel(c) }
      receive_many(na)
    end
  end

  def to_channel
    self
  end
end

class Scheduler
  def self.send_in_microseconds(chan, microseconds)
    Ruby.primitive :channel_send_in_microseconds
    raise PrimitiveFailure, "primitive failed"
  end

  def self.send_in_seconds(chan, seconds)
    Ruby.primitive :channel_send_in_seconds
    raise PrimitiveFailure, "primitive failed"
  end

  def self.send_on_readable(chan, io, buffer, nbytes)
    Ruby.primitive :channel_send_on_readable
    raise PrimitiveFailure, "primitive failed"
  end

  def self.send_on_writable(chan, io)
    Ruby.primitive :channel_send_on_readable
    raise PrimitiveFailure, "primitive failed"
  end

  def self.send_on_signal(chan, signum)
    Ruby.primitive :channel_send_on_signal
    raise PrimitiveFailure, "primitive failed"
  end

  def self.send_on_stopped_prim(chan, pid, flags)
    Ruby.primitive :channel_send_on_stopped
    raise PrimitiveFailure, "primitive failed"
  end

  def self.send_on_stopped(chan, pid=-1, flags=0)
    send_on_stopped_prim(chan, pid, flags)
  end

  def self.cancel(id)
    Ruby.primitive :scheduler_cancel
    raise PrimitiveFailure, "primitive failed"
  end
end


class Scheduler
  def self.send_in_microseconds(chan, microseconds, tag)
    Ruby.primitive :channel_send_in_microseconds
    raise PrimitiveFailure, "primitive failed"
  end

  def self.send_in_seconds(chan, seconds, tag)
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


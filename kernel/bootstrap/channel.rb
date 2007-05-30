class Channel
  ivar_as_index :waiting => 0, :value => 1
  def waiting; @waiting ; end
  def value  ; @value   ; end
  
  def self.new
    Ruby.primitive :channel_new
  end

  def send(obj)
    Ruby.primitive :channel_send
  end

  def receive_raw
    Ruby.primitive :channel_receive
  end

  def receive
    receive_raw[1]
  end

  def self.receive_many(ary)
    Ruby.primitive :channel_receive_many
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
  def self.send_in_microseconds(chan, count)
    Ruby.primitive :channel_send_in_microseconds
  end

  def self.send_on_readable(chan, io, buffer, count)
    Ruby.primitive :channel_send_on_readable
  end

  def self.send_on_writable(chan, io)
    Ruby.primitive :channel_send_on_readable
  end

  def self.send_on_signal(chan, number)
    Ruby.primitive :channel_send_on_signal
  end
  
  def self.send_on_stopped(chan, pid)
    Ruby.primitive :channel_send_on_stopped
  end
end


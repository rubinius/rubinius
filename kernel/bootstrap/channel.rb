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

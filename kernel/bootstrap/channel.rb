# Channel a simple, thread aware value passing class.
#
# The semantics of it are simple. Channel#send puts a value
# into the channel. A channel can hold any number of objects.
#
# A call to Channel#receive returns the first available object,
# in the order that they were passed to #send.
#
# The power is what happens when the Channel is empty. If
# Channel#receive is called on an empty channel, the VM puts
# the current Thread (t1) to sleep. At some future time, when
# Channel#send is called on that same thread, the VM wakes up
# t1 and the value passed to #send is returned.
#
# This allows us to implement all manner of Thread semantics, such as
# Mutex.
#
# Channel is used heavily by Scheduler, to allow ruby code to
# interact with the outside world in a thread aware manner.
#

class Channel
  ivar_as_index :waiting => 1, :value => 2

  # Return nil if nothing is waiting, or a List object
  # which contains all Thread objects waiting on this Channel.
  #
  def waiting; @waiting ; end

  # Return nil if there are no values, otherwise a List object
  # containing all values the Channel contains.
  #
  def value  ; @value   ; end

  # Create a new Channel, and register it with the VM.
  #
  def self.new
    Ruby.primitive :channel_new
    raise PrimitiveFailure, "primitive failed"
  end

  # Put the value +obj+ into the Channel. If there are waiting threads,
  # the first thread will be woken up and handed +obj+.
  #
  def send(obj)
    Ruby.primitive :channel_send
    raise PrimitiveFailure, "primitive failed"
  end

  # Remove and return the first value from the Channel. If the channel
  # is empty, Thread.current is put to sleep until #send is called.
  #
  def receive
    Ruby.primitive :channel_receive
    raise PrimitiveFailure, "primitive failed"
  end

  # Convert +obj+ into a Channel using #to_channel.
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

  # Legacy API. To be removed.
  def self.receive(obj)
    return convert_to_channel(obj).receive
  end

  # API compliance, returns self.
  def to_channel
    self
  end
end

##
# A SendSite exists in the InstructionSequence wherever you would call a
# method (send a message).  When searching for an Executable to run, the
# SendSite is examined first.  If there is an Executable stored inside it, it
# is used.  Otherwise the MethodTable is consulted (see MethodTable) and the
# SendSite is filled in with that Executable.

class SendSite
  def self.new(name)
    SendSite.create name
  end

  def self.create(name)
    Ruby.primitive :sendsite_create
    raise PrimitiveFailure, "primitive failed"
  end

  def at(index)
    Ruby.primitive :sendsite_at
    raise PrimitiveFailure, "primitive failed"
  end

  def name
    at(0)
  end

  def selector
    at(1)
  end

  def receiver
    at(2)
  end

  def hits
    at(6)
  end

  def misses
    at(7)
  end

  def data(which)
    at(1 + which)
  end

  def sender
    at(8)
  end

  ##
  # Sets the sender field on the SendSite.
  # +cm+ must be a CompiledMethod object
  def sender=(cm)
    Ruby.primitive :sendsite_set_sender
    raise PrimitiveFailure, "primitive failed"
  end

  def inspect
    "#<SendSite:0x#{object_id.to_s(16)} name=#{name} hits=#{hits} misses=#{misses}>"
  end
end

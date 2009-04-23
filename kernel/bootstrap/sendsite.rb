##
# A SendSite exists in the InstructionSequence wherever you would call a
# method (send a message).  When searching for an Executable to run, the
# SendSite is examined first.  If there is an Executable stored inside it, it
# is used.  Otherwise the MethodTable is consulted (see MethodTable) and the
# SendSite is filled in with that Executable.

module Rubinius
  class SendSite

    attr_accessor :name
    attr_accessor :sender
    attr_accessor :selector
    attr_accessor :method
    attr_accessor :module
    attr_accessor :recv_class

    def self.new(name)
      SendSite.create name
    end

    def self.create(name)
      Ruby.primitive :sendsite_create
      raise PrimitiveFailure, "SendSite.create primitive failed"
    end

    def hits
      Ruby.primitive :sendsite_hits
      raise PrimitiveFailure, "Sendsite#hits primitive failed"
    end

    def misses
      Ruby.primitive :sendsite_misses
      raise PrimitiveFailure, "Sendsite#misses primitive failed"
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
end

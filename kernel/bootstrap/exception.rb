# depends on: module.rb lookuptable.rb symbol.rb string.rb

##
# Abstract class for implementing flow control in Rubinius.  Never seen by
# ruby code.

class FlowControlException < Exception
end

##
# Flow control exception used to implement return inside an ensure.  Never
# seen by ruby code.

class ReturnException < FlowControlException
  attr_reader :return_value

  def initialize(val)
    super(nil) # HACK
    @return_value = val
  end
end

class ContextDestinationException < FlowControlException
  attr_accessor :destination
  attr_accessor :value
end

##
# Flow control exception used to implement break inside a block. Never seen
# by ruby code.

class BlockBreakException < ContextDestinationException
  def self.directed_to(context, value)
    exc = allocate
    exc.destination = context.env.home_block
    exc.value = value
    return exc
  end
end

##
# Raised when returning from a block to handle proper flow control.  Never
# seen by ruby code.

class LongReturnException < ContextDestinationException
  def self.directed_to(context, value)
    exc = allocate
    exc.destination = context.env.home
    exc.value = value
    return exc
  end
end

##
# Implements the Composite pattern on SexpProcessor. Need we say more?
#
# Yeah... probably. Implements a SexpProcessor of SexpProcessors so
# you can easily chain multiple to each other. At some stage we plan
# on having all of them run +process+ and but only ever output
# something when +generate+ is called, allowing for deferred final
# processing.

module Compiler1
class CompositeSexpProcessor

  ##
  # The list o' processors to run.

  attr_reader :processors

  def initialize # :nodoc:
    @processors = []
  end

  ##
  # Add a +processor+ to the list of processors to run.

  def <<(processor)
    @processors << processor
  end

  ##
  # Run +exp+ through all of the processors, returning the final
  # result.

  def process(exp)
    @processors.each do |processor|
      exp = processor.process(exp)
    end
    exp
  end

  def on_error_in(node_type, &block)
    @processors.each do |processor|
      processor.on_error_in(node_type, &block)
    end
  end
end
end

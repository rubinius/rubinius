# depends on: enumerable.rb

##
# A simplified Hash that only works with symbols as keys.

class LookupTable
  include Enumerable

  def empty?
    @entries == 0
  end

  def to_s
    "#<#{self.class}:0x#{object_id.to_s(16)} #{entries} entries>"
  end

  def inspect
    ents = collect { |k,v| " #{k.inspect}=>#{v}" }.join(",")
    "#<#{self.class}" + ents + ">"
  end
end

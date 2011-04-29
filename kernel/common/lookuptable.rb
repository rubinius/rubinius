##
# A simplified Hash that only works with symbols as keys.

class Rubinius::LookupTable
  include Enumerable

  def to_s
    "#<#{self.class}:0x#{object_id.to_s(16)} #{entries} entries>"
  end

  def inspect
    ents = collect { |k,v| " #{k.inspect}=>#{v.inspect}" }.join(",")
    "#<#{self.class}#{ents}>"
  end
end

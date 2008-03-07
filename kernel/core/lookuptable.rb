# depends on: enumerable.rb

class LookupTable
  include Enumerable
  
  alias_method :length,   :size
  alias_method :has_key?, :key?
  alias_method :include?, :key?
  alias_method :member?,  :key?
  alias_method :clone,    :dup

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

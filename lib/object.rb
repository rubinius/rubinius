require 'memory'

class RObject
  def initialize(address)
    @address = address
    @tag = nil
  end
  
  attr_accessor :address, :tag
  
  HeaderSize = 12
  
  def self.setup(heap, klass, fields)
    if klass.kind_of? RObject
      klass = klass.address
    elsif !klass
      klass = 4 # nil
    end
    
    size = HeaderSize + (fields * 4)
    address = heap.allocate(size)
    
    raise "Unable to allocate object" unless address

    Memory.store_long address + 2, 0, klass
    Memory.store_long address + 6, 0, fields
    start = address + HeaderSize
    
    0.upto(fields) do |i|
      Memory.store_long start, i, 4
    end
    
    obj = RObject.new(address)
    obj.flags = 0
    obj.flags2 = 0
    return obj
  end
    
  def memory_size
    HeaderSize + (self.fields * 4)
  end
  
  def flags
    Memory.fetch_byte @address
  end
  
  def flags=(fl)
    Memory.store_byte @address, fl.to_i
  end
  
  def flags2
    Memory.fetch_byte @address + 1
  end
  
  def flags2=(fl)
    Memory.store_byte @address + 1, fl.to_i
  end
  
  def fields
    Memory.fetch_long address + 6, 0
  end
  
  def references
    out = []
    0.upto(fields) do |i|
      obj = at(i)
      if obj.reference?
        out << [obj,i] unless out.find { |ent| ent.first == obj }
      end
    end
    out
  end
  
  attr_reader :address
  
  def rclass
    m = Memory.fetch_long @address + 2, 0
    return RObject.new(m)
  end
  
  def ==(obj)
    @address == obj.address
  end
  
  def at(idx)
    add = Memory.fetch_long field_address(idx), 0
    return RObject.new(add)
  end
  
  def field_address(idx)
    @address + HeaderSize + (idx.to_i * 4)
  end
  
  def raw_put(idx, obj)
    Memory.store_long field_address(idx), 0, obj.address
    return obj
  end
  
  def wb_put(idx, obj)
    raw_put idx, obj
    @@memory_handler.write_barrier self, obj
    return obj
  end
  
  alias :put :raw_put
  
  # Adds the ability to install a memory handler which will
  # be told about write's by sending it #write_barrier.
  def self.use_write_barrier(obj)
    @@memory_handler = obj
    alias :put :wb_put
  end
  
  def self.wrap(val)
    i = case val
    when FalseClass
      0
    when TrueClass
      2
    when NilClass
      4
    when Fixnum
      (val << 1) | 1
    else
      raise ArgumentError, "Unable to wrap #{val}"
    end
    
    return RObject.new(i)
  end
  
  def reference?
    @address > 10 and (@address & 1 == 0)
  end
  
  def immediate?
    @address < 10
  end
  
  def nil?
    @address == 4
  end
  
  def fixnum?
    @address & 1 == 1
  end
  
  def true?
    @address == 2
  end
  
  def false?
    @address == 0
  end
end
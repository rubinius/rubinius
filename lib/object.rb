require 'memory'
require 'types'

class RObject
  def initialize(address)
    @address = address
    @tag = nil
  end
  
  attr_accessor :address, :tag
  
  HeaderSize = 12
  
  def self.setup(heap, klass, fields)
    obj = create(heap, klass, fields)
    obj.initialize_fields
    return obj
  end
  
  def self.create(heap, klass, fields)
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
    obj = RObject.new(address)
    obj.flags = 0
    obj.flags2 = 0
    return obj
  end
  
  def initialize_fields
    start = address + HeaderSize
    
    0.upto(fields) do |i|
      Memory.store_long start, i, 4
    end
    
    return self
  end
  
  def self.setup_bytes(heap, klass, words)
    obj = create(heap, klass, words)
    
    obj.make_byte_storage
    obj.initialize_bytes
    return obj
  end
  
  def initialize_bytes
    Memory.clear_memory byte_start, (fields * 4)
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
  
  def flag_set?(fl)
    flags & fl == fl
  end
  
  def flag_set(fl)
    self.flags = flags | fl
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
  
  def hash_int
    @address
  end
  
  def rclass
    m = Memory.fetch_long @address + 2, 0
    obj = RObject.new(m)
    while obj.reference? and Rubinius::MetaClass.metaclass?(obj)
      obj.as :class
      obj = obj.superclass
    end
    obj.as :class
    return obj
  end
  
  def rclass=(obj)
    Memory.store_long @address + 2, 0, obj.address
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
  
  # Removes any installed write barrier.
  def self.remove_write_barrier
    alias :put :raw_put
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
      fx = (val.abs << 3) | 1
      fx |= 4 if val < 0
      fx
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
  
  def self.nil
    RObject.new(4)
  end
  
  def fixnum?
    @address & 1 == 1 and @address & 2 == 0
  end
  
  def fixnum_neg?
    @address & 4 == 4
  end
  
  def self.symbol(idx)
    fx = idx << 2
    fx |= 3
    out = RObject.new(fx)
    out.as :symbol
    return out
  end
  
  def symbol?
    @address & 3 == 3
  end
  
  def symbol_index
    @address >> 2
  end
  
  def to_int
    fx = @address >> 3
    fx = -fx if fixnum_neg?
    fx
  end
  
  def true?
    @address == 2
  end
  
  def self.true
    RObject.new(2)
  end
  
  def false?
    @address == 0
  end
  
  def self.false
    RObject.new(0)
  end
  
  def self.undef
    RObject.new(6)
  end
  
  def undef?
    @address == 6
  end
  
  StoresBytesFlag = 0x04
  
  def stores_bytes?
    return false unless reference?
    return true if flag_set?(StoresBytesFlag)
    return false
  end
  
  def make_byte_storage
    flag_set StoresBytesFlag
  end
  
  def byte_start
    address + HeaderSize
  end
  
  def store_byte(idx, byte)
    addr = byte_start + idx
    Memory.store_byte addr, byte
  end
  
  def fetch_byte(idx)
    addr = byte_start + idx
    Memory.fetch_byte addr
  end
  
  def as(type)
    extend Rubinius::Types[type]
    return self
  end
  
  def as_string
    "#<RObject:0x#{@address.to_s(16)}>"
  end
  
  def create_metaclass(sup=nil)
    unless sup
      cls = self.direct_class
      sup = cls.metaclass
      # puts "Using metaclass of #{cls.address} (#{sup.address}) for #{address}"
    end
    meta = Rubinius::MetaClass.attach(self)
    meta.superclass = sup
    return meta
  end
  
  def access_metaclass
    m = Memory.fetch_long @address + 2, 0
    obj = RObject.new(m)
    if obj.reference? and !Rubinius::MetaClass.metaclass?(obj)
      # puts "#{obj.inspect} is in the class slot, but not a metaclass!"
      # obj = RObject.nil
      obj = RObject.nil
    end

    return obj
  end
  
  def metaclass
    meta = access_metaclass
    if meta.nil?
      meta = create_metaclass
      meta.setup_fields
    end
    meta.as :metaclass
    return meta
  end
  
  def direct_class
    m = Memory.fetch_long @address + 2, 0
    obj = RObject.new(m)
    obj.as :class
    return obj
  end
end
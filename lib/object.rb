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
  
  def self.create(heap, klass_obj, fields)
    klass = klass_obj.address
        
    size = HeaderSize + (fields * 4)
    address = heap.allocate(size)
    
    raise "Unable to allocate object (#{size} words)" unless address

    Memory.store_long address + 4, 0, klass
    Memory.store_long address + 8, 0, fields
    obj = RObject.new(address)
    obj.flags = 0
    obj.flags2 = 0
    return obj
  end
  
  def initialize_fields
    start = @address + HeaderSize
    i = 0
    0.upto(fields-1) do
      Memory.store_long start, i, 4
      i += 1
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
    cur = Memory.fetch_byte @address
    cur = cur | fl
    Memory.store_byte @address, cur.to_i
  end
  
  def flags2
    Memory.fetch_byte @address + 1
  end
  
  def flags2=(fl)
    Memory.store_byte @address + 1, fl.to_i
  end
  
  def fields
    Memory.fetch_long @address + 8, 0
  end
  
  def references
    out = []
    
    return out if stores_bytes?
    i = 0
    0.upto(fields-1) do
      obj = at(i)
      if obj.reference?
        out << [obj,i]
      end
      i += 1
    end
    out
  end
  
  attr_reader :address
  
  def hash_int
    @address
  end
    
  # T:RObject => bool
  def ==(obj)
    @address == obj.address
  end
  
  def at(idx)
    if idx >= self.fields
      raise "Unable to fetch data beyond end of object (#{idx} > #{self.fields})."
    end
    
    add = Memory.fetch_long field_address(idx), 0
    # Log.debug "Accessing field #{idx} of #{address} => #{add}"
    
    return RObject.new(add)
  end
  
  def field_address(idx)
    @address + HeaderSize + (idx.to_i * 4)
  end
  
  def raw_put(idx, obj)
    if idx >= self.fields
      raise "Unable to write data beyond end of object (#{idx} > #{self.fields})."
    end
    
    # Log.debug "Writing field #{idx} of #{address} => #{obj.address}"
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
    fx = (val.abs << 3) | 1
    fx |= 4 if val < 0
    i = fx
    
    return RObject.new(i)
  end
  
  def to_int
    fx = @address >> 3
    fx = -fx if fixnum_neg?
    fx
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
  
  # hint:skip_translation
  def as(type)
    extend Rubinius::Types[type]
    return self
  end
  
  def copy_fields(dest, count)
    da = dest.byte_start
    sz = count * 4 # 4 should be the size of a long!
    Memory.transfer_memory byte_start, sz, da
  end
  
  def copy_fields_from(dest, first, count)
    da = dest.byte_start
    sz = count * 4 # 4 should be the size of a long!
    start = (first * 4) + byte_start
    Memory.transfer_memory start, sz, da
  end
  
  def copy_bytes_into(dest, count, offset)
    da = dest.byte_start + offset
    Memory.transfer_memory byte_start, count, da
  end
  
  def as_string
    "#<RObject:0x" + @address.to_s(16) + ">"
  end
  
  IsMetaFlag = 0x80
  
  def rclass
    m = Memory.fetch_long @address + 4, 0
    obj = RObject.new(m)
    while obj.reference? and obj.flag_set?(IsMetaFlag)
      obj.as :class
      obj = obj.superclass
    end
    obj.as :class
    return obj
  end
  
  def rclass=(obj)
    Memory.store_long @address + 4, 0, obj.address
  end
  
  def create_metaclass(sup)
    unless sup
      sup = self.direct_class
      # sup = cls.metaclass
      # puts "Using metaclass of #{cls.address} (#{sup.address}) for #{address}"
    end
    meta = Rubinius::MetaClass.attach(self)
    meta.superclass = sup
    return meta
  end
  
  def access_metaclass
    m = Memory.fetch_long @address + 4, 0
    obj = RObject.new(m)
    if obj.reference? and !obj.flag_set?(IsMetaFlag)
      # puts "#{obj.inspect} is in the class slot, but not a metaclass!"
      # obj = RObject.nil
      obj = RObject.nil
    end

    return obj
  end
  
  def metaclass
    meta = access_metaclass
    if meta.nil?
      meta = create_metaclass(nil)
      meta.setup_fields
    end
    meta.as :metaclass
    return meta
  end
  
  def direct_class
    m = Memory.fetch_long @address + 4, 0
    obj = RObject.new(m)
    obj.as :class
    return obj
  end
  
  def logical_class
    if reference?
      return direct_class
    else
      return immediate_class
    end
  end
  
  def real_class
    if reference?
      return rclass
    else
      return immediate_class
    end
  end
  
  def immediate_class
    # Log.debug "Looking logical class of immediate #{self.address}"
    if fixnum?
      return CPU::Global.fixnum
    elsif symbol?
      return CPU::Global.symbol
    elsif true?
      return CPU::Global.true_class
    elsif false?
      return CPU::Global.false_class
    elsif nil?
      return CPU::Global.nil_class
    elsif undef?
      return CPU::Global.undef_class
    else
      raise "Fuck. can't figure the class of #{self.address} out."
    end
  end
end
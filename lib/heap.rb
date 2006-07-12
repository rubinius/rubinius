require 'memory'
require 'object'

class Heap
  def initialize(size)
    @size = size
    allocate_memory
  end
  
  def deallocate
    return unless allocated?
    Memory.release_memory @address
    @address = 0
  end
  
  def allocate_memory
    @address = Memory.allocate_memory @size
    Memory.clear_memory @address, @size
    @current = 0
  end
  
  def reset!
    @current = 0
  end
  
  attr_accessor :current
  
  def contains?(addr)
    return false if addr < @address
    return false if addr >= @address + @size
    return true
  end
  
  def allocated?
    @address > 0
  end
  
  attr_reader :address, :size, :current
  
  def allocate(size)
    return nil unless enough_space?(size)
    addr = @address + @current
    @current += size
    return addr
  end
  
  def enough_space?(size)
    if @size - current < size
      return false
    end
    
    return true
  end
  
  true
  
  def copy_object(obj)
    return obj if contains?(obj.address)
    sz = obj.memory_size
    d2 = allocate(sz)
    return nil unless d2
    Memory.transfer_memory obj.address, sz, d2
    return RObject.new(d2)
  end
  
  def each_object
    addr = @address
    last = @address + @current
    while addr < last
      obj = RObject.new(addr)
      yield obj
      addr += obj.memory_size
    end
  end
end
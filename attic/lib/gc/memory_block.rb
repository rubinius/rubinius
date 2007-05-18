require 'memory'

class MemoryBlock
  def initialize(size, clear=false)
    @size = size
    @address = Memory.allocate_memory size
    if clear
      Memory.clear_memory @address, @size
    end
  end
  
  def deallocate
    Memory.release_memory @address
  end
  
  attr_reader :address, :size
    
  def byte(index)
    Memory.fetch_byte(@address + index)
  end
  
  def long(index)
    Memory.fetch_long @address, index
  end
  
  def store_byte(index, data)
    Memory.store_byte(@address + index, data)
  end
  
  def store_long(index, data)
    Memory.store_long(@address, index, data)
  end
  
  def long_in_bounds?(index)
    long = Memory.long_size
    start = index * long
    if start > @size - long
      return false
    end
    
    return true
  end
  
  def byte_in_bounds?(index)
    if index > @size
      return false
    end
    
    return true
  end
end
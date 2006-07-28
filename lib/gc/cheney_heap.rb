require 'heap'

class CheneyHeap < Heap
  def initialize(size)
    super
    @scan = @address
    @next = @address
  end
  
  attr_accessor :scan, :next
  
  def allocate(size)
    addr = @next
    @next += size
    return addr
  end
  
  def fully_scanned?
    @scan == @next
  end
  
  def unscanned_objects
    return if fully_scanned?
    until @scan >= @next
      obj = RObject.new(@scan)
      yield obj
      @scan += obj.memory_size
    end
  end
end
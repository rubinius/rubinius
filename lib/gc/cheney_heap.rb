require 'gc/heap'

class CheneyHeap < Heap
  def initialize(size)
    super(size)
    @scan = @address
  end
  
  attr_accessor :scan
  
  def next
    @current
  end
  
  def fully_scanned?
    @scan == @current
  end
  
  def unscanned_objects
    return if fully_scanned?
    until @scan >= @current
      obj = RObject.new(@scan)
      yield obj
      @scan += obj.memory_size
    end
  end
  
  def next_unscanned
    return nil if fully_scanned?
    obj = RObject.new(@scan)
    @scan += obj.memory_size
    return obj
  end
end

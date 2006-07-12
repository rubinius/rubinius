require 'heap'
require 'object'
require 'gc'

class MarkSweepGC < GarbageCollector
  def initialize(size)
    super()
    @heap = Heap.new(size)
    @size = size
    @used = 0
    @start = @heap.allocate(size)
    @allocations = [[@start, size]]
    @robject = RObject.new(0) # Used to calculate object memory sizes.
  end
  
  def destroy!
    @heap.deallocate
    @size = 0
    @used = 0
  end
  
  def space_at_end?(sz)
    sz < @size - @used
  end
  
  def allocate(size)
    if false and space_at_end?(size)
      addr = @heap.allocate(size)
      @used += size
      return addr
    end
    
    addr = find_allocation(size)
    return addr
  end
  
  def find_allocation(size)
    @allocations.each do |ent|
      addr, addr_size = ent
      
      # puts "msgc: #{[addr_size, size, addr].inspect}"
      
      if addr_size >= size
        rem = addr_size - size
        if rem == 0
          @allocations.delete(ent)
        else
          ent[0] = addr + size
          ent[1] = rem
        end
        # puts "  ment: #{ent.inspect}"
        return addr
      end
    end
    
    return nil
  end
  
  def reclaim_address(addr)
    @robject.address = addr
    @allocations.unshift [addr, @robject.memory_size]
  end
  
  def contains?(obj)
    @heap.contains?(obj.address)
  end
  
  alias :mutate_object :mark_object
    
  alias :already_mutated? :object_marked?
  
  # Mark/Sweep doesn't move a mutate object, so we just return the same object.
  def fetch_new_mutation(obj)
    obj
  end
  
  alias :collect :mutate
    
  def sweep
    each_object do |obj|
      if object_marked?(obj)
        unmark_object obj
      else
        reclaim_address obj.address
      end
    end
  end
  
  def sort_allocations
    @allocations.sort do |a,b|
      a[0] <=> b[0]
    end
  end
  
  def reorder!
    lst = sort_allocations
    @allocations = [lst.shift]
    
    lst.each do |addr, size|
      curr = @allocations.last
      # puts "CURR: #{curr.inspect}, #{addr}, #{curr.first + curr.last}"
      if curr.first + curr.last == addr
        curr[-1] += size
      else
        @allocations << [addr, size]
      end
    end
  end
  
  def regions_in_use
    holes = sort_allocations
    
    if holes.first[0] == @start
      ent = holes.shift
      start = ent[0] + ent[1]
    else
      start = @start
    end
    
    out = []
    curr = start
    until holes.empty?
      ent = holes.shift
      sz = ent[0] - curr
      if sz > 0
        out << [curr, sz]
      end
      curr = ent[0] + ent[1]
    end
    return out
  end
  
  def each_object
    regions_in_use.each do |addr, addr_size|
      curr = addr
      last = addr + addr_size
      while curr < last
        obj = RObject.new(curr)
        yield obj
        curr += obj.memory_size
      end
    end
  end
  
  def clear_marks!
    each_object do |obj|
      unmark_object(obj)
    end
  end
  
  def after_mutation
  end
  
  def complete_cycle(roots)
    collect roots
    sweep
    reorder!
  end
end
require 'baker_gc'
require 'mark_sweepgc'

class HybridGC < GarbageCollector
  def initialize(eden_size, mature_size)
    super()
    # Where all objects begin...
    @eden = BakerGC.new(eden_size)
    @eden.stack = @stack
    
    # if they live long enough, they are moved here. Objects
    # here are still young, but have a higher probability of living
    # on.
    @survivor = BakerGC.new(eden_size)
    @survivor.stack = @stack
    
    # If life has treated an object well, it is promoted to the mature
    # space, where it lives out it's life.
    @mature = MarkSweepGC.new(mature_size)
    @mature.stack = @stack
    
    # Controls how many times @eden is swept before
    # moving an object to survivor.
    @compacts_before_survivor = 2
    
    # Counter tracking how many compacts for survivor.
    @compacts_til_survivor = @compacts_before_survivor
        
    # Controls how many times @survivor is swept before
    # promotion is run again.
    @compacts_before_mature = 5
    
    # Counter tracking how many compacts need to be done.
    @compacts_til_mature = @compacts_before_mature
    
  end
  
  attr_reader :compacts_til_mature, :compacts_til_survivor
  attr_accessor :compacts_before_mature, :compacts_before_survivor
  
  def allocate(size)
    @eden.allocate(size)
  end
  
  def clean_eden(roots)
    @eden.collect roots
  end
  
  def clean_survivor(roots)
    @survivor.collect roots
  end
  
  def promote(obj, where)
    size = obj.memory_size
    addr = where.allocate(size)
    Memory.transfer_memory obj.address, size, addr
    nw = RObject.new(addr)
    update_stack obj, nw
    return nw
  end
  
  def promote_to_mature(obj)
    promote obj, @mature
  end
  
  def promote_to_survivor(obj)
    promote obj, @survivor
  end
  
  def promote_between(from, to)
    # A hash of all objects promoted in this go around.
    promoted = {}
    
    # Go through eden and promote all objects that are marked.
    from.each_object do |obj|
      if object_marked?(obj)
        # puts "unmarking: #{obj.address}"
        unmark_object(obj)
        promoted[obj.address] = promote(obj, to)
      else
        # Otherwise mark it so that the next time around we promote
        # it.
        mark_object(obj)
      end
    end
    
    # For all objects in +from+...
    from.each_object do |obj|
      # look at all references
      obj.references.each do |ref, idx|
        # and if +ref+ was promoted, update the reference
        if promo = promoted[ref.address]
          obj.put idx, promo
        end
      end
    end
    
    from.remember_set.each do |obj|
      obj.references.each do |ref, idx|
        if promo = promoted[ref.address]
          obj.put idx, promo
          # migrate remember set objects to the to space too.
          to.remember_set << obj
        end
      end
    end
    
    # puts "promoted #{promoted.size} from #{from} to #{to}"
  end
  
  def update_remember_sets(obj, ref)
    if immature?(ref)
      unless immature? obj
        @eden.remember_set << obj
      end
    elsif survivor?(ref)
      
    end
  end
  
  def compact(roots)
    clean_eden roots

    # Order here is important. We have to move from survivor to mature
    # before we move from eden to survivor so that objects we JUST moved
    # from eden don't get picked up in the sweep of mature.
    if @compacts_til_mature == 0
      promote_between @survivor, @mature
      @compacts_til_mature = @compacts_before_mature
    else
      @compacts_til_mature -= 1
    end
    
    if @compacts_til_survivor == 0
      clean_survivor roots
      promote_between @eden, @survivor
      @compacts_til_survivor = @compacts_before_survivor
    else
      @compacts_til_survivor -= 1
    end
    
  end
  
  def mature?(obj)
    @mature.contains?(obj)
  end
  
  def survivor?(obj)
    @survivor.contains?(obj)
  end
  
  def immature?(obj)
    @eden.contains?(obj)
  end
end
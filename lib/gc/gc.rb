class GarbageCollector
  
  def initialize
    # The set of non-root, non-local objects which have references to
    # objects contained in this 2 space.
    @remember_set = []
    
    # The stack is used to keep track of RObject's which are located
    # on a running stack and must be updated with a new address upon
    # compacting.
    @stack = Hash.new    
  end
  
  # Allow for someone to set the stack to something else.
  attr_accessor :stack
  
  attr_reader :remember_set
  
  def mutate_from(obj, mutator=self)
    if contains?(obj)
      mutator.mutate_object obj
    end
    
    stack = [obj]
    until stack.empty?
      iobj = stack.pop
      iobj.references.each do |ref, idx|
        if mutator.already_mutated?(ref)
          new_obj = mutator.fetch_new_mutation(ref)
        elsif contains?(ref)
          new_obj = mutator.mutate_object ref
          # puts "#{ref.address}(#{idx}) => #{new_obj.address}"
          # puts "    #{new_obj.at(0).inspect}"
          stack.unshift new_obj
        else
          # The reference isn't for an object contained in this
          # space.
          new_obj = ref
          stack.unshift new_obj 
        end
        iobj.put idx, new_obj
      end
    end
  end
  
  def mutate(roots)
    (@remember_set + roots).each do |root_obj|
      mutate_from root_obj
    end
    
    after_mutation
  end
  
  def on_stack(*objs)
    objs.each do |obj|
      @stack[obj.address] = obj
    end
  end
  
  def update_stack(old, nw)
    if obj = @stack.delete(old.address)
      obj.address = nw.address
      @stack[nw.address] = obj
    end
  end
  
  MarkBit = 0x40
  
  def mark_object(obj)
    obj.flags = obj.flags | MarkBit
    return obj
  end
  
  def unmark_object(obj)
    obj.flags = obj.flags ^ MarkBit
  end
  
  def object_marked?(obj)
    obj.flags & MarkBit == MarkBit
  end
  
end

class ForwardingGC < GarbageCollector
  ForwardingMagic = 0xff
  
  def forwarded?(obj)
    byte = Memory.fetch_byte obj.address
    byte == ForwardingMagic
  end
  
  def forwarding_object(obj)
    o = Memory.fetch_long obj.address + 1, 0
    return RObject.new(o)
  end
  
  def set_forwarding_address(obj, dest)
    Memory.store_byte obj.address, ForwardingMagic
    Memory.store_long obj.address + 1, 0, dest.address
  end
end
require 'gc/heap'
require 'object'
require 'gc/gc'

class BakerGC < ForwardingGC
  def initialize(size)
    super()
    @space_a = Heap.new(size)
    @space_b = Heap.new(size)
        
    @current = @space_a
    @next = @space_b
  end
    
  def swap!
    @current, @next = @next, @current
    @next.reset!
  end
  
  def destroy!
    @space_a.deallocate
    @space_b.deallocate
  end
  
  def allocate(size)
    @current.allocate(size)
  end
  
  def new_object(klass, fields)
    RObject.new self, klass, fields
  end
        
  def mutate_object(obj)
    dest = @next.copy_object obj
    
    set_forwarding_address obj, dest
    update_stack obj, dest
    
    return dest
  end
  
  def each_object(&b)
    @current.each_object(&b)
  end
  
  def contains?(obj)
    @current.contains?(obj.address)
  end
  
  alias :already_mutated? :forwarded?
  alias :fetch_new_mutation :forwarding_object
  
  alias :collect_from :mutate_from
  alias :collect :mutate
  
  def after_mutation
    swap!
  end
end

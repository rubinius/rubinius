require 'gc/cheney_heap'
require 'object'
require 'gc/gc'

class BakerGC < ForwardingGC
  def initialize(size)
    super()
    @space_a = CheneyHeap.new(size)
    @space_b = CheneyHeap.new(size)
        
    @current = @space_a
    @next = @space_b
  end
  
  def start_address
    @current.address
  end
  
  def used
    @current.current - @current.address
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
  
  def mutate_from(iobj)
    if already_mutated?(iobj)
      iobj = fetch_new_mutation(iobj)
    elsif contains?(iobj)
      iobj = mutate_object iobj
    end
    
    ret = iobj
        
    while iobj
      cls = iobj.direct_class
      if cls.reference?
        if already_mutated?(cls)
          new_cls = fetch_new_mutation(cls)
        elsif contains?(cls)
          new_cls = mutate_object cls
        else
          new_cls = cls
        end

        # puts "Mutating #{iobj.address} / #{cls.address} => #{new_cls.address}"
      
        iobj.rclass = new_cls
      else
        # puts "#{iobj.address} has a non-ref class!"
      end
      
      iobj.references.each do |ref, idx|
        if already_mutated?(ref)
          new_obj = fetch_new_mutation(ref)
        elsif contains?(ref)
          new_obj = mutate_object ref
        else
          # The reference isn't for an object contained in this
          # space.
          new_obj = ref
        end
        iobj.put idx, new_obj
      end
      iobj = @next.next_unscanned 
    end
    
    return ret
  end
  
  alias :already_mutated? :forwarded?
  alias :fetch_new_mutation :forwarding_object
  
  alias :collect_from :mutate_from
  alias :collect :mutate
  
  
  def after_mutation
    swap!
  end
end

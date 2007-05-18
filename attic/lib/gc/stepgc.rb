require 'heap'
require 'object'
require 'gc'

class StepGC < ForwardingGC
  def initialize(steps, step_sz, older_gc)
    @num_steps = steps
    @step_size = step_sz
    @steps = []
    @older_gc = older_gc
    
    steps.times { add_step }
  end
  
  def add_step
    step = Heap.new(@step_size)
    @steps << step
  end
  
  def promote
    oldest = @steps.pop
    oldest.each_object do |obj|
      
    end
  end
  
  def mutate_object(obj)
    dest = @.copy_object obj
    
    set_forwarding_address obj, dest
    update_stack obj, dest
    
    return dest
  end
  
end

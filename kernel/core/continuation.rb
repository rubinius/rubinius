class Continuation
  def initialize(task)
    @task = task
  end
  
  def call(value=nil)
    task = @task.dup
    task.instance_variable_set(:@__value__, value)
    Task.current = task
  end

  alias :[] :call
end

module Kernel
  def callcc(&block)
    first = true
    task = Task.current.dup
    if first
      first = false
      cont = Continuation.new(task)
      block.call(cont)
      return nil
    else
      return task.instance_variable_get(:@__value__)
    end
  end
end

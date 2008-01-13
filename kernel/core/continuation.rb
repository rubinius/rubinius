# depends on: class.rb module.rb

class Continuation
  def initialize(task=nil)
    @task = task
    @value = nil
  end
  
  def task=(task)
    @task = task
  end
  
  def value
    @value
  end
  
  def call(*value)
    task = @task.dup
    if value.empty?
      @value = nil
    elsif value.size == 1
      @value = value.pop
    else
      @value = value
    end
    Task.current = task
  end

  alias_method :[], :call
end

module Kernel
  def callcc
    cont = Continuation.new
    # Task#dup appears as though it returns nil in the dup'd
    # task, kinda like fork().
    task = Task.current.dup
    if task
      cont.task = task
      yield cont
    else
      return cont.value
    end
  end
  module_function :callcc
end

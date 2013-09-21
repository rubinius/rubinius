class Continuation
#  def self.create(task=nil)
#    cont = self.allocate
#    cont.setup(task)
#    cont
#  end
#
#  def setup(task=nil)
#    @task = task
#    @value = nil
#  end
#
#  def call(*value)
#    task = @task.dup
#    if value.empty?
#      @value = nil
#    elsif value.size == 1
#      @value = value.pop
#    else
#      @value = value
#    end
#    Rubinius::Task.current = task
#  end
#
#  alias_method :[], :call
end

module Kernel

  def callcc
    raise NotImplementedError, "call/cc is not implemented!"
#    cont = Continuation.create
#    # Task#dup appears as though it returns nil in the dup'd
#    # task, kinda like fork().
#    task = Rubinius::Task.current.dup
#    if task
#      cont.task = task
#      yield cont
#    else
#      return cont.value
#    end
  end
  module_function :callcc
end

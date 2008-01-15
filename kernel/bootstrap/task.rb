class Task
  def self.current
    Ruby.primitive :task_current
  end

  def self.create
    Ruby.primitive :task_dup
  end

  def self.new(&pr)
    raise "Must pass in a block." unless pr
    t = Task.create
    t.associate pr.block
    return t
  end

  def dup
    Ruby.primitive :task_dup
  end

  def at(index)
    Ruby.primitive :task_at
  end

  def main
    at(0)
  end

  def current_context
    at(1)
  end

  def inspect
    "#<Task:#{self.object_id.to_s(16)}>"
  end

  def self.current=(task)
    Ruby.primitive :task_set_current
    raise ArgumentError, "Unable to set '#{task.inspect}' as the current Task"
  end

  def associate(be)
    Ruby.primitive :task_associate
  end

  def set_debugging(dc, cc)
    Ruby.primitive :task_set_debugging
  end

  def debug_channel
    Ruby.primitive :task_debug_channel
  end

  def control_channel
    Ruby.primitive :task_control_channel
  end
  
  def raise(exc)
    Ruby.primitive :task_raise
  end

  # Returns the current size of the stack for this task.
  def stack_size
    Ruby.primitive :task_stack_size
  end

  # Returns the value at the specified depth on the task stack, where depth is
  # a positive integer counting down from the top of the stack.
  def get_stack_value(depth)
    Ruby.primitive :task_get_stack_value
  end
end

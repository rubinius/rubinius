class Task
  def self.current
    Ruby.primitive :task_current
    Kernel.raise PrimitiveFailure, "primitive failed"
  end

  def self.create
    Ruby.primitive :task_dup
    Kernel.raise PrimitiveFailure, "primitive failed"
  end

  def self.new(&pr)
    Kernel.raise "Must pass in a block." unless pr
    t = Task.create
    t.associate pr.block
    return t
  end

  def dup
    Ruby.primitive :task_dup
    Kernel.raise PrimitiveFailure, "primitive failed"
  end

  def at(index)
    Ruby.primitive :task_at
    Kernel.raise PrimitiveFailure, "primitive failed"
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
    Kernel.raise ArgumentError, "Unable to set '#{task.inspect}' as the current Task"
  end

  def associate(be)
    Ruby.primitive :task_associate
    Kernel.raise PrimitiveFailure, "primitive failed"
  end

  def set_debugging(dc, cc)
    Ruby.primitive :task_set_debugging
    Kernel.raise PrimitiveFailure, "primitive failed"
  end

  def debug_channel
    Ruby.primitive :task_debug_channel
    Kernel.raise PrimitiveFailure, "primitive failed"
  end

  def control_channel
    Ruby.primitive :task_control_channel
    Kernel.raise PrimitiveFailure, "primitive failed"
  end

  # Returns the current state of the debug_context_change flag
  def debug_context_change
    Ruby.primitive :task_get_debug_context_change
    Kernel.raise PrimitiveFailure, "primitive failed"
  end
  
  # Sets a flag on the task which will cause the task to yield to the debugger
  # when the context changes. This is used to handle breakpoints that need to be
  # triggered following a control flow change, e.g. after a send or return.
  def debug_context_change=(val)
    Ruby.primitive :task_set_debug_context_change
    Kernel.raise PrimitiveFailure, "primitive failed"
  end
  
  def raise(exc)
    Ruby.primitive :task_raise
    Kernel.raise PrimitiveFailure, "primitive failed"
  end

  # Returns the current size of the stack for this task.
  def stack_size
    Ruby.primitive :task_stack_size
    Kernel.raise PrimitiveFailure, "primitive failed"
  end

  # Returns the value at the specified depth on the task stack, where depth is
  # a positive integer counting down from the top of the stack.
  def get_stack_value(depth)
    Ruby.primitive :task_get_stack_value

    # If we get here, the primitive failed
    # Kernel raise is used, since we don't want to use the raise primitive above
    Kernel.raise ArgumentError, "Task stack index out of range"
  end
end

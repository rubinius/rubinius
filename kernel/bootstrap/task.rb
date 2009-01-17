class Rubinius::Task
  attr_accessor :probe

  def self.current
    Ruby.primitive :task_current
    Kernel.raise PrimitiveFailure, "Rubinius::Task.current primitive failed"
  end

  def self.create
    Ruby.primitive :task_dup
    Kernel.raise PrimitiveFailure, "Rubinius::Task.create primitive failed"
  end

  def self.new(&pr)
    Kernel.raise "Must pass in a block." unless pr
    t = create
    t.associate pr.block
    return t
  end

  def dup
    Ruby.primitive :task_dup
    Kernel.raise PrimitiveFailure, "Rubinius::Task#dup primitive failed"
  end

  def current_context
    Ruby.primitive :task_current_context
    Kernel.raise PrimitiveFailure, 'Rubinius::Task#current_context primitive failed'
  end

  def inspect
    "#<#{self.class}:#{self.object_id.to_s(16)}>"
  end

  def self.current=(task)
    Ruby.primitive :task_set_current
    Kernel.raise ArgumentError, "Unable to set '#{task.inspect}' as the current task"
  end

  def call_object(obj, meth, ary)
    Ruby.primitive :task_call_object
    Kernel.raise PrimitiveFailure, "Unable to call into task"
  end

  def associate(be)
    call_object be, :call, []
  end

  def set_debugging(dc, cc)
    Ruby.primitive :task_set_debugging
    Kernel.raise PrimitiveFailure, "Rubinius::Task#set_debugging primitive failed"
  end

  def debug_channel
    Ruby.primitive :task_debug_channel
    Kernel.raise PrimitiveFailure, "Rubinius::Task#debug_channel primitive failed"
  end

  def control_channel
    Ruby.primitive :task_control_channel
    Kernel.raise PrimitiveFailure, "Rubinius::Task#control_channel primitive failed"
  end

  # Returns the current state of the debug_context_change flag
  def debug_context_change
    Ruby.primitive :task_get_debug_context_change
    Kernel.raise PrimitiveFailure, "Rubinius::Task#debug_context_change primitive failed"
  end

  # Sets a flag on the task which will cause the task to yield to the debugger
  # when the context changes. This is used to handle breakpoints that need to be
  # triggered following a control flow change, e.g. after a send or return.
  def debug_context_change=(val)
    Ruby.primitive :task_set_debug_context_change
    Kernel.raise PrimitiveFailure, "Rubinius::Task#debug_context_change= primitive failed"
  end

  def raise(exc)
    Ruby.primitive :task_raise
    Kernel.raise PrimitiveFailure, "Rubinius::Task#raise primitive failed"
  end
end

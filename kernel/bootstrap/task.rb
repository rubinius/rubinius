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
    "#<Task:#{self.object_id.to_s(16)} #{main.inspect}>"
  end

  def self.current=(task)
    Ruby.primitive :task_set_current
  end

  def associate(be)
    Ruby.primitive :task_associate
  end

  def set_debugging(dc, cc)
    Ruby.primitive :task_set_debugging
  end
end

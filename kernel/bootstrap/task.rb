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

  def swap(task)
    Ruby.primitive :task_swap
  end

  def switch
    Task.current.swap(self)
  end

  def associate(be)
    Ruby.primitive :task_associate
  end
end

class Channel
  def self.new
    Ruby.primitive :channel_new
  end

  def send(obj)
    Ruby.primitive :channel_send
  end

  def receive
    Ruby.primitive :channel_receive
  end

  def waiting
    at(0)
  end

  def value
    at(1)
  end

  def send_in_microseconds(count)
    Ruby.primitive :channel_send_in_microseconds
  end

  def send_on_readable(io)
    Ruby.primitive :channel_send_on_readable
  end

  def send_on_writable(io)
    Ruby.primitive :channel_send_on_readable
  end

  def send_on_signal(number)
    Ruby.primitive :channel_send_on_signal
  end
end

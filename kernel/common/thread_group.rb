class ThreadGroup
  def initialize
    @threads = []
  end

  def add(thread)
    if thread.group
      thread.group.remove(thread)
    end
    thread.add_to_group self
    prune
    @threads << WeakRef.new(thread)
    self
  end

  def prune
    @threads.delete_if { |w| !w.weakref_alive? or !w.object.alive? }
  end

  private :prune

  def remove(thread)
    prune
    @threads.delete_if { |w| w.object == thread }
  end

  def list
    prune
    @threads.map { |w| w.object }
  end

  def list_all
    @threads.delete_if { |w| !w.weakref_alive? }
    @threads.map { |w| w.object }
  end

end

class ThreadGroup
  def initialize
    @threads = []
  end

  def add(thread)
    if thread.group
      thread.group.remove(thread)
    end
    thread.add_to_group self

    @threads.delete_if { |w| !w.weakref_alive? or !w.object.alive? }
    @threads << WeakRef.new(thread)
    self
  end

  def remove(thread)
    @threads.delete_if { |w| !w.weakref_alive? or !w.object.alive? or w.object == thread }
  end

  def list
    list = []
    @threads.each { |w| list << w.object if w.weakref_alive? and w.object.alive? }
    list
  end
end

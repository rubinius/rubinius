class ThreadGroup
  def initialize
    @threads = []
    @enclosed = false
  end

  Default = ThreadGroup.new

  def add(thread)
    if thread.group
      thread.group.remove(thread)
    end
    thread.add_to_group self

    @threads.delete_if do |w|
      obj = w.__object__
      !(obj and obj.alive?)
    end

    @threads << WeakRef.new(thread)
    self
  end

  def remove(thread)
    if enclosed?
      raise ThreadError, "can't move from the enclosed thread group"
    end
    @threads.delete_if { |w| w.__object__ == thread }
  end

  def list
    list = []
    @threads.each do |w|
      obj = w.__object__
      list << obj if obj and obj.alive?
    end
    list
  end

  def enclose
    @enclosed = true
    self
  end

  def enclosed?
    @enclosed
  end
end

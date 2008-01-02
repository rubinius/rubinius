# depends on: class.rb

class ThreadGroup
  def initialize
    @threads = []
  end
  
  def add(thread)
    if thread.group
      thread.group.remove(thread)
    end
    thread.add_to_group self
    @threads << thread
    self
  end
  
  def remove(thread)
    @threads.delete(thread)
  end
  
  def list
    @threads
  end
end
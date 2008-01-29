class TimerAction
  attr_reader :start, :stop
  
  def register
    MSpec.register :start, self
    MSpec.register :finish, self
  end
  
  def start
    @start = Time.now
  end
  
  def finish
    @stop = Time.now
  end
  
  def format
    "Finished in %f seconds" % (@stop - @start)
  end
end

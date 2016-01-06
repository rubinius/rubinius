class TimerAction
  def register
    MSpec.register :start, self
    MSpec.register :finish, self
  end

  def start
    @start = current_time
  end

  def finish
    @stop = current_time
  end

  def elapsed
    @stop - @start
  end

  def format
    "Finished in %f seconds" % elapsed
  end

  def current_time
    Time.now
  end
end

class MockProxy
  def count
    @count ||= [:exactly, 0]
  end
  
  def arguments
    @arguments
  end
  
  def returning
    @returning
  end
  
  def calls
    @calls ||= 0
  end
  
  def called
    @calls = calls + 1
  end
  
  def exactly(n)
    @count = [:exactly, n_times(n)]
    self
  end
  
  def at_least(n)
    @count = [:at_least, n_times(n)]
    self
  end
  
  def at_most(n)
    @count = [:at_most, n_times(n)]
    self
  end
  
  def once
    exactly 1
  end
  
  def twice
    exactly 2
  end
  
  def any_number_of_times
    at_least 0
  end
  
  def with(*args)
    raise ArgumentError, "you must specify the expected arguments" if args.empty?
    @arguments = args
    self
  end
  
  def and_return(*args)
    @returning = args
    self
  end
  
  private
  
  def n_times(n)
    case n
    when :once
      1
    when :twice
      2
    else
      Integer n
    end
  end
end

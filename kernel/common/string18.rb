class String
  include Enumerable

  alias_method :each, :each_line

  def upto(stop, exclusive=false)
    stop = StringValue(stop)
    return self if self > stop

    after_stop = exclusive ? stop : stop.succ
    current = self

    until current == after_stop
      yield current
      current = StringValue(current.succ)
      break if current.size > stop.size || current.size == 0
    end

    self
  end

end

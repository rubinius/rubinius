class Time

  def inspect
    if @is_gmt
      strftime("%Y-%m-%d %H:%M:%S UTC")
    else
      strftime("%Y-%m-%d %H:%M:%S %z")
    end
  end

  alias_method :to_s, :inspect

  # TODO: MRI uses struct timespec in 1.9
  def nsec
    0
  end

  alias_method :tv_nsec, :nsec
end

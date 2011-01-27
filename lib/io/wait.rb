class IO
  # Return self if the IO object has data ready to read.
  def ready?
    return false if closed?
    return self  if IO.select([self], nil, nil, 0)
  end

  # Wait until IO has data to read.
  #
  # +timeout+ specifies how long to wait for data.
  def wait(timeout=nil)
    return false if closed?
    return self  if IO.select([self], nil, nil, timeout)
  end
end


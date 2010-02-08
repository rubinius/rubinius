def Bench.run
  i = 0
  while @should_run
    # string#sub(...)
    raise "string#sub(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

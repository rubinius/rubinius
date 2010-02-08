def Bench.run
  i = 0
  while @should_run
    # string#intern(...)
    raise "string#intern(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

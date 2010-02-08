def Bench.run
  i = 0
  while @should_run
    # string#partition(...)
    raise "string#partition(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

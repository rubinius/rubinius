def Bench.run
  i = 0
  while @should_run
    # string#next(...)
    raise "string#next(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

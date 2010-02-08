def Bench.run
  i = 0
  while @should_run
    # string#size(...)
    raise "string#size(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

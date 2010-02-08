def Bench.run
  i = 0
  while @should_run
    # string#rpartition(...)
    raise "string#rpartition(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

def Bench.run
  i = 0
  while @should_run
    # string#inspect(...)
    raise "string#inspect(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

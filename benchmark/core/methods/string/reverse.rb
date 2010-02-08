def Bench.run
  i = 0
  while @should_run
    # string#reverse(...)
    raise "string#reverse(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

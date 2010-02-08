def Bench.run
  i = 0
  while @should_run
    # string#scan(...)
    raise "string#scan(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

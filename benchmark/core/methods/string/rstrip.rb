def Bench.run
  i = 0
  while @should_run
    # string#rstrip(...)
    raise "string#rstrip(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

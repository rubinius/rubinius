def Bench.run
  i = 0
  while @should_run
    # string#count(...)
    raise "string#count(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

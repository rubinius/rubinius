def Bench.run
  i = 0
  while @should_run
    # string#sum(...)
    raise "string#sum(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

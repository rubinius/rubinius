def Bench.run
  i = 0
  while @should_run
    # string#split(...)
    raise "string#split(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

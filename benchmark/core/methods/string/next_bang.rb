def Bench.run
  i = 0
  while @should_run
    # string#next_bang(...)
    raise "string#next_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

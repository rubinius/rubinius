def Bench.run
  i = 0
  while @should_run
    # string#reverse_bang(...)
    raise "string#reverse_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

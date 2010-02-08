def Bench.run
  i = 0
  while @should_run
    # string#delete_bang(...)
    raise "string#delete_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

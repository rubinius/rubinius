def Bench.run
  i = 0
  while @should_run
    # string#strip_bang(...)
    raise "string#strip_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

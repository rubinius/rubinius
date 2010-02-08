def Bench.run
  i = 0
  while @should_run
    # string#chop_bang(...)
    raise "string#chop_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

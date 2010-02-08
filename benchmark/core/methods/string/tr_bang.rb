def Bench.run
  i = 0
  while @should_run
    # string#tr_bang(...)
    raise "string#tr_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

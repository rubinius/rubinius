def Bench.run
  i = 0
  while @should_run
    # string#tr_s_bang(...)
    raise "string#tr_s_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

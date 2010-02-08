def Bench.run
  i = 0
  while @should_run
    # string#tr_s(...)
    raise "string#tr_s(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

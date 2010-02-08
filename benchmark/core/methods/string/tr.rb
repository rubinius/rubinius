def Bench.run
  i = 0
  while @should_run
    # string#tr(...)
    raise "string#tr(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

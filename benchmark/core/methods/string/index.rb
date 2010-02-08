def Bench.run
  i = 0
  while @should_run
    # string#index(...)
    raise "string#index(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

def Bench.run
  i = 0
  while @should_run
    # string#to_f(...)
    raise "string#to_f(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

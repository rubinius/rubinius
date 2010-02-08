def Bench.run
  i = 0
  while @should_run
    # string#to_s(...)
    raise "string#to_s(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

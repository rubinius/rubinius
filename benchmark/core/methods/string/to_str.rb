def Bench.run
  i = 0
  while @should_run
    # string#to_str(...)
    raise "string#to_str(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

def Bench.run
  i = 0
  while @should_run
    # string#to_i(...)
    raise "string#to_i(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

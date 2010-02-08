def Bench.run
  i = 0
  while @should_run
    # string#equal_op(...)
    raise "string#equal_op(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

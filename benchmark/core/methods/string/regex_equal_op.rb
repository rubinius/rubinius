def Bench.run
  i = 0
  while @should_run
    # string#regex_equal_op(...)
    raise "string#regex_equal_op(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

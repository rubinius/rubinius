def Bench.run
  i = 0
  while @should_run
    # string#lines(...)
    raise "string#lines(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

def Bench.run
  i = 0
  while @should_run
    # string#swapcase_bang(...)
    raise "string#swapcase_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

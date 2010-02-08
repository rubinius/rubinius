def Bench.run
  i = 0
  while @should_run
    # string#downcase_bang(...)
    raise "string#downcase_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

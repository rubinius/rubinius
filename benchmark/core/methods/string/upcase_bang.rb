def Bench.run
  i = 0
  while @should_run
    # string#upcase_bang(...)
    raise "string#upcase_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

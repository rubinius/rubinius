def Bench.run
  i = 0
  while @should_run
    # string#sub_bang(...)
    raise "string#sub_bang(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

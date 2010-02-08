def Bench.run
  i = 0
  while @should_run
    # string#modulo_op(...)
    raise "string#modulo_op(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

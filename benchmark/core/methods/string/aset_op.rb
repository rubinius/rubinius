def Bench.run
  i = 0
  while @should_run
    # string#aset_op(...)
    raise "string#aset_op(...) benchmark is not implemented"
    i += 1
  end

  @iterations = i
end

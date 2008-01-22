require 'benchmark'

total = ENV['TOTAL'] || 10_000

total = total.to_i

Benchmark.bmbm do |x|
  x.report("Class const_set level 1") do
    i = 0
    while i < total
      Object.const_set(:C, Class.new)
      C.to_s
      Object.send(:remove_const, :C)
      i += 1
    end
  end

  x.report("Class const_set level 2") do
    i = 0
    class C; end
    while i < total
      C.const_set(:C, Class.new)
      C::C.to_s
      C.send(:remove_const, :C)
      i += 1
    end
    Object.send(:remove_const, :C)
  end

  x.report("Class const_set level 3") do
    i = 0
    class C; end
    class C::C; end
    while i < total
      C::C.const_set(:C, Class.new)
      C::C::C.to_s
      C::C.send(:remove_const, :C)
      i += 1
    end
    Object.send(:remove_const, :C)
  end

  x.report("Module const_set level 1") do
    i = 0
    while i < total
      Object.const_set(:M, Module.new)
      M.to_s
      Object.send(:remove_const, :M)
      i += 1
    end
  end

  x.report("Module const_set level 2") do
    i = 0
    module M end
    while i < total
      M.const_set(:M, Module.new)
      M::M.to_s
      M.send(:remove_const, :M)
      i += 1
    end
    Object.send(:remove_const, :M)
  end

  x.report("Module const_set level 3") do
    i = 0
    module M end
    module M::M end
    while i < total
      M::M.const_set(:M, Module.new)
      M::M::M.to_s
      M::M.send(:remove_const, :M)
      i += 1
    end
    Object.send(:remove_const, :M)
  end

end

require 'benchmark'

total = (ENV['TOTAL'] || 2_000_000).to_i

puts "Reading..."
Benchmark.bm(10) do |x|
  x.report("loop") do
    total.times do |i|
      i
    end
  end

  x.report("1 local") do
    a = 1
    total.times do |i|
      z = a
    end
  end

  x.report("2 locals") do
    a = 1
    b = 2
    total.times do |i|
      z = a
      z = b
    end
  end

  x.report("3 locals") do
    a = 1
    b = 2
    c = 3
    total.times do |i|
      z = a
      z = b
      z = c
    end
  end

  x.report("4 locals") do
    a = 1
    b = 2
    c = 3
    d = 4
    total.times do |i|
      z = a
      z = b
      z = c
      z = d
    end
  end

  x.report("5 locals") do
    a = 1
    b = 2
    c = 3
    d = 4
    e = 5
    total.times do |i|
      z = a
      z = b
      z = c
      z = d
      z = e
    end
  end

  x.report("6 locals") do
    a = 1
    b = 2
    c = 3
    d = 4
    e = 5
    f = 6
    total.times do |i|
      z = a
      z = b
      z = c
      z = d
      z = e
      z = f
    end
  end

  x.report("1 ivar") do
    @a = 1
    total.times do |i|
      z = @a
    end
  end

  x.report("2 ivars") do
    @a = 1
    @b = 2
    total.times do |i|
      z = @a
      z = @b
    end
  end

  x.report("3 ivars") do
    @a = 1
    @b = 2
    @c = 3
    total.times do |i|
      z = @a
      z = @b
      z = @c
    end
  end

  x.report("4 ivars") do
    @a = 1
    @b = 2
    @c = 3
    @d = 4
    total.times do |i|
      z = @a
      z = @b
      z = @c
      z = @d
    end
  end

  x.report("5 ivars") do
    @a = 1
    @b = 2
    @c = 3
    @d = 4
    @e = 5
    total.times do |i|
      z = @a
      z = @b
      z = @c
      z = @d
      z = @e
    end
  end

  x.report("6 ivars") do
    @a = 1
    @b = 2
    @c = 3
    @d = 4
    @e = 5
    @f = 6
    total.times do |i|
      z = @a
      z = @b
      z = @c
      z = @d
      z = @e
      z = @f
    end
  end
end

puts "Writing..."
Benchmark.bm(10) do |x|
  x.report("loop") do
    total.times do |i|
      i
    end
  end

  x.report("1 local") do
    total.times do |i|
      a = i
    end
  end

  x.report("2 locals") do
    total.times do |i|
      a = i
      b = i
    end
  end

  x.report("3 locals") do
    total.times do |i|
      a = i
      b = i
      c = i
    end
  end

  x.report("4 locals") do
    total.times do |i|
      a = i
      b = i
      c = i
      d = i
    end
  end

  x.report("5 locals") do
    total.times do |i|
      a = i
      b = i
      c = i
      d = i
      e = i
    end
  end

  x.report("6 locals") do
    total.times do |i|
      a = i
      b = i
      c = i
      d = i
      e = i
      f = i
    end
  end

  x.report("1 ivar") do
    total.times do |i|
      @a = i
    end
  end

  x.report("2 ivars") do
    total.times do |i|
      @a = i
      @b = i
    end
  end

  x.report("3 ivars") do
    total.times do |i|
      @a = i
      @b = i
      @c = i
    end
  end

  x.report("4 ivars") do
    total.times do |i|
      @a = i
      @b = i
      @c = i
      @d = i
    end
  end

  x.report("5 ivars") do
    total.times do |i|
      @a = i
      @b = i
      @c = i
      @d = i
      @e = i
    end
  end

  x.report("6 ivars") do
    total.times do |i|
      @a = i
      @b = i
      @c = i
      @d = i
      @e = i
      @f = i
    end
  end
end

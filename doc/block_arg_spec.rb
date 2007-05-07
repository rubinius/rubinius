class Runner

def stub(count)
  case count
  when 0
    yield
  when 1
    yield 1
  when 2
    yield 1, 2
  when 3
    yield 1, 2, 3
  end
end

def r(bargs, count)
  puts "Case: block takes #{bargs}, called with #{count}"
  args = ('a' .. 'z').to_a[0, bargs]
  sargs = args.join(",")
  str =  "stub(#{count}) do |#{sargs}|\n"
  str << "[#{sargs}]\n"
  str << "end\n"

  p eval(str)
end

def test
  0.upto(3) do |i|
    0.upto(3) do |j|
      r i, j
    end
  end

  puts "Case: block takes one and splat, passed 0"
  o = stub(0) do |a,*b|
    p [a, b]
  end

  puts "Case: block takes one and splat, passed 1"
  o = stub(1) do |a,*b|
    p [a, b]
  end
  
  puts "Case: block takes one and splat, passed 2"
  o = stub(2) do |a,*b|
    p [a, b]
  end
  
  puts "Case: block takes one and splat, passed 3"
  o = stub(3) do |a,*b|
    p [a, b]
  end
  
  class << self
    def doit
      ary = [1,2,3]
      yield ary
    end
  end
  
  puts "Case: block takes 1, passed array of 3"
  o = doit do |a|
    p [a]
  end
  
  puts "Case: block takes 2, passed array of 3"
  o = doit do |a,b|
    p [a, b]
  end
  
  class << self
    def doit(&b)
      ary = [1,2,3]
      b.call(ary)
    end
  end
  
  puts "Case: block takes 1, passed array of 3 (&block)"
  o = doit do |a|
    p [a]
  end
  
  puts "Case: block takes 2, passed array of 3 (&block)"
  o = doit do |a,b|
    p [a, b]
  end

end

end

r = Runner.new
r.test


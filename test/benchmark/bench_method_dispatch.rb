require 'benchmark'

total = ENV['TOTAL'] || 1_000_000

total = total.to_i

puts "Control: 1m loops accessing a local variable 100 times"
5.times {
puts Benchmark.measure {
  a = 5; 
  i = 0;
  while i < total
    a; a; a; a; a; a; a; a; a; a;
    a; a; a; a; a; a; a; a; a; a;
    a; a; a; a; a; a; a; a; a; a;
    a; a; a; a; a; a; a; a; a; a;
    a; a; a; a; a; a; a; a; a; a;
    a; a; a; a; a; a; a; a; a; a;
    a; a; a; a; a; a; a; a; a; a;
    a; a; a; a; a; a; a; a; a; a;
    a; a; a; a; a; a; a; a; a; a;
    a; a; a; a; a; a; a; a; a; a;
    i += 1;
  end
}
}

puts "Test STI: 1m loops accessing a fixnum var and calling to_i 100 times"
5.times {
puts Benchmark.measure {
  a = 5; 
  i = 0;
  while i < total
    a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i;
    a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i;
    a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i;
    a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i;
    a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i;
    a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i;
    a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i;
    a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i;
    a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i;
    a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i; a.to_i;
    i += 1;
  end
}
}

def foo
  self
end

puts "Test ruby method: 1m loops calling self's foo 100 times"
5.times {
puts Benchmark.measure {
  a = []; 
  i = 0;
  while i < total
    foo; foo; foo; foo; foo; foo; foo; foo; foo; foo;
    foo; foo; foo; foo; foo; foo; foo; foo; foo; foo;
    foo; foo; foo; foo; foo; foo; foo; foo; foo; foo;
    foo; foo; foo; foo; foo; foo; foo; foo; foo; foo;
    foo; foo; foo; foo; foo; foo; foo; foo; foo; foo;
    foo; foo; foo; foo; foo; foo; foo; foo; foo; foo;
    foo; foo; foo; foo; foo; foo; foo; foo; foo; foo;
    foo; foo; foo; foo; foo; foo; foo; foo; foo; foo;
    foo; foo; foo; foo; foo; foo; foo; foo; foo; foo;
    foo; foo; foo; foo; foo; foo; foo; foo; foo; foo;
    i += 1;
  end
}
}

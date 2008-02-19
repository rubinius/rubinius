require 'benchmark'

total = ENV['TOTAL'] || 1_000_000

total = total.to_i

per = 2

puts "Control: 1m loops accessing a local variable 100 times"
per.times {
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

puts "Control 2: 1m loops accessing nil 100 times"
per.times {
puts Benchmark.measure {
  a = 5; 
  i = 0;
  while i < total
    nil; nil; nil; nil; nil; nil; nil; nil; nil; nil;
    nil; nil; nil; nil; nil; nil; nil; nil; nil; nil;
    nil; nil; nil; nil; nil; nil; nil; nil; nil; nil;
    nil; nil; nil; nil; nil; nil; nil; nil; nil; nil;
    nil; nil; nil; nil; nil; nil; nil; nil; nil; nil;
    nil; nil; nil; nil; nil; nil; nil; nil; nil; nil;
    nil; nil; nil; nil; nil; nil; nil; nil; nil; nil;
    nil; nil; nil; nil; nil; nil; nil; nil; nil; nil;
    nil; nil; nil; nil; nil; nil; nil; nil; nil; nil;
    nil; nil; nil; nil; nil; nil; nil; nil; nil; nil;
    i += 1;
  end
}
}

puts "Test fastmath: 1m loops accessing a fixnum var and calling + 1"
per.times {
puts Benchmark.measure {
  a = 5; 
  i = 0;
  while i < total
    a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1;
    a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1;
    a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1;
    a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1;
    a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1;
    a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1;
    a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1;
    a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1;
    a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1;
    a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1; a + 1;
    i += 1;
  end
}
}

class Fixnum
  alias :plus :+
end

puts "Test prim math: 1m loops accessing a fixnum var and calling plus(1)"
per.times {
puts Benchmark.measure {
  a = 5; 
  i = 0;
  while i < total
    a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1);
    a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1);
    a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1);
    a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1);
    a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1);
    a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1);
    a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1);
    a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1);
    a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1);
    a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1); a.plus(1);
    i += 1;
  end
}
}

cm = def foo
  self
end

puts "Test ruby method (prim): 1m loops calling self's foo 100 times"
per.times {
puts Benchmark.measure {
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

cm.primitive = nil

puts "Test ruby method: 1m loops calling self's foo 100 times"
per.times {
puts Benchmark.measure {
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


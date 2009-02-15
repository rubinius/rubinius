# test context switching between two threads with very different stack sizes
# from Brent Roman MBARI patches  bogus1.rb file
# http://sites.google.com/site/brentsrubypatches/

def outer name
  if ($depth+=1) < 2000
    outer name
  else
    250.times {
      Thread.new do
        inner :inner, name, Thread.current
      end.join
    }
  end
end

def inner innerName, outerName, parent
  Thread.new do
    parent.join
    k = Proc.new {|n0, n1| q = n0.to_s << n1.to_s }
    k[innerName, outerName]
  end
end

$depth = 0
Bench.run [1] do |n|
  outer :outer
end

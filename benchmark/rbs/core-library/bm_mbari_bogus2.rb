# test context switching between two threads with very different stack sizes
# from the bogus2.rb file
# of the Brent Roman MBARI patches
# http://sites.google.com/site/brentsrubypatches/

def outer name
  ball = Thread.current
  inner = Thread.new Thread.current do | outer |
    loop {
      Thread.stop until (Thread.critical=true; ball == inner)
      (ball=outer).run
    }
  end
  2000.times {|i|
    Thread.stop until (Thread.critical=true; ball == Thread.current)
    (ball=inner).run
  }
end

def recurse someArg
  if ($depth+=1) < 2000
    recurse 3.14159
  else
    outer :outer
  end
end

$depth = 0
Bench.run [1] do |n|
  recurse "top"
end

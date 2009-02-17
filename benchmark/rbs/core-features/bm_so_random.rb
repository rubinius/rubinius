# from http://www.bagley.org/~doug/shootout/bench/random/random.ruby

def gen_random(max, last, im, ia, ic)
  (max * (last = (last * ia + ic) % im)) / im
end

Bench.run [100_000, 500_000, 1_000_000] do |n|
  n.times{ gen_random(100.0, 42.0, 139968.0, 3877.0, 29573.0) }
end

i=0

while i < 5_000 # benchmark loop 3
  i += 1
  Thread.new(i) do |x|
    Array.new(x) { |y| y * y }
  end.join
end

# This test modified from its original form in the YARV benchmarks, since
# that original test was not useful as a cross implementation benchmark. It
# was superficial in that the threads did no work themselves.
# At the time of this modification, the original test was approximately 130x
# slower in Rubinius, but making the test run 5,000 iterations (to reduce
# startup time differences) and actually do some work cut the speed
# differential down to 3.8x (ish) slower

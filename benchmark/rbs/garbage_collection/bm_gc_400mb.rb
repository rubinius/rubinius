Bench.run [1] do |n|
  a = []
  3000000.times { a << []} # use up some RAM
  100000.times {[]}
end

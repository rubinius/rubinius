Bench.run [1] do |n|
  a = []
  500000.times { a << []} # use up some RAM
  3000000.times {[]}
end

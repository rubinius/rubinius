Bench.run [1] do |n|
  a = []
  1000000.times { a << []} # use up some RAM
  3000000.times {[]}
end

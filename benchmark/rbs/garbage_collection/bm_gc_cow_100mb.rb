GC.copy_on_write_friendly=true if GC.respond_to? :copy_on_write_friendly=

Bench.run [1] do |n|
  a = []
  1000000.times { a << []} # use up some RAM
  3000000.times {[]}
end

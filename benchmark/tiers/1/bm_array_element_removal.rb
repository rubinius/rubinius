require 'benchmark'
require 'benchmark/ips'

# had to size this so that array allocation didn't swamp
# the other numbers; it needs to be small enough to keep
# allocation fast yet large enough that the tests don't
# need to allocate too many of them
BaseArray = (0...10_000).to_a


Benchmark.ips do |x|
  
  x.report "array delete_at" do |times|
    scratch_array = BaseArray.dup
    i = 0
    while i < times
      scratch_array.delete_at(0)
      scratch_array = BaseArray.dup if scratch_array.empty?
      i += 1
    end
  end
  
  x.report "array slice!" do |times|
    scratch_array = BaseArray.dup
    i = 0
    while i < times
      scratch_array.slice!(0)
      scratch_array = BaseArray.dup if scratch_array.empty?
      i += 1
    end
  end

  x.report "array drop" do |times|
    scratch_array = BaseArray.dup
    i = 0
    while i < times
      scratch_array.drop(1)
      scratch_array = BaseArray.dup if scratch_array.empty?
      i += 1
    end
  end
  
  x.report "array shift" do |times|
    scratch_array = BaseArray.dup
    i = 0
    while i < times
      scratch_array.shift
      scratch_array = BaseArray.dup if scratch_array.empty?
      i += 1
    end    
  end

end
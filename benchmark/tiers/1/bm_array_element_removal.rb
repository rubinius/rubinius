require 'benchmark'
require 'benchmark/ips'


Benchmark.ips do |x|
  
  base_array = (0...5_000).to_a
  
  x.report "array delete_at" do
    scratch_array = base_array.dup
    scratch_array.delete_at(0) until scratch_array.empty?
  end
  
  x.report "array slice!" do
    scratch_array = base_array.dup
    scratch_array.slice!(0) until scratch_array.empty?
  end

  x.report "array drop" do
    scratch_array = base_array.dup
    scratch_array = scratch_array.drop(1) until scratch_array.empty?
  end
  
  x.report "array shift" do
    scratch_array = base_array.dup
    scratch_array.shift until scratch_array.empty?
  end

end
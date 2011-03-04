require 'benchmark'
require 'benchmark/ips'

# these are all different ways of removing the first element from
# an array until it is empty

Benchmark.ips do |x|

  x.compare!

  base_array = (0...5_000).to_a

  x.report "array delete_at, front to back" do
    scratch_array = base_array.dup
    scratch_array.delete_at(0) until scratch_array.empty?
  end

  x.report "array slice!, front to back" do
    scratch_array = base_array.dup
    scratch_array.slice!(0) until scratch_array.empty?
  end

  x.report "array slice, front to back" do
    scratch_array = base_array.dup
    scratch_array = scratch_array.slice(1, scratch_array.size - 1) until scratch_array.empty?
  end

  x.report "array []=, front to back" do
    scratch_array = base_array.dup
    scratch_array[0, 1] = nil until scratch_array.empty?
  end

  x.report "array drop, front to back" do
    scratch_array = base_array.dup
    scratch_array = scratch_array.drop(1) until scratch_array.empty?
  end

  x.report "array shift, front to back" do
    scratch_array = base_array.dup
    scratch_array.shift until scratch_array.empty?
  end

  x.report "array drop_while, front to back" do
    scratch_array = base_array.dup
    scratch_array.drop_while { true }
  end

  x.report "array delete_if, front to back" do
    scratch_array = base_array.dup
    scratch_array.delete_if { true }
  end

  x.report "array delete, front to back" do
    scratch_array = base_array.dup
    scratch_array.size.times { |index| scratch_array.delete(index) }
  end

  x.report "array pop, back to front" do
    scratch_array = base_array.dup
    scratch_array.pop until scratch_array.empty?
  end

  x.report "array delete_at, back to front" do
    scratch_array = base_array.dup
    i = scratch_array.size - 1
    while i > 0
      scratch_array.delete_at(i)
      i -= 1
    end
  end

  x.report "array slice!, back to front" do
    scratch_array = base_array.dup
    i = scratch_array.size - 1
    while i > 0
      scratch_array.slice!(i)
      i -= 1
    end
  end

  x.report "array slice, back to front" do
    scratch_array = base_array.dup
    i = scratch_array.size - 1
    while i > 0
      scratch_array = scratch_array.slice(0, i)
      i -= 1
    end
  end

  x.report "array delete, back to front" do
    scratch_array = base_array.dup
    i = scratch_array.size - 1
    while i > 0
      scratch_array.delete(i)
      i -= 1
    end
  end

  x.report "array take, back to front" do
    scratch_array = base_array.dup
    i = scratch_array.size - 1
    while i > 0
      scratch_array = scratch_array.take(i - 1)
      i -= 1
    end
  end

end

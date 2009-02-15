# Submitted by Emil Ivanov

class Array
  def merge_sort
    len = self.length
    return self if len <= 1
    middle = len / 2
    left = self.slice(0, middle).merge_sort
    right = self.slice(middle, len - middle).merge_sort
    merge(left, right)
  end

  protected

  def merge(left, right)
    result = []

    while (left.length > 0 && right.length > 0)
      if (left.first < right.first)
        result.push(left.shift)
      else
        result.push(right.shift)
      end
    end

    if left.length > 0
      result += left
    end

    if right.length > 0
      result += right
    end

    result
  end
end

Bench.run [1] do |n|
  fname = File.dirname(__FILE__) + "/random.input"
  array = File.read(fname).split(/\n/).map!{|m| m.to_i }
  puts "Mergesort verified." if array.merge_sort == array.sort
end

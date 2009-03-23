#from http://www.bagley.org/~doug/shootout/bench/lists/lists.ruby

def test_lists(size)
  # create a list of integers (Li1) from 1 to SIZE
  li1 = (1..size).to_a
  # copy the list to li2 (not by individual items)
  li2 = li1.dup
  # remove each individual item from left side of li2 and
  # append to right side of li3 (preserving order)
  li3 = Array.new
  while (not li2.empty?)
    li3.push(li2.shift)
  end
  # li2 must now be empty
  # remove each individual item from right side of li3 and
  # append to right side of li2 (reversing list)
  while (not li3.empty?)
    li2.push(li3.pop)
  end
  # li3 must now be empty
  # reverse li1 in place
  li1.reverse!
  # check that first item is now SIZE
  if li1[0] != size then
    p "not size"
    0
  else
    # compare li1 and li2 for equality
    if li1 != li2 then
      return(0)
    else
      # return the length of the list
      li1.length
    end
  end
end

Bench.run [1000] do |n|
  n.times { test_lists(10_000) }
end

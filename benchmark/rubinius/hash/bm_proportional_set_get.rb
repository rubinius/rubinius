require File.dirname(__FILE__) + '/setup'

# The proportion of #[]= : #[] is 5:3 based on Hash instances
# above 95% from running Hash spy while running 'rails -h'.

sizes_and_iterations.each do |max, iterations|
  Bench.run iterations do |n|
    n.times do
      h = new_hash
      (0...max).each do
        h[get_key(max)] = 0
        h[get_key(max)] = 0
        a = h[get_key(max)]
        h[get_key(max)] = 0
        a = h[get_key(max)]
        h[get_key(max)] = 0
        a = h[get_key(max)]
        h[get_key(max)] = 0
      end
    end
  end
end

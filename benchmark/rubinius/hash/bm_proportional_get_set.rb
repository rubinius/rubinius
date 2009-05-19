require File.dirname(__FILE__) + '/setup'

# The proportion of #[]= : #[] is 1:5 based on Hash instances
# at the 98% from running Hash spy while rdoc'ing Rdoc. Below
# 75%, the ratio was close to 1:1, which is covered by the
# balanced get/set benchmark. See bm_proportional_set_get.rb

sizes_and_iterations.each do |max, iterations|
  Bench.run iterations do |n|
    n.times do
      h = new_hash
      (0...max).each do
        a = h[get_key(max)]
        a = h[get_key(max)]
        h[get_key(max)] = 0
        a = h[get_key(max)]
        a = h[get_key(max)]
        a = h[get_key(max)]
      end
    end
  end
end

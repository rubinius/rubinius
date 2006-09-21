hsh = Hash.new do |h,k|
    puts "in block! #{h.inspect}, #{k.inspect}"
    h[k] = 3
end

hsh[:blah]

p hsh[:blah]

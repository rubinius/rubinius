cm = def blah(a,b,*c,&d)
  1 + 1
end

tup = cm.local_names
puts tup[0].to_s
puts tup[1].to_s
puts tup[2].to_s
puts tup[3].to_s

# OUTPUT
# a
# b
# c
# d

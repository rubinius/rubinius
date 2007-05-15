module Printf
  # printf() takes a variable number of args. Because of this
  # currently, we have to attach printf as the same function, but
  # with different signatures.
  
  # This is for one that takes a string an a number only.
  attach_function nil, "printf", :show_number, [:string, :int], :int
  
  # This one takes 2 strings.
  attach_function nil, "printf", :show_string, [:string, :string], :int
end

Printf.show_number "%d is a great number\n", 47
Printf.show_string "Hey %s, get over here!\n", "evan"

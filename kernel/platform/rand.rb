module Random
  attach_function nil, 'srand', [:uint], :void
  attach_function nil, 'rand', [], :int
end

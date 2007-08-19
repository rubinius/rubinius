module Platform::POSIX
  attach_function nil, 'srand', [:uint], :void
  attach_function nil, 'rand', [], :int
  attach_function nil, "getcwd", [:string, :int], :string
end
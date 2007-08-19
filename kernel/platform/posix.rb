module Platform::POSIX
  attach_function nil, 'srand', [:uint], :void
  attach_function nil, 'rand', [], :int
  attach_function nil, 'access', [:string, :int], :int
  attach_function nil, 'chmod', [:string, :int], :int
  attach_function nil, 'fchmod', [:int, :int], :int
  attach_function nil, 'unlink', [:string], :int
  attach_function nil, 'getcwd', [:string, :int], :string
end
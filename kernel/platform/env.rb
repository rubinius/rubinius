# depends on: library.rb

class EnvironmentVariables
  extend FFI::Library

  attach_function :getenv,   [:string], :string
  attach_function :putenv,   [:string], :int
  attach_function :setenv,   [:string,  :string, :int], :int
  attach_function :unsetenv, [:string], :int
  attach_function :environ,  'ffi_environ', [], :pointer
end

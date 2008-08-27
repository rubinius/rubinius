# depends on: ffi.rb

class EnvironmentVariables
  include FFI

  attach_function 'getenv', :getenv, [:string], :string
  attach_function 'putenv', :putenv, [:string], :int
  attach_function 'setenv', :setenv, [:string, :string, :int], :int
  attach_function 'unsetenv', :unsetenv, [:string], :int

  attach_function 'ffi_environ', :environ, [], :pointer
end

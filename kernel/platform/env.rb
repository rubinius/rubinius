module Rubinius
  module EnvironmentAccess
    extend FFI::Library

    attach_function :getenv,   [:string], :string
    attach_function :putenv,   [:string], :int
    attach_function :setenv,   [:string,  :string, :int], :int
    attach_function :unsetenv, [:string], :int
    attach_function :environ,  'ffi_environ', [], :pointer
    
    def environ_as_hash
      env = environ()
      ptr_size = FFI.type_size FFI.find_type(:pointer)

      i = 0

      hash = {}

      offset = 0
      cur = env + offset

      until cur.read_pointer.null?
        entry = cur.read_pointer.read_string
        key, value = entry.split '=', 2
        value.taint if value
        key.taint if key

        hash[key] = value

        offset += ptr_size
        cur = env + offset
      end
      
      hash
    end
    module_function :environ_as_hash
  end
end

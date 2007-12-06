# depends on: ffi.rb

module Rubinius
  attach_function 'class_constitute', [:state, :object, :object], :object
  attach_function 'module_setup_fields', [:state, :object], :void
end

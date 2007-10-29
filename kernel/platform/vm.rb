# depends on: ffi.rb

module Rubinius
  attach_function nil, 'class_constitute', [:state, :object, :object], :object
  attach_function nil, 'module_setup_fields', [:state, :object], :void
end

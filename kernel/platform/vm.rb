# depends on: ffi.rb

module Rubinius
  attach_function nil, 'class_constitute', [:state, :object, :object], :object
end

# depends on: ffi.rb

module Compression
  class ZLib
    attach_function "compress_inflate", :inflate, [:state, :string], :object
    attach_function "compress_deflate", :deflate, [:state, :string], :object
  end
end

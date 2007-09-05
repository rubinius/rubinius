# See rake build:platform
# depends on: ffi.rb

module Compression
  class ZLib
    attach_function nil, "compress_inflate", :inflate, [:state, :string], :object
    attach_function nil, "compress_deflate", :deflate, [:state, :string], :object
  end
end

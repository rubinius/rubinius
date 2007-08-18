module Compression
  class ZLib
    module Foreign
      attach_function nil, "compress_inflate", :inflate, [:state, :string], :object
      attach_function nil, "compress_deflate", :deflate, [:state, :string], :object
    end
    
    def self.inflate(input)
      Foreign.inflate input
    end

    def self.deflate(input)
      Foreign.deflate input
    end
  end
end

module Compression
  class ZLib
    module Foreign
      attach_function nil, "compress_inflate", :inflate, [:state, :string], :object
      attach_function nil, "compress_deflate", :deflate, [:state, :string], :object
    end
    
    def self.inflate(input)
      Foreign.inflate nil, input
    end

    def self.deflate(input)
      Foreign.deflate nil, input
    end
  end
end

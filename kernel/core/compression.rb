module Compression
  class ZLib
    def self.inflate(input)
      Ruby.primitive :zlib_inflate
    end

    def self.deflate(input)
      Ruby.primitive :zlib_deflate
    end
  end
end

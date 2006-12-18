module Compression
  class LibZ
    def self.inflate(input)
      Ruby.primitive :libz_inflate
    end

    def self.deflate(input)
      Ruby.primitive :libz_deflate
    end
  end
end

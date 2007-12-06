module Zlib
  class Inflate
    def self.inflate(data)
      Compression::ZLib.new.inflate data
    end
  end

  class Deflate
    def self.deflate(data)
      Compression::ZLib.new.deflate data
    end
  end
end

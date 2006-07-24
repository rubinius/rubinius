module Rubinius
  module ByteArray
    Super = nil
    Fields = []
    
    def self.new(size)
      words = size / 4
      
      if size % 4 != 0
        words += 1
      end
      
      obj = allocate(words)
      obj.as :bytearray
      obj.make_byte_storage
      return obj
    end
  end
  
  add_type :bytearray, ByteArray
end
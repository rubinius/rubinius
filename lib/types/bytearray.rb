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
      obj.initialize_bytes
      return obj
    end
    
    def self.from_string(str)
      obj = new(str.size)
      0.upto(str.size - 1) do |i|
        obj.store_byte i, str[i]
      end
      return obj
    end
    
    def bytes
      self.fields * 4
    end
    
    def clear
      
    end
    
    def as_string
      i = fields * 4
      
      str = " " * i
      0.upto(i - 1) do |idx|
        str[idx] = fetch_byte(idx)
      end
      
      return str
    end
  end
  
  add_type :bytearray, ByteArray
end
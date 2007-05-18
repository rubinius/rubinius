module Rubinius
  module Bignum
    Super = nil
    Fields = []
    
    def self.new(str)
      size = str.size + 1
      words = size / 4
      
      if size % 4 != 0
        words += 1
      end
      
      obj = allocate(words)
      obj.as :bignum
      obj.make_byte_storage
      obj.initialize_bytes
      i = 0
      0.upto(str.size - 1) do |i|
        obj.store_byte i, str[i]
      end
      
      obj.store_byte(i+1, 0)
      
      return obj
    end
    
    def as_string
      i = fields * 4
      
      str = ""
      0.upto(i - 1) do |idx|
        byte = fetch_byte(idx)
        break if byte == 0
        str << byte
      end
      
      return str
    end            
  end
  
  add_type :bignum, Bignum
end
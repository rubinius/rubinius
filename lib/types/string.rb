module Rubinius
  module String
    Super = nil
    Fields = [:bytes, :characters, :encoding, :data]
    
    def self.new(str)
      obj = allocate()
      obj.bytes = RObject.wrap(str.size)
      obj.characters = RObject.wrap(str.size)
      obj.encoding = RObject.nil
      data = Rubinius::ByteArray.new(str.size)
      i = 0
      str.each_byte do |byte|
        data.store_byte i, byte
        i += 1
      end
      obj.data = data
      return obj
    end
    
    def as_string
      i = self.bytes.to_int
      data = self.data
      
      str = " " * i
      0.upto(i - 1) do |idx|
        str[idx] = data.fetch_byte(idx)
      end
      
      return str
    end
    
    HashPrime = 16777619
    
    # Implements hashing using the FNV algorithm.
    def hash_int
      str = as_string
      prime = HashPrime
      
      hv = 0x811c9dc5
      str.size.times do |i|
          hv = (hv * prime)
          hv = 0xffffffff & (hv ^str[i])
      end

      mask = 0x0fffffff
      hv = (hv >> 28) ^ (hv & mask)
      
      return hv
    end
    
    def hash_int_old
      str = as_string
      len = str.size
      key = 0
      0.upto(len - 1) do |idx|
        key = (key & 0xffffffff) * 65599 + str[idx]
      end
      
      key = key + (key >> 5)
      
      (key & 0x0fffffff)
    end
    
    def to_sym
      CPU::Global.symbols.lookup(self)
    end
  end
  
  add_type :string, String
end
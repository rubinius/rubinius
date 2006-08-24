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
    
    def self.new_empty(size)
      obj = allocate()
      obj.bytes = RObject.wrap(size)
      obj.characters = RObject.wrap(size)
      obj.encoding = RObject.nil
      obj.data = Rubinius::ByteArray.new(size)
      
      return obj
    end
    
    def bytes_address
      bytes = self.data
      return bytes.byte_start
    end
    
    def append(other)
      # puts other.rclass.name.as(:symbol).as_string + " #{other.address}"
      # puts self.rclass.name.as(:symbol).as_string + " #{self.address}"
      
      cur = self.data.as(:bytearray)
      other.as :string
      obs = other.data.as(:bytearray)
      cur_sz = self.bytes.to_int
      oth_sz = other.bytes.to_int
      ns = cur_sz + oth_sz
      if ns > cur.bytes
        nd = Rubinius::ByteArray.new(ns)
        cur.copy_bytes_into nd, cur_sz, 0
        obs.copy_bytes_into nd, oth_sz, cur_sz
        self.data = nd
      else
        obs.copy_bytes_into cur, oth_sz, cur_sz
      end
      self.bytes = RObject.wrap(ns)
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
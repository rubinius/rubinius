module Rubinius
  module Hash
    Fields = [:keys, :values, :bins, :entries]
    
    Increments = 16
    
    def self.new
      obj = allocate
      obj.keys = Rubinius::Tuple.allocate(Increments)
      obj.values = Rubinius::Tuple.allocate(Increments)
      obj.bins = RObject.wrap(Increments)
      obj.entries = RObject.wrap(0)
      return obj
    end
    
    def self.new_from_tuple(tup)
      hsh = new
      i = 0
      while i < tup.fields
        key = tup.at(i)
        val = tup.at(i+1)
        i += 2
        hsh.set key, val
      end
      
      return hsh
    end
    
    def entry_new(hash, key, data)
      tup = Rubinius::Tuple.allocate(4)
      tup.put 0, RObject.wrap(hash)
      tup.put 1, key
      tup.put 2, data
      tup.put 3, RObject.nil
      
      return tup
    end
    
    def entry_append(top, nxt)
      cur = top.at 3
      last = top
      until cur.nil?
        last = cur
        cur = cur.at 3
      end
      
      last.put 3, nxt
      return nxt
    end
    
    def add(hash, key, data)
      entry = find_entry(hash)
      
      unless entry.nil?
        entry.put 2, data
        return data
      end
      
      ent = entry_new(hash, key, data)
      add_entry hash, ent
      return data
    end
    
    def set(key, val)
      add key.hash_int, key, val
    end
    
    def add_entry(hash, ent)
      bin = hash % self.bins.to_int
      entry = self.values.at bin
      
      if entry.nil?
        self.values.put bin, ent
      else
        entry_append entry, ent
      end
      
      self.entries = RObject.wrap(self.entries.to_int + 1)
      return ent
    end
    
    def get(hash)
      entry = find_entry(hash)
      if !entry.nil?
        return entry.at(2)
      end
      
      return RObject.nil
    end
    
    def find(key)
      get key.hash_int
    end
    
    def find_entry(hash)
      bin = hash % self.bins.to_int
      entry = self.values.at(bin)
      
      until entry.nil?
        th = entry.at(0)
        if th.to_int == hash
          return entry
        end
        
        entry = entry.at(3)
      end
      
      return RObject.nil
    end
    
    def delete(hash)
      bin = hash % self.bins.to_int
      entry = self.values.at(bin)
      
      until entry.nil?
        th = entry.at(0)
        lk = entry.at(3)
        
        if th.to_int == hash
          val = entry.at(2)
          values.put bin, lk
          self.entries = RObject.wrap(self.entries.to_int - 1)
          return val
        end
        
        entry = lk
      end
      
      return RObject.nil
    end
    
    def as_array
      current = self.values
      out = []
      0.upto(current.fields - 1) do |idx|
        entry = current.at(idx)
        until entry.nil?
          out << [entry.at(1), entry.at(2)]
          entry = entry.at(3)
        end
      end
      
      return out
    end
    
    def as_string
      str = as_array.map! { |k,v| 
        "#{k.as_string} => #{v.as_string}"
      }.join(", ")
      
      return "{#{str}}"
    end
  end
  
  add_type :hash, Hash
end
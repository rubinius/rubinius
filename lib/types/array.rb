module Rubinius
  module Array
    Super = nil
    Fields = [:total, :tuple]
    
    def self.new(count)
      obj = allocate
      tp = Rubinius::Tuple.new(count)
      obj.total = RObject.wrap(count)
      obj.tuple = tp
      return obj
    end
    
    def set(idx, val)
      self.tuple.put idx, val
    end
    
    def get(idx)
      self.tuple.at idx
    end
  end
  
  add_type :array, Array
end
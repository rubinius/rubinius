module Rubinius
  module Object
    Fields = [:instance_variables]
    
    def self.new
      obj = allocate
      obj.instance_variables = Rubinius::Hash.new
      return obj
    end
    
    def get_ivar(sym)
      tbl = self.instance_variables
      tbl.as :hash
      tbl.get sym.hash_int
    end
    
    def set_ivar(sym, val)
      tbl = self.instance_variables
      tbl.as :hash
      tbl.add sym.hash_int, sym, val
      return val
    end
  end
  
  add_type :object, Object
end
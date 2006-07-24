module Rubinius
  module Module
    Fields = [:methods, :name, :constants]
    
    def setup_constants
      self.constants = Rubinius::Hash.new
    end
    
    def setup_name(str)
      sym = Rubinius::String.new(str).to_sym
      self.name = sym
      
      CPU::Global.object.const_set sym, self
    end
    
    def setup(str)
      setup_constants
      setup_name(str)
    end
    
    def const_set(sym, obj)
      cnt = self.constants
      cnt.as :hash
      cnt.add sym.hash_int, sym, obj
    end
  end
  
  add_type :module, Module
end
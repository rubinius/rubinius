module Rubinius
  module Symbol
    Super = nil
    Fields = []
    
    def string
      CPU::Global.symbols.find_string(self)
    end
    
    def as_string
      str = string()
      if str.nil?
        puts "WARNING: Symbol #{self.inspect} could not be looked up!"
        ":(NULL)"
      else
        ":#{str.as_string}"
      end
    end
  end
  
  add_type :symbol, Symbol
  
  module SymbolTable
    Fields = [:symbols, :strings]
    
    StartSize = 128
    Increment = 32
    
    def self.new
      obj = allocate()
      obj.symbols = Rubinius::Tuple.new(StartSize)
      obj.strings = Rubinius::Hash.new
      return obj
    end
    
    def lookup(string)
      hash = string.hash_int
      strs = self.strings
      strs.as :hash
      
      syms = self.symbols
      syms.as :tuple
      
      idx = strs.get(hash)
      if idx.nil?
        idx = strs.entries
        syms.put idx.to_int, string
        strs.add hash, RObject.wrap(hash), idx
      end
      
      obj = RObject.symbol(idx.to_int)
      obj.as :symbol
      return obj
    end
    
    def find_string(sym)
      idx = sym.symbol_index
      str = self.symbols.at(idx)
      str.as :string
      return str
    end
  end
  
  add_type :symtbl, SymbolTable
end
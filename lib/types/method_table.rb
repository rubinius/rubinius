module Rubinius
  module MethodTable
    Super = :Hash
    Fields = []
    
    def self.new
      obj = allocate
      Rubinius::Hash.setup(obj)
    end
  end
  
  add_type :methtbl, MethodTable
end
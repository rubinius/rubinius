module Rubinius
  module Tuple
    Super = nil
    Fields = []
    
    def self.new_anonymous(size)
      obj = new_blank(size)
      obj.as :tuple
      return obj
    end
    
    def self.new(size)
      allocate size
    end
  end
  
  add_type :tuple, Tuple
end
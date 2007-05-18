module Rubinius
  module IO
    Super = nil
    Fields = [:descriptor]
      
    def self.new_from_object(io)
      int = RObject.wrap(io.fileno)
      obj = allocate()
      obj.descriptor = int
      return obj
    end
  
    def self.new(int)
      obj = allocate()
      obj.descriptor = RObject.wrap(int)
      return obj
    end
  end
  
  add_type :io, IO
end
  
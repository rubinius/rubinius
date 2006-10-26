module Rubinius
  module Class
    Super = :Module
    Fields = [:superclass, :instance_fields, :instance_flags]
    
    def self.create
      obj = Rubinius.cpu.new_object CPU::Global.class, NumberOfFields
      obj.as :class
      return obj
    end
    
    def self.create_normal(str, sup, fel)
      obj = create()
      obj.instance_fields = RObject.wrap(fel)
      obj.superclass = sup
      obj.setup str
      return obj
    end
    
    def new_instance
      count = self.instance_fields.to_int
      obj = Rubinius.cpu.new_object self, count
      return obj
    end
        
    def fields_as_hash
      i = 0
      TotalFields.map do |fel|
        obj = at(i)
        obj = obj.to_int if obj.fixnum?
        obj = nil if obj.nil?
        r = [fel, obj]
        i += 1
        r
      end
    end
  end
  
  add_type :class, Class
end
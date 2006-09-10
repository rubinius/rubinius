module Rubinius
  module MetaClass
    Super = :Class
    Fields = [:attached_instance]
  
    IsMetaFlag = 0x80
  
    def self.attach(obj)
      meta = allocate
      meta.flag_set IsMetaFlag
      meta.attached_instance = obj
      obj.rclass = meta
      return meta
    end
  
    def self.metaclass?(obj)
      obj.flag_set? IsMetaFlag
    end
  
  end

  add_type :metaclass, MetaClass
end
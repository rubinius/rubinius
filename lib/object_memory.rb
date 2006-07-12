class ObjectMemory
  DefaultSize = 128_000 # 128k
  
  def initialize
    @baker = BakerGC.new(DefaultSize)
  end
  
  def new_object(klass, fields)
    RObject.setup @baker, klass, fields
  end
  
  def bootstrap
    cls = new_object nil, 
  end
end
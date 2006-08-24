require 'gc/baker_gc'

class ObjectMemory
  DefaultSize = 256_000 # 128k
  
  def initialize
    @baker = BakerGC.new(DefaultSize)
  end
  
  def delete
    @baker.destroy!
  end
  
  def new_object(klass, fields)
    RObject.setup @baker, klass, fields
  end
end

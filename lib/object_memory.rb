require 'baker_gc'

class ObjectMemory
  DefaultSize = 128_000 # 128k
  
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
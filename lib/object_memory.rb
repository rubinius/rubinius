require 'gc/baker_gc'

class ObjectMemory
  DefaultSize = 384_000 * 2 # 256k
  
  def initialize
    @baker = BakerGC.new(DefaultSize)
  end
  
  def delete
    @baker.destroy!
  end
  
  def used
    @baker.used
  end
  
  def collect(roots)
    @baker.collect(roots)
  end
  
  def new_object(klass, fields)
    RObject.setup @baker, klass, fields
  end
  
  def find_instance(klass, start=0)
    
  end
end

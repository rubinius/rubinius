class GC
  def self.start
    run(false)
  end
    
  def self.run(tenure)
    Ruby.primitive :gc_start
    raise PrimitiveFailure, "primitive failed"
  end
  
  def self.collect_references(obj)
    Ruby.primitive :gc_collect_references
    raise PrimitiveFailure, "primitive failed"
  end
end

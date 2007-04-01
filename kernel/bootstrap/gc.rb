class GC
  
  def self.start
    run(false)
  end
  
  def self.promote_all
    run(true)
  end
  
  def self.run(tenure)
    Ruby.primitive :gc_start
  end
  
  def self.collect_references(obj)
    Ruby.primitive :gc_collect_references
  end
end

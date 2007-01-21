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
end

module VM
  def self.stats
    Ruby.primitive :vm_stats
  end
  
  def self.load_library(path, name)
    Ruby.primitive :load_library
  end
end
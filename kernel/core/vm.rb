module VM
  def self.stats
    Ruby.primitive :vm_stats
  end
end
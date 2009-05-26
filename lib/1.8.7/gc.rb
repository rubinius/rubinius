class GC
  def self.stress
    false
  end

  def self.stress=(flag)
    raise NotImplementedError
  end
end

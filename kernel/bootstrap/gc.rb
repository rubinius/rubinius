class GC
  def self.start
    run(false)
  end

  def self.run(tenure)
    Ruby.primitive :vm_gc_start
    raise PrimitiveFailure, "GC.run primitive failed"
  end

  def self.stress
    false
  end

  def self.stress=(flag)
    raise NotImplementedError
  end
end

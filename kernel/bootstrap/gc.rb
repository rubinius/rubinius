class GC
  def self.start
    run(false)
  end

  def self.run(tenure)
    Ruby.primitive :vm_gc_start
    raise PrimitiveFailure, "GC.run primitive failed"
  end
end

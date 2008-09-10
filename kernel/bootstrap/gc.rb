class GC
  def self.start
    run(false)
  end

  def self.run(tenure)
    Ruby.primitive :gc_start
    raise PrimitiveFailure, "primitive failed"
  end
end

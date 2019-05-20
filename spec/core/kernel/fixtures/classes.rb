module KernelSpecs
  class A; end

  class B
    def to_str
      1
    end
  end

  class C
    def to_str
      "zam"
    end
  end

  def self.empty_binding
    binding
  end

  def self.objectifier
    yield Object.new
  end
end

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

  class D
    def a
      cm = def b; :added; end
      __add_method__ :c, cm
    end
  end
end

class IncludesMath
  include Math
end

module MathSpecs
  class Float
    def to_f
      1.0
    end
  end

  class Integer
    def to_int
      2
    end
  end
end

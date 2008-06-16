module VariablesSpecs
  class ParAsgn
    attr_accessor :x

    def initialize
      @x = 0
    end

    def inc
      @x += 1
    end

    def to_ary
      [1,2,3,4]
    end
  end

  class OpAsgn
    attr_accessor :a, :side_effect
    
    def do_side_effect
      self.side_effect = true
      return @a
    end
  end

  class Hashalike
    def [](k) k end
    def []=(k, v) [k, v] end
  end

  def self.reverse_foo(a, b)
    return b, a
  end
end

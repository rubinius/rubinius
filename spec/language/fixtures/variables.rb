module VariablesSpecs
  class ParAsgn
    attr_accessor :x
  end

  class OpAsgn
    attr_accessor :a, :side_effect
    
    def do_side_effect
      self.side_effect = true
      return @a
    end
  end
  
  def self.reverse_foo(a, b)
    return b, a
  end
end

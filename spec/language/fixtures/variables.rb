module VariablesSpecs
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

module ComparableSpecs 
  class Weird
    include Comparable
  
    def initialize(int)
      @int = int
    end
  
    def negative?
      @int < 0
    end
  
    def <=>(other)
      return 0 if self.negative? == other.negative?
      return 1 if self.negative?
      -1
    end
  end
end

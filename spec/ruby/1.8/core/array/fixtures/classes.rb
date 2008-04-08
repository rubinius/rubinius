module ArraySpecs
  def self.frozen_array
    @frozen_array ||= [1,2,3]
    @frozen_array.freeze
    @frozen_array
  end

  class MyArray < Array; end

  class Sexp < Array
    def initialize(*args)
      super(args)
    end
  end

  class ToAryArray < Array
    def to_ary() ["to_ary", "was", "called!"] end
  end

  class MyRange < Range; end

  class AssocKey
    def ==(other); other == 'it'; end
  end

  class D 
    def <=>(obj) 
      return 4 <=> obj unless obj.class == D
      0
    end
  end
  
  class SubArray < Array
    attr_reader :special
    
    def initialize(size=0)
      @special = size
    end
  end
end

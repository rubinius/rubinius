module StringSpecs
  class MyString < String; end
  class MyArray < Array; end
  class MyRange < Range; end

  class SubString < String
    attr_reader :special
    
    def initialize(size=0)
      @special = size
    end
  end
end

module StringSpecs
  class MyString < String; end
  class MyArray < Array; end
  class MyRange < Range; end

  class SubString < String
    attr_reader :special
    
    def initialize(str=nil)
      @special = str
    end
  end
end

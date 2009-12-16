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
  
  class StringWithRaisingConstructor < String
    def initialize(str)
      raise ArgumentError.new('constructor was called') unless str == 'silly:string'
      self.replace(str)
    end
  end

  class StringNotReallyConvertable
    def to_str
      raise "Oups"
    end
  end

end

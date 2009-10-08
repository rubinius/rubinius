module MethodSpecs


  class SourceLocation
    def self.location # This needs to be on this line 
      :location       # for the spec to pass
    end
    
    def self.redefined
      :first
    end

    def self.redefined
      :last
    end

    def original
    end

    alias :aka :original 
  end

end

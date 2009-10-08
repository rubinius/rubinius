module MethodSpecs


  class SourceLocation
    def location # This needs to be on this line 
      :location  # for the spec to pass
    end
    
    def redefined
      :first
    end

    def redefined
      :last
    end

    def original
    end

    alias :aka :original 
  end

end

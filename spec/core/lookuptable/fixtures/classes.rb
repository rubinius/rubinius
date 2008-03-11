module LookupTableSpecs
  class SubTable < LookupTable
    attr_reader :special
    
    def initialize(size)
      @special = size
    end
  end
end

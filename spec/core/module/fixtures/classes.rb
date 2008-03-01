module ModuleSpecs
  class SubModule < Module
    attr_reader :special
    def initialize
      @special = 10
    end
  end
end

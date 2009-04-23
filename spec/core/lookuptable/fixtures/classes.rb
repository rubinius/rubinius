module LookupTableSpecs
  class SubTable < Rubinius::LookupTable
    attr_reader :special

    def initialize(size)
      @special = size
    end
  end
end

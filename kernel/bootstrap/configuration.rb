module Rubinius
  class Configuration
    def initialize
    end

    private :initialize

    def get_variable(name)
      Rubinius.primitive :vm_get_config_item
      raise PrimitiveFailure, "Unable to get config variable"
    end

    def get_section(section)
      Rubinius.primitive :vm_get_config_section
      raise PrimitiveFailure, "Unable to get config section"
    end

    def section(section)
      ary = get_section(section)
      i = 0
      while i < ary.size
        tup = ary[i]
        yield tup[0], tup[1]
        i += 1
      end
    end

    def [](name)
      get_variable(name)
    end

    alias_method :get, :[]
  end

  Config = Configuration.new
end

module Rubinius
  class Configuration
    def initialize
    end

    private :initialize

    def get_variable(name)
      Rubinius.primitive :vm_get_config_item
      raise PrimitiveFailure, "Rubinius::Configuration#get_variable primitive failed"
    end

    def get_section(section)
      Rubinius.primitive :vm_get_config_section
      raise PrimitiveFailure, "Rubinius::Configuration#get_section primitive failed"
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

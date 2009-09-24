module Rubinius
  class StaticScope
    def const_set(name, value)
      @module.const_set name, value
    end
  end
end

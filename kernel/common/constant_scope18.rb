# -*- encoding: us-ascii -*-

module Rubinius
  class ConstantScope

    def const_defined?(name)
      scope = self
      while scope and scope.module != Object
        return true if scope.module.const_defined?(name)
        scope = scope.parent
      end

      return Object.const_defined?(name)
    end
  end
end

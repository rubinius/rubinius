module Rubinius
  class Reflector

    def self.new(target)
      Rubinius.primitive :reflector_create
      raise PrimitiveFailure, "Reflector.new failed"
    end

    def get(ivar)
      Rubinius.primitive :reflector_get
      raise PrimitiveFailure, "Reflector#get failed"
    end

    def set(ivar, name)
      Rubinius.primitive :reflector_set
      raise TypeError, "'#{name}' is not a ivar name"
    end
  end
end

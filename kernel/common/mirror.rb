module Rubinius
  class Mirror
    def self.reflect(obj)
      klass = Rubinius.invoke_primitive :module_mirror, obj
      klass.new obj if klass
    end

    def initialize(obj)
      @object = obj
    end
  end
end

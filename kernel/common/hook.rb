module Rubinius
  class Hook
    def initialize
      @hooks = []
    end

    def add(obj)
      @hooks << obj
    end

    def remove(obj)
      @hooks.delete(obj)
    end

    def trigger!(*args)
      @hooks.each do |obj|
        obj.call(*args)
      end
    end
  end
end

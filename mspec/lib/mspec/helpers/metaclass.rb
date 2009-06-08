class Object
  unless method_defined? :metaclass
    def metaclass
      class << self; self; end
    end
  end
end

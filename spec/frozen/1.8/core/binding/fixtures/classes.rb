module BindingSpecs
  class Demo
    def initialize(n)
      @secret = n
    end
  
    def square(n)
      n * n
    end

    def get_binding
      a = true
      binding
    end
  end
end

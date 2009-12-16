module ClassSpecs
  class A; end
  
  class B
    @@cvar = :cvar
    @ivar = :ivar
  end
  
  class C
    def self.make_class_variable
      @@cvar = :cvar
    end

    def self.make_class_instance_variable
      @civ = :civ
    end
  end
  
  class D
    def make_class_variable
      @@cvar = :cvar
    end
  end
  
  class E
    def self.cmeth() :cmeth end
    def meth() :meth end
    
    class << self
      def smeth() :smeth end
    end
    
    CONSTANT = :constant!
  end
  
  class F; end
  class F
    def meth() :meth end
  end
  class F
    def another() :another end
  end
  
  class G
    def override() :nothing end
    def override() :override end
  end
  
  class Container
    class A; end
    class B; end
  end

  O = Object.new
  class << O
    def smeth
      :smeth
    end
  end
  
  class H
    def self.inherited(sub)
      track_inherited << sub
    end
    
    def self.track_inherited
      @inherited_modules ||= []
    end
  end
  
  class K < H; end
  
  class I
    class J < self 
    end
  end

  class K
    def example_instance_method
    end
    def self.example_class_method
    end
  end

  class L; end

  class M < L; end
end

class Class
  def example_instance_method_of_class; end
  def self.example_class_method_of_class; end
end
class << Class
  def example_instance_method_of_metaclass; end
  def self.example_class_method_of_metaclass; end
end
class Object
  def example_instance_method_of_object; end
  def self.example_class_method_of_object; end
end

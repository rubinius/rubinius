module ConstantSpecs

module A
  Place = "boston"
  
  module B  
    Name = "specs"
        
    class C
      Number = 47
      
      def self.number
        Number
      end
      
      def self.name
        Name
      end
      
      def self.place
        Place
      end
      
      def number
        Number
      end
      
      def name
        Name
      end
      
      def place
        Place
      end
      
      def fire_missing
        NotAround
      end
      
      def self.const_missing(name)
        :missing!
      end
    end
  end
end

class D < A::B::C
  def self.number
    Number
  end
  
  def number
    Number
  end
end

TopNumber = 8

class E
  
  TopNumber = 1

  def go
    F.new.go
  end
end

class F
  def go
    TopNumber
  end
end

module G
  def go
    Exception
  end  
end

class H
  include G
end

class I
  
  Exception = 10
  
  def go
    H.new.go
  end
end

class Foo
  COMMAND_LINE = 47

  def Foo.foo
    Bar.bar do
      COMMAND_LINE
    end
  end

  def Foo.foo2
    Bar.bar do

      obj = Object.new
      def obj.foo
        COMMAND_LINE
      end

      obj.foo
    end
  end
end

class Bar
  def Bar.bar
    yield
  end
end

Foo.foo


end


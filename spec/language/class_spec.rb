require File.dirname(__FILE__) + '/../spec_helper'

# Specifications:
#
# Keywords:
#   class
#   def
#
# Behaviour:
#   Constants
#   Class variables
#   Class instance variables
#   Instance variables
#

describe "A new class definition" do
  it "should create a new class" do
    class LanguageClassA; end

    LanguageClassA.class.should == Class
    LanguageClassA.new.class.should == LanguageClassA
  end
  
  it "should have no class variables" do
    class LanguageClassB; end;
    
    LanguageClassB.class_variables.should == []
  end
  
  it "should have no class-level instance variables" do
    class LanguageClassC; end
    
    LanguageClassC.instance_variables.should == []
  end

  it "should allow the declaration of class variables in the body" do
    class LanguageClassD
      @@bar = 'foo'
    end
    
    LanguageClassD.class_variables.should == ["@@bar"]
  end
  
  it "should allow the declaration of class-level instance variables in the body" do
    class LanguageClassE
      @bar = 'foo'
    end
    
    LanguageClassE.instance_variables.should == ["@bar"]
  end

  it "should allow the declaration of class variables in a class method" do
    class LanguageClassF
      def self.setup_cv
        @@bar = 'foo'
      end
    end

    LanguageClassF.class_variables.should == []
    LanguageClassF.setup_cv
    LanguageClassF.class_variables.should == ["@@bar"]
  end
  
  it "should allow the declaration of class variables in an instance method" do
    class LanguageClassG
      def setup_cv
        @@bar = 'foo'
      end
    end

    LanguageClassG.class_variables.should == []
    LanguageClassG.new.setup_cv
    LanguageClassG.class_variables.should == ["@@bar"]
  end
  
  it "should allow the definition of methods" do
    class LanguageClassH
      def foo
        'foo'
      end
    end
    
    LanguageClassH.new.foo.should == 'foo'
  end
  
  it "should allow the definition of class methods" do
    class LanguageClassI
      def self.foo
        'foo'
      end
    end
    
    LanguageClassI.foo.should == 'foo'
  end
  
  it "should allow the definition of class methods using class << self" do
    class LanguageClassJ
      class << self
        def foo
          'foo'
        end
      end
    end
    
    LanguageClassJ.foo.should == 'foo'
  end
  
  it "should allow the definition of Constants" do
    class LanguageClassO; CONST = 'foo!'; end
    
    Object.const_defined?('CONST').should == false
    LanguageClassO.const_defined?('CONST').should == true
    LanguageClassO::CONST.should == 'foo!'
  end
  
  it "should return the value of the last statement in the body" do
    value = class LanguageClassK; end
    value.should == nil
    
    value = class LanguageClassL; 20; end
    value.should == 20
    
    value = class LanguageClassM; 20 + 10; end
    value.should == 30
    
    value = class LanguageClassN; class << self; 'foo'; end; end
    value.should == 'foo'
  end
end

describe "Nested class definitions" do
  it "should make the outer class contain the inner classes" do
    class LanguageClassZ
      class LanguageClassA1; end
      class LanguageClassA2; end
    end
    
    LanguageClassZ.constants.include?('LanguageClassA1').should == true
    LanguageClassZ.constants.include?('LanguageClassA2').should == true
  end
end

describe "A Class Definitions extending an object" do
  it "should allow adding methods" do
    a = "a string"
    class << a
      def xyz
        self
      end
    end
    
    a.xyz.should == "a string"
  end
  
  it "should raise a TypeError when trying to extend numbers" do
    should_raise(TypeError) do
      eval <<-CODE
        class << 1
          def xyz
            self
          end
        end
      CODE
    end
  end
end

describe "Multiple Definitions of the same Class" do
  it "should extend previous definitions" do
    class LanguageClassX; def abc(); 'foo' end; end
    class LanguageClassX; def xyz(); 'bar' end; end
    
    x = LanguageClassX.new
    x.abc.should == 'foo'
    x.xyz.should == 'bar'
  end
  
  it "should overwrite existing methods" do
    class LanguageClassW; def abc() 'bar' end; end
    class LanguageClassW; def abc() 'foo' end; end
    
    LanguageClassW.new.abc.should == 'foo'
  end
  
  it "should raise a TypeError when superclasses mismatch" do
    should_raise(TypeError) do
      class LanguageClassV < Array; end
      class LanguageClassV < Fixnum; end
    end    
  end
end

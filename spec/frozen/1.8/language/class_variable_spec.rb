require File.dirname(__FILE__) + '/../spec_helper'

module ClassVariableSpec
  class A
    @@a_cvar = :a_cvar
    def a_cvar() @@a_cvar end
    def a_cvar=(val) @@a_cvar = val end
  end

  class B < A; end

  module M
    @@cvar = :value

    def cvar
      @@cvar
    end
    
    def cvar=(val)
      @@cvar = val
    end
  end

  class C
    extend M
    
    def self.cvar?
      self.class_variable_defined?(:@@cvar)
    end
    
    def self.cvar2=(val)
      @@cvar = val
    end
  end
end

describe "A Class Variable" do
  it "can be accessed from a subclass" do
    ClassVariableSpec::B.new.a_cvar.should == :a_cvar
  end

  it "sets the value in the superclass from the subclass" do
    a = ClassVariableSpec::A.new
    b = ClassVariableSpec::B.new
    b.a_cvar = :new_val

    [a.a_cvar].should == [:new_val]
  end

  it "retrieves the value from the place it is defined" do
    [ClassVariableSpec::C.cvar.should] == [:value]
  end
end

describe "A Class Variable defined in a Module" do
  it "can be accessed from Classes that are extended by the Module" do
    ClassVariableSpec::C.cvar.should == :value
  end
  
  it "is not defined in these Classes" do
    ClassVariableSpec::C.cvar?.should be_false
  end
  
  it "only updates the Class Variable in the Module when using a Method defined in the Module" do
    ClassVariableSpec::C.cvar = "new value"
    ClassVariableSpec::C.cvar.should == "new value"
    
    ClassVariableSpec::C.cvar?.should be_false
  end
  
  it "does updates the Class Variable in the Module but in the Class when using a Method defined in the Class" do
    ClassVariableSpec::C.cvar2 = "new value"    
    ClassVariableSpec::C.cvar?.should be_true
  end
  
  it "allows to retrieve the Class Variable in the Class with methods from the Module" do
    ClassVariableSpec::C.cvar2 = "new value"
    
    ClassVariableSpec::C.cvar.should == "new value"
  end
end

require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/class'

describe "self in a metaclass body (class << obj)" do
  it "is TrueClass for true" do
    class << true; self; end.should == TrueClass
  end

  it "is FalseClass for false" do
    class << false; self; end.should == FalseClass
  end

  it "is NilClass for nil" do
    class << nil; self; end.should == NilClass
  end

  it "raises a TypeError for numbers" do
    lambda { class << 1; self; end }.should raise_error(TypeError)
  end

  it "raises a TypeError for symbols" do
    lambda { class << :symbol; self; end }.should raise_error(TypeError)
  end

  it "is a singleton Class instance" do
    cls = class << mock('x'); self; end
    cls.is_a?(Class).should == true
    cls.should_not equal(Object)
  end
  
  deviates_on(:rubinius) do 
    it "is a MetaClass instance" do
      cls = class << mock('x'); self; end
      cls.is_a?(MetaClass).should == true
    end

    it "has the object's class as superclass" do
      cls = class << "blah"; self; end
      cls.superclass.should == String
    end
  end
  
end

require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/fixtures/constants'

describe "Constant lookup rule" do
  it "finds a toplevel constant" do
    Exception.should == ::Exception
  end
  
  it "looks up the static, lexical scope in a class method" do
    A::B::C.number.should == 47
    A::B::C.name.should == "specs"
    A::B::C.place.should == "boston"
  end
  
  it "looks up the static, lexical scope in an instance method" do
    A::B::C.new.number.should == 47
    A::B::C.new.name.should == "specs"
    A::B::C.new.place.should == "boston"
  end
  
  it "looks up the superclass chain" do
    D.new.number.should == 47
    D.number.should == 47
  end
  
  it "isn't influenced by the calling scope" do
    E.new.go.should == 8
  end
  
  it "isn't influenced by the calling scope, in modules" do
    I.new.go.should == ::Exception
  end
  
  it "calls const_missing on the original scope" do
    A::B::C.new.fire_missing.should == :missing!
  end  
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Method#==" do
  before(:each) do
    @m = MethodSpecs::Methods.new
    @m2 = MethodSpecs::Methods.new
    @a = MethodSpecs::A.new
  end

  it "returns true if methods are the same" do
    m1 = @m.method(:foo)
    m2 = @m.method(:foo)

    (m1 == m1).should == true
    (m1 == m2).should == true
  end

  it "returns true on aliased methods" do
    m1 = @m.method(:foo)
    m2 = @m.method(:bar)

    (m1 == m2).should == true
  end
  
  it "returns false on a method which is neither aliases nor the same method" do
    m1 = @m.method(:foo)
    m2 = @m.method(:zero)
    
    (m1 == m2).should == false
  end
  
  it "returns false for a method which is not bound to the same object" do
    m1 = @m.method(:foo)
    m2 = @m2.method(:foo)

    a = @a.method(:baz)
    
    (m1 == m2).should == false
    (m1 == a).should == false
    (m2 == a).should == false
  end
end

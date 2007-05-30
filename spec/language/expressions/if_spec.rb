require File.dirname(__FILE__) + '/../../spec_helper'

describe "The if expression" do
  it "should evaluate the if body when the expression is true" do
    if true
      a = true
    else
      a = false
    end
    
    a.should == true
  end
  
  it "should return the last statement in the body" do
    if true
      'foo'
      'bar'
      'baz'
    end.should == 'baz'
  end

  it "should evaluate the else body when the expression is false" do
    if false
      'foo'
    else
      'bar'
    end.should == 'bar'
  end
  
  it "should take an optional then after the expression" do
    if true then
      'baz'
    end.should == 'baz'
  end
  
  it "should have no return value when the expression is false" do
    if false; end.should == nil
  end

  it "should allow expression and body to be on one line (using ':')" do
    if true: 'foo'; else 'bar'; end.should == 'foo'
  end
  
  it "should allow expression and body to be on one line (using 'then')" do
    if true then 'foo'; else 'bar'; end.should == 'foo'
  end
end
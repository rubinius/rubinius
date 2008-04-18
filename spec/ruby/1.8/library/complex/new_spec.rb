require File.dirname(__FILE__) + '/../../spec_helper'
require 'complex'


describe "Complex.new" do
  it "needs to be reviewed for spec completeness" do
  end
  
  it "creates a new object of class Complex" do
    Complex.new(1, 2).class.should == Complex
  end
  
  it "raises an error with non-Numeric arguments" do
    lambda { Complex.new(1, "foo") }.should raise_error
    lambda { Complex.new("bar", 2) }.should raise_error
  end
end

describe "Complex.new!" do
  it "needs to be reviewed for spec completeness" do
  end
  
  it "creates a new object of class Complex" do
    Complex.new!(1, 2).class.should == Complex
  end
  
  it "defaults to 0 for the second argument" do
    Complex.new!(5).should == Complex.new!(5, 0)
    lambda { Complex.new!("foo") }.should raise_error
  end
  
  it "behaves just like Complex.new when given 2 arguments" do
    Complex.new!(5, 6).should == Complex.new(5, 6)
    lambda { Complex.new(1, "foo") }.should raise_error
  end
end
require File.dirname(__FILE__) + '/../spec_helper'

# &, ^, to_s, |

describe "FalseClass#&" do
  def false_and(other)
    false & other
  end
  
  it "returns false" do
    false_and(false).should == false
    false_and(true).should == false
    false_and(nil).should == false
    false_and("").should == false
    false_and(Object.new).should == false
  end
end

describe "FalseClass#^" do
  def false_xor(other)
    false ^ other
  end
  
  it "returns false if other is nil or false, otherwise true" do
    false_xor(false).should == false
    false_xor(true).should == true
    false_xor(nil).should == false
    false_xor("").should == true
    false_xor(Object.new).should == true
  end
end

describe "FalseClass#to_s" do
  it "returns the string 'false'" do
    false.to_s.should == "false"
  end
end

describe "FalseClass#|" do
  def false_or(other)
    false | other
  end
  
  it "return false if other is nil or false, otherwise true" do
    false_or(false).should == false
    false_or(true).should == true
    false_or(nil).should == false
    false_or("").should == true
    false_or(Object.new).should == true
  end
end

describe "FalseClass#inspect" do
  it "returns the string 'false'" do
    false.inspect.should == "false"
  end
end

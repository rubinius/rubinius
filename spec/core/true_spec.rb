require File.dirname(__FILE__) + '/../spec_helper'

# &, ^, to_s, |

describe "TrueClass#&" do
  def true_and(other)
    true & other
  end
  
  it "returns false if other is nil or false, otherwise true" do
    true_and(true).should == true
    true_and(false).should == false
    true_and(nil).should == false
    true_and("").should == true
    true_and(Object.new).should == true
  end
end

describe "TrueClass#^" do
  def true_xor(other)
    true ^ other
  end
  
  it "returns true if other is nil or false, otherwise false" do
    true_xor(true).should == false
    true_xor(false).should == true
    true_xor(nil).should == true
    true_xor("").should == false
    true_xor(Object.new).should == false
  end
end

describe "TrueClass#to_s" do
  it "returns the string 'true'" do
    true.to_s.should == "true"
  end
end

describe "TrueClass#|" do
  def true_or(other)
    true | other
  end
  
  it "returns true" do
    true_or(true).should == true
    true_or(false).should == true
    true_or(nil).should == true
    true_or("").should == true
    true_or(Object.new).should == true
  end
end

describe "TrueClass#inspect" do
  it "returns the string 'true'" do
    true.inspect.should == "true"
  end
end

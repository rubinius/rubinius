require File.dirname(__FILE__) + '/../spec_helper'

# &, ^, inspect, nil?, to_a, to_f, to_i, to_s, |

describe "NilClass#&" do
  def nil_and(other)
    nil & other
  end
  
  it "returns false" do
    nil_and(nil).should == false
    nil_and(true).should == false
    nil_and(false).should == false
    nil_and("").should == false
    nil_and(Object.new).should == false
  end
end

describe "NilClass#^" do
  def nil_xor(other)
    nil ^ other
  end
  
  it "returns false if other is nil or false, otherwise true" do
    nil_xor(nil).should == false
    nil_xor(true).should == true
    nil_xor(false).should == false
    nil_xor("").should == true
    nil_xor(Object.new).should == true
  end
end

describe "NilClass#inspect" do
  it "returns the string 'nil'" do
    nil.inspect.should == "nil"
  end
end

describe "NilClass#nil?" do
  it "returns true" do
    nil.nil?.should == true
  end
end

describe "NilClass#to_a" do
  it "returns an empty array" do
    nil.to_a.should == []
  end
end

describe "NilClass#to_f" do
  it "returns 0.0" do
    nil.to_f.should == 0.0
  end
  
  it "does not cause NilClass to be coerced to Float" do
    (0.0 == nil).should == false
  end
end

describe "NilClass#to_i" do
  it "returns 0" do
    nil.to_i.should == 0
  end
  
  it "does not cause NilClass to be coerced to Fixnum" do
    (0 == nil).should == false
  end
end

describe "NilClass#to_s" do
  it "returns the string ''" do
    nil.to_s.should == ""
  end
end

describe "NilClass#|" do
  def nil_or(other)
    nil | other
  end
  
  it "returns false if other is nil or false, otherwise true" do
    nil_or(nil).should == false
    nil_or(true).should == true
    nil_or(false).should == false
    nil_or("").should == true
    nil_or(Object.new).should == true
  end
end

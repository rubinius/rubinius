require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#==" do
  it "should true if self has the same value as other" do
    (1 == 1).should == true
    (9 == 5).should == false
  end
  
  it "coerces fixnum and return true if self has the same value as other" do
    (1 == 1.0).should == true
    (2 == 0xffffffff).should == false
  end

  it "calls 'other == self' if coercion fails" do
    class X; def ==(other); 2 == other; end; end

    (1 == X.new).should == false
    (2 == X.new).should == true
  end
end

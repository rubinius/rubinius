require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#+@" do 
  it "returns the same value with same sign (twos complement)" do 
    34.56.send(:+@).should == 34.56
    -34.56.send(:+@).should == -34.56
    0.0.send(:+@).should == 0.0
  end  
end

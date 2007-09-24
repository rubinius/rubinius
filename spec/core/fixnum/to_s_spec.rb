require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#to_s" do
  it "returns self converted to a String" do
    255.to_s.should == '255'
    3.to_s.should == '3'
    0.to_s.should == '0'
    -9002.to_s.should == '-9002'
  end
end

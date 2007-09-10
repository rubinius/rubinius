require File.dirname(__FILE__) + '/../../spec_helper'

describe "Integer.induced_from" do
  it "converts other to Integer" do
    Integer.induced_from(2.5).should == 2 
    Integer.induced_from(-3.14).should == -3 
    Integer.induced_from(1.233450999123389e+12).should == 1233450999123
  end
end

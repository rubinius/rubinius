require 'matrix'
require File.dirname(__FILE__) + '/../../spec_helper'

describe "Matrix#initialize" do
  
  it "is private" do
    Matrix[].private_methods.should include("initialize")
  end
  
  it "should return an instance" do
    Matrix[].class.should == Matrix
  end
  
end

require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix.row_vector" do
  
  it "returns a Matrix" do
    Matrix.row_vector([]).should be_an_instance_of(Matrix)
  end

  it "returns a single-row Matrix with the specified values" do
    Matrix.row_vector([1,2]).should == Matrix[ [1,2] ]
  end  
 
  it "returns an empty Matrix when called with an empty Array" do
    Matrix.row_vector([]).should == Matrix[ [] ]
  end

  it "returns a single-element Matrix when called with a Fixnum" do
    Matrix.row_vector(1).should == Matrix[[1]]
  end  
   
end

require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#**" do

  it "multiples the Matrix by itself _n_ times" do
    (Matrix[ [7,6], [3,9] ] ** 2).should == Matrix[ [67, 96], [48,99] ]
  end
  
  it "raises a ErrOperationNotDefined exception for powers that aren't Integers" do
    lambda {Matrix[ [1,2], [8,2] ] ** 2.5}.should 
      raise_error(Matrix::ErrOperationNotDefined)
  end  

end

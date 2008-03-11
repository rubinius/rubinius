require 'matrix'
require File.dirname(__FILE__) + '/../../spec_helper'

describe "Matrix.+" do
  
  before :each do
    @a = Matrix[ [1,2], [3,4] ]
    @b = Matrix[ [4,5], [6,7] ]
  end
  
  it "should only add Matrices" do
    lambda { @a + @b }.should_not raise_error
    lambda { @a + Object.new }.should raise_error
  end
  
  it "should return an instance of Matrix" do
    (@a + @b).class.should == Matrix
  end
  
  it "should only add Matrices of the same size" do
    lambda { @a + Matrix[ 1 ] }.should raise_error Matrix::ErrDimensionMismatch
  end
  
  it "should return the correct result" do
    (@a + @b).should == Matrix[ [5,7], [9,11] ]
  end
  
end

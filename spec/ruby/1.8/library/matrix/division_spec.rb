require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix./" do
  
  before :each do
    @a = Matrix[ [1,2],
                 [3,4] ]
    @b = Matrix[ [4,5],
                 [6,7] ]
  end
  
  describe "arguments" do
    
    it "should accept a Matrix" do
      lambda { @a / @b }.should_not raise_error
    end

    it "should accept a Numeric" do
      lambda { @a / 1 }.should_not raise_error
    end
    
    it "should only multiply Matrices of the same size" do
      lambda { @a / Matrix[ 1 ] }.should raise_error Matrix::ErrDimensionMismatch
    end
    
  end
  
  describe "result" do
    
    it "should return an instance of Matrix" do
      (@a / @b).class.should == Matrix
    end
    
    it "should return the correct result" do
      (@a / @b).should == Matrix[[-2, 0], [-4, 0]]
      # Second test just so that the library can't return a static number
      (@a / @b / @b).should == Matrix[ [0,0], [0,0] ]
    end
    
    it "should not be multiplicable" do
      (@a / @b).should_not == (@b * @a)
    end
    
  end
  
end

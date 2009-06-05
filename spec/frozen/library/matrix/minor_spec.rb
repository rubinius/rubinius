require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#minor with start_row, nrows, start_col, ncols" do
  
  before(:each) do
    @matrix = Matrix[ [1,2], [3,4], [5,6] ]
  end  
  
  it "returns the given portion of the Matrix" do
    @matrix.minor(0,1,0,2).should == Matrix[ [1, 2] ]
    @matrix.minor(1,2,1,1).should == Matrix[ [4], [6] ]
  end
 
  it "returns an empty Matrix unless nrows and ncols are greater than 0" do
    @matrix.minor(0,0,0,0).should == Matrix[]
    @matrix.minor(1,0,1,0).should == Matrix[]
    @matrix.minor(1,0,1,1).should == Matrix[]
    @matrix.minor(1,1,1,0).should == Matrix[[]] # FIXME: Inconsistent
  end

  # This actually raises a NoMethodError: "(undefined method `call' for
  #  Matrix[]:Matrix)", but we don't spec for this as it is yet another 
  #  bug in Matrix.
  it "raises for out-of-bounds arguments" do
    @matrix.minor(3,0,0,0).should raise_error
    @matrix.minor(0,0,0,9).should raise_error
    lambda{@matrix.minor(9,9,9,9)}.should raise_error
  end  

end

describe "Matrix#minor with col_range, row_range" do
  
  before(:each) do
    @matrix = Matrix[ [1,2], [3,4], [5,6] ]
  end  
  
  it "returns the given portion of the Matrix" do
    @matrix.minor(0..0,0..1).should == Matrix[ [1, 2] ]
    @matrix.minor(1..2,1..2).should == Matrix[ [4], [6] ]
    @matrix.minor(0...3,0...3).should == @matrix
  end
 
  it "returns an empty Matrix if col_range or row_range don't select any elements" do
    @matrix.minor(3..6,3..6).should == Matrix[]
  end

end

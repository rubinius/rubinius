require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#square?" do
  
  it "returns true when the Matrix is square" do
    Matrix[ [1,2], [2,4] ].square?.should be_true
    Matrix[ [100,3,5], [9.5, 4.9, 8], [2,0,77] ].square?.should be_true
  end

  # .square? should return false if the Matrix is non square. However, there
  # is a bug whereby .square? will return true as long as the first row has
  # the same number of columns as the Matrix has rows. Hence the awkward
  # examples.
  it "returns false when the Matrix is not square and the first row has a
  different number of columns than the Matrix has rows" do
    Matrix[ [1, 2] ].square?.should be_false
    Matrix[ [1], [9,7], [2] ].square?.should be_false
  end

  # FIXME: Remove guard when bug is fixed.
  ruby_bug "#1515", "1.9.1.192" do 
    it "returns false when the Matrix is not square
    and the first row has the same number of columns as the matrix has rows." do
      Matrix[ [1,2,3], [4,5], [6] ].square?.should be_false 
    end
  end  

end  

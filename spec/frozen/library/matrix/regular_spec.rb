require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#regular?" do

  # Bug #1020 and an unfamiliarity with properties of matrices stopped me
  # from writing a complete specification here. 
  it "needs to be reviewed for spec completeness"

  it "returns false if the Matrix is ragged" do
    Matrix[ [1,2], [3], [2,1] ].regular?.should be_false
  end

  # On Ruby 1.9.1 the below tests cause hangs, presumably due to bug #1020.
  # They pass in 1.8.7. When #1020 is fixed, we can determine whether it was
  # the cause of this failure...
  ruby_bug "#1020", "1.9.1.129" do
    it "returns false unless rank(A) != n" do
      m = Matrix[ [1,2,3], [3,4,3], [0,0,0] ]
      m.square?.should be_true
      m.rank.should_not == m.column_size
      m.regular?.should be_false
    end 

    it "returns false if the determinant is 0" do
      m = Matrix[ [1,2,9], [3,4,9], [1,2,9] ]
      m.det.should == 0
      m.square?.should be_true
      m.regular?.should be_false
    end

    it "returns true if the Matrix is regular" do
      Matrix[ [0,1], [1,0] ].regular?.should be_true
    end  
  end    

end

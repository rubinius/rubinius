require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/spec_helper'
require 'matrix'

describe "Matrix#/" do
  before :each do
    @a = Matrix[ [1, 2], [3, 4] ]
    @b = Matrix[ [4, 5], [6, 7] ]
    @c = Matrix[ [1.2, 2.4], [3.6, 4.8] ]
  end

  ruby_bug "?", "1.9" do
    it "returns the result of dividing self by another Matrix" do
      (@a / @b).should be_close_to_matrix([[2.5, -1.5], [1.5, -0.5]])
    end
  end

  conflicts_with :Prime do
    it "returns the result of dividing self by a Fixnum" do
      (@a / 2).should == Matrix[ [0, 1], [1, 2] ]
    end
  end

  conflicts_with :Prime do
    it "returns the result of dividing self by a Bignum" do
      (@a / bignum_value).should == Matrix[ [0, 0], [0, 0] ]
    end
  end

  it "returns the result of dividing self by a Float" do
    (@c / 1.2).should == Matrix[ [1, 2], [3, 4] ]
  end

  it "raises a Matrix::ErrDimensionMismatch if the matrices are different sizes" do
    lambda { @a / Matrix[ [1] ] }.should raise_error(Matrix::ErrDimensionMismatch)
  end

  it "returns an instance of Matrix" do
    (@a /@b).should be_kind_of(Matrix)
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#-" do
  before :each do
    @a = Matrix[ [1, 2], [3, 4] ]
    @b = Matrix[ [4, 5], [6, 7] ]
  end

  it "returns the result of subtracting the corresponding elements of other from self" do
    (@a - @b).should == Matrix[ [-3,-3], [-3,-3] ]
  end

  it "returns an instance of Matrix" do
    (@a - @b).should be_kind_of(Matrix)
  end

  it "raises a Matrix::ErrDimensionMismatch if the matrices are different sizes" do
    lambda { @a - Matrix[ [1] ] }.should raise_error(Matrix::ErrDimensionMismatch)
  end

  it "raises a ExceptionForMatrix::ErrOperationNotDefined if other is a Numeric Type" do
    lambda { @a - 2            }.should raise_error(ExceptionForMatrix::ErrOperationNotDefined)
    lambda { @a - 1.2          }.should raise_error(ExceptionForMatrix::ErrOperationNotDefined)
    lambda { @a - bignum_value }.should raise_error(ExceptionForMatrix::ErrOperationNotDefined)
  end

  it "raises an exception if other is not a Matrix" do
    # Note that MRI raises NoMethodError because #coerce is called
    # on objects that don't provide it. This appears to be more of
    # an "oops" rather than an aspect of the interface. We don't
    # spec the exception class.

    lambda { @a - nil        }.should raise_error
    lambda { @a - "a"        }.should raise_error
    lambda { @a - [ [1, 2] ] }.should raise_error
    lambda { @a - Object.new }.should raise_error
  end
end

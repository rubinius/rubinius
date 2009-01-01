require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#column_size" do
  it "returns the number of elements in a column" do
    data =  [[1,2]]
    Matrix[ *data ].column_size.should == 2
  end

  it "raises an exception on an empty Matrix" do
    # Note that MRI raises NoMethodError because #size is called
    # on nil. This appears to be more of an "oops" rather than
    # an aspect of the interface. We don't spec the exception class.
    lambda{ Matrix[].column_size }.should raise_error
  end
end


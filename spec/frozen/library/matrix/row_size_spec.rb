require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#row_size" do
  it "returns the number of elements in a row" do
    data =  [[1,2], [3, 4]]
    Matrix[ *data ].row_size.should == 2
  end

  it "returns 0 for an empty Matrix" do
    Matrix[].row_size.should == 0
  end
end

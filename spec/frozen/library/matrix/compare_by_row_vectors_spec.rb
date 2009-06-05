require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#compare_by_row_vectors" do

  before(:all) do
    @elements = [ [1,2], [3,4] ]
    @m = Matrix[ *@elements ]
   end

  it "returns true if the two matrices contain equal elements" do
    @m.compare_by_row_vectors(@elements).should be_true
  end

  it "returns false if the two matrices don't contain equal elements" do
    @m.compare_by_row_vectors(@elements.reverse).should be_false
  end

end

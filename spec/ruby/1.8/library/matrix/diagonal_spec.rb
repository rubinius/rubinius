require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix.diagonal" do
  it "needs to be reviewed for spec completeness" do
  end
  
  before(:each) do
    @m = Matrix.diagonal(10, 11, 12, 13, 14)
  end
  
  it "returns an object of type Matrix" do
    @m.class.should == Matrix
  end
  
  it "puts its arguments on the diagonal of a matrix" do
    (0..4).each do |i|
      @m[i, i].should == i + 10
    end
  end
  
  it "fills all non-diagonal cells with 0" do
    (0..4).each do |i|
      (0..4).each do |j|
        if i != j
          @m[i, j].should == 0
        end
      end
    end
  end
end

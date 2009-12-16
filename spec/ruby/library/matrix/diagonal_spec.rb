require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix.diagonal" do
  before(:each) do
    @m = Matrix.diagonal(10, 11, 12, 13, 14)
  end
  
  it "returns an object of type Matrix" do
    @m.class.should == Matrix
  end

  it "returns a square Matrix of the right size" do
    @m.column_size.should == 5
    @m.row_size.should == 5
  end
  
  it "sets the diagonal to the arguments" do
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

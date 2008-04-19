require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix.zero" do
  it "returns an object of type Matrix" do
    Matrix.zero(3).class.should == Matrix
  end
  
  it "creates a n x n matrix" do
    m3 = Matrix.zero(3)
    m3.row_size.should == 3
    m3.column_size.should == 3
    
    m8 = Matrix.zero(8)
    m8.row_size.should == 8
    m8.column_size.should == 8
  end
  
  it "initializes all cells to 0" do
    size = 10 
    m = Matrix.zero(size)
    
    (0...size).each do |i|
      (0...size).each do |j|
        m[i, j].should == 0
      end
    end
  end
end

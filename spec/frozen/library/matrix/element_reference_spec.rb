require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix.[]" do
  # Matrix.[] is really a constructor, not an element reference function...

  before(:each) do
    @a = [1, 2, 3]
    @b = [4, 5, 6]
    @c = [7, 8, 9]
    @m = Matrix[@a, @b, @c]
  end
  
  it "returns an object of type Matrix" do
    @m.class.should == Matrix
  end
  
  it "makes each argument into a row" do
    @m.row(0).to_a.should == @a
    @m.row(1).to_a.should == @b
    @m.row(2).to_a.should == @c
  end
end

describe "Matrix#[]" do
  it "returns element at (i, j)" do
    m = Matrix[[0, 1, 2], [3, 4, 5], [6, 7, 8], [9, 10, 11]]
    (0..3).each do |i|
      (0..2).each do |j|
        m[i, j].should == (i * 3) + j
      end
    end
  end
end

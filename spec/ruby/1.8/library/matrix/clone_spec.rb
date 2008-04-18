require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#clone" do
  it "needs to be reviewed for spec completeness" do
  end
  
  before(:each) do
    @a = Matrix[[1, 2], [3, 4], [5, 6]]
  end
  
  it "returns a copy of the matrix, but with all the references different" do
    b = @a.clone
    b.class.should == Matrix
    b.should == @a
    b.should_not === @a
    0.upto(2) do |i|
      0.upto(1) do |j|
        b[i, j].should == @a[i, j]
        b[i, j].should_not === @a[i, j]
      end
    end
  end
end

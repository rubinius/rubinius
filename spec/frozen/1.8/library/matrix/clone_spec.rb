require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#clone" do
  before(:each) do
    @a = Matrix[[1, 2], [3, 4], [5, 6]]
  end
  
  it "returns a shallow copy of the matrix" do
    b = @a.clone
    @a.should_not equal(b)
    b.class.should == Matrix
    b.should == @a
    0.upto(@a.row_size - 1) do |i|
      @a.row(i).should_not equal(b.row(i))
    end
  end
  
  before(:each) do
    @a = Matrix[[1, 2], [3, 4], [5, 6]]
  end
  
quarantine! do # The test fails on MRI 1.8.6 pl 111, pl 114, MRI 1.8.7, JRuby
  it "returns a copy of the matrix, but with all the references different" do
    b = @a.clone
    b.class.should == Matrix
    b.should == @a
    b.should_not === @a
    0.upto(@a.row_size - 1) do |i|
      0.upto(@a.column_size - 1) do |j|
        b[i, j].should == @a[i, j]
        b[i, j].should_not === @a[i, j]
      end
    end
  end
end

end

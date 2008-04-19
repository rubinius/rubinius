require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#clone" do
  before(:each) do
    @a = Matrix[[1, 2], [3, 4], [5, 6]]
  end
  
  it "returns a shallow copy of the matrix" do
    b = @a.clone
    b.class.should == Matrix
    b.should == @a
    0.upto(@a.row_size - 1) do |i|
      @a.row(i).object_id.should_not == b.row(i).object_id
    end
  end
end

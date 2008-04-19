require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix.columns" do
  before :each do
    @a = [1, 2]
    @b = [3, 4]
    @m = Matrix.columns([@a, @b])
  end
  
  it "creates a matrix from argument columns" do
    @m.column(0).to_a.should == @a
    @m.column(1).to_a.should == @b
  end

  it "references the original columns" do
    @a << 3
    @b << 6
    @m.column(0).should.equal?(@a)
    @m.column(1).should.equal?(@b)
  end
end

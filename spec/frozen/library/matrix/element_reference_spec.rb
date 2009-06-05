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

  before(:all) do
    @m = Matrix[[0, 1, 2], [3, 4, 5], [6, 7, 8], [9, 10, 11]]
  end

  it "returns element at (i, j)" do
    (0..3).each do |i|
      (0..2).each do |j|
        @m[i, j].should == (i * 3) + j
      end
    end
  end

  # FIXME: Update this guard when the bug is fixed.
  ruby_bug "#1518", "1.9.1.129" do
    # A NoMethodError is raised when the _first_ index is out of bounds,
    # (http://redmine.ruby-lang.org/issues/show/1518); otherwise nil is
    # returned.
    it "returns nil for an invalid index pair" do
      @m[8,1].should be_nil
      @m[1,8].should be_nil
    end
  end
end

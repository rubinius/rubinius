require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash#each_item" do
  before :each do
    @hash = { 1 => :a, 2 => :b, 3 => :c }
  end

  it "yields key, value for each entry" do
    a = []
    @hash.each_item { |k, v| a << [k, v] }
    a.sort.should == [[1, :a], [2, :b], [3, :c]]
  end

  it "raises LocalJumpError if not passed a block" do
    lambda { @hash.each_item }.should raise_error(LocalJumpError)
  end
end

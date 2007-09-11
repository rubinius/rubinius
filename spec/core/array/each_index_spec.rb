require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#each_index" do
  it "it passes the index of each element to the block" do
    a = []
    x = ['a', 'b', 'c', 'd']
    x.each_index { |i| a << i }.equal?(x).should == true
    a.should == [0, 1, 2, 3]
  end
end

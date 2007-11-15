require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Enumerable#partition" do

  it "returns two arrays, the first containing elements for which the block is true, the second containing the rest" do
    EnumerableSpecs::Numerous.new.partition { |i| i % 2 == 0 }.should == [[2, 6, 4], [5, 3, 1]]
  end
  
  it "throws LocalJumpError if called without a block" do
    should_raise(LocalJumpError) { EnumerableSpecs::Numerous.new.partition }
  end

end

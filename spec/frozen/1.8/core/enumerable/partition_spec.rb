require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Enumerable#partition" do
  it "returns two arrays, the first containing elements for which the block is true, the second containing the rest" do
    EnumerableSpecs::Numerous.new.partition { |i| i % 2 == 0 }.should == [[2, 6, 4], [5, 3, 1]]
  end
  
  ruby_version_is "" ... "1.8.7" do
    it "throws LocalJumpError if called without a block" do
      lambda { EnumerableSpecs::Numerous.new.partition }.should raise_error(LocalJumpError)
    end
  end

  ruby_version_is "1.8.7" do
    it "returns an Enumerator if called without a block" do
      EnumerableSpecs::Numerous.new.partition.should be_kind_of(Enumerable::Enumerator)
    end
  end
end

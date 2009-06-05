require File.dirname(__FILE__) + '/../../spec_helper'

describe "Array#combination" do
  ruby_version_is "1.8.7" do
    it "returns an enumerator when no block is provided" do
      [1, 2, 3, 4].combination(2).should be_kind_of(enumerator_class)
    end

    it "returns self when a block is given" do
      a = [1, 2, 3, 4]
      a.combination(2){}.should equal(a)
    end

    it "yields nothing for out of bounds length and return self" do
      a = [1, 2, 3, 4]
      a.combination(5).to_a.should == []
      a.combination(-1).to_a.should == []
    end

    it "yields the excepted combinations" do
      a = [1, 2, 3, 4]
      a.combination(3).to_a.sort.should == [[1,2,3],[1,2,4],[1,3,4],[2,3,4]]
    end

    if "yields [] when length is 0"
      [1,2,3].combination(0).to_a.should == [[]] # one combination of length 0
      [].combination(0).to_a.should == [[]] # one combination of length 0
    end
  end
end
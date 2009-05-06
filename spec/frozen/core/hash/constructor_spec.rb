require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash.[]" do
  it "creates a Hash; values can be provided as the argument list" do
    hash_class[:a, 1, :b, 2].should == new_hash(:a => 1, :b => 2)
    hash_class[].should == new_hash
    hash_class[:a, 1, :b, new_hash(:c => 2)].should ==
      new_hash(:a => 1, :b => new_hash(:c => 2))
  end

  it "creates a Hash; values can be provided as one single hash" do
    hash_class[:a => 1, :b => 2].should == new_hash(:a => 1, :b => 2)
    hash_class[new_hash(1 => 2, 3 => 4)].should == new_hash(1 => 2, 3 => 4)
    hash_class[new_hash].should == new_hash
  end

  it "raises an ArgumentError when passed an odd number of arguments" do
    lambda { hash_class[1, 2, 3] }.should raise_error(ArgumentError)
    lambda { hash_class[1, 2, new_hash(3 => 4)] }.should raise_error(ArgumentError)
  end

  ruby_bug "#", "1.8.6" do
    it "call to_hash" do
      obj = mock('x')
      def obj.to_hash() new_hash(1 => 2, 3 => 4) end
      hash_class[obj].should == new_hash(1 => 2, 3 => 4)
    end
  end

  it "returns an instance of the class it's called on" do
    hash_class[MyHash[1, 2]].class.should == hash_class
    MyHash[hash_class[1, 2]].class.should == MyHash
  end
end

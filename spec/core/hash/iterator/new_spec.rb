require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Hash::Iterator.new" do
  it "initializes the iterator" do
    a = [1, 2, 3, nil, 4, nil, 5, nil, nil, nil, 6]
    iter = Hash::Iterator.new a, a.size
    iter.instance_variable_get(:@bins).should == a
    iter.instance_variable_get(:@records).should == a.size
    iter.instance_variable_get(:@index).should == -1
  end
end

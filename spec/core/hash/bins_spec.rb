require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash#bins" do
  it "returns the storage vector for the Hash" do
    h = Hash.allocate
    h.bins.should == h.instance_variable_get(:@bins)
  end
end

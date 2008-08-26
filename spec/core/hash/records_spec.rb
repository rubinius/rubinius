require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash#records" do
  it "returns the size of the storage vector for the Hash" do
    h = Hash.allocate
    h.records.should == h.instance_variable_get(:@records);
  end
end

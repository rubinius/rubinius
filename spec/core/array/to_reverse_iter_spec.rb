require File.dirname(__FILE__) + '/../../spec_helper'

describe "Array#to_reverse_iter" do
  it "returns an instance of Array::Iterator" do
    [1, 2].to_reverse_iter.should be_an_instance_of(Array::Iterator)
  end
end

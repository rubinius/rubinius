require File.expand_path('../../../spec_helper', __FILE__)

describe "Array#to_iter" do
  it "returns an instance of Array::Iterator" do
    [1, 2].to_iter.should be_an_instance_of(Array::Iterator)
  end
end

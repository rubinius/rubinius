require File.expand_path('../../../../spec_helper', __FILE__)

describe "Array::Iterator.allocate" do
  it "raises a TypeError" do
    lambda { Array::Iterator.allocate }.should raise_error(TypeError)
  end
end

require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Array::Iterator.allocate" do
  it "raises a TypeError" do
    lambda { Array::Iterator.allocate }.should raise_error(TypeError)
  end
end

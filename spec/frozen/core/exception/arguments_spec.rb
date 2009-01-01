require File.dirname(__FILE__) + '/../../spec_helper'

describe "ArgumentError" do
  it "is a subclass of StandardError" do
    StandardError.should be_ancestor_of(ArgumentError)
  end
end

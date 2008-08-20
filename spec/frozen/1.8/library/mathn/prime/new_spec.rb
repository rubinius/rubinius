require File.dirname(__FILE__) + '/../../../spec_helper'
require 'mathn'

describe "Prime.new" do
  it "returns a new Prime number" do
    Prime.new.class.should == Prime
  end
   
  it "raises a TypeError when is called with some arguments" do
    lambda { Prime.new(1) }.should raise_error(ArgumentError)
  end
end

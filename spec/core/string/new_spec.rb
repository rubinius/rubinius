require File.dirname(__FILE__) + '/../../spec_helper'

describe "String.new" do
  it "accepts a Fixnum argument to create a string of length characters" do
    s = String.new 10
    s.size.should == 10
  end
end

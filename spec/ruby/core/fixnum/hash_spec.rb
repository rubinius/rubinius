require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#hash" do
  it "is provided" do
    1.respond_to?(:hash).should == true
  end

  it "is stable" do
    1.hash.should == 1.hash
  end
end

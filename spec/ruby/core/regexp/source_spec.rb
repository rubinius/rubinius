require File.dirname(__FILE__) + '/../../spec_helper'

describe "Regexp#source" do
  it "returns the original string of the pattern" do
    /ab+c/ix.source.should == "ab+c"
    /x(.)xz/.source.should == "x(.)xz"
  end
end

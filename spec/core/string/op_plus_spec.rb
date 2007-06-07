require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#+(other_string)" do
  it "should return a new string containing other_string concatenated to self" do
    ("Ruby !" + "= Rubinius").should == "Ruby != Rubinius"
  end
end
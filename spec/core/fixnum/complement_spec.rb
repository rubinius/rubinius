require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#~" do
  it "returns self bitwise inverted" do
    (~0).should == -1
    (~1221).should == -1222
    (~-599).should == 598
    (~-2).should == 1
  end
end

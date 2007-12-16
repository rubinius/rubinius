require File.dirname(__FILE__) + '/../../spec_helper'

describe "FalseClass#^" do
  it "returns false if other is nil or false, otherwise true" do
    (false ^ false).should == false
    (false ^ true).should == true
    (false ^ nil).should == false
    (false ^ "").should == true
    (false ^ Object.new).should == true
  end
end

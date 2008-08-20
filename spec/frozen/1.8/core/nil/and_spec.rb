require File.dirname(__FILE__) + '/../../spec_helper'

describe "NilClass#&" do
  it "returns false" do
    (nil & nil).should == false
    (nil & true).should == false
    (nil & false).should == false
    (nil & "").should == false
    (nil & mock('x')).should == false
  end
end

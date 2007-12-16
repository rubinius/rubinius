require File.dirname(__FILE__) + '/../../spec_helper'

describe "TrueClass#|" do
  it "returns true" do
    (true | true).should == true
    (true | false).should == true
    (true | nil).should == true
    (true | "").should == true
    (true | Object.new).should == true
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'

describe "TrueClass#inspect" do
  it "returns the string 'true'" do
    true.inspect.should == "true"
  end
end

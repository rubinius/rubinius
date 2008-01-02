require File.dirname(__FILE__) + '/../../spec_helper'

describe "Regexp#inspect" do
  it "returns a formatted string" do
    /ab+c/ix.inspect.should == "/ab+c/ix"
    /a(.)+s/n.inspect.should == "/a(.)+s/n"
  end
end

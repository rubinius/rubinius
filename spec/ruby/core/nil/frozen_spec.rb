require File.expand_path('../../../spec_helper', __FILE__)

describe "NilClass#frozen?" do
  it "returns true" do
    nil.frozen?.should == true
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'

describe "ENV.to_s" do
  it "returns \"ENV\"" do
    ENV.to_s.should == "ENV"
  end
end

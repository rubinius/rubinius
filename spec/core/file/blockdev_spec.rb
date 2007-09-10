require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.blockdev?" do
  it "returns true/false depending if the named file is a block device" do
    File.blockdev?("/tmp").should == false
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.chardev?" do
  it "returns true/false depending if the named file is a char device" do
    File.chardev?("/tmp").should == false
  end
end 

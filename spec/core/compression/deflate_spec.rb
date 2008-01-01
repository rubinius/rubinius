require File.dirname(__FILE__) + '/../../spec_helper'

describe "Compression::ZLib.deflate" do
  it "converts other to compressed data" do
    Compression::ZLib.deflate("blah").should == "x\234K\312I\314\000\000\003\372\001\230"
  end
end

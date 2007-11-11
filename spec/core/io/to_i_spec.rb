require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#to_i" do
  it "return the numeric file descriptor of the given IO object" do
    $stdout.to_i.should == 1
  end
end

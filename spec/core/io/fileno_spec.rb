require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#fileno" do
  it "return the numeric file descriptor of the given IO object" do
    $stdout.fileno.should == 1
  end
end

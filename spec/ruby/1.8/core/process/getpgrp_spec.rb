require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.getpgrp" do
  it "takes no arguments" do
    lambda { Process.getpgrp(1) }.should raise_error(ArgumentError)
  end

  it "returns the process group ID of the calling process" do
    Process.getpgrp.should == Process.pid
  end
end

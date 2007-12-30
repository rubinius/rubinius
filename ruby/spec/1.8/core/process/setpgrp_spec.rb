require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.setpgrp" do
  it "takes no arguments" do
    lambda { Process.setpgrp(0) }.should raise_error(ArgumentError)
  end

  it "returns the process group ID of the calling process" do
    pid = Process.fork { Process.setpgrp; Process.exit! }
    sleep(0.1) # wait for child to change process groups
    Process.getpgid(pid).should == pid
  end

  it "returns zero" do
    Process.setpgrp.should == 0
  end
end

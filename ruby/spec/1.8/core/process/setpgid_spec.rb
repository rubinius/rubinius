require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.setpgid" do
  it "requires two arguments" do
    lambda { Process.setpgid }.should raise_error(ArgumentError)
    lambda { Process.setpgid(0) }.should raise_error(ArgumentError)
  end

  it "sets the process group id of the specified process" do
    pid = Process.fork { Process.exit! }
    Process.getpgid(pid).should == Process.pid
    Process.setpgid(pid, pid).should == 0
    Process.getpgid(pid).should == pid
  end

end

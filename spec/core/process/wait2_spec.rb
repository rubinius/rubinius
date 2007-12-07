require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.wait2" do
  before :each do
    # this kludge is temporarily necessary because some misbehaving
    # spec somewhere else does not clear processes
    Process.waitall
  end
  
  it "should return the pid and status of child process" do
    pidf = Process.fork { exit 99 }
    results = Process.wait2
    results.size.should == 2
    pidw, status = results
    pidf.should == pidw
    status.exitstatus.should == 99
  end

  it "should raise a StandardError if no child processes exist" do
    lambda { Process.wait2 }.should raise_error(Errno::ECHILD)
    lambda { Process.wait2 }.should raise_error(StandardError)
  end
end

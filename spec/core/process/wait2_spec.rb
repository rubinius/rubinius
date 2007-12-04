require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.wait2" do
  it "should return the pid and status of child process" do
    pidf = Process.fork { exit 99 }
    results = Process.wait2
    results.size.should == 2
    pidw,status = results
    (pidf == pidw).should == true
    status.exitstatus.should == 99
  end

  it "should raise a StandardError if no child processes exist" do
    lambda { Process.wait2 }.should raise_error(Errno::ECHILD)
    lambda { Process.wait2 }.should raise_error(StandardError)
  end
end

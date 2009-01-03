require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.pid" do
  it "returns the process id of this process" do
    pid = Process.pid
    pid.class.should == Fixnum
    Process.pid.should == pid 
  end
end

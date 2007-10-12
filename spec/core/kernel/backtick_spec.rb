require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#`" do
  it "returns the result of the executed sub-process" do
    ip = 'world'
    `echo disc #{ip}`.should == "disc world\n"
  end

  it "sets $? to the exit status of the executed sub-process" do
    ip = 'world'
    `echo disc #{ip}`
    $?.class.should == Process::Status
    $?.stopped?.should == false
    $?.exited?.should == true
    $?.exitstatus.should == 0
    $?.success?.should == true
    `echo disc #{ip}; exit 99`
    $?.class.should == Process::Status
    $?.stopped?.should == false
    $?.exited?.should == true
    $?.exitstatus.should == 99
    $?.success?.should == false
  end
end

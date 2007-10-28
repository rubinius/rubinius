require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#`" do
  it "returns the standard output of the executed sub-process" do
    ip = 'world'
    `echo disc #{ip}`.should == "disc world\n"
  end
  
  it "tries to convert the given argument to String using to_str" do
    (obj = Object.new).should_receive(:to_str, :returning => "echo test")
    Kernel.`(obj).should == "test\n"

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :count => :any, :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "echo another test")
    Kernel.`(obj).should == "another test\n"
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

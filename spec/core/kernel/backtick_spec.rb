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
    $?.should == 0
    `echo disc #{ip}; exit 99`
    $?.should == 99
  end
end

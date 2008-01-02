require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.getpgid" do
  it "requires one argument" do
    lambda { Process.getpgid }.should raise_error(ArgumentError)
  end

  it "returns the process group ID for the given process id" do
    Process.getpgid(Process.ppid).should == Process.ppid
  end
end

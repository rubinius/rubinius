require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.euid" do
  it "returns the effective user ID for this process" do
    Process.euid.class.should == Fixnum
  end

  it "also goes by Process::UID.eid" do
    Process::UID.eid.should == Process.euid
  end

  it "also goes by Process::Sys.geteuid" do
    Process::Sys.geteuid.should == Process.euid
  end
end

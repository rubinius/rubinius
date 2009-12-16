require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.egid" do
  it "returns the effective group ID for this process" do
    Process.egid.should be_kind_of(Integer)
  end

  it "also goes by Process::GID.eid" do
    Process::GID.eid.should == Process.egid
  end

  it "also goes by Process::Sys.getegid" do
    Process::Sys.getegid.should == Process.egid
  end
end

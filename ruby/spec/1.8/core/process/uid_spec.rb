require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.uid" do
  it "returns the correct uid for the user executing this process" do
    current_uid_according_to_unix = `id -ur`.to_i
    Process.uid.should == current_uid_according_to_unix
  end

  it "also goes by Process::UID.rid" do
    Process::UID.rid.should == Process.uid
  end

  it "also goes by Process::Sys.getuid" do
    Process::Sys.getuid.should == Process.uid
  end
end

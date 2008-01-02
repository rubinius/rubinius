require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.groups" do
  it "takes no arguments" do
    lambda { Process.groups(1) }.should raise_error(ArgumentError)
  end

  it "gets an Array of the gids of groups in the supplemental group access list" do
    groups = `id -G`.scan(/\d+/).map {|i| i.to_i}
    Process.groups.sort.should == groups.sort
  end

  # NOTE: This is kind of sketchy.
  it "sets the list of gids of groups in the supplemental group access list" do
    groups = Process.groups
    if Process.uid == 0
      Process.groups = []
      Process.groups.should == []
      Process.groups = groups
      Process.groups.sort.should == groups.sort
    else
      lambda { Process.groups = [] }.should raise_error(Errno::EPERM)
    end
  end
end

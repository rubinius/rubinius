require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.groups" do
  platform_is_not :windows do
    it "takes no arguments" do
      lambda { Process.groups(1) }.should raise_error(ArgumentError)
    end

    it "gets an Array of the gids of groups in the supplemental group access list" do
      groups = `id -G`.scan(/\d+/).map {|i| i.to_i}

      # NOTE: if/when this fails, make sure you are running in the most
      # basic environment you have available. This spec fails
      # consistently on me (Ryan Davis) when I'm running the specs
      # inside a shell inside emacs that was launched by OSX's
      # windowserver (double click in finder or quicksilver/launchbar
      # etc). When run any other way the spec passes.
      Process.groups.uniq.sort.should == groups.uniq.sort
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
end

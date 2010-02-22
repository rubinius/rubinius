require File.expand_path('../../../spec_helper', __FILE__)

describe "Process.getpriority" do
  platform_is_not :windows do
    it "takes two arguments" do
      lambda { Process.getpriority }.should raise_error(ArgumentError)
      lambda {
        Process.getpriority(Process::PRIO_PROCESS)
      }.should raise_error(ArgumentError)
    end

    it "gets the scheduling priority for a specified process" do
      Process.getpriority(Process::PRIO_PROCESS, 0).should be_kind_of(Fixnum)
    end

    it "gets the scheduling priority for a specified process group" do
      Process.getpriority(Process::PRIO_PGRP, 0).should be_kind_of(Fixnum)
    end

    it "gets the scheduling priority for a specified user" do
      Process.getpriority(Process::PRIO_USER, 0).should be_kind_of(Fixnum)
    end
  end
end

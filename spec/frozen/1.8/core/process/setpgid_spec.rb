require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.setpgid" do
  it "requires two arguments" do
    lambda { Process.setpgid }.should raise_error(ArgumentError)
    lambda { Process.setpgid(0) }.should raise_error(ArgumentError)
  end

  it "sets the process group id of the specified process" do
    rd, wr = IO.pipe

    pid = Process.fork do
      wr.close
      rd.read
      rd.close
      Process.exit!
    end

    rd.close

    Process.getpgid(pid).should == Process.getpgrp
    Process.setpgid(pid, pid).should == 0
    Process.getpgid(pid).should == pid

    wr.write ' '
    wr.close
  end
end

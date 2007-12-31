require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.setpgrp" do
  it "takes no arguments" do
    lambda { Process.setpgrp(0) }.should raise_error(ArgumentError)
  end

  it "returns the process group ID of the calling process" do
    # there are two synchronization points here:
    # One for the child to let the parent know that it has finished
    #   setting its process group;
    # and another for the parent to let the child know that it's ok to die.
    read1, write1 = IO.pipe
    read2, write2 = IO.pipe
    pid = Process.fork do
      read1.close
      write2.close
      Process.setpgrp
      write1 << "?"
      write1.close
      read2.read(1)
      read2.close
      Process.exit!
    end
    write1.close
    read2.close
    read1.read(1) # wait for child to change process groups
    read1.close

    Process.getpgid(pid).should == pid

    write2 << "!"
    write2.close
  end

  it "returns zero" do
    Process.setpgrp.should == 0
  end
end

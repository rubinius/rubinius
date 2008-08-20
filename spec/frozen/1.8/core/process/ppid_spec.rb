require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.ppid" do
  it "returns the process id of the parent of this process" do

    read, write = IO.pipe

    child_pid = Process.fork {
      read.close
      write << "#{Process.ppid}\n"
      write.close
      exit!
    }
    write.close
    pid = read.gets
    read.close
    Process.wait(child_pid)
    pid.to_i.should == Process.pid
  end
end

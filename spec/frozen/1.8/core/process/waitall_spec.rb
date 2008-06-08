require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.waitall" do
  before :all do
    Process.waitall
  end

  it "returns an empty array when there are no children" do
    Process.waitall.should == []
  end

  it "takes no arguments" do
    lambda { Process.waitall(0) }.should raise_error(ArgumentError)
  end

  not_supported_on :windows do
    it "waits for all children" do
      pids = []
      pids << Process.fork { Process.exit! 2 }
      pids << Process.fork { Process.exit! 1 }
      pids << Process.fork { Process.exit! 0 }
      Process.waitall
      pids.each { |pid|
        lambda { Process.kill(0, pid) }.should raise_error(Errno::ESRCH)
      }
    end
  
    it "returns an array of pid/status pairs" do
      pids = []
      pids << Process.fork { Process.exit! 2 }
      pids << Process.fork { Process.exit! 1 }
      pids << Process.fork { Process.exit! 0 }
      a = Process.waitall
      a.class.should == Array
      a.size.should == 3
      pids.each { |pid|
        pid_status = a.assoc(pid)
        pid_status.class.should == Array
        pid_status.size.should == 2
        pid_status.first.should == pid
        pid_status.last.class.should == Process::Status
      }
    end
  end
end

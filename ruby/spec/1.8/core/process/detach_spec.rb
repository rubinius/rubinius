require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process#detach" do
  it "requires one argument" do
    lambda { Process.detach }.should raise_error(ArgumentError)
  end

  it "returns a thread" do
    p1 = Process.fork { sleep 0.1; Process.exit! }
    Process.detach(p1).class.should == Thread
  end

  it "reaps the child process's status automatically" do
    p1 = Process.fork { sleep 0.1; Process.exit! }
    p2 = Process.fork { sleep 0.2; Process.exit! }
    Process.waitpid(p2)
    Process.detach(p1)
    lambda { Process.waitpid(p1) }.should raise_error(Errno::ECHILD)
  end
end

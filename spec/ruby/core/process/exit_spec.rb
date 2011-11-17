require File.expand_path('../../../spec_helper', __FILE__)

describe "Process.exit" do
  it "needs to be reviewed for spec completeness"
end

describe "Process.exit!" do

  platform_is_not :windows do

    it "exits with the given status" do
      pid = Process.fork { Process.exit!(1) }
      pid, status = Process.waitpid2(pid)
      status.exitstatus.should == 1
    end

    it "exits immediately when called from a thread" do
      pid = Process.fork do
        Thread.new { Process.exit!(1) }
        sleep 1
        Process.exit!(2)
      end
      pid, status = Process.waitpid2(pid)
      status.exitstatus.should == 1
    end

  end
end

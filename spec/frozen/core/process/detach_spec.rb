require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process#detach" do
  platform_is_not :windows do
    it "requires one argument" do
      lambda { Process.detach }.should raise_error(ArgumentError)
    end

    it "returns a thread" do
      p1 = Process.fork { Process.exit! }
      Process.detach(p1).class.should == Thread
    end

    platform_is_not :openbsd do
      it "reaps the child process's status automatically" do
        p1 = Process.fork { Process.exit! }
        Process.detach(p1)

        t = Time.now
        while true
          alive = Process.kill(0, p1) rescue nil
          break unless alive && (Time.now - t < 5) # fail safe
        end
        lambda { Process.waitpid(p1) }.should raise_error(Errno::ECHILD)
      end
    end
  end
end

require File.expand_path('../../../spec_helper', __FILE__)
require 'timeout'

describe "Process.detach" do
  platform_is_not :windows do
    it "returns a thread" do
      p1 = Process.fork { Process.exit! }
      Process.detach(p1).should be_kind_of(Thread)
    end

    platform_is_not :openbsd do
      it "reaps the child process's status automatically" do
        p1 = Process.fork { Process.exit! }
        th = Process.detach(p1)
        timeout(3) { th.join }
        lambda { Process.waitpid(p1) }.should raise_error(Errno::ECHILD)
      end
    end
  end
end

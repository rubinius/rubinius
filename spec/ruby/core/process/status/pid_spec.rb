require File.expand_path('../../../../spec_helper', __FILE__)

describe "Process::Status#pid" do

  platform_is_not :windows do

    with_tty do

      before :each do
        @pid = Process.fork {
          Process.exit! 0
        }
        Process.waitpid(@pid)
      end

      it "returns the pid of the process" do
        $?.pid.should == @pid
      end
    end
  end
end

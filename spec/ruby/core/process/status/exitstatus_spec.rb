require File.expand_path('../../../../spec_helper', __FILE__)

describe "Process::Status#exitstatus" do

  platform_is_not :windows do

    with_tty do

      before :each do
        @pid = Process.fork {
          Process.exit! 42
        }
        Process.waitpid(@pid)
      end

      it "returns the process exit code" do
        $?.exitstatus.should == 42
      end
    end
  end
end

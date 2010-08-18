require File.expand_path('../../../../spec_helper', __FILE__)

describe "Process::Status#termsig" do

  platform_is_not :windows do

    with_tty do

      describe "a cleanly exited child" do

        before :each do
          @pid = Process.fork {
            Process.exit! 0
          }
          Process.waitpid(@pid)
        end

        it "returns nil" do
          $?.termsig.should be_nil
        end
      end

      describe "a terminated child" do

        before :each do
          @pid = Process.fork {
            sleep 2
            Process.exit! 42
          }
          Process.kill("SIGKILL", @pid)
          Process.waitpid(@pid)
        end

        it "returns the process exit code" do
          $?.termsig.should == Signal.list["KILL"]
        end
      end
    end
  end
end

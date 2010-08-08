require File.expand_path('../../../../spec_helper', __FILE__)

describe "Process::Status#signaled?" do

  platform_is_not :windows do

    with_tty do

      describe "a cleanly exited child" do

        before :each do
          @pid = Process.fork {
            Process.exit! 0
          }
          Process.waitpid(@pid)
        end

        it "returns that the child wasn't signaled" do
          $?.signaled?.should be_false
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

        it "returns that the child was signaled" do
          $?.signaled?.should be_true
        end
      end
    end
  end
end

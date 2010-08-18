require File.expand_path('../../../../spec_helper', __FILE__)

describe "Process::Status#success?" do

  platform_is_not :windows do

    with_tty do

      describe "a successful child" do

        before :each do
          @pid = Process.fork {
            Process.exit! 0
          }
          Process.waitpid(@pid)
        end

        it "returns that the process was successful" do
          $?.success?.should be_true
        end
      end

      describe "an unsuccessful child" do

        before :each do
          @pid = Process.fork {
            Process.exit! 42
          }
          Process.waitpid(@pid)
        end

        it "returns that the process was unsuccesful" do
          $?.success?.should be_false
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

        it "returns nil" do
          $?.success?.should be_nil
        end
      end
    end
  end
end

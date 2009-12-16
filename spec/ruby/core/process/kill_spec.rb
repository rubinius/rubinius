require File.dirname(__FILE__) + '/../../spec_helper'

# if run indirectly (eg via CI), kills the runner. TODO: needs guard
describe "Process.kill" do
  it "requires at least two arguments" do
    lambda { Process.kill }.should raise_error(ArgumentError)
    lambda { Process.kill(0) }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError for unknown signals" do
    lambda { Process.kill("FOO", 0) }.should raise_error(ArgumentError)
  end

  it "doesn't accept lowercase signal names" do
    lambda { Process.kill("hup", 0) }.should raise_error(ArgumentError)
  end

  it "doesn't tolerate leading or trailing spaces in signal names" do
    lambda { Process.kill(" HUP", 0) }.should raise_error(ArgumentError)
    lambda { Process.kill("HUP ", 0) }.should raise_error(ArgumentError)
  end

  platform_is_not :windows do
    it "tests for the existence of a process without sending a signal" do
      Process.kill(0, 0).should == 1
      pid = Process.fork {
        begin
          Signal.trap("HUP") { Process.exit! 99 }
          sleep(2) # only so that failures are cleaned up
        ensure
          Process.exit!
        end
      }

      # Give the child enough time to setup the HUP trap.
      sleep(0.5)

      Process.kill(0, pid).should == 1
      Process.kill(1, pid).should == 1
      Process.waitpid(pid)
      lambda { Process.kill(0, pid) }.should raise_error(Errno::ESRCH)
    end
  end

  it "raises an EPERM if permission is denied" do
    if Process.uid != 0
      lambda { Process.kill(1, 1) }.should raise_error(Errno::EPERM)
    end
  end
end

describe "Process.kill" do
  platform_is_not :windows do
    before :all do
      @saved_trap = Signal.trap("HUP") {}
    end

    before :each do
      @foo = 0
      @read, @write = IO.pipe
      Signal.trap("HUP") {
        @foo = 42
        @write << "1"
        @write.close
      }
    end

    after :each do
      @read.gets # the signal handler has run
      @read.close
      @foo.should == 42
    end

    after :all do
      Signal.trap("HUP", @saved_trap)
    end

    with_tty do # 0 goes to the entire process group, not just current pid
      it "sends the given signal to the current process if pid is zero" do
        Process.kill("HUP", 0).should == 1
      end

      it "accepts integer signal numbers" do
        Process.kill(1, 0).should == 1
      end

      it "accepts POSIX signal names without 'SIG' prefix" do
        Process.kill("HUP", 0).should == 1
      end

      it "accepts POSIX signal names with 'SIG' prefix" do
        Process.kill("SIGHUP", 0).should == 1
      end
    end
  end
end

describe "Process.kill" do
  platform_is_not :windows do
    before :each do
      @read, @write = IO.pipe
      @pid = Process.fork {
        begin
          @read.close
          Process.setpgid(0, 0)
          Signal.trap("HUP") { Process.exit! 99 }
          @write << "1"
          @write.close
          sleep(2) # only so that failures are cleaned up
        ensure
          Process.exit! 42
        end
      }
      @write.close
      @read.gets # the child has changed process groups
      @read.close
    end

    after :each do
      Process.waitpid(@pid).should == @pid
      $?.exitstatus.should == 99
    end

    with_tty do
      it "sends the given signal to the specified process" do
        Process.kill("HUP", @pid).should == 1
      end

      it "kills process groups if signal is negative" do
        Process.kill(-1, Process.getpgid(@pid)).should == 1
      end

      it "kills process groups if signal starts with a minus sign" do
        Process.kill("-HUP", Process.getpgid(@pid)).should == 1
      end

      it "kills process groups if signal starts with a minus sign and 'SIG'" do
        Process.kill("-SIGHUP", Process.getpgid(@pid)).should == 1
      end
    end
  end
end

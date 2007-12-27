require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.kill" do
  it "requires at least two arguments" do
    lambda { Process.kill }.should raise_error(ArgumentError)
    lambda { Process.kill(0) }.should raise_error(ArgumentError)
  end

  it "raises ArgumentError for unknown signals" do
    lambda { Process.kill("FOO", 0) }.should raise_error(ArgumentError)
  end

  it "doesn't accept lowercase signal names" do
    lambda { Process.kill("hup", 0) }.should raise_error(ArgumentError)
  end

  it "doesn't tolerate leading or trailing spaces in signal names" do
    lambda { Process.kill(" HUP", 0) }.should raise_error(ArgumentError)
    lambda { Process.kill("HUP ", 0) }.should raise_error(ArgumentError)
  end

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
    Process.kill(0, pid).should == 1
    Process.kill(1, pid).should == 1
    Process.waitpid(pid)
    lambda { Process.kill(0, pid) }.should raise_error(Errno::ESRCH)
  end

  it "raises EPERM if permission is denied" do
    lambda { Process.kill(1, 1) }.should raise_error(Errno::EPERM)
  end
end

describe "Process.kill" do
  before :each do
    @foo = 0
    Signal.trap("HUP") { @foo = 42 }
  end

  after :each do
    sleep(0.1) # give the signal handler time to run
    @foo.should == 42
  end

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

describe "Process.kill" do
  before :each do
    @pid = Process.fork {
      begin
        Process.setpgid(0, 0)
        Signal.trap("HUP") { Process.exit! 99 }
        sleep(2) # only so that failures are cleaned up
      ensure
        Process.exit! 42
      end
    }
    # give the child time to change process groups before we look for it
    sleep(0.1)
  end

  after :each do
    Process.waitpid(@pid).should == @pid
    $?.exitstatus.should == 99
  end

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

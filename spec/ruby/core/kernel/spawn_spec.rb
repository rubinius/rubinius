require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  describe "Kernel.spawn" do
    before(:each) do
      @f = tmp("spawn-#{rand}-#{$$}")
    end

    after(:each) do
      File.unlink(@f) if File.exists?(@f)
    end

    it "executes the given command" do
      File.exists?(@f).should be_false
      pid = spawn("ruby -e 'print :spawn' >#{@f}")
      sleep 0.1
      File.exists?(@f).should be_true
      File.read(@f).should == "spawn"
    end

    it "executes the given command as a new process" do
      pid = spawn("ruby -v >#{@f}")
      pid.should_not == Process.pid
    end

    it "returns the process ID of the new process as a Fixnum" do
      pid = spawn("ruby -v >#{@f}")
      pid.should be_an_instance_of(Fixnum)
    end

    it "returns immediately" do
      start = Time.now
      pid = spawn('ruby -e "sleep 10"')
      (Time.now - start).should < 1
    end

    it "sets the child's environment variables according to a supplied hash" do
      pid = spawn({:spawn_of_ruby => 'yes'}, "ruby -e 'print ENV[\"spawn_of_ruby\"]' >#{@f}")
      sleep 0.1
      File.exists?(@f).should be_true
      File.read(@f).should == "yes"
    end

    it "joins the current process group by default" do
      pid =  spawn("ruby -e 'print Process.getpgid(Process.pid)' >#{@f}")
      sleep 0.1
      File.exists?(@f).should be_true
      pgid = File.read(@f).to_i
      pgid.should == Process.getpgid(Process.pid)
    end

    it "joins a new process group if :pgroup => true" do
      pid =  spawn("ruby -e 'print Process.getpgid(Process.pid)' >#{@f}", {:pgroup => true})
      sleep 0.1
      File.exists?(@f).should be_true
      pgid = File.read(@f).to_i
      pgid.should_not == Process.getpgid(Process.pid)
    end

    it "uses the current working directory as its working directory" do
      pid =  spawn("ruby -e 'print Dir.pwd' >#{@f}")
      sleep 0.1
      File.exists?(@f).should be_true
      File.read(@f).should == Dir.pwd
    end

    it "uses the given working directory if :chdir => dir is supplied" do
      dir = File.expand_path('../')
      dir.should_not == Dir.pwd
      pid =  spawn("ruby -e 'print Dir.pwd' >#{@f}", {:chdir => dir})
      sleep 0.1
      File.exists?(@f).should be_true
      File.read(@f).should == dir
    end

    it "redirects STDERR to the given file descriptior if if :err => Fixnum" do
      fd = File.open(@f,'w').fileno
      spawn("ruby -e 'warn(:glark)'", {:err => fd})  
      sleep 0.1
      File.read(@f).should =~ /glark/
    end

    it "redirects STDERR to the given file if :err => String" do
      spawn("ruby -e 'warn(:glark)'", {:err => @f})  
      sleep 0.1
      File.read(@f).should =~ /glark/
    end

  end
end

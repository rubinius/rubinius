require File.expand_path('../../../spec_helper', __FILE__)

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
      Process.wait pid
      File.exists?(@f).should be_true
      File.read(@f).should == "spawn"
    end

    it "executes the given command as a new process" do
      pid = spawn("ruby -v >#{@f}")
      Process.wait pid
      pid.should_not == Process.pid
    end

    it "returns the process ID of the new process as a Fixnum" do
      pid = spawn("ruby -v >#{@f}")
      Process.wait pid
      pid.should be_an_instance_of(Fixnum)
    end

    it "returns immediately" do
      start = Time.now
      pid = spawn('ruby -e "sleep 10"')
      (Time.now - start).should < 1
      Process.kill :KILL, pid
      Process.wait pid
    end

    it "sets the child's environment variables according to a supplied hash" do
      pid = spawn({"spawn_of_ruby" => 'yes'}, "ruby -e 'print ENV[\"spawn_of_ruby\"]' >#{@f}")
      Process.wait pid
      File.exists?(@f).should be_true
      File.read(@f).should == "yes"
    end

    platform_is_not :windows do
      it "joins the current process group by default" do
        pid = spawn("ruby -e 'print Process.getpgid(Process.pid)' >#{@f}")
        Process.wait pid
        File.exists?(@f).should be_true
        pgid = File.read(@f).to_i
        pgid.should == Process.getpgid(Process.pid)
      end

      it "joins a new process group if :pgroup => true" do
        pid = spawn("ruby -e 'print Process.getpgid(Process.pid)' >#{@f}", {:pgroup => true})
        Process.wait pid
        File.exists?(@f).should be_true
        pgid = File.read(@f).to_i
        pgid.should_not == Process.getpgid(Process.pid)
      end
    end

    it "uses the current working directory as its working directory" do
      pid = spawn("ruby -e 'print Dir.pwd' >#{@f}")
      Process.wait pid
      File.exists?(@f).should be_true
      File.read(@f).should == Dir.pwd
    end

    it "uses the given working directory if :chdir => dir is supplied" do
      dir = File.expand_path('../')
      dir.should_not == Dir.pwd
      pid = spawn("ruby -e 'print Dir.pwd' >#{@f}", {:chdir => dir})
      Process.wait pid
      File.exists?(@f).should be_true
      File.read(@f).should == dir
    end

    it "redirects STDOUT to the given file descriptior if :out => Fixnum" do
      file = File.open(@f, 'w')
      fd = file.fileno
      pid = spawn("ruby -e 'print(:glark)'", {:out => fd})
      Process.wait pid
      file.close
      File.read(@f).should =~ /glark/
    end

    it "redirects STDOUT to the given file if :out => String" do
      pid = spawn("ruby -e 'print(:glark)'", {:out => @f})
      Process.wait pid
      File.read(@f).should =~ /glark/
    end

    it "redirects STDOUT to the given file if :out => IO" do
      r, w = IO.pipe
      pid = spawn("ruby -e 'print(:glark)'", {:out => w})
      Process.wait pid
      w.close
      r.read.should =~ /glark/
    end

    it "redirects STDERR to the given file descriptior if :err => Fixnum" do
      file = File.open(@f, 'w')
      fd = file.fileno
      pid = spawn("ruby -e 'warn(:glark)'", {:err => fd})
      Process.wait pid
      File.read(@f).should =~ /glark/
      file.close
    end

    it "redirects STDERR to the given file if :err => String" do
      pid = spawn("ruby -e 'warn(:glark)'", {:err => @f})
      Process.wait pid
      File.read(@f).should =~ /glark/
    end

    it "redirects STDOUT to the given file if :err => IO" do
      r, w = IO.pipe
      pid = spawn("ruby -e 'warn(:glark)'", {:err => w})
      Process.wait pid
      w.close
      r.read.should =~ /glark/
    end

    it "redirects both STDERR and STDOUT to the given file descriptior, name or IO" do
      file = File.open(@f, 'w')
      pid = spawn("ruby -e 'print(:glark); warn(:bang)'", {[:out, :err] => file.fileno})
      Process.wait pid
      file.close
      File.read(@f).should =~ /glark/
      File.read(@f).should =~ /bang/

      pid = spawn("ruby -e 'print(:glark); warn(:bang)'", {[:out, :err] => @f})
      Process.wait pid
      File.read(@f).should =~ /glark/
      File.read(@f).should =~ /bang/

      r, w = IO.pipe
      pid = spawn("ruby -e 'print(:glark); warn(:bang)'", {[:out, :err] => w})
      Process.wait pid
      w.close
      tmp = r.read
      tmp.should =~ /glark/
      tmp.should =~ /bang/
    end

    it 'generates a process of a command based on the given set of strings, regarding the first as the command and the others as the arguments' do
      Process.wait spawn('echo', 'a b', :out => @f)
      File.read(@f).should == "a b\n"
    end
  end
end

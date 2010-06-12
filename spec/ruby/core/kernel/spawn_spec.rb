require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "Kernel.spawn" do
    before :each do
      @name = tmp("kernel_spawn.txt")
      rm_r @name
    end

    after :each do
      rm_r @name
    end

    it "executes the given command" do
      pid = spawn("#{RUBY_EXE} -e 'print :spawn' >#{@name}")
      Process.wait pid
      @name.should have_data("spawn")
    end

    it "executes the given command as a new process" do
      pid = spawn("#{RUBY_EXE} -v >#{@name}")
      Process.wait pid
      pid.should_not == Process.pid
    end

    it "returns the process ID of the new process as a Fixnum" do
      pid = spawn("#{RUBY_EXE} -v >#{@name}")
      Process.wait pid
      pid.should be_an_instance_of(Fixnum)
    end

    it "returns immediately" do
      start = Time.now
      pid = spawn("#{RUBY_EXE} -e 'sleep 10'")
      (Time.now - start).should < 5
      Process.kill :KILL, pid
      Process.wait pid
    end

    it "sets the child's environment variables according to a supplied hash" do
      pid = spawn({"spawn_of_ruby" => 'yes'}, "#{RUBY_EXE} -e 'print ENV[\"spawn_of_ruby\"]' >#{@name}")
      Process.wait pid
      @name.should have_data("yes")
    end

    platform_is_not :windows do
      it "joins the current process group by default" do
        pid = spawn("#{RUBY_EXE} -e 'print Process.getpgid(Process.pid)' >#{@name}")
        Process.wait pid
        pgid = File.read(@name).to_i
        pgid.should == Process.getpgid(Process.pid)
      end

      it "joins a new process group if :pgroup => true" do
        pid = spawn("#{RUBY_EXE} -e 'print Process.getpgid(Process.pid)' >#{@name}", {:pgroup => true})
        Process.wait pid
        pgid = File.read(@name).to_i
        pgid.should_not == Process.getpgid(Process.pid)
      end
    end

    it "uses the current working directory as its working directory" do
      pid = spawn("#{RUBY_EXE} -e 'print Dir.pwd' >#{@name}")
      Process.wait pid
      @name.should have_data(Dir.pwd)
    end

    it "uses the given working directory if :chdir => dir is supplied" do
      dir = File.expand_path('../')
      dir.should_not == Dir.pwd
      pid = spawn("#{RUBY_EXE} -e 'print Dir.pwd' >#{@name}", {:chdir => dir})
      Process.wait pid
      @name.should have_data(dir)
    end

    it "redirects STDOUT to the given file descriptior if :out => Fixnum" do
      file = File.open(@name, 'w')
      pid = spawn("#{RUBY_EXE} -e 'print(:glark)'", {:out => file.fileno})
      Process.wait pid
      file.close
      @name.should have_data("glark")
    end

    it "redirects STDOUT to the given file if :out => String" do
      pid = spawn("#{RUBY_EXE} -e 'print(:glark)'", {:out => @name})
      Process.wait pid
      @name.should have_data("glark")
    end

    it "redirects STDOUT to the given file if :out => IO" do
      r, w = IO.pipe
      pid = spawn("#{RUBY_EXE} -e 'print(:glark)'", {:out => w})
      Process.wait pid
      w.close
      r.read.should == "glark"
    end

    it "redirects STDERR to the given file descriptior if :err => Fixnum" do
      file = File.open(@name, 'w')
      pid = spawn("#{RUBY_EXE} -e 'STDERR.print :glark'", {:err => file.fileno})
      Process.wait pid
      file.close
      @name.should have_data("glark")
    end

    it "redirects STDERR to the given file if :err => String" do
      pid = spawn("#{RUBY_EXE} -e 'STDERR.print :glark'", {:err => @name})
      Process.wait pid
      @name.should have_data("glark")
    end

    it "redirects STDERR to the given file descriptor if :err => IO" do
      r, w = IO.pipe
      pid = spawn("#{RUBY_EXE} -e 'STDERR.print :glark'", {:err => w})
      Process.wait pid
      w.close
      r.read.should == "glark"
    end

    it "redirects both STDERR and STDOUT to the given file descriptior" do
      file = File.open(@name, 'w')
      pid = spawn("#{RUBY_EXE} -e 'print(:glark); STDOUT.flush; STDERR.print(:bang)'",
                  {[:out, :err] => file.fileno})
      Process.wait pid
      file.close
      @name.should have_data("glarkbang")
    end

    it "does NOT redirect both STDERR and STDOUT at the time to the given name" do
      # this behavior is not guaranteed; it may be changed after 1.9.3 or later.  [ruby-dev:41433]
      File.open(@name, "w").close  # touch @name
      pid = spawn("#{RUBY_EXE} -e 'print(:glark); STDOUT.flush; STDERR.print(:bang)'",
                  {[:out, :err] => @name})
      Process.wait pid
      @name.should have_data("")
    end

    it "redirects both STDERR and STDOUT to the given IO" do
      r, w = IO.pipe
      pid = spawn("#{RUBY_EXE} -e 'print(:glark); STDOUT.flush; STDERR.print(:bang)'", {[:out, :err] => w})
      Process.wait pid
      w.close
      r.read.should == "glarkbang"
    end

    it "generates a process of a command based on the given set of strings, regarding the first as the command and the others as the arguments" do
      Process.wait spawn('echo', 'a b', :out => @name)
      File.read(@name).should == "a b\n"
    end
  end
end

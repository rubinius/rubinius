describe :process_spawn, :shared => true do
  before :each do
    @name = tmp("kernel_spawn.txt")
    rm_r @name
  end

  after :each do
    rm_r @name
  end

  it "executes the given command" do
    pid = @object.spawn("#{RUBY_EXE} -e 'print :spawn' >#{@name}")
    Process.wait pid
    @name.should have_data("spawn")
  end

  it "executes the given command as a new process" do
    pid = @object.spawn("#{RUBY_EXE} -v >#{@name}")
    Process.wait pid
    pid.should_not == Process.pid
  end

  it "returns the process ID of the new process as a Fixnum" do
    pid = @object.spawn("#{RUBY_EXE} -v >#{@name}")
    Process.wait pid
    pid.should be_an_instance_of(Fixnum)
  end

  it "returns immediately" do
    start = Time.now
    pid = @object.spawn("#{RUBY_EXE} -e 'sleep 10'")
    (Time.now - start).should < 5
    Process.kill :KILL, pid
    Process.wait pid
  end

  it "sets the child's environment variables according to a supplied hash" do
    pid = @object.spawn({"spawn_of_ruby" => 'yes'}, "#{RUBY_EXE} -e 'print ENV[\"spawn_of_ruby\"]' >#{@name}")
    Process.wait pid
    @name.should have_data("yes")
  end

  platform_is_not :windows do
    it "joins the current process group by default" do
      pid = @object.spawn("#{RUBY_EXE} -e 'print Process.getpgid(Process.pid)' >#{@name}")
      Process.wait pid
      pgid = File.read(@name).to_i
      pgid.should == Process.getpgid(Process.pid)
    end

    it "joins a new process group if :pgroup => true" do
      pid = @object.spawn("#{RUBY_EXE} -e 'print Process.getpgid(Process.pid)' >#{@name}", {:pgroup => true})
      Process.wait pid
      pgid = File.read(@name).to_i
      pgid.should_not == Process.getpgid(Process.pid)
    end
  end

  it "uses the current working directory as its working directory" do
    pid = @object.spawn("#{RUBY_EXE} -e 'print Dir.pwd' >#{@name}")
    Process.wait pid
    @name.should have_data(Dir.pwd)
  end

  it "uses the given working directory if :chdir => dir is supplied" do
    dir = File.expand_path('../')
    dir.should_not == Dir.pwd
    pid = @object.spawn("#{RUBY_EXE} -e 'print Dir.pwd' >#{@name}", {:chdir => dir})
    Process.wait pid
    @name.should have_data(dir)
  end

  it "redirects STDOUT to the given file descriptior if :out => Fixnum" do
    file = File.open(@name, 'w')
    pid = @object.spawn("#{RUBY_EXE} -e 'print(:glark)'", {:out => file.fileno})
    Process.wait pid
    file.close
    @name.should have_data("glark")
  end

  it "redirects STDOUT to the given file if :out => String" do
    pid = @object.spawn("#{RUBY_EXE} -e 'print(:glark)'", {:out => @name})
    Process.wait pid
    @name.should have_data("glark")
  end

  it "redirects STDOUT to the given file if :out => IO" do
    r, w = IO.pipe
    pid = @object.spawn("#{RUBY_EXE} -e 'print(:glark)'", {:out => w})
    Process.wait pid
    w.close
    r.read.should == "glark"
  end

  it "redirects STDERR to the given file descriptior if :err => Fixnum" do
    file = File.open(@name, 'w')
    pid = @object.spawn("#{RUBY_EXE} -e 'STDERR.print :glark'", {:err => file.fileno})
    Process.wait pid
    file.close
    @name.should have_data("glark")
  end

  it "redirects STDERR to the given file if :err => String" do
    pid = @object.spawn("#{RUBY_EXE} -e 'STDERR.print :glark'", {:err => @name})
    Process.wait pid
    @name.should have_data("glark")
  end

  it "redirects STDERR to the given file descriptor if :err => IO" do
    r, w = IO.pipe
    pid = @object.spawn("#{RUBY_EXE} -e 'STDERR.print :glark'", {:err => w})
    Process.wait pid
    w.close
    r.read.should == "glark"
  end

  it "redirects both STDERR and STDOUT to the given file descriptior" do
    file = File.open(@name, 'w')
    pid = @object.spawn("#{RUBY_EXE} -e 'print(:glark); STDOUT.flush; STDERR.print(:bang)'",
                        {[:out, :err] => file.fileno})
    Process.wait pid
    file.close
    @name.should have_data("glarkbang")
  end

  it "does NOT redirect both STDERR and STDOUT at the time to the given name" do
    # this behavior is not guaranteed; it may be changed after 1.9.3 or later.  [ruby-dev:41433]
    File.open(@name, "w").close  # touch @name
    pid = @object.spawn("#{RUBY_EXE} -e 'print(:glark); STDOUT.flush; STDERR.print(:bang)'",
                        {[:out, :err] => @name})
    Process.wait pid
    @name.should have_data("")
  end

  it "redirects both STDERR and STDOUT to the given IO" do
    r, w = IO.pipe
    pid = @object.spawn("#{RUBY_EXE} -e 'print(:glark); STDOUT.flush; STDERR.print(:bang)'", {[:out, :err] => w})
    Process.wait pid
    w.close
    r.read.should == "glarkbang"
  end

  it "generates a process of a command based on the given set of strings, regarding the first as the command and the others as the arguments" do
    Process.wait @object.spawn('echo', 'a b', :out => @name)
    File.read(@name).should == "a b\n"
  end

  # argv processing

  describe "with a single argument" do
    it "subjects the specified command to shell expansion" do
      lambda { Process.wait @object.spawn("echo *") }.should_not output_to_fd("*\n")
    end

    it "creates an argument array with shell parsing semantics for whitespace" do
      lambda { Process.wait @object.spawn("echo a b  c 	d") }.should output_to_fd("a b c d\n")
    end

    it "calls #to_str to convert the argument to a String" do
      o = mock("to_str")
      o.should_receive(:to_str).and_return("echo foo")
      lambda { Process.wait @object.spawn(o) }.should output_to_fd("foo\n")
    end

    it "raises an ArgumentError if the command includes a null byte" do
      lambda { @object.spawn "\000" }.should raise_error(ArgumentError)
    end

    it "raises a TypeError if the argument does not respond to #to_str" do
      lambda { @object.spawn :echo }.should raise_error(TypeError)
    end
  end

  describe "with multiple arguments" do
    it "does not subject the arguments to shell expansion" do
      lambda { Process.wait @object.spawn("echo", "*") }.should output_to_fd("*\n")
    end

    it "preserves whitespace in passed arguments" do
      lambda { Process.wait @object.spawn("echo", "a b  c 	d") }.should output_to_fd("a b  c 	d\n")
    end

    it "calls #to_str to convert the arguments to Strings" do
      o = mock("to_str")
      o.should_receive(:to_str).and_return("foo")
      lambda { Process.wait @object.spawn("echo", o) }.should output_to_fd("foo\n")
    end

    it "raises an ArgumentError if an argument includes a null byte" do
      lambda { @object.spawn "echo", "\000" }.should raise_error(ArgumentError)
    end

    it "raises a TypeError if an argument does not respond to #to_str" do
      lambda { @object.spawn "echo", :foo }.should raise_error(TypeError)
    end
  end

  describe "with a command array" do
    it "uses the first element as the command name and the second as the argv[0] value" do
      lambda { Process.wait @object.spawn(["/bin/sh", "argv_zero"], "-c", "echo $0") }.should output_to_fd("argv_zero\n")
    end

    it "does not subject the arguments to shell expansion" do
      lambda { Process.wait @object.spawn(["echo", "echo"], "*") }.should output_to_fd("*\n")
    end

    it "preserves whitespace in passed arguments" do
      lambda { Process.wait @object.spawn(["echo", "echo"], "a b  c 	d") }.should output_to_fd("a b  c 	d\n")
    end

    it "calls #to_ary to convert the argument to an Array" do
      o = mock("to_ary")
      o.should_receive(:to_ary).and_return(["/bin/sh", "argv_zero"])
      lambda { Process.wait @object.spawn(o, "-c", "echo $0") }.should output_to_fd("argv_zero\n")
    end

    it "calls #to_str to convert the first element to a String" do
      o = mock("to_str")
      o.should_receive(:to_str).and_return("echo")
      lambda { Process.wait @object.spawn([o, "echo"], "foo") }.should output_to_fd("foo\n")
    end

    it "calls #to_str to convert the second element to a String" do
      o = mock("to_str")
      o.should_receive(:to_str).and_return("echo")
      lambda { Process.wait @object.spawn(["echo", o], "foo") }.should output_to_fd("foo\n")
    end

    it "raises an ArgumentError if the Array does not have exactly two elements" do
      lambda { @object.spawn([]) }.should raise_error(ArgumentError)
      lambda { @object.spawn([:a]) }.should raise_error(ArgumentError)
      lambda { @object.spawn([:a, :b, :c]) }.should raise_error(ArgumentError)
    end

    it "raises an ArgumentError if the Strings in the Array include a null byte" do
      lambda { @object.spawn ["\000", "echo"] }.should raise_error(ArgumentError)
      lambda { @object.spawn ["echo", "\000"] }.should raise_error(ArgumentError)
    end

    it "raises a TypeError if an element in the Array does not respond to #to_str" do
      lambda { @object.spawn ["echo", :echo] }.should raise_error(TypeError)
      lambda { @object.spawn [:echo, "echo"] }.should raise_error(TypeError)
    end
  end

  # error handling

  it "raises an Errno::ENOENT for an empty string" do
    lambda { @object.spawn "" }.should raise_error(Errno::ENOENT)
  end

  it "raises an Errno::ENOENT if the command does not exist" do
    lambda { @object.spawn "nonesuch" }.should raise_error(Errno::ENOENT)
  end

  it "raises an Errno::EACCES when the file does not have execute permissions" do
    lambda { @object.spawn __FILE__ }.should raise_error(Errno::EACCES)
  end

  it "raises an Errno::EACCES when passed a directory" do
    lambda { @object.spawn File.dirname(__FILE__) }.should raise_error(Errno::EACCES)
  end
end

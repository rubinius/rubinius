require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "IO.popen" do
  after :each do
    @io.close if @io and !@io.closed?
  end

  it "returns an open IO" do
    @io = IO.popen("yes", "r")
    @io.closed?.should be_false
  end

  it "reads a read-only pipe" do
    @io = IO.popen("echo foo", "r")
    @io.read.should == "foo\n"
  end

  it "raises IOError when writing a read-only pipe" do
    @io = IO.popen("echo foo", "r")
    lambda { @io.write('foo') }.should raise_error(IOError)
  end

  platform_is_not :windows do
    before :each do
      @fname = tmp("IO_popen_spec")
    end

    after :each do
      rm_r @fname
    end

    it "writes to a write-only pipe" do
      @io = IO.popen("cat > #{@fname}", "w")
      @io.write("bar")
      @io.close

      @fname.should have_data("bar")
    end

    it "raises IOError when reading a write-only pipe" do
      @io = IO.popen("cat", "w")
      lambda { @io.read }.should raise_error(IOError)
    end

    it "reads and writes a read/write pipe" do
      @io = IO.popen("cat", "r+")
      @io.write("bar")
      @io.read(3).should == "bar"
    end

    it "waits for the child to finish" do
      @io = IO.popen("cat > #{@fname} && sleep 1", "w")
      @io.write("bar")
      @io.close

      $?.exitstatus.should == 0

      @fname.should have_data("bar")
    end

    it "does not throw an exception if child exited and has been waited for" do
      @io = IO.popen("sleep 1000")
      Process.kill "KILL", @io.pid
      Process.wait @io.pid
      @io.close
      $?.exitstatus.should be_nil
    end
  end

  it "returns an instance of a subclass when called on a subclass" do
    @io = IOSpecs::SubIO.popen("true", "r")
    @io.should be_an_instance_of(IOSpecs::SubIO)
  end

  it "coerces mode argument with #to_str" do
    mode = mock("mode")
    mode.should_receive(:to_str).and_return("r")
    @io = IO.popen("true", mode)
  end

  describe "with a block" do
    it "yields an open IO to the block" do
      @io = IO.popen("yes", "r") do |io|
        io.closed?.should be_false
      end
    end

    it "yields an instance of a subclass when called on a subclass" do
      IOSpecs::SubIO.popen("true", "r") do |io|
        io.should be_an_instance_of(IOSpecs::SubIO)
      end
    end

    it "closes the IO after yielding" do
      @io = IO.popen("yes", "r") { |io| io }
      @io.closed?.should be_true
    end

    it "allows the IO to be closed inside the block" do
      @io = IO.popen('yes', 'r') { |io| io.close; io }
      @io.closed?.should be_true
    end

    it "returns the value of the block" do
      IO.popen("yes", "r") { :hello }.should == :hello
    end
  end

  it "starts returns a forked process if the command is -" do
    io = IO.popen("-")

    if io # parent
      io.gets.should == "hello from child\n"
      io.close
    else # child
      puts "hello from child"
      exit!
    end
  end

  with_feature :encoding do
    it "has the given external encoding" do
      io = IO.popen("true", :external_encoding => Encoding::EUC_JP)
      io.external_encoding.should == Encoding::EUC_JP
    end

    it "has the given internal encoding" do
      io = IO.popen("true", :internal_encoding => Encoding::EUC_JP)
      io.internal_encoding.should == Encoding::EUC_JP
    end

    it "sets the internal encoding to nil if it's the same as the external encoding" do
      io = IO.popen("true", :external_encoding => Encoding::EUC_JP,
                            :internal_encoding => Encoding::EUC_JP)
      io.internal_encoding.should be_nil
    end
  end

  platform_is_not :windows do # not sure what commands to use on Windows
    context "with a leading ENV Hash" do
      it "accepts a single String command" do
        IO.popen({"FOO" => "bar"}, "echo $FOO") do |io|
          io.read.should == "bar\n"
        end
      end

      it "accepts a single String command, and an IO mode" do
        IO.popen({"FOO" => "bar"}, "echo $FOO", "r") do |io|
          io.read.should == "bar\n"
        end
      end

      it "accepts a single String command with a trailing Hash of Process.exec options" do
        IO.popen({"FOO" => "nonexistent"}, "ls $FOO", :err => [:child, :out]) do |io|
          io.read.should =~ /No such file or directory/
        end
      end

      it "accepts a single String command with a trailing Hash of Process.exec options, and an IO mode" do
        IO.popen({"FOO" => "nonexistent"}, "ls $FOO", "r", :err => [:child, :out]) do |io|
          io.read.should =~ /No such file or directory/
        end
      end

      it "accepts an Array of command and arguments" do
        IO.popen({"FOO" => "bar"}, [["sh", "specfu"], "-c", "echo $FOO"]) do |io|
          io.read.should == "bar\n"
        end
      end

      it "accepts an Array of command and arguments, and an IO mode" do
        IO.popen({"FOO" => "bar"}, [["sh", "specfu"], "-c", "echo $FOO"], "r") do |io|
          io.read.should == "bar\n"
        end
      end

      it "accepts an Array command with a separate trailing Hash of Process.exec options" do |io|
        IO.popen({"FOO" => "bar"}, [RUBY_EXE, "-e", "STDERR.puts ENV[:FOO.to_s]"],
                 :err => [:child, :out]) do |io|
          io.read.should == "bar\n"
        end
      end

      it "accepts an Array command with a separate trailing Hash of Process.exec options, and an IO mode" do |io|
        IO.popen({"FOO" => "bar"}, [RUBY_EXE, "-e", "STDERR.puts ENV[:FOO.to_s]"],
                 "r", :err => [:child, :out]) do |io|
          io.read.should == "bar\n"
        end
      end
    end

    context "with a leading Array argument" do
      it "uses the Array as command plus args for the child process" do
        IO.popen(["echo", "hello"]) do |io|
          io.read.should == "hello\n"
        end
      end

      it "accepts a leading ENV Hash" do
        IO.popen([{"FOO" => "bar"}, "sh", "-c", "echo $FOO"]) do |io|
          io.read.should == "bar\n"
        end
      end

      it "accepts a trailing Hash of Process.exec options" do
        IO.popen(["sh", "-c", "does_not_exist", {:err => [:child, :out]}]) do |io|
          io.read.should =~ /not found/
        end
      end

      it "accepts an IO mode argument following the Array" do
        IO.popen(["sh", "-c", "does_not_exist", {:err => [:child, :out]}], "r") do |io|
          io.read.should =~ /not found/
        end
      end

      it "accepts [env, command, arg1, arg2, ..., exec options]" do |io|
        IO.popen([{"FOO" => "bar"}, RUBY_EXE, "-e", "STDERR.puts ENV[:FOO.to_s]",
                 :err => [:child, :out]]) do |io|
          io.read.should == "bar\n"
        end
      end

      it "accepts '[env, command, arg1, arg2, ..., exec options], mode'" do |io|
        IO.popen([{"FOO" => "bar"}, RUBY_EXE, "-e", "STDERR.puts ENV[:FOO.to_s]",
                 :err => [:child, :out]], "r") do |io|
          io.read.should == "bar\n"
        end
      end

      it "accepts '[env, command, arg1, arg2, ..., exec options], mode, IO options'" do |io|
        IO.popen([{"FOO" => "bar"}, RUBY_EXE, "-e", "STDERR.puts ENV[:FOO.to_s]",
                 :err => [:child, :out]], "r",
                 :internal_encoding => Encoding::EUC_JP) do |io|
          io.read.should == "bar\n"
          io.internal_encoding.should == Encoding::EUC_JP
        end
      end

      it "accepts '[env, command, arg1, arg2, ...], mode, IO + exec options'" do |io|
        IO.popen([{"FOO" => "bar"}, RUBY_EXE, "-e", "STDERR.puts ENV[:FOO.to_s]"], "r",
                 :err => [:child, :out], :internal_encoding => Encoding::EUC_JP) do |io|
          io.read.should == "bar\n"
          io.internal_encoding.should == Encoding::EUC_JP
        end
      end
    end
  end
end

describe :process_exec, :shared => true do
  it "raises Errno::ENOENT for an empty string" do
    lambda { @object.exec "" }.should raise_error(Errno::ENOENT)
  end

  it "raises Errno::ENOENT for a command which does not exist" do
    lambda { @object.exec "bogus-noent-script.sh" }.should raise_error(Errno::ENOENT)
  end

  it "raises an ArgumentError if the command includes a null byte" do
    lambda { @object.exec "\000" }.should raise_error(ArgumentError)
  end

  it "raises Errno::EACCES when the file does not have execute permissions" do
    lambda { @object.exec __FILE__ }.should raise_error(Errno::EACCES)
  end

  it "raises Errno::EACCES when passed a directory" do
    lambda { @object.exec File.dirname(__FILE__) }.should raise_error(Errno::EACCES)
  end

  it "runs the specified command, replacing current process" do
    ruby_exe('exec "echo hello"; puts "fail"', :escape => true).should == "hello\n"
  end

  describe "with a single argument" do
    before(:each) do
      @path = tmp("tmp")
      touch @path
    end

    after(:each) do
      rm_r @path
    end

    it "subjects the specified command to shell expansion" do
      ruby_exe('exec "echo *"', :escape => true, :dir => tmp("")).should == "#{File.basename(@path)}\n"
    end
  end

  describe "with multiple arguments" do
    it "does not subject the arguments to shell expansion" do
      ruby_exe('exec "echo", "*"', :escape => true).should == "*\n"
    end
  end

  ruby_version_is "1.9.2" do
    it "passes environment vars to the child environment" do
      ruby_exe('exec({"FOO" => "BAR"}, "echo $FOO")', :escape => true).should == "BAR\n"
    end
  end
end

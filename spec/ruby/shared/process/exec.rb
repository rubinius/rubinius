describe :process_exec, :shared => true do
  it "raises a SystemCallError if cmd cannot execute" do
    lambda { @object.exec "" }.should raise_error(SystemCallError)
  end

  it "raises a SystemCallError if cmd cannot execute and contains '-'" do
    lambda { @object.exec 'cmd-plugin' }.should raise_error(SystemCallError)
  end

  it "raises an ArgumentError if the cmd includes a null byte" do
    lambda { @object.exec "\000" }.should raise_error(ArgumentError)
  end

  it "raises Errno::ENOENT if the script does not exist" do
    lambda { @object.exec "bogus-noent-script.sh" }.should raise_error(Errno::ENOENT)
  end

  it "runs the specified command, replacing current process" do
    result = `#{RUBY_EXE} -e 'exec "echo hello"; puts "fail"'`
    result.should == "hello\n"
  end

  ruby_version_is "1.9.2" do
    it "passes environment vars to the child environment" do
      result = `#{RUBY_EXE} -e 'exec({"FOO" => "BAR"}, "echo $FOO"); puts "fail"'`
      result.should == "BAR\n"
    end
  end

  it "raises Errno::EACCES when the file does not have execute permissions" do
    lambda { @object.exec __FILE__ }.should raise_error(Errno::EACCES)
  end

  it "raises Errno::ACCES when passed a directory" do
    lambda { @object.exec File.dirname(__FILE__) }.should raise_error(Errno::EACCES)
  end
end

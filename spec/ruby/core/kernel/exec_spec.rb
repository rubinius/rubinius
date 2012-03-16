require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#exec" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:exec)
  end

  it "raises a SystemCallError if cmd cannot execute" do
    lambda { exec "" }.should raise_error(SystemCallError)
  end

  it "raises a SystemCallError if cmd cannot execute and contains '-'" do
    lambda { exec 'cmd-plugin' }.should raise_error(SystemCallError)
  end

  it "raises Errno::ENOENT if the script does not exist" do
    lambda { exec "bogus-noent-script.sh" }.should raise_error(Errno::ENOENT)
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
end

describe "Kernel#exec" do
  before :each do
    @script = tmp("tmp.sh")
    touch @script
  end

  after :each do
    rm_r @script
  end

  it "raises Errno::EACCES when the file does not have execute permissions" do
    lambda { exec @script }.should raise_error(Errno::EACCES)
  end

  it "raises Errno::ACCES when passed a directory" do
    lambda { exec File.dirname(@script) }.should raise_error(Errno::EACCES)
  end
end

describe "Kernel.exec" do
  it "needs to be reviewed for spec completeness"
end

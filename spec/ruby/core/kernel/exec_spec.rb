require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#exec" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:exec)
  end

  it "raises a SystemCallError if cmd cannot execute" do
    lambda { exec "" }.should raise_error(SystemCallError)
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

describe "Kernel.exec" do
  it "needs to be reviewed for spec completeness"
end

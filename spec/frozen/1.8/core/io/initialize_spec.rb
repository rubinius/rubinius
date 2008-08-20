require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#initialize" do
  before :each do
    @io = IO.allocate
  end

  it "succeeds when fd is a Fixnum" do
    lambda { @io.send :initialize, $stdout.fileno, 'w' }.should_not raise_error(TypeError)
  end

  it "succeeds when fd responds to #to_int" do
    obj = mock('fileno')
    def obj.to_int() $stdout.fileno end

    lambda { @io.send :initialize, obj, 'w' }.should_not raise_error(TypeError)
  end

  it "raises a TypeError when fd is an IO" do
    lambda { @io.send :initialize, $stdout, 'w' }.should raise_error(TypeError)
  end

  it "raises a TypeError when given a non-integer" do
    lambda { @io.send :initialize, @fname, 'w' }.should raise_error(TypeError)
  end
  
  it "raises IOError on closed stream" do
    lambda { @io.send :initialize, IOSpecs.closed_file.fileno }.should raise_error(IOError)
  end

  it "raises an Errno::EBADF when given an invalid file descriptor" do
    lambda { @io.send :initialize, -1, 'w' }.should raise_error(Errno::EBADF)
  end
end

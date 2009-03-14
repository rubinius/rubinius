require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#pid" do
  before :each do
    @file = File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt', 'r')
    @io = IO.open @file.fileno, 'r'
  end

  after :each do
    # we *must* close both in order to not leak descriptors
    @io.close unless @io.closed?
    @file.close unless @file.closed? rescue Errno::EBADF
  end

  it "returns nil for IO not associated with a process" do
    @io.pid.should == nil
  end

  it "returns the ID of a process associated with stream" do
    IO.popen(RUBY_EXE, "r+") { |io|
      io.pid.should_not == nil
    }
  end

  it "raises IOError on closed stream" do
    process_io = IO.popen(RUBY_EXE, "r+") { |io| io }
    lambda { process_io.pid }.should raise_error(IOError)
  end
end
